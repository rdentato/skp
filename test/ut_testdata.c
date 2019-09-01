#include <stdio.h>
#include "skp.h"
#include "dbg.h"

char buf[1024];
char pat[128];
char str[128];

int main(int argc, char *argv[])
{
   char *s;
   char *t;
   int   l;
   int ln=0; 
   skpcapt_t *cpt = skpcaptnew(10);

   while ((s=fgets(buf,1024,stdin))) {
     ln++;
     _dbgmsg("READ: %s",s);
     if ((t=skp(s,"%[PS0123456789%]:%L"))) {
       l = (int)(t-s)-2;

       _dbgmsg("FOUND: %.*s",l+2,s);
       switch(*s) {
         case 'P' : strncpy(pat,s+2,l);
                    pat[l] = '\0';
                    dbgprt("PTRN: '%s' \x9stdin:%d\n",pat,ln);
                    break;
         case 'S' : strncpy(str,s+2,l);
                    str[l] = '\0';
                    dbgprt("STRN: '%s' \x9stdin:%d\n",str,ln);
                    t=skp(str,pat,cpt);
                    break;
         case '0' : case '1' : case '2' : case '3' : case '4' : 
         case '5' : case '6' : case '7' : case '8' : case '9' : {
                      int n = *s -'0';
                      int l_chk = t-(s+2);
                      int l_cpt = (int)(cpt->str[n].end - cpt->str[n].start);
                      int chk = ((l_chk == l_cpt) && (l_chk == 0 || strncmp(s+2,cpt->str[n].start,l_chk) == 0));
                      dbgprt("%s: [%d] '%.*s' '%.*s' \x9stdin:%d\n",chk?"PASS":"FAIL",n,l_chk,s+2,l_chk,cpt->str[n].start,ln);
                    }
                    break;
       }
     }
   }


   skpcaptfree(cpt);

   exit(0);
}