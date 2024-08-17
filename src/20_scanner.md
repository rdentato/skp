# SKP - scanners 

## Table of Contents
- [SKP - scanners](#skp---scanners)
  - [Table of Contents](#table-of-contents)
  - [Introduction](#introduction)
  - [skpswitch()](#skpswitch)
  - [skpscan()](#skpscan)
  - [Implementation](#implementation)
  - [Scanner](#scanner)
  - [Save/Restore](#saverestore)

## Introduction
  A pretty common task is to match a given text with a sequence of 
patterns. While this could easily achieved using `skp()` within
`if` and `while` statments, `skp` offers two *instructions* to ease
the task:

- `skpswitch(text) { ... }` which matches a text against a set of
  patterns.

- `skpscan(text) { ... }` which does the same repeatedly until the
  entire string is scanned or no patterns matches.

  Within the body of `skpswitch` and `skpscan`, you can use the
following:

- `skpcase(pattern) { ... }`  defines the code to be executed if
  the pattern is matched. Note that `skpcase(NULL)` will *always*
  match and can be used to execute code if none of the patterns
  matched. Should be the last pattern (as it will prevent any 
  other pattern to be executed.)
  Also, note that `pattern` can be a string or a function with 
  prototype:  `int f(char *text, char** start, char **end)`

- `skpfrom` a `char *` variable that points at the first character
  of the matched text

- `skpto` a `char *` variable that points at the first character
  past the matched text

- `skp_info.alt` the return value of the the equivalent `skp()` function
  

## skpswitch()
  This *pseudo-instruction* will 

    char *text = "pippo pluto";
    int len;
    skpswitch(text) {
      skpcase("I") { len = skpto - skpfrom; printf("Identifier: %.*s\n",len,skpfrom); }
      skpcase("D") { len = skpto - skpfrom; printf("Number: %.*s\n",len,skpfrom); }
    }

## skpscan()

  It takes a string and tries to match it with a sequence of patterns.
If one of the patterns matches, the associated block of code is executed and the loop continues 
until all the string has been scanned.

  This is very similar to the the way the `AWK` programming language operates. 

For example, a simple CSV reader can be implemented like this:

``` C
   skpscan(csv,last) {
     skpcase("',' *w") { num_col++; }  // Comma followed by spaces
     skpcase("D")      { store_number(*skpfrom, *skpto, num_row, num_col);}
     skpcase("Q")      { store_string(*skpfrom, *skpto, num_row, num_col);}
     skpcase("*!','")  { store_string(*skpfrom, *skpto, num_row, num_col);}
     skpcase("n")      { num_row++; num_col = 0; } // Newline
     skpcase("+w")     { } // Skip spaces
   }
   if (*last != '\0') { 
     // abnormal exit
   }
```


## Implementation

```C

@("after:Public API")

#define skp_scan_loop(s,t,r) \
     for (skp_info_t skp_info = {.start = s, .alt =0, .matched = 0}; \
          (skp_info.start && *skp_info.start && !skp_info.alt ); \
          skp_info.alt = skp_info.alt? (((t)[0] = skp_info.start = skp_info.to[0]), r):1)

#define skpscan(...)    skp_vrg(skpscan_,__VA_ARGS__)
#define skpscan_1(s)    skpscan_2(s, skpto)
#define skpscan_2(s,t)  skp_scan_loop(s,t,0)

#define skpswitch(...)   skp_vrg(skpswitch_,__VA_ARGS__)
#define skpswitch_1(s)   skipswitch_2(s, skpto)
#define skpswitch_2(s,t) skp_scan_loop(s,t,1)

#define skpgroup(...)    skp_vrg(skpgroup_,__VA_ARGS__)
#define skpgroup_1(s)    skpgroup_3(s,skpto, &skpmatched)
#define skpgroup_2(s, t) skpgroup_3(s, t, &skpmatched)
#define skpgroup_3(s, t, r) for (char *skpstart = NULL; (skpstart == NULL) && (skpstart = s);  ) \
                              for (int skpmatched = -1; (skpmatched < 0) && (skpmatched = 1); skpmatched? (((t)[0] = skpto[0]) , ((r)[0] = skpmatched)) : 0)


```

  The variables needed to control the loop are in a single variable of type `skp_info_t`
but for ease of access, there are macros to make them look like regular variables.

```C
@("before:Public API")
typedef struct {
  char  *start;
  char  *from[SKP_MAXPATTERNS];
  char  *to[SKP_MAXPATTERNS];
  int    alt;
  int    matched;
} skp_info_t;


#define skplen(n) skp_len(n,skpfrom, skpto)
static inline int skp_len(int n, char **from, char **to) { return to[n] - from[n];}

```

```C
@("after:Public API")

#define skpcase(...) \
  for(char **skpfrom = skp_info.from, **skpto = skp_info.to, *skpstart = skp_info.start; \
      skpstart != NULL; skpstart = NULL) \
    if (skp_info.alt || !((skp_info.alt = skp(skp_info.start, __VA_ARGS__ ,skpfrom,skpto)))) ;\
    else

```
## Scanner

```C
@("after:Public API")
#define skpscanner skprecognizer
```

## Save/Restore
```C
@("after:Global types")
typedef struct {
  char *start;
  char *from;
  char *to;
  int alt;
} skp_save_t;

//extern skp_save_t *skp_save;

@("after:Global variables")
//skp_save_t *skp_save = &((skp_save_t){.matched = 0});

@("after:Public API")
#define skp_save() {skpstart, *skpfrom, *skpto, (skpmatched = 1) }

#define skp_restore() ((skpstart = skp_save.start, \
                       *skpfrom = skp_save.from, \
                         *skpto = skp_save.to), \
                                  0)

#define skpsetmatchstart() ((*skpfrom = skp_save.start),\
                                        0)

#define skp_do skp_save.alt

```

```C
@("after:Public API")

#define ifskp(...) \
  for ( int skp_count = 1;\
        skp_count && skpmatched && \
        (skpmatched = skp(skpstart, __VA_ARGS__, skpfrom, skpto));\
        skpstart = *skpto, \
        skp_count = 0 ) 

#define elseifskp(...) \
  if (skpmatched || !(skpmatched = 1)) ; \
  else ifskp(__VA_ARGS__)

#define elseskp \
  if (skpmatched || !(skpmatched = 1)) ; else 

#define whileskp(...) \
  if (!skpmatched) ; else \
    for ( int skp_count = 1;\
          \
          skp_count \
          && ((skpmatched = skp(skpstart, __VA_ARGS__, skpfrom, skpto)) \
              || ((skpmatched = (skp_count>1)) && (skp_count = 0)));\
          \
          skpstart = *skpto, \
          skp_count++ ) 
/*
//#define skp_1(p) \
//  for ( int skp_count = 1;\
//        skp_count && skpmatched && \
//        (skpmatched = skp_(skpstart, p, skpfrom, skpto));\
//        skpstart = *skpto, \
//        skp_count = 0 ) 
*/

#define mustskp \
  if (!skpmatched) ; else for (skp_save_t skp_save = skp_save(); \
       skpmatched && skp_do; \
       skp_do = skpmatched ? skpsetmatchstart() : skp_restore())

#define notskp \
  if (!skpmatched) ; else for (skp_save_t skp_save = skp_save(); \
       skpmatched && skp_do; \
       skp_do = skp_restore(), skpmatched = !skpmatched)

#define peekskp \
  if (!skpmatched) ; else for (skp_save_t skp_save = skp_save(); \
       skpmatched && skp_do; \
       skp_do = skp_restore())

#define mayskp \
  if (!skpmatched) ; else for (skpmatched = 0; \
       (skpmatched == 0) && (skpmatched = 1) ; \
       skpmatched = 1) mustskp

#define orskp  if (skpmatched || !(skpmatched = 1)) ; else mustskp
#define andskp if (!skpmatched) ; else



```

