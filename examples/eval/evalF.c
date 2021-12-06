

#define SKP_MAIN
#include "skp.h"


/*
expr = '-' term | term (('+'|'-') term)* ;
term = fact (('*'|'/') fact)* ;
fact = num | id | '(' expr ')'
*/

skpdef(eval) {
  skprule_(expr);
  skpmatch_("&*s&!."); // Ensure all input is consumed
}

skpdef(expr) {
  skprule(term); astlift;
  skpany {
    skprule_(op_add);
    skprule(term); astlift;
    astswap;
  }
}

skpdef(neg) {
    skpstring_("-");
    skprule_(fact);
}

skpdef(term) {
  skprule_(fact);
  skpany {
    skprule_(op_mult);
    skprule_(fact);
    astswap;
  }
}

skpdef(fact) {
  skpmatch_("&*s");
  skponce { skpmatch("&D\5"); }
    skpor { skprule(neg); }
    skpor { skpmatch("&I\6"); }
    skpor {
      skpmatch_("(&*s");
      skprule(expr);  astlift;
      skpmatch_("&*s)");
    }
}

skpdef(op_add) {
  skpmatch_("&*s");
  skpmatch("+\1-");
}

skpdef(op_mult) {
  skpmatch_("&*s");
  skpmatch("*\1/");
}

#define trace(...) (fprintf(stderr,__VA_ARGS__),fputc('\n',stderr))
int main(int argc, char *argv[])
{
  FILE *out=stdout;
  ast_t ast;

  if (argc<2) {fprintf(stderr,"string?\n"); return 1;}

  ast = skpparse(argv[1],eval);
  dbgtrc("Nodes: %d",astnumnodes(ast));

  astprint(ast,out);
  printf("\n");

  if (asterror(ast)) {
    trace("Error @: '%s'",asterror(ast));
    trace("In rule: '%s'",asterrorrule(ast));
  }
  else {
    astvisitdf(ast,node) {
      if (astisnodeentry(ast,node)) {
        if (astnodeis(ast,node,STR)) {
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



