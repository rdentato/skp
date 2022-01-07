# Lox

This language is defined in the great book "*Crafting Interpreters*" by Robert Nystrom.
Visit the [Crafting Interpreters](http://craftinginterpreters.com/) website for more details.

There exist a lot of implementation for this language and I thought it was a good idea to 
see how easy would have been to create a parser for this language (that has functions, classes, etc)

It turned out it was very easy (thanks to the Lox grammar being well defined and well documented).
To create the parser I only had to do the following:

  - Copy the Lox grammar verbatim from the book (in the [Appendix I](http://craftinginterpreters.com/appendix-i.html)) in the file `lox.bnf`
  - Convert it to the `skp` format (just a matter of replacing `|` with `/`, using `skp` patterns etc).
  - Fix a small issue with the grammar (see below).
  - Use `skpgen` to generate the parser.

The issue I had to fix had to do with the following rules:

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
