# Test files for lox
The source files in this directory have been taken from various
repositories of lox implementations.

Credits for good things go to them, blames for bad things to me :)

The file name prefix reveals from which one they come from:

     - `clox` : https://github.com/jalalmostafa/clox
     - `loxlox` : https://github.com/benhoyt/loxlox

```
assignment   = ( call DOT )? IDENTIFIER EQUAL __ assignment / logic_or ;
call         = primary ( _OPN_PAREN arguments? _CLS_PAREN / DOT IDENTIFIER )* __ ;
```

Imagine you have to parse `a.b = 3`. The rule `call` will eat any sequence
of `DOT` `IDENTIFIER` and will leave nothing to parse for the `IDENTIFIER` portion
in the `assign` rule.

The easiest way was to add a *look-ahead* pattern that will make `call` stop matching
before the last `IDENTIFIER`:

```
call         = primary ( _OPN_PAREN arguments? _CLS_PAREN / DOT IDENTIFIER _"@[.(]" )* __ ;
```

And that's done! after compiling `lox` (with `make`) You can process one of the examples
in the `test` directory and have a look at the resulting AST file.
