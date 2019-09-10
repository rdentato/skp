/* 
**  (C) 2018 by Remo Dentato (rdentato@gmail.com)
**
** This software is distributed under the terms of the MIT license:
**  https://opensource.org/licenses/MIT
**
**  *** DEBUG AND TESTING MACROS ***
**
**   DEBUG                     --> If undefined, removes all debug instructions.
**                                 If defined sets the level of debugging and enables
**                                 the dbg functions:
**
**                                          level         enabled functions
**                                      ------------  --------------------------
**                                      DBGLVL_ERROR  dbgerr() dbgmsg() dbgprt()
**                                      DBGLVL_WARN   as above plus dbgwrn()
**                                      DBGLVL_INFO   as above plus dbginf()
**                                      DBGLVL_TEST   all dbg functions.
**
**                                 Note NDEBUG has higher priority than DEBUG, if
**                                 NDEBUG is defined, DEBUG will be undefined.
**
**   dbgmsg(char *, ...)       --> Prints a message on stderr (works as printf(...)).
**                                 If DEBUG is not defined, do nothing.
** 
**   dbgprt(char *, ...)       --> Prints a message on stderr (works as printf(...)) omitting
**                                 filename and line. If DEBUG is not defined, do nothing.
** 
**   dbginf(char *, ...)       --> Prints an "INFO:" message depending on the DEBUG level.
**   dbgwrn(char *, ...)       --> Prints a  "WARN:" message depending on the DEBUG level.
**   dbgerr(char *, ...)       --> Prints an "FAIL:" message.
**   dbgtst(char *, ...)       --> Starts a test case.
**                                 If DEBUG is undefined or lower than DBGLVL_TEST, do nothing.
**
**   dbgchk(test, char *, ...) --> Perform the test and set errno (0: ok, 1: ko). If test fails
**                                 prints a message on stderr (works as printf(...)).
**                                 If DEBUG is undefined or lower than DBGLVL_TEST, do nothing.
** 
**   dbgreq(test, char *, ...) --> As dbgchk() but if test fails exit the program with abort().
** 
**   dbgclk {...}              --> Measure the time needed to execute the block. If DEBUG is
**                                 undefined or lower than DBGLVL_TEST, execute the block but
**                                 don't measure the elapsed time.
** 
**   dbgblk {...}              --> Execute the block only if DEBUG is defined as DBGLVEL_TEST.
**
**   dbgtrk( ... ) {...}       --> Specify the strings to be tracked within the scope of the
**                                 block. If DEBUG is not defined or lower than DBGLVL_TEST,
**                                 execute the block but don't mark track strings.
**                                 (See 'dbgstat' in the tools directory for documentation)
**
**  _dbgmsg(char *, ...)       --> Do nothing. Used to disable the debug message.
**  _dbgprt(char *, ...)       --> Do nothing. Used to disable the debug message.
**  _dbginf(char *, ...)       --> Do nothing. Used to disable the debug message.
**  _dbgwrn(char *, ...)       --> Do nothing. Used to disable the debug message.
**  _dbgerr(char *, ...)       --> Do nothing. Used to disable the debug message.
**  _dbgchk(test, char *, ...) --> Do nothing. Used to disable the debug message.
**  _dbgreq(test, char *, ...) --> Do nothing. Used to disable the debug message.
**  _dbgclk {...}              --> Execute the block but don't measure time.
**  _dbgblk {...}              --> Do not execute the code block.
**  _dbgtrk( ... ) {...}       --> Execute the block but don't mark string tracking.
**
*/

#ifndef DBG_VERSION
#define DBG_VERSION     0x0102001C
#define DBG_VERSION_STR "dbg 1.2.1-rc"

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
#include <stdarg.h>

#define DBGLVL_ERROR 0
#define DBGLVL_WARN  1
#define DBGLVL_INFO  2
#define DBGLVL_TEST  3

#define dbgprt(...)   ((fflush(stdout), fprintf(stderr,__VA_ARGS__),     \
                        fflush(stderr)), dbg=0)

#define dbgmsg(...)   ((fflush(stdout), fprintf(stderr,__VA_ARGS__),     \
                        fprintf(stderr," \x9%s:%d\n",__FILE__,__LINE__), \
                        fflush(stderr)), dbg=0)

#define dbgerr(...)                                    dbgmsg("FAIL: " __VA_ARGS__)
#define dbgtst(...)    if (DEBUG < DBGLVL_TEST) ; else dbgmsg("TEST: " __VA_ARGS__)
#define dbgwrn(...)    if (DEBUG < DBGLVL_WARN) ; else dbgmsg("WARN: " __VA_ARGS__)
#define dbginf(...)    if (DEBUG < DBGLVL_INFO) ; else dbgmsg("INFO: " __VA_ARGS__)

#define dbgchk(e,...)  if (DEBUG < DBGLVL_TEST) ; else do { int dbg_err=!(e);                                          \
                          fflush(stdout); /* Ensure dbg_err message appears *after* pending stdout prints */ \
                          fprintf(stderr,"%s: (%s) \x9%s:%d\n",(dbg_err?"FAIL":"PASS"),#e,__FILE__,__LINE__);\
                          if (dbg_err && *(dbg_exp(dbg_0(__VA_ARGS__))))                                       \
                            { fprintf(stderr,"    : " __VA_ARGS__); fputc('\n',stderr); }                    \
                          fflush(stderr); errno = dbg_err; dbg=0;                                            \
                       } while(0)

#define dbgreq(e,...)  do { dbgchk(e,__VA_ARGS__); if (errno) abort();} while(0)

typedef struct {
  clock_t    clk;       time_t     time;    
  char       tstr[32];  struct tm *time_tm; 
  long int   elapsed; 
} dbgclk_t;

#if DEBUG < DBGLVL_TEST
#define dbgclk 
#define dbgtrk() 
#else
#define dbgclk          for (dbgclk_t dbg_ = {.elapsed = -1};   \
                             \
                             time(&dbg_.time), dbg_.time_tm=localtime(&dbg_.time),    \
                             strftime(dbg_.tstr,32,"%Y-%m-%d %H:%M:%S",dbg_.time_tm), \
                             dbg_.clk = clock(), dbg_.elapsed < 0 ;                   \
                            \
                            dbg_.elapsed=(long int)(clock()-dbg_.clk),                \
                            dbgmsg("TIME: %s +%ld/%ld sec.",dbg_.tstr,                \
                                       dbg_.elapsed, (long int)CLOCKS_PER_SEC)        \
                            )

static inline int dbg_trk_str(char *file, int line, ...) {
  char *t;
  va_list args;
  fflush(stdout); fprintf(stderr,"TRK[: ");
  va_start(args, line);
  while((t = va_arg(args, char *)))
    fprintf(stderr,"\"%s\" ",t);
  fprintf(stderr,"\x9%s:%d\n",file,line);
  va_end(args);  
  return 1;
}

#define dbgtrk(...)   for (int dbg_trk=dbg_trk_str(__FILE__,__LINE__,__VA_ARGS__,NULL); \
                               dbg_trk;                                  \
                               dbg_trk=dbgmsg("TRK]: "))

#endif

#define dbgblk         if (DEBUG < DBGLVL_TEST) ; else

static volatile int dbg=0;  // This will always be 0. Used to suppress warnings

#define dbg_exp(x)      x
#define dbg_0(x,...)   (x)

#else // DEBUG
#define dbgmsg(...)
#define dbgprt(...)
#define dbgtst(...)
#define dbginf(...)
#define dbgwrn(...)
#define dbgerr(...)
#define dbgchk(e,...)
#define dbgreq(e,...)
#define dbgclk
#define dbgtrk(...)
#define dbgblk         while(0)
#endif // DEBUG

#define _dbgmsg(...) 
#define _dbgprt(...)
#define _dbgtst(...)
#define _dbginf(...)
#define _dbgwrn(...)
#define _dbgerr(...)
#define _dbgchk(e,...)
#define _dbgreq(e,...)
#define _dbgclk
#define _dbgtrk(...)
#define _dbgblk       while(0)

#endif // DBG_H_VER

