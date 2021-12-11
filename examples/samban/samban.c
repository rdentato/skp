
// la.samban. 
// (tr. the computer language)
//
// A toy language to show skp features

/* GRAMMAR 
expr = rel  (('<>'|'<='|'<'|'>'|'>=') rel )*
rel  = term (('+'|'-') term )*
term = fact (('*'|'/') fact )*
fact = '&D' | '-' fact | var | '(' expr ')'
var  = '&I'

prog = statement+
bloc = '{' statement+ '}' | statement ';'

statement = 'if' expr 'then' bloc ('else' bloc)?
          | 'while' expr 'do' bloc 
          | 'print' expr ';'
          | var '=' expr ';'

*/
#define SKP_MAIN
#include "skp.h"

/* expressions */

skpdef(expr) {
  skprule(rel);
  astlift;
  skpany {
    skpanyspaces();
    skpmatch("==\1<>\1<=\1<\1>=\1>");
    skprule(rel);
    astlift;
    astswap;  // Convert in RPN
  }
}

skpdef(rel) {
  skpanyspaces();
  skprule_(term);
  skpany {
    skpanyspaces();
    skpmatch("+\1-");
    skprule_(term);
    astswap;  // Convert in RPN
  }
}


skpdef(term) {
  skpanyspaces();
  skprule_(fact);
  skpany {
    skpanyspaces();
    skpmatch("*\1/");
    skprule_(fact);
    astswap;  // Convert in RPN
  }
}

skpdef(fact) {
  skpanyspaces();
  skponce { skpmatch("&D\2&I\3"); }
    skpor { skprule(neg); }
    skpor { skpmatch_("&*s("); 
            skprule(expr);
            astlift;
            skpmatch_("&*s)&*s"); 
          }
}

skpdef(neg) {
  skpmatch_("-&*s");
  skprule_(fact);
}

#define TOK_IF    -201
#define TOK_PRINT -202
#define TOK_WHILE -203

// Check keywords
int32_t keywords(char **src)
{
  int alt;
  char *to;
  // Imagine a huge lookup table! (or a perfect hash table) ...
  alt = skp(*src, "&!Cif\1&!Cprint\2&!Cwhile\3",&to);
  
  if (alt == 0) {
    *src = NULL;  // Failed to match!
    return 0;     // on failure, return value is not relevant.
  }

  *src = to; // Success (move to the end of matching text)

  // Negative values will eliminate the string nodes from the AST
  return -(200+alt);
}

skpdef(statement) {
  skpanyspaces();
  skplookup(keywords) {
    case TOK_IF:    skprule(expr);
                    astlift;
                    skpmatch_("&*s&!CTHEN&*s");
                    skprule(bloc);
                    skpmaybe {
                      skpmatch_("&*s&!CELSE&*s");
                      skprule(bloc);
                    }
                    break;

    case TOK_PRINT: skprule(expr);
                    astlift;
                    skpmatch_("&*s;&*s");
                    break;

    case TOK_WHILE: skptrace("WHILE");
                    skprule(expr);
                    astlift;
                    skprule(bloc);
                    break;
  }
  skpor {
    skpmatch("&I\4"); // Type 4 strings are identifier to be assigned
    skpmatch_("&*s=&*s");
    skprule(expr);
    astlift;
    skpmatch_("&*s;&*s");
  }
}

skpdef(bloc) {
  skpanyspaces();
  skponce {
    skpstring_("{");
    skpany {
      skprule(statement);
    }
    skpanyspaces();
    skpstring_("}");
  }
  skpor {
    skprule(statement);
  }
}

skpdef(program) {
  skpany {
    skprule(statement);
  }
  skpmatch_("&*s&!.");
}




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
int main(int argc, char *argv[])
{
  char *source = NULL;
  ast_t ast = NULL;

  if (argc<2) {fprintf(stderr,"Usage: samban source.smb\n"); exit(1);}

  source = loadsource(argv[1]);

  if (source) {
    ast = skpparse(source,program);

    if (asterror(ast)) {
      trace("Error @: '%s'",asterror(ast));
      trace("In rule: '%s'",asterrorrule(ast));
    }
    else {
      astprint(ast,stdout);
      printf("\n");
    }
  
    free(source);
    astfree(ast);
  }

}