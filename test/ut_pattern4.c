#include "skp.h"
#include "dbg.h"

int main(int argc, char *argv[])
{
   char *s;
   char *t;

   _dbgclk {

   s="ax";
   t=skp(s,"a%|b");
   dbgchk(t && t[0]=='x',"t=[%s]",t);

   s="bx";
   t=skp(s,"a%|b");
   dbgchk(t && t[0]=='x',"t=[%s]",t);

   s="pippox";
   t=skp(s,"pluto%|pippo");
   dbgchk(t && t[0]=='x',"t=[%s]",t);

   s="XpippoYx";
   t=skp(s,"X%(pluto%|pi%(%*p%)o%)Y");
   dbgchk(t && t[0]=='x',"t=[%s]",t);

   s="XpioYx";
   t=skp(s,"X%(pluto%|pi%*%(p%)o%)Y");
   dbgchk(t && t[0]=='x',"t=[%s]",t);

   s="XacccYx";
   t=skp(s,"X%(a%|b%)%?%(%+c%)Y");
   dbgchk(t && t[0]=='x',"t=[%s]",t);

   s="XaYx";
   t=skp(s,"X%(a%|b%)%?%(%+c%)Y");
   dbgchk(t && t[0]=='x',"t=[%s]",t);

   s="XaYx";
   t=skp(s,"X%(a%|b%)%*%(c%)Y");
   dbgchk(t && t[0]=='x',"t=[%s]",t);

   s="XaccccYx";
   t=skp(s,"X%(a%|b%)%*%(c%)Y");
   dbgchk(t && t[0]=='x',"t=[%s]",t);

   s="XaYx";
   t=skp(s,"X%(a%|b%)%(%*c%)Y");
   dbgchk(t && t[0]=='x',"t=[%s]",t);

   s="XaccccYx";
   t=skp(s,"X%(a%|b%)%(%*c%)Y");
   dbgchk(t && t[0]=='x',"t=[%s]",t);
   }
   return 0;
}