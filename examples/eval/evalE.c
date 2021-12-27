

#define SKP_MAIN
#include "skp.h"

/*
eval = expr
expr = term (opadd term)* ;
term = fact (opmult fact)* ;
fact = num | id | - fact | '(' expr ')'
opadd = '+'|'-'
opmult = '*'|'/'|'%'
*/

skpdef (eval)
{
  skprule_ (expr);
  astliftall;
  skpmatch_ ("&*s&!.");		// Ensure all input is consumed
}

skpdef (expr)
{
  skprule (term);
  astliftall;
  skpany
  {
    skprule_ (op_add);
    skprule (term);
    astliftall;
    astswap;
  }
}

skpdef (term)
{
  skprule (fact);
  astliftall;
  skpany
  {
    skprule_ (op_mult);
    skprule (fact);
    astliftall;
    astswap;
  }
}

skpdef (fact)
{
  skpmatch_ ("&*s");
  skponce
  {
    skpmatch ("&D\2");
  }
  skpor
  {
    skpstring ("-", 6);
    skprule (fact);
    astliftall;
    astswap;
  }
  skpor
  {
    skpmatch_ ("(&*s");
    skprule_ (expr);
    astliftall;
    skpmatch_ ("&*s)");
  }
}

skpdef (op_add)
{
  skpmatch_ ("&*s");
  skpmatch ("+\1-");
}

skpdef (op_mult)
{
  skpmatch_ ("&*s");
  skpmatch ("*\1/");
}

#define STK_MAX 100
int32_t val_stk[STK_MAX];
int32_t stk_cnt = 0;

int32_t
pop ()
{
  return (stk_cnt > 0) ? val_stk[--stk_cnt] : 0;
}

void
push (int32_t n)
{
  if (stk_cnt < STK_MAX - 1)
    val_stk[stk_cnt++] = n;
}

#define empty() (stk_cnt = 0)
int32_t
calc (ast_t ast)
{
  int32_t a, b;
  empty ();
  astvisit (ast)
  {
    astifentry
    {
      astifnodeis (STR2)
      {
	push (atoi (astcurfrom));
      }
      astifnodeis (STR1)
      {
	switch (*astcurfrom)
	  {
	  case '+':
	    a = pop ();
	    b = pop ();
	    push (a + b);
	    break;

	  case '-':
	    a = pop ();
	    b = pop ();
	    push (b - a);
	    break;

	  case '*':
	    a = pop ();
	    b = pop ();
	    push (a * b);
	    break;

	  case '/':
	    a = pop ();
	    b = pop ();
	    push (a ? b / a : 0);
	    break;
	  }
      }
      astifnodeis (STR6)
      {
	a = pop ();
	push (-a);
      }
    }
  }
  a = pop ();
  return a;
}

#define trace(...) (fprintf(stderr,__VA_ARGS__),fputc('\n',stderr))
int
main (int argc, char *argv[])
{
  ast_t ast;
  printf ("nodesize: %ld\n", sizeof (ast_node_t));

  if (argc < 2)
    {
      fprintf (stderr, "string?\n");
      return 1;
    }

  ast = skpparse (argv[1], eval);

  if (asthaserr (ast))
    {
      trace ("In rule: '%s'", asterrrule (ast));
      char *ln = asterrline (ast);
      char *endln = ln;
      while (*endln && *endln != '\r' && *endln != '\n')
	endln++;
      int32_t col = asterrcolnum (ast);
      trace ("Error: %s", asterrmsg (ast));
      trace ("%.*s", (int) (endln - ln), ln);
      trace ("%*.s^", col, "");
      //trace("Error '%s'",asterrmsg(ast));
      //trace("@: '%s'",asterrpos(ast));
    }
  else
    {
      astprint (ast, stderr);
      fprintf (stderr, "\n");
      printf ("Expr: %s\n", argv[1]);
      printf (" Res: %d\n", calc (ast));
    }

  astfree (ast);
}
