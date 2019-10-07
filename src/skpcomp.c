#include "skp.h"
#include "skpopcodes.h"

#define ESC '\\'

#define MAXCAPT 16

#define NEG_LOOKUP '\2'
#define POS_LOOKUP '\1'

typedef struct {
  uint8_t  *prg;
  uint16_t  prg_cnt;
  uint16_t  prg_max;
  uint16_t *lbl;
  uint16_t  lbl_cnt;
  uint16_t  lbl_max;
  uint32_t  ch;
  uint16_t  nxt;
  uint16_t  cur;
  uint16_t  prt_lbl;
  uint8_t   can_fail;
  uint8_t   in_range;
  uint8_t   csens;
  uint8_t   lvl_cnt;
  uint8_t   lvl_max;
  uint8_t   enc;
  struct {
    uint16_t prv;
    uint16_t clo;
    uint16_t lbl_clo;
    uint16_t lbl_end;
  } capt[MAXCAPT];
} env_t;


static uint32_t encode_chr(uint32_t cp,int enc)
{
  uint32_t e=0;

  if (enc != 0) return (cp & 0xFF); // ISO 8859

  if (cp < 0x00110000) {
    if (cp >= 0x00010000)  {
      e  =            (0xF0 | ((cp >> 18) & 0x07));
      e  = (e << 8) | (0x80 | ((cp >> 12) & 0x3F));
      e  = (e << 8) | (0x80 | ((cp >>  6) & 0x3F));
      e  = (e << 8) | (0x80 | ((cp      ) & 0x3F));
    }
    else if (cp >= 0x0800) {
      e  =            (0xE0 | ((cp >> 12) & 0x0F));
      e  = (e << 8) | (0x80 | ((cp >>  6) & 0x3F));
      e  = (e << 8) | (0x80 | ((cp      ) & 0x3F));
    }
    else if (cp >= 0x80) {
      e =            (0xC0 | ((cp >> 6) & 0x1F));
      e = (e << 8) | (0x80 | ((cp     ) & 0x3F));
    }
    else e = cp;
  }
  return e;
}

static void store_prg(int n, uint8_t *instr, env_t *env)
{
   uint8_t *prg;
   uint16_t max;

   if (env->prg_cnt+n >= env->prg_max) {
     max = env->prg_max * 2;
     if (max == 0) max = 512;
     if ((prg =realloc(env->prg,max))) {
       env->prg = prg;
       env->prg_max = max;
     }
   }
   if (env->prg_cnt+n < env->prg_max) {
     for (int k=0; k<n; k++)
       env->prg[env->prg_cnt++] = instr[k];
   }
}

static void store_op(uint8_t op, uint32_t arg1, uint32_t arg2, env_t *env)
{
  uint8_t instr[8];

  instr[0] = op;
  instr[1] = instr[2] = instr[3] = instr[4] = 0x80;
  uint32_t t;
  
  if (op >= OP_CHR_BASE) { // Adjust CHR and RNG
    _dbginf("OP: %02X",op);
    if (!env->csens) arg1 = skpfoldlatin(arg1,env->enc);
    if (arg1 >= 0xFFFFFF) {
      instr[0] = OP_CHH4;
      instr[1] = (arg1 >> 24) & 0xFF;
      instr[2] = (arg1 >> 16) & 0xFF;
      store_prg(3,instr,env);
      arg1 &= 0xFFFF;
    }
    else if (arg1 >= 0xFFFF) {
      instr[0] = OP_CHH3;
      instr[1] = (arg1 >> 16) & 0xFF;
      store_prg(2,instr,env);
      arg1 &= 0xFFFF;
    }
    t = arg1;
    op |= env->csens;
    if (t > 0xFF) {
      op |= 0x10;
      instr[2]  =  t & 0xFF ; t>>=8;  // STORED IN BIG ENDIAN
      instr[1]  = (t & 0xFF); t>>=8;
    }
    else instr[1] = t;
    instr[0]  = op;
    env->can_fail = 1;
  }
  else if (op >= OP_JPM) {
    instr[0] |= (arg1 >> 8) & 0xFF;
    instr[1]  =  arg1 & 0xFF;
  }
  else if (op == OP_BGN || op == OP_END) { 
    instr[0] |= arg1 & 0x0F;
    env->can_fail = 1;
  }
  
  store_prg(skpoplen(op),instr,env);
}

static void store_lbl(uint16_t lbl, env_t *env)
{
  uint16_t  max;
  uint16_t *ptr;
  if ((env->prg_cnt & 0xFF) == 0) { // Avoid storing 00
    store_op(OP_NOP, 0, 0, env);
  }
  if (env->lbl_cnt >= env->lbl_max) {
    max = env->lbl_max * 2;
    if (max == 0) max = 256;
    if ((ptr = realloc(env->lbl,max))) {
      env->lbl = ptr;
      env->lbl_max = max;
    }
  }
  if (env->lbl_cnt < env->lbl_max) {
    env->lbl[lbl] = env->prg_cnt;
  }

  env->prt_lbl = lbl;
}

static uint16_t lbl_nxt(env_t *env)
{
  uint16_t lbl = env->lbl_cnt++;
  if ((lbl & 0xFF) == 0) lbl = env->lbl_cnt++;
  return lbl;
}

static void comp_esc(char *s, env_t *env, uint8_t not,uint8_t range)
{
  if (not) not = OP_ISN;
  
  env->can_fail = 1; 
  switch (env->ch) {
    case 'D': not ^= OP_ISN;
    case 'd': store_op(not | OP_ISA_DIGIT,0,0,env);
              break;

    case 'S': not ^= OP_ISN;
    case 's': store_op(not | OP_ISA_SPACE,0,0,env);
              break;

    default : store_op((OP_CHR_BASE | OP_RNG_BASE * range) | !!(not),env->ch,0,env);
  }
}

//                                               1 1 1 1 1 1  1 
//                  ISO 8859-X 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5  6 
static uint8_t isolatin[16] = {1,2,3,4,1,1,1,1,5,6,1,1,7,8,9,10};
static void setoption(char *s, char *e, env_t *env)
{
  uint16_t x;
  uint16_t n=env->enc;
  uint16_t c=env->csens;

  while (s<e && (*--e < '0' || '9' < *e)) {}
  while (s<e && ('0' <= e[-1] && e[-1] <= '9')) {e--;}
  x = atoi(e);
  switch (*s) {
    case 'u' :
    case 'U' : if (x == 8 || x == 0) n = 0;
               break;

    case 'l' :
    case 'L' : if (x < 1 || 10 < x) n = 1;
               else n=x;
               break;

    case 'i' :
    case 'I' : if (x < 1 || 16 < x) n = 1;
               else n = isolatin[n-1];
               break;

    case 'c' :
    case 'C' : if (*e != '0') c = 4;
               else c = !!x << 2;
  }
  _dbginf("ENC: x=%d n=%d env->env=%d e=\"%s\"",x,n,env->enc,e);
  if (n != env->enc) {
    env->enc = n;
    store_op(OP_ENC+n,0,0,env);
  }
  if (c != env->csens) {
    env->csens = c;
  }
  _dbginf("CASE: %d (set)",env->csens);
}

static char *comp_ent(char *s, char *e, env_t *env, uint8_t not, uint8_t range)
{ uint32_t ch;
  if (not) not = OP_ISN;
  if (e == s) {
    while (*e && *e != ';') {e++;}
    if (*e == '\0') e = s;
    else e++;
  }
 _dbginf("ENT s: '%s'",s);
 _dbginf("ENT e: '%s'",e);
  if (e >= s+1) { 
    env->can_fail = 1; 
    if (*s == '^') { not ^= OP_ISN; s++;}
    switch (*s) {
      case '=' : setoption(s+1,e,env);  break;
      
      case 'u' : store_op(not | OP_ISA_UPPER, 0, 0,env); break;
      case 'l' : store_op(not | OP_ISA_LOWER, 0, 0,env); break;
      case 'a' : store_op(not | OP_ISA_ALPHA, 0, 0,env); break;
      case 'Q' : store_op(not | OP_ISA_QSTRING, 0, 0,env); break;
      case 'B' : store_op(not | OP_ISA_BALANCED, 0, 0,env); break;
      case '#' : if (*++s == 'x') ch = strtol(s+1,NULL,16);
                 else ch = strtol(s,NULL,10); 
                 ch = encode_chr(ch,env->enc);
                 store_op((OP_CHR_BASE | OP_RNG_BASE * range) | !!(not),ch,0,env);
                 break;
      
      default  : store_op(OP_NOP, 0, 0,env); break;
    }
    return e;
  }
  return s+1;
}

static void comp_char(uint32_t ch, env_t *env)
{
  store_op(OP_CHR_BASE,ch,0,env);
}

static void comp_alt(char *s, env_t *env)
{
  store_lbl(env->capt[env->cur].lbl_end, env);
  env->capt[env->cur].lbl_end = lbl_nxt(env);
  store_op(OP_JPM,env->capt[env->cur].lbl_end,0,env);
  store_op(OP_RST, env->cur, 0, env);
  env->can_fail = 0; 
}

static void comp_range(char *s, char *e, env_t *env)
{
  int not = 0;
  int lbl;
  int n=0;
  uint32_t ch;

  s++;
  while (e>s && *e != ']') e--;
  if (s>=e) return;

  if (*s == '^') { not=1; s++;}
  if (s >= e) return;

  lbl = lbl_nxt(env);
  while (s<e) {
    if (n++ > 0) {
      store_op(OP_JPM | not << 4,lbl,0,env);
    }
    s=skpnext(s,&ch,env->enc);
    if (ch == ESC) {
      s=skpnext(s,&env->ch,env->enc);
      comp_esc(s,env,not,0);
    }
    else if (ch == '&') {
      s=comp_ent(s,s,env,not,0);
    }
    else {
      store_op(OP_CHR_BASE | not,ch,0,env);
    }
    if (*s == '-') {
      switch(s[1]) {
         case ESC: s=skpnext(s+2,&env->ch,env->enc);
                   comp_esc(s,env,not,1);
                   break;

         case '&': s=comp_ent(s+2,s+2,env,not,1);   break;
              
         default : s=skpnext(s+1,&ch,env->enc);
                   store_op(OP_RNG_BASE | not,ch,0,env);
      }
    }
  }
  store_lbl(lbl, env);
}

static char *end_pattern(char *s,env_t *env)
{ int n;
  char *t=s;
  switch (*s) {

    case '[' : while (*s) {
                 if (*s == ']') { s++; break; }
                 if (*s == ESC) s++;
                 if (*s) s++;
               }
               break;

    case '(' : n = 1;
               while (*++s && n != 0) {
                 if (*s == '(') n++;
                 else if (*s == ')') n--;
                 else if (*s == ESC) s++;
               }
               break;

    case '&' : n=0;
              _dbginf("before &:'%s'",s);
               for (n=0; n<16 && *s; n++, s++){
                 if (*s == ';') { s++; break; }
               }
               if (n==16) s = t+1;
              _dbginf("after  &:'%s'",s);
               break;

    case ESC : s++; // FALL THROUGH
    default  : s=skpnext(s, &env->ch, env->enc);
               break;

  }
  env->capt[env->cur].clo = '\0';
 
  if (*s == '*' || *s == '?' || *s == '+') {
    env->capt[env->cur].clo = *s++;
  }
  return s;
}

static void closure_start(env_t *env)
{
  if (env->can_fail) 
     store_op(OP_JPF, env->capt[env->cur].lbl_end, 0, env);
  
  env->capt[env->cur].lbl_clo = lbl_nxt(env);

  switch(env->capt[env->cur].clo) {
    case '*': store_op(OP_FOR_STAR, 0, 0, env);
    case '?': break;

    case '+': store_op(OP_FOR_PLUS, 0, 0, env);
              break;

    case NEG_LOOKUP:
    case POS_LOOKUP:
              store_op(OP_STO, 0, 0, env);
              return;

    default : return ;
  }
  store_lbl(env->capt[env->cur].lbl_clo, env);
  env->can_fail = 1;
}

static void closure_end(env_t *env)
{
  switch (env->capt[env->cur].clo) {
    case '\0':       return;

    case NEG_LOOKUP: store_op(OP_RCL, env->cur, 0, env);
                     store_op(OP_NOT, 0, 0, env);
                     store_op(OP_JPF, env->capt[env->cur].lbl_end, 0, env);
                     env->can_fail = 0;
                     break;

    case POS_LOOKUP: store_op(OP_RCL, env->cur, 0, env);
                     store_op(OP_JPF, env->capt[env->cur].lbl_end, 0, env);
                     env->can_fail = 0;
                     break;

    case '*':        store_op(OP_NXT, env->capt[env->cur].lbl_clo, 0, env);
                     env->can_fail = 0;
                     break;

    case '?':        store_op(OP_MTC, 0, 0, env);
                     env->can_fail = 0;
                     break;

    case '+':        store_op(OP_NXT, env->capt[env->cur].lbl_clo, 0, env);
                     env->can_fail = 1;
                     break;
  }
  env->capt[env->cur].clo = '\0';
}

static void capture_start(char *s,env_t *env)
{
  env->capt[env->nxt].prv = env->cur;
  env->capt[env->nxt].lbl_end = lbl_nxt(env);
  env->cur = env->nxt++;
  env->lvl_cnt++;
  if (env->lvl_cnt > env->lvl_max) env->lvl_max++;
  store_op(OP_BGN, env->cur, 0, env);
  env->can_fail = 0;
}

static void capture_end(char *s,env_t *env)
{
  store_lbl(env->capt[env->cur].lbl_end, env);
  store_op(OP_END, env->cur, 0, env);
  env->cur = env->capt[env->cur].prv;
  env->lvl_cnt--;
}

char *skpcomp(char *s)
{
  env_t  env_data;
  env_t *env = &env_data;
  char *e;
  
  env->cur             = 0;
  env->nxt             = 1;
  env->can_fail        = 1;
  env->in_range        = 0;
  env->prg             = NULL;
  env->prg_cnt         = 0;
  env->prg_max         = 0;
  env->capt[0].prv     = 0;
  env->lbl             = NULL;
  env->lbl_cnt         = 0;
  env->lbl_max         = 0;
  env->prt_lbl         = 0;
  env->enc             = 0;   // UTF-8
  env->csens           = 4;   // Case Sensitive!
  env->ch              = 0;
  env->lvl_cnt         = 1;
  env->lvl_max         = 1;

  store_op(OP_HDR_START, 0xF000, 0, env);
  store_op(OP_HDR_NCAPT, 0x11, 0, env);
  store_op(OP_HDR_END, 0, 0, env);

  if (*s == '^') { 
    store_op(OP_BOL, env->cur, 0, env);
    s++;
  }

  env->capt[0].lbl_end = lbl_nxt(env);
  store_op(OP_BGN, 0, 0, env);

  env->can_fail = 0;

  while (*s) {
    e = end_pattern(s, env);
    switch(*s) {
      case '(':      if (s[1] == '^') { env->capt[env->cur].clo = NEG_LOOKUP ; s++; }
                else if (s[1] == '+') { env->capt[env->cur].clo = POS_LOOKUP ; s++; }
                closure_start(env);
                capture_start(s, env);
                e=s+1;
                break;

      case ')': capture_end(s, env);
                closure_end(env);
                break;

      case '|': comp_alt(s, env);
                break;

      case '[': closure_start(env); 
                comp_range(s, e, env);
                closure_end(env);
                break;

      case ESC: closure_start(env); 
                comp_esc(s, env, 0, 0); 
                closure_end(env);
                break;

      case '&': if (s[1] != '=') closure_start(env); 
                comp_ent(s+1, e, env, 0, 0);
                if (s[1] != '=') closure_end(env);
                break;

      case '.': closure_start(env); 
                store_op(OP_ISA_NOTEOL, 0, 0, env);
                env->can_fail = 1; 
                closure_end(env);
                break;

      case '$': closure_start(env); 
                store_op(OP_ISA_EOL, 0, 0, env);
                env->can_fail = 1; 
                closure_end(env);
                break;

      default : closure_start(env); 
                comp_char(env->ch, env);
                closure_end(env);
                break;
    }
    s=e;
  }
  
  while (env->cur != 0) { // Add missing ')' at the end
    capture_end(s,env);
    env->capt[env->cur].clo = '\0';
    closure_end(env);
  }
  
  store_lbl(env->capt[env->cur].lbl_end, env);
  store_op(OP_END, 0, 0, env);
  
  if ((env->prg_cnt & 0xFF) == 0)   // avoid a 0 for len before the end
    store_op(OP_NOP, 0, 0, env);

  store_op(OP_HLT, 0, 0, env);

  // Fix jmp
  if (env->lbl) {
    uint8_t op;
    uint16_t n;

    for (uint8_t *p=env->prg; *p; p += skpoplen(op)) {
      op = *p & 0xF0;
      if (0x90 <= op && op <= 0xC0) {
        n= ((p[0] & 0x0F)<<8) | p[1];
        p[0] = op | ((env->lbl[n] & 0x0F00) >> 8);
        p[1] = env->lbl[n] & 0xFF;
      }
    }

    if (env->lbl) free(env->lbl);
  }

  // Fix Header
  if (env->prg) {
    env->prg[1] |= ((env->prg_cnt >> 8) & 0x0F) | 0xF0; 
    env->prg[2] |= (env->prg_cnt & 0xFF);               // 
    env->prg[4]  = (env->lvl_max << 4) | env->nxt;      //
  }
  
  return (char *)env->prg;
}

#if 0
// ***************************************************************************
// SKP -> C    ***************************************************************
// ***************************************************************************
void skptoc(char *prg, FILE *f)
{
  uint8_t *p = (uint8_t *)prg;
  uint8_t *x;

  if (!prg) return; 
  if (f == NULL) f = stdout;
  
  while (*p) {
    x=p;
    p = print_cline(p, p-(uint8_t *)prg);
    if (x==p) break;
  }
  print_line(p, p-(uint8_t *)prg);
}
#endif
