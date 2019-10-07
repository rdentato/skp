#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "dbg.h"

#define skpfree(x) free(x)

char *skpcomp(char *s);
void skpdisasm(char *prg, FILE *f);

uint16_t *skpexec(char *prg, char *str);

uint8_t *skpload(FILE *f);
int      skpsave(FILE *f, uint8_t p);


// ** PRIVATE **
char *skpnext(char *s, uint32_t *cptr, int enc);
uint32_t skpfoldlatin(uint32_t c, int enc);

