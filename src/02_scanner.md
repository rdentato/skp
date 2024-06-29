
```C
@("after:Public API")

typedef struct {
  char *start;
  char *to;
  char *end;
  int   alt;
} skp_loop_t;

#define skp_1(s) for (skp_loop_t skp_loop = {s,NULL,NULL,1}; \
                      skpstart && *skpstart && skpalt && !(skpalt = skp_zero);\
                      s = skpstart = skpto)

#define skpif(p) if (skpalt || !(skpalt = skp_(skpstart,p,&skpto,&skpend))) ; else

#define skpelse  if (skpalt || !(skpend = skpto = skpstart)) ; else
#define skpstart skp_loop.start

#define skpto    skp_loop.to
#define skpend   skp_loop.end
#define skpalt   skp_loop.alt
#define skplen   skp_loop_len(skp_loop.start,skp_loop.to)

static inline int skp_loop_len(char *start, char *to)
{int ret = to-start; return (0 <= ret && ret <= (1<<16)?ret:0);}

```