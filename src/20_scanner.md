# SKP - scanners 

## Table of Contents
- [SKP - scanners](#skp---scanners)
  - [Table of Contents](#table-of-contents)
  - [Introduction](#introduction)
  - [SKP Groups](#skp-groups)
  - [SKP Scanner](#skp-scanner)
    - [How It Works:](#how-it-works)
  - [Matching](#matching)
  - [Implementation](#implementation)
  - [Save/Restore](#saverestore)

## Introduction
  A pretty common task is to match a given text with a sequence of 
patterns. While this could easily achieved using `skp()` within
`if` and `while` statments, `skp` offers two *instructions* to ease
the task.

## SKP Groups

The `skpgroup` instruction allows you to group multiple `skp` statements into a single unit. The syntax for `skpgroup` is as follows:

``` C
skpgroup(char *text, [char **next], [int *matched]) { _block_ }
```

When you use `skpgroup`, you can enclose a series of `skp` statements within the curly braces `{ _block_ }`. This group of statements is evaluated as a whole. 

Here's an example:

``` C
char *next = "";
skpgroup("ABC", &next) {
    ifskp("l")     { printf("matched a lowercase\n"); }
    elseifskp("u") { printf("matched an uppercase\n"); }
}
// In this example, *next will point to 'B' after execution
```

- The `skpgroup` block will either succeed or fail as a single entity.
- If the entire group matches, one of the `printf` statements will be executed depending on whether the text starts with a lowercase or uppercase letter.
- If the `skpgroup` fails, the `next` pointer will remain unchanged.

You can also provide an optional integer pointer `matched` as an argument to `skpgroup`. This integer will be set to `1` if the group matches successfully, or `0` if it fails. This allows you to check the success or failure of the entire group after it has been processed.

## SKP Scanner

The `skpscan` instruction is designed to repeatedly execute a block of `skp` statements over a given string until the block fails. This is useful when you need to perform the same operation multiple times on different parts of the string.

``` C
skpscan(char *text, [char **next], [int *matched]) { _block_ }
```

### How It Works:

The `skpscan` instruction works similarly to `skpgroup`, but with an important difference: instead of stopping after one successful execution, `skpscan` continues to execute the block as long as the block succeeds.

Here’s a detailed explanation:

- **Initialization**: `skpscan` starts by attempting to match the block of `skp` statements to the beginning of the `text`.
  
- **Repetition**: If the block matches successfully, `skpscan` advances the `next` pointer to the end of the matched portion and then reattempts to execute the block on the remaining part of the string. This process continues until the block fails to match.

- **Termination**: Once the block fails to match, `skpscan` stops. At this point, the `next` pointer will be positioned at the first character where the block could not match.


``` C
char *next = "";
skpscan("abcabcabc", &next) {
    ifskp("'abc'") { printf("matched 'abc'\n"); } // Will be executed three times
}
// After execution, *next will point to the end of the string
```

In this example, the block inside `skpscan` will successfully match and print `"matched 'abc'"` three times, once for each occurrence of `"abc"` in the input string. After the final match, `skpscan` will stop, and the `next` pointer will be positioned at the end of the string (or the first unmatched portion if any).

As with `skpgroup`, you can include an optional integer pointer `matched` as an argument to `skpscan`. This integer will be set to `1` after the block succeeds at least once. If the block never matches, `matched` will be set to `0`. This allows you to easily check whether the block was able to match at least once over the entire string.

The key point on `skpscan` are:

- **Entire String Scanning**: `skpscan` is particularly useful when you need to search for patterns throughout the entire string, applying the same logic repeatedly until no further matches can be found.

- **Efficiency**: The `skpscan` function is efficient for tasks that involve repeated pattern matching, such as parsing tokens or finding recurring sequences within a string.

By using `skpscan`, you can simplify code that requires repeated matching, reducing the need for manual loops and improving readability.


## Matching 
The following instructions—`ifskp`, `elsifskp`, `elseskp`, and `whileskp`—are fundamental for controlling the flow of text matching in a structured and logical manner. They allow you to define conditional and iterative logic based on whether certain patterns are found in a string. These instructions form the core of text matching in this context.

**`ifskp(pattern [, ...]) { ... }`**

The `ifskp` instruction is used to execute a block of code only if the specified patterns are successfully matched in the string. 

  ``` C
  ifskp(pattern [, ...]) { ... }
  ```
  - You can specify one or more patterns within the `ifskp` instruction.
  - If all specified patterns match the beginning of the string, the code block within the curly braces `{ ... }` is executed.
  - If any of the patterns fail to match, the code block is skipped.

  ``` C
  ifskp("'abc'", "'123'") { printf("Matched 'abc' followed by '123'\n"); }
  ```
  This code will print the message only if the string starts with `"abc"` followed by `"123"`.

**`elsifskp(pattern [, ...]) { ... }`**

The `elseifskp` instruction serves as an alternative path that is taken if the preceding `ifskp` (or another `elsifskp`) statement fails to match.

  ``` C
  elseifskp(pattern [, ...]) { ... }
  ```

  - If the preceding `ifskp`, `elsifskp` oe `whileskp` fails to match all the specified patterns, the code block associated with the `elsifskp` will be executed only if the patterns within `elsifskp` are matched.
  - You can chain multiple `elseifskp` statements to check for different patterns sequentially.

  ``` C
  ifskp("'abc'") {
      printf("Matched 'abc'\n");
  } elsifskp("'def'") {
      printf("Matched 'def' instead\n");
  }
  ```
  This code will print `"Matched 'abc'"` if the string starts with `"abc"`. If it doesn’t, but instead starts with `"def"`, it will print `"Matched 'def' instead"`.

**`elseskp { ... }`**

The `elseskp` instruction provides a final fallback option, executing its code block if none of the previous `ifskp` or `elsifskp` conditions matched.

  ``` C
  elseskp { ... }
  ```

  - The `elseskp` block is executed only if all preceding `ifskp` and `elsifskp` conditions fail.
  - It does not require a pattern to match; it simply catches any cases that did not meet the previous conditions.

  ``` C
  ifskp("'abc'") {
      printf("Matched 'abc'\n");
  } elsifskp("'def'") {
      printf("Matched 'def'\n");
  } elseskp {
      printf("Matched neither 'abc' nor 'def'\n");
  }
  ```
  In this example, if the string doesn’t start with either `"abc"` or `"def"`, the code will print `"Matched neither 'abc' nor 'def'"`.

**`whileskp(pattern [, ...]) { ... }`**

The `whileskp` instruction is used to repeatedly execute a block of code as long as the specified patterns continue to match the string.

  ``` C
  whileskp(pattern [, ...]) { ... }
  ```

  - The block of code within `whileskp` is executed repeatedly as long as the specified patterns match the string.
  - After each successful match, the `next` pointer advances, and the block is executed again on the remaining string.
  - The loop ends when the patterns fail to match.

  ``` C
  whileskp("'abc'") {
      printf("Found 'abc'\n");
  }
  ```
  This example will repeatedly print `"Found 'abc'"` for each occurrence of `"abc"` at the beginning of the string until `"abc"` can no longer be matched.


These instructions—`ifskp`, `elsifskp`, `elseskp`, and `whileskp`—are essential for creating complex text-matching logic:

- **`ifskp`**: Executes a block if specified patterns match.
- **`elsifskp`**: Provides an alternative path if the previous match fails.
- **`elseskp`**: Executes a block if none of the previous conditions matched.
- **`whileskp`**: Repeatedly executes a block as long as the patterns continue to match.

Together, they allow you to build flexible, conditional, and iterative matching logic that can handle various text processing tasks efficiently.


## Implementation

```C

@("after:Public API")

#define skpscan(...)    skpgroup(__VA_ARGS__) manyskp

#define skpgroup(...)    skp_vrg(skpgroup_,__VA_ARGS__)
#define skpgroup_1(s)    skpgroup_3(s,skpto, &skpmatched)
#define skpgroup_2(s, t) skpgroup_3(s, t, &skpmatched)
#define skpgroup_3(s, t, r) for (char *skpstart = NULL; (skpstart == NULL) && (skpstart = s);  ) \
                              for (int skpmatched = -1; (skpmatched < 0) && (skpmatched = 1); skpmatched? (((t)[0] = skpto[0]) , ((r)[0] = skpmatched)) : 0)


#define skplen(n) skp_len(n,skpfrom, skpto)
static inline int skp_len(int n, char **from, char **to) { return to[n] - from[n];}

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


@("after:Public API")
#define skp_save() {skpstart, *skpfrom, *skpto, (skpmatched = 1) }

#define skp_restore() ((skpstart = skp_save.start, \
                       *skpfrom = skp_save.from, \
                         *skpto = skp_save.to), \
                                  0)

#define skp_setmatchstart() ((*skpfrom = skp_save.start),\
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

#define whileskp(...) manyskp ifskp(__VA_ARGS__)
/*  if (!skpmatched) ; else \
    for ( int skp_count = 1;\
          \
          skp_count \
          && ((skpmatched = skp(skpstart, __VA_ARGS__, skpfrom, skpto)) \
              || ((skpmatched = (skp_count>1)) && (skp_count = 0)));\
          \
          skpstart = *skpto, \
          skp_count++ ) 
*/

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
       skp_do = skpmatched ? skp_setmatchstart() : skp_restore())

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

#define manyskp \
  if (!skpmatched) ; else for  (skp_save_t skp_save = skp_save(); \
       skpmatched || ((skpmatched = (skp_do > 2) ? 1 : skp_restore()) && skp_zero) ; \
       skp_do++) 

#define orskp  if (skpmatched || !(skpmatched = 1)) ; else mustskp
#define andskp if (!skpmatched) ; else



```

