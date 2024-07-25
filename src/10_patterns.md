# SKP - Patterns

## Table of Contents

- [SKP - Patterns](#skp---patterns)
  - [Table of Contents](#table-of-contents)
  - [Introduction](#introduction)
  - [The `skp()` function](#the-skp-function)
    - [Skip a sequence of patterns](#skip-a-sequence-of-patterns)
    - [Handling non-matched recognizers](#handling-non-matched-recognizers)
    - [Handling matched recognizers](#handling-matched-recognizers)
    - [The '\>' modifier](#the--modifier)
  - [The `match()` function](#the-match-function)
  - [Skipping characters.](#skipping-characters)
    - [The CRLF marker](#the-crlf-marker)

## Introduction
  This is the basic functionality of `skp`: matching a string against a pattern.

```C
@("after:Functions")
  @(":Constants for match")
  @(":Auxiliary functions for match")
  @(":The skp function")
  @(":The skp_multi function")
  @(":The match function")

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
@("after:Global Declarations")
extern char *skp_writeonly_charptr;

@("after:Global Variables")
char *skp_writeonly_charptr; // Write only!

@("before: Public API")
typedef int (*skp_funcpattern_t)(char *, char**, char **);

#define skprecognizer(recog) \
   int recog(char *skpstart, char **skpfrom, char **skpto)

@("after:Public API")

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
```


```C
@("after:The skp function")
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
```

```C
@("after:The skp function")
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

  @(":skp local variables")

  if (!pat || !txt) { return 0; }

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
    
    // Match the next recognizer
    matched = match(p,s,&p_end,&s_end,&flg); 

    if (matched) {
      @(":Adjust values for matched recognizer")
    }
    else {
      @(":Handle a non match in the pattern")
    }
    while (is_space(*p)) p++; // skip useless spaces in the pattern
  }

 _skptrace("pat: '%s'",p);

  @(":Fix the goal")
  @(":Fix return values")

  return ret;
}
```
  If a goal ha been set, the end of the matching string is brought
back to the goal.

  The test `!matched && goalnot` checks if a *negative* goal was set
(i.e. we wanted the rest of the string *not* to match a certain pattern)
and we actually failed to match it. In this case, it means that we
do actually matched the text before the goal.

```C
@("after:Fix the goal")
  if (!matched && goalnot) {
    goal = goalnot;
    matched = MATCHED;
    // reach the end of the subpattern to ensure a proper return value.
    while (*p > '\7') p++;
  }

  if (goal) s = goal; // Move back the end of the string to the goal
```

  We need to set the return value to the subpattern that matched (or
to `0` if there was no match).
  We also have to set the limits of the text that matched (`from` and `end`).

```C
@("after:Fix return values")
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

```

### Skip a sequence of patterns

Actually, `skp()` is capable of matching a sequence of patterns that can either be strings or functions:

    char *from[3];
    char *to[3];
    if (skp(text, "S '(' S", keyword, "S ')' S", from, to )) {
      printf("found keyword: %.*s,to[2]-from[2],from[2]);
    }

In this form, the parameters `from` and `to` are mandatory and they must be an array of `char *`
with at least as many elements as the number of patterns plus one.

The pair `from[0]` and `to[0]` will return the boundaries of the entire match, the 
pairs `from[1]` and `to[1]` the boundaries of the match for the first pattern and so on.


```C
@("before:Global Declarations")
#define SKP_MAXPATTERNS 5

@("after:Public API")
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


@("after:The skp_multi function")
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
        skp_next(start,&start,flg & 2);
        s = start;
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

  If the recognizer we just matched was `&` then the `matched` variable holds
the value `MATCHED_GOAL` and we need to save the current position within the string
since this is where the end of the actual matching text is (the rest is the *look-ahead*).
  The same goes for `!&` in which case the value of `matched` would be `MATCHED_GOALNOT`.

  The two variables `goal` and `goalnot` will contain the saved position.

  Note that only the first `&` or `!&` is considered the others are discarded.

```C
@("after:skp local variables")
  char *goal = NULL;
  char *goalnot = NULL;

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
_skptrace("SKP_: txt:'%s' pat:'%s'",txt,pat);

  if (*pat == '>') {
    skp_to = 1;
    pat++ ;
  }
_skptrace("SKP_: txt:'%s' pat:'%s' skp_to:%d",txt,pat,skp_to);

```

## The `match()` function

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
