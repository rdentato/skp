/* 
**  (C) 2019 by Remo Dentato (rdentato@gmail.com)
**
** This software is distributed under the terms of the MIT license:
**  https://opensource.org/licenses/MIT
**
*/

#ifndef SKP_VERSION 
#define SKP_VERSION      0x0002000B
#define SKP_VERSION_STR "skp 0.2.0-beta"

#include <string.h>
#include <ctype.h>
#include <stdlib.h> 
#include <stddef.h>

typedef struct skp_s skp_t;

static inline skp_t *skpnew(int max);
#define skpfree free

static inline  int  skplen(skp_t *capt, int n);
static inline char *skpstart(skp_t *capt, int n);

char *skpmatch(char *s, char *p, skp_t *capt);
#define skp(s,...) skpmatch(s, skpexp(skp0(__VA_ARGS__,NULL)), \
                               skpexp(skp1(__VA_ARGS__,NULL,NULL)))

/******************************************************/
/***** PRIVATE! DON'T EVEN LOOK BEYOND THIS LINE!!! ***/
/******************************************************/

#define skpexp(x)      x
#define skp0(x,...)   (x)
#define skp1(y,x,...) (x)

struct skp_s {
  int num;
  int cur;
  int max;
  struct skp_ptr_s {
    char *start;
    char *end;
  } str[0];
};

static inline skp_t *skpnew(int max)
{ skp_t *capt;
  if (max<2) max = 2;
  if ((capt=malloc(sizeof(skp_t)+max *sizeof(struct skp_ptr_s))))
    capt->max = max;  
  return capt;
}

static inline int skplen(skp_t *capt, int n)
{
  if (capt && n<capt->max && capt->str[n].end != NULL ) 
    return capt->str[n].end - capt->str[n].start;
  else 
    return 0;
}

static inline char *skpstart(skp_t *capt, int n)
{
  if (capt && n<capt->max && capt->str[n].end != NULL ) 
    return capt->str[n].start;
  else 
    return "";
}

static inline char *skpend(skp_t *capt, int n)
{
  if (capt && n<capt->max && capt->str[n].end != NULL ) 
    return capt->str[n].end;
  else 
    return "";
}

#endif
