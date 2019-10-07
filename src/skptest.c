#include "skp.h"


static char buf[1024];
//char exp[1024];

int main(int argc, char *argv[])
{
  char *s;
  char *p = NULL;
  while ((s=fgets(buf,1024,stdin))) {
    if (*s == '#') continue;
    if (*s == ' ') { // string to match
      if (p) {
        skpexec(p,s+1);
      }
      else printf("NO EXPRESSION");
    }
    else { // express to compile
      if (p) skpfree(p);
      for (char *e=s; *e ; e++) if (*e == '\n' || *e == '\r') { *e='\0'; break;}
      printf("     ;; %s\n",s);
      p=skpcomp(s);
      skpdisasm(p,NULL);
    }
  }
  if (p) skpfree(p);
  exit(0);
}
