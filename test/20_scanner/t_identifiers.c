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

tstsuite("User defined functions") {
 char *text;
 // char *to;
  int len;
  int cnt;
// int ret;
  tstcase("scan identifiers") {
    skpscan("pippo pluto") {
        skpcase("I") { len = skpto-skpfrom; 
                       tstcheck(len > 0);
                       tstnote("id: %.*s",len, skpfrom);
                       tstcheck(*skpfrom == 'p');
                     }

        skpcase("+s") { tstcheck(*skpfrom == ' '); }
    }

    cnt = 0;
    skpscan("pippo 32 pluto") {
        skpcase("I") { len = skpto-skpfrom; 
                       tstcheck(len > 0);
                       tstnote("id: %.*s",len, skpfrom);
                       tstcheck(*skpfrom == 'p');
                       cnt++;
                     }

        skpcase("+s") { tstcheck(*skpfrom == ' '); }

        skpcase(NULL) { tstcheck(*skpfrom == '3'); }
    }
    tstcheck(cnt == 1);

    cnt = 0;
    skpscan("pippo 32 pluto") {
        skpcase("I") { len = skpto-skpfrom; 
                       tstcheck(len > 0);
                       tstnote("id: %.*s",len, skpfrom);
                       tstcheck(*skpfrom == 'p');
                       cnt++;
                     }

        skpcase("+s") { tstcheck(*skpfrom == ' '); }

        skpcase(1) { tstcheck(*skpfrom == '3'); }
    }
    tstcheck(cnt == 1);
    
    cnt = 0;
    skpscan("pippo 32 pluto") {
        skpcase("I") { len = skpto-skpfrom; 
                       tstcheck(len > 0);
                       tstnote("id: %.*s",len, skpfrom);
                       tstcheck(*skpfrom == 'p');
                       cnt++;
                     }

        skpcase("+s") { tstcheck(*skpfrom == ' '); }

        skpcase(".") { skpto++; }
    }
    tstcheck(cnt == 2);

    skpscan("pippo if pluto") {
        skpcase(keyword) {
          len = skpto-skpfrom; 
          tstcheck(len > 0);
          tstnote("kwd: %.*s",len, skpfrom);
          tstcheck(*skpfrom == 'i');
        }

        skpcase("I") {
          len = skpto-skpfrom; 
          tstcheck(len > 0);
          tstnote("id: %.*s",len, skpfrom);
          tstcheck(*skpfrom == 'p');
        }

        skpcase("+s") { tstcheck(*skpfrom == ' '); }
    }
  }

  tstcase("A simple switch") {

    text = "topo lino";

    cnt = 0;
    skpscan(text) {
      skpcase("D") { cnt = -1; break; }
      skpcase("I") { cnt++; }
      skpcase(".") { }
    }
    tstcheck(cnt == 2);

    cnt = 0;
    skpswitch(text) {
      skpcase("D") { cnt = -1; break; }
      skpcase("I") { cnt++; }
      skpcase(".") { }
    }
    tstcheck(cnt == 1);

    text = "32 topo";
    cnt = 0;
    skpscan(text) {
      skpcase("D") { cnt = -1; break; }
      skpcase("I") { cnt++; }
      skpcase(".") { }
    }
    tstcheck(cnt == -1);

   cnt = 0;
    skpswitch(text) {
      skpcase("D") { cnt = -1; break; }
      skpcase("I") { cnt++; }
      skpcase(".") { }
    }
    tstcheck(cnt == -1);
  }
}