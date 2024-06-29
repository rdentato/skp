#include "tst.h"
#define SKP_MAIN
#include "skp.h"
  

tstsuite("Basic Patterns")
{
  int alt;
  char *from = "123X";
  char *to;
  int len;

  tstcase("alternatives") {
    alt = skp(from,"D\2",&to); len = (int)(to-from);
  
    tstcheck(alt == 2 && len == 3, "alt: %d len: %d",alt,len);
  }
}