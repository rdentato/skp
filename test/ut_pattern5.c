#include "skp.h"
#include "dbg.h"

int main(int argc, char *argv[])
{
  char *s;
  char *t;

  s="xy";
  t=skp(s,"%.");
  dbgchk(t && t[0] == 'y',"t=[%s]",t);

  s="dd";
  t=skp(s,"4");
  dbgchk(t == NULL,"t=[%s]",t);

  return 0;
}