PROGRAM collatz
VAR X;

FUNCTION collatz(n) BEGIN
  if odd n then n := 3*n +1 ;
  if even n then n :=  n / 2 ;
  return n
END; 

BEGIN 
  X := 0;
  WHILE x < 1 DO BEGIN
    write 'Start value: ';
    read x 
  END;

  WHILE x <> 1 DO BEGIN
    x := collatz(x);
    write 'x = '; write x ;  write '\n' 
  END
END.
