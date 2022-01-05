PROGRAM test2(a,b)
  VAR X;

  FUNCTION norm(x,y)
  VAR XX, YY;
  BEGIN
    XX := x*x; YY := Y*Y;
    RETURN XX+YY
  END;

BEGIN
  WRITE 'a*a+b*b = ';
  X := norm(a,b);
  WRITE  X  ;
  WRITE '\n'
END.
