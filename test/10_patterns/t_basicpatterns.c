
#include "tst.h"
#define SKP_MAIN
#include "skp.h"

  
tstsuite("Basic Patterns")
{
  //int alt;
  char *text = "123X";
  char *from;
  char *to;
  int len;

  tstcase("a  ASCII alphabetic char"){
    text = "xyz4";
    tstcheck(skp(text,"a",&from,&to) == 1);
    tstcheck(skp(text,"+a",&from,&to) == 1);
    tstcheck((int)(to-from) == 3);
    tstcheck(skp(text,"*a",&from,&to) == 1);
    tstcheck((int)(to-from) == 3);
    tstcheck(skp(text,"!a",&from,&to) == 0);
    text = "3abc";
    tstcheck(skp(text,"a",&from,&to) == 0);
    tstcheck(skp(text,"!a",&from,&to) == 1);
  }

  tstcase("l  ASCII lower case"){
    text = "xyz4";
    tstcheck(skp(text,"l",&from,&to) == 1);
    tstcheck(skp(text,"+l",&from,&to) == 1);
    tstcheck((int)(to-from) == 3);
    tstcheck(skp(text,"*l",&from,&to) == 1);
    tstcheck((int)(to-from) == 3);
    tstcheck(skp(text,"!l",&from,&to) == 0);
    text = "Xabc";
    tstcheck(skp(text,"l",&from,&to) == 0);
    tstcheck(skp(text,"!l",&from,&to) == 1);
    text = "#abc";
    tstcheck(skp(text,"l",&from,&to) == 0);
    tstcheck(skp(text,"!l",&from,&to) == 1);
  }

  tstcase("u  ASCII upper case"){
    text = "XYZ4";
    tstcheck(skp(text,"u",&from,&to) == 1);
    tstcheck(skp(text,"+u",&from,&to) == 1);
    tstcheck((int)(to-from) == 3);
    tstcheck(skp(text,"*u",&from,&to) == 1);
    tstcheck((int)(to-from) == 3);
    tstcheck(skp(text,"!u",&from,&to) == 0);
    text = "abc";
    tstcheck(skp(text,"u",&from,&to) == 0);
    tstcheck(skp(text,"!u",&from,&to) == 1);
    text = "#abc";
    tstcheck(skp(text,"u",&from,&to) == 0);
    tstcheck(skp(text,"!u",&from,&to) == 1);
  }

  tstcase("d  decimal digit"){
    text = "123X";
    tstcheck(skp(text,"d",&from,&to) == 1);
    tstcheck(skp(text,"+d",&from,&to) == 1);
    tstcheck((int)(to-from) == 3);
    tstcheck(skp(text,"*d",&from,&to) == 1);
    tstcheck((int)(to-from) == 3);
    tstcheck(skp(text,"!d",&from,&to) == 0);
    text = "abc";
    tstcheck(skp(text,"d",&from,&to) == 0);
    tstcheck(skp(text,"!d",&from,&to) == 1);
    text = "#abc";
    tstcheck(skp(text,"d",&from,&to) == 0);
    tstcheck(skp(text,"!d",&from,&to) == 1);
  }

  tstcase("x  hex digit"){
    text = "1FeX";
    tstcheck(skp(text,"x",&from,&to) == 1);
    tstcheck(skp(text,"+x",&from,&to) == 1);
    tstcheck((int)(to-from) == 3);
    tstcheck(skp(text,"*x",&from,&to) == 1);
    tstcheck((int)(to-from) == 3);
    tstcheck(skp(text,"!x",&from,&to) == 0);
    text = "xyz";
    tstcheck(skp(text,"x",&from,&to) == 0);
    tstcheck(skp(text,"!x",&from,&to) == 1);
    text = "#abc";
    tstcheck(skp(text,"x",&from,&to) == 0);
    tstcheck(skp(text,"!x",&from,&to) == 1);
  }

  tstcase("w  white space (includes some Unicode spaces)"){
    text = " \t\xC2\xA0P";
    tstcheck(skp(text,"w",&from,&to) == 1);
    tstcheck(skp(text,"+w",&from,&to) == 1);
    tstcheck((int)(len=to-from) == 4,"Expected len = 4, got %d",len);
    tstcheck(skp(text,"*w",&from,&to) == 1);
    tstcheck((int)(len=to-from) == 4,"Expected len = 4, got %d",len);
    tstcheck(skp(text,"!w",&from,&to) == 0);
    text = " \t\xA0P";
    tstcheck(skp(text,"!Uw",&from,&to) == 1);
    tstcheck(skp(text,"!U+w",&from,&to) == 1);
    tstcheck((int)(len=to-from) == 3,"Expected len = 3, got %d",len);
    tstcheck(skp(text,"!U*w",&from,&to) == 1);
    tstcheck((int)(len=to-from) == 3,"Expected len = 3, got %d",len);
    tstcheck(skp(text,"!U!w",&from,&to) == 0);
    text = "xyz";
    tstcheck(skp(text,"w",&from,&to) == 0);
    tstcheck(skp(text,"!w",&from,&to) == 1);
    text = "#abc";
    tstcheck(skp(text,"w",&from,&to) == 0);
    tstcheck(skp(text,"!w",&from,&to) == 1);
  }

  tstcase("s  white space and vertical spaces (e.g. LF)"){
    text = "\n\f\t ";
    tstcheck(skp(text,"s",&from,&to) == 1);
    tstcheck(skp(text,"+s",&from,&to) == 1);
    tstcheck((int)(to-from) == 4);
    tstcheck(skp(text,"*s",&from,&to) == 1);
    tstcheck((int)(to-from) == 4);
    tstcheck(skp(text,"!s",&from,&to) == 0);
    text = "\n\xC2\x85\r";
    tstcheck(skp(text,"s",&from,&to) == 1);
    tstcheck(skp(text,"+s",&from,&to) == 1);
    tstcheck((int)(to-from) == 4);
    tstcheck(skp(text,"*s",&from,&to) == 1);
    tstcheck((int)(to-from) == 4);
    tstcheck(skp(text,"!s",&from,&to) == 0);
    text = "xyz";
    tstcheck(skp(text,"s",&from,&to) == 0);
    tstcheck(skp(text,"!s",&from,&to) == 1);
    text = "abc";
    tstcheck(skp(text,"s",&from,&to) == 0);
    tstcheck(skp(text,"!s",&from,&to) == 1);
    tstcheck((int)(to-from) == 0,"start: %p from: %p to: %p",(void *)text, (void *)from, (void *)to);
    tstcheck(skp(text,"*s",&from, &to) == 1);
    tstcheck((int)(to-from) == 0);
  }

  tstcase("c  control"){
    text = "\n\f\x1B\01 ";
    tstcheck(skp(text,"c",&from,&to) == 1);
    tstcheck(skp(text,"+c",&from,&to) == 1);
    tstcheck((int)(to-from) == 4);
    tstcheck(skp(text,"*c",&from,&to) == 1);
    tstcheck((int)(to-from) == 4);
    tstcheck(skp(text,"!c",&from,&to) == 0);
    text = "\xC2\x9F\xC2\x85 ";
    tstcheck(skp(text,"c",&from,&to) == 1);
    tstcheck(skp(text,"+c",&from,&to) == 1);
    tstcheck((int)(to-from) == 4);
    tstcheck(skp(text,"*c",&from,&to) == 1);
    tstcheck((int)(to-from) == 4);
    tstcheck(skp(text,"!c",&from,&to) == 0);
    text = "xyz";
    tstcheck(skp(text,"c",&from,&to) == 0);
    tstcheck(skp(text,"!c",&from,&to) == 1);
    text = "#abc";
    tstcheck(skp(text,"c",&from,&to) == 0);
    tstcheck(skp(text,"!c",&from,&to) == 1);   
  }
  tstcase("n  newline"){
    text = "\n\f\r ";
    tstcheck(skp(text,"n",&from,&to) == 1);
    tstcheck(skp(text,"+n",&from,&to) == 1);
    tstcheck((int)(to-from) == 3);
    tstcheck(skp(text,"*n",&from,&to) == 1);
    tstcheck((int)(to-from) == 3);
    tstcheck(skp(text,"!n",&from,&to) == 0);
    text = "\xC2\x85\x0D\x0A ";
    tstcheck(skp(text,"n",&from,&to) == 1);
    tstcheck(skp(text,"+n",&from,&to) == 1);
    tstcheck((int)(to-from) == 4);
    tstcheck(skp(text,"*n",&from,&to) == 1);
    tstcheck((int)(to-from) == 4);
    tstcheck(skp(text,"!n",&from,&to) == 0);
    text = "xyz";
    tstcheck(skp(text,"n",&from,&to) == 0);
    tstcheck(skp(text,"!n",&from,&to) == 1);
    text = "#abc";
    tstcheck(skp(text,"n",&from,&to) == 0);
    tstcheck(skp(text,"!n",&from,&to) == 1);   
  }
  tstcase("@  alfanumeric "){
    text = "xyz4#";
    tstcheck(skp(text,"@",&from,&to) == 1);
    tstcheck(skp(text,"+@",&from,&to) == 1);
    tstcheck((int)(to-from) == 4);
    tstcheck(skp(text,"*@",&from,&to) == 1);
    tstcheck((int)(to-from) == 4);
    tstcheck(skp(text,"!@",&from,&to) == 0);
    text = "#3abc";
    tstcheck(skp(text,"@",&from,&to) == 0);
    tstcheck(skp(text,"!@",&from,&to) == 1);
  }
  tstcase(".  any character (UTF-8 or ISO character)"){
    text = "X";
    tstcheck(skp(text,".",&to)== 1);
    tstcheck((len = (int)(to-text)) == 1);
    text = "è";
    tstcheck(skp(text,"!U.",&to)== 1);
    tstcheck((len = (int)(to-text)) == 1);
    text = "è";
    tstcheck(skp(text,".",&to)== 1);
    tstcheck((len = (int)(to-text)) == 2);
    text = "è";
    tstcheck(skp(text,"!U.",&to)== 1);
    tstcheck((len = (int)(to-text)) == 1);
  }

  tstcase("Always match null string") {
    text ="X";

    tstcheck(skp(text,0,&to) == 1);
    tstcheck((len = (int)(to-text)) == 0);
    
    tstcheck(skp(text,"",&to) == 1);
    tstcheck((len = (int)(to-text)) == 0);
    
    tstcheck(skp(text,"*d",&to) == 1);
    tstcheck((len = (int)(to-text)) == 0);
    
    tstcheck(skp(text,"!s",&to) == 1);
    tstcheck((len = (int)(to-text)) == 0);
    
    text = "X3%%%";
    tstcheck(skp(text,"&ad",&to) == 1);
    tstcheck((len = (int)(to-text)) == 0,"start: %p to: %p",(void *)text, (void *)to);
    
    tstcheck(skp(text,"!&d3",&to) == 1);
    tstcheck((len = (int)(to-text)) == 0,"start: %p to: %p",(void *)text, (void *)to);

    text = "";
    tstcheck(skp(text,".",&to) == 0);
    tstcheck(skp(text,"!.",&to) == 1);
    tstcheck((len = (int)(to-text)) == 0,"start: %p to: %p",(void *)text, (void *)to);

  }
}
