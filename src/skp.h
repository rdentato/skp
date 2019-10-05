#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "dbg.h"

#define skpfree(x) free(x)

char *skpcomp(char *s);
void skpprint(char *prg,FILE *f);


uint8_t *skpload(FILE *f);
int      skpsave(FILE *f, uint8_t p);

char  *skpexec(char *prg, char *str);

// ** PRIVATE **
char *skpnext(char *s, uint32_t *cptr, int enc);
uint32_t skpfoldlatin(uint32_t c, int enc);

/* FF YX XX 0F
1111 1111 
yyyy xxxx
xxxx xxx1
0000 1111
*/