#include "utl.h"

int main(int argc, char *argv[])
{
  char *s;
  char *t;

  skpcapt_t *capt = skpcaptnew(4);

  s="pY";
  t=skp(s,"%[m-q%]",capt);
  dbgchk(t && t[0]=='Y',"t=[%s]",t);

  s="pY";
  t=skp(s,"%[^m-q%]",capt);
  dbgchk(t == NULL,"t=[%s]",t);

  s="aY";
  t=skp(s,"%[^m-q%]",capt);
  dbgchk(t && t[0]=='Y',"t=[%s]",t);

  s="aY";
  t=skp(s,"%[m-q%]",capt);
  dbgchk(t == NULL,"t=[%s]",t);

  skpcaptfree(capt);

  return 0;
}
