#define SKP_MAIN
#include "skp.h"

/*
// grammar = (_'&*s' ruledef)+  _'&*s&!.' ;
// ruledef = rulename _'&*s=&*s' alt _'&*s;' ;
// alt = seq (_'&*s/' seq)* ;
// seq = (_'&*s' match)+ ;
// match = string repeat? /  match_term repeat? / '&[!&]' _'&*s' match ;
// match_term = _'(' alt _'&*s)' / '&?[^_<]' rulename ;
// repeat = _"&*s" "&[*+?]" ;
// string = '&?[_]' "&Q" ;
// rulename = "&I" ;
// spc = (&+s / '#&N')*
skpdef(pippo)
*/
// skpdef(pluto)
// grammar = (_'&*s' ruledef)+  _'&*s&!.' ;
skpdef(grammar) {
  skprule_(spc_);
  skpmany{ skprule_(ruledef); skprule_(spc_); }
  skpmatch_("&!.");
}

// ruledef = rulename _'&*s=&*s' alt _'&*s;' ;
// rulename = "&I" ;

skpdef(ruledef) {
  skpmatch("&I\7"); skprule_(spc_); skpmatch_("=");
                    skprule_(alt); //astlift; 
                    skprule_(spc_); skpmatch_(";");
}

// alt =  alt_once ( _spc_ _'&*s/' alt_or)* ;
skpdef(alt) {
  skprule(alt_once);
  skpany{ skprule_(spc_); skpmatch_("/");  skprule(alt_or); }
}

skpdef(alt_once) {
  skprule_(seq);
}

skpdef(alt_or) {
  skprule_(seq);
}

// seq = (_'&*s' match)+ ;
skpdef(seq) {
  skpmany{ skprule_(spc_); skprule_(match); }
}

// match =  match_term repeat? / '&[!&]\4' match / _'#' '&D';
// match_term = string / _'(' _spc_ alt _spc_ _')' / '_\5<\5^^\5^' rulenameref ;
// repeat = _"&*s" "&[*+?]" ;
// string = '&?[_]' "&Q" ;
skpdef(match) {
  skponce { skprule(match_term); skpmatch("&?[+*?]\6");
            astswapnoempty;
            astliftall;
          } 
    skpor { skpmatch("&[!&]\4"); skprule_(match); }
}
 
// match_term = _'(' alt _'&*s)' / '&?[^_<]' rulename ;
// rulename = "&I" ; skpmatch("&*[^_<]\5"); 

skpdef(match_term) {
  skponce { skprule_(modifier); astnoemptyleaf; skpmatch("&Q\3"); }
    skpor { skprule_(modifier); astnoemptyleaf; skpmatch("&I\2"); }
    skpor { skpstring_("("); skprule_(spc_); 
            skprule(alt); astliftall;
            skprule_(spc_); skpstring_(")");
          }
    skpor { skpstring_("#"); skpmatch("&D"); /*skptrace("INFO: %d %d",astfailed, *astcurfrom);*/ }
}


skpdef(modifier) {
    skpmatch("&?'_'&?[!?]&?'<'&?'?'&?'^'&?'^'\5");
}

// spc = (&+s / '//&N')*
skpdef(spc_) { skpany{ skpmatch_("&+s\1//&N"); } }
skpdef(spc) { skpmany{ skpmatch_("&+s\1//&N"); } }

/************************************/

#define MAXFNAME 128
static char fnamebuf[MAXFNAME];

void prtrepeat(char repeat, int32_t indent, FILE *src)
{
  switch (repeat) {
    case '*' : fprintf(src,"%*.sskpany {\n",indent,skpemptystr); break;
    case '+' : fprintf(src,"%*.sskpmany {\n",indent,skpemptystr); break;
    case '?' : fprintf(src,"%*.sskpmaybe {\n",indent,skpemptystr); break;
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


void generatecode(ast_t ast, FILE *src, FILE *hdr)
{
  int rules = 0;
  uint8_t modifier = 0;
  char repeat = '\0';
  uint8_t rpt;
  int32_t indent = 0;

  astvisit(ast) {
    astifentry {
       astifnodeis(grammar) {
        fprintf(src,"#include \"skp.h\"\n\n");
      }

      astifnodeis(STR7) {
        if (rules++ >0) fprintf(src,"}\n\n");
        fprintf(src,"skpdef(%.*s) {\n",astcurlen, astcurfrom);
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
      astifnodeis(STR5) {
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

      astifnodeis(STR6) { 
        repeat = *astcurfrom;
      }

      astifnodeis(STR2) {
        if (repeat) { prtrepeat(repeat,indent,src); indent += 2; }
        fprintf(src,"%*.sskprule%s", indent, skpemptystr, modifier & MOD_FLAT ? "_":"");
        fprintf(src,"(%.*s);", astcurlen, astcurfrom);
        prtmodifier(modifier,src);
        fprintf(src,"\n");
        if (repeat) {indent-=2; fprintf(src,"%*s}\n",indent,skpemptystr); }
        repeat = '\0';
        modifier = 0;
      }

      astifnodeis(STR3) { 
        if (repeat) { prtrepeat(repeat,indent,src); indent += 2; }
        fprintf(src,"%*sskpmatch%s",indent, skpemptystr, modifier & MOD_FLAT ? "_":"");
        fprintf(src,"(%.*s);\n",astcurlen,astcurfrom);
        prtmodifier(modifier,src);
        if (repeat) { indent-=2 ;fprintf(src,"}\n"); }
        repeat = '\0';
        modifier = 0;
      }

      astifnodeis(alt) {
        rpt = 0;
        if (repeat) { prtrepeat(repeat, indent, src); rpt++;}
        indent += 2*rpt;
        push(rpt);
        repeat = '\0';
      }

      astifnodeis(alt_once) {
        rpt = 0;
        if (repeat) { prtrepeat(repeat, indent, src); rpt++;}
        if (astnodeis(ast,astright(astcur,astcurnode),alt_or)) {
          fprintf(src,"%*sskponce {\n",indent,skpemptystr);
          rpt++;
        }
        indent += 2*rpt;
        push(rpt);
        repeat = '\0';
      }

      astifnodeis(alt_or) {
        fprintf(src,"%*sskpor {\n",indent,skpemptystr); indent+=2;
        push(1);
        repeat = '\0';
      }
    }
    astifexit {
      astifnodeis(alt_or, alt_once, alt) {
        rpt = pop();
        while (rpt-- > 0) {indent -=2; fprintf(src,"%*s}\n",indent,skpemptystr); }
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
  

  if (argc<2) usage();

  grammarbuf = loadsource(argv[1]);

  if (!grammarbuf) usage();

  ast = skpparse(grammarbuf,grammar);

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
      snprintf(fnamebuf,MAXFNAME,"%s.h",argv[1]);
      hdr = fopen(fnamebuf,"w");
      if (hdr) {
        fprintf(hdr,"#include \"skp.h\"\n");
        fprintf(hdr,"#ifndef SKP_PARSE_%s\n",argv[1]);
        fprintf(hdr,"#define SKP_PARSE_%s\n",argv[1]);
        generatecode(ast,src,hdr);
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