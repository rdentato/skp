#define SKP_MAIN
#include "skp.h"

skpdef(grammar) {
  skprule_(spc_);
  skpmany{       skprule(code); 
           skpor skprule_(ruledef);
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
           skpor { skpmaybe { skprule(lookahead);} skprule_(match); }
           skpor skprule_(match); 
         }
}

skpdef(lookahead) {
  skpmatch_("[!&]\4");
}

skpdef(retval) {
  skpmatch_("?'?'");
  skpmaybe skpmatch_("D");
}

skpdef(match) {
  skponce { skprule(match_term); skprule(repeat);
            astswapnoempty;
            astliftall;
          } 
}
 
skpdef(repeat) { skpmatch_("?[+*?]"); }

skpdef(match_term) {
  skponce { skprule(modifier); astnoemptyleaf;
            skponce { skprule(pattern); }
              skpor { skprule(chkfun); skpmatch_("S '[' S ']' S"); }
              skpor { skprule(lookup); }
              skpor { skprule(ruleref); } 
          }
    skpor { skpstring_("("); skprule_(spc_); 
            skprule(alt); astliftall;
            skprule_(spc_); skpstring_(")");
          }
    skpor { skpstring_("#"); skpmatch("D"); /*skptrace("INFO: %d %d",astfailed, *astcurfrom);*/ }
}

skpdef(ruleref)  { skpmatch_("I !@ S ':'"); }

skpdef(pattern)  { skpmaybe { skpstring_("$"); if (!astfailed) {astretval(1);} }
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

skpdef(code) {skpmatch_("@'{'"); skpmatch_("B"); }
skpdef(incode) {skpmatch_("@'{'"); skpmatch_("B"); }

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
  if (modifier & MOD_LIFTALL)     fprintf(src," astliftall;");
  if (modifier & MOD_LIFT)        fprintf(src," astlift;");
  if (modifier & MOD_SWAP)        fprintf(src," astswap;");
  if (modifier & MOD_NOEMPTY)     fprintf(src," astnoemptyleaf;");
  if (modifier & MOD_NOLEAF)      fprintf(src," astnoleaf;");
  if (modifier & MOD_SWAPNOEMPTY) fprintf(src," astswapnoempty;");
}

uint8_t altstk[256];
uint8_t stkcnt = 0;
#define push(n) (altstk[stkcnt++] = (n))
#define pop()   (altstk[--stkcnt])


void generatecode(ast_t ast, FILE *src, FILE *hdr, int nl)
{
  int rules = 0;
  uint8_t modifier = 0;
  char repeat = '\0';
  uint8_t rpt;
  int32_t indent = 0;
  int32_t line = 1 ;
  char *line_pos = NULL ;

  astvisit(ast) {
    astifentry {
       astifnodeis(grammar) {
        fprintf(src,"#include \"skp.h\"\n\n");
        line_pos = astcurfrom;
       _skptrace("LN0: (%p) '%.4s",(void*)line_pos,line_pos);
      }

      astifnodeis(rulename) {
        if (rules++ >0) fprintf(src,"}\n\n");
        // count the number of nl and add them to line
        while (line_pos < astcurfrom) {
         _skptrace("LN1: (%p) (%p) '%.4s",(void*)astcurfrom,(void*)line_pos,line_pos);
          if (*line_pos == '\r' ) { line++; if (line_pos[1] == '\n') line_pos++;}
          if (*line_pos == '\n' ) { line++; }
          line_pos++;
        }
        fprintf(src,"#line %d\n",line);
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
      astifnodeis(modifier) {
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

      astifnodeis(repeat) { 
        repeat = *astcurfrom;
      }

      astifnodeis(ruleref) {
        if (repeat) { prtrepeat(repeat,indent,src,nl); indent+=2; }
        fprintf(src,"%*.sskprule%s", indent, skpemptystr, modifier & MOD_FLAT ? "_":"");
        fprintf(src,"(%.*s);", astcurlen, astcurfrom);
        prtmodifier(modifier,src);
        fprintf(src,"%c",nl);
        if (repeat) { indent-=2; fprintf(src,"%*s}%c",indent,skpemptystr,nl); }
        repeat = '\0';
        modifier = 0;
      }

      astifnodeis(pattern) { 
        if (repeat) { prtrepeat(repeat,indent,src, nl); indent+=2; }
        if (astcurnodeinfo == 1) {
          fprintf(src,"%*sskpstring%s",indent, skpemptystr, modifier & MOD_FLAT ? "_":"");
          fprintf(src,"(\"%.*s\");",astcurlen-3,astcurfrom+2);
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

      astifnodeis(chkfun) { 
        if (repeat) { prtrepeat(repeat,indent,src,nl); indent+=2; }
        fprintf(src,"%*sskpcheck%s",indent, skpemptystr, modifier & MOD_FLAT ? "_":"");
        fprintf(src,"(%.*s);",astcurlen,astcurfrom);
        prtmodifier(modifier,src);
        fprintf(src,"%c",nl);
        if (repeat) { indent-=2; fprintf(src,"%*s}%c",indent,skpemptystr,nl); }
        repeat = '\0';
        modifier = 0;
      }

      astifnodeis(lu_func) {
        rpt = 0;
        if (repeat) { prtrepeat(repeat, indent, src,nl); rpt++;}
        fprintf(src,"%*sskplookup(%.*s) {%c",indent,skpemptystr,astcurlen,astcurfrom,nl);
        rpt++;
        indent += 2*rpt;
        push(rpt);
        repeat = '\0';
      }

      astifnodeis(lu_case) {
        fprintf(src,"%*scase %.*s:%c",indent,skpemptystr,astcurlen,astcurfrom,nl);
        indent +=4;
      }
      
      astifnodeis(retval) {
        char *s=astcurfrom;
        if (*astcurfrom == '?') {
          fprintf(src,"%*sif (!astfailed) {%c",indent,skpemptystr,nl);
          s++; indent+=2;
        }
        fprintf(src,"%*sastretval(",indent,skpemptystr);
        if (astcurto>s) fprintf(src,"%d);%c",atoi(s),nl);
        else fprintf(src,"astlastinfo);%c",nl);
        if (*astcurfrom == '?') {
          indent-=2;
          fprintf(src,"%*s}%c",indent,skpemptystr,nl);
        }
      }

      astifnodeis(alt) {
        rpt = 1;
        if (repeat) { prtrepeat(repeat, indent, src,nl); }
        else fprintf(src,"%*sskpgroup {%c",indent,skpemptystr,nl);
        indent += 2*rpt;
        push(rpt);
        repeat = '\0';
      }

      astifnodeis(alt_once) {
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

      astifnodeis(alt_or) {
        fprintf(src,"%*sskpor {%c",indent,skpemptystr,nl); indent+=2;
        push(1);
        repeat = '\0';
      }

      astifnodeis(code) {
        if (rules >0) { fprintf(src,"}\n\n"); indent = 0; rules = 0; }
        char *start = astcurfrom+1; int len = astcurlen-2;
        if (*start == '?') {
          fprintf(src,"if (!astfailed) {\n%.*s\n}\n",len-1,start+1);
        }
        else fprintf(src,"%.*s",len,start);
        fprintf(src,"#line %d",line);
      }
      astifnodeis(incode) {
        char *start = astcurfrom+1; int len = astcurlen-2;
        if (*start == '?') {
          fprintf(src,"if (!astfailed) {\n%.*s\n}\n",len-1,start+1);
        }
        else fprintf(src,"%.*s",len,start);
        fprintf(src,"#line %d",line);
      }
    }
    astifexit {
      astifnodeis(alt_or, alt_once, alt, lookup) {
        rpt = pop();
        while (rpt-- > 0) {indent -=2; fprintf(src,"%*s}%c",indent,skpemptystr,nl); }
      }
      astifnodeis(alt_case) {
        fprintf(src,"%*sbreak;%c",indent,skpemptystr,nl);
        indent -=4;
      }
    }
  }
  if (rules >0 ) fprintf(src,"}\n");
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
  fprintf(stderr,"Usage: skpgen grammar\n");
  fprintf(stderr,"  Use grammar.skp to generate grammar.c and grammar.h\n");
  exit(1);
}

int main(int argc, char *argv[])
{
  char *grammarbuf = NULL;
  ast_t ast = NULL;
  FILE *src=NULL;
  FILE *hdr=NULL;

  int newline = ' ';

  if (argc<2) usage();

  grammarbuf = loadsource(argv[1]);

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

    snprintf(fnamebuf,MAXFNAME,"%s.t",argv[1]);
    src=fopen(fnamebuf,"w");
    if (src) {
      astprint(ast,src);
      fprintf(src,"\n");
      fclose(src);
    }
    snprintf(fnamebuf,MAXFNAME,"%s.c",argv[1]);
    src = fopen(fnamebuf,"w");
    if (src) {
      fprintf(src,"#line 1 \"%s.skp\"\n",argv[1]);
      snprintf(fnamebuf,MAXFNAME,"%s.h",argv[1]);
      hdr = fopen(fnamebuf,"w");
      if (hdr) {
        fprintf(hdr,"#include \"skp.h\"\n");
        fprintf(hdr,"#ifndef SKP_PARSE_%s\n",argv[1]);
        fprintf(hdr,"#define SKP_PARSE_%s\n",argv[1]);
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