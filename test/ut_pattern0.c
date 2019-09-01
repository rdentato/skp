#include "skp.h"
#include "dbg.h"

int main(int argc, char *argv[])
{
   char *s;
   char *t;

   s="Y X";
   t=skp(s,"Y%,,%*Y");
   dbgchk(t[0] == ' ',"t=[%s]",t);

   s="Y , Y";
   t=skp(s,"Y%,,%*Y");
   dbgchk(t[0] == '\0',"t=[%s]",t);

   s="Y , X";
   t=skp(s,"Y%,,%*Y");
   dbgchk(t[0] == ' ',"t=[%s]",t);

   return 0;

   s="xcxa3";
   t=skp(s,"%*%l");
   dbgchk(t[0]=='3',"t=[%s]",t);

   s="s";
   t=skp(s,"%*%D");
   dbgchk(t[0]=='s',"t=[%s]",t);

}