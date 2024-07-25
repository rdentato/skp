
#include "tst.h"
#define SKP_MAIN
#include "skp.h"
  

/* [Patterns]

   C  case sensitive (ASCII) comparison
   U  utf-8 encoding (or ASCII/ISO-8859)

   *  zero or more match
   ?  zero or one match
   +  one or more match
   !  negate

   &  set goal
   !& set negative goal

   [...] set
  
   .  (any non \0 character)
   !. (end of text)

   >  skip to the start of pattern

   & the character '&'
*/

tstsuite("Basic Patterns")
{
  int alt;
  char *text = "123X";
  char *from;
  char *to;
  int len;

  tstcase("Q  Quoted string with '\' as escape") {
    text = "'xyz'";
    tstcheck((alt=skp(text,"Q",&from,&to)) == 1);
    tstcheck((len = (int)(to-from)) == 5);
    tstcheck((alt=skp(text,"!Q",&from,&to)) == 0, "expected 0 got %d", alt);
    text = "'abc";
    tstcheck(skp(text,"Q",&from,&to) == 0);
    tstcheck(skp(text,"!Q",&from,&to) == 1);
    tstcheck((int)(to-from) == 0);

    text = "'x\\'y' ";
    tstcheck((alt=skp(text,"Q",&from,&to)) == 1);
    tstcheck((int)(to-from) == 6);
    tstcheck((alt=skp(text,"!Q",&from,&to)) == 0, "expected 0 got %d", alt);
    text = "'abc";
    tstcheck(skp(text,"Q",&from,&to) == 0);
    tstcheck(skp(text,"!Q",&from,&to) == 1);
    tstcheck((int)(to-from) == 0);

    text = "`xyz`";
    tstcheck((alt=skp(text,"Q",&from,&to)) == 1);
    tstcheck((int)(to-from) == 5);
    tstcheck((alt=skp(text,"!Q",&from,&to)) == 0, "expected 0 got %d", alt);
    text = "`abc";
    tstcheck(skp(text,"Q",&from,&to) == 0);
    tstcheck(skp(text,"!Q",&from,&to) == 1);
    tstcheck((int)(to-from) == 0);

    text = "\"xyz\"";
    tstcheck((alt=skp(text,"Q",&from,&to)) == 1);
    tstcheck((int)(to-from) == 5);
    tstcheck((alt=skp(text,"!Q",&from,&to)) == 0, "expected 0 got %d", alt);
    text = "\"abc";
    tstcheck(skp(text,"Q",&from,&to) == 0);
    tstcheck(skp(text,"!Q",&from,&to) == 1);
    tstcheck((int)(to-from) == 0);
  }

  tstcase("B  Balanced sequence of parenthesis (can be '()''[]''{}')") {
    text = "(xyz)";
    tstcheck((alt=skp(text,"B",&from,&to)) == 1);
    tstcheck((len = (int)(to-from)) == 5);
    tstcheck((alt=skp(text,"!B",&from,&to)) == 0, "expected 0 got %d", alt);
    text = "(abc";
    tstcheck(skp(text,"B",&from,&to) == 0);
    tstcheck(skp(text,"!B",&from,&to) == 1);
    tstcheck((int)(to-from) == 0);

    text = "(x(yz))";
    tstcheck((alt=skp(text,"B",&from,&to)) == 1);
    tstcheck((len = (int)(to-from)) == 7);
    tstcheck((alt=skp(text,"!B",&from,&to)) == 0, "expected 0 got %d", alt);
    text = "(a(bc)";
    tstcheck(skp(text,"B",&from,&to) == 0);
    tstcheck(skp(text,"!B",&from,&to) == 1);
    tstcheck((int)(to-from) == 0);
     
    text = "[xyz]";
    tstcheck((alt=skp(text,"B",&from,&to)) == 1);
    tstcheck((len = (int)(to-from)) == 5);
    tstcheck((alt=skp(text,"!B",&from,&to)) == 0, "expected 0 got %d", alt);
    text = "[abc)";
    tstcheck(skp(text,"B",&from,&to) == 0);
    tstcheck(skp(text,"!B",&from,&to) == 1);
    tstcheck((int)(to-from) == 0);

    text = "[x[yz]]";
    tstcheck((alt=skp(text,"B",&from,&to)) == 1);
    tstcheck((len = (int)(to-from)) == 7);
    tstcheck((alt=skp(text,"!B",&from,&to)) == 0, "expected 0 got %d", alt);
    text = "[a[bc]";
    tstcheck(skp(text,"B",&from,&to) == 0);
    tstcheck(skp(text,"!B",&from,&to) == 1);
    tstcheck((int)(to-from) == 0);
  }
  tstcase("() Balanced parenthesis (only '()')") {
    text = "(xyz)";
    tstcheck((alt=skp(text,"()",&from,&to)) == 1);
    tstcheck((len = (int)(to-from)) == 5);
    tstcheck((alt=skp(text,"!()",&from,&to)) == 0, "expected 0 got %d", alt);
    text = "(abc";
    tstcheck(skp(text,"()",&from,&to) == 0);
    tstcheck(skp(text,"!()",&from,&to) == 1);
    tstcheck((int)(to-from) == 0);

    text = "abc";
    tstcheck(skp(text,"()",&from,&to) == 0);
    tstcheck(skp(text,"!()",&from,&to) == 1);
    tstcheck((int)(to-from) == 0);

    text = "(x(yz))";
    tstcheck((alt=skp(text,"()",&from,&to)) == 1);
    tstcheck((len = (int)(to-from)) == 7);
    tstcheck((alt=skp(text,"!()",&from,&to)) == 0, "expected 0 got %d", alt);
    text = "(a(bc)";
    tstcheck(skp(text,"()",&from,&to) == 0);
    tstcheck(skp(text,"!()",&from,&to) == 1);
    tstcheck((int)(to-from) == 0);
    
  }
  tstcase("I  Identifier ([_A-Za-z][_0-9A-Za-z]*)") {
    text = "xy3z_t";
    tstcheck((alt=skp(text,"I",&from,&to)) == 1);
    tstcheck((len = (int)(to-from)) == 6);
    tstcheck((alt=skp(text,"!I",&from,&to)) == 0, "expected 0 got %d", alt);
    text = "3xyz_t";
    tstcheck(skp(text,"I",&from,&to) == 0);
    tstcheck((alt=skp(text,"!I",&from,&to)) == 1, "expected 1 got %d", alt);
    tstcheck(len=((int)(to-from)) == 0);
    text = "_xy3zt";
    tstcheck((alt=skp(text,"I",&from,&to)) == 1);
    tstcheck((len = (int)(to-from)) == 6);
    tstcheck((alt=skp(text,"!I",&from,&to)) == 0, "expected 0 got %d", alt);
    text = ",xyz_t";
    tstcheck(skp(text,"I",&from,&to) == 0);
    tstcheck((alt=skp(text,"!I",&from,&to)) == 1, "expected 1 got %d", alt);
    tstcheck((int)(to-from) == 0);
  }
  tstcase("N  up to the end of line") {
    text = "xy3z\nt";
    tstcheck((alt=skp(text,"N",&from,&to)) == 1);
    tstcheck((len = (int)(to-from)) == 4,"Expected 4, got: %d",len);
    tstcheck((alt=skp(text,"!N",&from,&to)) == 0, "expected 0 got %d", alt);
    text = "xy3z\r\nt";
    tstcheck((alt=skp(text,"N",&from,&to)) == 1);
    tstcheck((len = (int)(to-from)) == 4,"Expected 4, got: %d",len);
    tstcheck((alt=skp(text,"!N",&from,&to)) == 0, "expected 0 got %d", alt);
    text = "xy3z\xC2\x85t";
    tstcheck((alt=skp(text,"N",&from,&to)) == 1);
    tstcheck((len = (int)(to-from)) == 4,"Expected 4, got: %d",len);
    tstcheck((alt=skp(text,"!N",&from,&to)) == 0, "expected 0 got %d", alt);
    text = "xy3z";
    tstcheck((alt=skp(text,"N",&from,&to)) == 1);
    tstcheck((len = (int)(to-from)) == 4,"Expected 4, got: %d",len);
    tstcheck((alt=skp(text,"!N",&from,&to)) == 0, "expected 0 got %d", alt);
    
    text = "\na";
    tstcheck((alt=skp(text,"N",&from,&to)) == 1);
    tstcheck((len = (int)(to-from)) == 0,"Expected 0, got: %d",len);
    tstcheck((alt=skp(text,"!N",&from,&to)) == 1);
    tstcheck((len = (int)(to-from)) == 0,"Expected 0, got: %d",len);
    text = "";
    tstcheck((alt=skp(text,"N",&from,&to)) == 1);
    tstcheck((len = (int)(to-from)) == 0,"Expected 0, got: %d",len);
    tstcheck((alt=skp(text,"!N",&from,&to)) == 1);
    tstcheck((len = (int)(to-from)) == 0,"Expected 0, got: %d",len);
  }
  tstcase("D  integer decimal number (possibly signed)") {
    text = "342x";
    tstcheck((alt=skp(text,"D",&from,&to)) == 1);
    tstcheck((len = (int)(to-from)) == 3,"Expected 3, got: %d",len);
    tstcheck((alt=skp(text,"!D",&from,&to)) == 0, "expected 0 got %d", alt);  
    text = "x342";
    tstcheck((alt=skp(text,"D",&from,&to)) == 0);
    tstcheck((alt=skp(text,"!D",&from,&to)) == 1, "expected 0 got %d", alt);  
    tstcheck((len = (int)(to-from)) == 0,"Expected 0, got: %d",len);
    text = "-42x";
    tstcheck((alt=skp(text,"D",&from,&to)) == 1);
    tstcheck((len = (int)(to-from)) == 3,"Expected 3, got: %d",len);
    tstcheck((alt=skp(text,"!D",&from,&to)) == 0, "expected 0 got %d", alt);  
    text = "+42x";
    tstcheck((alt=skp(text,"D",&from,&to)) == 1);
    tstcheck((len = (int)(to-from)) == 3,"Expected 3, got: %d",len);
    tstcheck((alt=skp(text,"!D",&from,&to)) == 0, "expected 0 got %d", alt);  
    text = "- 42x";
    tstcheck((alt=skp(text,"D",&from,&to)) == 1);
    tstcheck((len = (int)(to-from)) == 4,"Expected 4, got: %d",len);
    tstcheck((alt=skp(text,"!D",&from,&to)) == 0, "expected 0 got %d", alt);  
    text = "+ 42x";
    tstcheck((alt=skp(text,"D",&from,&to)) == 1);
    tstcheck((len = (int)(to-from)) == 4,"Expected 4, got: %d",len);
    tstcheck((alt=skp(text,"!D",&from,&to)) == 0, "expected 0 got %d", alt);  
  }
  tstcase("F  floating point number (possibly with sign and exponent)") {
    text = "x342x";
    tstcheck((alt=skp(text,"F",&from,&to)) == 0);
    tstcheck((alt=skp(text,"!F",&from,&to)) == 1, "expected 1 got %d", alt);  
    
    text = "342x";
    tstcheck((alt=skp(text,"F",&from,&to)) == 1);
    tstcheck((len = (int)(to-from)) == 3,"Expected 3, got: %d",len);
    tstcheck((alt=skp(text,"!F",&from,&to)) == 0, "expected 0 got %d", alt);  
    
    text = "3.2x";
    tstcheck((alt=skp(text,"F",&from,&to)) == 1);
    tstcheck((len = (int)(to-from)) == 3,"Expected 3, got: %d",len);
    tstcheck((alt=skp(text,"!F",&from,&to)) == 0, "expected 0 got %d", alt);  

    text = "-342x";
    tstcheck((alt=skp(text,"F",&from,&to)) == 1);
    tstcheck((len = (int)(to-from)) == 4,"Expected 4, got: %d",len);
    tstcheck((alt=skp(text,"!F",&from,&to)) == 0, "expected 0 got %d", alt);  
    
    text = "-3.2x";
    tstcheck((alt=skp(text,"F",&from,&to)) == 1);
    tstcheck((len = (int)(to-from)) == 4,"Expected 4, got: %d",len);
    tstcheck((alt=skp(text,"!F",&from,&to)) == 0, "expected 0 got %d", alt);  

    text = "+342x";
    tstcheck((alt=skp(text,"F",&from,&to)) == 1);
    tstcheck((len = (int)(to-from)) == 4,"Expected 4, got: %d",len);
    tstcheck((alt=skp(text,"!F",&from,&to)) == 0, "expected 0 got %d", alt);  
    
    text = "+3.2x";
    tstcheck((alt=skp(text,"F",&from,&to)) == 1);
    tstcheck((len = (int)(to-from)) == 4,"Expected 4, got: %d",len);
    tstcheck((alt=skp(text,"!F",&from,&to)) == 0, "expected 0 got %d", alt);  
    
    text = ".234X";
    tstcheck((alt=skp(text,"F",&from,&to)) == 1);
    tstcheck((len = (int)(to-from)) == 4,"Expected 4, got: %d",len);
    tstcheck((alt=skp(text,"!F",&from,&to)) == 0, "expected 0 got %d", alt);  
    
    text = "-.23x";
    tstcheck((alt=skp(text,"F",&from,&to)) == 1);
    tstcheck((len = (int)(to-from)) == 4,"Expected 4, got: %d",len);
    tstcheck((alt=skp(text,"!F",&from,&to)) == 0, "expected 0 got %d", alt);  

    text = ".25e3|";
    tstcheck((alt=skp(text,"F",&from,&to)) == 1);
    tstcheck((len = (int)(to-from)) == 5,"Expected 4, got: %d",len);
    tstcheck((alt=skp(text,"!F",&from,&to)) == 0, "expected 0 got %d", alt);  
    
    text = "-.2E3|";
    tstcheck((alt=skp(text,"F",&from,&to)) == 1);
    tstcheck((len = (int)(to-from)) == 5,"Expected 4, got: %d",len);
    tstcheck((alt=skp(text,"!F",&from,&to)) == 0, "expected 0 got %d", alt);  

    text = ".25e-1.3|";
    tstcheck((alt=skp(text,"F",&from,&to)) == 1);
    tstcheck((len = (int)(to-from)) == 8,"Expected 8, got: %d",len);
    tstcheck((alt=skp(text,"!F",&from,&to)) == 0, "expected 0 got %d", alt);  
    
    text = ".25E-1.3|";
    tstcheck((alt=skp(text,"F",&from,&to)) == 1);
    tstcheck((len = (int)(to-from)) == 8,"Expected 8, got: %d",len);
    tstcheck((alt=skp(text,"!F",&from,&to)) == 0, "expected 0 got %d", alt);  

    text = "3.2e-1.3|";
    tstcheck((alt=skp(text,"F",&from,&to)) == 1);
    tstcheck((len = (int)(to-from)) == 8,"Expected 8, got: %d",len);
    tstcheck((alt=skp(text,"!F",&from,&to)) == 0, "expected 0 got %d", alt);  
    
    text = "-3.E-1.3|";
    tstcheck((alt=skp(text,"F",&from,&to)) == 1);
    tstcheck((len = (int)(to-from)) == 8,"Expected 8, got: %d",len);
    tstcheck((alt=skp(text,"!F",&from,&to)) == 0, "expected 0 got %d", alt);  
  }
  tstcase("X  hex number (possibly with leading 0x)") {
    text = "x342x";
    tstcheck((alt=skp(text,"X",&from,&to)) == 0);
    tstcheck((alt=skp(text,"!X",&from,&to)) == 1, "expected 1 got %d", alt);  
    
    text = "0x3F|";
    tstcheck((alt=skp(text,"X",&from,&to)) == 1);
    tstcheck((len = (int)(to-from)) == 4,"Expected 4, got: %d",len);
    tstcheck((alt=skp(text,"!X",&from,&to)) == 0, "expected 0 got %d", alt);  
  
    text = "E03F|";
    tstcheck((alt=skp(text,"X",&from,&to)) == 1);
    tstcheck((len = (int)(to-from)) == 4,"Expected 4, got: %d",len);
    tstcheck((alt=skp(text,"!X",&from,&to)) == 0, "expected 0 got %d", alt);  
  
  }

}
