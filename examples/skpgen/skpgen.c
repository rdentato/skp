#define SKP_MAIN
#include "skp.h"

skpdef(grammar) {
  skprule_(spc_);
  skpmany{       skprule(code); 
           skpor skprule(ruledef);
           skprule_(spc_);
         }
  skpmatch_("!.");
}

skpdef(ruledef) {
  skprule(rulename); skprule_(spc_); skpmatch_("'='");
                     skprule_(alt); //astlift; 
                     skprule_(spc_); skpmatch_("';'");
}

skpdef(rulename) { skpmatch_("I"); }

skpdef(alt) {
  skprule(alt_once);
  skpany{ skprule_(spc_); skpmatch_("'/'");  skprule(alt_or); }
}

skpdef(alt_once) {
  skprule_(seq);
}

skpdef(alt_or) {
  skprule_(seq);
}

skpdef(seq) {
  skpmany{ skprule_(spc_); 
                 skprule(incode);
           skpor { skpmatch_("'#'"); skprule(retval); }
           skpor { skpmatch_("'<@'"); skprule(abort); }
           skpor { skpmatch_("'@'?'>'"); skprule(errmsg); }
           skpor { skprule(lookahead); }
           skpor skprule_(match); 
         }
}

skpdef(errmsg) { skponce { skpmatch_("&'\"'"); skpmatch_("Q"); } // errmsg
                   skpor { skpmatch_("I"); }                     // char *
               }

skpdef(abort) { skponce { skpmatch_("&'\"'"); skpmatch_("Q"); }
                skpor { skpmatch_("I"); }
              }

skpdef(lookahead) { skpmatch_("[!&]\4");  if (!astfailed) { astretval(astcurptr[-1]); }
                    skprule_(match); }

skpdef(retval) {
  skpmatch_("?'?'");
  skpmaybe skpmatch_("D\1[xX]X\1'='I");
}

skpdef(match) {
  skponce { skprule(match_term); skprule(repeat);
            skpast(swapnoempty);
            skpast(liftall);
          } 
}
 
skpdef(repeat) { skpmatch_("?[+*?]"); }

skpdef(match_term) {
  skponce { skprule(modifier); skpast(delempty);
            skponce { skprule(pattern); }
              skpor { skprule(chkfun); skpmatch_("S '[' S ']' S"); }
              skpor { skprule(lookup); }
              skpor { skprule(ruleref); } 
          }
    skpor { skpstring_("("); skprule_(spc_); 
            skprule(alt); skpast(liftall);
            skprule_(spc_); skpstring_(")");
          }
    // skpor { skpstring_("#"); skpmatch("D"); /*skptrace("INFO: %d %d",astfailed, *astcurfrom);*/ }
}

skpdef(ruleref)  { skpmatch_("I !& S ':'"); }

#define PATTERN_IS_STRING 1

skpdef(pattern)  { skpmaybe { skpstring_("$"); if (!astfailed) {astretval(PATTERN_IS_STRING);} }
                   skpmatch_("Q");
                 }

skpdef(modifier) {
    skpmatch_("?'_' ?[!?] ?'<' ?'?' ?'^' ?'^'");
}

skpdef(chkfun) { skpmatch_("I"); };

// lkup [ TOK: expr; expr; TOK2: expr; ]

skpdef(lookup) {
  skprule(lu_func);
  skpmatch_("S'['");
  skpmany {
    skprule_(spc_);
    skprule(lu_case);
    skpmatch_("S ':' S");
    skprule(alt_case);
    skprule_(spc_);
    skpmatch_("';'");
   _skptrace("lkup %d '%.4s'",astfailed,astcurfrom);
  }
  skpmatch_("S ']'");
}

skpdef(lu_func) { skpmatch_("I") ;}
skpdef(lu_case) { skpmatch_("I\1D"); }
skpdef(alt_case) { skprule_(alt); }

skpdef(code) {skpmatch_("&'{'"); skpmatch_("B"); }
skpdef(incode) {skpmatch_("&'{'"); skpmatch_("B"); }

skpdef(spc_) { skpany{ 
                   skpmatch_("+s");
                 skpor 
                   skpmatch_("'%' N");
               }
             }

/************************************/

#define MAXFNAME 128
static char fnamebuf[MAXFNAME];

void prtrepeat(char repeat, int32_t indent, FILE *src, int nl)
{
  switch (repeat) {
    case '*' : fprintf(src,"%*.sskpany {%c",indent,skpemptystr,nl); break;
    case '+' : fprintf(src,"%*.sskpmany {%c",indent,skpemptystr,nl); break;
    case '?' : fprintf(src,"%*.sskpmaybe {%c",indent,skpemptystr,nl); break;
  }
}

#define MOD_FLAT        0x01
#define MOD_SWAP        0x02 
#define MOD_LIFT        0x04
#define MOD_LIFTALL     0x08
#define MOD_NOEMPTY     0x10
#define MOD_NOLEAF      0x20
#define MOD_SWAPNOEMPTY 0x40 

void prtmodifier(uint8_t modifier, FILE *src)
{
  if (modifier & MOD_LIFTALL)     fprintf(src," skpast(liftall);");
  if (modifier & MOD_LIFT)        fprintf(src," skpast(lift);");
  if (modifier & MOD_SWAP)        fprintf(src," skpast(swap);");
  if (modifier & MOD_NOEMPTY)     fprintf(src," skpast(delempty);");
  if (modifier & MOD_NOLEAF)      fprintf(src," skpast(delleaf);");
  if (modifier & MOD_SWAPNOEMPTY) fprintf(src," skpast(swapnoempty);");
}

void prtruledef(char *from, int32_t len, FILE *src)
{
  fprintf(src,"\n// ");
  while (len--) {
     fputc(*from,src);
     if (*from == '\n') fprintf(src,"// ");
     from++;
  }
  fputc('\n',src);
}


uint8_t altstk[256];
uint8_t stkcnt = 0;
#define push(n) (altstk[stkcnt++] = (n))
#define pop()   (altstk[--stkcnt])


void generatecode(ast_t ast, FILE *src, FILE *hdr, int nl)
{
  uint8_t modifier = 0;
  char repeat = '\0';
  uint8_t rpt;
  int32_t indent = 0;
  int32_t line = 1 ;
  char *line_pos = NULL ;

  astvisit(ast) {
    astonentry {
       astcase(grammar) {
        fprintf(src,"#include \"skp.h\"\n");
        line_pos = astcurfrom;
       _skptrace("LN0: (%p) '%.4s",(void*)line_pos,line_pos);
      }

      astcase(ruledef) {
        if (nl == '\n') prtruledef(astcurfrom, astcurlen, src);
      }

      astcase(rulename) {
        // count the number of nl and add them to line
        while (line_pos < astcurfrom) {
         _skptrace("LN1: (%p) (%p) '%.4s",(void*)astcurfrom,(void*)line_pos,line_pos);
          if (*line_pos == '\r' ) { line++; if (line_pos[1] == '\n') line_pos++;}
          if (*line_pos == '\n' ) { line++; }
          line_pos++;
        }
        if (nl == ' ') fprintf(src,"#line %d\n",line);
        fprintf(src,"skpdef(%.*s) {%c",astcurlen, astcurfrom,nl);
        indent = 2;
        fprintf(hdr,"extern char *skp_N_%.*s;\n",astcurlen, astcurfrom);
        fprintf(hdr,"void skp_R_%.*s();\n",astcurlen, astcurfrom);
      }

      // <? swap or flat if empty
      // <  swap
      // ^  liftall
      // ^^ liftone
      // _  flat (don't build node)
      // _? flat (don't buoild node) if leaf 
      // _! flat if empty match
      astcase(modifier) {
        modifier = 0;
        for (char *c = astcurfrom; c<astcurto; c++) {
          switch (*c) {
            case '_' : if (c[1] == '!') { modifier |= MOD_NOLEAF; c++; }
                       else if (c[1] == '?') { modifier |= MOD_NOEMPTY; c++; }
                       else modifier |= MOD_FLAT;
                       break;

            case '<' : if (c[1] == '?') { modifier |= MOD_SWAPNOEMPTY; c++; }
                       else modifier |= MOD_SWAP;
                       break;

            case '^' : if (c[1] == '^') { modifier |= MOD_LIFTALL; c++; }
                       else modifier |= MOD_LIFT;
                       break;
          }
        }
      }

      astcase(repeat) { 
        repeat = *astcurfrom;
      }

      astcase(ruleref) {
        if (repeat) { prtrepeat(repeat,indent,src,nl); indent+=2; }
        fprintf(src,"%*.sskprule%s", indent, skpemptystr, modifier & MOD_FLAT ? "_":"");
        fprintf(src,"(%.*s);", astcurlen, astcurfrom);
        prtmodifier(modifier,src);
        fprintf(src,"%c",nl);
        if (repeat) { indent-=2; fprintf(src,"%*s}%c",indent,skpemptystr,nl); }
        repeat = '\0';
        modifier = 0;
      }

      astcase(pattern) { 
        if (repeat) { prtrepeat(repeat,indent,src, nl); indent+=2; }
        if (astcurnodeinfo == PATTERN_IS_STRING) {
          fprintf(src,"%*sskpstring%s",indent, skpemptystr, modifier & MOD_FLAT ? "_":"");
          fprintf(src,"(\"%.*s\");",astcurlen-3,astcurfrom+2);
        }
        else if (*astcurfrom == '\'') {
          fprintf(src,"%*sskpstring%s",indent, skpemptystr, modifier & MOD_FLAT ? "_":"");
          fprintf(src,"(\"%.*s\");",astcurlen-2,astcurfrom+1);
        }
        else {
          fprintf(src,"%*sskpmatch%s",indent, skpemptystr, modifier & MOD_FLAT ? "_":"");
          fprintf(src,"(\"%.*s\");",astcurlen-2,astcurfrom+1);
        }

        prtmodifier(modifier,src);
        fprintf(src,"%c",nl);
        if (repeat) { indent-=2; fprintf(src,"%*s}%c",indent,skpemptystr,nl); }
        repeat = '\0';
        modifier = 0;
      }

      astcase(chkfun) { 
        if (repeat) { prtrepeat(repeat,indent,src,nl); indent+=2; }
        fprintf(src,"%*sskpcheck%s",indent, skpemptystr, modifier & MOD_FLAT ? "_":"");
        fprintf(src,"(%.*s);",astcurlen,astcurfrom);
        prtmodifier(modifier,src);
        fprintf(src,"%c",nl);
        if (repeat) { indent-=2; fprintf(src,"%*s}%c",indent,skpemptystr,nl); }
        repeat = '\0';
        modifier = 0;
        fprintf(hdr,"extern char *skp_N_%.*s;\n",astcurlen, astcurfrom);
      }

      astcase(lu_func) {
        rpt = 0;
        if (repeat) { prtrepeat(repeat, indent, src,nl); rpt++;}
        fprintf(src,"%*sskplookup%s",indent, skpemptystr, modifier & MOD_FLAT ? "_":"");
        fprintf(src,"(%.*s)",astcurlen,astcurfrom);
        fprintf(src," {%c",nl);
        rpt++;
        indent += 2*rpt;
        push(rpt);
        repeat = '\0';
        modifier = 0;
        fprintf(hdr,"extern char *skp_N_%.*s;\n",astcurlen, astcurfrom);
      }

      astcase(lu_case) {
        if (strncmp("default",astcurfrom,7) == 0) 
          fprintf(src,"%*sdefault :%c",indent,skpemptystr,nl);
        else 
          fprintf(src,"%*scase %.*s:%c",indent,skpemptystr,astcurlen,astcurfrom,nl);
        indent +=4;
      }
      
      astcase(retval) {
        char *s=astcurfrom;
        if (*astcurfrom == '?') {
          fprintf(src,"%*sif (!astfailed) {%c",indent,skpemptystr,nl);
          s++; indent+=2;
        }
        fprintf(src,"%*sastretval(",indent,skpemptystr);
        if (astcurto>s) {
          int32_t n;
          if (*s=='=') {
            while (++s<astcurto) {
              fputc(*s,src);
            }
          }
          else { 
             if (*s=='x' || *s == 'X') n = strtoul(s+1,NULL,16);
             else n=strtoul(s,NULL,10);
             fprintf(src,"%d",n);
          }
          fprintf(src,");%c",nl);
        }
        else fprintf(src,"astlastinfo);%c",nl);
        if (*astcurfrom == '?') {
          indent-=2;
          fprintf(src,"%*s}%c",indent,skpemptystr,nl);
        }
      }

      astcase(errmsg) {
        fprintf(src,"%*sif (!astfailed) { skpseterrmsg(%.*s); }%c",indent,skpemptystr,astcurlen, astcurfrom,nl);
      }

      astcase(abort) {
        fprintf(src,"%*sif (astfailed) { skpabort(%.*s); }%c",indent,skpemptystr,astcurlen, astcurfrom,nl);
      }

      astcase(alt) {
        rpt = 1;
        if (repeat) { prtrepeat(repeat, indent, src,nl); }
        else fprintf(src,"%*sskpgroup {%c",indent,skpemptystr,nl);
        indent += 2*rpt;
        push(rpt);
        repeat = '\0';
      }

      astcase(alt_once) {
        rpt = 0;
        if (repeat) { prtrepeat(repeat, indent, src,nl); rpt++;}
        if (astnodeis(ast,astright(astcur,astcurnode),alt_or)) {
          fprintf(src,"%*sskponce {%c",indent,skpemptystr,nl);
          rpt++;
        }
        indent += 2*rpt;
        push(rpt);
        repeat = '\0';
      }

      astcase(alt_or) {
        fprintf(src,"%*sskpor {%c",indent,skpemptystr,nl); indent+=2;
        push(1);
        repeat = '\0';
      }

      astcase(lookahead) {
        fprintf(src,"%*sskp%s {%c",indent,skpemptystr,astcurtag == '!' ? "not":"peek",nl); indent+=2;
        push(1);
        repeat = '\0';
      }

      astcase(code) {
        char *start = astcurfrom+1; int len = astcurlen-2;
        if (*start == '?') {
          fprintf(src,"if (!astfailed) {\n%.*s\n}\n",len-1,start+1);
        }
        else fprintf(src,"%.*s\n",len,start);
      }

      astcase(incode) {
        char *start = astcurfrom+1; int len = astcurlen-2;
        fprintf(src,"%*s",indent,skpemptystr);
        if (*start == '?') {
          fprintf(src,"if (!astfailed) { ");
          len--; start++;
        }
        fprintf(src,"%.*s ",len,start);
        if (start != astcurfrom+1) {
          fprintf(src,"}");
        }
        fprintf(src,"\n");
      }
    }
    astonexit {
      astcase(ruledef) {
        fprintf(src,"}\n");
      }

      astcase(alt_or, alt_once, alt, lookup, lookahead) {
        rpt = pop();
        while (rpt-- > 0) {indent -=2; fprintf(src,"%*s}%c",indent,skpemptystr,nl); }
      }

      astcase(alt_case) {
        fprintf(src,"%*sbreak;%c",indent,skpemptystr,nl);
        indent -=4;
      }
    }
  }
}

/************************************/

char *loadsource(char *fname)
{
  FILE *f;
  int32_t size=0;
  char *src=NULL;

  snprintf(fnamebuf,MAXFNAME,"%s.skp",fname);

  f = fopen(fnamebuf,"rb");
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
  fprintf(stderr,"Usage: skpgen [-c] grammar\n");
  fprintf(stderr,"      Generate grammar.c and grammar.h from grammar.skp\n");
  fprintf(stderr,"  -c  Generate a readable .c source file\n");
  exit(1);
}

int main(int argc, char *argv[])
{
  char *grammarbuf = NULL;
  ast_t ast = NULL;
  FILE *src=NULL;
  FILE *hdr=NULL;
  int newline = ' ';
  int argn = 1;

  if (argc<2) usage();

  if (argv[1][0] == '-' && argv[1][1] == 'c') {
    newline = '\n';
    argn++;
    if (argc <= argn) usage();
  }
  
  grammarbuf = loadsource(argv[argn]);

  if (!grammarbuf) usage();

  ast = skpparse(grammarbuf,grammar,0);

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

    snprintf(fnamebuf,MAXFNAME,"%s.t",argv[argn]);
    src=fopen(fnamebuf,"w");
    if (src) {
      astprint(ast,src);
      fprintf(src,"\n");
      fclose(src);
    }
    snprintf(fnamebuf,MAXFNAME,"%s.c",argv[argn]);
    src = fopen(fnamebuf,"w");
    if (src) {
      if (newline == ' ') fprintf(src,"#line 1 \"%s.skp\"\n",argv[argn]);
      snprintf(fnamebuf,MAXFNAME,"%s.h",argv[argn]);
      hdr = fopen(fnamebuf,"w");
      if (hdr) {
        fprintf(hdr,"#include \"skp.h\"\n");
        fprintf(hdr,"#ifndef SKP_PARSE_%s\n",argv[argn]);
        fprintf(hdr,"#define SKP_PARSE_%s\n",argv[argn]);
        generatecode(ast,src,hdr,newline);
      }
    }
  }

  if (src) fclose(src);
  if (hdr) {
    fprintf(hdr,"#endif\n");
    fclose(hdr);
  }
  if (grammarbuf) free(grammarbuf);
  if (ast) astfree(ast);

}