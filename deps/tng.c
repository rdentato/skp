/**************************************
**   GENERATED FILE. DO NOT MODIFY   **
** Compile with: cc -O2 -o tng tng.c **
** For more information on tng visit **
**  https://github.com/rdentato/tng  **
**************************************/

#define VRGCLI
#define NDEBUG
#define exception_info char *msg; int aux;

#line 1 "dbg.h"
/* 
**  (C) by Remo Dentato (rdentato@gmail.com)
**
** This software is distributed under the terms of the MIT license:
**  https://opensource.org/licenses/MIT
*/

//  # DEBUG AND TESTING MACROS
//
// ## Index
//     - ## Usage
//     - ## Disabling functions
//     - ## Debugging Levels
//     - ## No DEBUG
//     - ## Standard Includes
//     - ## Testing
//     - ## Tracking
//     - ## Profiling
//     - ## Grouping
//     - ## Memory usage

#ifndef DBG_VERSION
#define DBG_VERSION     0x0103000C
#define DBG_VERSION_STR "dbg 1.3.0-rc"

//  ## Usage
//  To enable the debugging functions, DEBUG must be defined before 
//  including dbg.h. See section "## Debugging levels" for more details.
//
//  Note that NDEBUG has higher priority than DEBUG: if NDEBUG
//  is defined, then DEBUG will be undefined.

#ifdef NDEBUG
#ifdef DEBUG
#undef DEBUG
#endif
#endif

// ## Disabling functions
// For each function provided by `dbg.h`, there is a *disabled*
// counterpart whose name begins with one underscore.
// This avoids having to comment out or delete debugging functions that
// are not needed at the moment but can be useful at a later time.
// Note that from a C standard point of view, this is a compliant usage
// as all these symbols have file scope. (7.1.3)
// The meaning and usage of `DBG_OFF()` is detailed in the section
// titled "Grouping".

#define DBG_OFF(...)

#define _dbgmsg DBG_OFF
#define _dbgprt DBG_OFF
#define _dbgtst(...) while(0)
#define _dbginf DBG_OFF
#define _dbgtrc DBG_OFF
#define _dbgwrn DBG_OFF
#define _dbgerr DBG_OFF
#define _dbgchk DBG_OFF
#define _dbgmst DBG_OFF
#define _dbgtrk DBG_OFF
#define _dbgptr DBG_OFF
#define _dbgclk DBG_OFF
#define _dbgblk while(0)

// ## No DEBUG
// If DEBUG is not defined, the dbgxxx macros should atill be
// defined to ensure the code compiles, but they should do nothing.

#define DBG_ON  DBG_OFF
#define dbgmsg _dbgmsg
#define dbgprt _dbgprt
#define dbgtst _dbgtst
#define dbginf _dbginf
#define dbgtrc _dbgtrc
#define dbgtrk _dbgtrk
#define dbgwrn _dbgwrn
#define dbgerr _dbgerr
#define dbgchk _dbgchk
#define dbgmst _dbgmst
#define dbgtrk _dbgtrk
#define dbgclk _dbgclk
#define dbgblk _dbgblk
#define dbgptr _dbgptr

#ifdef DEBUG

// ## Standard Includes

#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <string.h>

// This variable will always be 0. It's used to suppress warnings
// and ensures that some debugging code is not optimized.
static volatile int dbg_zero = 0;  

// ## Debugging Level
//
//   DEBUG       If undefined, removes all debug instructions.
//               If defined sets the level of debugging and enables
//               the dbg functions:
//
//                     level         enabled functions
//                 ------------  --------------------------
//                 DEBUG_ERROR   dbgerr() dbgmsg() dbgprt()
//                 DEBUG_WARN    as above plus dbgwrn()
//                 DEBUG_INFO    as above plus dbginf()
//                 DEBUG_TEST    all dbg functions except dbgptr()
//                 DEBUG_MEM     all dbg functions plus redefinition
//                               of malloc(), free(), realloc(), calloc(),
//                               strdup() and strndup() (if available)

#define DEBUG_ERROR 0
#define DEBUG_WARN  1
#define DEBUG_INFO  2
#define DEBUG_TEST  3
#define DEBUG_MEM   4

// ## Printing messages
//
//   dbgprt(char *, ...)      Prints a message on stderr (works as printf(...)).
//   dbgmsg(char *, ...)      Prints a message on stderr (works as printf(...)).
//                            Adds filename and line of the instruction.
//   dbginf(char *, ...)      Prints an "INFO:" message depending on the DEBUG level.
//   dbgwrn(char *, ...)      Prints a  "WARN:" message depending on the DEBUG level.
//   dbgerr(char *, ...)      Prints a  "FAIL:" message.

#undef  dbgprt
#define dbgprt(...)  (fprintf(stderr,"" __VA_ARGS__), dbg_zero=0)

#undef  dbgmsg
#define dbgmsg(...)  (fprintf(stderr,"" __VA_ARGS__),     \
                      fprintf(stderr," \xF%s:%d\n",__FILE__,__LINE__), \
                      dbg_zero=0)
#undef  dbgerr
#define dbgerr(...)   dbgmsg("FAIL: " __VA_ARGS__)

#if DEBUG >= DEBUG_WARN
#undef  dbgwrn
#define dbgwrn(...)   dbgmsg("WARN: " __VA_ARGS__)
#endif

#if DEBUG >= DEBUG_INFO
#undef  dbginf
#define dbginf(...)   dbgmsg("INFO: " __VA_ARGS__)
#endif

#if DEBUG >= DEBUG_TEST
#undef  dbgtrc
#define dbgtrc(...)   dbgmsg("TRCE: " __VA_ARGS__)
#endif

// ## Testing 
//   dbgtst(char *, ...){ ...}     Starts a test case.
//                                 If DEBUG is undefined or lower than DEBUG_TEST, do nothing.
//                                 Stastics will be collected separately for each test case by dbg
//
//   dbgchk(test, char *, ...)     Perform the test and set errno (0: ok, 1: ko). If test fails
//                                 prints a message on stderr (works as printf(...)).
//                                 If DEBUG is undefined or lower than DEBUG_TEST, do nothing.
// 
//   dbgmst(test, char *, ...)     As dbgchk() but if test fails exit the program with abort().
// 
//   dbgblk {...}                  Execute the block only if DEBUG is defined as DBGLVEL_TEST.
//

#if DEBUG >= DEBUG_TEST

#undef  dbgtst
#define dbgtst(...)   for (int dbg_cnt = 1; \
                           dbg_cnt-- && !dbgmsg("TST[: " __VA_ARGS__); \
                           dbg_cnt = dbgmsg("TST]:")) 

#undef  dbgchk
#define dbgchk(e,...) \
  do { \
    int dbg_err=!(e); \
    fprintf(stderr,"%s: (%s) \xF%s:%d\n",(dbg_err?"FAIL":"PASS"),#e,__FILE__,__LINE__); \
    if (dbg_err) { fprintf(stderr,"    : " __VA_ARGS__); fputc('\n',stderr); } \
    errno = dbg_err; \
  } while(0)

#undef  dbgmst
#define dbgmst(e,...)  do { dbgchk(e,__VA_ARGS__); if (errno) abort();} while(0)

#undef  dbgblk
#define dbgblk     if (dbg_zero) ; else

// ## Tracking
//  Tracking is inspired to a novel idea presented by Kartik Agaram in his blog
//  quite some time ago: http://akkartik.name/post/tracing-tests
//
//  The basic idea is that we can flexibly set up test by monitoring the appearnce
//  (or the lack) of certain strings in a log. This will lessen the ties between 
//  the test and the code.
//
//  Consider the following test that checks that the string "INGESTION SUCCESSFUL" 
//  appears in the log but not one of the other two strings:
//
//    dbgtrk(",=INGESTION SUCCESSFUL" ",!INGESTION FAILED" ",!INGESTION HALTED") {
//      ... some code and function calls
//    }
// 
//  As long as the code emits those messages you are free to re-factor the code as
//  you please whereas a test like
//
//    ingest_err = ingest(file);
//    dbgchk(ingest_err == 0, "Ingestion failed with code %d",ingest_err);
//    if (!ingest_err) ...
//
//  relies on the exitance of a specific integer variable. For example you could not
//  simply write `if (!ingest(file)) ...` just because the test is there and needs
//  ingest_err to be there as well.
//  Sure, you still have to count on the code to emit certain strings, but this is
//  a much weaker coupling between the code and the test than before.
//  
//  Note that the tracking is *not* done during the execution as this might have
//  too great of an impact on the performance. Instead the check is done on the 
//  log itself by the `dbg` tool (see `dbg.c`). 
//
//   dbgtrk(char *) {...}    Specify the strings to be tracked within the scope of the
//                           block. If DEBUG is not defined or lower than DEBUG_TEST,
//                           execute the block but don't mark track strings.
//                           
//                           Theres NO comma between the strings, the separator is 
//                           the first character of the first pattern.
//                             Example: dbgtrk(",!test" ",=prod") {
//                                        ...
//                                      }
//
//  _dbgtrk(char *) {...}    Execute the block but don't mark string tracking.
//

#undef dbgtrk
#define dbgtrk(x)  for (int dbg_trk=!dbgmsg("TRK[: %s",x); \
                          dbg_trk;                                  \
                          dbg_trk=dbgmsg("TRK]:"))

// ## Profiling
//  The `dbgchk` function is intended as a quick and dirty way to determine the elapsed time
//  spent in a block of code. It's accuracy depends on the implementation of the `clock()`
//  function. The elapsed time is reported as a fraction:
//     Examples:
//        CLK]: +64000/1000000 sec. ut_test.c:67   -> 64 milliseconds
//        CLK]: +64/1000 sec. ut_test.c:67         -> 64 milliseconds
//
//   dbgclk(char *, ...) {...}     Measure the time needed to execute the block. If DEBUG is
//                                 undefined or lower than DEBUG_TEST, execute the block but
//                                 don't measure the elapsed time.
// 
//  _dbgclk(char *, ...) {...}     Execute the block but don't measure time.
//  

typedef struct {
  clock_t    clk;       time_t     time;    
  char       tstr[32];  struct tm *time_tm; 
  long int   elapsed; 
} dbgclk_t;

#undef  dbgclk
#define dbgclk(...)  \
  for (dbgclk_t dbg_ = {.elapsed = -1};   \
      \
       (dbg_.elapsed < 0) && ( \
          time(&dbg_.time), dbg_.time_tm=localtime(&dbg_.time),    \
          strftime(dbg_.tstr,32,"%Y-%m-%d %H:%M:%S",dbg_.time_tm),\
          dbgprt("CLK[: %s ",dbg_.tstr), dbgmsg(__VA_ARGS__) , \
          dbg_.clk = clock() \
       ) ;   \
      \
       dbg_.elapsed=(long int)(clock()-dbg_.clk),               \
       dbgmsg("CLK]: +%ld/%ld sec.", dbg_.elapsed, (long int)CLOCKS_PER_SEC) \
     )

// ## Grouping
// 
//  Say you are testing/developing a function and you placed some debugging 
//  code related to that function in various places in your code.
//  Now that everything works fine, you have to go back and delete or
//  comment out them to avoid having your log cluttered with now useless
//  messages. A little bit inconvenient, expecially if you feel 
//  you may have to re-enable them at a later stage at a later time.
//  In cases like this, you can plan in advance and define a "debugging 
//  group" and wrap the relevant messages with that group.
//  An example may clarify the concept better:
// 
//      // Define a group to debug/trace ingestion phase
//      // Set it to DBG_ON to enable it, set to DBG_OFF to disable it
//      #define DBG_CHECK_INGEST DBG_ON  
//      ...
//      DBG_CHECK_INGEST(dbgchk(dataread > 0,"No data read! (%d)",dataread));
//      ... (some functions later)
//      DBG_CHECK_INGEST(dbginf("Read %d read so far", dataread));
//      ... (into a function further away)
//      DBG_CHECK_INGEST(dbgchk(feof(f),"File had still data to read!"));
//
//  Defining `DBG_CHECK_INGEST` as `DBG_ON` the code will be compiled in,
//  defining it as `DBG_OFF` the code won't be compiled.
//
//   DBG_ON      Turn a debugging group ON
//   DBG_OFF     Turn a debugging group OFF
//

#undef  DBG_ON
#define DBG_ON(...)  __VA_ARGS__

#endif // DEBUG >= DEBUG_TEST

#if DEBUG >= DEBUG_MEM

// ## Memory usage
//  
//  If DEBUG is set to DEBUG_MEM, each call to the memory
//  related functions will produce a line in the log like this:
//
//       MTRK: function(args) ptr_in size ptr_out
//
//  0 N P malloc, calloc, strdup, realloc(NULL,N)
//  P 0 0 free, realloc(P,0)
//  P N P realloc(P,N)
//
//  The function dbgptr is meant to verify that a pointer to allocated
//  memory is "valid", i.e. that it belongs to a block previously allocated:
// 
//    dbgptr(void *p)   Checks if the pointer p is witin a block
//                      allocated with malloc(), calloc(), etc.
//
//  It can be helpful to check for buffer overruns
//  Also the more common functions that cause issues with allocated
//  memory (strcpy, memset, ...) now emit a line in the log:
//
//        MCHK: function(args) 32ABFE0 32ABFE4
//
//   If the first pointer is within an allocated block, the second one must
//   be as well.
//   If there is only one pointer at the end of a MCHK: line, that pointer
//   must be within a block allocated with malloc.
//
//  The checks are performed on the log by the dbg tool, not at runtime!


#include <stdlib.h>
#include <inttypes.h>

#define dbg_write(...)    (fprintf(stderr,__VA_ARGS__))
#define dbg_writeln(...)  (fprintf(stderr,__VA_ARGS__) , fprintf(stderr," \xF%s:%d\n",file,line))

// The only reason we need dbg_ptr2int is that %p representation of NULL pointers is
// compiler dependant. This way the pointer is uniquely converted in an integer.
// We'll need it in dbg as a label, we'll never convert it back to a pointer.
#define dbg_ptr2int(x)  ((uintptr_t)(x))

static inline void *dbg_malloc(size_t size, char *file, int line)
{
  dbg_write("MTRK: malloc(%zd) 0 %zd ",size,size);
  void *ptr = malloc(size);
  dbg_writeln("%zX",dbg_ptr2int(ptr));
  return ptr;
}

static inline void dbg_free(void *ptr, char *file, int line)
{
  dbg_writeln("MTRK: free(%zX) %zX 0 0",dbg_ptr2int(ptr),dbg_ptr2int(ptr));
  free(ptr);
}

static inline void *dbg_calloc(size_t count, size_t size, char *file, int32_t line)
{
  dbg_write("MTRK: calloc(%zd,%zd) 0 %zd ",count,size,count*size);
  void *ptr = calloc(count,size);
  dbg_writeln("%zX",dbg_ptr2int(ptr));
  return ptr;
}

static inline void *dbg_realloc(void *ptr, size_t size, char *file, int32_t line)
{
  dbg_write("MTRK: realloc(%zX,%zd) %zX %zd ",dbg_ptr2int(ptr),size,dbg_ptr2int(ptr),size);
  ptr = realloc(ptr,size);
  dbg_writeln("%zX",dbg_ptr2int(ptr));
  return ptr;
}

// strdup() is somewhat special being a Posix but not a C standard function
// I decided to provide an implementation of strdup/strndup rather than
// relying on the compiler library to have it. It's not 100% correct but
// I didn't want to make it more complicated than it is already.

static inline char *dbg_strdup(const char *s, char *file, int line)
{
  dbg_write("MTRK: strdup(%zX) 0 ",dbg_ptr2int(s));
  size_t size = strlen(s)+1;
  char *ptr = (char *)malloc(size);
  if (ptr) strcpy(ptr,s);
  dbg_writeln("%zd %zX",size,dbg_ptr2int(ptr));
  return ptr;
}

static inline char *dbg_strndup(const char *s, size_t size, char *file, int line)
{
  dbg_write("MTRK: strndup(%zX,%zd) 0 ",dbg_ptr2int(s),size);
  char *ptr = (char *)malloc(size+1);
  if (ptr) { strncpy(ptr,s,size); ptr[size] = '\0'; }
  dbg_writeln("%zd %zX",size+1,dbg_ptr2int(ptr));
  return ptr;
}

#define strdup(s)     dbg_strdup(s,__FILE__, __LINE__)
#define strndup(s,n)  dbg_strndup(s,n,__FILE__, __LINE__)


// Check boundaries for the most common functions

static inline char *dbg_strcpy(char *dest, char *src,char *file, int line)
{
  size_t size = src? strlen(src)+1 : 0;
  dbg_writeln("MCHK: strcpy(%zX,%zX) %zX %zX",dbg_ptr2int(dest),dbg_ptr2int(src),dbg_ptr2int(dest),dbg_ptr2int(dest+size));
  return strcpy(dest,src);
}

static inline char *dbg_strncpy(char *dest, char *src, size_t size, char *file, int line)
{
  dbg_writeln("MCHK: strncpy(%zX,%zX,%zd) %zX %zX",dbg_ptr2int(dest),dbg_ptr2int(src),size,dbg_ptr2int(dest),dbg_ptr2int(dest+size));
  return strncpy(dest,src,size);
}

static inline char *dbg_strcat(char *dest, char *src,char *file, int line)
{
  size_t size = (dest ? strlen(dest) : 0) + (src? strlen(src)+1 : 0);
  dbg_writeln("MCHK: strcat(%zX,%zX) %zX %zX",dbg_ptr2int(dest),dbg_ptr2int(src),dbg_ptr2int(dest),dbg_ptr2int(dest+size));
  return strcpy(dest,src);
}

static inline char *dbg_strncat(char *dest, char *src,size_t size, char *file, int line)
{
  dbg_writeln("MCHK: strncat(%zX,%zX,%zd) %zX %zX",dbg_ptr2int(dest),dbg_ptr2int(src),size,dbg_ptr2int(dest),dbg_ptr2int(dest+((dest ? strlen(dest) : 0) + size)));
  return strncpy(dest,src,size);
}

static inline char *dbg_memcpy(void *dest, void *src, size_t size, char *file, int line)
{
  dbg_writeln("MCHK: memcpy(%zX,%zX,%zd) %zX %zX",dbg_ptr2int(dest),dbg_ptr2int(src),size,dbg_ptr2int(dest),dbg_ptr2int((char *)dest+size));
  return memcpy(dest,src,size);
}

static inline char *dbg_memmove(void *dest, void *src, size_t size, char *file, int line)
{
  dbg_writeln("MCHK: memmove(%zX,%zX,%zd) %zX %zX",dbg_ptr2int(dest),dbg_ptr2int(src),size,dbg_ptr2int(dest),dbg_ptr2int((char *)dest+size));
  return memmove(dest,src,size);
}

static inline void *dbg_memset(void *dest, int c, size_t size, char *file, int line)
{
  dbg_writeln("MCHK: memset(%zX,%zd) %zX %zX",dbg_ptr2int(dest),size,dbg_ptr2int(dest),dbg_ptr2int((char *)dest+size));
  return memset(dest,c,size);
}

#define malloc(sz)    dbg_malloc(sz,__FILE__, __LINE__)
#define calloc(n,sz)  dbg_calloc(n,sz,__FILE__, __LINE__)
#define realloc(p,sz) dbg_realloc(p,sz,__FILE__, __LINE__)
#define free(p)       dbg_free(p,__FILE__, __LINE__)

#define strcpy(d,s)    dbg_strcpy(d,s,__FILE__, __LINE__)
#define strncpy(d,s,n) dbg_strncpy(d,s,n,__FILE__, __LINE__)
#define strcat(d,s)    dbg_strcat(d,s,__FILE__, __LINE__)
#define strncat(d,s,n) dbg_strncat(d,s,n,__FILE__, __LINE__)
#define memcpy(d,s,n)  dbg_memcpy(d,(void*)(s),n,__FILE__, __LINE__)
#define memmove(d,s,n) dbg_memmove(d,(void*)(s),n,__FILE__, __LINE__)

#undef  dbgptr
#define dbgptr(p) dbgmsg("MCHK: ptr %zX", dbg_ptr2int(p))

#endif // DEBUG >= DEBUG_MEM


#endif  // DEBUG

#endif // DBG_H_VER

#line 1 "try.h"
/*  (C) by:         Remo Dentato (rdentato@gmail.com)
**  License:        https://opensource.org/licenses/MIT
**  Documentation:  https://github.com/rdentato/try
**  Discord server: https://discord.gg/QFzP9vaR8j
*/

#ifndef TRY_VERSION // 0.3.3-rc
#define TRY_VERSION    0x0003003C

#include <stdio.h>
#include <setjmp.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#ifndef exception_info  // Additional fileds for the exception
#define exception_info 
#endif

typedef struct exception_s {
   int   exception_num;
   int   line_num;
   char *file_name;
   exception_info
} exception_t;

typedef struct try_ctx_s {                    // Context variables for a try block
                    jmp_buf  jmp_buffer;     
  volatile struct try_ctx_s *prev_ctx;        // Link to the parent context for nested try blocks
  volatile              int  exception_num;   // Non-zero if an exception has been thrown in this context
  volatile              int  caught;          // Non-zero if an exception has been caught in this context
} try_ctx_t;

// If your compiler has a different keyword for thread local variables, define TRY_THREAD 
// before including `try.h`. Define it as empty if there is no support at all.
//#define TRY_THREAD
#ifndef TRY_THREAD
#ifdef _MSC_VER
  #define TRY_THREAD __declspec( thread )
#else
  #define TRY_THREAD __thread
#endif
#endif

extern TRY_THREAD try_ctx_t  *try_ctx_list;
extern TRY_THREAD exception_t exception;

// Declare only ONCE a variable of type `try_t` as if it was an int. For example: try_t catch = 0;
#define try_t TRY_THREAD try_ctx_t *try_ctx_list=NULL; exception_t TRY_THREAD exception; int

#ifndef tryabort
#define tryabort() (fprintf(stderr,"ERROR: Unhandled exception %d. %s:%d\n",\
                            exception.exception_num,exception.file_name,exception.line_num))
#endif

static inline int try_abort() {abort(); return 1;}

#define try        for ( try_ctx_t try_ctx = {.exception_num = 0, .prev_ctx = try_ctx_list, .caught = -1 }; \
                        (try_ctx.exception_num && !try_ctx.caught)   ? \
                                           (tryabort(), try_abort()) : \
                                           ((try_ctx.caught++ < 0) && (try_ctx_list = &try_ctx)); \
                         try_ctx_list = (try_ctx_t *)(try_ctx.prev_ctx)) \
                     if (setjmp(try_ctx.jmp_buffer) == 0) 

#define catch(...)   else if (catch__check(__VA_ARGS__ +0) && catch__caught()) 

// The argument to `catch()` can be an integer or a function from integers to integers
#define catch__check(x) _Generic((x), int(*)(int): (((int(*)(int))(x)) == NULL) || ((int(*)(int))(x))(try_ctx.exception_num), \
                                          default: ((int)((uintptr_t)(x)) == 0) || catch__eq((int)((uintptr_t)(x)),try_ctx.exception_num) )

static inline int catch__eq(int x, int e) {return x == e;}

#define catch__caught() (try_ctx_list=(try_ctx_t *)(try_ctx.prev_ctx),try_ctx.caught=1)


// To be consistent with setjmp/longjmp behaviour, if `exc` is 0, it is set to 1.
#define throw(exc, ...) \
  do { \
    memset(&exception,0,sizeof(exception_t)); \
    exception = ((exception_t){exc, __LINE__, __FILE__, __VA_ARGS__});\
    if (exception.exception_num == 0) exception.exception_num = 1; \
    if (try_ctx_list == NULL) { tryabort(); abort(); } \
    try_ctx_list->exception_num = exception.exception_num; \
    longjmp(try_ctx_list->jmp_buffer, exception.exception_num); \
  } while(0)

// Pass the same exception to parent try/catch block
#define rethrow(...) throw(try_ctx.exception_num, __VA_ARGS__)

// Quit a try/block in a clean way
#define leave() continue

#endif  // TRY_VERSION

#line 1 "vrg.h"
/*
**  (C) by Remo Dentato (rdentato@gmail.com)
**
** This software is distributed under the terms of the MIT license:
**  https://opensource.org/licenses/MIT
**
** The technique to handle 0 arguments function has been taken from:
** https://gustedt.wordpress.com/2010/06/08/detect-empty-macro-arguments/
*/

/*
# Variadic functions

Say you want to define a variadic function with the following prototype:

    myfunc([int a [, char b [, void *c]]])

In other words, you want all arguments to be argional.

Simply, define your function with another name (say `my_func()`) and specify
how it should be called when invoked with 0, 1, 2 or 3 paramenters as shown
in the example below.

Example:

    #include "vrg.h"

    int my_func(int a, char b, void *c);

    #define myfunc(...)     vrg(myfunc, __VA_ARGS__)
    #define myfunc01()       my_func(0,'\0',NULL)
    #define myfunc_1(a)      my_func(a,'\0',NULL)
    #define myfunc_2(a,b)    my_func(a,b,NULL)
    #define myfunc_3(a,b,c)  my_func(a,b,c)

Note that the `_1`, `_2`, etc, is appended to the name of the function.
Having 0 argument is a special case and `01` will be appended to the function name.

# Command line options
 A minimal replacement of getarg.

 Define `VRGARGS` before including `vrg.h` only once, usually in the same source where `main()` is.

  #define VRGCLI
  #include "vrg.h"

... In the code:

 vrgcli("Version 1.0", argc,argv) {  // as received by main

   vrgarg("-f filename\tLoad file") { // explanation of the arg comes after \t
      // Will work both for `-f pippo` and `-fpippo`
      // Set what you need to set
      printf("file: `.*s`",vrglen, vrgargarg);
   }

   vrgarg("-o [filename]\tEnable output (on stdout by default)") {
       // Here filename is argional
   }

   vrgarg() { // Default handler (if none of the above triggers)
      // cought an unrecognized parameter.
   }
 }
 // At the end, the vrgargn variable contains the index of the argument
 // that is sill to be processed:
 //
 // Example (assuming -f takes an argument):
 //    prg -f fname pluto
 // vrgargn will be 3 pointing to "pluto" (argv[3])
 //

 vrgusage(); // Will print an error message (if specified) and the usage. Then will exit.
 vrgerror(); // Will print an error message (if specified). Then will exit.

 Read README.md for full details.
***/

#ifndef VRG_VERSION
#define VRG_VERSION 0x0009000C // 0.9.0-RC

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// Variadic functions

#define vrg_cnt(_1,_2,_3,_4,_5,_6,_7,_8,_9,_N, ...) _N
#define vrg_argn(...)    vrg_cnt(__VA_ARGS__, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define vrg_commas(...)  vrg_cnt(__VA_ARGS__,    0, 0, 0, 0, 0, 0, 0, 0, _)
#define vrg_comma(...) ,
#define vrg_empty_(_0, _1, _2, _3) vrg_commas(vrg_cat5(vrg_empty_, _0, _1, _2, _3))
#define vrg_empty____0 ,
#define vrg_empty(...) vrg_empty_( vrg_commas(__VA_ARGS__)    , vrg_commas(vrg_comma __VA_ARGS__), \
                                   vrg_commas(__VA_ARGS__ ( )), vrg_commas(vrg_comma __VA_ARGS__ ( )) )

#define vrg_cat5(_0, _1, _2, _3, _4) _0 ## _1 ## _2 ## _3 ## _4
#define vrg_cat3(_0, _1, _2)         vrg_cat5(_0, _1, _2, , )

// There are two of them because if one of the arguments of `vrg` is defined with `vrg`, the
// macro expansion would stop. In that case define one with `vrg` and the other with `VRG`.
// Hopefully two will be enough.
#define vrg(vrg_f,...) vrg_cat3(vrg_f, vrg_empty(__VA_ARGS__) , vrg_argn(__VA_ARGS__))(__VA_ARGS__)
#define VRG(vrg_f,...) vrg_cat3(vrg_f, vrg_empty(__VA_ARGS__) , vrg_argn(__VA_ARGS__))(__VA_ARGS__)

// If you want just to distinguish between having or not having arguments you cam use `vrg_`:
// #define vf(...) vrg_(vf,__VA_ARGS__)
// #define vf01()    printf("ARGS: none\n");
// #define vf__(...) printf("ARGS: " __VA_ARGS__)

#define vrg_zcommas(...) vrg_cnt(__VA_ARGS__, __, __, __, __, __, __, __, __, 01)
#define vrg_(vrg_f,...) vrg_cat3(vrg_f, vrg_zcommas(__VA_ARGS__) , )(__VA_ARGS__)
#define VRG_(vrg_f,...) vrg_cat3(vrg_f, vrg_zcommas(__VA_ARGS__) , )(__VA_ARGS__)

// Command line options arguments

#ifdef VRGCLI

#ifndef VRG_STR_USAGE
#define VRG_STR_USAGE        "USAGE"
#endif

#ifndef VRG_STR_ERROR       
#define VRG_STR_ERROR        "ERROR"
#endif

#ifndef VRG_STR_OPTIONS
#define VRG_STR_OPTIONS      "OPTIONS"
#endif

#ifndef VRG_STR_INVALID     
#define VRG_STR_INVALID      "Invalid value '%V' for %T '%N'"
#endif

#ifndef VRG_STR_INV_ARGUMENT
#define VRG_STR_INV_ARGUMENT "argument"
#endif

#ifndef VRG_STR_INV_OPTION  
#define VRG_STR_INV_OPTION   "option"
#endif

#ifndef VRG_STR_NO_ARGOPT  
#define VRG_STR_NO_ARGOPT    "Missing argument for option '%.*s'"
#endif

#ifndef VRG_STR_NO_ARGUMENT  
#define VRG_STR_NO_ARGUMENT  "Missing argument '%.*s'"
#endif

#ifndef VRG_STR_ARGUMENTS
#define VRG_STR_ARGUMENTS    "ARGUMENTS"
#endif

//   prog  -b 23 -a  pippo pluto
//         ╰─┬─╯ ╰┬╯╰────┬─────╯
//           │    │      ╰────────── arguments
//           │    ╰───────────────── flag
//           ╰────────────────────── flag with an argument

//  "-h|--help\tPrint usage details"
//  "-f|--file filename\nThe file to open"
//  "--no-duplicates [Y/N]\tEliminates duplicates (defaults to 'Y')"

// Private version of vrg()
#define vRG(vrg_f,...) vrg_cat3(vrg_f, vrg_empty(__VA_ARGS__) , vrg_argn(__VA_ARGS__))(__VA_ARGS__)

#define VRG_ARG_IS_MANDATORY    0x01
#define VRG_ARG_IS_OPTIONAL     0x02
#define VRG_ARG_IS_OPTWITHARG   0x03
#define VRG_ARG_FOUND           0x80
#define VRG_ARG_IS_SHORTOPT     0x10
#define VRG_ARG_IS_LONGOPT      0x20
#define VRG_ARG_IS_OPTION       0x30
#define VRG_ARG_IS_POSITIONAL   0x40

#define vrg_argdef_set(n,f) ((n)->argis |= VRG_ARG_ ## f)
#define vrg_argdef_check(n,f) ((n)->argis & VRG_ARG_ ## f)

// The definitions of CLI arguments are stored in a linked list. Each node of the list
// holds information on the type of argument specified by `vrgarg()`

typedef struct vrg_def_s {
    char             *def;
    struct vrg_def_s *next;
    signed char       pos;    // Which positional argument this is (-1 for flags)
    unsigned char     tab;    // up to the \t charcter
    unsigned char     argis;  // See flags above
    unsigned char     fst;    // first char of the (long) flag or argument name
    unsigned char     lst;    // lentgth of the (long) flag or argument name
} vrg_def_t;

static vrg_def_t *vrg_arglist = NULL;

static int   vrgargn = 0;
static char *vrgarg;

static int    vrg_argfound = 0;
static int    vrg_pos;

// These are usually seto to the value of `argc` and `argv` passed to `main()`
static int    vrg_argc;
static char **vrg_argv=NULL;

// We encode some of the CLI characteristic to ease the generation of `vrgusage()` text.
static int vrg_cli_has = 0;

#define VRG_CLI_HAS_OPTIONS     0x01   // The CLI has at least one option
#define VRG_CLI_HAS_DEFAULT     0x02   // The CLI handles unknown arguments
#define VRG_CLI_HAS_ARGS        0x04   // The CLI has positional arguments
#define VRG_CLI_HAS_NOMOREOPT   0x10   // Encountered '--' in the args, no more options checked

// To simplify reading, the bit masking operation are abstracted away by the `set` and `check` functions
#define vrg_cli_set(F)   (vrg_cli_has |= (VRG_CLI_ ## F))
#define vrg_cli_check(F) (vrg_cli_has  & (VRG_CLI_ ## F))

static char *vrg_emptystr  = "";
static char *vrg_help      = NULL;

#define vrgerror(...) do { fflush(stdout); fprintf(stderr, VRG_STR_ERROR ": " __VA_ARGS__); exit(1); } while(0)

#define vrgusage(...) vrg_(vrgusage,__VA_ARGS__)
#define vrgusage__(...) (fflush(stdout), fprintf(stderr,"" __VA_ARGS__), vrgusage01())

// static int vrgusage01();

#define vrgcli(...) vRG(vrgcli,__VA_ARGS__)
#define vrgcli01()  vrgcli_3(NULL,argc,argv)
#define vrgcli_1(s) vrgcli_3(s,argc,argv)

#define vrgcli_3(hlp_, argc_, argv_) \
  for (vrgargn = 0, vrg_pos = 0, vrg_argfound = vrg_invalid(NULL), vrg_help = hlp_, vrg_argc = argc_, vrg_argv = argv_, vrgarg=vrg_emptystr  \
      ; ((vrgargn < vrg_argc) || vrg_check_mandatory()) && ((vrgargn == 1) ? !(vrg_pos=0) : 1) \
      ; vrg_argfound ? vrg_argfound = 0 : vrgargn++ )

#define vrgarg(...) vRG(vrgarg,__VA_ARGS__)
                                                                           //╭─ Just assign vrgarg to current argument
#define vrgarg01() if (vrgargn == 0 || vrgargn >= vrg_argc || vrg_argfound || !(vrgarg = vrg_argv[vrgargn])) \
                       vrg_cli_set(HAS_DEFAULT); \
                   else

#define vrg_argdef vrg_cat3(vrg_def_, __LINE__,) // Each invocation of vrgarg() will create a unique node.
//               ╰──────────────────────╮  
#define vrgarg_1(def_) static vrg_def_t vrg_argdef = { .def = def_, .pos = -1, .argis = 0,  .tab = 0 }; \
                            if (vrgargn == 0) vrg_add_argdef(&vrg_argdef); \
                       else if (vrgargn >= vrg_argc) { vrg_check_mandatory(); break; } \
                       else if (!vrg_checkarg(&vrg_argdef, vrg_argv[vrgargn])) ; \
                       else

#define vrgarg_2(def_, chk_)             vrgarg_1(def_) if (!(chk_(vrgarg) || vrg_invalid(&vrg_argdef))); else
#define vrgarg_3(def_, chk_, a_)         vrgarg_1(def_) if (!(chk_(vrgarg, a_) || vrg_invalid(&vrg_argdef))); else
#define vrgarg_4(def_, chk_, a_, b_)     vrgarg_1(def_) if (!(chk_(vrgarg, a_, b_) || vrg_invalid(&vrg_argdef))); else
#define vrgarg_5(def_, chk_, a_, b_, c_) vrgarg_1(def_) if (!(chk_(vrgarg, a_, b_, c_) || vrg_invalid(&vrg_argdef))); else


static int vrg_invalid(vrg_def_t *node)
{
  if (node == NULL) return 0;
  if ((vrgarg == NULL || *vrgarg == '\0') && (vrg_argdef_check(node,IS_MANDATORY) == 0)) return 1;

  fputs(VRG_STR_ERROR ": ", stderr);
  for (char *m = VRG_STR_INVALID; *m; m++) {
      if (m[0] == '%' && m[1]) {
          m++;
          switch (*m) {
              case 'V' : fprintf(stderr,"%s",vrgarg); break;
              case 'T' : fprintf(stderr,"%s",node->def[0] == '-'? VRG_STR_INV_OPTION : VRG_STR_INV_ARGUMENT); break;
              case 'N' : fprintf(stderr,"%.*s",node->lst, node->def); break;
              case '%' : fputc('%',stderr); break;
              default  : fprintf(stderr,"%%%c",*m);
          }
      } 
      else fputc(*m,stderr);
  }
  fputc('\n',stderr);
  exit(1);
}

static void vrg_set_option_def(vrg_def_t *node, char *cur_def)
{
  cur_def++;
  if (*cur_def != '-') {
      vrg_argdef_set(node,IS_SHORTOPT);
      node->fst = cur_def - node->def;
      node->lst = node->fst + 1;
      // Move forward in case there's a long format version for this option
      while (*cur_def && *cur_def != '\t' && *cur_def != '-') cur_def++;
      if (*cur_def == '-') cur_def++;
  }

  if (*cur_def == '-') { // check for long
      cur_def++;
      if (*cur_def && *cur_def != ' ' && *cur_def != '\t') { // Found a long one
          vrg_argdef_set(node,IS_LONGOPT);         
          // compute the length
          node->fst = cur_def - node->def;
          while (*cur_def && *cur_def != ' ' && *cur_def != '\t') cur_def++;
          node->lst = cur_def - node->def;
      }
  }

  // Check if this option has an argument
  while (*cur_def == ' ') cur_def++;
  if (*cur_def == '[') vrg_argdef_set(node,IS_OPTIONAL); // optional argument
  else if (*cur_def != '\0' && *cur_def != '\t') vrg_argdef_set(node,IS_MANDATORY); // mandatory argument
}

static void vrg_add_argdef(vrg_def_t *node)
{
  char *cur_def = node->def;
  
  node->next = vrg_arglist;
  vrg_arglist = node;

  while(isspace(*cur_def)) cur_def++;
  node->def = cur_def;
  
  if (cur_def[0] == '-' && cur_def[1] != '\0')  { // it's an option
      vrg_cli_set(HAS_OPTIONS);
      vrg_set_option_def(node, cur_def);
  }
  else { // it's a positional argument
      vrg_cli_set(HAS_ARGS);
      node->pos = vrg_pos++;
      node->fst = cur_def - node->def;

      vrg_argdef_set(node,IS_POSITIONAL);

      if (*cur_def == '[') vrg_argdef_set(node,IS_OPTIONAL); // and it's optional
      else vrg_argdef_set(node,IS_MANDATORY);

      while (*cur_def && !isspace(*cur_def))  cur_def++;

      node->lst = cur_def - node->def;
  }
}

// -x [data]\tdescription -y delta
// file
static int vrg_checkarg(vrg_def_t *node, char *cli_arg)
{
  char *cur_def = node->def;

  vrgarg = vrg_emptystr;

  if (!vrg_cli_check(HAS_NOMOREOPT) && (cli_arg[0] == '-')) { // found a flag

      if ((cli_arg[1] == '-') ) { // Double dash '--'
          if (cli_arg[2] == '\0') { // Stop options
              vrg_cli_set(HAS_NOMOREOPT); // Signalling the end of options.
              vrgargn++;
              vrg_argfound = 1;
              return 0;
          }
          if (!vrg_argdef_check(node,IS_LONGOPT)) return 0;
          cli_arg += 2;
          int n;
          for (n = node->fst; *cli_arg && (n < node->lst) && (node->def[n] == *cli_arg); n++) cli_arg++;
          if (n < node->lst) return 0;
          if (*cli_arg == '=') cli_arg++;
          else if (*cli_arg != '\0') return 0;
      }
      else {
          if (!vrg_argdef_check(node,IS_SHORTOPT)) return 0; // The node is a not a flag ...
          if (cli_arg[1] != cur_def[1]) return 0; // ... or it's not the same flag
          cli_arg += 2;
      }

      // Now `cli_arg` is right after the end of the option. for example:
      //  -x     --x-rays=
      //    ^             ^   
      if (vrg_argdef_check(node,IS_OPTWITHARG)) { // look for an argument to the option
          if (cli_arg[0] != '\0' || cli_arg[-1] == '=') // option argument is attached to the option itself -x32 --x-rays=32
              vrgarg = cli_arg;  
          else if ((vrgargn+1 < vrg_argc) && vrg_argv[vrgargn+1][0] != '-') // take next argument (if any) but ignore flags
              vrgarg = vrg_argv[++vrgargn];

          if (vrgarg == vrg_emptystr && vrg_argdef_check(node,IS_MANDATORY))
              vrgerror(VRG_STR_NO_ARGOPT "\n",node->lst,cur_def);

          // while (isspace(*vrgarg)) vrgarg++;
      }
  }
  else { // Checking for an agument

     // but a flag has been found.
      if (!vrg_cli_check(HAS_NOMOREOPT) && cli_arg[0] == '-') return 0;

      // It's an argument but the current pos doesnt match!
      if (vrg_pos != node->pos) return 0;

      vrgarg = cli_arg;
      vrg_argdef_set(node,FOUND);
      vrg_pos++;
  }

  vrgargn++;
  vrg_argfound = 1;
  return 1;
}

static int vrg_check_mandatory()
{
  vrg_def_t *node = vrg_arglist;
  int errors=0;
  while (node) {
      if (vrg_argdef_check(node,IS_POSITIONAL) && vrg_argdef_check(node,IS_MANDATORY) && !vrg_argdef_check(node,FOUND)) {
          errors++;
          fprintf(stderr,VRG_STR_ERROR ": " VRG_STR_NO_ARGUMENT "\n", node->lst,node->def);
      }
      node = node->next;
  }
  if (errors) exit(1);
  return 0;
}

#define vrgusage01() exit(vrghelp())

static int vrghelp()
{
  vrg_def_t *node = vrg_arglist;
  vrg_def_t *inverted = NULL;
  vrg_def_t *tmp_node;
  char *s=NULL;
  int max_tab = 0;

  fflush(stdout);

  // Just print the help message passed to vrgcli()
  if (vrg_help && *vrg_help == '#') {
      while (*vrg_help && *vrg_help != '\n') vrg_help++;
      if (*vrg_help) vrg_help++;
      fprintf(stderr,"%s\n",vrg_help);
      exit(1);
  }

  // Invert the list so that arguments are in the same order 
  // they were specified in `vrgcli()` and compute the space
  // needed to align the definition part.
  while (node) {
      s = node->def;
      while (*s && *s != '\t') s++;
      node->tab = s - node->def;
      if (node->tab > max_tab) max_tab = node->tab;

      tmp_node = node->next;
      node->next = inverted;
      inverted = node;
      node = tmp_node;
  }
  vrg_arglist = inverted;

  char *prgname = vrg_argv[0];
  while (*prgname) prgname++;
  while ((prgname > vrg_argv[0]) && (prgname[-1] !='\\') && (prgname[-1] != '/'))
      prgname--;

  fprintf(stderr, "%s:\n  %s ",VRG_STR_USAGE, prgname);

  if (vrg_cli_check(HAS_OPTIONS)) fprintf(stderr, "[%s] ",VRG_STR_OPTIONS);

  if (vrg_cli_check(HAS_ARGS)) {
      for (node = vrg_arglist; node ; node = node->next) {
          if (node->def[0] != '-')
              fprintf(stderr,"%.*s ",node->tab,node->def);
      }
  }

  if (vrg_cli_check(HAS_DEFAULT)) fprintf(stderr,"...");

  fprintf(stderr,"\n");
  if (vrg_help && *vrg_help) fprintf(stderr,"  %s\n",vrg_help);

  if (vrg_cli_check(HAS_ARGS)) {
      fprintf(stderr,"\n%s:\n",VRG_STR_ARGUMENTS);
      for (node = vrg_arglist; node ; node = node->next) {
          if (node->def[0] != '-') {
              fprintf(stderr,"  %.*s%*s%s\n",node->tab,node->def,max_tab - node->tab, "", node->def + node->tab);
          }
      }
  }

  if (vrg_cli_check(HAS_OPTIONS)) {
    fprintf(stderr,"\n%s:\n", VRG_STR_OPTIONS);
    for (node = vrg_arglist; node ; node = node->next) {
        if (node->def[0] == '-') {
            fprintf(stderr,"  %.*s%*s%s\n",node->tab,node->def,max_tab - node->tab, "", node->def + node->tab);
        }
    }
  }

  return 1;
}

#endif
#endif
#line 1 "val.h"
// Tue Jun  4 12:37:56 CEST 2024
#line 1 "val_.h"
//  SPDX-FileCopyrightText: © 2023 Remo Dentato <rdentato@gmail.com>
//  SPDX-License-Identifier: MIT
//  PackageVersion: 0.1.0 Beta

#ifndef VAL_VERSION
#define VAL_VERSION 0x0001000B

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

// DBG
#if defined(NDEBUG) && defined(DEBUG)
#undef DEBUG
#endif

#ifdef DEBUG
  #define valdbg(...) (fprintf(stderr,"      INFO|  "),fprintf(stderr, __VA_ARGS__),fprintf(stderr," [%s:%d]\n",__FILE__,__LINE__))
#else
  #define valdbg(...)
#endif
#define val_dbg(...)

#define VAL_cnt(x1,x2,x3,x4,xN, ...) xN
#define VAL_n(...)       VAL_cnt(__VA_ARGS__, 4, 3, 2, 1, 0)
#define VAL_join(x ,y)   x ## y
#define VAL_cat(x, y)    VAL_join(x, y)
#define VAL_vrg(f, ...)  VAL_cat(f, VAL_n(__VA_ARGS__))(__VA_ARGS__)
#define VAL_VRG(f, ...)  VAL_cat(f, VAL_n(__VA_ARGS__))(__VA_ARGS__)

// ## REFERENCES
//
// "Crafting Interpreters" by Robert Nystrom 
// https://craftinginterpreters.com/optimization.html
//
// "NaN boxing or how to make the world dynamic"
// https://piotrduperas.com/posts/nan-boxing
//
// Nanobox implementation by Viktor Söderqvist 
// https://github.com/zuiderkwast/nanbox/blob/master/nanbox.h
//
// Intel® 64 and IA-32 Architectures Software Developer’s Manual vol 1.
// https://xem.github.io/minix86/manual/intel-x86-and-64-manual-vol1/o_7281d5ea06a5b67a.html
// (see table 4.3 on pages 90)

//                  ╭─── Quiet NaN
//                  │╭── QNaN Floating-Point Indefinites has 0 fron here up to the end
//                  ▼▼ 
//   x111 1111 1111 1xxx FF FF FF FF FF FF
//    ╰──────┬──────╯    ╰───────┬───────╯
//           │                   │ 
//    12 bits set to 1   Payload (48 bits)
//          7FF8

typedef struct {uint64_t v;} val_t;

typedef struct vec_s *vec_t;
typedef struct buf_s *buf_t;

// Some bitmask
#define VAL_TYPE_MASK ((uint64_t)0xFFFF000000000000)
#define VAL_NAN_MASK  ((uint64_t)0x7FF0000000000000)
#define VAL_PAYLOAD   ((uint64_t)0x0000FFFFFFFFFFFF)

#define VAL_BUF_MASK  ((uint64_t)0xFFFE000000000000)
#define VAL_VEC_MASK  ((uint64_t)0xFFFF000000000000)

#define VAL_STR_MASK  ((uint64_t)0x7FFC000000000000)
#define VAL_PTR_MASK  ((uint64_t)0x7FFD000000000000)
#define VAL_UNS_MASK  ((uint64_t)0x7FFE000000000000)
#define VAL_INT_MASK  ((uint64_t)0x7FFF000000000000)
#define VAL_CST_MASK  ((uint64_t)0xFFF9000000000000)
#define VAL_BOL_MASK  ((uint64_t)0xFFF8FFFFFFFFFF00)
#define VAL_STO_MASK  ((uint64_t)0x7FF9000000000000)

#define VALDOUBLE   0x0001
#define VALBOOL     0x0002
#define VALNIL      0x0004
#define VALSTORED   0x7FF9
#define VALSTRING   0x7FFC
#define VALPOINTER  0x7FFD
#define VALINTEGER  0x7FFF
#define VALCONST    0xFFF9
#define VALBUF      0xFFFE
#define VALVEC      0xFFFF
#define VALPREDEF   0xFFF8


// Check the type of a val_t variable
#define val_isbool(x)    (((x).v & (uint64_t)0xFFFFFFFFFFFFFFFE) == VAL_BOL_MASK )
#define val_isdouble(x)  (((x).v & VAL_NAN_MASK)  != VAL_NAN_MASK)
#define val_isnil(x)     ((x).v == valnil.v)

#define valisinteger(x) ((val(x).v & (uint64_t)0xFFFE000000000000) == VAL_UNS_MASK)
#define valissigned(x)  ((val(x).v & VAL_TYPE_MASK) == VAL_INT_MASK)
#define valisbool(x)    ((val(x).v & (uint64_t)0xFFFFFFFFFFFFFFFE) == VAL_BOL_MASK )
#define valisdouble(x)  ((val(x).v & VAL_NAN_MASK)  != VAL_NAN_MASK)
#define valispointer(x) ((val(x).v & VAL_TYPE_MASK) == VAL_PTR_MASK)
#define valisstring(x)  ((val(x).v & VAL_TYPE_MASK) == VAL_STR_MASK)
#define valisvec(x)     ((val(x).v & VAL_TYPE_MASK) == VAL_VEC_MASK)
#define valisbuf(x)     ((val(x).v & VAL_TYPE_MASK) == VAL_BUF_MASK)
#define valisstored(x)  valisstored_(val(x))
int valisstored_(val_t x);

#define valisownedvec(x)  ((val(x).v & (VAL_TYPE_MASK | 1)) == (VAL_VEC_MASK | 1))
#define valisownedbuf(x)  ((val(x).v & (VAL_TYPE_MASK | 1)) == (VAL_BUF_MASK | 1))

#define valisnil(x)     (val(x).v == valnil.v)
#define valiszero(x)    ((val(x).v & VAL_PAYLOAD) == 0)
#define valiserror(x)   (val(x).v == valerror.v)

// Store a value into a val_t variable
static inline val_t val_fromchar(char v)             {val_t ret; ret.v = VAL_INT_MASK | ((uint64_t)(v) & VAL_PAYLOAD);  return ret;}
static inline val_t val_fromuchar(unsigned char v)   {val_t ret; ret.v = VAL_UNS_MASK | ((uint64_t)(v) & VAL_PAYLOAD);  return ret;}
static inline val_t val_fromint(int v)               {val_t ret; ret.v = VAL_INT_MASK | ((uint64_t)(v) & VAL_PAYLOAD);  return ret;}
static inline val_t val_fromuint(unsigned int v)     {val_t ret; ret.v = VAL_UNS_MASK | ((uint64_t)(v) & VAL_PAYLOAD);  return ret;}
static inline val_t val_fromshort(short v)           {val_t ret; ret.v = VAL_INT_MASK | ((uint64_t)(v) & VAL_PAYLOAD);  return ret;}
static inline val_t val_fromushort(unsigned short v) {val_t ret; ret.v = VAL_UNS_MASK | ((uint64_t)(v) & VAL_PAYLOAD);  return ret;}
static inline val_t val_fromlong(long v)             {val_t ret; ret.v = VAL_INT_MASK | ((uint64_t)(v) & VAL_PAYLOAD);  return ret;}
static inline val_t val_fromulong(unsigned long v)   {val_t ret; ret.v = VAL_UNS_MASK | ((uint64_t)(v) & VAL_PAYLOAD);  return ret;}
static inline val_t val_frombool(_Bool v)            {val_t ret; ret.v = VAL_BOL_MASK | ((uint64_t)((v) & 1));          return ret;}
static inline val_t val_fromptr(void *v)             {val_t ret; ret.v = VAL_PTR_MASK | ((uintptr_t)(v) & VAL_PAYLOAD); return ret;}
static inline val_t val_fromstr(void *v)             {val_t ret; ret.v = VAL_STR_MASK | ((uintptr_t)(v) & VAL_PAYLOAD); return ret;}
static inline val_t val_fromvec(void *v)             {val_t ret; ret.v = VAL_VEC_MASK | ((uintptr_t)(v) & VAL_PAYLOAD); return ret;}
static inline val_t val_frombuf(void *v)             {val_t ret; ret.v = VAL_BUF_MASK | ((uintptr_t)(v) & VAL_PAYLOAD); return ret;}

static inline val_t val_fromdouble(double v)         {val_t ret; memcpy(&ret,&v,sizeof(val_t)); return ret;}
static inline val_t val_fromfloat(float f)           {return val_fromdouble((double)f);}

static inline val_t val_fromval(val_t v)             {return v;}

#define val_(x) _Generic((x), int: val_fromint,    \
                             char: val_fromchar,   \
                            short: val_fromshort,  \
                             long: val_fromlong,   \
                            _Bool: val_frombool,   \
                     unsigned int: val_fromuint,   \
                    unsigned char: val_fromuchar,  \
                   unsigned short: val_fromushort, \
                    unsigned long: val_fromulong,  \
                           double: val_fromdouble, \
                            float: val_fromfloat,  \
                            val_t: val_fromval,    \
                            vec_t: val_fromvec,    \
                            buf_t: val_frombuf,    \
                  unsigned char *: val_fromstr,    \
                           char *: val_fromstr,    \
                           void *: val_fromptr,    \
                          default: val_fromptr) (x)

#define val(x) _Generic((x), val_t: x, default: val_(x))

#define valeq(x,y)      valeq_(val(x),val(y))

static inline int valeq_(val_t x, val_t y)
{
  if (x.v == y.v) return 1;
  if (valisinteger(x))
    return (x.v & ((uint64_t)0xFFFEFFFFFFFFFFFF)) == (y.v & ((uint64_t)0xFFFEFFFFFFFFFFFF));
  return 0;
}

#define valpayload(x) (val(x).v & VAL_PAYLOAD)

// Retrieve a value from a val_t variable
#define valtodouble(v) val_todouble(val(v))
static inline   double val_todouble(val_t v)  { double d; memcpy(&d,&v,8); return d;}

#define valtofloat(v) val_tofloat(val(v))
static inline  float  val_tofloat(val_t v)    { return (float)val_todouble(v);}

#define valtobool(v) val_tobool(val(v))
static inline  _Bool val_tobool(val_t v)      { return (_Bool)((v.v)&1);}

#define VAL_SIGNED_MASK ((uint64_t)0x0001800000000000)
#define valtointeger(x) val_tointeger(val(x))
static inline      int64_t val_tointeger(val_t v) \
                                  { int64_t ret = ((v.v) & VAL_PAYLOAD); 
                                    if ( ((v.v) & VAL_SIGNED_MASK) == VAL_SIGNED_MASK) // if signed and negative
                                       ret |= (uint64_t)0xFFFF000000000000;            // then extend sign
                                    return ret;
                                  }

static inline   void *val_topointer(val_t v, uint64_t mask) { return (void *)((uintptr_t)((v.v) & mask));}

#define val_to_pointer(v_, m_) ((void *)((uintptr_t)((val(v_).v) & (m_))))

#define valtopointer(v) val_to_pointer(v,VAL_PAYLOAD)
#define valtocleanpointer(v) ((void *)val_to_pointer(v,(uint64_t)0x0000FFFFFFFFFFFE))

#define valtovec(v)    ((vec_t)valtocleanpointer(v))
#define valtobuf(v)    ((buf_t)valtocleanpointer(v))

char *valtostring(val_t v);

// Some constant
#define valfalse      ((val_t){0xFFF8FFFFFFFFFF00})
#define valtrue       ((val_t){0xFFF8FFFFFFFFFF01})
#define valnil        ((val_t){0xFFF80FFFFFFFFFE0})

#define valundefined  ((val_t){0xFFF80FFFFFFFFFD0})
#define valerror      ((val_t){0xFFF80FFFFFFFFFD1})
#define valdeleted    ((val_t){0xFFF80FFFFFFFFFC0})
#define valempty      ((val_t){0xFFF80FFFFFFFFFB0})
#define valmarked     ((val_t){0xFFF80FFFFFFFFFE1})
#define valnewvec     ((val_t){0xFFF80AAAAAAAAA01})
#define valnewbuf     ((val_t){0xFFF80AAAAAAAAA02})
#define valown        ((val_t){0xFFF80AAAAAAAAAA1})
#define valdisown     ((val_t){0xFFF80AAAAAAAAAA2})
#define valnilpointer ((val_t){0xFFFD000000000000})


#define val_first(x,...) x
#define val_rest(x,...) __VA_ARGS__

#define valconst(...) valconst_(val_first(__VA_ARGS__),val_rest(__VA_ARGS__) +0)
#define valconst_(x,y)  ((val_t){((0xFFF9000000000000 + (y) * 0x1000000000000)) | ((uint64_t)((uintptr_t)(x)) & VAL_PAYLOAD)})

#define valisconst(...) valisconst_(val_first(__VA_ARGS__),val_rest(__VA_ARGS__) +0)
#define valisconst_(x,y) ((val(x).v & (uint64_t)0xFFFF000000000000) == ((uint64_t)0xFFF9000000000000 + (y) * 0x1000000000000))

//#define valconst(x)   ((val_t){0xFFF9000000000000 | ((uint64_t)((uintptr_t)(x)) & VAL_PAYLOAD)})
//#define valisconst(x) ((val(x).v & (uint64_t)0xFFFF000000000000) == ((uint64_t)0xFFF8000000000000))

extern char *valemptystr;

#define valnilstr   val_nilstr()
static inline val_t val_nilstr() {return val_fromstr(valemptystr);}

extern char *VALDOUBLE_fmt  ;
extern char *VALSIGNED_fmt  ;
extern char *VALUNSIGNED_fmt;
extern char *VALSTRING_fmt  ;
extern char *VALCONST_fmt   ;
#define valfmt(t,s) (VAL##t##_fmt = s)

void valprintf(val_t v, FILE *f);

int  valtype(val_t v);
int  valcmp(val_t a, val_t b);

#define valreturnif(x,r,e) if (!(x)) errno=0; else return (errno = (e), r)

#define valaux(...) VAL_vrg(valaux_,__VA_ARGS__)
#define valaux_1(v)   val_getaux(v)
#define valaux_2(v,n) val_setaux(v,n)

uint32_t val_getaux(val_t v);
uint32_t val_setaux(val_t v, uint32_t n);

#define valhash(x) valhash_(val(x))
uint32_t valhash_(val_t v);

#endif
#line 1 "buf_.h"
//  SPDX-FileCopyrightText: © 2023 Remo Dentato <rdentato@gmail.com>
//  SPDX-License-Identifier: MIT

#ifndef BUF_VERSION
#define BUF_VERSION 0x0001000B

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stddef.h>
#include <inttypes.h>
#include <assert.h>
#include <errno.h>

#ifndef VAL_VERSION
#include "val_.h"
#endif

#define BUF_cnt(x1,x2,x3,x4,xN, ...) xN
#define BUF_n(...)       BUF_cnt(__VA_ARGS__, 4, 3, 2, 1, 0)
#define BUF_join(x ,y)   x ## y
#define BUF_cat(x, y)    BUF_join(x, y)
#define BUF_vrg(f, ...)  BUF_cat(f, BUF_n(__VA_ARGS__))(__VA_ARGS__)
#define BUF_VRG(f, ...)  BUF_cat(f, BUF_n(__VA_ARGS__))(__VA_ARGS__)

typedef struct buf_s {
  char    *buf;
  uint32_t sze;
  uint32_t end;
  uint32_t pos;
  uint32_t aux;
} *buf_t;

extern int64_t bufallocatedmem;

#define BUFNONDX   0xFFFFFFFF
#define BUFLOADALL 0xFFFFFFF1
#define BUFLOADLN  0xFFFFFFF2
#define BUFMAXNDX  0xFFFFFFF0

#define bufnew(...) bufnew_(__VA_ARGS__ +0)
val_t bufnew_(uint32_t sze);
val_t buffree(val_t bb);

uint32_t bufsize(val_t bb, uint32_t sze);

#define bufpos(...) BUF_vrg(bufpos_,__VA_ARGS__)
uint32_t bufpos_1(val_t bb);
uint32_t bufpos_2(val_t bb, int32_t pos);

#define buflen(...) BUF_vrg(buflen_,__VA_ARGS__)
#define buflen_1(bb) buflen_2(bb,BUFMAXNDX)
uint32_t buflen_2(val_t bb, uint32_t len);

#define bufsize(...) BUF_vrg(bufsize_,__VA_ARGS__)
#define bufsize_1(bb) bufsize_2(bb,0)
uint32_t bufsize_2(val_t bb, uint32_t sze);

uint32_t bufprintf(val_t bb, const char *fmt, ...);

#define bufputs(...) BUF_vrg(bufputs_,__VA_ARGS__)
#define bufputs_2(bb,s) bufputs_3(bb,s,0)
uint32_t bufputs_3(val_t bb, const char *src, uint32_t len);

#define bufload(...) BUF_vrg(bufload_,__VA_ARGS__)
#define bufload_1(bb) bufload_3(bb,0,stdin)
#define bufload_2(bb, f) bufload_3(bb,0,f)
uint32_t bufload_3(val_t bb, uint32_t n, FILE *f);

uint32_t bufloadln(val_t bb, FILE *f);

#define bufsave(...) BUF_vrg(bufsave_,__VA_ARGS__)
#define bufsave_1(bb) bufsave_3(bb,0,stdout)
#define bufsave_2(bb,f) bufsave_3(bb,0,f)
uint32_t bufsave_3(val_t bb, uint32_t n, FILE *f);

uint32_t bufsaveln(val_t bb, FILE *f);

#define buf(...) BUF_vrg(buf_,__VA_ARGS__)
#define buf_1(b) buf_2(b,BUFMAXNDX)
char *buf_2(val_t bb, uint32_t start);

#define bufdel(...) BUF_vrg(bufdel_,__VA_ARGS__)
#define bufdel_1(bb) bufdel_2(bb,BUFMAXNDX)
uint32_t bufdel_2(val_t bb, uint32_t len);

uint32_t bufins_n(val_t bb,uint32_t len);
uint32_t bufins_s(val_t bb,char *str);

#define bufins(b,x) _Generic((x),          char *: bufins_s, \
                                  unsigned char *: bufins_s, \
                                           void *: bufins_s, \
                                              int: bufins_n, \
                                     unsigned int: bufins_n, \
                                            short: bufins_n, \
                                   unsigned short: bufins_n, \
                                             long: bufins_n, \
                                    unsigned long: bufins_n  \
                            ) (b,x)

#define bufsearch(...) BUF_vrg(bufsearch_,__VA_ARGS__)
#define bufsearch_2(b,s) bufsearch_3(b,s,BUFMAXNDX)
uint32_t bufsearch_3(val_t buf, char *str, uint32_t start);

#define BUF_STORES_NUM 4

extern val_t buf_stores[BUF_STORES_NUM];
extern uint8_t buf_stores_cnt[BUF_STORES_NUM];

#define bufstore(...) BUF_vrg(bufstore_,__VA_ARGS__)
#define bufstore_1(v) bufstore_2(0,v)
val_t bufstore_2(int sto, char *s);

#define bufclearstore(...) bufclearstore_(__VA_ARGS__ + 0)
void bufclearstore_(int sto);

#define bufclearstores() for (int sto=0; sto<BUF_STORES_NUM; sto++) bufclearstore_(sto)

#endif
#line 1 "vec_.h"
//  SPDX-FileCopyrightText: © 2023 Remo Dentato <rdentato@gmail.com>
//  SPDX-License-Identifier: MIT
//  PackageVersion: 0.4.0 Beta

#ifndef VEC_VERSION
#define VEC_VERSION 0x0004000B

/* ## Index
 *  ### Managing vectors
 *    - vecnew()
 *    - vecfree()
 *    - vecclear()
 *    - vecsize()
 *    - veccount()
 *    - vec()      Returns the array of values
 * 
 *  ### Array style
 *    - vecset(vec, ndx, val)
 *    - vecget(vec, ndx)
 *    - vecins(vec,ndx,val)
 *    - vecdel(vec,from,to)
 * 
 *  ### Stack style
 *    - vecpush(vec, val)
 *    - vectop(vec)
 *    - vecdrop(vec)
 *
 *  ### Queue style
 *    - vecenq(vec, val)
 *    - vecdeq(vec)
 *    - vecdrop(vec)
 *    - vecfirst(vec)
 *    - veclast(vec)
 * 
 *  ### List style
 *    - vechead
 *    - vectail
 *    - veccur
 *    - vecnext
 *    - vecprev
 *    - vecinsnext(vec,val)
 *    - vecinsprev(vec,val)
 *    - vecadd()
*/

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stddef.h>
#include <inttypes.h>
#include <assert.h>

#ifndef VAL_VERSION
#include "val_.h"
#endif

#define VEC_ISOWNED ((uint8_t)0x80)
#define VEC_SORTED  ((uint8_t)0x10)
#define VEC_ISVEC   ((uint8_t)0x00)
#define VEC_ISARRAY ((uint8_t)0x01)
#define VEC_ISQUEUE ((uint8_t)0x02)
#define VEC_ISSTACK ((uint8_t)0x03)
#define VEC_ISLIST  ((uint8_t)0x04)
#define VEC_ISMAP   ((uint8_t)0x05)
#define VEC_ISNOT   ((uint8_t)0x0F)
#define VEC_NOTYPE  ((uint8_t)0xE0)

#define VECTOPNDX    0xFFFFFFFE
#define VECHEADNDX   0xFFFFFFFD
#define VECTAILNDX   0xFFFFFFFC
#define VECFIRSTNDX  0xFFFFFFFB
#define VECLASTNDX   0xFFFFFFFA
#define VECCURNDX    0xFFFFFFF9
#define VECNEXTNDX   0xFFFFFFF8
#define VECPREVNDX   0xFFFFFFF7
#define VECCOUNTNDX  0xFFFFFFF4
#define VECSIZENDX   0xFFFFFFF3
#define VECNONDX     0xFFFFFFF2
#define VECERRORNDX  0xFFFFFFF1
#define VECMAXNDX    0xFFFFFFF0

#define VEC_cnt(x1,x2,x3,x4,xN, ...) xN
#define VEC_n(...)       VEC_cnt(__VA_ARGS__, 4, 3, 2, 1, 0)
#define VEC_join(x, y)   x ## y
#define VEC_cat(x, y)    VEC_join(x, y)
#define VEC_vrg(f, ...)  VEC_cat(f, VEC_n(__VA_ARGS__))(__VA_ARGS__)
#define VEC_VRG(f, ...)  VEC_cat(f, VEC_n(__VA_ARGS__))(__VA_ARGS__)

/**
 * @struct vec_s
 *
 * @brief A structure representing a dynamic vector.
 *
 * `vec_s` is designed to represent a dynamic array, providing functionalities
 * to manage a collection of elements with dynamic resizing, while keeping 
 * track of various relevant indices and state-related flags.
 *
 * @typedef vec_t
 * A pointer to a `vec_s` structure, used in API functions to manipulate vectors.
 *
 * @param vec Pointer to an array of `val_t` elements, storing the actual content of the vector.
 *        The array is dynamically allocated and resized as needed.
 *
 * @param sze A `uint32_t` value representing the total capacity of the vector, 
 *        i.e., the maximum number of `val_t` elements that `vec` can currently hold.
 *
 * @param end A `uint32_t` value representing the current end of the vector, 
 *        i.e., the index of the element next to the last one in the vector.
 *
 * @param fst A `uint32_t` value used as an index or pointer to the first element in `vec`.
 *            It is used internally in various data structures (e.e. queues).
 *
 * @param aux A `uint32_t` value used by the valaux() function.
 *
 * @param cur A `uint32_t` value used as an index or pointer to the current element in `vec`.
 *        This may be used to keep track of the element being accessed in the most recent operation.
 *
 * @param flg A `uint16_t` value used to store flags that provide additional information about the state of the vector,
 *        such as whether it is sorted, reversed, or has other special properties.
 *
 * @param typ A `uint8_t` value used to store the type of data structure contained in the vector, 
 *        which may be utilized to validate or identify the type of data structures being used or manipulated.
 *
 * @param opt A `uint8_t` value to be used freely
 *
 * Example usage:
 * @code
 *     val_t my_vector = vecnew(); // Utilizing the vecnew function as described previously
 *     // Perform operations using my_vector...
 * @endcode
 *
 */
typedef struct vec_s {
        val_t *vec;
      uint32_t sze;
      uint32_t end;  // Pointer to the element past the last one
      uint32_t fst;  // Pointer to the first element
      uint32_t cur;  // Pointer to the current element
      uint32_t aux;  // auxiliary value
      uint16_t flg;  // Flags
       uint8_t typ;  // Type of structure
       uint8_t opt;  
} *vec_t;

/**
 * @fn val_t vecnew()
 * 
 * @brief Create and initialize a new vector.
 *
 * The `vecnew` function is intended to create a new object of type `val_t` which represents a vector.
 * It initializes any necessary properties of the vector, such as its size, 
 * capacity, and any internal data structures used for storing elements of type `struct vec_s`.
 *
 * @return A `val_t` value holding the `vec_t` value for a new vector, initialized and ready for use.
 *         If the vector creation fails (for instance, due to memory allocation issues), 
 *         the function should return valnil and errno is set to ENOMEM.
 *
 * Example usage:
 * @code
 *     val_t my_vector = vecnew();
 *     if (!valisnil(my_vector)) {
 *         // Handle error (e.g., due to failed memory allocation)
 *     }
 * @endcode
 *
 * Note: Users should check the return value to ensure that the vector was 
 *       created successfully before attempting to use it. This might involve 
 *       checking that the returned value is not valnil and errno is not ENOMEM.
 */
val_t vecnew();

/**
 * @fn val_t vecfree(val_t v)
 * 
 * @brief Safely deallocates the memory occupied by a vector and its internal array.
 *
 * The `vecfree` function is developed to securely release the memory 
 * allocated for a vector `v` of type `val_t`, as well as the memory 
 * for its internal array (`v->vec`). This helps to prevent memory leaks 
 * and ensures the graceful management of vector memory throughout the application.
 *
 * @param v A vector of type `val_t` to be deallocated. 
 *
 * @return Always returns vecnil, facilitating the safe nullification of the 
 *         deallocated vector, e.g., `my_vector = vecfree(my_vector);`.
 *
 * Example usage:
 * @code
 *     val_t my_vector = vecnew();
 *     // Perform operations using my_vector...
 *     my_vector = vecfree(my_vector); // Free memory and update pointer
 * @endcode
 */
val_t vecfree(val_t vv);

/**
 * @fn val_t *vec(val_t v)
 * 
 * @brief Returns a pointer to the array of values stored in the given vector.
 * 
 * This function allows for direct access to the underlying array of the vector,
 * enabling operations that might require bypassing the vector's API.
 *
 * @param v The vector from which the underlying array pointer will be returned.
 *
 * @return A pointer to the array of values in the vector.
 *
 * @warning Directly manipulating the underlying array can cause undefined behavior
 *          if the vector's metadata (e.g., size, count, etc.) is not updated accordingly.
 */
val_t *vec(val_t v);

/**
 * 
 * @fn uint32_t veccount(val_t v [, uint32_t n])
 * 
 * @brief Retrieve the number of elements currently stored in a vector.
 *
 * The `veccount` function provides a mechanism to determine the number
 * of elements currently stored in the provided vector `v` of type `val_t`. 
 * This count is distinct from the total capacity of the vector and 
 * represents the number of actual elements the user has added to the vector.
 *
 * @param v A vector of type `val_t` whose count of elements is to be retrieved. 
 *
 * @return A `uint32_t` value representing the number of elements currently stored in the vector.
 *         The function will return the value stored in `v->end`. If `v` is NULL or not a 
 *         properly initialized vector, the function returns 0 and sets `errno` to `EINVAL`.
 *
 * Example usage:
 * @code
 *     val_t my_vector = vecnew();
 *     // Add some elements to my_vector... 
 *     uint32_t count = veccount(my_vector); // Retrieve the count of elements in my_vector
 * @endcode
 *
 */
#define veccount(...) VEC_vrg(veccount_,__VA_ARGS__)
#define veccount_1(v) veccount_2(v,VECNONDX)
uint32_t veccount_2(val_t v, uint32_t n);

/**
 * @fn uint32_t vecsize(val_t [, uint32_t n])
 * 
 * @brief Gets or sets the capacity of the vector.
 *
 * The `vecsize` function can be used to either retrieve the current capacity of
 * the vector (i.e., the maximum number of elements it can hold without needing 
 * to resize) or to set a new capacity for the vector.
 *
 * If the `n` parameter is specified, the function adjusts the vector's capacity 
 * to ensure that it can hold at least `n` elements. Otherwise, it simply returns 
 * the current capacity.
 *
 * @param v The vector of type `val_t` whose capacity is to be retrieved or set.
 *
 * @param n (Optional) The desired capacity for the vector. If specified, the 
 *          vector's capacity will be adjusted to at least this size.
 *
 * @return The current or updated capacity of the vector. This represents the 
 *         maximum number of elements the vector can hold without needing a resize.
 *
 * Note: 
 * - `vecsize` can not shrink the vector. If you need to size down the vector,
 *    you should create a new one,  copy the elements in the new one and
 *    free up the current vector.
 *
 * Example usage:
 * @code
 *     val_t my_vector = vecnew();
 *     uint32_t current_capacity = vecsize(my_vector, 0);
 *     printf("Current capacity: %u\n", current_capacity);
 *     
 *     uint32_t new_capacity = vecsize(my_vector, 100);
 *     printf("New capacity: %u\n", new_capacity);
 * @endcode
 */
#define vecsize(...) VEC_vrg(vecsize_,__VA_ARGS__)
#define vecsize_1(v) vecsize_2(v,VECNONDX)
uint32_t vecsize_2(val_t vv, uint32_t n);

#define vectype(...) VEC_vrg(vectype_,__VA_ARGS__)
#define vectype_1(v) vectype_2(v,-1)
int vectype_2(val_t vv,int type);

/**
 * @fn int vecisqueue(val_t v)
 * 
 * @brief Checks if the given vector {@code v} represents a queue.
 * @param v The vector of type {@code val_t} to be checked for queue properties.
 * @return Returns 1 if it is a queue, 0 otherwise.
 */
#define vecisqueue(v) (vectype(v) == VEC_ISQUEUE)

/**
 * @fn int vecisstack(val_t v)
 * 
 * @brief Checks if the given vector {@code v} represents a stack.
 * @param v The vector of type {@code val_t} to be checked for stack properties.
 * @return Returns 1 if it is a stack, 0 otherwise.
 */
#define vecisstack(v) (vectype(v) == VEC_ISSTACK)

/**
 *  @fn int vecisempty(val_t v)
 * 
 *  @brief Checks if the vector is empty.
 *
 * The `vecisempty` function evaluates the state of the provided `val_t` data 
 * structure to determine if it contains any elements or not.
 *
 * @param v The vector of type `val_t` whose state is to be checked.
 *
 * @return Returns a non-zero number if the vector is empty.
 *         Returns 0 if the vector contains at least one element. 
 *
 * Example usage:
 * @code
 *     val_t my_vector = vecnew();
 *     // ... (some operations on my_vector)
 *     if (vecisempty(my_vector)) {
 *         printf("The vector is empty.\n");
 *     } else {
 *         printf("The vector has elements.\n");
 *     }
 * @endcode
 */
#define vecisempty(v) (veccount(v) == 0)

#define vecindex(...) VEC_vrg(vecindex_,__VA_ARGS__)
#define vecindex_1(v)   vecindex_3(v,0,0)
#define vecindex_2(v,n) vecindex_3(v,n,0)
uint32_t vecindex_3(val_t vv, uint32_t ndx, int32_t delta);


/**
 * @fn val_t vecset(val_t v ,uint32_t i, val_t x)
 * 
 * @brief Set a value at the specified index in the vector, possibly adjusting its size.
 *
 * The function `vecset` is designed to set the value `x` of type `val_t` at the 
 * specified index `i` within the vector `v` of type `val_t`. 
 * The function ensures that there is enough room in the vector to 
 * perform the operation (or fails if there's not enough room).
 *
 * @param v The vector in which the value will be set. 
 *
 * @param i The index at which the value will be set. Iit must be a non-negative 
 *          integer.
 *
 * @param x The value to be set at index `i` in vector `v`. It can be a base
 *          type (e.g. an integer) or a val_t type.
 *
 * @return The value that has been set or `valerror` in case of failure.
 *
 * @note The function `makeroom` is called to ensure that there is enough room 
 *       in `v` to set the value `x` at index `i`. 
 *
 * Example usage:
 * @code
 *     val_t my_vector = vecnew() // some initialized vector
 *     uint32_t index = 3;
 *     val_t new_value = val(5.4) // value to set
 *     uint32_t set_index = vecset(my_vector, index, new_value);
 *     uint32_t new_index = vecset(my_vector, index+1, 42);
 * @endcode
 */
#define  vecset(v,i,x)     vecset_(v,i,val(x))
val_t vecset_(val_t v, uint32_t i, val_t x);

/**
 * @fn val_t vecadd(val_t v, val_t x)
 * 
 * @brief Append an element to the end of the vector.
 *
 * The `vecadd` function is employed to insert a new element, represented by `x`, 
 * at the end of the vector `v` of type `val_t`.
 *
 * @param v A vector of type `val_t` to which the element is to be appended.
 *          If `v` is not a valid and initialized vector, 
 * 
 * @param x The element of type `val_t` to be appended to the vector.
 *
 * @return A `uint32_t` value representing the index at which the element has been stored,
 *         or `VECNONDX` in case of an error.
 *
 * @note To remove the last appended elements, you can use the `vecdrop()` function
 * 
 * Example usage:
 * @code
 *     val_t my_vector = vecnew();
 *     val_t my_value = ...; // Some value of type val_t
 *     uint32_t new_count = vecadd(my_vector, my_value); // Append my_value to my_vector
 * @endcode
 */
#define vecadd(v,x)    vecset_(v,VECNONDX,val(x))

/**
 * @fn val_t vecget(val_t v , val_t ii)
 * 
 * @brief Retrieve an element from the vector at the specified index.
 *
 * This function retrieves the value at the specified index `i` 
 * from the vector `v`. If `i` is out of bounds, the function 
 * returns `valnil` and sets `errno`. Users should  ensure that `i`
 * is within the valid range of indices for the vector.
 *
 * @param v The vector from which to retrieve the value.
 *
 * @param i The index of the element to retrieve. It is of type `uint32_t`,
 *          ensuring that only non-negative integer values are valid.
 *          If it is unspecified (`vecget(v)`),  the last element of the
 *          vector will be returned.
 * 
 * @param delta A displacement (+/-) with respect to the desired index. If the
 *          resulting index is out of bounds, `vecget()` will return `valnil`
 *
 * @return The value of type `val_t` stored at index `i` in the vector `v`,
 *         or `valnil` upon error.
 *
 * Example usage:
 * @code
 *     val_t my_vector = ... // some initialized vector
 *     uint32_t index = 3;
 *     val_t value = vecget(my_vector, index);
 * @endcode * @
 *
 */
#define vecget(...) VEC_vrg(vecget_,__VA_ARGS__)
#define vecget_1(v) vecget_(v,vecindex(VECCURNDX,0),NULL)
#define vecget_2(v,i) vecget_(v,val(i),NULL)
#define vecget_3(v,i,k) vecget_(v,val(i),k)
val_t vecget_(val_t v, val_t ii, val_t *key);

val_t vecgetfirst(val_t v);
val_t vecgetnext(val_t v);
val_t vecgetmin(val_t v);
val_t vecgetmax(val_t v);

/**
 * @fn val_t vecdel(val_t v, uint32_t i [, uint32_t j] )
 * 
 * @brief Delete a range of elements from a vector, shrinking its size.
 *
 * The `vecdel` function is intended to remove elements from the vector `v` 
 * starting at index `i` and ending at index `j`. If the parameter `j` is not 
 * provided, it defaults to `i`, meaning only the element at index `i` will be removed.
 * 
 * After deletion, the vector is effectively shrunk, and any elements after the deleted 
 * range will be shifted to fill the gap. This ensures that the vector remains contiguous.
 *
 * @param v A vector of type `val_t` from which the elements are to be deleted.
 * 
 * @param i The start index from which deletion begins. Ensure that `i` is within 
 *          the bounds of the vector's current count.
 * 
 * @param j The end index where deletion stops. If not specified, defaults to `i`.
 *          Ensure that `j` is within the bounds of the vector's current count and 
 *          that `j >= i`.
 *
 * @return Returns the number of remaining elements in the vector or `VECERRORNDX`
 *         upon error.
 *
 * Example usage:
 * @code
 *     val_t my_vector = vecnew();
 *     // ... (add some elements to my_vector)
 *     val_t last_deleted = vecdel(my_vector, 2, 4); // Delete elements from index 2 to 4
 *     val_t single_deleted = vecdel(my_vector, 3);  // Delete only the element at index 3
 * @endcode
 */
#define vecdel(...) VEC_vrg(vecdel_,__VA_ARGS__)
#define vecdel_2(v,i) vecdel_3_(v,val(i),VECNONDX)
#define vecdel_3(v,i,j) vecdel_3_(v,val(i),j)
uint32_t vecdel_3_(val_t v, val_t ii, uint32_t j);

#define vecmakegap(...)   VEC_vrg(vec_gap_,__VA_ARGS__)
#define vec_gap_1(v)      vec_gap_3(v, VECNONDX, VECNONDX)
#define vec_gap_2(v,l)    vec_gap_3(v,VECNONDX, l)
int vec_gap_3(val_t v, uint32_t i, uint32_t l);

#define vecins(v,i,x)    vecins_(v,i,val(x))
val_t vecins_(val_t vv, uint32_t i, val_t x);

/**
 * @fn val_t vecpush(val_t v, val_t x)
 * 
 * @brief Push a value onto the stack.
 *
 * The `vecpush` function is designed to append a value to the end of the 
 * `val_t` data structure, effectively treating it as the top of a stack.
 *
 * @param v The stack of type `val_t` onto which the value is to be pushed.
 
 * @param x The value of type `val_t` to be pushed onto the stack.
 *
 * @return The index at which the value was set or `VECNONDX` in case of failure.
 *
 * Example usage:
 * @code
 *     val_t my_stack = vecnew();
 *     vecpush(my_stack, 4.3);      // Push a value onto my_stack
 *     vecpush(my_stack, "Total");  // Push another value onto my_stack
 * @endcode
 */
#define vecpush(v,x)    vecpush_(v,val(x),0)
#define vecpushown(v,x) vecpush_(v,val(x),1)
val_t vecpush_(val_t vv, val_t x, int own);

/**
 * @fn val_t vectop(val_t v, int32_t delta)
 * 
 * @brief Retrieve the top value of the stack without removing it.
 *
 * The `vectop` function provides insight into the topmost value of the stack 
 * without actually modifying the stack.
 *
 * @param v The stack of type `val_t` whose top value is to be retrieved.
 *
 * @return The topmost value of type `val_t` in the stack. If the stack is 
 *         empty, the function returns `valnil`.
 *
 * Example usage:
 * @code
 *     val_t my_stack = vecnew();
 *     // ... (push some elements onto my_stack)
 *     val_t topValue = vectop(my_stack);  // Get the top value without popping it
 * @endcode
 */
#define vectop(...)  VEC_vrg(vectop_,__VA_ARGS__)
#define vectop_1(v)   vectop_2(v,0)
static inline val_t vectop_2(val_t v, uint32_t d) {
  return vecget_2(v,vecindex_3(v,VECTOPNDX,d));
}

/**
 * @fn val_t vecdrop(val_t v [, uint32_t n])
 * 
 * @brief Drop the last `n` elements from a vector-like data structure.
 *
 * The `vecdrop` function is developed to remove the last `n` elements from the 
 * data structure `v` of type `val_t`. This operation essentially reduces the size 
 * of the data structure by `n` elements.
 * 
 * This function is versatile and suitable for different data structures such as 
 * vectors, stacks, and queues, given that they all can be represented with the 
 * type `val_t`.
 *
 * @param v A data structure of type `val_t` from which the elements are to be dropped.
 * 
 * @param n The number of elements to be dropped from the end. If `n` exceeds the 
 *          current count of elements in the data structure, the entire structure 
 *          should be emptied or an appropriate error handling mechanism should be 
 *          in place. If `n` is not specified, it defaults to 1.
 *
 * @return Returns the number of remaining elements in the vector or `VECERRORNDX`
 *         upon error.
 *
 * Example usage:
 * @code
 *     val_t my_vector = vecnew();
 *     // ... (add some elements to my_vector)
 *     val_t last_dropped = vecdrop(my_vector, 3);  // Drop the last 3 elements
 * @endcode
 */
#define vecdrop(...) VEC_vrg(vecdrop_,__VA_ARGS__)
#define vecdrop_1(v) vecdrop_2(v,1)
uint32_t vecdrop_2(val_t v, uint32_t n);

#define vecpop(...) vecdrop(__VA_ARGS__)

/**
 * @fn val_t vecenq(val_t v, val_t x);
 * @brief Enqueue a value onto the queue.
 *
 * The `vecenq` function inserts a value at the end of the vector, 
 * effectively treating it as a queue.
 *
 * @param v The queue of type `val_t` onto which the value is to be enqueued.
 *
 * @param x The value of type `val_t` to be enqueued.
 *
 * @return The value that has been enqueued
 *
 * Example usage:
 * @code
 *     val_t my_queue = vecnew();
 *     vecenq(my_queue, someValue);  // Enqueue someValue onto my_queue
 * @endcode
 */
#define vecenq(v,x) vecenq_(v,val(x),0)
#define vecenqown(v,x) vecenq_(v,val(x),1)
val_t vecenq_(val_t v, val_t x, int own);

/**
 * @fn uint32_t vecdeq(val_t v [, uint32_t n])
 * 
 * @brief Dequeue multiple values from the front of the queue.
 *
 * The `vecdeq` function removes the next `n` values from the front of the 
 * `val_t` data structure, treating it as a queue. After dequeuing the specified 
 * number of values, it returns the number of remaining elements in the queue.
 *
 * @param v The queue of type `val_t` from which the values are to be dequeued.
 *
 * @param n The number of values to dequeue from the front of the queue. If `n` 
 *          is not specified, it defaults to 1.
 *
 * @return The number of remaining elemnets in the queue. If an error occurs, 
 *         returns `0` and sets `errno`. 
 *
 * @note You can eliminate the last inserted elements in the queue using
 *       the `vecdrop()` function.
 * 
 * Example usage:
 * @code
 *     val_t my_queue = vecnew();
 *     // ... (enqueue some elements onto my_queue)
 *     vecenq(my_queue, 100);
 *     vecenq(my_queue, 200);
 *     vecenq(my_queue, 300);
 *     val_t x = vecfirst(my_queue);  // x will be 100;
 *     vecdeq(my_queue);              // Dequeue the first 2 values from my_queue
 *     x = vecfirst(my_queue)         // x will be 300 
 * @endcode
 */
#define vecdeq(...) VEC_vrg(vecdeq_,__VA_ARGS__)
#define vecdeq_1(v) vecdeq_2(v,1)
uint32_t vecdeq_2(val_t v, uint32_t n);

// #define vechead(vv) vecget(vv,VECHEADNDX)
// #define vectail(vv) vecget(vv,VECTAILNDX)

/**
 * @fn val_t veclast(val_t v,int32_t delta)
 * 
 * @brief Retrieve the last inserted value in the queue without removing it.
 *
 * The `vechead` function gives insight into the first value of the queue 
 * without actually modifying the queue.
 *
 * @param v The queue of type `val_t` whose first value is to be retrieved.
 *          Ensure that `v` is a valid and initialized queue.
 *
 * @param delta A displacement to peek the values in the queue.
 *          If the resulting index is out of bounds, it will return `valnil`
 *
 * @return The last inserted value of type `val_t` in the queue. If the queue is 
 *         empty, the function returns `valnil`.
 *
 * Example usage:
 * @code
 *     val_t my_queue = vecnew();
 *     // ... (enqueue some elements onto my_queue)
 *     val_t firstValue = veclast(my_queue);  // Get the last value without dequeuing it
 * @endcode
 */
#define veclast(...)  VEC_vrg(veclast_, __VA_ARGS__)
#define veclast_1(v)  veclast_2(v, 0)
static inline val_t   veclast_2(val_t v,uint32_t d) {
  return vecget_2(v,vecindex_3(v,VECLASTNDX,d)); 
}

/**
 * @fn val_t vecfirst(val_t v,int32_t delta)
 * 
 * @brief Retrieve the first value in the queue without removing it.
 *
 * The `vectail` function gives insight into the first value of the queue 
 * without actually modifying the queue.
 *
 * @param v The queue of type `val_t` whose first value is to be retrieved.
 *          Ensure that `v` is a valid and initialized queue.
 *
 * @param d The offset from the first (toward the last) to be retrieved.
 * 
 * @return The first value of type `val_t` in the queue. If the queue is 
 *         empty, or the offset is past the end of the queue, the function
 *         returns `valnil`.
 *
 * Example usage:
 * @code
 *     val_t my_queue = vecnew();
 *     // ... (enqueue some elements onto my_queue)
 *     val_t firstValue = vecfirst(my_queue);  // Get the first inserted value
 * @endcode
 */

val_t vecmapfirst_(val_t vv, val_t *key);

#define vecfirst(...)  VEC_vrg(vecfirst_,__VA_ARGS__)
#define vecfirst_1(v)  vecfirst_2_(v,valnil)
#define vecfirst_2(v,d) vecfirst_2_(v,val(d))
static inline val_t vecfirst_2_(val_t v, val_t dd) {
  if (vectype(v) == VEC_ISMAP) {
    val_t k; val_t *kptr = NULL;
    if (valispointer(dd)) kptr = valtocleanpointer(dd);
    if (kptr == NULL) kptr = &k;
    return vecmapfirst_(v, kptr);
  }
  int32_t d = 0;
  if (valisinteger(dd)) d = valtointeger(dd);
  val_dbg("d: %d",d);
  return vecget_2(v,vecindex_3(v,VECFIRSTNDX,d)); 
}

val_t vecmapnext_(val_t vv, val_t *key);

#define vecnext(...)  VEC_vrg(vecnext_,__VA_ARGS__)
#define vecnext_1(v)   vecnext_2_(v,valnil)
#define vecnext_2(v,d) vecnext_2_(v,val(d))
static inline val_t vecnext_2_(val_t v, val_t dd) {
  if (vectype(v) == VEC_ISMAP) {
    val_t k; val_t *kptr = NULL;
    if (valispointer(dd)) kptr = valtocleanpointer(dd);
    if (kptr == NULL) kptr = &k;
    return vecmapnext_(v, kptr);
  }
  uint32_t d = 0;
  if (valisinteger(dd)) d = valtointeger(dd);
  return vecget_2(v,vecindex_3(v,VECNEXTNDX,d)); 
}

/**
 * @fn val_t vecown(val_t vv, uint32_t ndx, val_t x)
 * 
 * @brief Sets the value of the specified element in the vector 'vv' to the value 'x'.
 *        If 'x' is a vector itself, it is marked as "owned" by 'vv'.
 *        An "owned vector" is automatically freed when the owning vector 'vv' is freed.
 *
 * This function is similar to 'vecset()' but with the added functionality of handling
 * ownership of vectors. If 'x' is a vector, it becomes owned by 'vv', meaning that
 * it will be automatically deallocated when 'vv' is freed or when the elements are 
 * deleted from the vector (e.g. with vecdrop()) or overwritten.
 *
 * @param vv The vector in which the element is to be set. This vector becomes the owner
 *           if 'x' is a vector. Must not be NULL.
 * @param ndx The index of the element in 'vv' to be set. 
 * @param x The value to set at the specified index in 'vv'. If 'x' is a vector,
 *          it becomes owned by 'vv'.
 *
 * @return The x value (possibly modified if it has been owned)
 *
 * @note Owned vectors must be treated with extreme care. The general rule is
 *       that, at any given time, there must be ONE AND ONLY ONE owner. This
 *       meaans, for example, that you can't do something like:
 *                x = vecnew();
 *                x = vecown(v, 3, x);
 *                x = vecset(t, 2, x); // THIS WILL RESULT IN TWO OWNERS!!
 *       The same is valid also if you 
 */

#define vecown(...) VEC_vrg(vecown_,__VA_ARGS__)
#define vecown_2(v,i)   vecown_3_(v,i,valown)
#define vecown_3(v,i,x) vecown_3_(v,i,val(x))
val_t vecown_3_(val_t vv, uint32_t i, val_t x);

val_t vecdisown(val_t vv, uint32_t i);

#define vecdisowntop(...) VEC_vrg(vcedisowntop_,__VA_ARGS__)
#define vecdisowntop_1(vv) vecdisownfirst_2(vv,0)
static inline val_t vecdisowntop_2(val_t vv, int32_t delta) {
  return vecdisown(vv,vecindex_3(vv,VECTOPNDX,delta)); 
}

#define vecdisownfirst(...) VEC_vrg(vcedisownfirst_,__VA_ARGS__)
#define vecdisownfirst_1(vv) vecdisownfirst_2(vv,0)
static inline val_t vecdisownfirst_2(val_t vv, int32_t delta) {
  return vecdisown(vv,vecindex_3(vv,VECFIRSTNDX,delta)); 
}

uint32_t vecsearch(val_t v, val_t x, val_t aux);

typedef  int(*vec_cmp_t)(val_t,val_t);

#define vecsort(...) VEC_vrg(vecsort_,__VA_ARGS__)
#define vecsort_1(v)   vecsort_3(val(v),valcmp,valnil)
#define vecsort_2(v,c) vecsort_3(val(v),c,valnil)
int vecsort_3(val_t v, int(*cmp)(val_t,val_t), val_t aux);

#define vecissorted(v) vecissorted_(val(v))
int vecissorted_(val_t vv);

#define vecsearch(v,k) vecsearch_(val(v),val(k))
uint32_t vecsearch_(val_t vv, val_t key);

extern int64_t vecallocatedmem;

#define vecmap(v,K,V) vecmap_(v,val(K),val(V))
uint32_t vecmap_(val_t vv, val_t key, val_t value);

#define vecunmap(v,K) vecunmap_(v,val(K))
uint32_t vecunmap_(val_t vv, val_t key);

#define vec_root(v) ((v)->fst)

void vec_printtree(vec_t v, FILE *f);
void vecprinttree(val_t vv, FILE *f);

#endif

#line 1 "val.c"
#ifndef VAL_VERSION
#include "val.h"
#endif

#line 1 "buf.c"
//  SPDX-FileCopyrightText: © 2023 Remo Dentato <rdentato@gmail.com>
//  SPDX-License-Identifier: MIT

#ifndef BUF_VERSION
#include "buf_.h"
#endif

#include <stdalign.h>

#ifndef RETURN_IF
#define RETURN_IF valreturnif
#endif

#ifdef _MSC_VER
val_t buf_stores[BUF_STORES_NUM] = {0xFFF80FFFFFFFFFE0, 0xFFF80FFFFFFFFFE0, 0xFFF80FFFFFFFFFE0, 0xFFF80FFFFFFFFFE0};
#else 
val_t buf_stores[BUF_STORES_NUM] = {valnil, valnil, valnil, valnil};
#endif

//val_t buf_stores[BUF_STORES_NUM] = {valnil, valnil, valnil, valnil};
uint8_t buf_stores_cnt[BUF_STORES_NUM] = {0,0,0,0} ;

val_t bufstore_2(int sto, char *s)
{
  val_t v;

  sto &= 3;

  if (!valisbuf(buf_stores[sto])) buf_stores[sto] = bufnew();
  RETURN_IF(!valisbuf(buf_stores[sto]),valnil,EINVAL);

  bufputs(buf_stores[sto],"\0\0\0\0",4);   // Add 4 bytes for the aux

  v.v = VAL_STO_MASK | ((uint64_t)(buf_stores_cnt[sto]) << 40) |  ((uint64_t)sto) << 32 | bufpos(buf_stores[sto]);
  
  bufputs(buf_stores[sto],s);

  uint32_t n = bufpos(buf_stores[sto])+1;
  n = (4 - n % 4) % 4;
  val_dbg("n: %d",n);
  bufputs(buf_stores[sto],"\0\0\0\0",1+n); // Add \0 until next pos will be aligned with 4 bytes.

  return v;
}

void bufclearstore_(int sto)
{
  sto &= 3;
  buf_stores[sto] = buffree(buf_stores[sto]);
  buf_stores_cnt[sto] += 1;
}

// Ensure the buftor is large enough to store a value at index n
static int buf_makeroom(buf_t b, uint32_t n)
{
  uint32_t new_sze;
  char *new_buf;

  errno = 0;
  val_dbg("buf Making room %p (%p)[%d]->[%d]",(void *)b,(void *)(b->buf),b->sze,n);

  RETURN_IF(!b, 0, EINVAL);
  RETURN_IF(n == 0, 1, 0); // success for sure!

  // There should be at least n+1 bytes free:
  n += 1;

  if (n <= b->sze) return 1; // There's enough room

  RETURN_IF(n > BUFMAXNDX,0,ERANGE); // Max number of elments in a buftor reached.
  
  if (n >= 0xD085FAF0) new_sze = n; // an n higher than that would make the new size overflow
  else {
    new_sze = b->sze ? b->sze : 4;
    while (new_sze <= n) { 
      new_sze = (new_sze * 13) / 8; // (new_sze + (new_sze/2) + (new_sze/8));  
    }
  }
  
  new_sze += (new_sze & 1); // ensure is even
  
  RETURN_IF(new_sze <= n,0,ERANGE);

  val_dbg("MKROOM: realloc(%p,%lu) [%u]->[%u]",(b->buf),new_sze ,b->sze, new_sze);
  new_buf = realloc(b->buf, new_sze);
  val_dbg("MKROOM: got(%p,%d) [%d]",new_buf,new_sze ,new_sze);

  RETURN_IF(new_buf == NULL,0,ENOMEM);

  // set the last byte of the buffer to \0;
  new_buf[new_sze-1] = '\0';

  b->buf = new_buf;
  b->sze = new_sze;
  
  return 1;
}

val_t bufnew_(uint32_t sze) {
  errno = 0;
  buf_t b = malloc(sizeof(struct buf_s));
  if (b) {
    b->pos = 0;
    b->end = 0;
    b->sze = sze;
    b->buf = NULL;
    if (!buf_makeroom(b,sze)) {free(b); b = NULL;}
  }
  RETURN_IF(b == NULL, valnil, ENOMEM);
  
  memset(b,0,sizeof(struct buf_s));
  return val(b);
}

val_t buffree(val_t bb)
{
  buf_t b;
  errno = 0;
  RETURN_IF(!valisbuf(bb),bb,EINVAL);
  b = valtocleanpointer(bb); 

  free(b->buf);
  free(b); 
  return valnil;
}

uint32_t bufpos_1(val_t bb)
{
  RETURN_IF(!valisbuf(bb), 0, EINVAL);
  buf_t b = valtocleanpointer(bb);

  return b->pos;    
}

uint32_t bufpos_2(val_t bb, int32_t pos)
{
  RETURN_IF(!valisbuf(bb), 0, EINVAL);
  buf_t b = valtocleanpointer(bb);

  if (pos >= 0) {
    b->pos =  pos >  b->end ? b->end : pos;
  } else {
    b->pos = -pos >= b->end ? 0 : b->end + pos;
  }
  return b->pos;
}

uint32_t bufputs_3(val_t bb, const char *src, uint32_t len)
{

  RETURN_IF(!valisbuf(bb), 0, EINVAL);
  RETURN_IF(src == NULL, 0, 0);

  buf_t b = valtocleanpointer(bb);

  if (len == 0) len = strlen(src);
  
  if (len > 0) {
    RETURN_IF(!buf_makeroom(b,b->pos+len), 0, ENOMEM);

    memmove(b->buf + b->pos, src, len);

    b->pos += len;
    if (b->pos >= b->end) {
      b->end = b->pos;
      b->buf[b->end] = '\0';
    }
  }

  return len;
}

uint32_t bufprintf(val_t bb, const char *fmt, ...)
{
  RETURN_IF(!valisbuf(bb), 0, EINVAL);
  buf_t b = valtocleanpointer(bb);

  va_list args;
  int len;

  // Determine how much space is needed
  va_start(args, fmt);
  len = vsnprintf(NULL, 0, fmt, args);
  va_end(args);

  if (len > 0) {
    RETURN_IF(!buf_makeroom(b,b->pos+len), 0, ENOMEM);
    val_dbg("POS[end]: %d", b->buf[b->pos+len]);
    int oldend = b->buf[b->pos+len];
    va_start(args, fmt);
    len = vsnprintf(b->buf+b->pos,len+1,fmt,args);
    va_end(args);
    b->pos += len;
    b->buf[b->pos] = oldend;
    if (b->pos >= b->end) {
      b->end = b->pos;
      b->buf[b->end] = '\0';
    }
  }

  return len;
}

uint32_t bufsize_2(val_t bb, uint32_t sze)
{
  RETURN_IF(!valisbuf(bb), 0, EINVAL);
  buf_t b = valtocleanpointer(bb);

  if (sze > b->sze) {
    RETURN_IF(!buf_makeroom(b,sze), 0, ENOMEM);
    b->sze = sze;
  }
  return b->sze;
}

char *buf_2(val_t bb, uint32_t start)
{
  RETURN_IF(!valisbuf(bb), NULL, EINVAL);
  buf_t b = valtocleanpointer(bb);

  if (start > b->pos) start = b->pos;

  return b->buf? b->buf+start : NULL;
}

uint32_t buflen_2(val_t bb, uint32_t n)
{
  RETURN_IF(!valisbuf(bb), 0, EINVAL);
  buf_t b = valtocleanpointer(bb);

  if (n < BUFMAXNDX) {
    RETURN_IF(!buf_makeroom(b,n), 0, ENOMEM);
    b->end = n;
    b->pos = b->end;
    if (b->buf) b->buf[b->end] = '\0';
  }

  return b->end;
}

uint32_t bufload_3(val_t bb, uint32_t n, FILE *f)
{
  RETURN_IF(!valisbuf(bb), 0, EINVAL);
  buf_t b = valtocleanpointer(bb);

  if (n == 0) n = UINT32_MAX;
   
  uint32_t blksize = 128;
 
  if (n<blksize) blksize = n;

  uint32_t l = 0;
  uint32_t r;
  while (!feof(f) && l <= n) {
    RETURN_IF(!buf_makeroom(b,b->pos+blksize), 0, ENOMEM);

    r = fread(b->buf+b->pos,1,blksize,f);
    if (r == 0) break;
    l += r;
    b->pos += r;
    if (b->pos > b->end) {
      b->end = b->pos;
      b->buf[b->pos] = '\0';
    }
  }
  return l;
}

uint32_t bufloadln(val_t bb, FILE *f)
{
  int c;
  char last_c = '\0';
  
  RETURN_IF(!f || feof(f), 0, EINVAL);

  RETURN_IF(!valisbuf(bb), 0, EINVAL);
  buf_t b = valtocleanpointer(bb);

  RETURN_IF(!buf_makeroom(b,b->pos+32), 0, ENOMEM);

  uint32_t l = 0;

  while ((c=fgetc(f)) != EOF) {

    RETURN_IF(!buf_makeroom(b, b->pos+32), 0, ENOMEM);

    if (c == '\n') break;

    last_c = b->buf[b->pos];
    b->buf[b->pos] = c;
    b->pos += 1;
    l += 1;
  }
  if (b->pos > 0) {
    if (b->buf[b->pos-1] == '\r') {
      b->buf[b->pos-1] = last_c;
      b->pos -= 1;
      l -= 1;
    }

    if (b->pos >= b->end) {
      b->end = b->pos;
      b->buf[b->pos] = '\0';
    }
  }
  else {
    b->end = 0;
    b->buf[0] = '\0';
  }
  return l;
}

uint32_t bufsave_3(val_t bb, uint32_t n, FILE *f)
{
  RETURN_IF(!valisbuf(bb), 0, EINVAL);
  buf_t b = valtocleanpointer(bb);

  if (n == 0 || (b->pos + n) > b->end) n = b->end - b->pos;
  if (n > 0) {
    n = fwrite(b->buf + b->pos, 1, n, f);
    b->pos += n;
  }

  return n;
}

uint32_t bufdel_2(val_t bb, uint32_t len)
{
  RETURN_IF(!valisbuf(bb), 0, EINVAL);
  buf_t b = valtocleanpointer(bb);
  
  if (b->pos == b->end) return 0;

  if (len == 0) return 0;
  
  if (len >= b->end - b->pos) {
    len = b->end - b->pos;
    b->end = b->pos;
    b->buf[b->end] = '\0';
    return len;
  }

  char *ptr = b->buf + b->pos;
  uint32_t len_to_move = b->end - (b->pos + len);
  memmove(ptr, ptr+len, len_to_move+1);

  b->end -= len;
  b->buf[b->end] = '\0';

  return len;
}

static int makegap(buf_t b, uint32_t l)
{
  uint32_t i = b->pos;

  val_dbg("entering makegap(%d,%d)",i,l);
  RETURN_IF(!buf_makeroom(b, i+l),0,ENOMEM);

  if (i < b->end) {
    /*                     __l__
    **  ABCDEFGH       ABC/-----\DEFGH
    **  |  |    |      |  |      |    |
    **  0  i    len    0  i     i+l   len+l */  
    memmove(b->buf+(i+l), b->buf+i, b->end-i);
  }
  b->pos += l;
  b->end += l;
  b->buf[b->end] = '\0';
  return 1;
}

uint32_t bufins_x(val_t bb, uint32_t len, char *fill)
{
  RETURN_IF(!valisbuf(bb), 0, EINVAL);
  buf_t b = valtocleanpointer(bb);

  uint32_t p = b->pos;

  RETURN_IF(!makegap(b,len), 0, ENOMEM);

  uint32_t k=0;
  while (p < b->pos) {
    b->buf[p++] = fill[k++];
    if (fill[k] == '\0') k = 0;
  }
  return(len);
}

uint32_t bufins_n(val_t bb, uint32_t n)
{
  if (n == 0) return 0;
  else return bufins_x(bb,n,"                                ");
}

uint32_t bufins_s(val_t bb, char *s)
{
  if (s == NULL || *s == '\0') return 0;
  else return bufins_x(bb,strlen(s),s);
}

uint32_t bufsearch_3(val_t bb, char *str, uint32_t start)
{
  RETURN_IF(!valisbuf(bb), BUFNONDX, EINVAL);
  buf_t b = valtocleanpointer(bb);

  if (start == BUFMAXNDX) start = b->pos;
 
  RETURN_IF(start >= b->end, BUFNONDX, 0);

  char *heystack = b->buf+start;
  char *found = strstr(heystack,str);

  RETURN_IF(found == NULL, BUFNONDX, 0);

  b->pos = (uint32_t)(found-heystack);
  return b->pos;
}

#line 1 "val.c"
//  SPDX-FileCopyrightText: © 2023 Remo Dentato <rdentato@gmail.com>
//  SPDX-License-Identifier: MIT

#ifndef VAL_VERSION
#include "val_.h"
#include "vec_.h"
#include "buf_.h"
#endif

char *valemptystr="\0\0\0";

int valisstored_(val_t v)
{
  if ((v.v & VAL_TYPE_MASK) != VAL_STO_MASK) return 0;

  return ((v.v & 0x0000FF0000000000) >> 40) == buf_stores_cnt[(v.v & 0x0000000300000000) >> 32];
}

int valtype(val_t v)
{
#if 0
  if (valisdouble(v))  return VALDOUBLE;
  if (valisbool(v))    return VALBOOL;
  if (valisnil(v))     return VALNIL;
  if (valispointer(v)) return VALPOINTER;
  if (valisstring(v))  return VALSTRING;
  if (valisvec(v))     return VALVEC; 
  if (valisbuf(v))     return VALBUF; 
  if (valisconst(v))   return VALCONST; 
  if (valisstored(v))  return VALSTORED;
#endif
#if 0
  uint64_t t = v.v & VAL_TYPE_MASK;
  switch (t) {
    case VAL_BUF_MASK: return VALBUF;
    case VAL_VEC_MASK: return VALVEC;
    case VAL_STR_MASK: return VALSTRING;
    case VAL_PTR_MASK: return VALPOINTER;
    case VAL_UNS_MASK: return VALINTEGER;
    case VAL_INT_MASK: return VALINTEGER;
    case VAL_CST_MASK: return VALCONST;
    case VAL_STO_MASK: return VALSTORED;
    case ((uint64_t)0xFFF8000000000000): return (t & 0xF0) == 0xE0 ? VALNIL : VALBOOL;
  }
  if (valisdouble(v))  return VALDOUBLE;
 #endif

  if (val_isdouble(v))  return VALDOUBLE;
  if (val_isnil(v))  return VALNIL;
  if (val_isbool(v)) return VALBOOL;
  return (v.v >> 48);

}

char *valtostring(val_t v)
{
  char *s =NULL;
  uint64_t n;
  val_t b;
  int sto;
  errno = 0;
  switch (valtype(v)) {
    case VALSTRING: s = ((char *)valtopointer(v));  val_dbg("STRING IS STRING OR POINTER (%p) [%s]",(void *)s,s); break; 
    case VALBUF:    s = buf(v,0); val_dbg("STRING IS BUF (%p) [%s]",(void *)s,s); break;
    case VALSTORED: n = v.v & VAL_PAYLOAD;
                    sto = (n & 0x0000000300000000) >> 32;
                    b = buf_stores[sto];
                    if ((n >> 40) == buf_stores_cnt[sto])
                       s = buf(b, (n & 0xFFFFFFFF));
                    break;
  } 
  if (s == NULL) s = valemptystr;
  return s;
}

#if 0
int valcmp(val_t a, val_t b)
{
  double d_a, d_b;
  char *s_a, *s_b;

  int t_a = valtype(a);
  int t_b = valtype(b);

  switch (t_a << 4 | t_b) {

    case VALDOUBLE   << 4 | VALDOUBLE  :   d_a = valtodouble(a);          d_b = valtodouble(b);          goto cmpdbl;
    case VALDOUBLE   << 4 | VALINTEGER :   d_a = valtodouble(a);          d_b = (double)valtointeger(b); goto cmpdbl;
    case VALINTEGER  << 4 | VALDOUBLE  :   d_a = (double)valtointeger(a); d_b = valtodouble(b);          goto cmpdbl;
    cmpdbl:
      return d_a == d_b ? 0 : d_a > d_b ? 1 : -1;
          
    case VALSTRING  << 4 | VALSTRING  :
    case VALSTRING  << 4 | VALBUF     :
    case VALSTRING  << 4 | VALSTORED  :
    case VALBUF     << 4 | VALSTRING  :
    case VALBUF     << 4 | VALBUF     :
    case VALBUF     << 4 | VALSTORED  :
    case VALSTORED  << 4 | VALSTRING  :
    case VALSTORED  << 4 | VALBUF     :
    case VALSTORED  << 4 | VALSTORED  :
      s_a = valtostring(a); s_b = valtostring(b);
      val_dbg("[%s] [%s]",s_a,s_b);
      if (s_a == s_b)  return  0;
      if (s_a == NULL) return -1;
      if (s_b == NULL) return  1;
      return strcmp(s_a, s_b);

    case VALNIL << 4 | VALNIL:  return 0;

    default:
      if (t_a == VALNIL) return -1;
      return a.v == b.v ? 0 : a.v > b.v ? 1 : -1;
  }
  return 1;
}
#endif

int valcmp(val_t a, val_t b)
{
  double d_a, d_b;
  char *s_a, *s_b;

  if (a.v == b.v) return 0;
  if (a.v == valnil.v) return -1;
  if (b.v == valnil.v) return 1;

  uint32_t t_a = valtype(a);
  uint32_t t_b = valtype(b);

  switch (t_a << 16 | t_b) {

    case VALINTEGER  << 16 | VALINTEGER: 
      return valtointeger(a) - valtointeger(b);
    
    case VALSTRING  << 16 | VALSTRING:
    case VALSTRING  << 16 | VALBUF:
    case VALSTRING  << 16 | VALSTORED:
    case VALBUF     << 16 | VALSTRING:
    case VALBUF     << 16 | VALBUF:
    case VALBUF     << 16 | VALSTORED:
    case VALSTORED  << 16 | VALSTRING:
    case VALSTORED  << 16 | VALBUF:
    case VALSTORED  << 16 | VALSTORED:
      s_a = valtostring(a);
      s_b = valtostring(b);
      if (s_a == s_b)  return  0;
      if (s_a == NULL) return -1;
      if (s_b == NULL) return  1;
      return strcmp(s_a, s_b);

    case VALDOUBLE   << 16 | VALDOUBLE:  d_a = valtodouble(a);           d_b = valtodouble(b);   break;
    case VALDOUBLE   << 16 | VALINTEGER: d_a = valtodouble(a);           d_b = (double)valtointeger(b);  break;
    case VALINTEGER  << 16 | VALDOUBLE:  d_a = (double)valtointeger(a);  d_b = valtodouble(b);  break;

    default:
      return a.v > b.v ? 1 : -1;
  }

  return d_a == d_b ? 0 : d_a > d_b ? 1 : -1;
}

char *VALDOUBLE_fmt   = "%f";
char *VALSIGNED_fmt   = "%ld";
char *VALUNSIGNED_fmt = "%lu";
char *VALSTRING_fmt   = "%s";
char *VALCONST_fmt    = "%016X"; 
    
void valprintf(val_t v, FILE *f) 
{
   switch (valtype(v)) {
    case VALBOOL:    fprintf(f,"%s",valeq(v,valtrue)? "true" : "false") ; break;
    case VALNIL:     fprintf(f,"nil") ; break;
    case VALVEC:     fprintf(f,"%p[]",(void *)valtopointer(v)) ; break;
    case VALPOINTER: fprintf(f,"%p",(void *)valtopointer(v)) ; break;
    case VALBUF:
    case VALSTORED:
    case VALSTRING:  fprintf(f,VALSTRING_fmt,valtostring(v)); break;
    case VALDOUBLE:  fprintf(f,VALDOUBLE_fmt,valtodouble(v)); break;
    case VALINTEGER: if (valissigned(v)) fprintf(f,VALSIGNED_fmt,(int64_t)valtointeger(v)); 
                     else fprintf(f,VALUNSIGNED_fmt,(uint64_t)valtointeger(v));
                     break;
    case VALCONST:   fprintf(f,VALCONST_fmt,v.v); break;
    default:         fprintf(f,"%016lX",v.v); break;
   }
}

#define FNV_32_PRIME ((uint32_t)0x01000193)
#define FNV1_32_INIT ((uint32_t)0x811c9dc5)
#define hash fnv_1a
static uint32_t fnv_1a(void *buf, size_t len)
{
    unsigned char *bp = (unsigned char *)buf;
    uint32_t hval = FNV1_32_INIT;
    if (len > 0) {
      while (len--) {
        hval ^= (uint32_t)*bp++;
        hval *= FNV_32_PRIME;
      }
    }
    else {
      while (*bp) {
        hval ^= (uint32_t)*bp++;
        hval *= FNV_32_PRIME;
      }
    }
    return hval;
}


uint32_t valhash_(val_t v)
{
  void *s = valtostring(v);
  uint32_t h;
  if (errno == 0)
    h = hash(s, 0);
  else
    h = hash((void*)&v, sizeof(val_t));

 	h ^= h << 13;
	h ^= h >> 17;
	h ^= h << 5;
  return h;
}

uint32_t val_getaux(val_t v)
{
  errno = 0;
  uint32_t *aux;

  switch(valtype(v)) {
    case VALVEC:    return valtovec(v)->aux;
    case VALBUF:    return valtobuf(v)->aux;
    case VALSTORED: aux = (uint32_t *)valtostring(v);
                    if (aux != (uint32_t *)valemptystr) return aux[-1];
  }
  errno = EINVAL;
  return 0;
}

uint32_t val_setaux(val_t v, uint32_t n)
{
  uint32_t *aux;
  errno = 0;
  switch(valtype(v)) {
    case VALVEC:    return valtovec(v)->aux = n;
    case VALBUF:    return valtobuf(v)->aux = n;
    case VALSTORED: aux = (uint32_t *)valtostring(v);
                    if (aux != (uint32_t *)valemptystr) return aux[-1] = n;
  }
  errno = EINVAL;
  return 0;
}
#line 1 "vec.c"
//  SPDX-FileCopyrightText: © 2023 Remo Dentato <rdentato@gmail.com>
//  SPDX-License-Identifier: MIT

#ifndef VEC_VERSION
#include "vec_.h"
#endif

#include <stdalign.h>

#ifndef RETURN_IF
#define RETURN_IF valreturnif
#endif

#define VALS_PER_MAPNODE 4

typedef struct vec_mapnode_s {
  val_t    value;
  val_t    key;
  uint32_t level;
  uint32_t parent;
  uint32_t left;
  uint32_t right;
} vec_mapnode_t;

static_assert(sizeof(vec_mapnode_t) == VALS_PER_MAPNODE * sizeof(val_t));

#define getnode(v,n)   ((vec_mapnode_t *) &((v)->vec[n]))
#define setleft(n,l)   ((n)->left = l)
#define getleft(n)     ((n)->left)
#define setright(n,r)  ((n)->right = r)
#define getright(n)    ((n)->right)
#define getroot(t)     ((t)->fst)
#define setroot(t,n)   ((t)->fst = n)
#define getkey(n)      ((n)->key)
#define getvalue(n)    ((n)->value)
#define setkey(n,k)    ((n)->key = k)
#define setvalue(n,v)  ((n)->value = v)

#define setlevel(n,l)  ((n)->level = l)

#define getlevel(...) VEC_vrg(getlevel_,__VA_ARGS__)
#define getlevel_1(n)    ((n)->level)
#define getlevel_2(v,n)  (getnode(v,n)->level)

#define getparent(...) VEC_vrg(getparent_,__VA_ARGS__)
#define getparent_1(n)   ((n)->parent)
#define getparent_2(v,n)   (getnode(v,n)->parent)

#define setparent(...) VEC_vrg(setparent_,__VA_ARGS__)
#define setparent_2(n,p)   ((n)->parent = p)
#define setparent_3(v,n,p) (getnode(v,n)->parent = p)

#define getnumnodes(v) (((v)->end) / VALS_PER_MAPNODE)
#define incnumnodes(v) ((v)->end += VALS_PER_MAPNODE)
#define decnumnodes(v) ((v)->end -= VALS_PER_MAPNODE)
#define lastnodendx(v) ((v)->end - VALS_PER_MAPNODE)

#define is_emptytree(v) ((v)->end == 0)

#define marknode(n)     ((n)->level |= 0x80000000)
#define unmarknode(n)   ((n)->level &= 0x7FFFFFFF)
#define ismarkednode(n) ((n)->level &  0x80000000)

#define NULLNODE       VECNONDX

static int volatile ZERO = 0;

val_t vecnew() {
  errno = 0;
  vec_t v = malloc(sizeof(struct vec_s));
  
  RETURN_IF(v == NULL, valnil, ENOMEM);
  
  memset(v,0,sizeof(struct vec_s));
  return val(v);
}

static val_t vec_free(val_t vv);

static void freevecs(val_t *v,uint32_t from, uint32_t to)
{
  if (v) for (int k=from; k<to; k++) {
    //val_dbg("free: %016lX %d %d",v[k].v,k,valisownedvec(v[k]));
    if (valisownedvec(v[k])) {
      v[k] = vec_free(v[k]);
    }
  }
}

static val_t vec_free(val_t vv) 
{ 
  vec_t v;
  errno = 0;
  v = (vec_t)valtocleanpointer(vv); 
  //freevecs(v->vec,v->fst,v->end);
  free(v->vec);
  free(v); 
  return valnil;
}

val_t vecfree(val_t vv) {
  errno = 0;
  
  RETURN_IF(!valisvec(vv) || valisownedvec(vv), vv, EINVAL);
  
  return vec_free(vv);
}

// Ensure the vector is large enough to store a value at index n
static int vec_makeroom(vec_t v, uint32_t n)
{
  uint32_t new_sze;
  val_t   *new_vec;

  errno = 0;
  RETURN_IF(!v,0, EINVAL);

  val_dbg("vec Making room %p (%p)[%d]->[%d]",(void *)v,(void *)(v->vec),v->sze,n);

  // There should be at least n+1 slots:
  n += 1;

  if (n <= v->sze) return 1; // There's enough room

  RETURN_IF(n > VECMAXNDX,0,ERANGE); // Max number of elments in a vector reached.
  
  if (n >= 0xD085FAF0) new_sze = n; // an n higher than that would make the new size overflow
  else {
    new_sze = v->sze ? v->sze : 4;
    while (new_sze <= n) { 
      new_sze = (new_sze * 13) / 8; // (new_sze + (new_sze/2) + (new_sze/8));  
    }
  }
  
  new_sze += (new_sze & 1); // ensure is even
  
  RETURN_IF(new_sze <= n,0,ERANGE);

  val_dbg("MKROOM: realloc(%p,%lu) [%u]->[%u]",(v->vec),new_sze * sizeof(val_t),v->sze, new_sze);
  new_vec = realloc(v->vec, new_sze * sizeof(val_t));
  val_dbg("MKROOM: got(%p,%d) [%d]",new_vec,new_sze * sizeof(val_t),new_sze);

  RETURN_IF(new_vec == NULL,0,ENOMEM);

  // set the newly allocated area to 0;
  memset(&(new_vec[v->end]),0,(new_sze-v->sze)*sizeof(val_t));

  v->vec = new_vec;
  v->sze = new_sze;
  
  return 1;
}

int vec_gap_3(val_t vv, uint32_t i, uint32_t l)
{
  vec_t v;
  int n;
  errno = 0;

  RETURN_IF(!valisvec(vv),0,EINVAL);

  v = (vec_t)valtovec(vv);

  if (i == VECNONDX) i = v->end;
  if (l == VECNONDX) l = 1;
  val_dbg("GAP : %d %d",i,l);

  if (i < v->end) {
    n = v->end+l;
    /*                    __l__
    **  ABCDEFGH       ABC-----DEFGH
    **  |  |    |      |  |    |    |
    **  0  i    end    0  i   i+l   end+l
    */
  }
  else {
    n = i+l;
    /*                                           __l__
    **  ABCDEFGH........          ABCDEFGH............
    **  |       |     |           |             |     |
    **  0       end   i           0             i     i+l
    */
  }
  val_dbg("GAP end:%d i:%d l:%d n:%d",v->end,i,l,n);
  if (!vec_makeroom(v, n)) return 0;
  if (i < v->end) {
    memmove(&(v->vec[i+l]),  &(v->vec[i]),  (v->end-i)*sizeof(val_t));
    memset(&(v->vec[i]),0, l * sizeof(val_t));
  }
  v->end = n;
  return 1;
}

val_t *vec(val_t v)
{
  RETURN_IF(!valisvec(v),NULL,EINVAL);
  return ((vec_t)valtocleanpointer(v))->vec;
}

int vectype_2(val_t vv,int type)
{ 
  vec_t v;
  RETURN_IF(!valisvec(vv),VEC_ISNOT,EINVAL);
  v = (vec_t)valtovec(vv);
  if (type >=0 ) v->typ = (type & 0x0F) | (v->typ & 0xF0);
  return (v->typ & 0x0F);
}

uint32_t vecsize_2(val_t vv, uint32_t n)
{
  vec_t v;
  errno = 0;
  RETURN_IF(!valisvec(vv),0,EINVAL); 
  v = (vec_t)valtovec(vv);
  RETURN_IF(n != VECNONDX && !vec_makeroom(v,n),0,ENOMEM);
  return v->sze;
}

uint32_t veccount_2(val_t vv, uint32_t n)
{
  vec_t v;
  errno = 0;

  RETURN_IF(!valisvec(vv),0,EINVAL);
  
  v = (vec_t)valtovec(vv);

  if (n == 0) {
    v->end = 0;
    v->fst = 0;
    v->flg &= VEC_NOTYPE;
    return 0;
  }

  if (vectype(vv) == VEC_ISMAP) return getnumnodes(v);

  uint32_t first = 0;
  if (vectype(vv) == VEC_ISQUEUE) first = v->fst;

  if (n != VECNONDX && (n < (VECMAXNDX - first)))  {
    n += first;
    RETURN_IF(!vec_makeroom(v, n ), 0, ENOMEM);
    if (n > v->end) v->flg &= ~VEC_SORTED;
    v->end = n;
    if (v->fst > n) v->fst = n;
  }

  return v->end - first;
}

// ADDING VALUES TO VEC

static val_t setval(vec_t v, uint32_t i, val_t x)
{
  // I'm about to overwrite an owned vector. Free it! (unless it is the same!)
  if ((v->fst <= i && i < v->end) && valisownedvec(v->vec[i]))
    if (v->vec[i].v != (x.v | 1)) vec_free(v->vec[i]);
  v->vec[i] = x;
  v->flg &= ~VEC_SORTED;
  return x;
}

val_t vecins_(val_t vv, uint32_t i, val_t x)
{
  vec_t v;

  RETURN_IF(!vecmakegap(vv,i,1),valerror,ENOMEM);

  v = (vec_t)valtocleanpointer(vv);
  return setval(v, i, x);
}

val_t vecset_(val_t vv, uint32_t i, val_t x)
{
  errno = 0;
  RETURN_IF(!valisvec(vv), valerror, EINVAL);

  vec_t v = (vec_t)valtocleanpointer(vv);

  if (i == VECNONDX) i = v->end;
  
  RETURN_IF(!vec_makeroom(v,i), valerror, ENOMEM);

  x=setval(v, i, x);

  if (i >= v->end) v->end = i+1;
  return x;
}

val_t vecown_3_(val_t vv, uint32_t i, val_t x)
{
  if (valeq(x,valown)) x = vecget(vv,i);
  if (valisvec(x)) x.v |= 1;
  return vecset_(vv,i,x);
}

val_t vecdisown(val_t vv, uint32_t i)
{
  val_t x = vecget(vv,i);
 
  if (valisownedvec(x)) {
    //val_dbg("disowned");
    x.v &= ((uint64_t)0xFFFFFFFFFFFFFFFE);
    ((vec_t)valtocleanpointer(vv))->vec[i] = x;
    return x;
  }
  else return x;
}

val_t vecpush_(val_t vv, val_t x, int own) 
{ 
  errno = 0;
  RETURN_IF(!valisvec(vv), valerror, EINVAL);
  vectype(vv, VEC_ISSTACK); 
  if (own) return vecown_3_(vv,VECNONDX,x);
  return vecset_(vv,VECNONDX,x); 
}

val_t vecenq_(val_t vv, val_t x,int own)
{
  vec_t v;
  errno = 0;
  RETURN_IF(!valisvec(vv), valerror, EINVAL);

  vectype(vv,VEC_ISQUEUE);
  v = (vec_t)valtocleanpointer(vv);
  if ((v->fst > (v->end / 2)) && (v->end >= v->sze)) {
    memmove(v->vec, &(v->vec[v->fst]), (v->end - v->fst) * sizeof(val_t));
    v->end -= v->fst;
    v->fst = 0;
  }
  
  if (own) return vecown_3_(vv,VECNONDX,x);
  return vecset_(vv,VECNONDX,x); 
}

#define vec_data(v_) ((v_)->vec)

uint32_t vec_search_map(vec_t v, val_t key, uint32_t *parent_ndx, int *right, int *depth);

val_t vecget_(val_t vv, val_t ii, val_t *key)
{
  vec_t v;
  errno = 0;
  val_t x;
  uint32_t parent_ndx;
  int right;
  int depth;

  RETURN_IF(!valisvec(vv), valnil, EINVAL);
  v = (vec_t)valtovec(vv);

  if (vectype(vv) == VEC_ISMAP) {
    val_dbg("Retrieve from map");
    if (key) *key = valnil;
    uint32_t node_ndx = vec_search_map(v,ii,&parent_ndx,&right,&depth);
    RETURN_IF(node_ndx == NULLNODE, valnil,0);
    vec_mapnode_t *node = getnode(v,node_ndx);
    if (key) *key = getkey(node);
    return getvalue(node);
  }
  else {
    uint32_t i = valtointeger(ii);

    if (i == VECNONDX) i = v->end - 1; // Get the last element
    RETURN_IF(i >= v->end, valnil, ERANGE);

    x = v->vec[i];
  }
  return x;
}

val_t vecgetfirst(val_t vv)
{
  vec_t v;
  errno = 0;
  val_t x;
  uint32_t first = 0;
  int vtype;

  RETURN_IF(!valisvec(vv), valnil, EINVAL);
  v = (vec_t)valtovec(vv);

  RETURN_IF(v->end == 0, valnil, 0);

  vtype = v->flg & 0x0F;
  if (vtype == VEC_ISMAP) {
    return valnil;
  }
  else {
    if (vtype == VEC_ISQUEUE) first = v->fst;
    else if (vtype == VEC_ISSTACK) first = v->end - 1;
    x = v->vec[first];
  }
  return x;
}

val_t vecgetnext(val_t v);
val_t vecgetmin(val_t vv);
val_t vecgetmax(val_t vv);

uint32_t vecdel_3_(val_t vv, val_t ii, uint32_t j)
{
  uint32_t l,i;
  vec_t v;
  errno = 0;
  
  RETURN_IF(!valisvec(vv), 0, EINVAL);
  RETURN_IF(vecisempty(vv), 0, ERANGE);

  v = (vec_t)valtocleanpointer(vv);

  if (vectype(vv) == VEC_ISMAP) return vecunmap_(vv,ii);

  i = valtointeger(ii);

  val_dbg("i = %d, j = %d, end = %d, sze = %d",i,j,v->end,v->sze);
  if (i == VECNONDX) { i = v->end-1; j = i; }
  else if (j == VECNONDX) j = i;
  if (i >= v->end || j<i) return 0;

  if (j >= v->end-1) {
    freevecs(v->vec,i,v->end);
    v->end = i; // Just drop last elements
  }
  else {
    freevecs(v->vec,i,j+1);

    //       __l__
    //    ABCdefghIJKLM              ABCIJKLM
    //    |  |    |    |             |  |    |    
    //    0  i   i+l   end           0  i    end-l  

    l = (j-i)+1;
    memmove(&(v->vec[i]) , &(v->vec[i+l]),  (v->end-(i+l)) * sizeof(val_t));
    v->end -= l;
  }
  val_dbg("i = %d, j = %d, end = %d, sze = %d",i,j,v->end,v->sze);
  return(v->end);
}

uint32_t vecdrop_2(val_t vv, uint32_t n) {
  vec_t v;
  uint32_t cnt;
  errno = 0;

  RETURN_IF(!valisvec(vv), 0, EINVAL);

  cnt = veccount(vv);

  if (n != 0 && cnt != 0) {
    v = (vec_t)valtocleanpointer(vv);
    if (n >= cnt) n = cnt;

    freevecs(v->vec,v->end-n,v->end);

    v->end -= n;
    if (v->fst >= v->end) {
       v->fst = 0; v->end = 0; v->typ = VEC_ISVEC;
    }
    cnt = (v->end);
  }
  return cnt;
}

// Deque (removes the next n elements form the queue)
uint32_t vecdeq_2(val_t vv, uint32_t n) 
{
  uint32_t ret = 0;
  vec_t v;
  errno = 0;

  RETURN_IF(!valisvec(vv), 0, EINVAL);
  RETURN_IF(vectype(vv) != VEC_ISQUEUE, 0, EINVAL);
  
  ret = veccount(vv);

  if (ret > 0) {
    v = (vec_t)valtocleanpointer(vv);
    n += v->fst;
    if (n > v->end) n = v->end;

    freevecs(v->vec, v->fst, n);
    v->fst = n;
    if (v->fst >= v->end) {
      v->fst = 0; v->end = 0;
      v->typ = VEC_ISVEC;
    }
    ret = v->end - v->fst;
  }
  return ret;
}

uint32_t vecindex_3(val_t vv,uint32_t ndx, int32_t delta)
{
  uint32_t ret = VECERRORNDX;
  errno = 0;
  
  RETURN_IF(!valisvec(vv), VECERRORNDX, EINVAL);
  RETURN_IF(vecisempty(vv), VECERRORNDX, EINVAL);

  vec_t v = (vec_t)valtovec(vv);

  if (ndx < VECNONDX)          ret = ndx;
  else if (ndx == VECNONDX)    ret = ndx;
  else if (ndx == VECCOUNTNDX) ret = v->end;
  else if (ndx == VECSIZENDX)  ret = v->sze;
  else switch(vectype(vv)) {
    case VEC_ISSTACK: switch(ndx) {
                        case VECNEXTNDX:
                        case VECFIRSTNDX:
                        case VECTOPNDX:  ret = v->end-1;
                                         if (delta < 0) delta = -delta;
                                         if (delta > ret) ret = VECERRORNDX;
                                         else ret -= delta;
                                         break;

                        case VECTAILNDX:
                        case VECLASTNDX: ret = 0; 
                                         if (delta < 0) delta = -delta;
                                         if (delta > v->end-1) ret = VECERRORNDX;
                                         else ret += delta;
                                         break;
    } 
    break;

    case VEC_ISQUEUE: switch(ndx) {
                        case VECNEXTNDX:
                        case VECFIRSTNDX:
                        case VECTOPNDX:
                        case VECHEADNDX: ret = v->fst;
                                         if (delta < 0) delta = -delta;
                                         if (delta > ((v->end-1) - v->fst)) ret = VECERRORNDX;
                                         else ret += delta;
                                         break;

                        case VECTAILNDX:
                        case VECLASTNDX: ret = v->end-1;
                                         if (delta < 0) delta = -delta;
                                         if (delta > ((v->end-1) - v->fst)) ret = VECERRORNDX;
                                         else ret -= delta;
                                         break;
    } 
    break;

    default: switch(ndx) {
                        case VECNEXTNDX:  ret = v->end;  break;

                        case VECHEADNDX:  
                        case VECFIRSTNDX: ret = 0;  break;

                        case VECTAILNDX:
                        case VECLASTNDX: ret = v->end-1;
                                         if (delta < 0) delta = -delta;
                                         if (delta > ret) ret = VECERRORNDX;
                                         else ret -= delta;
                                         break;

                        case VECCURNDX:  ret = v->cur; break;
    }
  }

  return ret;
}

int vec_cmp(const void *a, const void *b)
{
  int ret;
  ret = valcmp(*((val_t *)a),*((val_t *)b));
  val_dbg("comparing %016lX %016lX = %d",((val_t *)a)->v,((val_t *)b)->v, ret);
  return ret;
}

int vecsort_3(val_t vv, vec_cmp_t cmp, val_t aux)
{
  int ret = 0;
  errno = 0;

  RETURN_IF(!valisvec(vv), 1, EINVAL);
  RETURN_IF(vecisempty(vv), 0, 0);

  vec_t v = valtovec(vv);
  val_dbg("About to sort %p[%u]",(void *)v,v->end);

  val_dbg("[0] = %16lX",(v->vec[0]).v);
  qsort(v->vec,v->end,sizeof(val_t),vec_cmp);
  v->flg |= VEC_SORTED;
  return ret;
}

int vecissorted_(val_t vv)
{
  RETURN_IF(!valisvec(vv), 0, EINVAL);
  RETURN_IF(vecisempty(vv), 0, 0);

  vec_t v = valtovec(vv);

  return v->flg & VEC_SORTED;

}

uint32_t vec_search_sorted(vec_t v, val_t key)
{
  val_dbg("Searching in sorted");
  val_t *found = bsearch(&key, v->vec,v->end, sizeof(val_t), vec_cmp);
  RETURN_IF(!found, VECNONDX, 0);
  return (uint32_t)(found - v->vec);
}

uint32_t vec_search_linear(vec_t v, val_t key)
{
  val_dbg("Searching linear");
  for (uint32_t found = 0; found < v->end; found++)
    if (valcmp(v->vec[found], key) == 0) return found;
  return VECNONDX;
}

uint32_t vec_search_map(vec_t v, val_t key, uint32_t *parent_ndx, int *right,int *depth)
{
  uint32_t node_ndx = 0;
  vec_mapnode_t *node;
  *parent_ndx = VECNONDX;
  *right = 0;
  *depth = 0;
  if (!is_emptytree(v)) {
    node_ndx = getroot(v);
    val_dbg("SEARCHIG root: %d",node_ndx);
    while (node_ndx != NULLNODE) {
      node = getnode(v,node_ndx);
      val_dbg("node_ndx: %d left: %d right: %d",node_ndx, getleft(node), getright(node));
      int cmp = valcmp(key, getkey(node));

      if (cmp == 0) {
        val_dbg("FOUND! (%d)",*depth);
        return node_ndx;
      }

      *parent_ndx = node_ndx;
      *depth += 1;

      if (cmp < 0) {
        *right = 0;
        node_ndx = getleft(node);
      } else {
        *right = 1;
        node_ndx = getright(node);
      }
    }
  }
  
  val_dbg("NOT FOUND! (%d)",*depth);
  return VECNONDX;
}

uint32_t vecsearch_(val_t vv, val_t key)
{
  
  RETURN_IF(!valisvec(vv), VECNONDX, EINVAL);
  RETURN_IF(vecisempty(vv), VECNONDX, EINVAL);

  vec_t v = valtovec(vv);
  uint32_t parent_ndx;
  int right;
  int depth;
 
  if (v->flg & VEC_SORTED)
    return vec_search_sorted(v,key);
  else if (vectype(vv) == VEC_ISMAP)
    return vec_search_map(v,key, &parent_ndx, &right, &depth);
 else 
    return vec_search_linear(v,key);
}

#define MAXLEVEL 0xFFFFFFFF

#include <time.h>
#if 1
// Function to generate a random 32-bit number using Xorshift
static uint32_t RND() {
    static uint32_t state = 0; // A non-zero seed value
    if (state == 0) state = 2463534242 * (uint32_t)time(0);
    uint32_t x = state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    state = x;
    // if (x == MAXLEVEL) x -= 1;
    return x & 0x7FFFFFFF;
}
#endif

#if 0
// RNG by burtleburtle http://burtleburtle.net/bob/rand/smallprng.html

#define rot(x,k) (((x)<<(k))|((x)>>(32-(k))))
#define RND_() \
    e = a - rot(b, 27); \
    a = b ^ rot(c, 17);\
    b = c + d;\
    c = d + e;\
    d = e + a;

uint32_t RND( ) {
    static uint32_t a = 0;
    static uint32_t b,c,d;
    uint32_t e;

    if (a == 0) {
      a = 0xf1ea5eed, b = c = d = (uint32_t)time(0);
      for (int i=0; i<20; i++) {
        RND_();
      }
    }
    RND_();
    return d;
}
#endif

#if 0
// Generate 1 random bit (for 50% choices)
static int RNDBIT() {
  static int n = 0;
  static uint32_t state = 0;

  if (n==0) { state = RND(); n = 32; }
  n -= 1;
  return (state & (1<<n));
}
#endif

static uint32_t vec_newmapnode(vec_t v)
{
  uint32_t newnode;
  vec_mapnode_t *node;

  newnode = v->end;
  RETURN_IF(!vec_makeroom(v, newnode+VALS_PER_MAPNODE), VECNONDX, ENOMEM);
  v->end += VALS_PER_MAPNODE;
  node = getnode(v,newnode);

  setleft(node, NULLNODE);
  setright(node, NULLNODE);
  setparent(node, NULLNODE);
  setkey(node, valnil);
  setvalue(node, valnil);
  setlevel(node, RND());

  return newnode;
}

static void moveup(vec_t v, uint32_t node_ndx, uint32_t parent_ndx, int is_right_child)
{
  vec_mapnode_t *parent_node;
  vec_mapnode_t *node;
  uint32_t grandparent;
  vec_mapnode_t *grandparent_node;
  uint32_t chld_ndx;
  int right_next = 0;

  node = getnode(v, node_ndx);
  parent_node = getnode(v,parent_ndx);

  while (parent_ndx != NULLNODE && getlevel(node) >= getlevel(parent_node)) {
    grandparent = getparent(parent_node);
    setparent(node, grandparent);

    if (grandparent != NULLNODE) {
      grandparent_node = getnode(v,grandparent);
      if (getleft(grandparent_node) == parent_ndx) {
        setleft(grandparent_node, node_ndx);
        right_next = 0;
      }
      else {
        setright(grandparent_node, node_ndx);
        right_next = 1;
      }
    }
    else {
      grandparent_node = NULL;
      setroot(v, node_ndx);
      val_dbg("New root: %d", node_ndx);
    }

    if (is_right_child) { // node is parent's right children ROTATE LEFT!
        chld_ndx = getleft(node);
        setright(parent_node, chld_ndx); val_dbg("Setting right children to %d",chld_ndx);
        setleft(node, parent_ndx);
        val_dbg("ROTATED LEFT");
    } else {
        chld_ndx = getright(node); 
        setleft(parent_node, chld_ndx); val_dbg("Setting left children to %d",chld_ndx);
        setright(node, parent_ndx);
        val_dbg("ROTATED RIGHT");
    }
    if (chld_ndx != NULLNODE) setparent(v,chld_ndx,parent_ndx);
    setparent(parent_node, node_ndx);
    parent_ndx = grandparent;
    parent_node = grandparent_node;
    is_right_child = right_next;
  }
}

uint32_t vecmap_(val_t vv, val_t key, val_t value)
{
  RETURN_IF(!valisvec(vv), NULLNODE, EINVAL);
  vec_t v = valtovec(vv);
  uint32_t node_ndx = NULLNODE;
  uint32_t parent_ndx = NULLNODE;
  int is_right_child = 0;
  int depth = 0;
  vec_mapnode_t *node;
  vec_mapnode_t *parent_node;

  int t = vectype(vv);

  if (t == VEC_ISMAP && valisnil(value)) return vecunmap_(vv,key);

  vectype(vv,VEC_ISMAP);

  if (!is_emptytree(v)) {
    node_ndx = vec_search_map(v,key,&parent_ndx,&is_right_child,&depth);
  }
  else val_dbg("INSERT INTO EMPTY TREE");
  
  if (node_ndx == NULLNODE) {
    val_dbg("Adding a new node");
    node_ndx = vec_newmapnode(v);
    RETURN_IF(node_ndx == NULLNODE, NULLNODE, ENOMEM);
  }

  node = getnode(v,node_ndx);
  setkey(node, key);
  setvalue(node, value);
  setparent(node, parent_ndx);

  // Insert the node as a leaf
  if (parent_ndx != NULLNODE) {
    parent_node = getnode(v,parent_ndx);
    if (is_right_child) {
      setright(parent_node, node_ndx);
      val_dbg("MAP INSERT RIGHT parent: %d depth:%d (lvl: %d))",parent_ndx, depth, node_lvl);
    }
    else {
      setleft(parent_node, node_ndx);
      val_dbg("MAP INSERT LEFT parent: %d depth:%d (lvl: %d))",parent_ndx, depth, node_lvl);
    }
  }
  else {
    val_dbg("MAP INSERT ROOT parent: %d depth:%d (lvl: %d))",parent_ndx, depth, node_lvl);
    return node_ndx;
  }

  moveup(v, node_ndx, parent_ndx, is_right_child);

  return node_ndx;
}
      

uint32_t vecunmap_(val_t vv, val_t key)
{
  RETURN_IF(!valisvec(vv), NULLNODE, EINVAL);
  RETURN_IF(vectype(vv) != VEC_ISMAP, NULLNODE, EINVAL);

  vec_t v = valtovec(vv);
  uint32_t node_ndx = NULLNODE;
  uint32_t parent_ndx = NULLNODE;
  int is_right_child = 0;
  int depth = 0;
  vec_mapnode_t *node;
  vec_mapnode_t *parent_node;
  uint32_t left_level = 0, right_level = 0;

  errno = 0;
  
  if (is_emptytree(v)) return NULLNODE;

  node_ndx = vec_search_map(v,key,&parent_ndx,&is_right_child,&depth);

  if (node_ndx == NULLNODE) return NULLNODE;

  // If there is only one node, just remove it
  if (getnumnodes(v) == 1) {
    v->end = 0;
    setroot(v,NULLNODE);
    return NULLNODE;
  }

  node = getnode(v,node_ndx);

#if 0
  if (node_ndx != getroot(v)) {
    setlevel(node,MAXLEVEL);
    moveup(v, node_ndx, parent_ndx, is_right_child);
    val_dbg("Moved up %d",node_ndx);
    vec_printtree(v,stdout);
  }
  else val_dbg("Already at root");
#endif


  // starting from node 
  // if the node is a leaf, remove it and break
  // if the node has a single child, remove it and attach child to its grandparent and breal
  // else, rotate the node (moving it down) so that the children with highest level moves up

  // take the last node and copy it in the freed node.
  uint32_t grandparent_ndx;
  vec_mapnode_t *grandparent_node;
  uint32_t child_ndx;
  vec_mapnode_t *child_node;

  #define set_grandparent() \
      if (grandparent_ndx != NULLNODE) { \
        grandparent_node = getnode(v,grandparent_ndx); \
        if (getright(grandparent_node) == node_ndx) \
          setright(grandparent_node, child_ndx); \
        else \
          setleft(grandparent_node, child_ndx); \
      } \
      else { \
        setroot(v,child_ndx); \
      } \

  while (1) {
    val_dbg("Moving down %d",node_ndx);
    grandparent_ndx = getparent(node);
    
    child_ndx = getleft(node);
    if (child_ndx == NULLNODE) {  // either it's a leaf or only has a right child
      val_dbg("[ No left children");
      child_ndx = getright(node);

      set_grandparent();      

      if (child_ndx != NULLNODE) {  // Node only has a right child
        child_node = getnode(v, child_ndx);
        setparent(child_node, grandparent_ndx);
      }
      else val_dbg("No right children ]");

      break;
    }
    else {
      // Either it has two children or only has the left child
      if (getright(node) == NULLNODE) {
         val_dbg("[ No right children");
        // Only has a left child
  
        set_grandparent();      
  
        child_node = getnode(v, child_ndx);
        setparent(child_node, grandparent_ndx);
        break;
      }
      else {
        // Has both left and right child
        val_dbg("Both children");
        // rotate so that the children with highest level moves up
        left_level = getlevel(v,getleft(node));
        right_level = getlevel(v,getright(node));
        
        if (right_level > left_level)  child_ndx = getright(node); // Rotate left
        else  child_ndx = getleft(node);           // Rotate right

        set_grandparent();      

        child_node = getnode(v,child_ndx);
        setparent(child_node,grandparent_ndx);
        if (right_level > left_level) {
          setright(node,getleft(child_node));
          setleft(child_node,node_ndx);
        }
        else {
          setleft(node,getright(child_node));
          setright(child_node,node_ndx);
        }
        setparent(node,child_ndx);
      }
    }
  }

  // Fix the last node
  child_ndx = v->end - 4; // index of last element
  if (node_ndx != child_ndx) {
    child_node = getnode(v,child_ndx);
    *node = *child_node;
    parent_ndx = getparent(child_node);
    if (parent_ndx == NULLNODE) {
      setroot(v,node_ndx);
    }
    else {
      parent_node = getnode(v,parent_ndx);
      if (getleft(parent_node) == child_ndx)
        setleft(parent_node, node_ndx);
      else
        setright(parent_node, node_ndx);
    }
  }
  
  decnumnodes(v); // Decrement the number of nodes by one

  return 0;
}

void vec_printtree(vec_t v, FILE *f)
{
  fprintf(f,"graph BST {\nnode [shape=circle];\n");
  vec_mapnode_t *node;
  for (int k=0; k < v->end; k+=4) {
    node = getnode(v,k);

    fprintf(f,"%d [label=\"",k);
    valprintf(getkey(node),f);
    fprintf(f,"\n%08X",getlevel(node));
    fprintf(f,"\"];\n");

    fprintf(f,"%d -- ",k);

    if (getleft(node) != NULLNODE) fprintf(f,"%d",getleft(node));
    else fprintf(f,"L%d",k);

    fprintf(f," [label=\"L\"];\n");
    
    fprintf(f,"%d -- ",k);

    if (getright(node) != NULLNODE) fprintf(f,"%d",getright(node));
    else fprintf(f,"R%d",k);

    fprintf(f," [label=\"R\"];\n");

    if (getleft(node) == NULLNODE) fprintf(f,"L%d [shape=point];\n",k);
    if (getright(node) == NULLNODE) fprintf(f,"R%d [shape=point];\n",k);

  }

  fprintf(f,"}\n");
  fflush(f);

}

void vecprinttree(val_t vv, FILE *f)
{
  if (!valisvec(vv)) {errno = EINVAL; return ;};
  vec_t v = valtovec(vv);
  vec_printtree(v,f);
}

val_t vecmapfirst_(val_t vv, val_t *key)
{
  if (!valisvec(vv)) {errno = EINVAL; return valnil ;};
  vec_t v = valtovec(vv);
  
  uint32_t node_ndx = getroot(v);
  vec_mapnode_t *node;
  uint32_t child_ndx;
  while (1) {
    node = getnode(v, node_ndx);
    unmarknode(node);
    child_ndx = getleft(node);
    if (child_ndx == NULLNODE) {
      v->cur = node_ndx;
      marknode(node);
      if (key) *key = getkey(node);
      return (getvalue(node));
    }
    node_ndx = child_ndx;
  }
  return valnil;
}

val_t vecmapnext_(val_t vv, val_t *key)
{
  if (!valisvec(vv)) {errno = EINVAL; return valnil;};

  if (vectype(vv) != VEC_ISMAP) return valnil;

  vec_t v = valtovec(vv);

  if (getnumnodes(v) == 0) return valnil;

  uint32_t node_ndx = getroot(v);
  vec_mapnode_t *node;

  *key = valnil;

  if (v->end == 0 || v->cur >= v->end || v->cur & 0x03) return valnil;
  node_ndx = v->cur;
  node = getnode(v,node_ndx);
  if (getright(node) != NULLNODE) {
    node_ndx = getright(node);
    while (node_ndx != NULLNODE) {
      v->cur = node_ndx;
      node = getnode(v,node_ndx);
      unmarknode(node);
      node_ndx = getleft(node);
    } 
    marknode(node);
    *key = getkey(node);
    return getvalue(node);
  }
  else {
    node_ndx = getparent(node);
    while (node_ndx != NULLNODE) {
      node = getnode(v, node_ndx);
      if (!ismarkednode(node)) {
        v->cur = node_ndx;
        marknode(node);
        *key = getkey(node);
        return getvalue(node);
      }
      node_ndx = getparent(node);
    }
  }

  return valnil;
}

#line 1 "../src/tng.c"
#line 4 "tng.md"
  // SPDX-FileCopyrightText: © 2024 Remo Dentato <rdentato@gmail.com>
  // SPDX-License-Identifier: MIT
#line 109 "tng.md"
#line 809 "tng.md"
#include <stdio.h>

#line 878 "tng.md"
#if !defined(DEBUG) && !defined(NDEBUG)
#define DEBUG DEBUG_TEST
#endif
#line 758 "tng.md"
#ifndef VAL_VERSION
#include "val.h"
#endif
#line 769 "tng.md"
#ifndef VRG_VERSION
#ifndef VRGCLI
#define VRGCLI
#endif
#include "vrg.h"
#endif

#line 827 "tng.md"
#ifndef exception_info
#define exception_info char *msg; int aux;
#include "try.h"
#endif

#line 882 "tng.md"
#ifdef DEBUG
#include "dbg.h"
#endif 

#line 110 "tng.md"
#line 228 "tng.md"
val_t getbuffer(char prefix, char *waypoint);

#line 440 "tng.md"
#define TAG_NONE     0x00
#define TAG_TEXT     0x01
#define TAG_EMPTY    0x02
#define TAG_CODE     0x03
#define TAG_AFTER    0x04
#define TAG_BEFORE   0x05
#define TAG_VOID     0x06
#define TAG_WAYPOINT 0x07
#define TAG_INVOID   0x08
#define TAG_EXVOID   0x09
#define TAG_TICKS    0x0A

#line 720 "tng.md"
int count_out_recur = 0;
#line 840 "tng.md"
// These are the defined exceptions
#define EX_FILENOTFOUND 1
#define EX_OUTOFMEM     2
#define EX_SYNTAXERROR  3
#define EX_NOFILEBUFFER 4
#define EX_SYNTAXERR    5
#define EX_DUPLICATEBUF 6
#define EX_INFINITELOOP 7
#define EX_FILEEMPTY    8

#line 887 "tng.md"
#define err(...) (fflush(stdout),fprintf(stderr,"ERROR: " __VA_ARGS__),fputc('\n',stderr))
#line 147 "tng.md"
char **filelist = NULL;
#line 193 "tng.md"
val_t code_chunk;  // The code chunk

#line 207 "tng.md"
val_t chunks;      // The after/before chunks

#line 280 "tng.md"
char *to_chunkname(char prefix, char *s);

#line 325 "tng.md"
char *out_filename = NULL;
FILE *out_file;

#line 344 "tng.md"
val_t cur_buffer;

#line 375 "tng.md"
int linenum = 0;
val_t linebuf;

#line 509 "tng.md"
int global_indent = 0;

#line 777 "tng.md"
int nolinenums = 0;
int buildndx = 0;

#line 833 "tng.md"
try_t catch; // initialize the try/catch macros
#line 111 "tng.md"
#line 812 "tng.md"
static inline int isquote(int c)
{
  return (c == '\'' || c == '"' || c == '`');
}

#line 231 "tng.md"
val_t getbuffer(char prefix, char *waypoint)
{
  val_t cname;
  val_t buffer;
  char *name;

  if (prefix == 'C') return code_chunk;

  name = to_chunkname(prefix, waypoint);

  buffer = vecget(chunks,name,&cname);
  if (valisnil(buffer)) {
    // Buffer does not exists yet. Let's create it.
    buffer = bufnew();
    if (valisnil(buffer)) throw(EX_OUTOFMEM,"");

    // We need a copy of the chunk name to use as a key because the original
    // string will be overwritten when the next line is read
    cname = bufstore(name);
    if (valisnil(cname)) throw(EX_OUTOFMEM,"");
    vecmap(chunks,cname,buffer);
  }

  return buffer;
}
#line 283 "tng.md"
char *to_chunkname(char prefix, char *s)
{
  static char name[128];

  int len = 0;

  name[len++] = prefix;
  name[len++] = '~';
  name[len] = '\0';

  char c;
  while(*s && *s != '\x1E' && len < 120) {
    c = *s++;

    if ('0' <= c && c <= '9') {
      name[len++] = c;
    }
    else if ('A' <= c && c <= 'Z') {
      name[len++] = c + 32; // tolower
    }
    else if ('a' <= c && c <= 'z') {
      name[len++] = c;
    }
    else if (len > 2) {
      if (name[len-1] == ' ') len--;
      name[len++] = ' ';
    }
    name[len] = '\0';
  }
  while ( len>0 && name[len-1] == ' ') name[--len] = '\0';

  return name;
}
#line 400 "tng.md"
int parsefile(char *file_name)
{
#line 453 "tng.md"
int   tag = TAG_NONE;
char *tag_arg = NULL;
int   tag_indent = 0;
char  voidstr[32];  
      voidstr[0] = '\0';
char *bufstr;

#line 588 "tng.md"
int code = 0;

#line 403 "tng.md"

#line 423 "tng.md"
FILE *source_file = stdin;
if (file_name != NULL)  {
  source_file = fopen(file_name,"rb");
  if (!source_file) throw(EX_FILENOTFOUND, file_name);
  if (feof(source_file)) throw(EX_FILEEMPTY, file_name);
}
linenum = 0;

#line 405 "tng.md"
  
  cur_buffer = code_chunk;
  _dbgtrc("In parsefile()");
  while (!feof(source_file)) {
#line 387 "tng.md"
buflen(linebuf,0);
bufloadln(linebuf, source_file);
linenum += 1;

#line 410 "tng.md"
    _dbgtrc("ln: %.30s",buf(linebuf,0));
#line 461 "tng.md"
{
  bufstr = buf(linebuf,0);

  tag = TAG_NONE;
  tag_arg = NULL;
  tag_indent = 0;

  if (voidstr[0] != '\0') { // We are in the void
#line 477 "tng.md"
{
  tag = TAG_INVOID;
  char *s = bufstr;
  s = strstr(bufstr,"(void:");
  if (s && s > bufstr && isquote(s[-1])) s=NULL;
  if (s) {
    s += 6;
    if (strncmp(voidstr,s,strlen(voidstr)) == 0) {
       voidstr[0] = '\0';
       tag = TAG_EXVOID;
    }
  } 
}

#line 470 "tng.md"
  }
  else {
#line 492 "tng.md"
{
  char *s = bufstr;
  
  while (*s && *s != '(') {
    if (isquote(s[0]) && s[1] != 0) s++; // Ignore a '(' if preceded by a quote
    s++;
  }
    
  if (*s == '(') {
#line 517 "tng.md"
{
  tag_indent = (s-bufstr) * global_indent;
  while (tag_indent > 0 && !isspace(bufstr[tag_indent-1]))
    tag_indent -= 1;
  
  if (isquote(s[1])) s++; // Ignore a quote right after the '('
  s += 1;
           
       if (*s == ':')                   { tag = TAG_WAYPOINT; s += 1; }
  else if (strncmp("after:",s,6) == 0)  { tag = TAG_AFTER;    s += 6; }
  else if (strncmp("before:",s,7) == 0) { tag = TAG_BEFORE;   s += 7; }
  else if (strncmp("code:",s,5) == 0)   { tag = TAG_CODE;     s += 5; }
  else if (strncmp("text:",s,5) == 0)   { tag = TAG_TEXT;     s += 5; }
  else if (strncmp("void:",s,5) == 0)   { tag = TAG_VOID;     s += 5; }
  
  if (tag != TAG_NONE) {
#line 561 "tng.md"
{
  char *e = s;

  while (isspace(*e)) e++;
  tag_arg = e;
  while (*e && *e != ')') e++;
  if (*e) {
    if (isquote(e[-1])) e--; // Ignore a quote right before the ')'
    *e = '\0';
  }
  else { // We didn't find the ')'
    throw(EX_SYNTAXERR,"Unterminated tag");
  }
}
#line 534 "tng.md"
    if (tag == TAG_WAYPOINT && *tag_arg == '\0') {
      tag = TAG_EMPTY;
      tag_arg = NULL;
    }
  }
}

#line 502 "tng.md"
  }
  else {
#line 542 "tng.md"
{
  s = bufstr;
  while (isspace(*s)) s++;

  if (s[0]=='`' && s[1]=='`' && s[2]=='`') {
    tag = TAG_EMPTY;
    if (!code && s[3] != '\0' && !isspace(s[3])) {
      tag_indent = s-bufstr;
      s+=3;
      tag_arg = s;
      while(*s) s++;
      while(s>tag_arg && isspace(s[-1])) s--;
      *s='\0';
      tag = TAG_CODE;
    }
  }
}

#line 505 "tng.md"
  }
}

#line 473 "tng.md"
  }
}

#line 412 "tng.md"
#line 591 "tng.md"
switch (tag) {

  case TAG_EMPTY:    code = 0; break;
  case TAG_TEXT:     code = 0; break;

  case TAG_NONE:     if (code) { 
                       bufputs(cur_buffer,bufstr);
                       bufputs(cur_buffer,"\n");
                     }
                     break;

  case TAG_WAYPOINT: if (code) { 
                        // Insert a special code in the buffer (0x1B) to signal this is a waypoint
                        bufprintf(cur_buffer, "\x1B%08d %s\x1E\n",tag_indent, tag_arg);
#line 640 "tng.md"
if (!nolinenums) {
  if (file_name != NULL)
    bufprintf(cur_buffer, "\x1F#line %d \"%s\"\n",linenum+1,file_name);
  else
    bufprintf(cur_buffer, "\x1F#line %d\n",linenum+1);
}
#line 606 "tng.md"
                     }
                     break;

  case TAG_BEFORE:   cur_buffer = getbuffer('B',tag_arg);
#line 640 "tng.md"
if (!nolinenums) {
  if (file_name != NULL)
    bufprintf(cur_buffer, "\x1F#line %d \"%s\"\n",linenum+1,file_name);
  else
    bufprintf(cur_buffer, "\x1F#line %d\n",linenum+1);
}
#line 611 "tng.md"
                     code = 1;
                     break;

  case TAG_AFTER:    cur_buffer = getbuffer('A',tag_arg); 
#line 640 "tng.md"
if (!nolinenums) {
  if (file_name != NULL)
    bufprintf(cur_buffer, "\x1F#line %d \"%s\"\n",linenum+1,file_name);
  else
    bufprintf(cur_buffer, "\x1F#line %d\n",linenum+1);
}
#line 616 "tng.md"
                     code = 1; 
                     break;

  case TAG_CODE:     cur_buffer = code_chunk;
#line 640 "tng.md"
if (!nolinenums) {
  if (file_name != NULL)
    bufprintf(cur_buffer, "\x1F#line %d \"%s\"\n",linenum+1,file_name);
  else
    bufprintf(cur_buffer, "\x1F#line %d\n",linenum+1);
}
#line 621 "tng.md"
                     code = 1;
                     break;

  case TAG_VOID:     if (tag_arg && tag_arg[0]) {
                       strncpy(voidstr,tag_arg,31);
                       voidstr[31] ='\0';
                     }
                     break;

  case TAG_INVOID:   bufputs(cur_buffer,bufstr);
                     bufputs(cur_buffer,"\n");
                     break;

  case TAG_EXVOID:   
#line 640 "tng.md"
if (!nolinenums) {
  if (file_name != NULL)
    bufprintf(cur_buffer, "\x1F#line %d \"%s\"\n",linenum+1,file_name);
  else
    bufprintf(cur_buffer, "\x1F#line %d\n",linenum+1);
}
#line 636 "tng.md"
                     break;
}

#line 413 "tng.md"
  }

  _dbgtrc("out parsefile()");

#line 432 "tng.md"
if (file_name != NULL) fclose(source_file);

#line 418 "tng.md"

  return 0;
}

#line 656 "tng.md"
void reassemble(char prefix, char *name, int indent)
{

  val_t c;
  char *b;
  c = getbuffer(prefix,name);
  if (!valisnil(c)) {
#line 727 "tng.md"
if (count_out_recur++ > 10) throw(EX_INFINITELOOP,name,prefix);

#line 664 "tng.md"
    b = valtostring(c);
    int n = 1;
    int new_indent;
    char *new_name;
    while (*b) {

      if (*b == '\x1F') b++;
      else if (n && indent > 0) { 
        fprintf(out_file,"%*.s",indent," ");
        n = 0; 
      }

      if (*b == '\x1B') {
        new_indent = strtol(b+1,NULL,10);
        b += 10;
        new_name = b;
        while (*b && *b != '\n') b++;
  
        reassemble('B',new_name,new_indent);
        reassemble('A',new_name,new_indent);

      }
      else {
        fputc(*b,out_file);
      }

      n = (*b == '\n');
      b++;
    }
#line 744 "tng.md"
count_out_recur--;
#line 694 "tng.md"
  }
}

#line 112 "tng.md"

int main(int argc, char *argv[]) {

#line 196 "tng.md"
code_chunk = bufnew();
if (valisnil(code_chunk)) throw(EX_OUTOFMEM,"");

#line 210 "tng.md"
chunks = vecnew();
if (valisnil(chunks)) throw(EX_OUTOFMEM,"");

#line 329 "tng.md"
out_file = stdout;
out_filename = NULL;

#line 379 "tng.md"
linebuf = bufnew();
_dbgtrc("lnbuf: %p",(void *)buf(linebuf));
if (valisnil(linebuf)) throw(EX_OUTOFMEM, "");

#line 116 "tng.md"
#line 781 "tng.md"
vrgcli("version 1.0.001 (c) by Remo Dentato") {
  vrgarg("-n, --nolinenums\tNo line numbers") {
    nolinenums = 1;
  }

#line 333 "tng.md"
vrgarg("-o, --outfile outfilename\tThe output file (defaults to stdout)") {
  out_filename = vrgarg;
  out_file = fopen(out_filename,"wb");
  if (out_file == NULL) vrgerror("Unable to write on '%s'\n",out_filename);
}

#line 787 "tng.md"

#line 169 "tng.md"
vrgarg("[filename ...]\tThe files to be processed. (defaults to stdin)") {
  filelist = &argv[vrgargn-1];
  break;
}
#line 789 "tng.md"
  
#line 512 "tng.md"
vrgarg("-i, --indent\tKeep indentation") {
  global_indent = 1;
}

#line 791 "tng.md"

  vrgarg("-h, --help\tHelp") {
    vrghelp();
    exit(1);
  }

  vrgarg() {
    if (vrgarg[0] == '-') vrgerror("Unknown option '%s'\n",vrgarg);
  }
}

#line 117 "tng.md"

  try {
    _dbgtrc("About to parse");
#line 356 "tng.md"
if (filelist == NULL) {
  parsefile(NULL);
} else {
  char **fname = filelist;
  while (*fname) {
    parsefile(*fname);
    fname++;
  }
}

#line 121 "tng.md"
    _dbgtrc("Parsed");
#line 652 "tng.md"
// Start from the main code chunk
reassemble('C',"",0);

#line 123 "tng.md"
  }
#line 851 "tng.md"
catch(EX_FILENOTFOUND) {
  err("File not found: '%s'",exception.msg);
}
catch(EX_FILEEMPTY) {
  err("File empty: '%s'",exception.msg);
}
catch(EX_NOFILEBUFFER) {
  err("Unable to create a buffer for: '%s'",exception.msg);
}
catch(EX_OUTOFMEM) {
  err("Out of memory");
}
catch(EX_SYNTAXERR) {
  err("Unterminated tag %s:%d",out_filename,linenum);
}
catch(EX_DUPLICATEBUF) {
  err("Duplicate file buffer %s",exception.msg);
}
#line 730 "tng.md"
catch(EX_INFINITELOOP) {
  char *tag="";
  if (exception.aux == 'A') tag="after";
  else if (exception.aux == 'B') tag="before";
  char *e=exception.msg;
  while (*e && *e != '\n') e++;
  while (e>exception.msg && isspace(e[-1])) e--;
  err("Infinite recursion while expanding: (%s:%.*s)",tag,(int)(e-exception.msg),exception.msg);
}
#line 870 "tng.md"

#line 125 "tng.md"
  catch() {
    err("Unexpected error");
  }

#line 200 "tng.md"
code_chunk = buffree(code_chunk);
#line 214 "tng.md"
chunks = vecfree(chunks);
bufclearstore(); // free up the stored keys.
#line 340 "tng.md"
if (out_filename) fclose(out_file);


#line 384 "tng.md"
linebuf = buffree(linebuf);

#line 130 "tng.md"

  exit(0);
}
#line 146 "tng.md"
#line 168 "tng.md"
#line 192 "tng.md"
#line 206 "tng.md"
#line 227 "tng.md"
#line 279 "tng.md"
#line 324 "tng.md"
#line 355 "tng.md"
#line 374 "tng.md"
#line 399 "tng.md"
#line 439 "tng.md"
#line 587 "tng.md"
#line 651 "tng.md"
#line 726 "tng.md"
#line 743 "tng.md"
#line 757 "tng.md"
#line 768 "tng.md"
#line 808 "tng.md"
#line 826 "tng.md"
#line 839 "tng.md"
#line 877 "tng.md"
