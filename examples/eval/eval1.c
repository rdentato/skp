


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

int main(int argc, char *argv[])
{
  //char *last=NULL;
  ast_t ast;

  if (argc<2) {fprintf(stderr,"string?\n"); return 1;}

  ast = skpparse(argv[1],eval);
  dbgtrc("Nodes: %d",astnumnodes(ast));

  astprint(ast,stdout);
  printf("\n");

  dbgtrc("left: '%s'",ast->start+ast->pos);

  if (ast->err_pos >= 0) {
    dbgtrc("err@: '%s'",ast->start+ast->err_pos);
    dbgtrc("after: '%s'",ast->err_rule);
  }

  FILE *f=stdout;
  astvisitdf(ast,node) {
    _dbgtrc("NODE: %d %d",node,ast->par[node]);
    if (astisnodeentry(ast,node)) {
     _dbgtrc("entry node: %d %d",node, ast->par[node] >=0? node : ast->par[node]+node );
      if (astnodeis(ast,node,STR)) {
        fprintf(f,"%.*s ",astnodelen(ast,node), astnodefrom(ast,node));
      }
    }
    else {
     _dbgtrc("exit  node: %d %d",node,ast->par[node] >=0? node : ast->par[node]+node );
      if (astnodeis(ast,node,neg)) 
        fprintf(f,"-1 * ");
    }
  }
  fputc('\n',f);

  free(ast);
}

