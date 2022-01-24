# skp

Skp is a library of functions to help parsing text files.

Rather than answering the question "which portion of the text match the pattern?", skp
answer to the question "which portion of the text can be skipped to match the pattern?".
The difference is subtle but is there.

Skp is *Unicode-aware* but, being the focus on parsing programming languages and data files,
the uppercase/lower case conversion is only for the ASCII portion of the Latin alphabet.

Some of the Unicode spaces are recognized like U+00A0 (NO-BREAK SPACE), U+2001 (EM QUAD), etc.

Default encoding is UTF-8, alternatively the ISO-8859-1 encoding can be used.

Skp can be used at three increasing levels of complexity:
  - *Skipping*, where the focus is to identify portion of text that can be skipped according to a pattern;
  - *Scanning*, where a text is repeatedly checked against a set of patterns;
  - *Parsing*, where a Parsing Expression Grammar can be directly coded as a recursive descent parser.

## Level 1: Skipping

### Basic skipping

The basic function for skipping text is:

``` C
  int skp(char *start, char *pattern, char **to)
```

  If the text at `start` matches the pattern `pattern`, `skp` will return `1` and the `to`
variable will point right after the skipped text as shown in the picture below. 

```
        ╭─text that matches─╮
        │    the pattern    │
     ───┬───┬───┬───┬───┬───┬───┬───┬───
    ●●● │▒▒▒│▒▒▒│▒▒▒│▒▒▒│▒▒▒│   │   │ ●●●  
     ───┴───┴───┴───┴───┴───┴───┴───┴───
          ▲                   ▲
          ╰start              ╰to

```

  If there is no match, `skp` will return `0` and `to` will be set to `start`.

  For example: 

``` C
  char *txt = "Hello World";
  char *to;

  skp(txt,"Hello ",&to);
  printf("%s\n",to); // will print "World"

  skp(txt,"Goodbye ",&to);
  printf("%s\n",to); // will print "Hello World" as there's nothing to skip.
  
```

  Sometimes you just want to know if the text at `start` matches a certain pattern
or not. You can pass `NULL` as the value for `to` or use a simpler version of `skp()`:

``` C
  int skp(char *start, char *pattern)
```

  The following two lines are equivalent:

``` C
  skp(mytext, mypattern, NULL);
  skp(mytext, mypattern);
```

### Patterns

A *pattern* is a sequence of *recognizers* possibly separated by spaces.
For example `"d ?'cm'"` matches a digit followed by an optional `cm` string.

#### ASCII characters

Since `skp` is aimed at parsing *techincal* files (programs source code, data
files, etc..) we have restricted the concept of *letters* and *digit* to the
ones encoded in ASCII:

```
   a  ASCII alphabetic char
   l  ASCII lower case
   u  ASCII upper case
   d  ASCII decimal digit
```

```
   x  hex digit
```

```
   w  white space (includes some Unicode spaces)
   s  white space and vertical spaces (e.g. LF)
   c  control
   i  identifier character
   n  a newline (`\n`, `\r` or `\r\n`)
```
  
```
   ' " or ` a literal string (useful for optional and negatives)
   Q  Quoted string with '\' as escape
   B  Balanced sequence of parenthesis (can be '()''[]''{}')
   () Balanced parenthesis (only '()')
   I  Identifier ([_A-Za-z][_0-9A-Za-z]*)
   N  Up to the end of line
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
  
   .  (any non \0 character, UTF-8 or ISO encoded)
   !. (the \0 character, i.e. the end of text)

   >  skip to the start of pattern (skip to)

   & the character '&'

   \1 ... \7  alternatives
   \xE string alternatives
```

Examples:

``` C
   skp(cur,"*s", &cur);              // skip spaces (if any)
   skp(start,"'|' *!'|' '|'", &end); // skip text enclosed in '|'
   skp(text,"!C'foo'", &to);         // skips "foo", "FoO", "fOo", etc.

   alt = skp(text,"D\1I\2");   // returns 1 if it's an integer number
                               //         2 if it's an identifier
                               //         0 None of the above
```


### Skip to
Sometimes you want to skip ahead until you match a specified pattern:

```
              ╭─text that matches─╮
              │    the pattern    │
   ───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───
  ●●● │   │   │▒▒▒│▒▒▒│▒▒▒│▒▒▒│▒▒▒│   │   │ ●●●  
   ───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───
        ▲       ▲                   ▲  
        ╰start  ╰to                 ╰end
```

For example, you want to skip everything until you find an integer number.
To achieve this result, you can place a `>` before your pattern like
in this example:

``` C
   skp(text,">D",&num); // Set num to the the beginning of next number
```

To avoid calling `skp` again to reach the end of the text that matches
the pattern, you can use another form of the `skp` function:

``` C
  int skp(char *start, char *pattern, char** to, char **end)
```
If there is no match, both `to` and `end` will be set equal to `start`.

Here is a full example:

``` C
   char *text = "ab c 123 de";
   char *to, *end;

   skp(text, ">D", &to, &end);

   //   ab c 123 de
   //   ▲    ▲  ▲
   //   │    │  ╰── end
   //  text  ╰───── to

```

## Level 2: Scanning
  The `skp` function greatly simplifies extracting elements from a text.
Sometimes, however, is more useful to continously match a block of text against
a set of patterns. If a match is successfull, you process the matching text
and move forward. This is a model followed, for example, by the `AWK` programming
language. 

  There is a form of the `skp()` function that does this:

``` C
    skp(char *text) { 
      skpif( pat1 ) {
        /* code to run if match pat1 */
      }
      skpif( xxx ) {
        /* code to run if match pat1 */
      }
    }
```

Within the code guarded by `skp()` you can use the following *instructions*
``` C
    skpif(char *pattern) { ... }  // execute the code block if text match the pattern
    skpelse { }                   // execute the code if nothing matches
```

and the following *read-only* variables:
```
    char   *skpfrom   pointer to the the start of the match
    char   *skpto     pointer to the the end of the match
    int32_t skplen    length of the matched text
```

You can think of it as loop that will go through the text until all
the text has been scanned or no match is found.

For example, a simple CSV reader can be implemented with:
``` C
   skp(csv) {
     skpif("',' *s") { num_col++; }  // Comma followed by spaces
     skpif("D")      { store_number(skpfrom, skpto, num_row, num_col);}
     skpif("Q")      { store_string(skpfrom, skpto, num_row, num_col);}
     skpif("*!','")  { store_string(skpfrom, skpto, num_row, num_col);}
     skpif("n")      { num_row++; num_col = 0; } // Newline
     skpif("+s")     { } // Skip spaces
     skpelse         { prterror("Unexpected text at %d,%d\n",num_row, num_col); 
                       break;
                     }
     last = skpto; // Store position
   }
   if (*last != '\0') { // abnormal exit }
```

## Level 3: Parsing

For the most complicated case, you may want to define a real *parser* for the 
type of text you need to process. 
The functions (and macros) available in `skp` allow you to define a recursive
descent parser using a Parser Expression Grammar (PEG) directly represented
as C code.

  The main idea behind the implementation of this feature is in the following article:

```
   R. R. Redziejowski. Parsing Expression Grammar as a Primitive
   Recursive-Descent Parser with Backtracking.
   Fundamenta Informaticae 79, 3-4 (2007), pp. 513 - 524. 
   http://www.romanredz.se/papers/FI2007.pdf
```

  As with many other PEG based parser, left recursive grammars are not supported
and would lead to an infinite loop. If such loop is detected, the parser will stop
returning an error.

  Parser defined with `skp` are both compact and fast for practical, not overly
complicated, grammars. They use partial memoization to provides (but 
not guarantee) a quasi-linear parsing time with reasonable memory consumption.

  In the following, we'll assume familiarity with PEG and recursive descent parsers.

### Grammars, terminals and non-terminals.

  A `skp` parser tries to mimic closely the structure of the grammar it
is intended to parse. Once the grammar terminals and non-terminals have been defined,
a text can be parsed using the function on the starting rule:
```
   ast_t skpparse(char *text, skprule_t rule [, int debug])
```
which returns the corresponding *Abstract Syntax Tree* (an object of type `ast_t`).

  Properly building and traversing the AST is the key for using `skp` parsers at their
fullest potential, I reccomend to carefully read the sections about AST.


  A `skp` *rule* represent a non-terminal in the grammar and is defined as if it
was C function (well, it *is* a C function ...):
```
  skpdef(rule) {
     /* rule body */
  }
```

  Within the definition of a rule, you can reference other rules (non-terminals) 
uaing the function:
```
  void skprule(skprule_t rule)
``` 

 Grammar's *terminals* are defined using the following functions:
```
   skpstring(str[,n])     match a literal string
   skpmatch(pattern)      match a skp pattern
   skplookup(f) { ... }   call a C function and use the return code
   skpcheck(f)            call a C function
```

 Terminals and non-terminals can be grouped using the following functions:

 ```
   skponce { ... }       match the body exactly once
   skpmany { ... }       match the body one or more times
   skpany { ... }        match the body zero or more times
   skpmaybe { ... }      match the body zero or one times
   skpor { ... }         try to match the body if the previous failed
   skppeek { ... }       match the body but don't consume the input (look ahead)
   skpnot { ... }        match only if the body fails and don't consume input (negative look ahead)
 ```

 ### Example: simple arithmetic expressions
  
  Consider this  grammar for simple arithmetic expressions:

```
 expr = term ( op term )* 
 term = INTEGER / '(' expr ')' / '-' term
 op = '+' / '-'
```

  This can be represented using `skp` as follows:

``` C
// expr = term (op term)* 
skpdef(expr) {
  skprule(term); 
  skpany {
    skprule(op);
    skprule(term);
  }
}

// term = INTEGER / '(' expr ')' / '-' term
skpdef(term) {
  skponce { skpmatch("D"); }
    skpor { skpstring("(");
            skprule(expr);
            skpstring(")");
          }
    skpor { skpstring("-",4);
            skprule(term);
          }
}

// op = '+' / '-'
skpdef(op)  { skpmatch("'+'\2 '-'\3"); }

```

  It's easy to see the one to one correspondance between 
the grammar and the C code.

### Abstract Syntax tree.

  The `skpparse()` function returns an object of type `ast_t` which is a 
representation of the Abstract Syntax Tree (AST) obtained by parsing the text
according the grammar represented by the defined rules.

  For example, the expression `2+(4-3)` when processed by the parser described
in the previos section, will return this AST:
```
[expr]
    [term]
        [$ (1)] '2'
    [op]
        [$ (2)] '+'
    [term]
        [$ (1)] '('
        [expr]
            [term]
                [$ (1)] '4'
            [op]
                [$ (3)] '-'
            [term]
                [$ (1)] '3'
        [$ (1)] ')'
```

  The one above is a linearization of the following tree:

```
                       [expr]
      ╭─────────┬────────┴─────────────╮
   [term]     [op]                   [term]
      │         │        ╭─────────────┼─────────────────╮
   [$ (1)]   [$ (2)]  [$ (1)]        [expr]           [$ (1)]
      │         │        │      ╭──────┴─┬─────────╮      │
     '2'       '+'      '('  [term]    [op]     [term]   ')'
                                │        │         │ 
                             [$ (1)]  [$ (3)]   [$ (1)]
                                │        │         │ 
                               '4'      '-'       '3'
```

  The full tree usually contains nodes that are not relevant for the next 
processing steps, the section on AST *modifiers* will explain how to prune
the tree so that it is better suited at being processed.

  Note that some nodes are *tagged* with an integer. Tagging a node can
help during the AST traversal and avoid extra work. In the example 
above, the two strings below the `op` nodes, are tagged with `(1)` for
the `+` operation and with `(2)` for `-`.

### Visiting the AST depth-first
  A very common traversing order for AST is the *depth-first* one in which
all children of a node are visited before visiting its siblings.

```
       A    
      / \    A -> B -> D -> E -> C
     B   C
    / \
   D   E
```

  The diagram above shows a tree and the sequence in which nodes will be
visited during a depth-first traversal.

  You can perform actions (i.e. execute code) when *entering* a node
(i.e. before visiting any of its children) and/or when *exiting* a node
(i.e. after having visited all of its children). This correspond, respectively,
to the *pre-order* and the *post-order* visit strategy.

  The easiest way to visit the AST depth-first is to use
the `astvisit(ast_t ast) { ... }` function. Within the block code guarded
by `astvisit` you can use the `astonentry { ... }` and `astonexit { ... }`
to execute code when, respectively, entering/exiting a node. To determine 
the type of the node your are visting you can use the `astcase(rule, ...)`
function were you can specify up to five rulenames

```
  astvisit(ast, node) { ... } visit the ast with a depth-first strategy
  astonentry { ... }
  astonexit { ... }
  astcase(rule [, rule ... ]) { ... }
```

```

  int astnodeis(ast_t ast,int32_t node, rule)
  int astisleaf(ast_t ast, int32_t node);

  int32_t astnumnodes(ast_t ast);
  void astprint(ast_t ast, FILE *f);

  #define ASTNULL -1
  int32_t astnextdf(ast_t ast, int32_t ndx);

  int astisnodeentry(ast_t ast, int32_t ndx);
  int astisnodeexit(ast_t ast, int32_t ndx);

  char   *astnoderule(ast_t ast, int32_t node);
  char   *astnodefrom(ast_t ast, int32_t node);
  char   *astnodeto(ast_t ast, int32_t node);
  int32_t astnodelen(ast_t ast, int32_t node);
```

### AST in-grammar modifiers
Sometime you don't want certain nodes in the final AST.
Tipically, spaces and separators are useless for the next steps of
the transformation. 

You can prevent a node to be generated using a variant of the skp... functions
(ending with an underscore:)
```
  skpmatch_(char *pattern);
  skpstring_(char *string [, int alt]);
  skprule_(rule);
```

There also the following ast modifiers:
```
  - astswap;         swap last record with its left sibling;
  - astlift;         replace a node with its only child;
  - astliftall;      replace a node with all its children;
  - astinfo(n);      add an INFO node with the specified value;
  - astnoleaf;       remove previous node if it's a leaf;
  - astnoemptyleaf;  remove previous node if it's a leaf and it's empty;
  - astlastnode;     returns a pointer to last node (of type ast_node_t *); 
  - astremove;       deletes the last node;
  - astlastnodeisempty returns true if the text matched by last 
```

## skpgen
  Representing a grammar directly as `C` code may have advantages but can be
tedious sometimes. And, in any casy, one has to write down a description of
the grammar (as a PEG) before coding it.
  The tool to automate this process is in the `example/skpgen` directory
and it is used to create most of the other examples.

## Reference

