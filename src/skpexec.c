#include "skp.h"
#include "skpopcodes.h"

                      // 0 1 2 3 4 5 6 7 8 9 A B C D E F 
uint8_t skpoplen_[16] = {1,1,1,1,1,1,1,1,1,2,2,2,2,3,2,3};

 /*
   n = nesting  c=captures


  0  | start | onf  |sto/rcl| start0 |  end0  | start1 |  end1  | ... | startF |  endF  |
  1  | start | onf  |sto/rcl| start0 |  end0  | start1 |  end1  | ... | startF |  endF  |
  ...
  F  | start | onf  |sto/rcl| start0 |  end0  | start1 |  end1  | ... | startF |  endF  |

  
  base(lvl) = lvl*(c+3);
  onf(lvl) = base(lvl)
  sto(lvl) = base(lvl)+1
  start(lvl,n) = (base(lvl)+3)+2*n
  end(lvl,n) =start(lvl,n)+1

 */

typedef struct {
  uint8_t *str_start;
  uint8_t *str_cur;
  uint32_t chr;
  uint16_t len;
  uint16_t nest;
  uint16_t ncapt;
  uint16_t cur_lvl;
  uint16_t cur_capt;
  uint16_t match;
  struct {
    uint16_t sto_start;
    uint16_t sto_temp;
    uint16_t rpt_min;
    uint16_t rpt_max;
    uint16_t rpt_cnt;
    uint16_t start[16];
    uint16_t end[16];
  } stack[16];
} execenv_t;

static int chkhdr(uint8_t *p)
{
  return ( (p)              &&
           ((*p++) == 0xFF) && ((*p++ & 0xF0) == 0xF0) && (*p++) && //  000  FF F0 16  HDR LEN 016
           ((*p++) == 0xEF) && (*p++)                  &&           //  003  EF 11     HDR LVL 11
           ((*p++) == 0x0C)                                         //  005  0C        HDR END
         );
}

static op_0x00(uint8_t *p,char *s,execenv_t *env)
{
  uint8_t op=*p & 0x03;
  env->match = ((env->match ^ op) | (op>>1)) & 1;
}

static op_0x40(uint8_t *p,char *s,execenv_t *env)
{
   uint8_t n = *p & 0x0F;

   if (n==0) {
     env->cur_lvl = 0; 
     for (int k=0; k<16; k++) {
       env->stack[env->cur_lvl].start[k] = 1;
       env->stack[env->cur_lvl].end[k] = 0;
     }
     env->match =1;
   }
   else {
     env->cur_lvl++;
     for (int k=0; k<16; k++) {
       env->stack[env->cur_lvl].start[k] = env->stack[env->cur_lvl].start[k-1];
       env->stack[env->cur_lvl].end[k]   = env->stack[env->cur_lvl].end[k-1];
     }
   }
   env->stack[env->cur_lvl].sto_start = env->str_cur - env->str_start;
   env->stack[env->cur_lvl].start[n]  = env->stack[env->cur_lvl].sto_start;
   env->stack[env->cur_lvl].end[n]    = env->stack[env->cur_lvl].sto_start;
}

static op_0x50(uint8_t *p, char *s, execenv_t *env)
{
  uint8_t n = *p & 0x0F;
  if (env->match) {
    env->stack[env->cur_lvl].end[n] = env->str_cur - env->str_start;
    if (env->cur_lvl > 0) {
      for (int k=0; k<16; k++) {
        env->stack[env->cur_lvl].start[k-1] = env->stack[env->cur_lvl].start[k];
        env->stack[env->cur_lvl].end[k-1]   = env->stack[env->cur_lvl].end[k];
      }
      env->cur_lvl--;
    }
  }
  else {
    env->str_cur = env->str_start + env->stack[env->cur_lvl].sto_start;
    if (env->cur_lvl > 0 ) env->cur_lvl--;
  }
}

static op_0xE0(uint8_t *p,char *s,execenv_t *env)
{
  uint8_t op=*p & 0x03;
}

uint16_t *skpexec(char *prg, char *s)
{
    uint8_t *p = (uint8_t *)prg;
    uint8_t op;
    execenv_t  env_;
    execenv_t *env=&env_;
    int anywhere = 1;
    uint8_t *prg_start;

    if (chkhdr(p)) {
      env->len   = (p[1] & 0x0F) << 8 + p[2];
      env->nest  =  p[4] >> 4;
      env->ncapt =  p[4] & 0x0F;
      env->str_start = s;

      p+=6;
      if (*p == OP_BOL) {anywhere = 0; p++;}
      prg_start = p;
      do {
        p=prg_start;
        while(*p) {
          op = *p;
          switch(op & 0xF0) {
            case 0x00 : op_0x00(p,s,env); break;
            case 0x10 : break;
            case 0x20 : 
            case 0x30 : break;
            case 0x40 : op_0x40(p,s,env); break;
            case 0x50 : op_0x50(p,s,env); break;
            case 0x60 : 
            case 0x70 : break;
            case 0x80 : break;
            case 0x90 : break;
            case 0xA0 : break;
            case 0xB0 : break;
            case 0xC0 : break;
            case 0xD0 : break;
            case 0xE0 : break;
            case 0xF0 : break;
          }
          p+=skpoplen(op);
        }
      } while(0);
    }

    if (env) free(env);
    return NULL;
}
