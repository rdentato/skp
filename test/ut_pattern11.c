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

  s="AÈBÒ¾Y";
  t=skp(s,"%I9%Caèbòÿ");
  dbgchk(t && t[0]=='Y',"t=[%s]",t);

  s="¦´¼¾Y";
  t=skp(s,"%I9%C¨¸½ÿ");
  dbgchk(t && t[0]=='Y',"t=[%s]",t);

  s="¦´¼¾-";
  t=skp(s,"%I9%+%u");
  dbgchk(t && t[0]=='-',"t=[%s]",t);

  s="¨¸½ÿ-";
  t=skp(s,"%I9%+%l");
  dbgchk(t && t[0]=='-',"t=[%s]",t);

  s="¨¸½ÿ¦´¼¾-";
  t=skp(s,"%I9%+%a");
  dbgchk(t && t[0]=='-',"t=[%s]",t);


  return 0;
}
