/*
 See: "Dr. Dobb's Journal of Computer Calisthenics & Orthodontia, Vol. 1, No. 1"
      https://archive.org/details/dr_dobbs_journal_vol_01

   __ _ _ __ __ _ _ __ ___  _ __ ___   __ _ _ __ 
  / _` | '__/ _` | '_ ` _ \| '_ ` _ \ / _` | '__|
 | (_| | | | (_| | | | | | | | | | | | (_| | |   
  \__, |_|  \__,_|_| |_| |_|_| |_| |_|\__,_|_|   
   __/ |                                         
  |___/                                          
 
progam = line*
 
line = number statement EOL

statement = "PRINT" pr-expr-list  /
            "INPUT" in-var-list   /
            "LET"? var "=" expr /
            "GOTO" expr  /
            "GOSUB" expr  /
            "FOR" var "=" expr "TO" expr ("STEP" expr)? /
            "NEXT" /
            "RETURN" /
            "IF" expr relop expr "THEN" statement /
            "END" /
            "REM" /
            "POKE" expr "," expr 

var = "A" | "B" | ... | "Z"
string = '"' char* '"'

pr-expr-list = (string|expression) ((','/';') (string|expression) )* ';'?
in-var-list = var (',' var)*

expr = term (('+' / '-') term)*
term = fact (('*' / '/') fact)*
fact = '-' fact / '(' expr ')' / num / var / fun

fun = 'RND' '(' expr ')' /
      'PEEK' '(' expr ')' 

relop = '<' ('>' | '=')? | '>' '='? | '='

number = digit digit*
   
digit = '0' | '1' | '2' | '3' | ... | '8' | '9'
 
EOL =  '\r'?\n' 
*/

#define SKP_MAIN
#include "skp.h"

#define TOK_PRINT  100
#define TOK_INPUT  101
#define TOK_LET    102
#define TOK_GOTO   103
#define TOK_GOSUB  104
#define TOK_FOR    105
#define TOK_NEXT   106
#define TOK_RETURN 107
#define TOK_IF     108
#define TOK_END    109
#define TOK_REM    110
#define TOK_POKE   111

skpdef(program) {
  skpmany {
    skprule(line); 
    astnoleaf;
  }
  skpmatch_("&!.");
}

skpdef(line) {
  skpmatch_("&*s");
  skpmaybe skpmatch("&D\6");
  skpmatch_("&*w");
  skprule_(statement);
  skpmatch_("&*s");
}

skpdef(statement) {
  skponce { astinfo(TOK_PRINT);
            skpstring("PRINT");
            skpmatch_("&*w");
            skpany {
              skprule(expr);
              skpor {
                skpmatch("&Q");
              }
              skpmatch_("&*w");
            } 
            skpmaybe skpstring(";");
          }

    skpor { astinfo(TOK_INPUT);
            skpstring("INPUT");
            skpmatch_("&+w");
            skpmany {
               skpmatch("&a");
               skpmatch_("&*w&?[,]&*w");
            }
          }

    skpor { astinfo(TOK_LET);
            skpstring("LET"); 
            skpmatch_("&+w");
            skpmatch("&a");
            skpmatch_("&*w=&*w");
            skprule(expr);
          }

    skpor { astinfo(TOK_GOTO);
            skpstring("GOTO");
            skpmatch_("&*w");
            skprule(expr);
          }

    skpor { astinfo(TOK_GOSUB);
            skpstring("GOSUB");
            skpmatch_("&*w");
            skprule(expr);
          }

    skpor { astinfo(TOK_FOR);
            skpstring("FOR");
            skpmatch_("&+w");
            skpmatch("&a");
            skpmatch_("&*w=&*w");
            skprule(expr);
            skpstring("TO");
            skpmatch_("&*w");
            skprule(expr);
            skpmaybe {
              skpstring("STEP");
              skpmatch_("&*w");
              skprule(expr);
            }
          }

    skpor { astinfo(TOK_NEXT); skpstring("NEXT"); }

    skpor { astinfo(TOK_RETURN); skpstring("RETURN"); }

    skpor { astinfo(TOK_IF); 
            skpstring("IF");
            skpmatch_("&*w");
            skprule(expr);
            skpmatch("<=\1<>\1<\1>\1>=");
            skpmatch_("&*w");
            skprule(expr);
            skpstring("THEN");
            skpmatch_("&+w");
            skprule(statement);
          }

    skpor { astinfo(TOK_END); skpstring("END"); }
    
    skpor { //astinfo(TOK_REM);
            skpstring_("REM");
            skpmatch_("&*!n");
          }

    skpor { astinfo(TOK_POKE);
            skpstring("POKE");
            skpmatch_("&*w");
            skprule(expr);
            skpmatch_("&*w=&*w");
            skprule(expr);
          }
}

// Since the objective is just to convert tb to C, we
// only need to check that the expressions are syntactially
// correct. We don't care about associativity, precedence etc.
skpdef(expr) {
  skprule_(term);
  skpany {
    skpmatch_("&*w");
    skpmatch_("&[-+*/]");
    skprule_(term);
  }
}

skpdef(term) {
  skpmatch_("&*w");
  skponce {
    skpstring_("(");
    skprule_(expr);
    skpstring_(")");
  }
  skpor {
    skpmatch_("&D\2&a\3");
  }
  skpor {
    skpmatch_("&*w-");
    skprule_(term);
  }
  skpmatch_("&*w");
}

char *loadsource(char *fname)
{
  FILE *f;
  int32_t size;
  char *src=NULL;

  f = fopen(fname,"rb");
  if (f) {
    fseek(f,0,SEEK_END);
    size = ftell(f);
    fseek(f,0,SEEK_SET);
    src = malloc(size+1);
    if (src) {
      if (!fread(src,size,1,f)) {
        free(src);
        src = NULL;
      };
    }
  }
  if (f) fclose(f);
  return src;
}

#define trace(...) (fprintf(stderr,__VA_ARGS__),fputc('\n',stderr))
int main(int argc, char *argv[])
{
  char *source = NULL;
  ast_t ast = NULL;

  if (argc<2) {fprintf(stderr,"Usage: tbc file.tb\n"); exit(1);}

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