#include "utl.h"

int main(int argc, char *argv[])
{
   char *s;
   char *t;

   s="xcka3";
   t=skp(s,"%l%(cka%)%d");
   dbgchk(t && t[0]=='\0',"t=[%s]",t);

   s="x3";
   t=skp(s,"%l%?%(cka%)%d");
   dbgchk(t && t[0]=='\0',"t=[%s]",t);

   s="xababab3";
   t=skp(s,"%l%+%(ab%)%d");
   dbgchk(t && t[0]=='\0',"t=[%s]",t);

   s="12m";
   t=skp(s,"%+%d%_m%?%(eter%)");
   dbgchk(t && t[0]=='\0',"t=[%s]",t);

   s="12 meters";
   t=skp(s,"%+%d%_m%?%(eter%?%(s%)%)");
   dbgchk(t && t[0]=='\0',"t=[%s]",t);

   s="12meter";
   t=skp(s,"%+%d%_m%?%(eter%?%(s%)%)");
   dbgchk(t && t[0]=='\0',"t=[%s]",t);

   return 0;

}