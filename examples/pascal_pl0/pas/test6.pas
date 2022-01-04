
FUNCTION collatz(n) BEGIN
  n :=  n / 2;
  if odd n then n := 3*n +1 ;
  return n
END; 

BEGIN 
  write "Start value: ";
  read x ;
  WHILE x <> 0 DO BEGIN
    x := collatz(x);
    write "x = "; write x ;  write "\n" 
  END
END.
