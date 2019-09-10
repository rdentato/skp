#include "utl.h"

int main(int argc, char *argv[])
{
  char *s;
  char *t;

dbgclk {
 _dbgblk {
   s="XaccccYx";
   t=skp(s,"Xa%(%*c%)Y");
   dbgchk(t && t[0]=='x',"t=[%s]",t);

   s="XaYx";
   t=skp(s,"Xa%(%*c%)Y");
   dbgchk(t && t[0]=='x',"t=[%s]",t);

   s="XaYx";
   t=skp(s,"Xa%(%*c%|*b%)Y");
   dbgchk(t && t[0]=='x',"t=[%s]",t);
   t=skp(s,"Xa%*%(c%|b%)Y");
 }

   s="XabcdYx";
   t=skp(s,"Xa%*%lY");
   dbgchk(t && t[0]=='x',"t=[%s]",t);

   s="XabcdYx";
   t=skp(s,"Xa%?%(%*%l%)Y");
   dbgchk(t && t[0]=='x',"t=[%s]",t);

   s="XaYx";
   t=skp(s,"Xa%?%(%*%l%)Y");
   dbgchk(t && t[0]=='x',"t=[%s]",t);

   s="XaYx";
   t=skp(s,"Xa%*%(%l%)Y");
   dbgchk(t && t[0]=='x',"t=[%s]",t);
}
  return 0;
}