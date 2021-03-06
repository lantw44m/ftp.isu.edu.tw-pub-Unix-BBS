/*-------------------------------------------------------*/
/* util/query.c        ( NTHU CS MapleBBS Ver 2.36.sob ) */
/*-------------------------------------------------------*/
/* target : 站外 query 使用者                            */
/* create : 96/11/27                                     */
/* update : 隨時更新                                     */
/*-------------------------------------------------------*/
/* syntax : query <userid>                               */
/*-------------------------------------------------------*/

#include <stdio.h>
#include "bbs.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#define _MODES_C_

char buf[256];
char *ptr;
userec uec;
fileheader fh;
struct UTMPFILE *utmpshm;
user_info *ushm_head, *ushm_tail;


static void
attach_err(shmkey, name)
  int shmkey;
  char *name;
{
  char buf[80];

  sprintf(buf, "error, key = %x", shmkey);
/*  logit(name, buf);*/
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

resolve_utmp()
{
  if (utmpshm == NULL)
  {
    register struct UTMPFILE *ushm;
    register user_info *uentp;

    utmpshm = ushm = attach_shm(UTMPSHM_KEY, sizeof(*utmpshm));

    ushm_head = uentp = ushm->uinfo;
    ushm_tail = uentp + USHM_SIZE;
  }
}


char *
Cdate(clock)
  time_t *clock;
{
  static char foo[22];
  struct tm *mytm = localtime(clock);

  strftime(foo, 22, "%D %T %a", mytm);
  return (foo);
}

int
chkmail(userid)
char *userid;
{
    FILE *mail;

    sprintf(buf, BBSHOME"/home/%s/.DIR", userid);
    if( access(buf,F_OK) ) /* .DIR 不存在 */
        return 0;
    else
        if( (mail=fopen(buf,"r")) == NULL){
            perror("fopen:.DIR");
            exit(1);
        }
        else
            while( fread(&fh,sizeof(fileheader),1,mail) > 0)
                ;
            if( fh.filemode & FILE_READ )
                return 0;
            else
                return 1;
}

void
showuser(uec)
userec *uec;
{
    register int i=0;
    FILE *plans;
    char *modestr;
    user_info *uentp, *utail;

    printf("[1;33;44m                         批踢踢實業坊                     [m\n");

    printf("%s %s(%s) 共上站 %d 次，發表過 %d 篇文章\n",
    (uec->userlevel & PERM_LOGINOK) ? "㊣" : "？",
   uec->userid, uec->username, uec->numlogins, uec->numposts);
    ptr=uec->lasthost;
    if( strchr(ptr,'@') ){
        ptr=index(uec->lasthost,'@')+1;
    }
    printf("最近(%s)從[%s]上站\n", Cdate(&(uec->lastlogin)),ptr );

    modestr = "不在站上";

    uentp = ushm_head;
    utail = ushm_tail;
    do
    {
      if (!strcmp(uec->userid, uentp->userid))
      {
        if (!PERM_HIDE(uentp))
//           modestr = ModeTypeTable[uentp->mode];
        break;
      }
    } while (++uentp <= ushm_tail);


    printf("[1;33m[目前動態：%s][m  %s\n",
      modestr, chkmail(uec->userid) ? "有新進信件還沒看":"所有信件都看過了");

    sprintf(buf,BBSHOME"/home/%s/plans", uec->userid);
    if( !(i=access(buf, F_OK)) ){
        printf("%s 的名片:\n", uec->userid);
        if( (plans=fopen(buf,"r")) == NULL ){
            perror("fopen:plans");
            exit(1);
        }
        while (i++ < MAXQUERYLINES && fgets(buf,256,plans))
            printf("%s",buf);
       printf("",buf);
    }
    else
        printf("%s 目前沒有名片\n", uec->userid);

}

int
main(argc,argv)
   int argc;
   char **argv;
{
//    char *c;
    FILE *pwd;
//    int len;

    if (argc != 2){
        fprintf(stderr,"Wrong argument!\n");
        exit(1);
    }

    if( (pwd=fopen(BBSHOME"/.PASSWDS","r")) == NULL){
        perror("fopen");
        exit(1);
    }

    resolve_utmp();

    while( fread( &uec, sizeof(userec), 1, pwd) > 0 )
    {
        if( strcmp(uec.userid,argv[1]))
        {
            continue;
        }
        else
            showuser(&uec);
        exit(0);
    }
    fprintf(stderr,"Can not find user %s\n\n",argv[1]);
    exit(1);
}

