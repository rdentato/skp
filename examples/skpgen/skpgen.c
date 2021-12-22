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
            if (astlastnodeisempty) astremove;
            else astswap; 
            astliftall;
          } 
    skpor { skpmatch("&[!&]\4"); skprule_(match); }
    skpor { skpstring_("#"); skpmatch("&D"); }
}
 
// match_term = _'(' alt _'&*s)' / '&?[^_<]' rulename ;
// rulename = "&I" ;
skpdef(match_term) {
  skponce { skpmatch("&?[_]\5"); astnoemptyleaf; skpmatch("&Q\3"); }
    skpor { skpmatch("&?[^_<]\5");  astnoemptyleaf; skpmatch("&I\2"); }
    skpor { skpstring_("("); skprule_(spc_); 
            skprule(alt); astlift; astlift; 
            skprule_(spc_); skpstring_(")");
          }
}

// spc = (&+s / '//&N')*
skpdef(spc_) { skpany{ skpmatch_("&+s\1//&N"); } }
skpdef(spc) { skpmany{ skpmatch_("&+s\1//&N"); } }

/************************************/
void prtrepeat(char repeat)
{
  switch (repeat) {
    case '*' : printf("skpany {\n"); break;
    case '+' : printf("skpmany {\n"); break;
    case '?' : printf("skpmaybe {\n"); break;
  }
}

uint8_t altstk[256];
uint8_t stkcnt = 0;
#define push(n) (altstk[stkcnt++] = (n))
#define pop()   (altstk[--stkcnt])

void generatecode(ast_t ast)
{
  int rules = 0;
  char modifier = '\0';
  char repeat = '\0';
  uint8_t rpt;
  
  astvisitdf(ast, node) {
    astonentry(ast,node) {
 
      astifnodeis(grammar) {
        printf("#include \"skp.h\"\n\n");
      }

      astifnodeis(STR7) {
        if (rules++ >0) printf("}\n\n");
        printf("skpdef(%.*s){\n",astnodelen(ast,node),astnodefrom(ast,node));
      }

      astifnodeis(STR5) {
        modifier = *astnodefrom(ast,node);
      }

      astifnodeis(STR6) { 
        repeat = *astnodefrom(ast,node);
      }

      astifnodeis(STR2) {
        if (repeat) prtrepeat(repeat);
        printf("skprule%s",modifier == '_' ? "_":"");
        printf("(%.*s);",astnodelen(ast,node),astnodefrom(ast,node));
        if (modifier == '<') printf(" astswap;");
        printf("\n");
        if (repeat) printf("}\n");
        repeat = '\0';
        modifier = '\0';
      }

      astifnodeis(STR3) { 
        if (repeat) prtrepeat(repeat);
        printf("skpmatch%s",modifier == '_' ? "_":"");
        printf("(%.*s);\n",astnodelen(ast,node),astnodefrom(ast,node));
        if (repeat) printf("}\n");
        repeat = '\0';
        modifier = '\0';
      }

      astifnodeis(alt) {
        rpt = 0;
        if (repeat) { prtrepeat(repeat); rpt++;}
        push(rpt);
        repeat = '\0';
      }

      astifnodeis(alt_once) {
        rpt = 0;
        if (repeat) { prtrepeat(repeat); rpt++;}
        if (astnodeis(ast,astright(ast,node),alt_or)) {
          printf("skponce {\n");
          rpt++;
        }
        push(rpt);
        repeat = '\0';
      }

      astifnodeis(alt_or) {
        printf("skpor {\n");
        push(1);
        repeat = '\0';
      }
    }
    astonexit(ast, node) {
      astifnodeis(alt_or, alt_once, alt) {
        rpt = pop();
        while (rpt-- > 0) printf("}\n");
      }
    }
  }
  if (rules >0 ) printf("}\n");
}

/************************************/

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
  fprintf(stderr,"Usage: skpgen grammar.skp\n");
  exit(1);
}
int main(int argc, char *argv[])
{
  char *source = NULL;
  ast_t ast = NULL;

  if (argc<2) usage();

  source = loadsource(argv[1]);

  if (!source) usage();

  ast = skpparse(source,grammar);

  if (asterrpos(ast)) {
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
    astprint(ast,stderr);
    fprintf(stderr,"\n");

    generatecode(ast);
  }
  
  free(source);
  astfree(ast);

}