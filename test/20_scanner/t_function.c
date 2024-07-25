#include "tst.h"
#define SKP_MAIN
#include "skp.h"
  


skpscanner(string) {

  skpcase("+w") { tstnote("spaces: %.*s",skplen,skpfrom); }  
  skpcase("Q")  { tstnote("string: %.*s",skplen,skpfrom); }
  
  skpreturn(1);
}


tstsuite("function scanners") {
 // char *to;
  int len;
  int cnt;
// int ret;
  tstcase("scan identifiers") {
    skp("  'asd' 'cde'",string);
  }
}