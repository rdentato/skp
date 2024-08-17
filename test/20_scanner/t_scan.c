#include "tst.h"
#define SKP_MAIN
#include "skp.h"

skpdef(identifier)
{
  char *st = skpstart;
  char *nd = skpstart;
  int ret = 0;
  tst_note("check identifier: %s",skpstart);

  while (isalpha(*nd)) nd++;

  ret = nd > st;
  if (ret) {
    skpfrom[0] = st;
    skpto[0] = nd;
  }
  tst_note("is identifier: %d",ret);
  return ret;
}

skpdef(number)
{
  char *st = skpstart;
  char *nd = skpstart;
  int ret = 0;
  tst_note("check number: %s",skpstart);

  while (isdigit(*nd)) nd++;

  ret = nd > st;
  if (ret) {
    skpfrom[0] = st;
    skpto[0] = nd;
  }
  tst_note("is number: %d",ret);
  return ret;
}

tstsuite("Scan") {
  char *next = "";
  char *text;
  int count = 0;

  tstcase("Matching scan") {
    text = "ab423xy?";
    next = text;
    skpscan(next,&next) {
      ifskp("+a") { 
        tstcheck(skplen(0) == 2);
        tstnote("ALPHA: %.*s",skplen(0),skpfrom[0]); 
      }
      elseifskp("+d") { 
        tstcheck(skplen(0) == 3);
        tstnote("DIGITS: %.*s",skplen(0),skpfrom[0]); 
      }
    }
    tstcheck(*next == '?');

    text = "ab423xy";
    next = text;
    count = 0;
    skpscan(next,&next) {
      ifskp("+a") { 
        tstcheck(skplen(0) == 2);
        tstnote("ALPHA: %.*s",skplen(0),skpfrom[0]); 
        count++;
      }
      elseifskp("+d") { 
        tstcheck(skplen(0) == 3);
        tstnote("DIGITS: %.*s",skplen(0),skpfrom[0]); 
        count++;
      }
    }
    tstcheck(*next == '\0');
    tstcheck(count == 3);

    next = text;
    count = 0;
    skpscan(next) {
      ifskp("+a") { 
        tstcheck(skplen(0) == 2);
        tstnote("ALPHA: %.*s",skplen(0),skpfrom[0]); 
        count++;
      }
      elseifskp("+d") { 
        tstcheck(skplen(0) == 3);
        tstnote("DIGITS: %.*s",skplen(0),skpfrom[0]); 
        count++;
      }
    }
    tstcheck(*next == *text);
    tstcheck(count == 3);
  }

  tstcase("Matching scan (with whileskp)") {
    text = "ab423xy?";
    next = text;
    skpgroup(next,&next) {
      manyskp {
        ifskp("+a") { 
          tstcheck(skplen(0) == 2);
          tstnote("ALPHA: %.*s",skplen(0),skpfrom[0]); 
        }
        elseifskp("+d") { 
          tstcheck(skplen(0) == 3);
          tstnote("DIGITS: %.*s",skplen(0),skpfrom[0]); 
        }
      }
    }
    tstcheck(*next == '?');

    text = "ab423xy";
    next = text;
    count = 0;
    skpgroup(next,&next) manyskp {
      ifskp("+a") { 
        tstcheck(skplen(0) == 2);
        tstnote("ALPHA: %.*s",skplen(0),skpfrom[0]); 
        count++;
      }
      elseifskp("+d") { 
        tstcheck(skplen(0) == 3);
        tstnote("DIGITS: %.*s",skplen(0),skpfrom[0]); 
        count++;
      }
    }
    tstcheck(*next == '\0');
    tstcheck(count == 3);

    next = text;
    count = 0;
    skpscan(next) {
      ifskp("+a") { 
        tstcheck(skplen(0) == 2);
        tstnote("ALPHA: %.*s",skplen(0),skpfrom[0]); 
        count++;
      }
      elseifskp("+d") { 
        tstcheck(skplen(0) == 3);
        tstnote("DIGITS: %.*s",skplen(0),skpfrom[0]); 
        count++;
      }
    }
    tstcheck(*next == *text);
    tstcheck(count == 3, "Count = %d",count);
  }


  tstcase("Non matching switch") {
    text = "?ab423xy";
    next = text;
    count = 0;
    skpscan(next,&next) {
      ifskp("+a") { 
        tstcheck(skplen(0) == 2);
        tstnote("ALPHA: %.*s",skplen(0),skpfrom[0]); 
        count++;
      }
      elseifskp("+d") { 
        tstcheck(skplen(0) == 3);
        tstnote("DIGITS: %.*s",skplen(0),skpfrom[0]); 
        count++;
      }
    }
    tstcheck(*next == '?');
    tstcheck(count == 0);
  }

  tstcase("Matching with multi") {
    text = "(pluto) ( pippo ) (3) ";
    next = text;
    count = 0;
    skpscan(next,&next) {
      ifskp("S '(' S",identifier,"S ')' S") { 
        tstcheck(skplen(2) == 5);
        tstnote("id: %.*s",skplen(2),skpfrom[2]); 
        count++;
      }
      elseifskp("S '(' S",number,"S ')' S") { 
        tstcheck(skplen(2) == 1);
        tstnote("num: %.*s",skplen(2),skpfrom[2]); 
        count++;
      }
    }
    tstcheck(*next == '\0');
    tstcheck(count == 3);
  }
}
