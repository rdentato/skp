#include "tst.h"
#define SKP_MAIN
#include "skp.h"
  


tstsuite("MSUT") {
  char *next = "";
  int testcount;
  tstcase("simple must") {
    testcount = 0;
    skpgroup("ABC", &next) {
      mustskp {
        ifskp("'A'") {testcount += 1;}
        tstcheck(testcount == 1);
        ifskp("'B'") {testcount += 10;}
        tstcheck(testcount == 11);
      }
    }
    tstcheck(testcount == 11);
    tstcheck(*next == 'C');
  }

  tstcase("simple must") {
    testcount = 0;
    skpgroup("BCA", &next) {
      mustskp {
        ifskp("'A'") {testcount += 1;}
        tstcheck(testcount != 1);
        ifskp("'B'") {testcount += 10;}
        tstcheck(testcount != 11);
      }
      orskp {
        ifskp("'B'") {testcount += 2;}
        tstcheck(testcount == 2);
        ifskp("'C'") {testcount += 20;}
        tstcheck(testcount == 22);
      }
    }
    tstcheck(testcount == 22);
    tstcheck(*next == 'A');
  }

  tstcase("simple must") {
    testcount = 0;
    skpgroup("BC3", &next) {
      mustskp {
        ifskp("'A'") {testcount += 1;}
        ifskp("'B'") {testcount += 10;}
      }
      orskp {
        ifskp("'B'") {testcount += 2;}
        ifskp("'C'") {testcount += 20;}
      }
      andskp {
        ifskp("d") {testcount += 100;}
      }
    }
    tstcheck(testcount == 122);
    tstcheck(*next == '\0');
  }

  tstcase("simple must") {
    testcount = 0;
    skpgroup("3", &next) {
      mustskp {
        ifskp("'A'") {testcount += 1;}
        ifskp("'B'") {testcount += 10;}
      }
      orskp {
        ifskp("'B'") {testcount += 2;}
        ifskp("'C'") {testcount += 20;}
      }
      andskp {
        ifskp("d") {testcount += 100;}
      }
    }
    tstcheck(testcount == 0);
    tstcheck(*next == '\0');
  }

  tstcase("simple must") {
    testcount = 0;
    skpgroup("AB3", &next) {
      mayskp {
        ifskp("'A'") {testcount += 1;}
        ifskp("'B'") {testcount += 10;}
      }
      andskp {
        ifskp("d") {testcount += 100;}
      }
    }
    tstcheck(testcount == 111);
    tstcheck(*next == '\0');
  }

  tstcase("simple must") {
    testcount = 0;
    skpgroup("3", &next) {
      mayskp {
        ifskp("'A'") {testcount += 1;}
        ifskp("'B'") {testcount += 10;}
      }
      andskp {
        ifskp("d") {testcount += 100;}
      }
    }
    tstcheck(testcount == 100);
    tstcheck(*next == '\0');
  }

  tstcase("simple must") {
    testcount = 0;
    next = "\x0F";
    skpgroup("ABx", &next) {
      mayskp {
        ifskp("'A'") {testcount += 1;}
        ifskp("'B'") {testcount += 10;}
      }
      andskp {
        ifskp("d") {testcount += 100;}
      }
      tstnote("m: %d",skpmatched);
    }
    tstcheck(testcount == 11);
    tstcheck(*next == '\x0F');
  }

}