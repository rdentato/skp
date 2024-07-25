#line 16 "00_header.md"

#ifndef SKP_VERSION
#define SKP_VERSION 0x0004001C
#define SKP_VERSION_STR "0.4.0"

#line 80 "00_header.md"
#include <stdio.h>
#include <stddef.h>
#include <ctype.h>
#include <inttypes.h>
#include <limits.h>
#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <setjmp.h>

#line 22 "00_header.md"
#line 44 "00_header.md"
// Handle variable and default function arguments.
// Borrowed from vrg.h but renamed to minimize namespace pollution.
#define skp_v_cnt(skp_v1,skp_v2,skp_v3,skp_v4,skp_v5,skp_v6,skp_v7,skp_v8,skp_vN, ...) skp_vN
#define skp_v_argn(...)  skp_v_cnt(__VA_ARGS__, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define skp_v_cat0(x,y)  x ## y
#define skp_v_cat(x,y)   skp_v_cat0(x,y)
#define skp_vrg(skp_v_f,...) skp_v_cat(skp_v_f, skp_v_argn(__VA_ARGS__))(__VA_ARGS__)

#define skp_VRG(skp_v_f,...) skp_v_cat(skp_v_f, skp_v_argn(__VA_ARGS__))(__VA_ARGS__)
#define skp_Vrg(skp_v_f,...) skp_v_cat(skp_v_f, skp_v_argn(__VA_ARGS__))(__VA_ARGS__)

#line 60 "00_header.md"
// Borrowed from dbg.h but renamed to minimize namespace pollution.
#define _skptrace(...)
#define  skptrace(...) (fprintf(stderr,"%5d ",__LINE__),fprintf(stderr,"TRCE| " __VA_ARGS__), fprintf(stderr, "\x0F : %s\n",__FILE__))

#define _skptest(e_,...) 
#define  skptest(e_,...) \
  do { \
    int tst_=!(e_); \
    fprintf(stderr,"%s: (%s) \xF%s:%d\n","PASS\0FAIL"+tst_*5, #e_ ,__FILE__,__LINE__); \
    if (tst_) { fprintf(stderr,"    | " __VA_ARGS__); fputc('\n',stderr); } \
    errno = tst_; \
  } while(0)

#line 23 "00_header.md"
#line 257 "10_patterns.md"
#define SKP_MAXPATTERNS 5

#line 98 "00_header.md"
// Just a caution to avoid aggressive optimization.
extern volatile int skp_zero;

#line 67 "10_patterns.md"
extern char *skp_writeonly_charptr;

#line 24 "00_header.md"
#line 73 "10_patterns.md"
typedef int (*skp_funcpattern_t)(char *, char**, char **);

#define skprecognizer(recog) \
   int recog(char *skpstart, char **skpfrom, char **skpto)

#line 117 "20_scanner.md"
typedef struct {
  char  *start;
  char  *from[SKP_MAXPATTERNS];
  char  *to[SKP_MAXPATTERNS];
  int    alt;
} skp_info_t;


#define skplen(n) skp_len(n,skpfrom, skpto)
static inline int skp_len(int n, char **from, char **to) { return to[n] - from[n];}

#line 79 "10_patterns.md"

int skp_f(char *txt, skp_funcpattern_t f, char **from,char **end);
int skp_s(char *txt, char *pat, char **from,char **end);
int skp_n(char *txt, int n, char **from,char **end);
int skp_p(char *txt, void *p, char **from,char **end);

#define skp(...)    skp_vrg(skp_,__VA_ARGS__)
#define skp_4(s,p,f,t)  skp_(s, p, f,    t)
#define skp_3(s,p,t)    skp_(s, p, &skp_writeonly_charptr, t)
#define skp_2(s,p)      skp_(s, p, &skp_writeonly_charptr, &skp_writeonly_charptr)

#define skp_(s,p,f,t) _Generic((p), \
                               char *: skp_s, \
                    skp_funcpattern_t: skp_f, \
                                  int: skp_n, \
                               void *: skp_p  \
                                              )(s,p,f,t)
#line 260 "10_patterns.md"
#define skp_type(x) _Generic((x),char *: 'S', skp_funcpattern_t: 'F', void *: 'P', default: '\0')

typedef void *skp_voidptr;

int skp_multi(char *text,char **from, char **to, char *types, void **ptrns);

#define skp_5(txt,p1,p2,f,t) \
    skp_multi(txt,f,t, (char []){skp_type(p1),skp_type(p2),'\0'},\
                (skp_voidptr []){(void *)p1, (void *)p2})

#define skp_6(txt,p1,p2,p3,f,t) \
    skp_multi(txt,f,t, (char []){skp_type(p1),skp_type(p2),skp_type(p3),'\0'},\
                (skp_voidptr []){(void *)p1, (void *)p2, (void *)p3})

#define skp_7(txt,p1,p2,p3,p4,f,t) \
    skp_multi(txt,f,t, (char []){skp_type(p1),skp_type(p2),skp_type(p3),skp_type(p4),'\0'},\
                (skp_voidptr []){(void *)p1, (void *)p2, (void *)p3, (void *)p4})

#define skp_8(txt,p1,p2,p3,p4,p5,f,t) \
    skp_multi(txt,f,t, (char []){skp_type(p1),skp_type(p2),skp_type(p3),skp_type(p4),skp_type(p5),'\0'},\
                (skp_voidptr []){(void *)p1, (void *)p2, (void *)p3, (void *)p4, (void *)p5})


#line 999 "10_patterns.md"

uint32_t skp_next(char *text,char **end,int iso);

#define skpnext(...)    skp_vrg(skpnext_,__VA_ARGS__)
#define skpnextISO(...) skp_vrg(skpnextISO_,__VA_ARGS__)

#define skpnext_1(s)   skp_next(s,NULL,0)
#define skpnext_2(s,e) skp_next(s,e,0)

#define skpnextISO_1(s)   skp_next(s,NULL,1)
#define skpnextISO_2(s,e) skp_next(s,e,1)

#line 87 "20_scanner.md"

static inline int skp_sel(int m, int n) { return n; }

#define skpscanner(...) skp_vrg(skpscanner_,__VA_ARGS__)

#define skpscanner_1(s) skpscanner_2(s,skp_next) 

#define skpscanner_2(scanner,skp_next) \
   int scanner(char *skptext, char **skp_from, char **skp_next)

#define skp_scan_loop(s,t,r) \
     for (skp_info_t skp_info = {.start = s,.alt =0}; \
          (skp_info.start && *skp_info.start && !skp_info.alt ); \
          skp_info.alt = skp_info.alt? (((t)[0] = skp_info.start = skp_info.to[0]), r):1)

#define skpscan(...)    skp_vrg(skpscan_,__VA_ARGS__)
#define skpscan_1(s)    skpscan_2(s, &skp_writeonly_charptr)
#define skpscan_2(s,t)  skp_scan_loop(s,t,0)

#define skpswitch(...)   skp_vrg(skpswitch_,__VA_ARGS__)
#define skpswitch_1(s)   skipswitch_2(s, &skp_writeonly_charptr)
#define skpswitch_2(s,t) skp_scan_loop(s,t,1)

#line 132 "20_scanner.md"

#define skpcase(...) \
  for(char **skpfrom = skp_info.from, **skpto = skp_info.to, *skpstart = skp_info.start; \
      skpstart != NULL; skpstart = NULL) \
    if (skp_info.alt || !((skp_info.alt = skp(skp_info.start, __VA_ARGS__ ,skpfrom,skpto)))) ;\
    else

#line 25 "00_header.md"

  #ifdef SKP_MAIN
#line 102 "00_header.md"
volatile int skp_zero = 0;

#line 70 "10_patterns.md"
char *skp_writeonly_charptr; // Write only!

#line 28 "00_header.md"
#line 22 "10_patterns.md"
#line 458 "10_patterns.md"
#define MATCHED_FAIL    0 
#define MATCHED         1
#define MATCHED_GOAL    2
#define MATCHED_GOALNOT 3
#line 23 "10_patterns.md"
#line 731 "10_patterns.md"
// ************************************************************ 
// SKIPPING ***************************************************
// ************************************************************ 

/* [Patterns]
   a  ASCII alphabetic char
   l  ASCII lower case
   u  ASCII upper case
   d  decimal digit
   x  hex digit
   w  white space (includes some Unicode spaces)
   s  white space and vertical spaces (e.g. LF)
   c  control
   n  newline
   @  alfanumeric 
   
   .  any character (UTF-8 or ISO character)

   Q  Quoted string with '\' as escape
   B  Balanced sequence of parenthesis (can be '()''[]''{}')
   () Balanced parenthesis (only '()')
   I  Identifier ([_A-Za-z][_0-9A-Za-z]*)
   N  Past end of line
   D  integer decimal number (possibly signed)
   F  floating point number (possibly with sign and exponent)
   X  hex number (possibly with leading 0x)

   C  case sensitive (ASCII) comparison
   U  utf-8 encoding (or ASCII/ISO-8859)

   *  zero or more match
   ?  zero or one match
   +  one or more match
   !  negate

   &  set goal
   !& set negative goal

   [...] set
  
   .  (any non \0 character)
   !. (end of text)

   >  skip to the start of pattern

   & the character '&'
*/

static int chr_cmp(uint32_t a, uint32_t b, int fold)
{ _skptrace("CMP: %d %c %c",fold, a,b);
  if (fold && a <= 0x7F && b <= 0x7F) {
    a = tolower(a);
    b = tolower(b);
  }
  return (a == b);
}

static int is_blank(uint32_t c)
{
 _skptrace("Is space: %08X",c);
#if 1
  if (c <= 0xFF) return (c == 0x20)
                     || (c == 0x09)
                     || (c == 0xA0)
                     ;

  switch (c & 0xFFFFFF00) {
    case 0x0000C200 : return c == 0xC2A0;
    case 0x00E19A00 : return c == 0xE19A80;
    case 0x00E28000 : return ((0xE28080 <= c) && (c <= 0xE2808A))
                          || (c == 0xE280AF);
            default : return c == 0xE38080;
  }
  return 0;
#else
  return (c == 0x20) || (c == 0x09)
      || (c == 0xA0) || (c == 0xC2A0)
      || (c == 0xE19A80)
      || ((0xE28080 <= c) && (c <= 0xE2808A))
      || (c == 0xE280AF)
      || (c == 0xE2819F)
      || (c == 0xE38080)
      ;
#endif
}

static int is_break(uint32_t c)
{
  _skptrace("break: %08X",c);
#if 1
  if (c < 0x0F) return  (c == 0x0A)
                     || (c == 0x0C)
                     || (c == 0x0D);

  if (c < 0xFF) return (c == 0x85);

  if (c < 0xE280A8) return (c == 0x0D0A)    // CRLF (not a real UTF-8 CODEPOINT!!!)
                        || (c == 0xC285)    // U+0085 NEL next line         
                        ;

  return (c == 0xE280A8)  // U+2028 LS line separator     
      || (c == 0xE280A9)  // U+2029 PS paragraph separator
      ;

#else
  return (c == 0x0A)      // U+000A LF line feed           
      || (c == 0x0C)      // U+000C FF form feed          
      || (c == 0x0D)      // U+000D CR carriage return     
      || (c == 0x85)      // U+0085 NEL next line   ISO-8859-15      
      || (c == 0x0D0A)    // CRLF (not a real UTF-8 CODEPOINT!!!)
      || (c == 0xC285)    // U+0085 NEL next line         
      || (c == 0xE280A8)  // U+2028 LS line separator     
      || (c == 0xE280A9)  // U+2029 PS paragraph separator
      ;
#endif
}

static int is_space(uint32_t c)
{ return is_blank(c) || is_break(c); }

static int is_digit(uint32_t c)
{ return ('0' <= c && c <= '9'); }

static int is_xdigit(uint32_t c)
{
  return ('0' <= c && c <= '9')
      || ('A' <= c && c <= 'F')
      || ('a' <= c && c <= 'f');
}

static int is_upper(uint32_t c)
{ return ('A' <= c && c <= 'Z'); }

static int is_lower(uint32_t c)
{ return ('a' <= c && c <= 'z'); }

static int is_alpha(uint32_t c)
{ return ('A' <= c && c <= 'Z')
      || ('a' <= c && c <= 'z'); }

static int is_idchr(uint32_t c)
{ return ('A' <= c && c <= 'Z')
      || ('a' <= c && c <= 'z')
      || ('0' <= c && c <= '9')
      || (c == '_'); }

static int is_alnum(uint32_t c)
{ return (is_alpha(c) || is_digit(c)); }

static int is_ctrl(uint32_t c)
{ return (c < 0x20)
      || (0xC280 <= c && c < 0xC2A0)
      || (0x7F <= c && c < 0xA0);
}

static int is_oneof(uint32_t ch, char *set, int iso)
{
  uint32_t p_ch,q_ch;
  char *s;
  if (ch == '\0') return 0;

 _skptrace("set: <%s> chr: %c",set,ch);
  p_ch = skp_next(set,&s,iso);
  
  if (p_ch == ']') {
    if (ch == ']') return 1 ;
    else p_ch = skp_next(s,&s,iso);
  }

  while (p_ch != ']') {
    if (p_ch == ch) return 1;
    q_ch = p_ch;
    p_ch = skp_next(s,&s,iso);
    if ((p_ch == '-') && (*s != ']')) {
      p_ch = skp_next(s,&s,iso);
      if ((q_ch < ch) && (ch <= p_ch)) return 1;
      p_ch = skp_next(s,&s,iso);
    }
  }
  p_ch = skp_next(s,&s,iso);
  return 0;
}

static int is_string(char *s, char *p, int len, int flg)
{
  char *start = s;
  uint32_t p_chr,s_chr;
  char *p_end, *s_end;
  int mlen = 0;
 _skptrace("STR: %d '%s' '%.*s'",len,s,len,p);
  while (len) {
   _skptrace("ALT: %d '%s' '%s'",len, p,s);
    if (*p == '\xE') return mlen;

    p_chr = skp_next(p,&p_end,flg & 2);
    s_chr = skp_next(s,&s_end,flg & 2);

    if (chr_cmp(s_chr,p_chr,flg & 1)) {
      mlen += (int)(s_end - s);
      len  -= (int)(p_end - p);
      //if (*s_end == '\0') return mlen;
      p = p_end;  s = s_end;
    }
    else {
      while (len>0 && *p++ != '\xE') len--; // search for an alternative
      if (len-- <= 0) return 0;
      s = start;
      mlen = 0;
     _skptrace("ALT2: %d p:'%.*s' s:'%s'",len,len,p,s);
    }
  }
 _skptrace("MRET: %d",mlen);
  return mlen;
}

static uint32_t get_close(uint32_t open)
{
   switch(open) {
     case '(': return ')';
     case '[': return ']';
     case '{': return '}';
     case '<': return '>';
   }
   return 0;
}

static uint32_t get_qclose(uint32_t open)
{
   switch(open) {
     case '\'':
     case '"' :
     case '`' : return open;
   }
   return 0;
}

#line 24 "10_patterns.md"
#line 447 "10_patterns.md"
// skp() needs to know about mathc()
static int match(char *pat, char *txt, char **pat_end, char **txt_end,int *flg);
#line 101 "10_patterns.md"
int skp_f(char *txt, skp_funcpattern_t f, char **from,char **end)
{
  char *local_from = txt;
  char *local_end = txt;
  int ret = 0;
 _skptrace("skp_t: %s",txt);
  if (txt != NULL && f != NULL) {
    ret = f(txt,&local_from,&local_end);
    if (ret) {
      *from = local_from;
      *end = local_end;
    }
  }
  return ret & '\7'; // ensure return value is <= 7
}

int skp_n(char *txt, int n, char **from,char **end)
{
  char *local_end = txt;
  int iso=0;
  
  if (n<0) { n = -n; iso = 1; }

  while (n && *local_end) {
    n--;
    skp_next(local_end,&local_end,iso);
   _skptrace("N: %d %s %p %p",n,local_end,local_end,end);
  }

  if (n>0) return 0;

  *from = txt;
  *end = local_end;

  return 1;
}

int skp_p(char *txt, void *p, char **from,char **end)
{
  *from = txt;
  *end = txt;
  return 1;
}
#line 148 "10_patterns.md"
int skp_s(char *txt, char *pat, char **from,char **end)
{
  char *start = txt;
  char *s; char *p;
  char *s_end=NULL; 
  char *p_end=NULL;
  int   skp_to = 0;
  int   matched = 0;
  int   ret = 0;
  int   flg = 0; // By default: Case sensitive comparison and UTF-8 encoding

#line 412 "10_patterns.md"
  char *goal = NULL;
  char *goalnot = NULL;

#line 160 "10_patterns.md"

  if (!pat || !txt) { return 0; }

#line 431 "10_patterns.md"
_skptrace("SKP_: txt:'%s' pat:'%s'",txt,pat);

  if (*pat == '>') {
    skp_to = 1;
    pat++ ;
  }
_skptrace("SKP_: txt:'%s' pat:'%s' skp_to:%d",txt,pat,skp_to);

#line 164 "10_patterns.md"

  p = pat;
  s = start;

  // skip over the spaces at the beginning of the pattern (they are useless)
  while (is_space(*p)) p++;

  // Characters below ASCII 0x07, mark the end of a sub pattern (`0x00` marks
  // the end of the entire pattern string.).
  // If the current character in the pattern is greater than `0x07`, it means
  // that we are still in the pattern.

  while (*p > '\7') { // Loop over the pattern string
    
    // Match the next recognizer
    matched = match(p,s,&p_end,&s_end,&flg); 

    if (matched) {
#line 381 "10_patterns.md"
_skptrace("matched( '%s' '%s'",s,p);
 s = s_end;
 p = p_end;
_skptrace("matched) '%s' '%s'",s,p);
#line 183 "10_patterns.md"
    }
    else {
#line 332 "10_patterns.md"
#line 340 "10_patterns.md"
     _skptrace("notmatched  '%s' '%s'",s,p);
      // Skip over the current sub-pattern
      while (*p > '\7') p++;
     _skptrace("notmatched+ '%s' '%s'",s,p);
      if ((*p > '\0') && (p[1] > '\0')) { // Try a new pattern
        s = start;
        p++;
       _skptrace("resume from: %s (%c)", p,*s);
      }
#line 333 "10_patterns.md"
#line 355 "10_patterns.md"
      else if (skp_to) {
        goal = NULL;  goalnot = NULL;
        p = pat;
        skp_next(start,&start,flg & 2);
        s = start;
       _skptrace("retry '%s'",s);
        if (*s == '\0') break;
      }

#line 334 "10_patterns.md"
#line 371 "10_patterns.md"
       else break;
#line 335 "10_patterns.md"
#line 186 "10_patterns.md"
    }
    while (is_space(*p)) p++; // skip useless spaces in the pattern
  }

 _skptrace("pat: '%s'",p);

#line 208 "10_patterns.md"
  if (!matched && goalnot) {
    goal = goalnot;
    matched = MATCHED;
    // reach the end of the subpattern to ensure a proper return value.
    while (*p > '\7') p++;
  }

  if (goal) s = goal; // Move back the end of the string to the goal
#line 193 "10_patterns.md"
#line 224 "10_patterns.md"
  if (matched) { 
    ret = "\1\1\2\3\4\5\6\7"[(int)(*p) & 0x07]; // 0 defaults to 1

    *from = start;
    *end  = s;
  }
  else {
    ret = 0;
    // *from = txt;
    // *end  = txt;
  }

#line 194 "10_patterns.md"

  return ret;
}
#line 25 "10_patterns.md"
#line 284 "10_patterns.md"
int skp_multi(char *text,char **from, char **to, char *types, void **ptrns)
{
 _skptrace("MULTI TYPES: %s",types);
  char *tmp_from[SKP_MAXPATTERNS];
  char *tmp_to[SKP_MAXPATTERNS];
  int n = 0;
  int ret = 0;
  while(n < SKP_MAXPATTERNS && types[n]) {
    switch (types[n]) {
      case 'S' : ret = skp_s(text,(char *)(ptrns[n]), tmp_from+n, tmp_to+n);
                 if (ret == 0) return 0;
                _skptrace("S: %d %d %p %p %p %p",n,ret,tmp_from+n,tmp_to+n,tmp_from[n],tmp_to[n]);
                 break;
      case 'F' : ret = skp_f(text, (skp_funcpattern_t)(ptrns[n]), tmp_from+n, tmp_to+n);
                 if (ret == 0) return 0;
                _skptrace("F: %d %d %p %p %p %p",n,ret,tmp_from+n,tmp_to+n,tmp_from[n],tmp_to[n]);
                 break;
      case 'P' : ret = skp_p(text, ptrns[n], tmp_from+n, tmp_to+n);
                 if (ret == 0) return 0;
                _skptrace("P: %d %d %p %p %p %p",n,ret,tmp_from+n,tmp_to+n,tmp_from[n],tmp_to[n]);
                 break;
                
      default : return 0;
    }
    text = tmp_to[n];
    n++;
  }
  if (n == 0) return 0;
  from[0] = tmp_from[0];
  to[0]   = tmp_to[n-1];
  while (n>0) {
    from[n] = tmp_from[n-1];
    to[n] = tmp_to[n-1];
    n -= 1;
  }

  return ret;
}

#line 26 "10_patterns.md"
#line 482 "10_patterns.md"
static int match(char *pat, char *txt, char **pat_end, char **txt_end,int *flg)
{
  uint32_t s_chr;
  char *p_end, *s_end;
  int ret = MATCHED_FAIL;
  uint32_t match_min = 1;
  uint32_t match_max = 1;
  uint32_t match_cnt = 0;
  uint32_t match_not = 0;
  int intnumber  = false;
  char *s_tmp = txt;
  
  s_end = txt;
  s_chr = skp_next(s_end, &s_tmp,*flg & 2);

    if (*pat == '*') { match_min = 0;  match_max = UINT32_MAX; pat++; } 
    else if (*pat == '+') { match_max = UINT32_MAX; pat++; } 
    else if (*pat == '?') { match_min = 0; pat++; }
    
    if (*pat == '!') { match_not = 1; pat++; }
   _skptrace("min: %u max: %u not: %u",match_min, match_max, match_not);

    #define W(x)  \
      do { \
       _skptrace("matchedW: '%s' schr: %X test: %d (%s)",s_end,s_chr,(x),#x); \
        for (match_cnt = 0; \
             (match_cnt < match_max) && (s_chr && (!!(x) != match_not)); \
             match_cnt++) { \
          s_end = s_tmp; s_chr = skp_next(s_end,&s_tmp,*flg & 2); \
        } \
        ret = (match_cnt >= match_min); \
       _skptrace("cnt: %d ret: %d s: %c end: %c",match_cnt,ret,*s_tmp, *s_end); \
      } while (0)

    #define get_next_s_chr()   do {s_end = s_tmp; s_chr = *s_end ; s_tmp++;} while(0)
    #define get_next_U_s_chr() do {s_end = s_tmp; s_chr = skp_next(s_end,&s_tmp, *flg & 2); } while (0)
    
    intnumber = false;
    
    switch (*pat++) {

      case '.' : if (match_not) ret = (s_chr == 0);
                 else W(s_chr != 0);
                 break;

      case '$' : if (s_chr == 0) ret = 1; else 
      case 'n' : W(is_break(s_chr));
                 break;

      case 'd' : W(is_digit(s_chr));  break;
      case 'x' : W(is_xdigit(s_chr)); break;
      case 'a' : W(is_alpha(s_chr));  break;
      case 'u' : W(is_upper(s_chr));  break;
      case 'l' : W(is_lower(s_chr));  break;
      case 's' : W(is_space(s_chr));  break;
      case 'w' : W(is_blank(s_chr));  break;
      case 'c' : W(is_ctrl(s_chr));   break;
      case 'i' : W(is_idchr(s_chr));  break;
      case '@' : W(is_alnum(s_chr));  break;
      
      case '&' : ret = match_not? MATCHED_GOALNOT : MATCHED_GOAL;
                 break;

      case '[' : W(is_oneof(s_chr,pat,*flg & 2));
                 if (*pat == ']') pat++;
                 while (*pat && *pat != ']') pat++;
                 pat++;
                 break;

      case '"' : case '\'': case '`': {
                 int l = 0; int ml ; uint32_t quote = pat[-1];
                 while (pat[l] && pat[l] != quote) l++;
                 if (l>0 && ((ml=is_string(s_end,pat,l,*flg)) > 0)) {
                   if (!match_not) {
                     s_end += ml;
                     ret = MATCHED;
                   }
                 }
                 else if (match_min == 0 || match_not) ret = MATCHED;
                 pat += l+1;
                 break;  
               }
    
      case 'C' : *flg = (*flg & ~1) | match_not; ret = MATCHED;
                _skptrace("FOLD: %d",*flg & 1);
                 break;

      case 'U' : *flg = (*flg & ~2) | (match_not * 2); ret = MATCHED;
                _skptrace("ISO: %d", (*flg & 2));
                 break;

      case 'S' : while (is_space(s_chr)) get_next_U_s_chr();;
                 ret = MATCHED;
                 break;

      case 'W' : while (is_blank(s_chr)) get_next_U_s_chr();;
                 ret = MATCHED;
                 break;

      case 'N' : // Up to end of line
                 if (match_not) 
                   ret = !(s_chr && !is_break(s_chr));
                 else {
                   while(s_chr && !is_break(s_chr)) {
                      get_next_U_s_chr();
                   }
                   ret = MATCHED;
                 }
                 break;

      case 'I' : // Identifier
                 if (is_alpha(s_chr) || (s_chr == '_')) {
                   do {
                     get_next_s_chr();
                   } while (is_alnum(s_chr) || (s_chr == '_'));
                   ret = MATCHED;
                 } 
                 if (match_not) {
                   ret = !ret;
                   s_end = txt;
                 }
                 break;

      case '(' : if (*pat != ')') break;
                 pat++;
                 if (s_chr == '(')
      case 'B' : // Balanced parenthesis
                 {
                   uint32_t open;
                   uint32_t close;
                   int32_t count;
                   open = s_chr;
                   close = get_close(open);
                   if (close != '\0') {
                     count=1;
                     while (s_chr && count > 0) {
                       get_next_s_chr();
                       if (s_chr == open)  count++;
                       if (s_chr == close) count--;
                     }
                     if (count == 0) {
                       get_next_s_chr();
                       ret = MATCHED;
                     }
                   }
                 }
                 if (match_not) {
                   ret = !ret;
                   s_end = txt;
                 }
                 break;

      case 'Q' : // Quoted string
                 {
                   uint32_t qclose;
                   qclose = get_qclose(s_chr);
                   if (qclose != '\0') {
                     while (s_chr) {
                       get_next_s_chr();
                       if (s_chr == qclose) break;
                       if (s_chr == '\\')  get_next_s_chr();
                     }
                     if (s_chr) {
                       get_next_s_chr();
                       ret = MATCHED;
                     }
                   }
                   if (match_not) {
                     ret = !ret;
                     s_end = txt;
                   }
                 }
                 break;

      case 'X' : // hex number
                 if (   (s_chr == '0')
                     && (s_end[1] == 'x' || s_end[1] == 'X') 
                     && is_xdigit(s_end[2])
                    ) {
                   get_next_s_chr();
                   get_next_s_chr();
                   get_next_s_chr();
                   ret = MATCHED;
                 } 
                 while (is_xdigit(s_chr)) {
                   ret = MATCHED;
                   get_next_s_chr();
                 }
                 if (match_not) {
                   ret = !ret;
                   s_end = txt;
                 }
                 break;

      case 'D' : // Integer number 
                intnumber = true;

      case 'F' : // Floating point number
                 if (s_chr == '+' || s_chr == '-') { // sign
                   do {
                     get_next_s_chr();
                   } while (is_space(s_chr));
                 } 
   
                 while (is_digit(s_chr)) {
                   ret = MATCHED;
                   get_next_s_chr();
                 }
 
                 if (!intnumber) {
 
                   if (s_chr == '.') {
                     get_next_s_chr();
                   }
   
                   while (is_digit(s_chr)) {
                     ret = MATCHED;
                     get_next_s_chr();
                   }
     
                   if ((ret == MATCHED) && (s_chr == 'E' || s_chr == 'e')) {
                     get_next_s_chr();
                     if (s_chr == '+' || s_chr == '-')  get_next_s_chr();
                     while (is_digit(s_chr)) get_next_s_chr();
                     if (s_chr == '.') get_next_s_chr();
                     while (is_digit(s_chr)) get_next_s_chr();
                   }
                 }
                 if (match_not) {
                   ret = !ret;
                   s_end = txt;
                 }
                 break;

      default  : ret = MATCHED_FAIL; pat--; break;
    }
    p_end = pat;

  if (ret != MATCHED_FAIL) {
    if (pat_end) *pat_end = p_end;
    if (txt_end) *txt_end = s_end;
  }
  return ret;
}

#line 27 "10_patterns.md"

#line 1012 "10_patterns.md"

uint32_t skp_next(char *text,char **end,int iso)
{
  uint32_t c = 0;
  unsigned char *s = (unsigned char *)text;
  if (s && *s) {
    c = (*s++ & 0xFF);
    if (!iso) {
#if 1
      if ((*s & 0xC0) == 0x80) {
        c = (c << 8) | (*s++ & 0xFF);
        if ((*s & 0xC0) == 0x80) {
          c = (c << 8) | (*s++ & 0xFF);
          if ((*s & 0xC0) == 0x80) {
            c = (c << 8) | (*s++ & 0xFF);
          }
        }
      }
     _skptrace("Next: %08X",c);
#else
      while ((*s & 0xC0) == 0x80) {
        c = (c << 8) | *s++;
      }
#endif
    }
#line 1038 "10_patterns.md"
  }

 _skptrace("next: %08X iso: %d",c,iso);
  if (end) *end = (char *)s;
  return c;
}
#line 29 "00_header.md"
  #endif // SKP_MAIN

  

#endif // SKP_VERSION

#line 43 "00_header.md"
#line 59 "00_header.md"
#line 79 "00_header.md"
#line 97 "00_header.md"
#line 21 "10_patterns.md"
#line 66 "10_patterns.md"
#line 100 "10_patterns.md"
#line 147 "10_patterns.md"
#line 207 "10_patterns.md"
#line 223 "10_patterns.md"
#line 256 "10_patterns.md"
#line 331 "10_patterns.md"
#line 339 "10_patterns.md"
#line 354 "10_patterns.md"
#line 370 "10_patterns.md"
#line 380 "10_patterns.md"
#line 411 "10_patterns.md"
#line 430 "10_patterns.md"
#line 446 "10_patterns.md"
#line 457 "10_patterns.md"
#line 481 "10_patterns.md"
#line 730 "10_patterns.md"
#line 998 "10_patterns.md"
#line 1055 "10_patterns.md"
#line 85 "20_scanner.md"

#line 116 "20_scanner.md"
#line 131 "20_scanner.md"
