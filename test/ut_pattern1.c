#include "skp.h"
#include "dbg.h"

int main(int argc, char *argv[])
{
   char *s;
   char *t;

   s="pippo";
   t=skp(s,"p");
   dbgchk(t && t[0] == 'i',"t=[%s]",t);

   s="pippo";
   t=skp(s,"pi%^xp");
   dbgchk(t && t[0] == 'o',"t=[%s]",t);

   s="pluto";
   t=skp(s,"p%.");
   dbgchk(t && t[0] == 'u',"t=[%s]",t);

   s="pluto";
   t=skp(s,"p%a");
   dbgchk(t && t[0] == 'u',"t=[%s]",t);

   s="płuto";  // it's not an 'l'
   t=skp(s,"p%.");
   dbgchk(t && t[0] == 'u',"t=[%s]",t);

   s="płuto";  // it's not an 'l'
   t=skp(s,"p%a");
   dbgchk(t && t[0] == 'u',"t=[%s]",t);

   s="płuto";  // it's not an 'l'
   t=skp(s,"p%^%a");
   dbgchk(t == NULL,"t=[%s]",t);

   s="plùto";  // it's not an 'u' but is Latin-1!!
   t=skp(s,"pl%.");
   dbgchk(t && t[0] == 't',"t=[%s]",t);

   s="plùto";  // it's not an 'u' but is Latin-1!!
   t=skp(s,"pl%a");
   dbgchk(t && t[0] == 't',"t=[%s]",t);

   s="plùto";  // it's not an 'u' but is Latin-1!!
   t=skp(s,"pl%^%a");
   dbgchk(t == NULL,"t=[%s]",t);

   s="pippo";
   t=skp(s,"pi%+p");
   dbgchk(t && t[0] == 'o',"t=[%s]",t);
#if  1
   s="pIpPo";
   t=skp(s,"pipp");
   dbgchk(t == NULL,"t=[%s]",t);

   s="pIpPo";
   t=skp(s,"%Cpipp");
   dbgchk(t && t[0] == 'o',"t=[%s]",t);

   s="pIpPo";
   t=skp(s,"%cpipp");
   dbgchk(t == NULL,"t=[%s]",t);

   s="pipp0";
   t=skp(s,"%+%a");
   dbgchk(t && t[0] == '0',"t=[%s]",t);

   s="pippo";
   t=skp(s,"pi%?pp");
   dbgchk(t && t[0] == 'o',"t=[%s]",t);
   
   s="pipo";
   t=skp(s,"pip%?p");
   dbgchk(t && t[0] == 'o',"t=[%s]",t);
 
   s="'pippo'X";
   t=skp(s,"%Q");
   dbgchk(t && t[0] == 'X',"t=[%s]",t);

   s="   'pippo'X";
   t=skp(s,"%Q");
   dbgchk(t == NULL,"t=[%s]",t);

   s="   'pippo'X";
   t=skp(s,"%_%Q");
   dbgchk(t && t[0] == 'X',"t=[%s]",t);

   s="'pippo'X";
   t=skp(s,"%_%Q");
   dbgchk(t && t[0] == 'X',"t=[%s]",t);

   s="X'pippo'Y";
   t=skp(s,"X%?%Q");
   dbgchk(t && t[0] == 'Y',"t=[%s]",t);

   s="XY";
   t=skp(s,"X%?%Q");
   dbgchk(t && t[0] == 'Y',"t=[%s]",t);

   s="abc 3ef";
   t=skp(s,"%+%a%_");
   dbgchk(t && t[0] == '3',"t=[%s]",t);

   s="abc3ef";
   t=skp(s,"%+%a%_");
   dbgchk(t && t[0] == '3',"t=[%s]",t);

#if 0
   char *q;
   s="abcd";
   t=skp(s,"%>c",&q);
   dbgchk(t && t[0] == 'd' && q[0] == 'c',"t=[%s] q=[%s]",t,q);
#endif 

   s="-3.2";
   t=skp(s,"%F");
   dbgchk(t>s && *t=='\0',"t=[%s]",t);

   s="+.2";
   t=skp(s,"%F");
   dbgchk(t>s && *t=='\0',"t=[%s]",t);
 
   s="+3.";
   t=skp(s,"%F");
   dbgchk(t>s && *t=='\0',"t=[%s]",t);
 
   s="+1.2E-3.4";
   t=skp(s,"%F");
   dbgchk(t>s && *t=='\0',"t=[%s]",t);
 
   s="+.2E-3.4";
   t=skp(s,"%F");
   dbgchk(t>s && *t=='\0',"t=[%s]",t);
 
   s="+1.E-3.4";
   t=skp(s,"%F");
   dbgchk(t>s && *t=='\0',"t=[%s]",t);
 
   s="+.E-3.4";
   t=skp(s,"%F");
   dbgchk(t == NULL,"t=[%s]",t);

   s="56 92 XX 21";
   t=skp(s,"%+%(%_%D%_%)");
   dbgchk(t && t[0] == 'X',"t=[%s]",t);
 
   s="X92Xy";
   t=skp(s,"X%?%DX");
   dbgchk(t && t[0] == 'y',"t=[%s]",t);
 
   s="XXy";
   t=skp(s,"X%?%DX");
   dbgchk(t && t[0] == 'y',"t=[%s]",t);
 
#endif 
}