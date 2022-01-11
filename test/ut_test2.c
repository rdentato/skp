#include <stdio.h>

#define SKP_MAIN
#include "skp.h"

int main(int argc, char *argv[])
{
  int alt;
  char *to;
  char *from;
  int len;

  from = "A&B";

  alt = skp(from+1,"&\3",&to); len = (int)(to-(from+1));
  skptrace("alt:%d '%.*s'",alt,len,from+1);
  skptest(alt == 0 && len == 0, "alt: %d len: %d",alt,len);

  alt = skp(from,"A&&B\2",&to); len = (int)(to-from);
  skptrace("alt:%d '%.*s'",alt,len,from);
  skptest(alt == 0 && len == 0, "alt: %d len: %d",alt,len);

  from = "abCD";

  alt = skp(from,"'abCD'",&to); len = (int)(to-from);
  skptrace("alt:%d '%.*s'",alt,len,from);
  skptest(alt == 1 && len == 4, "alt: %d len: %d",alt,len);

  alt = skp(from,"'abcd'",&to); len = (int)(to-from);
  skptrace("alt:%d '%.*s'",alt,len,from);
  skptest(alt == 0 && len == 0, "alt: %d len: %d",alt,len);

  alt = skp(from,"!C'abcd'",&to); len = (int)(to-from);
  skptrace("alt:%d '%.*s'",alt,len,from);
  skptest(alt == 1 && len == 4, "alt: %d len: %d",alt,len);

  from = "aèi";

  alt = skp(from,"'a' . 'i'",&to); len = (int)(to-from);
  skptrace("alt:%d '%.*s'",alt,len,from);
  skptest(alt == 1 && len == 4, "alt: %d len: %d",alt,len);

  alt = skp(from,". [èì] .",&to); len = (int)(to-from);
  skptrace("alt:%d '%.*s'",alt,len,from);
  skptest(alt == 1 && len == 4, "alt: %d len: %d",alt,len);

  alt = skp(from,"'aè'\2 .",&to); len = (int)(to-from);
  skptrace("alt:%d '%.*s'",alt,len,from);
  skptest(alt == 2 && len == 3, "alt: %d len: %d",alt,len);

  char *end;
  alt = skp(from,"> 'è'",&to,&end); len = (int)(end-to);
  skptrace("alt:%d '%.*s'",alt,len,to);
  skptest(alt == 1 && len == 2, "alt: %d len: %d",alt,len);

}