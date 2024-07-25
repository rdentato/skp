#include "tst.h"
#define SKP_MAIN
#include "skp.h"
  
tstsuite("Unicode and ISO text") {
  char *text;
  char *from;
  char *to;
  int len;
  tstcase("Check from and to when matching") {
    text = "abc";
    tstcheck(skp(text,"+a",&from,&to) == 1);
    tstcheck((len = (int)(to-from)) == 3);
    
    from = (char *)(&from);
    to = (char *)(&to);
    tstcheck(skp(text,"+d\7'abc'\3",&from,&to) == 3);
    tstcheck(from != (char *)(&from));
    tstcheck(to != (char *)(&to));
    tstcheck((len = (int)(to-from)) == 3);

    text = "3abc";
    tstcheck(skp(text,">+a",&from,&to) == 1);
    tstcheck((len = (int)(to-from)) == 3);
    tstcheck((text < from));

    text = "abc/";
    tstcheck(skp(text,"+a",&from,&to) == 1);
    tstcheck((len = (int)(to-from)) == 3);
    
    text = "3abc/";
    tstcheck(skp(text,">+a",&from,&to) == 1);
    tstcheck((len = (int)(to-from)) == 3);
    tstcheck((text < from));
  }

  tstcase("Check from and to when NOT matching") {
    from = (char *)(&from);
    to = (char *)(&to);
    text = "abc";
    tstcheck(skp(text,"+d",&from,&to) == 0);
    tstcheck(from == (char *)(&from));
    tstcheck(to == (char *)(&to));

    tstcheck(skp(text,"+d\7'pippo'",&from,&to) == 0);
    tstcheck(from == (char *)(&from));
    tstcheck(to == (char *)(&to));

  }
}