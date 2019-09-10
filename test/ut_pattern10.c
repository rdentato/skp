#include "utl.h"

int main(int argc, char *argv[])
{
  char *s;
  char *t;

  skpcapt_t *capt = skpcaptnew(4);

  s="aèbŏÿY";
  t=skp(s,"%Caèbŏÿ",capt);
  dbgchk(t && t[0]=='Y',"t=[%s]",t);

  s="AÈBŎŸY";
  t=skp(s,"%Caèbŏÿ",capt);
  dbgchk(t && t[0]=='Y',"t=[%s]",t);

  skpcaptfree(capt);

  return 0;
}
