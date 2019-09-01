#include "skp.h"
#include "dbg.h"

int main(int argc, char *argv[])
{
  char *s;
  char *t;

  skpcapt_t *capt = skpcaptnew(4);

  s="pippo  ,pippoYX";
  t=skp(s,"%(%+%l%)%_,%_%1Y",capt);
  dbgchk(t && t[0]=='X',"t=[%s]",t);

  s="YpippoY\nXplutoX";
  t=skp(s,"%u%*%l%u%RX",capt);
  dbgchk(t && t[0]=='p',"t=[%s]",t);

  s="YpippoYXplutoX";
  t=skp(s,"%u%*%l%u%?%RX",capt);
  dbgchk(t && t[0]=='p',"t=[%s]",t);

  skpcaptfree(capt);

  return 0;
}
