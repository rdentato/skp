PROGRAM guess(hidden)
  CONST max = 100 ;
  VAR guess ;
  FUNCTION roundtomax(x)
  BEGIN
    if x < 0    then x :=-x;
    if x > 100  then x := x - (x/max) * max ;
    return x
  END;
BEGIN 
  hidden := roundtomax(hidden);
  write 'Guess a number between 0 and ';
  write max;
  write '\n';
  guess := hidden+1;
  while guess <> hidden do begin
    write 'Your guess? ';
    read guess;
    if guess = hidden then write 'YOU GOT IT!\n';
    if guess > hidden then write 'NOOO! Too big!\n';
    if guess < hidden then write 'NOOO! Too small!\n'
  end
END.
