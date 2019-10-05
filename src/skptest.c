#include "skp.h"


static char buf[1024];
//char exp[1024];

int main(int argc, char *argv[])
{
  char *s;
  char *p;
  while ((s=fgets(buf,1024,stdin))) {
    if (*s != '#') {
      for (char *e=s; *e ; e++) if (*e == '\n' || *e == '\r') { *e='\0'; break;}
      printf("     ;; %s\n",s);
      p=skpcomp(s);
      skpprint(p,NULL);
      skpfree(p);
    }
  }
  exit(0);
}
