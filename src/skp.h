/*
**  (C) by Remo Dentato (rdentato@gmail.com)
** 
** This software is distributed under the terms of the MIT license:
**  https://opensource.org/licenses/MIT
*/

#ifndef SKP_VER
#define SKP_VER 0x0003000C
#define SKP_VER_STR "0.3.0rc"

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

// Borrowed from vrg.h but renamed to minimize namespace pollution.
#define skp_v_cnt(skp_v1,skp_v2,skp_v3,skp_v4,skp_v5,skp_v6,skp_vN, ...) skp_vN
#define skp_v_argn(...)  skp_v_cnt(__VA_ARGS__, 6, 5, 4, 3, 2, 1, 0)
#define skp_v_cat0(x,y)  x ## y
#define skp_v_cat(x,y)   skp_v_cat0(x,y)
#define skp_varg(skp_v_f,...) skp_v_cat(skp_v_f, skp_v_argn(__VA_ARGS__))(__VA_ARGS__)

int skp_(char *src, char *pat, char **end, char **to);

#define skp(...)    skp_varg(skp_,__VA_ARGS__)
#define skp_2(s,p)      skp_(s,p,NULL,NULL)
#define skp_3(s,p,e)    skp_(s,p, e,NULL);
#define skp_4(s,p,e,t)  skp_(s,p, e,t);

typedef struct {
  char *start;
  char *to;
  char *end;
  int   alt;
} skp_loop_t;

#define skpstart skp_loop.start
#define skpto    skp_loop.to
#define skpend   skp_loop.end
#define skpalt   skp_loop.alt
#define skplen   skp_loop_len(skp_loop.start,skp_loop.to)

static inline int skp_loop_len(char *start, char *to)
{int ret = to-start; return (0 <= ret && ret <= (1<<16)?ret:0);}

// Just a caution to avoid aggressive optimization.
extern volatile int skp_zero;

#define skp_1(s) for (skp_loop_t skp_loop = {s,NULL,NULL,1}; \
                      skpstart && *skpstart && skpalt && !(skpalt = skp_zero);\
                      skpstart = skpto)

#define skpif(p) if (skpalt || !(skpalt = skp_(skpstart,p,&skpto,&skpend))) ; else

#define skpelse(p) if (skpalt || !(skpend = skpto = skpstart)) ; else


// ************************************************************ 
// MAIN *******************************************************
// ************************************************************ 
// Borrowed from dbg.h but renamed to minimize namespace pollution.
#define _skptrace(...)
#define  skptrace(...) (fprintf(stderr,"TRCE: " __VA_ARGS__), \
                         fprintf(stderr," \xF%s:%d\n",__FILE__,__LINE__))

#ifdef SKP_MAIN

/*
   a  ASCII alphabetic char
   l  ASCII lower case
   u  ASCII upper case
   d  decimal digit
   x  hex digit
   w  white space (includes some Unicode spaces)
   s  white space and vertical spaces (e.g. LF)
   c  control
   n  newline

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

   @  set goal
   !@ set negative goal

   [...] set
  
   .  (any non \0 character)
   !. (end of text)

   >  skip to the start of pattern

   & the character '&'
*/

volatile int skp_zero = 0;

static uint32_t skp_next(char *s,char **end,int iso)
{
  uint32_t c = 0;

  if (s && *s) {
    c = *s++;
    if (!iso) {
      while ((*s & 0xC0) == 0x80) {
         c = (c << 8) | *s++;
      }
    }
    if (c == 0x0D && *s == 0x0A) {
      c = 0x0D0A; s++;
    }
  }

  if (end) *end = s;
  return c;
}

static int chr_cmp(uint32_t a, uint32_t b, int fold)
{ _skptrace("CMP: %d %c %c",fold, a,b);
  if (fold && a <= 0xFF && b <= 0xFF) {
    a = tolower(a);
    b = tolower(b);
  }
  return (a == b);
}

static int is_blank(uint32_t c)
{
  return (c == 0x20) || (c == 0x09)
      || (c == 0xA0) || (c == 0xC2A0)
      || (c == 0xE19A80)
      || ((0xE28080 <= c) && (c <= 0xE2808A))
      || (c == 0xE280AF)
      || (c == 0xE2819F)
      || (c == 0xE38080)
      ;
}

static int is_break(uint32_t c)
{
  return (c == 0x0A)      // U+000A LF line feed           
      || (c == 0x0C)      // U+000C FF form feed          
      || (c == 0x0D)      // U+000D CR carriage return     
      || (c == 0x85)      // U+0085 NEL next line   ISO-8859-15      
      || (c == 0x0D0A)    // CRLF (not a real UTF-8 CODEPOINT!!!)
      || (c == 0xC285)    // U+0085 NEL next line         
      || (c == 0xE280A8)  // U+2028 LS line separator     
      || (c == 0xE280A9)  // U+2029 PS paragraph separator
      ;
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

static int is_alnum(uint32_t c)
{ return (is_alpha(c) || is_digit(c)); }

static int is_ctrl(uint32_t c)
{ return (c < 0x20)
      || (0xC280 <= c && c <= 0xC2A0)
      || (0x80 <= c && c <= 0xA0);
}

static int is_oneof(uint32_t ch, char *set, int iso)
{
  uint32_t p_ch,q_ch;
  char *s;
  if (ch == '\0') return 0;
 _skptrace("set: [%s] chr: %c",set,ch);
  p_ch = skp_next(set,&s,iso);
  
  if (p_ch == ']' && ch == ']') return 1;

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

#define MATCHED_FAIL    0 
#define MATCHED         1
#define MATCHED_GOAL    2
#define MATCHED_GOALNOT 3

static int match(char *pat, char *src, char **pat_end, char **src_end,int *flg)
{
  uint32_t p_chr, s_chr;
  char *p_end, *s_end;
  int ret = 0;
  uint32_t match_min = 1;
  uint32_t match_max = 1;
  uint32_t match_cnt = 0;
  uint32_t match_not = 0;
  int intnumber  = false;
  char *s_tmp = src;
  
  s_end = src;
  s_chr = skp_next(s_end, &s_tmp,*flg & 2);

  if (*pat == '&') {
    pat++;
    
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

    #define get_next_s_chr() do {s_end = s_tmp; s_chr = *s_end ; s_tmp++;} while(0)
    
    intnumber = false;
    
    switch (*pat++) {
      case '&' : ret = (s_chr == '&') ;  break;

      case '.' : if (match_not) ret = (s_chr == 0);
                 else W(s_chr != 0);
                 break;

      case 'd' : W(is_digit(s_chr));  break;
      case 'x' : W(is_xdigit(s_chr)); break;
      case 'a' : W(is_alpha(s_chr));  break;
      case 'u' : W(is_upper(s_chr));  break;
      case 'l' : W(is_lower(s_chr));  break;
      case 's' : W(is_space(s_chr));  break;
      case 'w' : W(is_blank(s_chr));  break;
      case 'n' : W(is_break(s_chr));  break;
      case 'c' : W(is_ctrl(s_chr));   break;
      
      case '@' : ret = match_not? MATCHED_GOALNOT : MATCHED_GOAL;
                 break;

      case '[' : W(is_oneof(s_chr,pat,*flg & 2));
                 while (*pat && *pat != ']') pat++;
                 if (*pat && pat[1]==']') pat++;
                 pat++;
                 break;

      case 'C' : *flg = (*flg & ~1) | match_not; ret = MATCHED;
                _skptrace("FOLD: %d",*flg & 1);
                 break;

      case 'U' : *flg = (*flg & ~2) | (match_not * 2); ret = MATCHED;
                 break;

      case 'N' : // Up to end of line
                 while(s_chr && !is_break(s_chr)) {
                    get_next_s_chr();
                 }
                 if (s_chr) {
                   get_next_s_chr();
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
                 break;

      case '(' : if (*pat != ')' || s_chr != '(') break;
                 pat++;

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
                 break;

      case 'D' : // Integer number 
                intnumber = true;

      case 'F' : // Floating point number
                 if (s_chr == '+' || s_chr == '-') {
                   do {
                     get_next_s_chr();
                   } while (is_space(s_chr));
                 } 
   
                 while (is_digit(s_chr)) {
                   ret = MATCHED;
                   get_next_s_chr();
                 }
 
                 if (intnumber) break;
 
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
 
                 break;

      default  : ret = MATCHED_FAIL; pat--; break;
    }
    p_end = pat;
  }
  else {
    p_chr = skp_next(pat,&p_end,*flg & 2);
    s_end = s_tmp;
    ret = chr_cmp(s_chr,p_chr,*flg & 1); 
  }

  if (ret != MATCHED_FAIL) {
    if (pat_end) *pat_end = p_end;
    if (src_end) *src_end = s_end;
  }
  return ret;
}

int skp_(char *src, char *pat, char **to,char **end)
{
  char *start = src;
  char *s; char *p;
  char *s_end=NULL; char *p_end=NULL;
  int   skp_to = 0;
  int   matched = 0;
  int   ret = 0;
  char *goal = NULL;
  char *goalnot = NULL;
  int   flg = 0;

  if (!pat || !src) { return 0; }

 _skptrace("SKP_: src:'%s' pat:'%s'",src,pat);

  if ((pat[0] == '&') && (pat[1] == '>')) {
    skp_to = 1;
    pat   += 2;
  }
 _skptrace("SKP_: src:'%s' pat:'%s' skp_to:%d",src,pat,skp_to);

  p = pat;
  s = start;
  while (*p > '\7') {
    if ((matched = match(p,s,&p_end,&s_end,&flg))) {
     _skptrace("matched( '%s' '%s'",s,p);
      s = s_end; p = p_end;
     _skptrace("matched) '%s' '%s'",s,p);
      if (matched == MATCHED_GOAL && !goalnot) goal = s;
      else if (matched == MATCHED_GOALNOT) goalnot = s;
    }
    else {
     _skptrace("notmatched  '%s' '%s'",s,p);
      while (*p > '\7') p++;
     _skptrace("notmatched+ '%s' '%s'",s,p);
      if ((*p > '\0') && (p[1] > '\0')) { // Try a new pattern
        s = start;
        p++;
       _skptrace("resume from: %s (%c)", p,*s);
      }
      else if (skp_to) {
        goal = NULL;  goalnot = NULL;
        p = pat;
        s = ++start;
       _skptrace("retry '%s'",s);
        if (*s == '\0') break;
      }
      else break;
    }
  }

 _skptrace("pat: '%s'",p);

  if (!matched && goalnot) {
    goal = goalnot;
    matched = MATCHED;
    p="";
  }

  if (goal) s = goal;

  if (matched && (*p <= '\7')) {
    ret = (*p > 0)? *p : 1;

    if (to)  *to  = skp_to?start:s;
    if (end) *end = s;
    return ret;
  }

  if (end) *end = src;
  return 0;
}

#endif // SKP_MAIN

// ************************************************************ 
// PARSE ******************************************************
// ************************************************************ 

extern char *skp_N_STR;
extern char *skp_N_STR1;
#define skp_N_STR2 (skp_N_STR1 + (1*4))
#define skp_N_STR3 (skp_N_STR1 + (2*4))
#define skp_N_STR4 (skp_N_STR1 + (3*4))
#define skp_N_STR5 (skp_N_STR1 + (4*4))
#define skp_N_STR6 (skp_N_STR1 + (5*4))
#define skp_N_STR7 (skp_N_STR1 + (6*4))

/*

    +-------------,
    ^             |
    | |   | |     v
    ( ( ) ( ( ) ) )
    ^ ^-+ ^ ^-+ + +
    |     '-----' |
    '-------------'
*/
typedef struct ast_node_s {
  char   *rule;
  int32_t from;
  int32_t to;
  int32_t delta; // delta between parenthesis (>0)
} ast_node_t;

typedef struct ast_mmz_s {
  int32_t pos;
  int32_t endpos;
  int32_t numnodes;
  int32_t maxnodes;
  ast_node_t nodes[0];
} *ast_mmz_t;

#define SKP_MMZ_NULL ((ast_mmz_t)-1)

typedef struct ast_s {
  char       *start;
  char       *err_rule;
  ast_node_t *nodes;
  ast_mmz_t  **mmz;
  int32_t    *par;  // ≥ 0 ⇒ Open parenthesis (index into the nodes array)
                    // < 0 ⇒ Closed parenthesis (delta to the open par.)
  int32_t     nodes_cnt;
  int32_t     nodes_max;
  int32_t     par_cnt;
  int32_t     par_max;
  int32_t     pos;
  int32_t     err_pos;
  int32_t     mmz_cnt;
  int32_t     mmz_max;
  int32_t     last_info;
  int16_t     fail;
} *ast_t;

char *asterrorrule(ast_t ast);
char *asterror(ast_t ast);

ast_t ast_new();
ast_t astfree(ast_t ast);

int32_t ast_open(ast_t ast, int32_t from, char *rule);
int32_t ast_close(ast_t ast, int32_t to, int32_t open);

typedef void (*skprule_t)(ast_t);

ast_t skp_parse(char *src, skprule_t rule,char *rulename);
#define skpparse(s,r)  skp_parse(s, skp_R_ ## r, skp_N_ ## r)

#define skpdef(rule) \
    char *skp_N_ ## rule = #rule; \
    ast_mmz_t skp_M_ ## rule [4] = {SKP_MMZ_NULL, SKP_MMZ_NULL, SKP_MMZ_NULL, SKP_MMZ_NULL}; \
    void  skp_R_ ## rule (ast_t ast_) 

#define skp_match(how) \
    if (!ast_->fail) { \
      char *from = ast_->start+ast_->pos; int32_t par;\
      how;\
      if (n==0) {ast_->fail = 1;} \
      else { \
        par = ast_open(ast_,ast_->pos,skp_N_STR1+(n-1)*4);\
        ast_->pos += len; \
        par = ast_close(ast_,ast_->pos,par); \
      } \
    } else (void)0

#define skp_match_(how) \
    if (!ast_->fail) { \
      char *from = ast_->start+ast_->pos;\
      how;\
      if (n==0) { ast_->fail = 1;} \
      else { ast_->pos += len;} \
    } else (void)0

#define skp_how_match(pat) \
    char *to=from; int n = skp(from,pat,&to); int32_t len = (int32_t)(to-from)

#define skpmatch(pat)  skp_match(skp_how_match(pat))
#define skpmatch_(pat) skp_match_(skp_how_match(pat))

#define skp_how_string(str,alt) \
    int32_t len = strlen(str); int n = strncmp(from,str,len)?0:(alt)

#define skpstring(...)     skp_varg(skp_string,__VA_ARGS__)
#define skp_string1(str)            skp_string2(str,1)
#define skp_string2(str,alt)        skp_match(skp_how_string(str,alt))
#define skpstring_(str)             skp_match_(skp_how_string(str,1))

#define skpanyspaces()  skpmatch_("&*s");
#define skpmanyspaces() skpmatch_("&+s");
#define skpanyblanks()  skpmatch_("&*w");
#define skpmanyblanks() skpmatch_("&+w");

#define skplookup(...)      skp_varg(skp_lookup,__VA_ARGS__)
#define skp_lookup1(f)      skp_lookup2(f,1)
#define skp_lookup2(f,n)    skp_fcall(f,n) \
                            if (!ast_->fail) switch(ast_->last_info)

#define skplookup_(f)  skp_fcall_(f) \
                       if (!ast_->fail) switch(ast_->last_info)

#define skpcheck(...)      skp_varg(skp_check,__VA_ARGS__)
#define skp_check1(f)      skp_check2(f,1)
#define skp_check2(f,n)    skp_fcall(f,n) (void)0

#define skpcheck_(f)       skp_fcall_(f) (void)0

#define skp_fcall(f,n) \
    if (!ast_->fail) { \
      char *ptr = ast_->start+ast_->pos;\
      int32_t info = f(&ptr); \
      int32_t par; \
      if (ptr == NULL) {ast_->fail = 1;} \
      else { \
        if (info != 0) ast_setinfo(ast_,info); \
        if (info >= 0) par = ast_open(ast_,ast_->pos,skp_N_STR1+(n-1)*4);\
        ast_->pos = (int32_t)(ptr-ast_->start); \
        if (info >= 0) par = ast_close(ast_,ast_->pos,par); \
      } \
    } 

#define skp_fcall_(f) \
    if (!ast_->fail) { \
      char *ptr = ast_->start+ast_->pos;\
      int32_t info = f(&ptr); \
      if (ptr == NULL) {ast_->fail = 1;} \
      else { \
        ast_->last_info = info; \
        ast_->pos = (int32_t)(ptr-ast_->start); \
      } \
    } 

typedef struct {
  int32_t pos;
  int32_t par_cnt;
  int32_t nodes_cnt;
  int16_t flg;
} skp_save_t ;

#define skp_save    skp_save_t sav = (skp_save_t) {ast_->pos, ast_->par_cnt, ast_->nodes_cnt, 3} 
#define skp_resave  (sav.pos = ast_->pos, sav.par_cnt = ast_->par_cnt, sav.nodes_cnt = ast_->nodes_cnt,0)
#define skp_restore (ast_->pos = sav.pos, ast_->par_cnt = sav.par_cnt, ast_->nodes_cnt = sav.nodes_cnt)

#define skprule_(rule) \
    if (!ast_->fail) { \
      extern char *skp_N_ ## rule; \
      extern ast_mmz_t skp_M_ ## rule [4]; \
      void skp_R_ ## rule (ast_t ast_); \
      skp_save;\
      if (!skp_dememoize(ast_, skp_M_ ## rule ,skp_N_ ## rule)) { \
        skp_R_ ## rule(ast_); \
        if (ast_->fail) { \
          if (ast_->err_pos < ast_->pos) { \
            ast_->err_pos = ast_->pos; ast_->err_rule = skp_N_ ## rule; \
          } \
          skp_restore; \
        } \
        skp_memoize(ast_, skp_M_ ## rule ,skp_N_ ## rule, sav.pos, sav.par_cnt); \
      } \
    } else (void)0

#define skprule(rule) \
    if (!ast_->fail) { \
      extern char *skp_N_ ## rule; \
      extern ast_mmz_t skp_M_ ## rule [4]; \
      void skp_R_ ## rule (ast_t ast_); \
      int32_t sav_par_cnt = ast_->par_cnt; \
      int32_t sav_pos = ast_->pos;\
      if (!skp_dememoize(ast_, skp_M_ ## rule, skp_N_ ## rule)) { \
        int32_t par = ast_open(ast_,ast_->pos, skp_N_ ## rule); \
        skp_R_ ## rule(ast_); \
        if (ast_->fail) { \
          if (ast_->err_pos < ast_->pos) { \
            ast_->err_pos = ast_->pos; ast_->err_rule = skp_N_ ## rule; \
          } \
        } \
        par = ast_close(ast_,ast_->pos,par); \
        skp_memoize(ast_, skp_M_ ## rule ,skp_N_ ## rule,sav_pos,sav_par_cnt); \
      } \
    } else (void)0


//
// sav.flg controls the flow. It is initialized to 3
//   xxxx xx11
//          \\__ 1: 1 if it's the first time
//           \__ 2: 1 if stuff to be done
//
// Using two flags is really needed only for skpmany that has to
// determine if it has succeeded at least once.

#define skponce \
    if (ast_->fail) ;\
    else for (skp_save; \
              sav.flg & 2; \
              sav.flg = ast_->fail?(skp_restore,0):0)

#define skpor \
    if (!ast_->fail || (ast_->fail = 0)) ;\
    else for (skp_save; \
              sav.flg & 2; \
              sav.flg = ast_->fail?(skp_restore,0):0)

#define skpmaybe \
    if (ast_->fail) ;\
    else for (skp_save;\
              sav.flg & 2;\
              sav.flg = ast_->fail?(skp_restore,0):0, ast_->fail = 0)

#define skpnot \
    if (ast_->fail) ;\
    else for (skp_save;\
              sav.flg & 2;\
              skp_restore, sav.flg = 0, ast_->fail = !ast_->fail)

#define skppeek \
    if (ast_->fail) ;\
    else for (skp_save;\
              sav.flg & 2;\
              skp_restore, sav.flg = 0)

#define skpany \
    if (ast_->fail) ;\
    else for (skp_save;\
              sav.flg & 2;\
              ast_->fail = ast_->fail? (skp_restore, (sav.flg = 0)) \
                                     : skp_resave)
// On fail, sav.flg will be 1 if it's the first time or 0 if we have succeeded
// at least once (in which case `sav.flg=2` had cleared the first bit).
#define skpmany \
    if (ast_->fail) ;\
    else for (skp_save;\
              sav.flg & 2;\
              ast_->fail = ast_->fail? (skp_restore, (sav.flg &= 1) ) \
                                     : ((sav.flg = 2), skp_resave))


void skp_memoize(ast_t ast, ast_mmz_t *mmz,char *rule, int32_t old_pos, int32_t start_par);
int skp_dememoize(ast_t ast, ast_mmz_t *mmz, char *rule);

#define astinfo(n) ast_setinfo(ast_,n)
void ast_setinfo(ast_t ast, int32_t info);
int32_t astnodeinfo(ast_t ast, int32_t node);

#define astswap ast_swap(ast_)
void ast_swap(ast_t ast);

#define astlift ast_lift(ast_)
void ast_lift(ast_t ast);

#define astnoleaf ast_noleaf(ast_)
void ast_noleaf(ast_t ast);

#define astlastinfo (ast_->last_info)

#define astnodeis(a,n,r) ast_is(a,n,skp_N_ ## r)
int ast_is(ast_t ast, int32_t node, char*rulename);
int astisleaf(ast_t ast, int32_t node);

#define astnumnodes(a) ((a)->nodes_cnt)
void astprint(ast_t ast, FILE *f);

#define ASTNULL -1
int32_t astnextdf(ast_t ast, int32_t ndx);

#define astvisitdf(a,n) for (int32_t n=ASTNULL; (n=astnextdf(a,n)) != ASTNULL; )

int astisnodeentry(ast_t ast, int32_t ndx);
int astisnodeexit(ast_t ast, int32_t ndx);

char *astnoderule(ast_t ast, int32_t node);
char *astnodefrom(ast_t ast, int32_t node);
char *astnodeto(ast_t ast, int32_t node);
int32_t astnodelen(ast_t ast, int32_t node);


#ifdef SKP_MAIN

static int skp_par_makeroom(ast_t ast,int32_t needed);
static int skp_nodes_makeroom(ast_t ast,int32_t needed);

void skp_memoize(ast_t ast, ast_mmz_t *mmz, char *rule, int32_t old_pos, int32_t start_par)
{
  int32_t end_par = -1;
  int32_t slot = -1;
  int32_t min_pos = INT32_MAX;
  int32_t min_slot = 0;
  int32_t numnodes;

  assert(mmz);

  end_par = ast->par_cnt;
  if (ast->fail || end_par <= start_par) {
    start_par = -1;
    end_par = -1;
  }
  numnodes = (end_par - start_par)/2;

 _skptrace("MMZ: [%d] %s @%d par:[%d,%d]",ast->fail,rule,old_pos, start_par,end_par);
  // Find the proper slot
  //    Any empty slot?
  for (slot = 0; (slot<4) && mmz[slot] ; slot++) {
    if (mmz[slot]->pos > min_pos) {
       min_pos = mmz[slot]->pos;
       min_slot = slot;
    };
  }

  // If none, use the one with lowest pos.
  if (slot >= 4) {
    slot = min_slot;
    // Check if there's enough memory
    if (mmz[slot]->maxnodes < numnodes) {
      free(mmz[slot]);
      mmz[slot] = NULL;
    }
  }
 _skptrace("MMZ: storing %d",slot);
  if (mmz[slot] == NULL) {
    mmz[slot] = malloc(sizeof(struct ast_mmz_s) + numnodes * sizeof(ast_node_t));
    assert(mmz[slot]);
    mmz[slot]->maxnodes = numnodes;
  }

  // Record position
 _skptrace("MMZ: storing %d pos: %d",slot,old_pos);
  mmz[slot]->pos = old_pos;
  mmz[slot]->endpos = ast->pos;
  // Record fail
  mmz[slot]->numnodes = ast->fail? -1 : numnodes;

  // Save nodes (realloc if needed)
  //ast_node_t *nd;
  int32_t cur_node = 0;
  for (int32_t k=start_par; k<end_par; k++) {
    if (ast->par[k] >=0) {
      //nd = & ast->nodes[ast->par[k]];
      //skptrace("MMZ:   [%d] from: %d to: %d delta: %d rule: '%s':",k,nd->from,nd->to,nd->delta,nd->rule);
      mmz[slot]->nodes[cur_node++] = ast->nodes[ast->par[k]];
    }
  }

}

void skp_mmz_add(ast_t ast, ast_mmz_t *mmz)
{
  int32_t new_max;
  ast_mmz_t  **new_mmz;
  if (ast->mmz_cnt >= ast->mmz_max) {
    new_max = ast->mmz_max;
    new_max += new_max/2;
    new_max += new_max&1;
    new_mmz = realloc(ast->mmz,new_max * sizeof(ast_mmz_t *));
    assert(new_mmz);
    ast->mmz = new_mmz;
    ast->mmz_max = new_max;
  }
  ast->mmz[ast->mmz_cnt++] = mmz;
}

void skp_mmz_clean(ast_t ast)
{
  ast_mmz_t *mmz;
 _skptrace("MMZ clean: %p (%d)",(void *)ast->mmz,ast->mmz_cnt);
  while (ast->mmz_cnt) {
    mmz = ast->mmz[--ast->mmz_cnt];
    for (int k=0; k<4; k++) {
     _skptrace("MMZ clean: %p %p",(void *)mmz,(void *)mmz[k]);
      if (mmz[k]) free(mmz[k]); 
      mmz[k] = SKP_MMZ_NULL;
    }
  }
  ast->mmz_cnt = 0;
}

#define SKP_DELTA_MAX INT32_MAX
int skp_dememoize(ast_t ast, ast_mmz_t *mmz, char *rule)
{
 _skptrace("MM?: '%s' %d",rule,ast->pos);
  if (mmz[0] == SKP_MMZ_NULL) {
    skp_mmz_add(ast, mmz);
    for (int k=0; k<4; k++) mmz[k] = NULL;
   _skptrace("MM?: first mmz");
    return 0;
  }
  // Search for pos
  int slot = 0;
  while(1) {
   _skptrace("MM?:   search %d",slot);
    if (mmz[slot] && (mmz[slot]->pos == ast->pos)) break;
    if (++slot >= 4) return 0;
  }
  int32_t numnodes = mmz[slot]->numnodes;
 _skptrace("MM?: found at %d (nodes: %d)",slot,numnodes);
  int32_t cur_par, delta;
  ast->fail = (numnodes < 0); // restore fail flag
  ast->pos = mmz[slot]->endpos;
  if (numnodes > 0) {
    // Copy memoized result into the AST
    skp_nodes_makeroom(ast, numnodes); // ensure enough space for nodes
    skp_par_makeroom(ast,2 * numnodes); // same for parenthesis
    for (int k=ast->par_cnt; k<ast->par_cnt + 2*numnodes; k++) 
      ast->par[k] = SKP_DELTA_MAX;

    cur_par = ast->par_cnt;
    for (int32_t k=0; k<numnodes; k++) {
      // Copy node
      ast->nodes[ast->nodes_cnt] = mmz[slot]->nodes[k];
      // Rebuild tree
      while (ast->par[cur_par] != SKP_DELTA_MAX) cur_par++;
      ast->par[cur_par] = ast->nodes_cnt;
      delta = mmz[slot]->nodes[k].delta;
      ast->par[cur_par+delta] = -delta;
      ast->nodes_cnt++;
    }
    ast->par_cnt += 2*numnodes;
  } 
  return 1;
}

char *skp_N_STR1 = "$1\0 $2\0 $3\0 $4\0 $5\0 $6\0 $7\0";
char *skp_N_STR  = "$";
char *skp_N_INFO = "#";

ast_t skp_parse(char *src, skprule_t rule,char *rulename)
{
  ast_t ast = NULL;
  int32_t open;
  if (!(ast = ast_new())) return NULL;
  ast->start = src;
  ast->pos = 0;
  ast->fail = 0;
  ast->err_pos = -1;
  ast->err_rule = NULL;

 _skptrace("Parsing %s",rulename);
  if ((open = ast_open(ast, ast->pos, rulename)) >= 0) {
    rule(ast);
    if (ast->fail && ast->err_pos < ast->pos) {
      ast->err_pos = ast->pos; ast->err_rule = rulename;
    } 
    ast_close(ast, ast->pos, open);
    if (ast->nodes_cnt > 0) ast->err_pos = -1;
  }
  skp_mmz_clean(ast);
  return ast;
}


#if 0
static void dbg__prt(void *p,int32_t l)
{
  int8_t *s = p;
  while(l--) printf("%02X ",*s++);
  putchar('\n');
}
#endif

void ast_swap(ast_t ast)
{
  // Swap the last two nodes in the AST (if they belong to the same subtree)
  //          ,--- o1
  //         / ,-- c1
  //  (BBBB)(AA)
  //   \   '------ c2
  //    '--------- o2   
  //
  //  (AA)(BBBB)
  //   \  '------- o2+(c1-o1+1)
  //    '--------- o2   

  int32_t o1, c1, o2, c2;

 _skptrace("SWAP fail? %d",ast->fail);
  if (ast->fail || ast->par_cnt < 4) return;

  c1 = ast->par_cnt-1;
 _skptrace("SWAP c1: %d (%d)",c1,ast->par[c1]);
  if (c1<0 || ast->par[c1] >= 0) return;

  o1 = c1+ast->par[c1];
 _skptrace("SWAP o1: %d (%d)",o1,ast->par[o1]);
  if (o1<0 || ast->par[o1] < 0) return;

  c2 = o1-1;
 _skptrace("SWAP c2: %d (%d)",c2,ast->par[c2]);
  if (c2<0 || ast->par[c2] >= 0) return;

  o2 = c2+ast->par[c2];
 _skptrace("SWAP o2: %d (%d)",o2,ast->par[o2]);
  if (o2<0 || ast->par[o2] < 0) return;
  
  void *tmp = malloc((c2-o2+1) * sizeof(int32_t));
  if (tmp) {
   _skptrace("SWAP: tmp<- (%d,%d) [%d]",o2,c2,c2-o2+1);
    memcpy(tmp,ast->par+o2,(c2-o2+1)*sizeof(int32_t));

   _skptrace("SWAP: (%d)<-(%d,%d) [%d]",o2,o1,c1,c1-o1+1);
    memmove(ast->par+o2, ast->par+o1,(c1-o1+1)*sizeof(int32_t));
    
   _skptrace("SWAP: (%d)<-tmp [%d]",o2+(c1-o1+1),c2-o2+1);
    memcpy(ast->par+o2+(c1-o1+1),tmp,(c2-o2+1)*sizeof(int32_t));

    free(tmp);
   _skptrace("SWP: done");
  }
}

void ast_lift(ast_t ast)
{
  // If there's only one child lift it up
  //    ,-------- o1
  //   /     ,--- c1
  //  ((BBBB))
  //    \   '---- c2
  //     '------- o2   

  int32_t o1, c1, o2, c2;

  if (ast->fail || ast->par_cnt < 4) return;
  
  c1 = ast->par_cnt-1;
  if (c1<0 || ast->par[c1] >= 0) return;

  c2 = c1-1;
  if (c2<0 || ast->par[c2] >= 0) return;

  o1 = c1+ast->par[c1];
  if (o1<0 || ast->par[o1] < 0) return;

  o2 = c2+ast->par[c2];
  if (o2<0 || ast->par[o2] < 0) return;

  if (o2 != o1+1) return; // More than one child

  memmove(ast->par+o1,ast->par+o2,(c2-o2+1)*sizeof(int32_t));
  ast->par_cnt -= 2;
}

void ast_noleaf(ast_t ast)
{
  // If the last node is a leaf, remove it!
  //    ,------- o1
  //   /    ,--- c1
  //  ( ... )

  int32_t o1, c1;

  if (ast->fail || ast->par_cnt < 2) return;
  
  c1 = ast->par_cnt-1;
  if (c1<0 || ast->par[c1] >= 0) return;

  o1 = c1+ast->par[c1];
  if (o1<0 || ast->par[o1] < 0) return;

  if (c1 == o1+1) ast->par_cnt-=2;

}

/*******************************************************************************/
int ast_is(ast_t ast, int32_t node, char*rulename)
{
  ast_node_t *nd;
  if (!ast || node >= ast->par_cnt) return 0;

  if (ast->par[node]<0) node += ast->par[node];

  nd = &ast->nodes[ast->par[node]];
 _skptrace("IS: '%s' '%s'",nd->rule,rulename);
  if (rulename == skp_N_STR) {
    if (skp_N_STR1 <= nd->rule && nd->rule <= skp_N_STR7) {
      int ret = ((int)(nd->rule - skp_N_STR1) & 0X07);
      ret += (ret==0);
      return ret;
    }
    return 0;
  }
  return nd->rule == rulename;
}

char *asterror(ast_t ast)
{
  if (!ast || ast->err_pos <0) return NULL;
  return ast->start+ast->err_pos;
}

char *asterrorrule(ast_t ast)
{
  if (!ast || ast->err_pos <0) return NULL;
  return ast->err_rule;
}

char *astnoderule(ast_t ast, int32_t node)
{
  if (!ast || node >= ast->par_cnt || node < 0) return NULL;
  if (ast->par[node]<0) node += ast->par[node];
  return (ast->nodes[ast->par[node]].rule);
}

char *astnodefrom(ast_t ast, int32_t node)
{
  if (!ast || node >= ast->par_cnt || node < 0) return NULL;
  if (ast->par[node]<0) node += ast->par[node];
  if (ast->nodes[ast->par[node]].rule == skp_N_INFO) return skp_N_INFO+1;
  return (ast->start+ast->nodes[ast->par[node]].from);
}

char *astnodeto(ast_t ast, int32_t node)
{
  if (!ast || node >= ast->par_cnt || node < 0) return NULL;
  if (ast->par[node]<0) node += ast->par[node];
  if (ast->nodes[ast->par[node]].rule == skp_N_INFO) return skp_N_INFO+1;
  return (ast->start+ast->nodes[ast->par[node]].to);
}

int32_t astnodelen(ast_t ast, int32_t node)
{
  if (!ast || node >= ast->par_cnt || node < 0) return 0;
  if (ast->par[node]<0) node += ast->par[node];
  if (ast->nodes[ast->par[node]].rule == skp_N_INFO) return 0;
  return (ast->nodes[ast->par[node]].to - ast->nodes[ast->par[node]].from);
}

int astisleaf(ast_t ast, int32_t node)
{
  if (!ast || node >= ast->par_cnt || node < 0) return 0;
  if (ast->par[node]<0) node += ast->par[node];
  return ((ast->nodes[ast->par[node]].delta) == 1);
}

int32_t astnextdf(ast_t ast, int32_t ndx)
{
  ndx++;
  if (ndx < 0) return 0;
  if (ndx >= ast->par_cnt) return ASTNULL;
  return ndx;
}

int astisnodeentry(ast_t ast, int32_t ndx)
{
  if (ndx < ast->par_cnt && 0 <= ndx && ast->par[ndx] >= 0) return 1;
  return 0;
}

int astisnodeexit(ast_t ast, int32_t ndx)
{
  if (ndx < ast->par_cnt && 0 <= ndx && ast->par[ndx] < 0) return 1;
  return 0;
}


#define SKP_STARTNODES 8
ast_t ast_new()
{
  ast_t ast = NULL;
  ast = malloc(sizeof(struct ast_s));
  if (!ast) return NULL;

  ast->nodes_cnt = 0;
  ast->nodes_max = SKP_STARTNODES;
  ast->nodes = malloc(ast->nodes_max * sizeof(struct ast_node_s));
  if (!ast->nodes) { free(ast);
                     return NULL; }

  ast->par_cnt = 0;
  ast->par_max = SKP_STARTNODES * 2;
  ast->par = malloc(ast->par_max * sizeof(int32_t));
  if (!ast->par) { free(ast->nodes) ;
                   free(ast); 
                   return NULL; }

  ast->mmz_cnt = 0;
  ast->mmz_max = 64;
  ast->mmz = malloc(ast->mmz_max * sizeof(ast_mmz_t **));
  if (!ast->mmz) { free(ast->par);
                   free(ast->nodes);
                   free(ast);
                   return NULL; }
  return ast;
}

ast_t astfree(ast_t ast) 
{
  if (ast) {
    skp_mmz_clean(ast);
    free(ast->mmz);  
    free(ast->nodes);
    free(ast->par);  
    free(ast);
  }
  return NULL;
}

static int skp_par_makeroom(ast_t ast,int32_t needed)
{
  int32_t *new_par;
  int32_t new_max;
  if (ast->par_cnt+needed > ast->par_max) {
    new_max = ast->par_max;
    do {
      new_max += new_max/2;
      new_max += new_max &1;
    } while (ast->par_cnt+needed > new_max);

   _skptrace("NEW_PAR_MAX: %d * %d",new_max,(int)sizeof(int32_t));
    new_par = realloc(ast->par,new_max * sizeof(int32_t));
    assert(new_par);
    if (!new_par) {errno = ENOMEM; return 0; }
    ast->par = new_par;
    ast->par_max = new_max;
  }
  return 1;
}

int32_t ast_nextpar(ast_t ast)
{
  if (!skp_par_makeroom(ast,1)) return -1;
  return ast->par_cnt++;
}

static int skp_nodes_makeroom(ast_t ast,int32_t needed)
{
  int32_t new_max;
  ast_node_t *new_nodes;
  if (ast->nodes_cnt + needed > ast->nodes_max) {
    new_max = ast->nodes_max;
    do {
      new_max += new_max/2;
      new_max += new_max & 1;
    } while (ast->nodes_cnt + needed > new_max);
   _skptrace("NEW_NODE_MAX: %d * %d",new_max,(int)sizeof(struct ast_node_s));
    new_nodes = realloc(ast->nodes,new_max * sizeof(struct ast_node_s));
    assert(new_nodes);
    if (!new_nodes) {errno = ENOMEM; return 0;}
    ast->nodes = new_nodes;
    ast->nodes_max = new_max;
  }
  return 1;
}

int32_t ast_nextnode(ast_t ast)
{
  if (!skp_nodes_makeroom(ast,1)) return -1;
  return ast->nodes_cnt++; 
}

int32_t ast_open(ast_t ast, int32_t from, char *rulename)
{
  int32_t par;
  int32_t node; 

  if (ast->fail) return -1;
  if ((par = ast_nextpar(ast)) < 0) return -1;
  if ((node = ast_nextnode(ast)) < 0) return -1;
  ast->par[par] = node;
  ast->nodes[node] = (ast_node_t){rulename,from,0,0};
  return par;
}

int32_t ast_close(ast_t ast, int32_t to, int32_t open)
{
  ast_node_t *nd;
  int32_t par;
  
  if (open <0) return -1;

  nd = &ast->nodes[ast->par[open]];

  if (ast->fail) {
    ast->pos = nd->from;
    ast->nodes_cnt = ast->par[open];
    ast->par_cnt = open;
    return -1;
  }

  if ((par = ast_nextpar(ast)) < 0) return -1;
  nd->to = to;
  nd->delta = par-open;
  ast->par[par] = -(nd->delta); // Close parenthesis
  return par;
}

void ast_setinfo(ast_t ast, int32_t info)
{
  int32_t par;
  if (!ast->fail) {
    par = ast_open(ast,info,skp_N_INFO);
    ast_close(ast,info,par);
    ast->last_info = info;
  }
}

int32_t astnodeinfo(ast_t ast, int32_t node)
{
  if (!ast || node >= ast->par_cnt || node < 0) return 0;
  if (ast->par[node]<0) node += ast->par[node];
  if (ast->nodes[ast->par[node]].rule != skp_N_INFO) return 0;
  return ast->nodes[ast->par[node]].from;
}

void astprint(ast_t ast, FILE *f)
{
  int32_t node = ASTNULL;
  while ((node = astnextdf(ast,node)) != ASTNULL) {
    if (astisnodeentry(ast,node)) {
      fprintf(f,"(%s ",astnoderule(ast,node));
      if (astisleaf(ast,node)) {
        fputc('\'',f);
        if (astnoderule(ast,node) == skp_N_INFO) {
          fprintf(f,"%d",astnodeinfo(ast,node));
        }
        else for (char *s = astnodefrom(ast,node); s < astnodeto(ast,node); s++) {
          if (*s == '\'') fputc('\\',f);
          fputc(*s,f);
        }
        fputc('\'',f);
      }
    }
    else fputc(')',f);
  }
}


#endif // SKP_MAIN

#endif // SKP_VER
