#include <stdio.h>
#include <stdint.h>
#include "pl0.h"


#define trace(...) (fprintf(stderr,__VA_ARGS__),fputc('\n',stderr))
/************************************/

#define MAXFNAME 120
static char fnamebuf[MAXFNAME+8];
static char bnamebuf[MAXFNAME];

char *loadsource(char *fname)
{
  FILE *f;
  int32_t size=0;
  char *src=NULL;

  f = fopen(fname,"rb");
  if (f) {
    fseek(f,0,SEEK_END);
    size = ftell(f);
    fseek(f,0,SEEK_SET);
    src = malloc(size+1+!(size & 1));
    if (src) {
      *src = '\0';
      if (fread(src,size,1,f)) {
        src[size] = '\0';
      }
      else {
        free(src);
        src = NULL;
      };
    }
    fclose(f);
  }
  
  return src;
}

#define trace(...) (fprintf(stderr,__VA_ARGS__),fputc('\n',stderr))
void usage()
{
  fprintf(stderr,"Usage: pl0 sourcefile\n");
  fprintf(stderr,"  Convert pl0 sourcefile to C\n");
  exit(1);
}

int main(int argc, char *argv[])
{
  char *pasbuf = NULL;
  ast_t ast = NULL;
  FILE *src=NULL;
  FILE *hdr=NULL;
  char *s;
  
  if (argc<2) usage();

  pasbuf = loadsource(argv[1]);

  if (!pasbuf) usage();

  ast = skpparse(pasbuf,program,0);

  if (asthaserr(ast)) {
    trace("In rule: '%s'",asterrrule(ast));
    char *ln = asterrline(ast);
    char *endln = ln;
    while (*endln && *endln != '\r' && *endln != '\n') endln++;
    int32_t col = asterrcolnum(ast);
    trace("Error: %s",asterrmsg(ast));
    trace("%.*s",(int)(endln-ln),ln);
    trace("%.*s^",col,"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
    //trace("Error '%s'",asterrmsg(ast));
    //trace("@: '%s'",asterrpos(ast));
  }
  else {
    strcpy(bnamebuf,argv[1]);
    s=bnamebuf;
    while (*s) s++;
    while (s>bnamebuf && s[-1] != '.') s--;
    if (s>bnamebuf) s[-1] = '\0';
    
    sprintf(fnamebuf,"%s"".t",bnamebuf);
    hdr = fopen(fnamebuf,"w");
    if (hdr) {
      astprint(ast,hdr);
    }
  }

  if (src) fclose(src);
  if (hdr) {
    fclose(hdr);
  }
  if (pasbuf) free(pasbuf);
  if (ast) astfree(ast);
}