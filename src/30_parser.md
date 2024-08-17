# Parsers

## Table of Contents

## Introduction

An **Abstract Syntax Tree (AST)** is a tree-like data structure used in programming language compilers and interpreters to represent the hierarchical structure of source code. It abstracts away the syntactic details and focuses on the structure of the code's logical and semantic content, capturing the essential elements like operations, variables, and control flow.

**Parse trees**, on the other hand, represent the syntactic structure of the source code, including every detail dictated by the grammar rules of the programming language. While parse trees include all tokens and structural details, ASTs simplify this representation by omitting unnecessary nodes (like parentheses and specific syntactic sugar) and organizing the code in a way that is more directly useful for further processing, such as optimization or code generation.

In essence, the AST is derived from the parse tree but is a more abstract, condensed form that is easier to work with for subsequent stages of compilation or interpretation.

## The AST structure

     4 bytes  OPEN_NODE

     4 bytes CLOSE_NODE


     4 bytes OPEN_LEAF
     4 bytes from
     4 bytes to
     4 bytes CLOSE_LEAF

1111 1111 1111 delta CLOSE NODE
1111 1111 1110 delta CLOSE LEAF

1 rule      delta
  11 bits   20 bits

((("AA") ("xx")))

AST can have around 1M nodes and 2000 rules

  The two operation for creating an AST are:

  astopen(type) : add the "OPEN_NODE" value. Save the "start" pointer

  astclose(): find the first non-close "OPEN_NODE" (it has delta == 0).
              if there were no other nodes in between, than it's a leaf:
                    add from and to delta
                    add CLOSE_LEAF with delta
              else, it's an internal node:
                    add CLOSE_NODE with delta
              set the delta on the OPEN_NODE
              
  A "node" is the index to its "OPEN_NODE" element

  To determine if a node is a leaf, look at its closing node.

```C

@("after:Global types")

typedef struct {
  char *text;
  uint32_t *nodes;
  char     *rule_names;
  int32_t   nodes_max;
  int32_t   nodes_cnt;
  int32_t   rules_max;
  int32_t   rules_cnt;
  int32_t   current_run; // Used to check if we are ca
} *ast_t;

@("after:Utility macros")
#define AST_ERROR 0xFFFFFFFF

@("after:Public API")

ast_t astnew();
ast_t astfree(ast_t ast);
uint32_t ast_add(ast_t ast, uint32_t val);

@("after:Functions")
ast_t astnew(char *text)
{
  ast_t a;
  a = malloc(sizeof(ast_t));
  if (a) {
    a->text = text;
    a->nodes_max = 0;
    a->nodes_cnt = 0;
    a->nodes = NULL;
    a->rules_max = 0;
    a->rules_cnt = 0;
    a->rules = NULL;
    a->current_run = 1;
  }
  return a;
}

ast_t astfree(ast_t ast)
{ 
  if (ast) {
    ast->nodes_max = 0;
    ast->rules_max = 0;
    free(ast->nodes);
    free(ast->rules);
    free(ast);
  }
  return NULL;
}

uint32_t ast_add(ast_t ast, uint32_t n)
{
  if (!ast || !ast->text) return AST_ERROR;

  if (ast->nodes_cnt <= ast->nodes_max) {
    int32_t new_max = ast->nodes_max + (ast->nodes_max/2) + 1;
    new_max += (new_max & 1) ; // ensure it's even.
    uint32_t *new_nodes = realloc(ast->nodes, new_max * sizeof(uint32_t));
    if (new_nodes == NULL) return AST_ERROR;
    ast->nodes = new_nodes;
    ast->nodes_max = new_max;
  }
  ast->nodes[ast->nodes_cnt++] = n;
  return (ast->nodes_cnt-1);
}

@("after:Public API")

#define ast_node_close(a,m,s) 

#define astnode(name)   static int32_t skp_run_ ## name = -1; \
                        static int32_t skp_id_ ## name = 0; \
                        uint32_t node_open;\
                        if (skpast && skpast->current_run != skp_run_ ## name) { \
                          // TODO: REMOVE MEMOIZATION 
                          skp_id_ ## name = ast_addname(skpast, #name); \
                          skp_run_ ## name = skpast->current_run; \
                        } \
                        for (int doit = 1; \
                             doit && ((node_open = ast_add(skpast, 0x80000000 | skp_id_ ## name << 20)) != AST_ERROR); \
                             doit = ast_node_close(skpast, skpmatched, node_open))


@("after:Functions") 
{

}



```