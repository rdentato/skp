

#define SKP_MAIN
#include "skp.h"


/*
eval = expr
expr = term (('+'|'-') term)* ;
term = fact (('*'|'/') fact)* ;
fact = num | id | - fact | '(' expr ')'
*/

skpdef(eval) {
  skprule(expr);
  skpmatch("&*s&!.\7"); // Ensure all input is consumed
}

skpdef(expr) {
  skprule(term);
  skpany {
    skprule(op_add);
    skprule(term);
    astswap;
  }
}

skpdef(neg) {
    skpstring("-");
    skprule(fact);
}

skpdef(term) {
  skprule(fact);
  skpany {
    skprule(op_mult);
    skprule(fact);
    astswap;
  }
}

skpdef(fact) {
  skpmatch("&*s\7");
  skponce { skpmatch("&D\5"); }
    skpor { skprule(neg); }
    skpor { skpmatch("&I\6"); }
    skpor {
      skpmatch("(&*s\7");
      skprule(expr); 
      skpmatch("&*s)\7");
    }
}

skpdef(op_add) {
  skpmatch("&*s\7");
  skpmatch("+\1-");
}

skpdef(op_mult) {
  skpmatch_("&*s\7");
  skpmatch("*\1/");
}

#define trace(...) (fprintf(stderr,__VA_ARGS__),fputc('\n',stderr))
int main(int argc, char *argv[])
{
  FILE *out=stdout;
  ast_t ast;

  if (argc<2) {fprintf(stderr,"string?\n"); return 1;}

  ast = skpparse(argv[1],eval);
  trace("Nodes: %d",astnumnodes(ast));

  astprint(ast,out);
  printf("\n");

  if (asterror(ast)) {
    trace("Error @: '%s'",asterror(ast));
    trace("In rule: '%s'",asterrorrule(ast));
  }
  else {
    astvisitdf(ast,node) {
      if (astisnodeentry(ast,node)) {
        if (astnodeis(ast,node,STR) && !astnodeis(ast,node,STR7)) {
          fprintf(out,"%.*s ",astnodelen(ast,node), astnodefrom(ast,node));
        }
      }
      else {
        if (astnodeis(ast,node,neg)) fprintf(out,"-1 * ");
      }
    }
    fputc('\n',out);
  }

  astfree(ast);
}



