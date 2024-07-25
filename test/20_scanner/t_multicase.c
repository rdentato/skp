#include "tst.h"
#define SKP_MAIN
#include "skp.h"
  
 
int keyword(char *txt, char **start, char **end)
{
  *start = txt;
  *end = txt;
  tst_note("check keyword: %s",txt);
       if (strncmp(txt,"if",2) == 0)   *end += 2;
  else if (strncmp(txt,"then",4) == 0) *end += 4;
  else if (strncmp(txt,"else",4) == 0) *end += 4;

  if (isalpha((int)((*end)[0]))) *end = txt;

  int ret = *end > *start;
  tst_note("is_keyword: %d",ret);
  return ret;
}

int comment(char *txt, char **start, char **end)
{
  *start = txt;
  char *ptr = txt;

  skpscan(txt) {
    skpcase("'*/'") { ptr = skpto; break; }
    skpcase("n") { /* keep track of line number */ }
    skpcase(".") { }
  }
  *end = ptr;
  return (ptr != NULL);
}

char *text; 
tstsuite("multi case") {

  char *from;
  char *to;
  //int len;
  int cnt;
// int ret;

  tstcase("scan identifiers") {
    text = "  @then";
    cnt = 0;
    skpscan(text) { // 1 pattern
      skpcase("+w") {   }
      skpcase("'@'","I") { cnt++; tstnote("Identifier") ;}
    }
    tstcheck(cnt>0);
    cnt = 0;
    skpscan(text) { // 1 pattern
      skpcase("+w") {   }
      skpcase("'@'",keyword) { cnt++; tstnote("Keyword") ;}
    }
    tstcheck(cnt>0);
    text = "  @pippo";
    cnt = 0;
    skpscan(text) { // 1 pattern
      skpcase("+w") {   }
      skpcase("'@'","I") { cnt++; tstnote("Identifier") ;}
    }
    tstcheck(cnt>0);
    cnt = 0;
    skpscan(text) { // 1 pattern
      skpcase("+w") {   }
      skpcase("'@'",keyword) { cnt++; tstnote("Keyword") ;}
    }
    tstcheck(cnt==0);
  }

  tstcase("C comment") {
    text = "/* this is a\n quite big comment */ xx";
    cnt = 0;
    to = text;
    from = text;
    skpscan(text,&from,&to) {
      skpcase("+w") {   }
      skpcase("'/*'",comment) { cnt ++; skpstop(1);}
    }
    tstcheck(cnt>0);
    tstnote("comment: '%.*s'",(int)(to-from),from);
  }
}