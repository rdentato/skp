#include <stdio.h>
#include <stdint.h>
#include "pl0.h"

static void prtlower(FILE *f, char *s, int len)
{
  while(len--) fputc(tolower((int)(*s++)),f);
}

static void addinclude(FILE *f)
{
  fputs("#include <stdio.h>\n"
        "#include <stdlib.h>\n"
        "#include <stdint.h>\n", f );
}

void generate(ast_t ast, FILE *src)
{
  int argn = 0;
  int fnret = 0;

  addinclude (src);
  astvisit(ast) {
    astifentry {
      astifnodeis(prog_name) {
        fprintf(src, "#define PROGNAME \"%.*s\"\n",astcurlen,astcurfrom);
      }

      astifnodeis(prog_arg) {
        fprintf(src,"#define ARG%02d P_",argn);
        prtlower(src,astcurfrom,astcurlen);
        fprintf(src,"\nint P_");
        prtlower(src,astcurfrom,astcurlen);
        fprintf(src," = 0;\n");
        argn++;
      }

      astifnodeis(fndef_name) {
        fprintf(src,"int F_");
        prtlower(src,astcurfrom,astcurlen);
        fputc('(',src);
        fnret = 0;
      }

      astifnodeis(fndef_comma,fncall_comma) {
        fputc(',',src);
      }

      astifnodeis(fndef_arg) {
        fprintf(src,"int P_%.*s",astcurlen,astcurfrom);
      }

      astifnodeis(fncall_name) {
        fprintf(src,"F_");
        prtlower(src,astcurfrom, astcurlen);
        fprintf(src,"(");
      }

      astifnodeis(assign_var) {
        fprintf(src,"P_");
        prtlower(src,astcurfrom, astcurlen);
        fputc('=',src);
      }

      astifnodeis(expr,condition,odd_cond) {
        fputc('(',src);
      }

      astifnodeis(even_cond) {
        fputc('!',src); fputc('(',src);
      }

      astifnodeis(vardef_name) {
        fprintf(src,"int P_");
        prtlower(src,astcurfrom,astcurlen);
        fprintf(src," = 0;\n");
      }

      astifnodeis(const_name) {
        fprintf(src,"const int P_");
        prtlower(src,astcurfrom,astcurlen);
      }

      astifnodeis(const_val) {
        fprintf(src," = %.*s;\n",astcurlen,astcurfrom);
      }

      astifnodeis(idref) {
        fprintf(src,"P_");
        prtlower(src,astcurfrom,astcurlen);
      }

      astifnodeis(CMP) {
        if (astcurnodeinfo == 1) fprintf(src,"==");
        else if (astcurnodeinfo == 2) fprintf(src,"!=");
        else fprintf(src,"%.*s",astcurlen,astcurfrom);
      }

      astifnodeis(sign, add_op, mult_op) {
        fprintf(src,"%.*s",astcurlen,astcurfrom);
      }

      astifnodeis(value) {
        fprintf(src,"%.*s",astcurlen,astcurfrom);
      }

      astifnodeis(compound_stmt) {
        fputc('{',src); fputc('\n',src);
      }

      astifnodeis(if_stmt) {
        fprintf(src,"if ");
      }
      
      astifnodeis(while_stmt) {
        fprintf(src,"while ");
      }
      
      astifnodeis(return_stmt) {
        fprintf(src,"return ");
        fnret = 1;
      }
      
      astifnodeis(write_stmt) {
        fprintf(src,"printf(");
        if (astcurnodeinfo == 1) fprintf(src,"\"%s\",","%s");
        else fprintf(src,"\"%s\",","%d");
      }
      astifnodeis(STRING) {
        fprintf(src,"\"%.*s\"",astcurlen-2,astcurfrom+1);
      }
      astifnodeis(readvar) {
        fprintf(src,"{ char buf[20]; *buf=0; fgets(buf,20,stdin); P_");
        prtlower(src,astcurfrom,astcurlen);
        fprintf(src," = atoi(buf); }\n");
      }
    }

    astifexit {
      astifnodeis(write_stmt) {
        fputc(')',src); fputc(';',src); fputc('\n',src);
      }

      astifnodeis(decls) {
        fprintf(src,"int main(int argc, char *argv[])\n{/*args: %d*/\n",argn);
        while (argn) {
          fprintf(src,"if (argc > %d) ARG%02d=atoi(argv[%d]);\n",argn,argn-1,argn);
          argn--;
        }
      }

      astifnodeis(fndef_args) {
        fprintf(src,")\n{\n");
      }

      astifnodeis(fndef) {
        if (!fnret) fprintf(src,"return 0;\n");
        fprintf(src,"}\n");
      }

      astifnodeis(fncall) {
        fprintf(src,")");
      }

      astifnodeis(program) {
        fprintf(src,"}\n");
      }

      astifnodeis(odd_cond,even_cond) {
        fprintf(src,"%%2)");
      }
      
      astifnodeis(expr,condition) {
        fputc(')',src);
      }
      astifnodeis(assignment,return_stmt,fn_stmt) {
        fprintf(src,";\n");
      }

      astifnodeis(compound_stmt) {
        fprintf(src,"}\n");
      }
    }
  }
  fprintf(stderr,"pl0 to C transformation completed.\n");
}
