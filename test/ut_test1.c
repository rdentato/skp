#include <stdio.h>

#define SKP_MAIN
#include "skp.h"

int main(int argc, char *argv[])
{
  int alt;
  char *to;
  char *from;
  int len;

  from = "123X";

  alt = skp(from,"D\2",&to); len = (int)(to-from);
  skptrace("alt: %d '%.*s'",alt,len,from);
  skptest(alt == 2 && len == 3, "alt: %d len: %d",alt,len);

  alt = skp(from,"I\2",&to); len = (int)(to-from);
  skptrace("alt: %d '%.*s'",alt,len,from);
  skptest(alt == 0 && len == 0, "alt: %d len: %d",alt,len);

  alt = skp(from,"'1'\2",&to); len = (int)(to-from);
  skptrace("alt: %d '%.*s'",alt,len,from);
  skptest(alt == 2 && len == 1, "alt: %d len: %d",alt,len);

  alt = skp(from,"'2'\2",&to); len = (int)(to-from);
  skptrace("alt: %d '%.*s'",alt,len,from);
  skptest(alt == 0 && len == 0, "alt: %d len: %d",alt,len);

  alt = skp(from,"'12'\2",&to); len = (int)(to-from);
  skptrace("alt: %d '%.*s'",alt,len,from);
  skptest(alt == 2 && len == 2, "alt: %d len: %d",alt,len);

  alt = skp(from,"'13'\2",&to); len = (int)(to-from);
  skptrace("alt: %d '%.*s'",alt,len,from);
  skptest(alt == 0 && len == 0, "alt: %d len: %d",alt,len);

  alt = skp(from,"?'12'\3",&to); len = (int)(to-from);
  skptrace("alt: %d '%.*s'",alt,len,from);
  skptest(alt == 3 && len == 2, "alt: %d len: %d",alt,len);

  alt = skp(from,"?'23'\3",&to); len = (int)(to-from);
  skptrace("alt: %d '%.*s'",alt,len,from);
  skptest(alt == 3 && len == 0, "alt: %d len: %d",alt,len);

  alt = skp(from,"!'12'\4",&to); len = (int)(to-from);
  skptrace("alt: %d '%.*s'",alt,len,from);
  skptest(alt == 0 && len == 0, "alt: %d len: %d",alt,len);

  alt = skp(from,"!'23'\4",&to); len = (int)(to-from);
  skptrace("alt: %d '%.*s'",alt,len,from);
  skptest(alt == 4 && len == 0, "alt: %d len: %d",alt,len);

  alt = skp(from,"'1'.\2",&to); len = (int)(to-from);
  skptrace("alt: %d '%.*s'",alt,len,from);
  skptest(alt == 2 && len == 2, "alt: %d len: %d",alt,len);

  alt = skp(from,"'1'..a\2",&to); len = (int)(to-from);
  skptrace("alt: %d '%.*s'",alt,len,from);
  skptest(alt == 2 && len == 4, "alt: %d len: %d",alt,len);

  alt = skp(from,"'1' . . a\2",&to); len = (int)(to-from);
  skptrace("alt: %d '%.*s'",alt,len,from);
  skptest(alt == 2 && len == 4, "alt: %d len: %d",alt,len);

}