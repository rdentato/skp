# SKP - Patterns

## Table of Contents
- [SKP - Patterns](#skp---patterns)
  - [Table of Contents](#table-of-contents)
  - [Introduction](#introduction)
  - [The `skp()` function](#the-skp-function)
    - [Handling non-matched recognizers](#handling-non-matched-recognizers)
    - [Handling matched recognizers](#handling-matched-recognizers)
    - [The '\>' modifier](#the--modifier)

## Introduction
  This is the basic functionality of `skp`: matching a string against a pattern.

```C
@("after:Functions")
  @(":skp auxiliary functions")
  @(":The skp function")
```

  In the following we use the following terminology:
- *recognizer* : a single element of a pattern. For example `d` is a *recognizer
  for a digit, `'xy'` is the recgnizer for the string `xy`.
- *pattern* : a sequence of recognizer.
- *sub-pattern* : a sequence of recognizer within a pattern that ends with
  a character with ASCII <= 7.
- *matching text* : the portion of text that matches a certain pattern.

## The `skp()` function

  The `skp()` function is a variadic function. Some of its parameters
can be omitted:

- `skp(string,pattern)` : returns 0 if the string doesnot match the pattern (see the [sub-patterns](#Sub_patterns) sections for more details)
- `skp(string,pattern,end)` : as the previous one but also set `end` (a `char **`) right after the matching substring
- `skp(string,patterm,start,end)` : as the previous but also sets `start` to the starting of the text that matches
  this is used in combination with the `>` modifiers for patterns.

 The complexity of this function is due to two factors:

 - **Handling subpatterns**.
   A pattern may contain sub patterns separated by characters with ASCII code lower that 8. For example
   the pattern `"'ab'\2 'cd'\3'"` has two *subpatterns* (`'ab'` and `'cd'`) if the strings matches `'ab'`,
   `skp()` will return `2`, otherwise the patter `'cd'` is tried, if it matches, `skp()` will return `3`
   otherwise it will return `0` (no match).
   Note that by default the last subpattern will return `1` if not differently specified.

 - **Handling the `>` modifier**.
   If a pattern starts with `>`, it is tested across the entire string, not just at the beginning. Note
   how this is the opposite of what happens with regular expressions where the pattern is always tested
   across the entire string, unless anchored with `^`.
   This means that if we fail to match a pattern (i.e. all the subpatterns in a pattern), we have to 
   move one character ahead in the string and try the entire pattern again.


```C
@("after:Public API")
int skp_(char *src, char *pat, char **from, char **end);

#define skp(...)    skp_vrg(skp_,__VA_ARGS__)
#define skp_4(s,p,f,t)  skp_(s, p, f,t)
#define skp_3(s,p,t)    skp_(s, p, NULL, t)
#define skp_2(s,p)      skp_(s, p, NULL, NULL)
```

```C
@("after:The skp function")
int skp_(char *src, char *pat, char **from,char **end)
{
  char *start = src;
  char *s; char *p;
  char *s_end=NULL; 
  char *p_end=NULL;
  int   skp_to = 0;
  int   matched = 0;
  int   ret = 0;
  char *goal = NULL;
  char *goalnot = NULL;
  int   flg = 0;

  if (!pat || !src) { return 0; }

  @(":Record the presence of the > modifier")

  p = pat;
  s = start;

  // skip over the spaces at the beginning of the pattern (they are useless)
  while (is_space(*p)) p++;

  // Characters below ASCII 0x07, mark the end of a sub pattern (`0x00` marks
  // the end of the entire pattern string.).
  // If the current character in the pattern is greater than `0x07`, it means
  // that we are still in the pattern.

  while (*p > '\7') { // Loop over the pattern string
    
    matched = match(p,s,&p_end,&s_end,&flg); // Match the next recognizer

    if (matched) {
      @(":Adjust values for matched recognizer")
    }
    else {
      @(":Handle a non match in the pattern")
    }
    while (is_space(*p)) p++; // skip useless spaces in the pattern
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

    if (from)  *from  = skp_to?start:s;
    if (end) *end = s;
    return ret;
  }

  if (from)  *from  = src;
  if (end) *end = src;
  return 0;
}
```
### Handling non-matched recognizers

  There can be three cases to handle if the current recognizer in
the pattern didn't match:

```C
  @("after:Handle a non match in the pattern")
     @(":Look for a new subpattern")
     @(":Otherwise, handle the > modifier, if set")
     @(":Otherwise, exit with no match")
```
  We first try to find a subpattern:

```C
     @("after:Look for a new subpattern")
     _skptrace("notmatched  '%s' '%s'",s,p);
      // Skip over the current sub-pattern
      while (*p > '\7') p++;
     _skptrace("notmatched+ '%s' '%s'",s,p);
      if ((*p > '\0') && (p[1] > '\0')) { // Try a new pattern
        s = start;
        p++;
       _skptrace("resume from: %s (%c)", p,*s);
      }
```
  If no subpattern can be tested, check if the `>` modifier was set
and, in case, reset the pattern and advance the start of text by 1:

```C
     @("after:Otherwise, handle the > modifier, if set")
      else if (skp_to) {
        goal = NULL;  goalnot = NULL;
        p = pat;
        s = ++start;
       _skptrace("retry '%s'",s);
        if (*s == '\0') break;
      }

```
  If there is no other subpattern to test and there was
no `>` modifier set in the pattern, we just exit the loop.
The variable `matched` will be 0.

```C
      @("after:Otherwise, exit with no match")
       else break;
```

### Handling matched recognizers

  If a recgnizer has been matched, there are things to handle properly.
The first (and trivial one) is to advance past the recognizer and the matched text:

```C
 @("before:Adjust values for matched recognizer")
_skptrace("matched( '%s' '%s'",s,p);
 s = s_end;
 p = p_end;
_skptrace("matched) '%s' '%s'",s,p);
```
  The other one is to handle the *goal* of the match.
  The pattern `&` sets a *goal* in the matched string. For example, if the pattern
is  `"'ab' &d"`, the string `"ab3"` will match for the portion `ab` because the next
character is a digit.

  You can think of is as a sort of *look-ahead*. Setting a *goal* is a way to say that
a string (like `ab` in the example) matches only if what follws also matches a given
pattern (a digit, in the example above).

  You can specify a *negative* goal (i.e. negative look-ahead) to say that a piece
of text matches a pattern, unless it is followed by another pattern.

  For example, the string `"ab3"` will **not** match the pattern `"'ab' !&d'"` at all
because `"ab"` is followed by a digit.

  If the recognizer we just matched was `&`, the `matched` variable holds
the value `MATCHED_GOAL` and we need to save the current position within the string
since this is where the end of the actual matching text is (the rest is the *look-ahead*).
  The same goes for `!&` in which case the value of `matched` would be `MATCHED_GOALNOT`.

  The two variables `goal` and `goalnot` will contain the saved position.

  Note that only the first `&` or `!&` is considered the others are discarded.

```C
@("after:Adjust values for matched pattern")
if (matched == MATCHED_GOAL && !goalnot && !goal)
  goal = s_end;
else if (matched == MATCHED_GOALNOT && !goalnot && !goal) { 
  goalnot = s_end; /* skptrace("!GOAL: %.4s",s);*/ 
}
```

 ### The '>' modifier

  When a pattern starts with `>` we record it into the `skp_to` variable.
If it's `1` it means that if we fail, we have to try the pattern on the
next character.

```C
@("after:Record the presence of the > modifier")
_skptrace("SKP_: src:'%s' pat:'%s'",src,pat);

  if (*pat == '>') {
    skp_to = 1;
    pat++ ;
  }
_skptrace("SKP_: src:'%s' pat:'%s' skp_to:%d",src,pat,skp_to);

```


```C
  @("after:skp auxiliary functions")
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


static uint32_t skp_next(char *s,char **end,int iso)
{
  uint32_t c = 0;

  if (s && *s) {
    c = *s++;
    if (!iso) {
#if 1
      if ((*s & 0xC0) == 0x80) {
        c = (c << 8) | *s++;
        if ((*s & 0xC0) == 0x80) {
          c = (c << 8) | *s++;
          if ((*s & 0xC0) == 0x80) {
            c = (c << 8) | *s++;
          }
        }
      }
#else
      while ((*s & 0xC0) == 0x80) {
        c = (c << 8) | *s++;
      }
#endif
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
  if (fold && a <= 0x7F && b <= 0x7F) {
    a = tolower(a);
    b = tolower(b);
  }
  return (a == b);
}

static int is_blank(uint32_t c)
{
#if 1  

  if (c < 0xFF) return  (c == 0x20) || (c == 0x09);

  switch ( c & 0xFFFFFF00) {
    case 0x00000000 : return c == 0xA0; 
    case 0x0000C200 : return c == 0xC2A0;

    case 0x00E19A00 : return c == 0xE19A80;

    case 0x00E28000 : return ((0xE28080 <= c) && (c <= 0xE2808A))
                             || (c == 0xE280AF);

    case 0x00E38080 : return c == 0xE38080;
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
#if 1
  if (c < 0x0F) return  (c == 0x0A)
                     || (c == 0x0C)
                     || (c == 0x0D);

  if (c < 0xFF) return (c == 0x85);

  return (c == 0x0D0A)    // CRLF (not a real UTF-8 CODEPOINT!!!)
      || (c == 0xC285)    // U+0085 NEL next line         
      || (c == 0xE280A8)  // U+2028 LS line separator     
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

#define MATCHED_FAIL    0 
#define MATCHED         1
#define MATCHED_GOAL    2
#define MATCHED_GOALNOT 3

static int match(char *pat, char *src, char **pat_end, char **src_end,int *flg)
{
  uint32_t s_chr;
  char *p_end, *s_end;
  int ret = MATCHED_FAIL;
  uint32_t match_min = 1;
  uint32_t match_max = 1;
  uint32_t match_cnt = 0;
  uint32_t match_not = 0;
  int intnumber  = false;
  char *s_tmp = src;
  
  s_end = src;
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

    #define get_next_s_chr() do {s_end = s_tmp; s_chr = *s_end ; s_tmp++;} while(0)
    
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
                 break;

      case 'S' : while (is_space(s_chr)) get_next_s_chr();;
                 ret = MATCHED;
                 break;

      case 'W' : while (is_blank(s_chr)) get_next_s_chr();;
                 ret = MATCHED;
                 break;

      case 'N' : // Up to end of line
                 while(s_chr && !is_break(s_chr)) {
                    get_next_s_chr();
                 }
                 if (s_chr) get_next_s_chr();
                 ret = MATCHED;
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
                 if (s_chr == '+' || s_chr == '-') { // sign
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

  if (ret != MATCHED_FAIL) {
    if (pat_end) *pat_end = p_end;
    if (src_end) *src_end = s_end;
  }
  return ret;
}


```