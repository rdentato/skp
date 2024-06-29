# SKP - Header file

## Table of Contents
- [SKP - Header file](#skp---header-file)
  - [Table of Contents](#table-of-contents)
  - [The Header file](#the-header-file)
  - [General use Macros](#general-use-macros)
    - [Variable arguments macro](#variable-arguments-macro)
    - [Tracing and testing macros](#tracing-and-testing-macros)
  - [Standard includes](#standard-includes)
  - [Global variables](#global-variables)

## The Header file

```C

#ifndef SKP_VERSION
#define SKP_VERSION 0x0004001C
#define SKP_VERSION_STR "0.4.0"

  @(":Standard Includes")
  @(":Utility macros")
  @(":Global Declarations")
  @(":Public API")

  #ifdef SKP_MAIN
    @(":Global variables")
    @(":Functions")
  #endif // SKP_MAIN


#endif // SKP_VERSION

```
## General use Macros

### Variable arguments macro
  Let's set up the macros that will help us defining variadic functions.
Visit the [`vrg` library](https://github.com/rdentato/vrg) site for more information.

```C
@("after:Utility macros")
// Handle variable and default function arguments.
// Borrowed from vrg.h but renamed to minimize namespace pollution.
#define skp_v_cnt(skp_v1,skp_v2,skp_v3,skp_v4,skp_v5,skp_v6,skp_v7,skp_v8,skp_vN, ...) skp_vN
#define skp_v_argn(...)  skp_v_cnt(__VA_ARGS__, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define skp_v_cat0(x,y)  x ## y
#define skp_v_cat(x,y)   skp_v_cat0(x,y)
#define skp_vrg(skp_v_f,...) skp_v_cat(skp_v_f, skp_v_argn(__VA_ARGS__))(__VA_ARGS__)

```

### Tracing and testing macros
```C
@("after:Utility macros")
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

```


## Standard includes

```C
@("after:Standard Includes")
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

```

## Global variables

```C
@("after:Global declarations")
// Just a caution to avoid aggressive optimization.
extern volatile int skp_zero;

@("after:Global variables")
volatile int skp_zero = 0;

```



