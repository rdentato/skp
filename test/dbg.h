/* 
**  (C) 2018 by Remo Dentato (rdentato@gmail.com)
**
** This software is distributed under the terms of the MIT license:
**  https://opensource.org/licenses/MIT
**
**  *** DEBUG AND TESTING MACROS ***
**
**   dbgmsg(char *, ...)       --> Prints a message on stderr (works as printf(...)).
**                                 If DEBUG is not defined, do nothing.
** 
**   dbgtst(char *, ...)       --> Starts a test case.
**
**   dbgchk(test, char *, ...) --> Perform the test. If test fails prints a message on
**                                 stderr (works as printf(...)).
**                                 If DEBUG is not defined, do nothing.
** 
**   dbgclk {...}              --> Measure the time needed to execute the block.
**                                 If DEBUG is not defined, execute the block but don't 
**                                 measure time.
** 
**   dbgblk {...}              --> Execute the block only in DEBUG mode.
**                                 Note how the code is enclosed by '( ... )' not '{ ... }'
**
**   dbgtrk( ... ) {...}       --> Specify the strings to me tracked within the scope of the
**                                 block. If DEBUG is not defined, execute the block but don't 
**                                 track strings.
**                                 (See 'dbgstat' in the tools directory for documentation)
**
**  _dbgmsg(char *, ...)       --> Do nothing. Used to disable the debug message.
**  _dbgtst(char *, ...)       --> Do nothing. Used to disable the debug message.
**  _dbgchk(test, char *, ...) --> Do nothing. Used to disable the debug message.
**  _dbgclk {...}              --> Execute the block but don't measure time.
**  _dbgblk {...}              --> Do not execute the code block.
**  _dbgtrk( ... ) {...}       --> Execute the block but don't mark string tracking.
**
**  Note that if NDEBUG is defined, DEBUG will be undefined
*/

#ifndef DBG_VERSION
#define DBG_VERSION     0x0101000C
#define DBG_VERSION_STR "1.1.0-rc"

#ifdef NDEBUG
#ifdef DEBUG
#undef DEBUG
#endif
#endif

#ifdef DEBUG
#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <string.h>

#define dbgexp(x)      x
#define dbg0(x,...)   (x)

#define dbgprt(...)   ((fflush(stdout), fprintf(stderr,__VA_ARGS__),     \
                        fflush(stderr)), dbg=0)

#define dbgmsg(...)   ((fflush(stdout), fprintf(stderr,__VA_ARGS__),     \
                        fprintf(stderr," \x9%s:%d\n",__FILE__,__LINE__), \
                        fflush(stderr)), dbg=0)

#define dbgtst(...)    dbgmsg("TEST: " __VA_ARGS__)

#define dbgchk(e,...)  do {int dbg_=!(e);                                                                  \
                          fflush(stdout); /* Ensure dbg_ message appears *after* pending stdout prints */ \
                          fprintf(stderr,"%s: (%s) \x9%s:%d\n",(dbg_?"FAIL":"PASS"),#e,__FILE__,__LINE__); \
                          if (dbg_ && *(dbgexp(dbg0(__VA_ARGS__))))                                        \
                            { fprintf(stderr,"    : " __VA_ARGS__); fputc('\n',stderr); }                \
                          fflush(stderr); errno = dbg_; dbg=0;                                           \
                       } while(0) 

typedef struct {
  clock_t    clk;       time_t     time;    
  char       tstr[32];  struct tm *time_tm; 
  long int   elapsed; 
} dbgclk_t;

#define dbgclk          for (dbgclk_t dbg_ = {.elapsed = -1};                         \
                             \
                             time(&dbg_.time), dbg_.time_tm=localtime(&dbg_.time),    \
                             strftime(dbg_.tstr,32,"%Y-%m-%d %H:%M:%S",dbg_.time_tm), \
                             dbg_.clk = clock(), dbg_.elapsed < 0 ;                   \
                            \
                            dbg_.elapsed=(long int)(clock()-dbg_.clk),                \
                            dbgmsg("TIME: %s +%ld/%ld sec.",dbg_.tstr,                \
                                       dbg_.elapsed, (long int)CLOCKS_PER_SEC)        \
                            )

#define dbgblk         for (int dbg_=1; dbg_; dbg_=dbg) 

#define dbgtrk(...)    for (int dbg_=!dbgmsg("TRK[: %s",#__VA_ARGS__); \
                                dbg_;                                  \
                                dbg_=dbgmsg("TRK]:"))

static volatile int dbg = 0; // This will always be 0. Used to suppress warnings

#else // DEBUG
#define dbgmsg(...)
#define dbgtst(...)
#define dbgchk(e,...)
#define dbgclk
#define dbgtrk(...)
#define dbgblk         while(0)
#endif // DEBUG

#define _dbgmsg(...) 
#define _dbgtst(...)
#define _dbgchk(e,...)
#define _dbgclk
#define _dbgtrk(...)
#define _dbgblk       while(0)

#endif // DBG_H_VER

