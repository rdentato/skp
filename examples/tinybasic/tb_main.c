#include <stdio.h>
#include <stdint.h>
#include "tinybasic.h"

void generate(ast_t ast, FILE *src);

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
  fprintf(stderr,"Usage: tb sourcefile\n");
  fprintf(stderr,"  Parse tb sourcefile\n");
  exit(1);
}

int main(int argc, char *argv[])
{
  char *tbbuf = NULL;
  ast_t ast = NULL;
  FILE *src=NULL;
  FILE *hdr=NULL;
  char *s;
  int trc =0;
  
  if (argc<2) usage();

  s = argv[1];

  if (*s == '-' && s[1]=='t') {
    trc = 1;
    if (argc<3) usage();
    s = argv[2];
  }

  tbbuf = loadsource(s);

  if (!tbbuf) usage();

  ast = skpparse(tbbuf,program,trc);

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
      fclose(hdr); hdr = NULL;
      fprintf(stderr,"Open '%s' to analyze the AST\n",fnamebuf);
    }
#if 0
    sprintf(fnamebuf,"%s"".c",bnamebuf);
    src = fopen(fnamebuf,"w");
    if (src) {
      generate(ast,src);
      fclose(src); src = NULL;
    }
    else {
      fprintf(stderr,"Unable to write on %s\n",fnamebuf);
    }
#endif    
  }

  if (src) fclose(src);
  if (hdr) fclose(hdr);
   
  if (tbbuf) free(tbbuf);
  if (ast) astfree(ast);
}
