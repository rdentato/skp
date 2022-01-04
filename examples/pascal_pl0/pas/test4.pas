CONST A = 8, B=2 ;
VAR X,Y ;
begin 
  X := A ;
  Y := 8 ; 
  if X < 42 THEN  BEGIN 
     Y := X + B ;
     X := A
  END
END.
