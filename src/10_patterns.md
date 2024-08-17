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
extern char *skpfrom[SKP_MAXPATTERNS]; // Write only!
extern char *skpto[SKP_MAXPATTERNS]; // Write only!
//extern char *skpstart;
//extern int   skpmatched;

@("after:Global Variables")
char *skpfrom[SKP_MAXPATTERNS]; // Write only!
char *skpto[SKP_MAXPATTERNS]; // Write only!
// char *skpstart = NULL;
// int   skpmatched = 0;

@("after: Global Declarations")
typedef int (*skp_funcpattern_t)(char *, char**, char **, int);

#define skpdef(recog) \
   int recog(char *skpstart, char **skpfrom, char **skpto, int skpmatched)

@("after:Public API")

int skp_f(char *txt, skp_funcpattern_t f, char **from,char **end);
int skp_s(char *txt, char *pat, char **from,char **end);
int skp_n(char *txt, int n, char **from,char **end);
int skp_p(char *txt, void *p, char **from,char **end);

#define skp(...) skp_vrg(skp_,__VA_ARGS__)
#define skp_2(s,p)     skp_(s, p, skpfrom, skpto)
#define skp_3(s,p,t)   skp_(s, p, skpfrom, t    )
#define skp_4(s,p,f,t) skp_(s, p, f,       t    )

#define skp_(s,p,f,t) _Generic((p),\
                     char *: skp_s,\
          skp_funcpattern_t: skp_f,\
                        int: skp_n,\
                     void *: skp_p ) (s,p,f,t)

```

```C
@("after:The skp function")
int skp_f(char *txt, skp_funcpattern_t f, char **from,char **to)
{
  char *local_from[SKP_MAXPATTERNS];
  char *local_to[SKP_MAXPATTERNS];
  int ret = 0;
  local_from[0] = txt;
  local_to[0] = txt;
 _skptrace("skp_t: %s",txt);
  if (txt != NULL && f != NULL) {
    ret = f(txt,local_from,local_to,1);
    if (ret) {   // Only from[0] and to[0] are updated
      if (from) *from = local_from[0];
      if (to) *to = local_to[0];
    }
  }
  return (ret & '\7'); // ensure return value is <= 7
}

int skp_n(char *txt, int n, char **from,char **to)
{
  char *local_to = txt;
  int iso=0;
  int ret = 0;

  if (n<0) { n = -n; iso = 1; }

  while (n && *local_to) {
    n--;
    skp_next(local_to,&local_to,iso);
   _skptrace("N: %d %s %p %p",n,local_to,local_to,to);
  }

  if (n <= 0) {
    if (from) *from = txt;
    if (to) *to = local_to;
    ret = 1;
  }

  return ret;
}

int skp_p(char *txt, void *p, char **from,char **to)
{
  if (from) *from = txt;
  if (to) *to = txt;
  return (1);
}
```

```C
@("after:The skp function")
int skp_s(char *txt, char *pat, char **from,char **to)
{
  char *start = txt;
  char *s; char *p;
  char *s_end=NULL; 
  char *p_end=NULL;
  int   skp_to = 0;
  int   matched = 1;
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

  return (ret);
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

    if (from) *from = start;
    if (to) *to  = s;
  }
  else {
    ret = 0;
    // *from = txt;
    // *to  = txt;
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

@("after:Global types")
typedef struct {int x;} *skp_goal_t;
typedef struct {int y;} *skp_goalnot_t;


@("before:Global Declarations")
extern skp_goal_t    skpgoal;
extern skp_goalnot_t skpnot;

#define SKP_MAXPATTERNS 6

@("after:Global Variables")
skp_goal_t    skpgoal;
skp_goalnot_t skpnot;

@("after:Public API")
#define skp_type(x) _Generic((x),char *: 'S', \
                      skp_funcpattern_t: 'F', \
                             skp_goal_t: 'G', \
                          skp_goalnot_t: '!', \
                                 void *: 'P', \
                                   int : 'N', \
                                default: '\0')

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

#define skp_9(txt,p1,p2,p3,p4,p5,p6,f,t) \
    skp_multi(txt,f,t, (char []){skp_type(p1),skp_type(p2),skp_type(p3),skp_type(p4),skp_type(p5),skp_type(p6),'\0'},\
                (skp_voidptr []){(void *)p1, (void *)p2, (void *)p3, (void *)p4, (void *)p5, (void *)p6})

@("after:The skp_multi function")
int skp_multi(char *text,char **from, char **to, char *types, void **ptrns)
{
 _skptrace("MULTI TYPES: %s",types);
  char *tmp_from[SKP_MAXPATTERNS];
  char *tmp_to[SKP_MAXPATTERNS];
  char *goal = NULL;
  char *goalnot = NULL;

  int n = 0;
  int ret = 0;
  while(n < SKP_MAXPATTERNS && types[n]) {
   _skptrace("Multi text: %s",text);
    switch (types[n]) {
      case 'S' : ret = skp_s(text,(char *)(ptrns[n]), tmp_from+n, tmp_to+n);
                _skptrace("S: %d %d %p %p %p %p",n,ret,tmp_from+n,tmp_to+n,tmp_from[n],tmp_to[n]);
                 break;
      case 'F' : ret = skp_f(text, (skp_funcpattern_t)(ptrns[n]), tmp_from+n, tmp_to+n);
                _skptrace("F: %d %d %p %p %p %p",n,ret,tmp_from+n,tmp_to+n,tmp_from[n],tmp_to[n]);
                 break;
      case 'P' : ret = skp_p(text, ptrns[n], tmp_from+n, tmp_to+n);
                _skptrace("P: %d %d %p %p %p %p",n,ret,tmp_from+n,tmp_to+n,tmp_from[n],tmp_to[n]);
                 break;
      case 'N' : ret = skp_n(text, (int)((intptr_t)(ptrns[n])), tmp_from+n, tmp_to+n);
                _skptrace("N: %d %d %p %p %p %p",n,ret,tmp_from+n,tmp_to+n,tmp_from[n],tmp_to[n]);
                 break;
      case 'G' : if (goal || goalnot) return 0;
                 ret = 1; goal = text;
                 tmp_from[n] = tmp_to[n] = goal;
                 break;
      case '!' : if (goal || goalnot) return 0;
                 ret = 1; goalnot = text;
                 tmp_from[n] = tmp_to[n] = goalnot;
                 break;
      default : return 0;
    }

    if (ret == 0) break;

    text = tmp_to[n];
    n++;
  }

  if (goalnot) {
    ret = !ret;
    goal = goalnot;
  }

  if (ret) {
    if (from) {
      from[0] = tmp_from[0];
      for (int k = n; k>0; k--)
        from[k] = tmp_from[k-1];
    }
    if (to) {
      to[0]   = goal? goal : tmp_to[n-1];
      for (int k = n; k>0; k--)
        to[k] = tmp_to[k-1];
    }
  }

  return (ret);
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

@("after:Adjust values for matched recognizer")
if (matched == MATCHED_GOAL && !goalnot && !goal) {
  goal = s_end;  skptrace("GOAL: %.4s",s_end);
}
else if (matched == MATCHED_GOALNOT && !goalnot && !goal) { 
  goalnot = s_end; skptrace("!GOAL: %.4s",s_end);
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

