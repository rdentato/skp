CONST A = 8, B=2 ;
VAR X,Y ;
  function f(z,w) begin
    return b+z*w 
  end ;
begin 
  X := A ;
  Y := 8 ; 
  if X < 42 THEN  BEGIN 
     Y := X + B ;
     X := F(3,Y+3*A)
  END
END.
