# SKP - scanners 

## Table of Contents
- [SKP - scanners](#skp---scanners)
  - [Table of Contents](#table-of-contents)
  - [Introduction](#introduction)
  - [skpswitch()](#skpswitch)
  - [skpscan()](#skpscan)
  - [Implementation](#implementation)

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
   skpscan(csv) {
     skpcase("',' *s") { num_col++; }  // Comma followed by spaces
     skpcase("D")      { store_number(skpfrom, skpto, num_row, num_col);}
     skpcase("Q")      { store_string(skpfrom, skpto, num_row, num_col);}
     skpcase("*!','")  { store_string(skpfrom, skpto, num_row, num_col);}
     skpcase("n")      { num_row++; num_col = 0; } // Newline
     skpcase("+s")     { } // Skip spaces
     skpcase(NULL)     { prterror("Unexpected text at %d,%d\n",num_row, num_col); 
                         break;
                       }
     last = skpto; // Store position
   }
   if (*last != '\0') { 
     // abnormal exit
   }
```


## Implementation

```C

@("after:Public API")

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
