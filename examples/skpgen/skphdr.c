#include <stdio.h>
#include <stdlib.h>

#define SKP_MAIN
#include "skp.h"

#define LNSIZE 1024
char line[LNSIZE];

FILE *in = NULL;
FILE *out = NULL;

void prtdef(char *def)
{
  char *end;

  skp(def,"&I",&end);
  fprintf(out,"extern char *SKP_N_%.*s;\n",(int)(end-def),def);
}
char *emptystr ="";
#define IN_CODE    1
#define IN_COMMENT 2

int main(int argc, char *argv[])
{
   char *s;
   int state=IN_CODE;

  in = stdin;
  out = stdout;

   while (fgets(line,LNSIZE,in)) {
     s = line;
     while (*s) {
       if (*s && (state == IN_CODE)) {
         skp(s) {
           skpif("&+![s/'\"]") ;
           skpif("skpdef&*s(&*s") {
             prtdef(skpto);  s = emptystr; break;
           }
           skpif("//") { s = emptystr; break; }
           skpif("/*") { state = IN_COMMENT; break;}
           skpif("&Q") ;
           skpif("&.") ;
         }
       }
       if (*s && (state == IN_COMMENT)) {
         skp(s) {
           skpif("&+![*]") ;
           skpif("*/") { s = skpend; state = IN_CODE; break; }
           skpif("&.") ;
         }
       }
     }
   }
}
