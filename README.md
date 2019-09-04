# skp
Match and skip a pattern in a text string.

Model (each box is a codepoint according the used encoding):

```
    nxt=skp(str,pat)
       ___________________
      /  matched pattern  \
      +--+--+--+--+--+--+--+--+--+--
      |  |  |  |  |  |  |  |  |  | ... 
      +--+--+--+--+--+--+--+--+--+--
   str-^                nxt-^
      
```
Returns `NULL` if `str` doesn't match the pattern.


## Supported charset/encodings
 For upper/lower/alpha, ... : 

```
  UTF-8 Unicode Basic Latin, 0000–007F
        Unicode Latin-1 Supplement, 0080–00FF
        Unicode Latin Extended-A, 0100–017F
        
  ISO 8859-1  Latin-1
  ISO 8859-2  latin-2
  ISO 8859-3  Latin-3
  ISO 8859-4  Latin-4
  ISO 8859-9  Latin-5
  ISO 8859-10 Latin-6
  ISO 8859-13 Latin-7
  ISO 8859-14 Latin-8
  ISO 8859-15 Latin-9
  ISO 8859-16 Latin-10
```

## Patterns

  * Any character except for '%' matches itself
  * `%%`  the percent sign
  * `%#`  any control character (e.g. '\x04' or '\x8F')
  * `%=`  any character
  * `%.`  any character except the end of line
  * `%*`  Repeat next pattern 0 or more times
  * `%+`  Repeat next pattern 1 or more times
  * `%?`  Next pattern is optional
  * `%!`  Match the empty string if next pattern doesn't match
  * `%^`  
  * `%_`  an optional sequeance of blank characters
  * `%a`  an alphabetic character
  * `%B`  a balanced braced string starts with '(', '{' or '['
  * `%C`  set case insensitive comparison
  * `%c`  set case sensitive comparison (default)
  * `%D`  decimal integer number (possibly preceeded by +/-)
  * `%d`  decimal digit (0-9)
  * `%E`  set escape character (used by  %Q)
  * `%F`  floating point number (possibly with exponent)
  * `%I`  set ISO encoding (U:UTF-8 1:Latin-1 ... 9:Latin-9 0:Latin-10)
  * `%L`  rest of line (possibly empty)
  * `%l`  lowercase character
  * `%Q`  Quoted string (enclosed by ",' or ` )
  * `%N`  Newline (\r, \r\n or \n)
  * `%s`  a space character (includes \v and \f)
  * `%u`  uppercase character
  * `%X`  hex number (possibly preceeded by 0x)
  * `%x`  hex digit
  * `%w`  whitespace (only space, tab and nbsp)
  * `%( ... %)` submatch and capture
  * `%[ ... %]` range (same as in regex)
  * `%1`,...,`%9` captured text.
  * `%>` matches everywher (not just at the start)
      
```
typedef struct skpcapt_s skpcapt_t;

static inline skpcapt_t *skpcaptnew(int max);
#define skpcaptfree free

static inline  int  skpcaptlen(skpcapt_t *capt, int n);
static inline char *skpcaptstart(skpcapt_t *capt, int n);

char *skppattern(char *s, char *p, skpcapt_t *capt);
#define skp(s,...) skppattern(s, skpexp(skp0(__VA_ARGS__,NULL)), \
                                 skpexp(skp1(__VA_ARGS__,NULL,NULL)))

```
