
#define SKP_MAIN
#include "skp.h"

/*
// grammar = (_'&*s' ruledef)+  _'&*s&!.' ;
// ruledef = rulename _'&*s=&*s' alt _'&*s;' ;
// alt = seq (_'&*s/' seq)* ;
// seq = (_'&*s' match)+ ;
// match = string repeat? /  subdef repeat? / '&[!&]' _'&*s' match ;
// subdef = _'(' alt _'&*s)' / '&?[^_<]' rulename ;
// repeat = _"&*s" "&[*+?]" ;
// string = '&?[_]' "&Q" ;
// rulename = "&I" ;
// spc = (&+s / '#&N')*
*/

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
                    skprule_(alt); astlift; 
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
  skpmany{ skprule_(spc_); skprule(match); }
}

// match = string repeat? /  subdef repeat? / '&[!&]' match ;
// repeat = _"&*s" "&[*+?]" ;
// string = '&?[_]' "&Q" ;
skpdef(match) {
  skponce{ skpmatch("&?[_]\5"); skpmatch("&Q\3"); skpmatch("&?[+*?]\6"); astswap;}
    skpor{ skprule(subdef); skpmatch("&?[+*?]\6"); astswap; } 
    skpor{ skpmatch("&[!&]\4"); skprule(match); }
}
 
// subdef = _'(' alt _'&*s)' / '&?[^_<]' rulename ;
// rulename = "&I" ;
skpdef(subdef) {
  skponce{ 
    skpstring_("("); skprule_(spc_); skprule_(alt); skprule_(spc_); skpstring_(")");
  }
  skpor {
    skpmatch("&?[^_<]\5"); skpmatch("&I\2");
  }
}

// spc = (&+s / '#&N')*
skpdef(spc_) { skpany{ skpmatch_("&+s\1#&N"); } }
skpdef(spc) { skpmany{ skpmatch_("&+s\1#&N"); } }


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
    trace("Error %s",asterrmsg(ast));
    trace("%.*s",(int)(endln-ln),ln);
    trace("%.*s^",col,"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
    //trace("Error '%s'",asterrmsg(ast));
    //trace("@: '%s'",asterrpos(ast));
  }
  else {
    astprint(ast,stdout);
    printf("\n");
  }
  
  free(source);
  astfree(ast);

}