

#define SKP_MAIN
#include "skp.h"


/*
expr = (term ('+' / '-'))* term
term = (fact ('*' / '/'))* fact
fact = '-' fact / '(' expr ')' / num / var / fun
*/

skpdef(eval) {
  skprule_(expr);
  skpmatch_("&*s&!."); // Ensure all input is consumed
}

skpdef(expr) {
  skprule(term_add); 
  skprule(term);
}

skpdef(term_add) {
    skprule(term); 
    skprule(op_add);
    skpruleterm_add
}

skpdef(term) {
  skpany {
    skprule(fact);
    skprule(op_mult);
  }
  dbgtrc("TERM: 1 fail: %d",ast_->fail);
  skprule(fact);
  dbgtrc("TERM: 2 fail: %d",ast_->fail);
}

skpdef(fact) {
  skpmatch_("&*s");
  skponce { skpmatch("&D\5"); }
    skpor { skprule(neg); }
    skpor { skpmatch("&I\6"); }
    skpor {
      skpmatch_("(&*s");
      skprule(expr);
      skpmatch_("&*s)");
    }
}

skpdef(neg) {
    skpstring("-");
    skprule(fact);
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
  trace("Nodes: %d",astnumnodes(ast));

  astprint(ast,out);
  printf("\n");

  if (asterror(ast)) {
    trace("Error @: '%s'",asterror(ast));
    trace("In rule: '%s'",asterrorrule(ast));
  }
  else {
    astvisitdf(ast,node) {
      if (astisnodeentry(ast,node)) {  // Entering the node (preorder)
        if (astnodeis(ast,node,STR)) {
          fprintf(out,"%.*s ",astnodelen(ast,node), astnodefrom(ast,node));
        }
      }
      else { // Exiting the node (postorder)
        if (astnodeis(ast,node,neg)) fprintf(out,"-1 * ");
      }
    }
    fputc('\n',out);
  }

  astfree(ast);
}



