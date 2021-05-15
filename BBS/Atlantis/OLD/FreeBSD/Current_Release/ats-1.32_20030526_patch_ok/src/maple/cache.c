/*-------------------------------------------------------*/
/* cache.c      ( NTHU CS MapleBBS Ver 2.36 )            */
/*-------------------------------------------------------*/
/* target : cache up data by shared memory               */
/* create : 95/03/29                                     */
/* update : 02/07/19 (Dopin)                             */
/*-------------------------------------------------------*/

#include "bbs.h"
#include <sys/ipc.h>
#include <sys/shm.h>

#ifdef _BBS_UTIL_C_
char *fn_passwd = BBSHOME "/.PASSWDS";
char *fn_board = BBSHOME "/.BOARDS";
#endif

#ifdef  HAVE_MMAP
#include <sys/mman.h>
#else
int usernumber;
#endif


struct UCACHE *uidshm;


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

#if !defined(_BBS_UTIL_C_)
       log_usies("CACHE", "refork token");
#endif
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

       if ((fd = open(fn_passwd, O_RDONLY)) > 0)
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
     apply_record(fn_passwd, fillucache, sizeof(userec));
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

     if ((fd = open(fn_board, O_RDONLY)) > 0)
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



boardheader *
getbcache(bname)
  char *bname;
{
  register int i;
  register boardheader *bhdr;

  resolve_boards();
  for (i = 0, bhdr = bcache; i < numboards; i++, bhdr++)
    /* if (Ben_Perm(bhdr)) */
    if (!ci_strcmp(bname, bhdr->brdname))
      return bhdr;
  return NULL;
}


int
getbnum(bname)
  char *bname;
{
  register int i;
  register boardheader *bhdr;

  resolve_boards();
  for (i = 0, bhdr = bcache; i++ < numboards; bhdr++)
    /* if (Ben_Perm(bhdr)) */
    if (!ci_strcmp(bname, bhdr->brdname))
      return i;
  return 0;
}


/*-------------------------------------------------------*/
/* .UTMP cache                                           */
/*-------------------------------------------------------*/

struct UTMPFILE *utmpshm;
user_info *currutmp;

void
resolve_utmp()
{
  if (utmpshm == NULL)
  {
    utmpshm = attach_shm(UTMPSHM_KEY, sizeof(*utmpshm));
    if (utmpshm->uptime == 0)
      utmpshm->uptime = utmpshm->number = 1;
  }
}




#if !defined(_BBS_UTIL_C_)
int
getuser(userid)
  char *userid;
{
  int uid;

  if (uid = searchuser(userid))
    get_record(fn_passwd, &xuser, sizeof(xuser), uid);
  return uid;
}


char *
getuserid(num)
  int num;
{
  if (--num >= 0 && num < MAXUSERS)
  {
    return ((char *) uidshm->userid[num]);
  }
  return NULL;
}


void
setuserid(num, userid)
  int num;
  char *userid;
{
  if (num > 0 && num <= MAXUSERS)
  {
    if (num > uidshm->number)
      uidshm->number = num;
    strncpy(uidshm->userid[num - 1], userid, IDLEN + 1);
  }
}


int
searchnewuser(mode)
  int mode;

/* 0 ==> 找過期帳號 */
/* 1 ==> 建立新帳號 */
{
  register int i, num;

  resolve_ucache();
  num = uidshm->number;
  i = 0;
  while (i < num)
  {
    if (!uidshm->userid[i++][0])
      return (i);
  }
  if (mode && (num < MAXUSERS))
    return (num + 1);
  return 0;
}


#if 0
int
apply_users(func)
  void (*func) ();
{
  register int i, num;

  resolve_ucache();
  num = uidshm->number;
  for (i = 0; i < num; i++)
    (*func) (uidshm->userid[i], i + 1);
  return 0;
}
#endif


char *
u_namearray(buf, pnum, tag)
  char buf[][IDLEN + 1], *tag;
  int *pnum;
{
  register struct UCACHE *reg_ushm = uidshm;
  register char *ptr, tmp;
  register int n, total;
  char tagbuf[STRLEN];
  int ch, ch2, num;

  resolve_ucache();
  if (*tag == '\0')
  {
    *pnum = reg_ushm->number;
    return reg_ushm->userid[0];
  }
  for (n = 0; tag[n]; n++)
  {
    tagbuf[n] = chartoupper(tag[n]);
  }
  tagbuf[n] = '\0';
  ch = tagbuf[0];
  ch2 = ch - 'A' + 'a';
  total = reg_ushm->number;
  for (n = num = 0; n < total; n++)
  {
    ptr = reg_ushm->userid[n];
    tmp = *ptr;
    if (tmp == ch || tmp == ch2)
    {
      if (chkstr(tag, tagbuf, ptr))
        strcpy(buf[num++], ptr);
    }
  }
  *pnum = num;
  return buf[0];
}


void
setutmpmode(mode)
  int mode;
{
  if (currstat != mode)
    currutmp->mode = currstat = mode;
}


/*
woju
*/
resetutmpent()
{
  extern int errno;
  register time_t now;
  register int i;
  register pid_t pid;
  register user_info *uentp;

  resolve_utmp();
  now = time(NULL) - 79;
  if (now > utmpshm->uptime)
    utmpshm->busystate = 0;

  while (utmpshm->busystate)
  {
    sleep(1);
  }
  utmpshm->busystate = 1;

  /* ------------------------------ */
  /* for 幽靈傳說: 每 79 秒更新一次 */
  /* ------------------------------ */

  for (i = now = 0; i < USHM_SIZE; i++)
  {
    uentp = &(utmpshm->uinfo[i]);
    if (pid = uentp->pid)
    {
       if (pid == 1 || (kill(pid, 0) == -1) && (errno == ESRCH))
           memset(uentp, 0, sizeof(user_info));
        else
           now++;
    }
  }
  utmpshm->number = now;
  time(&utmpshm->uptime);
  utmpshm->busystate = 0;
}


void
getnewutmpent(up)
  user_info *up;
{
  register int i;
  register user_info *uentp;


  resetutmpent();
  while (utmpshm->busystate)
  {
    sleep(1);
  }
  utmpshm->busystate = 1;
  for (i = 0; i < USHM_SIZE; i++)
  {
    uentp = &(utmpshm->uinfo[i]);
    if (uentp->pid < 2)
    {
      memcpy(uentp, up, sizeof(user_info));
      currutmp = uentp;
      utmpshm->number++;
      utmpshm->busystate = 0;
      return;
    }
  }
  utmpshm->busystate = 0;
  sleep(1);
  exit(1);
}


int
apply_ulist(fptr)
  int (*fptr) ();
{
  register user_info *uentp;
  register int i, state;

  resolve_utmp();
  for (i = 0; i < USHM_SIZE; i++)
  {
    uentp = &(utmpshm->uinfo[i]);
    if (uentp->pid && (PERM_HIDE(currutmp) ||
        !(PERM_HIDE(uentp) && !(is_rejected(uentp) & 2))))
      if (state = (*fptr) (uentp))
          return state;
  }
  return 0;
}


user_info *
search_ulist(fptr, farg)
  int (*fptr) ();
int farg;
{
  register int i;
  register user_info *uentp;

  resolve_utmp();
  for (i = 0; i < USHM_SIZE; i++)
  {
    uentp = &(utmpshm->uinfo[i]);
    if ((*fptr) (farg, uentp))
      return uentp;
  }
  return 0;
}


int
count_multi()
{
  register int i, j;
  register user_info *uentp;

  resolve_utmp();
  for (i = j = 0; i < USHM_SIZE; i++)
  {
    uentp = &(utmpshm->uinfo[i]);
    if (uentp->uid == usernum)
      j++;
  }
  return j;
}


/* -------------------- */
/* for multi-login talk */
/* -------------------- */

user_info *
search_ulistn(fptr, farg, unum)
  int (*fptr) ();
int farg;
int unum;
{
  register int i, j;
  register user_info *uentp;

  resolve_utmp();
  for (i = j = 0; i < USHM_SIZE; i++)
  {
    uentp = &(utmpshm->uinfo[i]);
    if ((*fptr) (farg, uentp))
    {
      if (++j == unum)
        return uentp;
    }
  }
  return 0;
}


int
count_logins(fptr, farg, show)
  int (*fptr) ();
int farg;
int show;
{
  register user_info *uentp;
  register int i, j;

  resolve_utmp();
  for (i = j = 0; i < USHM_SIZE; i++)
  {
    uentp = &(utmpshm->uinfo[i]);
    if ((*fptr) (farg, uentp))
    {
      j++;
      if (show)
      {
        prints("(%d) 目前狀態為: %-17.16s(來自 %s)\n", j,
          modestring(uentp, 0), uentp->from);
      }
    }
  }
  return j;
}


void
purge_utmp(uentp)
  user_info *uentp;
{
  memset(uentp, 0, sizeof(user_info));
  if (utmpshm->number)
    utmpshm->number--;
}


int
count_ulist()
{
   int ans = utmpshm->number;
   register user_info *uentp;
   int ch = 0;

   while (ch < USHM_SIZE) {
      uentp = &(utmpshm->uinfo[ch++]);
      if (uentp->pid && (
          is_rejected(uentp) & 2 && !HAS_PERM(PERM_SYSOP) ||
          uentp->invisible && !HAS_PERM(PERM_SEECLOAK) &!HAS_PERM(PERM_SYSOP) ||
          !PERM_HIDE(currutmp) && PERM_HIDE(uentp) ||
          cuser.uflag & FRIEND_FLAG && !is_friend(uentp)
         ))
         --ans;
   }

   return ans;
}



int
apply_boards(func)
  int (*func) ();
{
  register int i;
  register boardheader *bhdr;

  resolve_boards();
  for (i = 0, bhdr = bcache; i < numboards; i++, bhdr++)
  {
    if (Ben_Perm(bhdr))
      if ((*func) (bhdr) == QUIT)
        return QUIT;
  }
  return 0;
}

int haspostperm(char *bname) {
  register int i;
  char buf[200];

  setbfile(buf, bname, fn_water);
  if(belong(buf, cuser.userid)) return 0;

  if(!ci_strcmp(bname, DEFAULT_BOARD)) return 1;

  if(!cuser.userlevel) {
    char *ptr;

    if(belong("etc/anonymous", cuser.userid)) {
      while(ptr = strtok(NULL, str_space)) {
        if(!ci_strcmp(bname, ptr)) return 1;
      }
    }
    return 0;
  }

  if(!HAS_PERM(PERM_POST)) return 0;
  if(!(i = getbnum(bname))) return 0;

  i = bcache[i - 1].level;

  /* 秘密看板特別處理 */
  if(i & PERM_SECRET || i & PERM_ENTERSC) return 1;

  return (HAS_PERM(i & ~PERM_POSTMASK & ~PERM_POST));
}
#endif                          /* !defined(_BBS_UTIL_C_) */
