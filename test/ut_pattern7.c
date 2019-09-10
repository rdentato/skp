#include "utl.h"

int main(int argc, char *argv[])
{
  char *s;
  char *t;

dbgclk {
  s="XababYx";
  t=skp(s,"X%(%+%l%)Y");
  dbgchk(t && t[0]=='x',"t=[%s]",t);
}

dbgclk {
  s="XababYx";
  t=skp(s,"X%+%(%l%)Y");
  dbgchk(t && t[0]=='x',"t=[%s]",t);
}

dbgclk {
  s="XababYx";
  t=skp(s,"X%+%(ab%)Y");
  dbgchk(t && t[0]=='x',"t=[%s]",t);
}

  s="Xa1a2Yx";
  t=skp(s,"X%+%(a%d%)Y");
  dbgchk(t && t[0]=='x',"t=[%s]",t);

  s="Xa1a2Yx";
  t=skp(s,"X%+%(a%(%d%)%)Y");
  dbgchk(t && t[0]=='x',"t=[%s]",t);

  s="Xa1a1a1a2a3a4a5a6a7a8a9Yx";
  t=skp(s,"X%+%(a%(%d%)%)Y");
  dbgchk(t && t[0]=='x',"t=[%s]",t);

  skpcapt_t *capt = skpcaptnew(30);
  s="Xa1a1a1a2a3a4a5a6a7a8a9Yx";
  t=skp(s,"X%+%(a%(%d%)%)Y",capt);
  dbgchk(t && t[0]=='x',"t=[%s]",t);

  s="XaabbbYx";
  t=skp(s,"X%+%(a%)%+%(b%)Y",capt);
  dbgchk(t && t[0]=='x',"t=[%s]",t);



  skpcaptfree(capt);

  capt = skpcaptnew(1);
  s="Xa1a1a1a2a3a4a5a6a7a8a9Yx";
  t=skp(s,"X%+%(a%(%d%)%)Y",capt);
  dbgchk(t && t[0]=='x',"t=[%s]",t);

  s="XaabbbYx";
  t=skp(s,"X%+%(a%)%+%(b%)Y",capt);
  dbgchk(t && t[0]=='x',"t=[%s]",t);

  skpcaptfree(capt);

  return 0;
}