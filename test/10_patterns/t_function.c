#include "tst.h"
#define SKP_MAIN
#include "skp.h"
  
skprecognizer(is_keyword)
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


tstsuite("User defined functions") {
  char *text;
  char *to;
  int len;
  int ret;
  tstcase("Single char") {
    text = "if"; to = text;
    tstcheck(((ret = skp(text,is_keyword,&to)) == 1), "Expected: 1 got: %d",ret);
    tstcheck((len = (int)(to-text)) == 2);
    text = "else"; to = text;
    tstcheck(((ret = skp(text,is_keyword,&to)) == 1), "Expected: 1 got: %d",ret);
    tstcheck((len = (int)(to-text)) == 4);
    text = "not";
    tstcheck(((ret = skp(text,is_keyword,&to)) == 0), "Expected: 0 got: %d",ret);
    text = "";
    tstcheck(((ret = skp(text,is_keyword,&to)) == 0), "Expected: 0 got: %d",ret);
    text = NULL;
    tstcheck(((ret = skp(text,is_keyword,&to)) == 0), "Expected: 0 got: %d",ret);
  }

}