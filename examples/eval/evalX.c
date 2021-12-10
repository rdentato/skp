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
  skpmany {
    skprule(expr);
    astnoleaf;
  }
  skpmatch_("&!.\7"); 
}

skpdef(expr) {
  skponce {
    skprule(AA);
    astlift;
    astnoleaf;
  }
  skpor {
    skprule(BB);
  }
  skpor {
    skprule(CC);
  }
}

skpdef(AA) {
  skpstring("AA");
  astlift;
}
skpdef(BB) {
  skpstring("B");
  skpstring("B");
}

skpdef(CC) {
  skpstring("C");
  skpstring("C");
}

skpdef(DD) {
  skpstring_("D");
  skpstring_("D");
}

#define trace(...) (fprintf(stderr,__VA_ARGS__),fputc('\n',stderr))
int main(int argc, char *argv[])
{
  ast_t ast;
  FILE *out=stdout;

  if (argc<2) {fprintf(stderr,"Usage: %s expr\n",argv[0]); return 1;}

  ast = skpparse(argv[1],DD);

  astprint(ast,out);
  printf("\n");

  if (asterror(ast)) {
    trace("Error @: '%s'",asterror(ast));
    trace("In rule: '%s'",asterrorrule(ast));
  }
  else {
    for (int k=0; k< ast->par_cnt; k++) {
      fprintf(out,"%4d",k);
    }
    fputc('\n',out);
    for (int k=0; k< ast->par_cnt; k++) {
      fprintf(out,"%4d",ast->par[k]);
    }
    fputc('\n',out);
    for (int k=0; k< ast->par_cnt; k++) {
      if (ast->par[k] >= 0) {
        fprintf(out,"%4d: %s '%.*s'\n",ast->par[k],astnoderule(ast,k),astnodelen(ast,k),astnodefrom(ast,k));
      }
    }
  }
  astfree(ast);
}

