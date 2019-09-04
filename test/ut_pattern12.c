#include "skp.h"
#include "dbg.h"

int main(int argc, char *argv[])
{
  char *s;
  char *t;

  // ISO 8859-15 (Latin-9)

  s="aèbòÿY";
  t=skp(s,"%I9%Caèbòÿ");
  dbgchk(t && t[0]=='Y',"t=[%s]",t);

  s="AÈBÒ?Y";
  t=skp(s,"%I9%Caèbòÿ");
  dbgchk(t && t[0]=='Y',"t=[%s]",t);

  s="????Y";
  t=skp(s,"%I9%C???ÿ");
  dbgchk(t && t[0]=='Y',"t=[%s]",t);

  s="????-";
  t=skp(s,"%I9%+%u");
  dbgchk(t && t[0]=='-',"t=[%s]",t);

  s="???ÿ-";
  t=skp(s,"%I9%+%l");
  dbgchk(t && t[0]=='-',"t=[%s]",t);

  s="???ÿ????-";
  t=skp(s,"%I9%+%a");
  dbgchk(t && t[0]=='-',"t=[%s]",t);


  return 0;
}
