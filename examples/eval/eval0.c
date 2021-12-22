/*
**  (C) by Remo Dentato (rdentato@gmail.com)
** 
** This software is distributed under the terms of the MIT license:
**  https://opensource.org/licenses/MIT
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SKP_MAIN
#include "skp.h"

/*
A simple and not entirely correct expressions grammar (it's right associative!!!)
```
eval = expr
expr = fact ('+' / '-') expr / fact ;
fact = num / '(' expr ')'
```

You can see how easy is to build the parser following the grammar rules one
by one:
*/

skpdef(eval) {
  skprule(expr);
  skpmatch("&!.\7"); // Ensure all input is consumed
}

skpdef(expr) {
  skponce {
    skprule(fact);
    skp_trace("FACT2: %d",ast_->fail);
    skpmatch("+\1-"); // + or -
    skprule(expr);
  }
  skpor {
    skprule(fact);
    skp_trace("FACT: %d",ast_->fail);
  }
}

skpdef(fact) {
  skponce {
    skpmatch("&D");
  }
  skpor {
    skpstring("(");
    skprule(expr);
    skpstring(")");
  }
}

/*


*/

#define trace(...) (fprintf(stderr,__VA_ARGS__),fputc('\n',stderr))
int main(int argc, char *argv[])
{
  ast_t ast;
  FILE *out=stdout;

  if (argc<2) {fprintf(stderr,"Usage: %s expr\n",argv[0]); return 1;}

  ast = skpparse(argv[1],eval);

  astprint(ast,out);
  printf("\n");

  if (asterror(ast)) {
    trace("Error @: '%s'",asterror(ast));
    trace("In rule: '%s'",asterrorrule(ast));
  }
  else {
    astvisitdf(ast,node) {
      if (astisnodeentry(ast,node)) {
        if (astnodeis(ast,node,STR1)) {
          fprintf(out,"%.*s ",astnodelen(ast,node), astnodefrom(ast,node));
        }
      }
    }
    fputc('\n',out);
  }
  astfree(ast);
}

