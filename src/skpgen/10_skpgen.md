# skpgen - a parser generator

## Table of Contents
- [skpgen - a parser generator](#skpgen---a-parser-generator)
  - [Table of Contents](#table-of-contents)
  - [Introduction](#introduction)
  - [Reading grammar](#reading-grammar)
  - [Extracting Rules](#extracting-rules)
  - [Generating Parser](#generating-parser)
  - [Set output file](#set-output-file)
  - [Command line](#command-line)
  - [Exceptions](#exceptions)
  - [Debugging](#debugging)

## Introduction
`skpgen` is a tool that generates a PEG defined parser.

```
    // Ordered choice: A = E1/E2/.../En       // Sequence: A = E1E2 ... En
    boolean A()                               boolean A()
    {                                         {
      if (E1()) return true;                   int p = pos;
      if (E2()) return true;                    if (!E1()) return false;
      ...                                       if (!E2()) return back(p);
                                                if (En()) return true; ...
      return false;                             if (!En()) return back(p);
    }                                           return true;
                                              }
    // And predicate: A = &E
    boolean A()                               // Not predicate: A = !E
    {                                         boolean A()
      int p = pos;                            {
      if (!E()) return false;                   int p = pos;
      return !back(p);                          if (!E()) return true;
    }                                           return back(p); ;
                                              }
    // One or more: A = E+
    boolean A()                               // Zero or more: A = E*
    {                                         boolean A()
      if (!E()) return false;                 {
      while (E());                              while (E());
      return true;                              return true;
    }                                         }
    // Zero or one: A = E?                    // Character class [s]
    boolean A()                               boolean nextIn(String s)
    {                                         {
      E();                                      if (pos>=endpos) return false;
      return true;                              if (s.indexOf(input[pos])<0)
    }                                               return false;
                                                pos++;
    // Character range [c1-c2]                  return true;
    boolean nextIn(char c1, char c2)          }
    { ... }
    // String "s"                             // Any character
    boolean next(String s)                    boolean next()
    { ... }                                   { ... }


    grammar = (__ ruledef)+  EOF ;
    ruledef = rulename __ '=' __ choices __ ';' ;
    choices =  sequence ( __ '|' sequence)* ;
    sequence = (__ term action)+ ;

    term =  match repeat? | '!' match ;
    
    match  = string | pattern | group | space | "?[_] *[^]" rulename;

    group = '(' __ choices ')' ;

    repeat = "[*+?]" ;
    rulename = "I" ;
    
    action = __ "B{}" ;

    string  = "&'\''" "Q" ;
    pattern = "&'\"'" "Q" ;

    _ = ("+s" | "'%' N$")*; 

```

```C
//  SPDX-FileCopyrightText: © 2024 Remo Dentato <rdentato@gmail.com>
//  SPDX-License-Identifier: MIT
  @(":Includes")
  #define SKP_MAIN
  #include "skp.h"
  @(":Defines")
  @(":Globals")
  @(":Functions")

  int main(int argc, char *argv[]) 
  {

    @(":Command Line")

    try {
      @(":Initialize")
      @(":Read grammar file")
      @(":Get grammar rules")
      @(":Generate parser")
    }
    @(":Catch errors")
    catch(EX_OUT_OF_MEM) {
      err("Out of memory");
    }
    catch() {
      err("Unexpected error");
    }

    @(":Cleanup")
  }
```

## Reading grammar

```C
@("after:includes")
#include "val.h"

@("after:Globals")
val_t grammar;
char *grammar_fname = NULL;

@("after:Exceptions")
EX_FILE_NOT_FOUND,
EX_OUT_OF_MEM,

@("after:Initialize")
grammar = bufnew();
if (valisnil(grammar)) throw(EX_OUT_OF_MEM);

@("after:Cleanup")
grammar = buffree(grammar);

@("after:Read grammar file")
{
  _dbgtrc("Grammar: %s",grammar_fname);
  FILE *f = stdin;
  if (grammar_fname) {
    f = fopen(grammar_fname,"rb");
    if (f == NULL) throw(EX_FILE_NOT_FOUND,grammar_fname);
  }
  bufload(grammar,f);
  _dbgtrc("grammar length: %d",buflen(grammar));
  if (errno) throw(EX_CANT_LOAD);
  if (grammar_fname) fclose(f);
}

@("after:Exceptions")
EX_CANT_LOAD,

@("after:catch error")
catch(EX_FILE_NOT_FOUND) {
  err("File not found %s",exception.msg);
}
catch(EX_FILE_CANT_LOAD) {
  err("Unable to read grammar file");
}

```

## Extracting Rules

```C
@("after:Globals")
val_t rules;
val_t rule_defs;

int linenum = 0;

@("after:Initialize")
rules = vecnew();
if (valisnil(rules)) throw(EX_OUT_OF_MEM,"rules");
rule_defs = vecnew();
if (valisnil(rule_defs)) throw(EX_OUT_OF_MEM,"rule_def");

@("after:Cleanup")
vecfree(rules);
vecfree(rule_defs);

@("after:globals")
char *code_start = NULL;
char *code_end = NULL;
uint32_t code_linenum = 0;

@("after:Functions")
int code(char *src, char **from, char **to)
{
  int count = 1;
  
  _dbgtrc("Checking code");
  code_start = src;
  code_end = src;
  code_linenum = linenum+1;

  skpscan(src, from, to) {
    skpcase("+w") ;
    skpcase("'//' N") ;
    skpcase("n") { linenum +=1 ;}
    skpcase("Q") ;
    skpcase("'{'") { count += 1; }
    skpcase("'}'") { count -= 1; if (count == 0) { code_end = skpto-1; skpstop; } }
    skpcase(".") ;
  }
  if (count != 0) { throw(EX_UNTERMINATED_RULE,"{code block}",code_linenum+1); }

  return (count == 0);
}

@("after:Get grammar rules")
_dbgtrc("Getting rules");
//fprintf(stderr,"%s\n",buf(grammar,0));
char *source = buf(grammar,0);

skpscan(source, &source) {
  skpcase("+w") ;
  skpcase("n")  { linenum += 1; }
  skpcase("'%' N") ;
  skpcase("'{'", code) ;
}

//source = tmp;
skpscan(source) {
  skpcase("+w") { _dbgtrc("Got space"); }
  skpcase("n")  { linenum += 1; }
  skpcase("'%' N") { _dbgtrc("Got comment"); }
  skpcase(ruledef) ;
  skpcase(NULL) { err("Unexpected!!!"); }
}

@("before:functions")
int ruledef(char *text, char **from, char **to);

@("after:functions")
int ruledef(char *text, char **from, char **to)
{
  char *id_start = text;
  char *id_end = text;
  if (!skp(id_start,"I",&id_end)) return 0;
  
  char *def_start = id_end;
  char *def_end = def_start;
  if (!skp(def_start,"*s '=' *s",&def_start)) return 0;
  def_end = def_start;

  id_end[0] = '\0';
  int old_linenum = linenum;

  skpscan(def_start, &def_end) {
    skpcase("+w") { }
    skpcase("n") {linenum += 1; }
    skpcase("'%' N") { }
    skpcase("Q") { }
    skpcase("';'") { *skpstart = '\0'; skpstop;}
    skpcase("'='") { throw(EX_UNTERMINATED_RULE,id_start,old_linenum+1); }
    skpcase(".") { }
  }

  if (def_start == def_end) 
    throw(EX_UNTERMINATED_RULE,id_start,old_linenum+1);

  if (!valisnil(vecget(rules,id_start))) 
    throw(EX_DUPLICATE_RULE,id_start,linenum+1);
  
  int rule_info = veccount(rule_defs);
  vecadd(rule_defs,old_linenum+1);
  vecadd(rule_defs,def_start);

  vecmap(rules,id_start,rule_info);

  _dbgtrc("Rule: %s",id_start);
  _dbgtrc("    : %s",def_start);

  *from = id_start;
  *to = def_end;
  return 1;
}

@("after:Exceptions")
EX_DUPLICATE_RULE,
EX_UNTERMINATED_RULE,

@("after:Catch errors")
catch(EX_DUPLICATE_RULE) {
  err("Duplicate definition for rule '%s' line: %d",exception.msg,exception.aux);
}
catch(EX_UNTERMINATED_RULE) {
  err("Unterminated rule definition '%s' line: %d",exception.msg,exception.aux);
}


@("before:Cleanup")
if (0) {
  val_t rule_def;
  val_t rule_name;
  int rule_info;

  rule_def = vecfirst(rules,&rule_name);
  while (!valisnil(rule_def)) {
    dbgtrc("Rule: %s",valtostring(rule_name));
    rule_info=valtointeger(rule_def);
    int lnum = valtointeger(vecget(rule_defs,rule_info));
    char *def = valtostring(vecget(rule_defs,rule_info+1));
    dbgtrc("%4d: %s",lnum,def);
    rule_def = vecnext(rules,&rule_name);
  }
}

```

## Generating Parser
 Now we have a map from rule names to ruls.

```C
@("after:Generate Parser")
  @(":Write out library code")
  @(":Write out grammar defined code")
  @(":Write out function prototypes")
  @(":Write out grammar rules")


@("after:Write out grammar defined code")
if (code_start) {
  fprintf(out_file,"#line %d \"%s\"\n",code_linenum,grammar_fname);
  fprintf(out_file,"%.*s\n",(int)(code_end-code_start),code_start);
}

@("after:Write out function prototypes")
{
  val_t rule_def;
  val_t rule_name;
  int rule_info;
  int lnum;
  char *rule_name_s;

  rule_def = vecfirst(rules,&rule_name);
  while (!valisnil(rule_def)) {
    rule_info=valtointeger(rule_def);
    lnum = valtointeger(vecget(rule_defs,rule_info));
    rule_name_s = valtostring(rule_name);
    fprintf(out_file, "int skp_R__%s(char *rulename, parser_t parser); // %s:%d\n",rule_name_s,grammar_fname,lnum);
    fprintf(out_file, "#define skp_R_%s(p) skp_R__%s(\"%s\",p)\n\n",rule_name_s,rule_name_s,rule_name_s);
    rule_def = vecnext(rules,&rule_name);
  }
}

@("after:Write out grammar rules")
{
  val_t rule_def;
  val_t rule_name;
  int rule_info;
  int lnum;
  char *rule_name_s;
  char *rule_def_s;

  rule_def = vecfirst(rules,&rule_name);
  while (!valisnil(rule_def)) {
    rule_info=valtointeger(rule_def);
    lnum = valtointeger(vecget(rule_defs,rule_info));
    rule_name_s = valtostring(rule_name);
    rule_def_s = valtostring(vecget(rule_defs,rule_info+1));

    fprintf(out_file, "#line %d \"%s\"\n",lnum,grammar_fname);
    fprintf(out_file, "int skp_R__%s(char *rulename, parser_t parser) { \n",rule_name_s);
    fprintf(out_file, "%s",rule_def_s);
    fprintf(out_file, "\n}\n");

    rule_def = vecnext(rules,&rule_name);
  }
}



```

## Set output file
```C
@("after:Globals")
char *out_fname = NULL;
FILE *out_file = NULL;

@("after:Initialize")
out_file = stdout;
if (out_fname)  {
  out_file = fopen(out_fname,"wb");
  if (out_file == NULL) throw(EX_CANT_WRITE_FILE,out_fname);
}

@("after:Cleanup")
if (out_fname != NULL) fclose(out_file);

@("after:Exceptions")
EX_CANT_WRITE_FILE,

@("after:Catch errors")
catch(EX_CANT_WRITE_FILE) {
  err("Cant create file '%s'", exception.msg);
}

@("after:CLI for output file name")
vrgarg("-o, --out filename\tOutput file name (defaults to stdout)") {
  out_fname = vrgarg;
}

```

## Command line

```C
@("after: includes")

#ifndef VRG_VERSION
  #ifndef VRGCLI
    #define VRGCLI 
    #endif
  #include "vrg.h"
#endif

@("after: Globals")
int trace = 0;

@("after: Command line")

vrgcli("version 0.9.001 (c) by Remo Dentato") {
  vrgarg("-h, --help\tHelp") {
    vrghelp();
    exit(1);
  }

  vrgarg("-t, --trace\tEnable parsing trace") {
    trace = 1;
  }

  @(":CLI for output file name")

  vrgarg("[grammar]\tGrammar file (defaults to stdin)") {
    grammar_fname = vrgarg;
  }

  vrgarg() {
    if (vrgarg[0] == '-') vrgerror("Unknown option '%s'\n",vrgarg);
  }
}

```
## Exceptions

```C

@("after:Includes")
#ifndef TRY_VERSION
#ifndef exception_info
#define exception_info char *msg; int aux;
#endif
#include "try.h"
#endif

@("after:Globals")
try_t catch = 0;

@("before:Globals")
enum EXCEPTION {
  EX_NONE = 0,
  @(":Exceptions")
  EX_LAST
};

```
## Debugging


```C
@("before:includes")
#if !defined(DEBUG) && !defined(NDEBUG)
#define DEBUG DEBUG_TEST
#endif
@("after:includes")
#ifdef DEBUG
#include "dbg.h"
#endif 

@("before:globals")
#define err(...) (fflush(stdout),fprintf(stderr,"ERROR: " __VA_ARGS__),fputc('\n',stderr))
```


