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

tstsuite("Multiple patterns") {
  char *text;
  char *from[3];
  char *to[3];
  int len;
  int ret;
  tstcase("Check multiple patterns") {
    text = "(if)";
    tstcheck(ret = skp(text,"S '(' S","'if'",from,to) == 1,"ret: %d",ret);
    tstcheck((len = to[0] - from[0]) == 3,"len: %d %p %p",len, (void*)(from[0]), (void*)(to[0]));
    tstcheck((len = to[1] - from[1]) == 1,"len: %d %p %p",len, (void*)(from[0]), (void*)(to[0]));
    tstcheck((len = to[2] - from[2]) == 2,"len: %d %p %p",len, (void*)(from[0]), (void*)(to[0]));
    tstcheck(strncmp("if",from[2],2) == 0);
    
    tstcheck(ret = skp(text,"S '(' S",is_keyword, "S ')' S", from,to) == 1,"ret: %d",ret);
    text = "( if ) ";
    tstcheck(ret = skp(text,"S '(' S",is_keyword, "S ')' S", from,to) == 1,"ret: %d",ret);
    tstcheck(strncmp("if",from[2],2) == 0);

    text = "( else) ";
    tstcheck(ret = skp(text,"S '(' S",is_keyword, "S ')' S", from,to) == 1,"ret: %d",ret);
    tstcheck(strncmp("else",from[2],4) == 0);
  }    

  tstcase("Numeric") {
    text = "123pippo321x";
    tstcheck(ret = skp(text,3,"'pippo'", 3, "'x'",from,to) == 1,"ret: %d",ret);
  }

  tstcase("Not match") {
    text = "532";
    from[0] = (char *)from;
    tstcheck(ret = skp(text,"S '(' S",is_keyword, "S ')' S", from,to) == 0,"ret: %d",ret);
    tstcheck(from[0] == (char *)from);

  }

}