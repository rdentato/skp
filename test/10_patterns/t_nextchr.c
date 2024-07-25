#include "tst.h"
#define SKP_MAIN
#include "skp.h"
  
tstsuite("Unicode and ISO text") {
  char *text;
  char *to;
  int len;
  int ret;
  tstcase("Single char") {
    text = "è"; // Encoded in two bytes
    tstcheck(skp(text,".",&to) == 1);
    tstcheck((len = (int)(to-text)) == 2);
    
    tstcheck(skp(text,"!U.",&to) == 1);
    tstcheck((len = (int)(to-text)) == 1);
  }
  tstcase("A set number of chars (ASCII)") {
    text = "abc";
    to = "X";
    tstcheck((ret = skp(text,0,&to)) == 1,"ret: %d",ret);
    tstcheck(*to == 'a');
    tstcheck((ret = skp(text,1,&to)) == 1,"ret: %d",ret);
    tstcheck(*to == 'b');
    tstcheck((ret = skp(text,2,&to)) == 1,"ret: %d",ret);
    tstcheck(*to == 'c');
    tstcheck((ret = skp(text,3,&to)) == 1,"ret: %d",ret);
    tstcheck(*to == '\0');
    to = "X";
    tstcheck((ret = skp(text,4,&to)) == 0,"ret: %d",ret);
    tstcheck(*to == 'X');
  }
  tstcase("A set number of chars (ISO ASCII)") {
    text = "abc";
    to = "X";
    tstcheck((ret = skp(text,0,&to)) == 1,"ret: %d",ret);
    tstcheck(*to == 'a');
    tstcheck((ret = skp(text,-1,&to)) == 1,"ret: %d",ret);
    tstcheck(*to == 'b');
    tstcheck((ret = skp(text,-2,&to)) == 1,"ret: %d",ret);
    tstcheck(*to == 'c');
    tstcheck((ret = skp(text,-3,&to)) == 1,"ret: %d",ret);
    tstcheck(*to == '\0');
    to = "X";
    tstcheck((ret = skp(text,-4,&to)) == 0,"ret: %d",ret);
    tstcheck(*to == 'X');
  }
  tstcase("A set number of chars (UTF-8 2 bytes)") {
    text = "àbç";
    to = "X";
    tstcheck((ret = skp(text,0,&to)) == 1,"ret: %d",ret);
    tstcheck(strncmp(to,"à",2) == 0);
    tstcheck((ret = skp(text,1,&to)) == 1,"ret: %d",ret);
    tstcheck(strncmp(to,"b",1) == 0);
    tstcheck((ret = skp(text,2,&to)) == 1,"ret: %d",ret);
    tstcheck(strncmp(to,"ç",2) == 0);
    tstcheck((ret = skp(text,3,&to)) == 1,"ret: %d",ret);
    tstcheck(*to == '\0');
    to = "X";
    tstcheck((ret = skp(text,4,&to)) == 0,"ret: %d",ret);
    tstcheck(*to == 'X');
  }
  tstcase("A set number of chars (ISO on UTF-8 2 bytes)") {
    text = "àbç";
    to = "X";
    tstcheck((ret = skp(text,0,&to)) == 1,"ret: %d",ret);
    tstcheck(*to == "à"[0]);
    tstcheck((ret = skp(text,-1,&to)) == 1,"ret: %d",ret);
    tstcheck(*to == "à"[1], "*to = %02X",*to);
    tstcheck((ret = skp(text,-2,&to)) == 1,"ret: %d",ret);
    tstcheck(*to == "b"[0]);
    tstcheck((ret = skp(text,-3,&to)) == 1,"ret: %d",ret);
    tstcheck(*to == "ç"[0]);
    tstcheck((ret = skp(text,-4,&to)) == 1,"ret: %d",ret);
    tstcheck(*to == "ç"[1]);
    tstcheck((ret = skp(text,-5,&to)) == 1,"ret: %d",ret);
    tstcheck(*to == '\0');
    to = "X";
    tstcheck((ret = skp(text,-6,&to)) == 0,"ret: %d",ret);
    tstcheck(*to == 'X');
  }
}