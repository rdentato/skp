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

typedef struct skpcapt_s skpcapt_t;

static inline skpcapt_t *skpcaptnew(int max);
#define skpcaptfree free

static inline  int  skpcaptlen(skpcapt_t *capt, int n);
static inline char *skpcaptstart(skpcapt_t *capt, int n);

char *skppattern(char *s, char *p, skpcapt_t *capt);
#define skp(s,...) skppattern(s, skpexp(skp0(__VA_ARGS__,NULL)), \
                                 skpexp(skp1(__VA_ARGS__,NULL,NULL)))

/******************************************************/
/***** PRIVATE! DON'T EVEN LOOK BEYOND THIS LINE!!! ***/
/******************************************************/

#define skpexp(x)      x
#define skp0(x,...)   (x)
#define skp1(y,x,...) (x)

typedef struct skpcapt_s {
  int num;
  int cur;
  int max;
  struct skpcapt_ptr_s {
    char *start;
    char *end;
  } str[0];
} skpcapt_t;

static inline skpcapt_t *skpcaptnew(int max)
{ skpcapt_t *capt;
  if (max<2) max = 2;
  if ((capt=malloc(sizeof(skpcapt_t)+max *sizeof(struct skpcapt_ptr_s))))
    capt->max = max;  
  return capt;
}

static inline int skpcaptlen(skpcapt_t *capt, int n)
{
  if (capt && n<capt->max && capt->str[n].end != NULL ) 
    return capt->str[n].end - capt->str[n].start;
  else 
    return 0;
}

static inline char *skpcaptstart(skpcapt_t *capt, int n)
{
  if (capt && n<capt->max && capt->str[n].end != NULL ) 
    return capt->str[n].start;
  else 
    return "";
}

#endif
