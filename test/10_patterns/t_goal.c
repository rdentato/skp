#include "tst.h"
#define SKP_MAIN
#include "skp.h"
  
skpdef(is_keyword)
{
  *skpfrom = skpstart;
  *skpfrom = skpstart;

       if (strncmp(skpstart,"if",2) == 0)   *skpto += 2;
  else if (strncmp(skpstart,"then",4) == 0) *skpto += 4;
  else if (strncmp(skpstart,"else",4) == 0) *skpto += 4;

  int ret = *skpto > *skpfrom;
  tst_note("is_keyword: %d",ret);
  return ret;
}

tstsuite("Setting a goal") {
  char *text;
  char *from[3];
  char *to[3];
  //int len;
  int ret;
  tstcase("Check goals") {
    text = "(if)";
    tstcheck(ret = skp(text,"S '(' S",skpgoal, "'if'",from,to) == 1,"ret: %d",ret);
    tstcheck(strncmp("if",to[0],2) == 0);

    tstcheck(ret = skp(text,"S '(' S",skpnot, "'else'",from,to) == 1,"ret: %d",ret);
    tstcheck(strncmp("if",to[0],2) == 0);

    tstcheck(ret = skp(text,"S '(' S",skpnot, "'if'","d",from,to) == 1,"ret: %d",ret);
    tstcheck(strncmp("if",to[0],2) == 0);

    tstcheck(ret = skp(text,"S '(' S",skpnot, "'if'",from,to) == 0,"ret: %d",ret);

  }    
}