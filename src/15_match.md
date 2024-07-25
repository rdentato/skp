# SKP - The `match()` function

## Table of Contents

- [SKP - The `match()` function](#skp---the-match-function)
  - [Table of Contents](#table-of-contents)
  - [Skipping characters.](#skipping-characters)
    - [The CRLF marker](#the-crlf-marker)


The `match()` function is the inner core of the `skp()` function.

```C
@("before:The skp function")
// skp() needs to know about mathc()
static int match(char *pat, char *txt, char **pat_end, char **txt_end,int *flg);
```

  It takes a pattern and a string to match and returns a non-zero value if the 
first recognizer in the pattern, matches the beginning of the string.

  The return value can be one of these:

```C
@("after:Constants for match")
#define MATCHED_FAIL    0 
#define MATCHED         1
#define MATCHED_GOAL    2
#define MATCHED_GOALNOT 3
```

Read the section [Handling matched recognizers](#handling-matched-recognizers) for more details on `MATCHED_GOAL` and `MATCHED_GOALNOT`

 If it matches, it also set the variable `pat_end` at the end of the recognizer
and the `txt_end` at the end of the text that matched.

  The parameter `flg` is used internally to control the behavior of the 
match algorithm. It is a bit mask:

      76543210
      xxxxxxxx
            ^^
             \\__ Comparison: 0: Case Sensitive
              \__ Encoding:   0: UTF-8  1: ISO-8159-15

  It is passed as a pointer since the recognzers `C` and `U` can alter its value.

```C
@("after:The match function")
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

```

```C
@("after:Auxiliary functions for match")
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

```

## Skipping characters.
  By default `skp` assums text is UTF-8 encoded. 

  The function `uint32_t skpnext(char *txt [, char **end])` will return
the UTF-8 encoding of the first codepoint in the `txt` string.

  If your file is iso-8859-x encoded (or any other 8-bit encoding), you can
use the `uint32_t skpnextISO(char *txt [, char **end])` function.

  If the `end` parameters it specified, it will contain a pointer to the 
first charater past the returned codepoint.

  For example, you can scan a string searching for string "易經"
  
    uint32_t chr;
    char *needle = "易經";
    int found = 0;
    int len = strlen(needle);
    do {
      found = ((strncmp(needle, haystack,len) == 0));
    } while (!found && (chr = skpnext(haystack,&haystack)))

  Of course the code below wuould have been much simpler :) :

         found = ((haystack = strstr(haystack, needle)) != NULL)
  
  but the point was to show how to move une Unicode codepoint at the time.

```C
@("after:Public API")

uint32_t skp_next(char *text,char **end,int iso);

#define skpnext(...)    skp_vrg(skpnext_,__VA_ARGS__)
#define skpnextISO(...) skp_vrg(skpnextISO_,__VA_ARGS__)

#define skpnext_1(s)   skp_next(s,NULL,0)
#define skpnext_2(s,e) skp_next(s,e,0)

#define skpnextISO_1(s)   skp_next(s,NULL,1)
#define skpnextISO_2(s,e) skp_next(s,e,1)

@("after:Functions")

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
    @(":Fix the CRLF End Of Line marker")
  }

 _skptrace("next: %08X iso: %d",c,iso);
  if (end) *end = (char *)s;
  return c;
}
```
### The CRLF marker
  To deal with the `CRLF` end of line used by Windows and some
network protocol, I decided to report the sequence `\r\n` as
a single encoding `0x0D0A` which is not a valid UTF-8 encoding
and can't be mistaken for anything else. This is still to be 
tested to be a good idea or not as there might be use cases
where we should not consider these two character together.
  But I still have not found any!!

```C
    @("after:Fix the Windows EOL")
    if (c == 0x0D && *s == 0x0A) {
      c = 0x0D0A; s++;
    }
```
