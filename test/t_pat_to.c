#include "tst.h"

#define SKP_MAIN
#include "skp.h"

tstsuite("Check from/to") {
  int alt;
  char *to;
  char *from;
  int len;

  from = "A&B";

  alt = skp(from,"A&&B\2",&to); len = (int)(to-from);
  skptrace("alt:%d '%.*s'",alt,len,from);
  tstcheck(alt == 0 && len == 0, "alt: %d len: %d",alt,len);

  from = "abCD";

  alt = skp(from,"'abCD'",&to); len = (int)(to-from);
  skptrace("alt:%d '%.*s'",alt,len,from);
  tstcheck(alt == 1 && len == 4, "alt: %d len: %d",alt,len);

  alt = skp(from,"'abcd'",&to); len = (int)(to-from);
  skptrace("alt:%d '%.*s'",alt,len,from);
  tstcheck(alt == 0 && len == 0, "alt: %d len: %d",alt,len);

  alt = skp(from,"!C'abcd'",&to); len = (int)(to-from);
  skptrace("alt:%d '%.*s'",alt,len,from);
  tstcheck(alt == 1 && len == 4, "alt: %d len: %d",alt,len);

  from = "aèi";

  alt = skp(from,"'a' . 'i'",&to); len = (int)(to-from);
  skptrace("alt:%d '%.*s'",alt,len,from);
  tstcheck(alt == 1 && len == 4, "alt: %d len: %d",alt,len);

  alt = skp(from,". [èì] .",&to); len = (int)(to-from);
  skptrace("alt:%d '%.*s'",alt,len,from);
  tstcheck(alt == 1 && len == 4, "alt: %d len: %d",alt,len);

  alt = skp(from,"'aè'\2 .",&to); len = (int)(to-from);
  skptrace("alt:%d '%.*s'",alt,len,from);
  tstcheck(alt == 2 && len == 3, "alt: %d len: %d",alt,len);

  char *end;
  alt = skp(from,"> 'è'",&to,&end); len = (int)(end-to);
  skptrace("alt:%d '%.*s'",alt,len,to);
  tstcheck(alt == 1 && len == 2, "alt: %d len: %d",alt,len);

  alt = skp(from,"> 'è'",&to); len = (int)(to-from);
  skptrace("alt:%d '%.*s'",alt,len,to);
  tstcheck(alt == 1 && len == 3, "alt: %d len: %d",alt,len);


}