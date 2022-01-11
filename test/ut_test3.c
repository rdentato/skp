#include <stdio.h>

#define SKP_MAIN
#include "skp.h"

int main(int argc, char *argv[])
{
  int alt;
  char *to;
  char *from;
  int len;
  from = "ABC";

  alt = skp(from,"'AB'",&to); len = (int)(to-from);
  skptrace("alt: %d '%.*s'",alt,len,from);
  skptest(alt == 1 && len == 2, "alt: %d len: %d",alt,len);

  alt = skp(from,"'XB'",&to);len = (int)(to-from);
  skptrace("alt: %d '%.*s'",alt,len,from);
  skptest(alt == 0 && len == 0, "alt: %d len: %d",alt,len);

  alt = skp(from,"'XB" "\xE" "AB'",&to); len = (int)(to-from);
  skptrace("alt: %d '%.*s'",alt,len,from);
  skptest(alt == 1 && len == 2, "alt: %d len: %d",alt,len);

  alt = skp(from,"'AB" "\xE" "XB'",&to); len = (int)(to-from);
  skptrace("alt: %d '%.*s'",alt,len,from);
  skptest(alt == 1 && len == 2, "alt: %d len: %d",alt,len);


  from = "12cm";
  alt = skp(from,"D @ 'cm\xEmm\xEpt'",&to); len = (int)(to-from);
  skptrace("alt: %d '%.*s'",alt,len,from);
  skptest(alt == 1 && len == 2, "alt: %d len: %d",alt,len);

  from = "12mm";
  alt = skp(from,"D @ 'cm\xEmm\xEpt'",&to); len = (int)(to-from);
  skptrace("alt: %d '%.*s'",alt,len,from);
  skptest(alt == 1 && len == 2, "alt: %d len: %d",alt,len);

  from = "12XX";
  alt = skp(from,"D @ 'cm\xEmm\xEpt'",&to); len = (int)(to-from);
  skptrace("alt: %d '%.*s'",alt,len,from);
  skptest(alt == 0 && len == 0, "alt: %d len: %d",alt,len);

  from = "12ptcm";
  alt = skp(from,"D @ 'cm\xEmm\xEpt'",&to); len = (int)(to-from);
  skptrace("alt: %d '%.*s'",alt,len,from);
  skptest(alt == 1 && len == 2, "alt: %d len: %d",alt,len);

}