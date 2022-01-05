PROGRAM collatz(start)
  VAR X;
  
  FUNCTION collatz(n) BEGIN
    if odd n then n := 3*n +1 ;
    if even n then n :=  n / 2 ;
    return n
  END; 

BEGIN 
  x := start;
  if x < 1 THEN X := 1;
  WHILE X > 0 DO BEGIN 
    WHILE x <> 1 DO BEGIN
      x := collatz(x);
      write 'x = '; write x ;  write '\n' 
    END;
  
    write 'Start value (0 to quit): ';
    read x 
  END
END.
