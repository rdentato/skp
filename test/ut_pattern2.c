#include "skp.h"
#include "dbg.h"

int main(int argc, char *argv[])
{
   char *s;
   char *t;

   s="Pixo";
   t=skp(s,"%!P%a");
   dbgchk(t == NULL,"t=[%s]",t);

   s="pixo";
   t=skp(s,"%!P%.");
   dbgchk(t && t[0] == 'i',"t=[%s]",t);

   s="pixo";
   t=skp(s,"%[spx%]");
   dbgchk(t && t[0] == 'i',"t=[%s]",t);

   s="pixo";
   t=skp(s,"%[so-qx%]%[aeiouàèìòù%]");
   dbgchk(t && t[0] == 'x',"t=[%s]",t);

   s="pìxo";
   t=skp(s,"%[so-qx%]%[aeiouàèìòù%]");
   dbgchk(t && t[0] == 'x',"t=[%s]",t);

}