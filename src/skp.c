/* 
**  (C) 2019 by Remo Dentato (rdentato@gmail.com)
**
** This software is distributed under the terms of the MIT license:
**  https://opensource.org/licenses/MIT
**
*/

#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include "skp.h"
#include "dbg.h"

#ifndef STATIC
#define STATIC static
#endif

typedef struct {
  char      *str_next;
  char      *pat_next;
  char      *pat_end;
  char      *pat_err;
  skpcapt_t *capt;
  int        prv_capt;
  int        num_match;
  uint32_t   esc;
  int8_t     nocase;
  int8_t     utf;
  int8_t     inv;
  int8_t     not;
}  skpargs_t;

char *skpemptystr = "";

STATIC char *skpmatch(char *str, char *pat, skpargs_t *args_init);

STATIC char *skpencoded(char *str, uint32_t *code_ret, int utf)
{
  uint32_t chr_code=0;
  chr_code = *((unsigned char *)str);
  if (*str && utf && (*str++ & 0x80)) {
    while ((*str & 0xC0) == 0x80) {
      chr_code = (chr_code << 8) | *((unsigned char *)str++);
    }
  }
  if (code_ret) *code_ret=chr_code;
  return str;
}

STATIC char *chkquoted(skpargs_t *args)
{
  char *str = args->str_next; 
  char quote;
  uint32_t esc = args->esc;
  quote =*str;
  if (quote && (quote == '\"' || quote == '\'' || quote == '`')) {
    for (str++; *str && *str != quote; str++) {
      if (*str == esc) str++;
    }
    if (*str) str++;
  }
  if (!(quote && (quote == '\"' || quote == '\'' || quote == '`'))) 
    str = NULL;
  return str;
}

STATIC char *chkbraced(skpargs_t *args)
{
  char *s = args->str_next; 
  char b_open = *s;
  char b_close;
  uint32_t esc = args->esc;
  
  switch (b_open) {
    case '(': b_close = ')';  break;
    case '[': b_close = ']';  break;
    case '{': b_close = '}';  break;
    case '<': b_close = '>';  break;
    default : b_close = '\0'; break;
  }

  if (b_close) {
    s++;
    for (int nest = 1; nest && *s; s++) {
      if (*s == esc) s++;
      else if (*s == b_close) nest--;
      else if (*s == b_open) nest++;
    }
  }
  if (s == args->str_next) s = NULL;
  return s;
}

STATIC int isdecdigit(int c) { return '0' <= c && c <= '9'; }
STATIC int ishexdigit(int c) { return c < 0x80 && isxdigit(c); }

STATIC char *chkintnumber(skpargs_t *args)
{ 
  char *s = args->str_next; 
  if ((*s == '+' || *s == '-') && isdecdigit(s[1])) s+=2;
  while(isdecdigit(*s)) s++;
  if (s == args->str_next) s = NULL;
  return s;
}

STATIC char *chkdecnumber (skpargs_t *args)
{
  char *start = args->str_next;
  char *s = start; 
  int d=0;
  if (*s) {
    if ((*s == '+' || *s == '-')) s++;
    while(isdecdigit(*s)) {d=1;s++;}
    if (*s == '.') s++;
    while(isdecdigit(*s)) {d=1;s++;}
    if (d == 0) return NULL;
    start=s;
    if (*s == 'e' || *s == 'E' ) {
      s++; d=0;
      if ((*s == '+' || *s == '-')) s++;
      while(isdecdigit(*s)) {d=1;s++;}
      if (*s == '.') s++;
      while(isdecdigit(*s)) {d=1;s++;}
      if (d == 0) s = start;
    }
  }
  if (s == args->str_next) s = NULL;
  return s;
}

STATIC char *chkhexnumber (skpargs_t *args)
{
  char *s = args->str_next; 
  if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X') && ishexdigit(s[2])) s+=3;
  while(ishexdigit(*s)) s++;
  if (s == args->str_next) s = NULL;
  return s;
}

#define CHK_F(f,t)  STATIC char *f (skpargs_t *args) {\
                      char *s = args->str_next; \
                      uint32_t c; \
                      if (*s) { \
                        char *s_tmp=skpencoded(s,&c,args->utf); \
                        if ((t) ^ args->inv) return s_tmp; \
                      } \
                      if (s == args->str_next) s = NULL; \
                      return s; \
                    }

STATIC int islowerlatin(uint32_t c)
{
  if ('a' <= c && c <= 'z') return 1;                      // ASCII
  if (0xDF <= c && c <= 0xFF && c != 0xF7) return 1;       // ISO-8859-1
  if (0xC39F <= c && c <= 0xC3BF && c != 0xC3B7) return 1; // UTF-8
  return 0;
}

STATIC int isupperlatin(uint32_t c)
{
  if ('A' <= c && c <= 'Z') return 1;                      // ASCII
  if (0xC0 <= c && c <= 0xDE && c != 0xD7) return 1;       // ISO-8859-1
  if (0xC380 <= c && c <= 0xC39E && c != 0xC397) return 1; // UTF-8
  return 0;
}

STATIC int isctrllatin(uint32_t c)
{
  if (0x01 <= c && c <= 0x1F ) return 1;     // ASCII
  if (0x80 <= c && c <= 0x9F ) return 1;     // ISO-8859-1
  if (0xC280 <= c && c <= 0xC29F ) return 1; // UTF-8
  return 0;
}

STATIC int isblanklatin(uint32_t c)
{ return (' ' == c || '\t' == c || 0xA0 == c || 0xC2A0 == c); }

STATIC int ispacelatin(uint32_t c)
{ return (isblanklatin(c) || '\v' == c || '\f' == c || '\r' == c || '\n' == c); }

CHK_F(chkspace  ,(ispacelatin(c)))
CHK_F(chkalpha  ,(islowerlatin(c) || isupperlatin(c)))
CHK_F(chkxdigit ,(ishexdigit(c)))
CHK_F(chkdigit  ,(isdecdigit(c)))
CHK_F(chklower  ,(islowerlatin(c)))
CHK_F(chkupper  ,(isupperlatin(c)))
CHK_F(chkblank  ,(isblanklatin(c)))
CHK_F(chkpercent,('%' == c))
CHK_F(chknoteol ,(c != '\r' && c != '\n'))
CHK_F(chkctrl   ,(isctrllatin(c)))
CHK_F(chkany    ,(c!='\0'))

STATIC char *chkeol(skpargs_t *args)
{
  char *s = args->str_next;
  args->pat_end = args->pat_next+1;
  if (*s == '\r') s++;
  if (*s == '\n') s++;
  return s;
}

STATIC char *chkline(skpargs_t *args)
{
  char *s = args->str_next;
  args->pat_end = args->pat_next+1;
  while (*s && *s != '\r' && *s != '\n') s++;
  return s;
}

STATIC char *chkequal(skpargs_t *args)
{
  uint32_t b,c;

  char *s = args->str_next;
  char *p = args->pat_next;

  p = skpencoded(p,&b,args->utf);
  s = skpencoded(s,&c,args->utf);
  args->pat_end = p;
  _dbgmsg("CHKEQ: %08X %08X",b,c);
  if ((b == c || (args->nocase && (b < 0x80) && (c < 0x80) &&
                       (tolower(b) == tolower(c)))) ^ args->inv) {
     return s;
  }
  return NULL;
}

STATIC char *chknone(skpargs_t *args)  {return NULL;}

STATIC char *chksubmatch(skpargs_t *args)
{
  char *start = args->str_next;
  char *str = start;
  char *pat = args->pat_next+1;
  int capt_cur;

  args->pat_end = pat;
  // reach the end of the pattern;
  for (int nest = 1; nest > 0 && *args->pat_end; ) {
     if (args->pat_end[0] == '%') {
       if (args->pat_end[1] == '(') nest++;
       else if (args->pat_end[1] == ')') nest--;
     }
     if (nest > 0) args->pat_end++;
  }

  _dbgmsg("SUBM[ str:%s pat:'%.*s'",start,(int)(args->pat_end-pat),pat) ;

  capt_cur = args->capt->cur;             // Save the current capture number
  if (args->num_match == 0 && args->capt->num < args->capt->max) {
    args->capt->cur = args->capt->num++;    // Set the new current capture number
    args->prv_capt = args->capt->cur;
    args->capt->str[args->capt->cur].start = start;
    _dbgmsg("beg: %d '%s'",args->capt->cur,str);
  } else {
    args->capt->cur = args->prv_capt;
  }
  str = skpmatch(start, pat, args);
  if (str)  {
    _dbgmsg("end: %d '%s'",args->capt->cur,str);
    args->capt->str[args->capt->cur].end = str;
  }
  args->capt->cur = capt_cur;             // Restore the current capture number
  
  if (*args->pat_end) args->pat_end++;
  if (*args->pat_end) args->pat_end++;
  _dbgmsg("SUBM] str_next=[%s] pat_next=[%s]",str?str:"(null)",args->pat_end);
  return str;
}

STATIC char *chkrange  (skpargs_t *args)
{
  char *start = args->str_next;
  char *str = start;
  char *pat = args->pat_next+1;
  uint32_t c, c1, c2;
  uint32_t inv=0;

  args->pat_end = pat;
  // reach the end of the pattern;
  while (*args->pat_end && !(args->pat_end[0] == '%' && args->pat_end[1] == ']')) {
    args->pat_end++;
  }

  str=skpencoded(str, &c, args->utf);
  _dbgmsg("RNG C:%08X pat:'%.*s'",c,(int)(args->pat_end-pat),pat);
  
  if (*pat == '^') { inv=1;  pat++; }
  c1 = '-';
  if (!(*pat == '-' && c == '-')) {
    c1 = 0;
    while (pat < args->pat_end) {
      pat=skpencoded(pat, &c1, args->utf);
      _dbgmsg("RNG C1:%08X pat:'%s'",c1,pat);
      if ( c == c1 ) break; // FOUND
      if (*pat == '-' && pat+1 != args->pat_end) {
        pat=skpencoded(pat+1, &c2, args->utf);
        _dbgmsg("RNG C2:%08X pat:'%s'",c2,pat);
        if (c1<c && c<=c2) break; // FOUND
      }
      c1 = 0;
    }
  }

  if ((!!c1) == inv) { // FAILED
    str=NULL;
  }

  _dbgmsg("eq? %d %s",c1,str);

  if (*args->pat_end) args->pat_end++;
  if (*args->pat_end) args->pat_end++;

  _dbgmsg("RANGE str_next=[%s] pat_next=[%s]",str,args->pat_end);
  if (str == start) str = NULL;
  return str;
}

STATIC char *chkcapt(skpargs_t *args)
{
  char *start=args->str_next;
  int n = *args->pat_next-'0';

  if (n<args->capt->max && args->capt->str[n].end) {
    int l = (int)(args->capt->str[n].end - args->capt->str[n].start);
    if (strncmp(start,args->capt->str[n].start,l) == 0) return start+l;
  }
  return NULL;
}

STATIC int chk(char *(*f)(skpargs_t *),skpargs_t *args)
{ 
  char *start=args->str_next;
  char *s=start;

  _dbgmsg("CHK s:[%s] p:[%s] ",args->str_next,args->pat_next);

  args->pat_end = args->pat_next+1; 

  s=f(args);
  args->str_next=start;
  
  if (s==NULL) return -1; // FAILED

  return s-start;
}

#define CHR  0xFFFFFFFF
#define MAX_MATCH (1<<30)

STATIC char *skpalt(char *pat)
{ 
  int nest = 1;
  while (*pat) {
    _dbgmsg("--> %s",pat);
    if (*pat == '%') {
      if (pat[1] == '(') nest++;
      else if (pat[1] == ')') nest--;
      if (nest == 0) break; 
    }
    pat++;
  }
  return pat;
}

STATIC char *skpto_alt(char *pat)
{ 
  int nest = 1;
  if (!pat) pat=skpemptystr;
 _dbgmsg("search alt: '%s'",pat);
  while (*pat) {
    if (*pat == '%') {
           if (pat[1] == '|') { pat += 2; break; } 
      else if (pat[1] == '(') nest++;
      else if (pat[1] == ')') nest--;
      if (nest == 0) { pat = skpemptystr; break; }
    }
    pat++;
  }
 _dbgmsg(" alt found:  '%s'",pat);
  return pat;
}

#define DO_NEXT_PATTERN args.pat_next++; p_chr='\0'; break

STATIC char *skpmatch(char *str, char *pat, skpargs_t *args_init)
{
  char *start = str;
  uint32_t p_chr;
  skpargs_t args;

  int min_match = 1;
  int max_match = 1;
  int num_match = 0;
  int len_match = 0;

  char *(*f_skp)(skpargs_t *) = chknone;

  if (!pat || !str) return NULL;
  
  if (!args_init) return NULL;
  args = *args_init;

  while (*pat) {
    _dbgmsg("MATCHING: s:[%s] p:[%s] case:%d min:%d max:%d",str,pat,args.nocase,min_match,max_match);
    args.str_next = str;
    args.pat_next = pat;
    num_match = 0;
    p_chr = *args.pat_next;
    f_skp = chknone;

    if (p_chr != '%') {
      p_chr = CHR;
    }
    else {
      p_chr = *++args.pat_next;
      switch(p_chr) {
        case '*' : //if (args.pat_next[1] == '%' && args.pat_next[2] == '(') goto invalid;
                   min_match=0; max_match=MAX_MATCH; DO_NEXT_PATTERN;
        case '+' : //if (args.pat_next[1] == '%' && args.pat_next[2] == '(') goto invalid;
                   min_match=1; max_match=MAX_MATCH; DO_NEXT_PATTERN;
        case '?' : min_match=0; max_match=1        ; DO_NEXT_PATTERN;
        case '^' : args.inv = 1;                     DO_NEXT_PATTERN;
        case '!' : args.not = 1;                     DO_NEXT_PATTERN;

        case ')' : pat=skpemptystr;
                   _dbgmsg("ENDING CAPT: str:'%s'",args.str_next);
                   goto endmatch;

      }
    }

    _dbgmsg("PCHR \\x%02X",p_chr);

    switch (p_chr) {
      case '_' : min_match = 0; 
      case 'w' : max_match = MAX_MATCH; 
                 f_skp = chkblank;     break;

      case '%' : f_skp = chkpercent;   break;
      case 's' : f_skp = chkspace;     break;
      case 'l' : f_skp = chklower;     break;
      case 'u' : f_skp = chkupper;     break;
      case 'a' : f_skp = chkalpha;     break;
      case '#' : f_skp = chkctrl;      break;
      case 'd' : f_skp = chkdigit;     break;
      case 'x' : f_skp = chkxdigit;    break;
      
      case '=' : f_skp = chkany;       break;
      case '.' : f_skp = chknoteol;    break;
      case CHR : f_skp = chkequal;     break;

      case '1' : case '2' : case '3' :
      case '4' : case '5' : case '6' :
      case '7' : case '8' : case '9' :
                 f_skp = chkcapt;      break;

      case 'X' : f_skp = chkhexnumber; break;
      case 'F' : f_skp = chkdecnumber; break;
      case 'D' : f_skp = chkintnumber; break;
      case 'Q' : f_skp = chkquoted;    break;
      case 'B' : f_skp = chkbraced;    break;
    
      case '(' : f_skp = chksubmatch;  break;
      case '[' : f_skp = chkrange;     break;

      case 'R' : f_skp = chkeol;       break;

      case 'L' : f_skp = chkline;      break;

      case 'C' : 
      case 'c' : args.nocase = (p_chr =='C') ; args.pat_next++;
                 break;

      case 'U' : 
      case 'I' : args.utf = (p_chr = 'U'); args.pat_next++;
                 break;
                 
      case 'e' : if (args.pat_next[1] == '\0' || args.pat_next[1] > 0x7F)  goto invalid;  // Invalid pattern 
                 args.esc = args.pat_next[1];
                 if (args.esc <= '\n') args.esc = '\0';
                 args.pat_next+=2;
                 break;

      case '\0': break;

      case '|' : args.pat_next = skpalt(args.pat_next+1);
                 break;

      default  : goto invalid;
    }

    if (f_skp != chknone) {
      while (num_match < max_match) {
        args.num_match = num_match;
        if ((len_match=chk(f_skp,&args)) < 0) break;
        args.str_next += len_match;
        _dbgmsg("CHK2: n:%d min:%d len: %d not:%d str:'%s' next:'%s' end:'%s'", num_match,min_match,len_match, args.not,args.str_next,args.pat_next,args.pat_end);
        num_match++;
      }
      args.pat_next = args.pat_end;
      if ((num_match<min_match) == args.not) { // MATCH!
        if (args.not) args.str_next = str;
      }
      else { // CHECK ALTERNATIVE!
        args.pat_next = skpto_alt(args.pat_next);
        if (*args.pat_next == '\0') goto endmatch; // FAIL 
        args.str_next = start;
      }
    }

    if (p_chr) {
      min_match = 1; max_match = 1;
      args.inv = 0; args.not = 0;
    }

    str = args.str_next;
    pat = args.pat_next;
  }

endmatch:            // Matching completed
  args.capt->str[args.capt->cur].end = args.str_next;
  if (*pat != '\0') { // If there is any pattern left
    str = NULL;       // then it's a NO MATCH!
  }

  return str;

invalid:
  args.capt->str[1].start = pat;
  while (*pat) pat++;
  args.capt->str[1].end = pat;
  return NULL;
}

char *skppattern(char *s, char *pat, skpcapt_t *capt)
{
  int anywhere = 0;
  char *next = NULL;
  char *start = s;

  skpargs_t args;
  skpcapt_t *capt_local = NULL;

  if (capt == NULL) {
    capt_local = skpcaptnew(0);
    capt = capt_local;
  }

  args.esc     = '\\';
  args.nocase  = 0;
  args.inv     = 0;
  args.utf     = 1;
  args.not     = 0;
  args.pat_err = NULL;
  args.capt    = capt;

  _dbgmsg("MATCHING: [%s] [%s] (start)",start,pat);
  
  if (!s || !pat) return NULL;

  if ((pat[0] == '%') && (pat[1] == '>')) {
    pat+=2;  anywhere = 1;  _dbgmsg("MATCHING ANYWHERE");
  }

  do {
    for (int k=0; k<capt->max; k++) {
      capt->str[k].start = (capt->str[k].end = NULL);
    }

    args.capt->num = 1;
    args.capt->cur = 0;
    args.capt->str[0].start = start;

    next = skpmatch(start, pat, &args);
  } while (!next && anywhere && *(start = skpencoded(start,NULL,args.utf)));

  for (int k=0; k<capt->max; k++) {
    if (capt->str[k].end == NULL) capt->str[k].start = NULL;
  }


  _dbgblk {
    for (int k=0; k<capt->max; k++) {
      if (capt->str[k].end)
         dbgmsg("CAPT: %2d \"%.*s\"",k,(int)(capt->str[k].end-capt->str[k].start),capt->str[k].start);
      else
         dbgmsg("CAPT: %2d (null)",k);
    }
  }

  if (capt_local) skpcaptfree(capt_local);

  if (next == NULL) { 
    args.capt->str[0].start = NULL;
    args.capt->str[0].end = NULL;
  }

  return next;
}
