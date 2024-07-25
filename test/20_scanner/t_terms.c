#include "tst.h"
#define SKP_MAIN
#include "skp.h"

/*
expr =  term term*;
term = __ ( identifier | '(' expr ')' );
*/
skprecognizer(expr) {
  int matched = 0;

  skpscan(skpstart, skpto) {
    skpcase(term) { matched = 1; }
  }
  skp(*skpto,"+w",skpto);
  return matched;
}

skprecognizer(term) {
  int matched = 0;
  char *saved;

  skp(skpstart,"+w",&skpstart);

  if (skp(skpstart,identifier,&skpstart)) return 1;

  save = skpstart;
  if (skp(skpstart, "'('")) {
    if (skp)
  }

  skpswitch(skpstart,skpto) {
    skpcase("+w") { }
    skpcase("I")  { fprintf(stderr,"<%.*s> %d\n",skplen(0),skpfrom[0],skplen(0));}
  }
  return matched;
}


char *from;
char *to;
char *text;
tstsuite("expressions") {
  tstcase("factors") {
    text = " pippo (pluto topolino) pappo";
    skp(text,expr,&from, &to);
  }
}