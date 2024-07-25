#include "tst.h"
#define SKP_MAIN
#include "skp.h"
  
skprecognizer(singleq)
{
   if (*skpstart != '\'') return 0;
   else return skp(skpstart,"Q",skpfrom, skpto);
}

skprecognizer(doubleq)
{
   if (*skpstart != '"') return 0;
   else return skp(skpstart,"Q",skpfrom, skpto);
}

tstsuite("Quoted strings") {
  char *text;
  char *to;
  int len;
  tstcase("Single quote") {
    text = "'ab'";
    tstcheck(skp(text,"Q",&to)== 1);
    tstcheck((len = (int)(to-text)) == 4);
    text = "`ab`";
    tstcheck(skp(text,"Q",&to)== 1);
    tstcheck((len = (int)(to-text)) == 4);
    text = "\"ab\"";
    tstcheck(skp(text,"Q",&to)== 1);
    tstcheck((len = (int)(to-text)) == 4);
    text = "ab";
    tstcheck(skp(text,"!Q.",&to)== 1);

    text = "'ab'" ;
    tstcheck(skp(text,singleq,&to) == 1);
    tstcheck((len = (int)(to-text)) == 4);
    tstcheck(skp(text,doubleq,&to) == 0);

    text = "\"ab\"";
    tstcheck(skp(text,doubleq,&to) == 1);
    tstcheck((len = (int)(to-text)) == 4);
    tstcheck(skp(text,singleq,&to) == 0);
  }

}