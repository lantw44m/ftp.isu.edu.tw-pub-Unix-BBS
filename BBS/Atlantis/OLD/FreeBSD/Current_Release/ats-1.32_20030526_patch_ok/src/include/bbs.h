/*-------------------------------------------------------*/
/* bbs.h        ( NTHU CS MapleBBS Ver 2.36 )            */
/*-------------------------------------------------------*/
/* target : all header files                             */
/* create : 95/03/29                                     */
/* update : 95/12/15                                     */
/*-------------------------------------------------------*/

#ifndef _BBS_H_
#define _BBS_H_

/*
woju
*/
#define PAUSE { outmsg("^[[37;45;1m                        ¡´ ½Ð«ö ^[[33m(OK)^[[37m Ä~Äò ¡´ ^[[0m");igetkey();}

#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/param.h>

#ifdef SunOS
#include <strings.h>
#endif

#ifdef  LINUX
#include <bsd/sgtty.h>
#else
#include <sgtty.h>
#endif


#ifdef  SYSV

#ifndef LOCK_EX
#define LOCK_EX         F_LOCK
#define LOCK_UN         F_ULOCK
#endif

#define getdtablesize()         (64)

#define usleep(usec)            {               \
    struct timeval t;                           \
    t.tv_sec = usec / 1000000;                  \
    t.tv_usec = usec % 1000000;                 \
    select( 0, NULL, NULL, NULL, &t);           \
}

#else                           /* SYSV */
   #ifndef MIN
      #define   MIN(a,b)        ((a<b)?a:b)
   #endif
   #ifndef MAX
      #define   MAX(a,b)        ((a>b)?a:b)
   #endif
#endif                          /* SYSV */


#define YEA (1)                 /* Booleans  (Yep, for true and false) */
#define NA  (0)

#define NOECHO (0)
#define DOECHO (1)              /* Flags to getdata input function */
#define LCECHO (2)

#define I_TIMEOUT   (-2)        /* Used for the getchar routine select call */
#define I_OTHERDATA (-333)      /* interface, (-3) will conflict with chinese */

/* static char station_name[16]; */
/* static char station_name[16] = { "atlantis" }; */

#include "config.h"             /* User-configurable stuff */
#include "perm.h"               /* user/board permission */
#include "struct.h"             /* data structure */
#include "global.h"             /* global variable & definition */
#include "modes.h"              /* The list of valid user modes */
#include "kaede.h"              /* Kaede's functions */
#endif                          /* _BBS_H_ */
