#include "tst.h"
#define SKP_MAIN
#include "skp.h"

skpdef(parpippo) {
  mustskp {
    ifskp("*w '(' *w") { tstnote("matched open %d",skpmatched); }
    ifskp("'pippo'")   { tstnote("matched pippo %d",skpmatched); }
    ifskp("*w ')' *w") { tstnote("matched closed %d",skpmatched); }
  }
  return skpmatched;
}

skpdef(parpluto) {
  ifskp("*w '(' *w","'pluto'","*w ')' *w") { tstnote("matched pluto %d '%.*s'", skpmatched, skplen(2), skpfrom[2]); };
  return skpmatched;
}

skpdef(parplippo) {
  ifskp("*w '(' *w") { tstnote("matched open %d",skpmatched); }
  andskp {
    ifskp("'pippo'")   { tstnote("matched pippo %d",skpmatched); }
    elseifskp("'pluto'") { tstnote("matched pluto %d",skpmatched); }
    elseskp  { tstnote("NOTHING");}
  }
  ifskp("*w ')' *w") { tstnote("matched closed %d",skpmatched); }
  return skpmatched;
}

char *from;
char *to;
char *text;
int ret;
tstsuite("seq") {
  tstcase("just seq") {
    text = "( pippo )";
    tstcheck((ret = skp(text,parpippo,&from,&to)) == 1);
    tstcheck((ret = skp(text,parpluto,&from,&to)) == 0);
    tstcheck((ret = skp(text,parplippo,&from,&to)) == 1);
    text = "( pluto )";
    tstcheck((ret = skp(text,parpluto,&from,&to)) == 1);
    tstcheck((ret = skp(text,parpippo,&from,&to)) == 0);
    tstcheck((ret = skp(text,parplippo,&from,&to)) == 1);
  }
  tstcase("just seq") {
    text = " pippo )";
    tstcheck((ret = skp(text,parpippo,&from,&to)) == 0);
    tstcheck((ret = skp(text,parpluto,&from,&to)) == 0);
    tstcheck((ret = skp(text,parplippo,&from,&to)) == 0);
    text = "( pluto ";
    tstcheck((ret = skp(text,parpluto,&from,&to)) == 0);
    tstcheck((ret = skp(text,parpippo,&from,&to)) == 0);
    tstcheck((ret = skp(text,parplippo,&from,&to)) == 0);
    text = "( topolino )";
    tstcheck((ret = skp(text,parpluto,&from,&to)) == 0);
    tstcheck((ret = skp(text,parpippo,&from,&to)) == 0);
    tstcheck((ret = skp(text,parplippo,&from,&to)) == 0);
  }
}