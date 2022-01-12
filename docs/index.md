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
  - Skipping, where the focus is to identify portion of text that can be skipped according a pattern;
  - Scanning, where a text is repeatedly checked against a set of patterns;
  - Parsing, where a Parsing Expression Grammar can be directly coded as a recursive descent parser.

## Level 1: Skipping

### Basic skipping

The basic function for skipping text is:

``` C
  int skp(char *start, char *pattern, char **to)
```

  If the text at `start` matches the pattern, `skp` will return `1` and the `to`
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



```
   a  ASCII alphabetic char
   l  ASCII lower case
   u  ASCII upper case
   d  decimal digit
   x  hex digit
   w  white space (includes some Unicode spaces)
   s  white space and vertical spaces (e.g. LF)
   c  control
   i  identifier character
   n  a newline (`\n`, `\r` or `\r\n`)

   
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

Here is a full example:

``` C
   char *text = "a b c 123 d e";
   char *to, *end;

   skp(text, ">D", &to, &end);

   // a b c 123 d e
   //       ▲  ▲
   //       │  ╰── end
   //       ╰─────── start

```

## Level 2: Scanning
The `skp` function greatly simplifies extracting elements from a text string.
Sometimes, however, is more useful to continously match a block of text.

There is a form of the skp function that does this:

``` C
    skp(char *text) { ... }
```

Within the code guarded by skp
``` 
    skpif(char *pattern) { ... }  execute the code block if text match the pattern
    skpelse { } execute the code if nothing matches

    char   *skpfrom   pointer to the the start of the match
    char   *skpto     pointer to the the end of the match
    int32_t skplen    length of the matched text
```

You can think of it as loop that will go through the text until all
the text has been scanned or no match is found. For example:
```
   skp(csv) {
     skpif(",&*s") { num_col++; }
     skpif("&D")   { store_number(skpfrom, skpto, num_row, num_col);}
     skpif("&Q")   { store_string(skpfrom, skpto, num_row, num_col);}
     skpif("&n")   { num_row++; num_col = 0; }
     skpif("&+s")  { }
     skpelse       { prterror("Unexpected text at %d,%d\n",num_row, num_col); 
                     break;
                   }
     last = skpto; // remember last
   }
   if (*last != '\0') { // abnormal exit }
```

## Level 3: Parsing

For the most complicated case, you may need to define a *grammar* for the 
type of text you need to parse.

The following function returns the Abstract Syntax Tree (AST) resulting
from parsing a text with the grammar starting from the specified rule:

```
  ast_t skpparse(char *text, rule);
```

The second argument, `rule`, must be the name of a grammar rule specified
with `skpdef()`. Let's use a running example with the grammar for simple
arithmetic expressions:

```
 expr = term / term addop term
 term = number / '(' expr ')' / '-' term
 addop = '+' / '-'
```

This can be represented in skp with a recursive descent parser as follows:

```
  skpdef(expr) {
    skponce {
      skprule(term);
    }
    skpor {
      skprule(term);
      skpmatch("+\1-\2");
      skprule(term);
    }
  }

  skpdef(term) {
    skponce {
      skpmatch("&D");
    }
    skpor {
      skpstring("(");
      skprule(expr);
      skpstring(")");
    }
    skpor {
      skpstring("-");
      skprule(term);
    }
  }
```

The basic idea of directly represent in code a grammar is in:
```
   R. R. Redziejowski. Parsing Expression Grammar as a Primitive
   Recursive-Descent Parser with Backtracking.
   Fundamenta Informaticae 79, 3-4 (2007), pp. 513 - 524. 
   http://www.romanredz.se/papers/FI2007.pdf
```

### Grammar spefier

```
  skpmatch(char *pattern)             create a node `$n` with 1<=n<=7
  skpstring(char *string [, int n])   create a node `$n` with 1<=n<=7 
  skprule(rule)                       create a note `rule`
```


```
  skponce    parse
  skpor      try alternative parse
  skpmaybe   one or zero times
  skpany     zero or many imes
  skpmany    one or many times
  skpnot     negative lookahead
  skppeek    positive lookahead
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

### AST traversing
```
  astvisitdf(ast, node) { ... } visit the ast with a depth-first strategy

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

## Reference

