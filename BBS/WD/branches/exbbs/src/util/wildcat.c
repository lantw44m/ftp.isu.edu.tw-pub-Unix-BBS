#include "bbs.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#ifdef  HAVE_MMAP
#include <sys/mman.h>
#else
int usernumber;
#endif
struct UCACHE *uidshm;
#if defined(_BBS_UTIL_C_)
void 
setapath(buf, boardname) 
  char *buf, *boardname;
{
  sprintf(buf, "man/boards/%s", boardname);
}
char *str_dotdir = ".DIR";
void
setadir(buf, path)
  char *buf, *path;
{
  sprintf(buf, "%s/%s", path, str_dotdir);
}
#endif

static void
attach_err(shmkey, name)
  int shmkey;
  char *name;
{
  fprintf(stderr, "[%s error] key = %x\n", name, shmkey);
  exit(1);
}


static void *
attach_shm(shmkey, shmsize)
  int shmkey, shmsize;
{
  void *shmptr;
  int shmid;

  shmid = shmget(shmkey, shmsize, 0);
  if (shmid < 0)
  {
    shmid = shmget(shmkey, shmsize, IPC_CREAT | 0600);
    if (shmid < 0)
      attach_err(shmkey, "shmget");
    shmptr = (void *) shmat(shmid, NULL, 0);
    if (shmptr == (void *) -1)
      attach_err(shmkey, "shmat");
    memset(shmptr, 0, shmsize);
  }
  else
  {
    shmptr = (void *) shmat(shmid, NULL, 0);
    if (shmptr == (void *) -1)
      attach_err(shmkey, "shmat");
  }
  return shmptr;
}

/*-------------------------------------------------------*/
/* .PASSWDS cache                                        */
/*-------------------------------------------------------*/


#ifndef HAVE_MMAP
static int
fillucache(uentp)
  userec *uentp;
{
  if (usernumber < MAXUSERS)
  {
    strncpy(uidshm->userid[usernumber], uentp->userid, IDLEN + 1);
    uidshm->userid[usernumber++][IDLEN] = '\0';
  }
  return 0;
}
#endif


/* -------------------------------------- */
/* (1) 系統啟動後，第一個 BBS user 剛進來 */
/* (2) .PASSWDS 更新時                    */
/* -------------------------------------- */
reload_ucache()
{
   if (uidshm->busystate)
   {
     /* ------------------------------------------ */
     /* 其他 user 正在 flushing ucache ==> CSMA/CD */
     /* ------------------------------------------ */

     if (uidshm->touchtime - uidshm->uptime > 30)
     {
       uidshm->busystate = 0;  /* leave busy state */
     }
     else
       sleep(1);
   }
   else
   {
     uidshm->busystate = 1;    /* enter busy state */
#ifdef  HAVE_MMAP
     {
       register int fd, usernumber;

       usernumber = 0;

       if ((fd = open(".PASSWDS", O_RDONLY)) > 0)
       {
         caddr_t fimage, mimage;
         struct stat stbuf;

         fstat(fd, &stbuf);
         fimage = mmap(NULL, stbuf.st_size, PROT_READ, MAP_SHARED, fd, 0);
         if (fimage == (char *) -1)
           exit(-1);
         close(fd);
         fd = stbuf.st_size / sizeof(userec);
         if (fd > MAXUSERS)
           fd = MAXUSERS;
         for (mimage = fimage; usernumber < fd; mimage += sizeof(userec))
         {
           memcpy(uidshm->userid[usernumber++], mimage, IDLEN);
         }
         munmap(fimage, stbuf.st_size);
       }
       uidshm->number = usernumber;
     }
#else
     usernumber = 0;
     rec_apply(".PASSWDS", fillucache, sizeof(userec));
     uidshm->number = usernumber;
#endif

     /* 等 user 資料更新後再設定 uptime */
     uidshm->uptime = uidshm->touchtime;

#if !defined(_BBS_UTIL_C_)
     log_usies("CACHE", "reload ucache");
#endif
     uidshm->busystate = 0;    /* leave busy state */
   }
}


void
resolve_ucache()
{
  if (uidshm == NULL)
  {
    uidshm = attach_shm(UIDSHM_KEY, sizeof(*uidshm));
    if (uidshm->touchtime == 0)
      uidshm->touchtime = 1;
  }

  while (uidshm->uptime < uidshm->touchtime)
     reload_ucache();
}

int
ci_strcmp(s1, s2)
  register char *s1, *s2;
{
  register int c1, c2, diff;

  do
  {
    c1 = *s1++;
    c2 = *s2++;
    if (c1 >= 'A' && c1 <= 'Z')
      c1 |= 32;
    if (c2 >= 'A' && c2 <= 'Z')
      c2 |= 32;
    if (diff = c1 - c2)
      return (diff);
  } while (c1);
  return 0;
}


int
searchuser(userid)
  char *userid;
{
  register char *ptr;
  register int i, j;

  resolve_ucache();
  i = 0;
  j = uidshm->number;
  while (i < j)
  {
    ptr = uidshm->userid[i++];
    if (!ci_strcmp(ptr, userid))
    {
      strcpy(userid, ptr);
      return i;
    }
  }
  return 0;
}

/*-------------------------------------------------------*/
/* .BOARDS cache                                         */
/*-------------------------------------------------------*/

struct BCACHE *brdshm;
boardheader *bcache;
int numboards = -1;

/* force re-caching */

void
touch_boards()
{
  time(&(brdshm->touchtime));
  numboards = -1;
}


reload_bcache()
{
   if (brdshm->busystate)
   {
     sleep(1);
   }
   else
   {
     int fd;

     brdshm->busystate = 1;
     if ((fd = open(".BOARDS", O_RDONLY)) > 0)
     {
       brdshm->number = read(fd, bcache, MAXBOARD * sizeof(boardheader))
         / sizeof(boardheader);
       close(fd);
     }

     /* 等所有 boards 資料更新後再設定 uptime */

     brdshm->uptime = brdshm->touchtime;
#if !defined(_BBS_UTIL_C_)
     log_usies("CACHE", "reload bcache");
#endif
     brdshm->busystate = 0;
   }
}

void
resolve_boards()
{
  if (brdshm == NULL)
  {
    brdshm = attach_shm(BRDSHM_KEY, sizeof(*brdshm));
    if (brdshm->touchtime == 0)
      brdshm->touchtime = 1;
    bcache = brdshm->bcache;
  }

  while (brdshm->uptime < brdshm->touchtime)
     reload_bcache();
  numboards = brdshm->number;
}

