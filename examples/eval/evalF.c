#define SKP_MAIN
#include "skp.h"

/*
eval = expr
expr = term (opadd term)* 
term = fact (opmult fact)*
fact = num | id | neg fact | '(' expr ')'
opadd = '+' | '-'
opmult = '*' | '/'
neg = '-'
num = "D"
*/

// eval = expr
skpdef(eval) {
  skprule(expr);
  skpmatch_("S!."); // Ensure all input is consumed
}

// expr = term (opadd term)* 
skpdef(expr) {
  skprule(term); 
  // astliftall; // Uncomment this to see how AST is simplified
  skpany {
    skprule(op_add);
    skprule(term);
    // astliftall; // Uncomment this to see how AST is simplified
    astswap;
  }
}

// term = fact (opmult fact)* ;
skpdef(term) {
  skprule(fact); 
  // astliftall; // Uncomment this to see how AST is simplified
  skpany {
    skprule(op_mult);
    skprule(fact);
    // astliftall; // Uncomment this to see how AST is simplified
    astswap;
  }
}

// fact = num | id | neg fact | '(' expr ')'
skpdef(fact) {
  skpmatch_("S");
  skponce { skprule(num); }
    skpor { skprule(neg);
            skprule(fact);
            // astliftall; // Uncomment this to see how AST is simplified
            astswap; }
    skpor { skpmatch_("'('S");  skprule(expr); skpmatch_("S')'"); }
}

// opadd = '+' | '-'
skpdef(op_add)  { skpmatch_("S[+-]"); }

// opmult = '*' | '/'
skpdef(op_mult) { skpmatch_("S[/*]"); }

// num = "D"
skpdef(num) { skpmatch_("D"); }

// neg = '-'
skpdef(neg) { skpstring_("-"); }


#define STK_MAX 100
int32_t val_stk[STK_MAX];
int32_t stk_cnt = 0;

int32_t pop() 
{ return (stk_cnt > 0)? val_stk[--stk_cnt] : 0; }

void push(int32_t n)
{ if (stk_cnt < STK_MAX-1) val_stk[stk_cnt++] = n; }

#define cleanstk() (stk_cnt = 0) 
int32_t calc(ast_t ast)
{
  int32_t a,b;
  cleanstk();
  astvisit(ast) {
    astonentry {
      astcase(num) {
        push(atoi(astcurfrom));
      }
      astcase(op_add, op_mult) {
        switch(*astcurfrom) {
          case '+' : a = pop(); b = pop(); push(a+b);
                     break;

          case '-' : a = pop(); b = pop(); push(b-a);
                     break;

          case '*' : a = pop(); b = pop(); push(a*b);
                     break;

          case '/' : a = pop(); b = pop(); push(a?b/a:0);
                     break;
        }
      }
      astcase(neg) {
        a = pop(); push(-a);
      }
    }
  }
  a = pop();
  return a;
}

#define trace(...) (fprintf(stderr,__VA_ARGS__),fputc('\n',stderr))
int main(int argc, char *argv[])
{
  ast_t ast;

  if (argc<2) {fprintf(stderr,"Usage: evalF expression\n"); return 1;}

  ast = skpparse(argv[1],eval);
  
  if (asthaserr(ast)) {
    trace("In rule: '%s'",asterrrule(ast));
    char *ln = asterrline(ast);
    char *endln = ln;
    while (*endln && *endln != '\r' && *endln != '\n') endln++;
    int32_t col = asterrcolnum(ast);
    trace("Error: %s",asterrmsg(ast));
    trace("%.*s",(int)(endln-ln),ln);
    trace("%*.s^",col,"");
  }
  else {
    astprint(ast,stderr);
    fprintf(stderr,"\n");
    printf("Expr: %s\n",argv[1]);
    printf(" Res: %d\n",calc(ast));
  }

  astfree(ast);
}

