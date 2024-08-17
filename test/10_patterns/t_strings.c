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

  tstcase("Simple string") {
    text = "pippo";
    tstcheck(skp(text,"'pippo'",&to) == 1);
    tstcheck(skp(text,"'pippoxx'",&to) == 0);

    tstcheck(skp(text,"'pluto'",&to) == 0);
    
  }

  tstcase("Simple string") {
    text = "pippo";
    tstcheck(skp(text,"`pippo",&to) == 1);
    tstcheck(skp(text,"`pippoxx",&to) == 0);

    tstcheck(skp(text,"`pluto",&to) == 0);
    
  }

  tstcase("Simple string") {
    char *from;
    text = "xy3";
    tstcheck(skp(text,"?`xy","d",&from, &to) == 1);

    text = "4";
    tstcheck(skp(text,"?`xy","d",&from, &to) == 1);
    
    text = "av";
    tstcheck(skp(text,"!`xy","ll",&from, &to) == 1);

    text = "xy";
    tstcheck(skp(text,"!`xy","ll",&from, &to) == 0);
  }

}