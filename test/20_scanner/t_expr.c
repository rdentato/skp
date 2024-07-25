#include "tst.h"
#define SKP_MAIN
#include "skp.h"

/*
expr = factor ( __ op1 factor)* ;
factor = __ term (__ op2 __ term)* ;
term = __ ('-' term | number | '(' expr ')' );
op1 = '+' | '-';
op2 = '*' | '/';
*/
skprecognizer(factor) {
  skpscan(skpstart,skpto) {
  skpcase("+w") { fprintf(stderr,"'%.*s' %d\n",skplen,skpfrom,skplen);}  
  skpcase("D")  { fprintf(stderr,"<%.*s> %d\n",skplen,skpfrom,skplen); skpstop(1); }
  skpcase(NULL) { fprintf(stderr,">>\n");  skpstop(0);}

  return(skpmatched);
}

/*

skpscanner(factor_tail) {
  char op = '\0';

  skpcase("+w")   {}
  skpcase("[+-]") { op = *skpfrom; }
  skpcase(factor) { op = '\0';    }
  skpreturn(1);
}
*/

skpscanner(expr) {
  skpcase(factor) { fprintf(stderr,"XX\n"); }
  skpreturn(skpmatched);
}

char *from;
char *to;
char *text;
tstsuite("expressions") {
  tstcase("factors") {
    text = " 23 42";
    skp(text,expr,&from,&to);
    tstnote("factors: '%.*s'",(int)(to-from),from);
    tstnote("        %p  %p",(void *)from,(void*)to);
  }
}