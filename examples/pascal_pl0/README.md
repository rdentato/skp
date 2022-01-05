# PL0 (mini Pascal)

This is a full example of how to use grammars as defined by `skpgen` to build a parser.

`pl0` converts a program written in PL0 into a C program you can compile and execute.

The following program tests the Collatz procedure and contains almost all the instructions supported by PL0. Note that only one level of nesting is allowed for the functions.
 
``` Pascal
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

```

The file `pl0.skp` contains the grammar in the format accepted by `skpgen`.
The conversion from the AST to C is in the file `pl0_genc.c`.
