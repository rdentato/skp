
#include "skp.h"

 #define OP_HLT             0x00
 #define OP_NOT             0x01
 #define OP_BOL             0x0E
 #define OP_MIN             0x10
 #define OP_ISA_ANY         0x20
 #define OP_ISA_NOTEOL      0x21
 #define OP_ISA_EOL         0x22
 #define OP_ISA_UPPER       0x23
 #define OP_ISA_LOWER       0x24
 #define OP_ISA_ALPHA       0x25
 #define OP_ISA_ALNUM       0x26
 #define OP_ISA_PUNCT       0x27
 #define OP_ISA_CTRL        0x28
 #define OP_ISA_DIGIT       0x29
 #define OP_ISA_HEXDIGIT    0x2A
 #define OP_ISA_SPACE       0x2B
 #define OP_ISA_BLANK       0x2C
 #define OP_ISA_WORDCHAR    0x2D
 #define OP_ISA_INT         0x30
 #define OP_ISA_HEXINT      0x31
 #define OP_ISA_FLOAT       0x32
 #define OP_ISA_BLANKSEQ    0x33
 #define OP_ISA_WORD        0x34
 #define OP_ISA_SIGN        0x35
 #define OP_ISA_BALANCED    0x36
 #define OP_ISA_QSTRING     0x37
 #define OP_ENV_START       0x40
 #define OP_ENV_END         0x50
 #define OP_ISN             0x60
 #define OP_CLO_PLUS        0x81
 #define OP_CLO_OPT         0x82
 #define OP_CLO_STAR        0x83
 #define OP_CUR_SAVE        0x90
 #define OP_CUR_RECALL      0x91
 #define OP_CUR_RESTART     0x92
 #define OP_JMP_FAIL        0xC0
 #define OP_JMP_MATCH       0xC1
 #define OP_JMP_LTMAX       0xC2
 #define OP_CLO             0xD0
 #define OP_CHR             0xF0
 #define OP_CHR_NOT         0xF1
 #define OP_CHR_NOCASE      0xF2
 #define OP_CHR_NOCASE_NOT  0xF3
 #define OP_RNG             0xF8
 #define OP_RNG_NOT         0xF9
 #define OP_RNG_NOCASE      0xFA
 #define OP_RNG_NOCASE_NOT  0xFB

#define MAXCAPT 16

typedef struct {
  uint16_t prg_count;
  uint16_t nxt;
  uint16_t cur;
  uint16_t can_fail;
  struct {
    uint16_t lbl_clo;
    uint16_t lbl_end;
    uint16_t lbl_nxt;
    uint16_t prv;
  } capt[MAXCAPT];
} env_t;

static int oplen(uint8_t op)
{
  return 1 + 2 *(op >= 0xC0) + 2 * (op >= 0xF0);
}

static uint16_t store_op(uint8_t op, uint32_t arg1, uint32_t arg2,env_t *env)
{
  env->prg_count += oplen(op);
  return env->prg_count;
}

static void store_lbl(uint16_t lbl,env_t *env)
{
    printf("%04X  L%03d[%02d]:\n",env->prg_count,lbl,env->cur);
}
static void comp_isa(char *s, char *what, env_t *env)
{
  printf("%04X   ISA %s\n",env->prg_count,what);
  env->can_fail = 1; 
}

static void comp_esc(char *s, env_t *env)
{
  printf("%04X   ESC %08X\n",env->prg_count,s[1]);
  env->can_fail = 1; 
}

static void comp_char(char *s, env_t *env)
{
  printf("%04X   CHR %08X\n",env->prg_count,*s);
  store_op(OP_CHR,*s,0,env);
  env->can_fail = 1; 
}

static void comp_alt(char *s, env_t *env)
{
  printf("%04X L%03d[%02d]:\n",env->prg_count,env->capt[env->cur].lbl_end,env->cur);
  env->capt[env->cur].lbl_end = env->capt[env->cur].lbl_nxt++;
  printf("%04X   JMP MATCH? L%03d[%02d]\n",env->prg_count,env->capt[env->cur].lbl_end,env->cur);
  printf("%04X   NOT\n",env->prg_count);
  printf("%04X   CUR RESTART[%02d]\n",env->prg_count,env->cur);
  env->can_fail = 0; 
}

static void comp_range(char *s, char *e, env_t *env)
{
  int not = 0;
  int lbl;
  int n=0;
  s++;
  while (e>s && *e != ']') e--;
  if (s>=e) return;

  if (*s == '^') { not=1; s++;}
  if (s >= e) return;

  lbl = env->capt[env->cur].lbl_nxt++;
  while (s<e) {
    if (n++ >0)
      printf("%04X   JMP %s L%03d[%02d]\n",env->prg_count,not?"FAIL? ":"MATCH?",lbl,env->cur);
    printf("%04X  CHR %s%08X\n",env->prg_count,not?"NOT ":"",*s);
    if (*++s=='-') {
      s++;
      printf("%04X   RNG %s%08X\n",env->prg_count,not?"NOT ":"",*s);
      s++;
    }
  }
  
  printf("%04X L%03d[%02d]:\n",env->prg_count,lbl,env->cur);
}

static char *endpatt(char *s)
{ int nest;
  switch (*s) {
    case '%' : s+=2; 
               break;

    case '[' : while (*s && *s++ != ']') {}
               break;

    case '(' : nest = 1;
               while (*++s && nest) {
                 if (*s == '(') nest++;
                 else if (*s == ')') nest--;
                 else if (*s == '%') s++;
               }
               break;

    default  : s++;
  }
  return s;
}

static void closure_start(char clo, env_t *env)
{
  if (env->can_fail) {
     printf("%04X   JMP FAIL?  L%03d[%02d]\n",env->prg_count,env->capt[env->cur].lbl_end,env->cur);
     store_op(OP_JMP_FAIL,env->capt[env->cur].lbl_end,0,env);
  }
  env->can_fail = 1;
  switch(clo) {
    case '*': printf("%04X   CLO 0 *\n",env->prg_count); 
              store_op(OP_CLO_STAR,0,0,env);
              break;
    case '+': printf("%04X   CLO 1 *\n",env->prg_count); 
              store_op(OP_CLO_PLUS,0,0,env);
              break;
    case '?': printf("%04X   CLO 0 1\n",env->prg_count);
              store_op(OP_CLO_OPT,0,0,env);
              break;
    case '!': printf("%04X   CUR SAVE[%02d]\n",env->prg_count,env->cur); 
              store_op(OP_CUR_SAVE,0,0,env);
              return;
    default : return ;
  }
  env->capt[env->cur].lbl_clo = env->capt[env->cur].lbl_nxt++;
  store_lbl(env->capt[env->cur].lbl_clo,env);
  env->capt[env->cur].lbl_end = env->capt[env->cur].lbl_nxt++;
  //env->capt[env->cur].lbl_nxt++;
  env->can_fail = 1;
}

static void closure_end(char clo, env_t *env)
{
  int cur;

  if (clo == '\0') return;

  cur=env->cur;

  if (clo == '!') {
    printf("%04X   CUR RECALL[%02d]\n",env->prg_count,cur);
    printf("%04X   NOT\n",env->prg_count);
    printf("%04X   JMP FAIL?  L%03d[%02d]\n",env->prg_count,env->capt[cur].lbl_end,cur);
    store_op(OP_CUR_RECALL,cur,0,env);
    store_op(OP_NOT,0,0,env);
    store_op(OP_JMP_FAIL,cur,env->capt[cur].lbl_end,env);
    env->can_fail = 0;
  }
  else {
    if (env->can_fail) {
      printf("%04X   JMP FAIL?  L%03d[%02d]\n",env->prg_count,env->capt[cur].lbl_clo+1,cur);
      store_op(OP_JMP_FAIL,cur,env->capt[cur].lbl_clo+1,env);
    }
    printf("%04X   JMP ++MAX? L%03d[%02d]\n",env->prg_count,env->capt[cur].lbl_clo,cur);
    store_lbl(env->capt[cur].lbl_clo+1,env);
    printf("%04X   MIN\n",env->prg_count);
  }
}

static void capture_start(char *s,int clo,env_t *env)
{
  env->capt[env->nxt].prv = env->cur;
  env->capt[env->nxt].lbl_nxt = 2;
  env->capt[env->nxt].lbl_end = 1;
  env->cur = env->nxt++;
  printf("%04X   ENV START %02d\n",env->prg_count,env->cur);
  store_op(OP_ENV_START,env->cur,0,env);
  env->can_fail = 0;
}

static void capture_end(char *s,env_t *env)
{
  printf("%04X L%03d[%02d]:\n",env->prg_count,env->capt[env->cur].lbl_end,env->cur);
  //printf("%04X   STO END[%02d]\n",env->prg_count,env->cur);
  printf("%04X   ENV END %02d\n",env->prg_count,env->cur);
  store_op(OP_ENV_END,env->cur,0,env);
  env->cur = env->capt[env->cur].prv;
  env->capt[env->cur].lbl_end = 1;
}

void skpcomp(char *s)
{
  env_t  env_data;
  env_t *env = &env_data;
  char *e;
  char clo;
  
  env->cur    = 0;
  env->nxt    = 1;
  env->capt[0].prv = 0;
  env->capt[0].lbl_nxt = 2;
  env->capt[0].lbl_end = 1;
  env->can_fail    = 1;
  env->prg_count   = 0;

  if (*s == '#') return;
  printf(";; %s\n",s);

  if (*s == '^') { printf("%04X   BOL\n",env->prg_count); s++; }

  printf("%04X   ENV START 00\n",env->prg_count);
  store_op(OP_ENV_START,env->cur,0,env);
  env->can_fail = 0;
  while (*s) {
    e = endpatt(s);
    clo = '\0';
    if (*e == '*' || *e == '?' || *e == '+' || *e == '!') {
      clo = *e++;
    }
    
    switch(*s) {
      case '(': closure_start(clo,env);
                capture_start(s,clo,env);
                e=s+1;
                break;

      case ')': capture_end(s,env);
                closure_end(clo,env);
                break;

      case '|': comp_alt(s,env);
                break;

      case '[': closure_start(clo,env); 
                comp_range(s,e,env);
                closure_end(clo,env);
                break;

      case '%': closure_start(clo,env); 
                comp_esc(s, env); 
                closure_end(clo,env);
                break;

      case '.': closure_start(clo,env); 
                comp_isa(s,"NOTEOL",env);
                closure_end(clo,env);
                break;

      case '$': comp_isa(s,"EOL",env);
                break;

      default : closure_start(clo,env); 
                comp_char(s, env);
                closure_end(clo,env);
                break;
    }
    //
    s=e;
  }
  
  while (env->cur != 0) {
    capture_end(s,env);
    closure_end('\0',env);
  }
  
  printf("%04X L%03d[00]:\n",env->prg_count,env->capt[0].lbl_end);
  //printf("%04X   STO END[00]\n",env->prg_count);
  printf("%04X   ENV END 00\n",env->prg_count);
  store_op(OP_ENV_END,0,0,env);
  printf("%04X   HLT\n",env->prg_count);
}
