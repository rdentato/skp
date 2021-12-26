#include <stdio.h>
#include <stdint.h>
#include "expr.h"


#define trace(...) (fprintf(stderr,__VA_ARGS__),fputc('\n',stderr))
int main(int argc, char *argv[])
{
  ast_t ast;

  if (argc<2) {fprintf(stderr,"string?\n"); return 1;}

  ast = skpparse(argv[1],expr);
  
  if (asthaserr(ast)) {
    trace("In rule: '%s'",asterrrule(ast));
    char *ln = asterrline(ast);
    char *endln = ln;
    while (*endln && *endln != '\r' && *endln != '\n') endln++;
    int32_t col = asterrcolnum(ast);
    trace("Error: %s",asterrmsg(ast));
    trace("%.*s",(int)(endln-ln),ln);
    trace("%*.s^",col,"");
    //trace("Error '%s'",asterrmsg(ast));
    //trace("@: '%s'",asterrpos(ast));
  }
  else {
    astprint(ast,stderr);
    fprintf(stderr,"\n");
    printf("Expr: %s\n",argv[1]);
    //printf(" Res: %d\n",calc(ast));
  }

  astfree(ast);
}

