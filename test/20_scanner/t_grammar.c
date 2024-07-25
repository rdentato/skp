#include "tst.h"
#define SKP_MAIN
#include "skp.h"
  
char *text = "% asdsa\n"
             " p\n";
 
tstsuite("User defined functions") {
#if 0
 // char *to;
  int len;
  int cnt;
// int ret;
  tstcase("scan identifiers") {
    skpscan(text) {
      skpcase("'%%' ")
    }
  }
#endif
}