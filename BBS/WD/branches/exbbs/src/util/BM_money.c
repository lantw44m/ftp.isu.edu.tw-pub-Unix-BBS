#include "bbs.h"
#include "record.c"
#include "cache.c"
#include <math.h>
#include "bbs_util.h"

#define pwdfile fn_passwds
#define logfile BBSHOME"/log/counter/bm_pay"

extern int numboards;
extern boardheader *bcache;
extern struct UCACHE *uidshm;
int c,n,nb=0;
userec xuser;

struct boardinfo
{
  char bname[IDLEN+1];
  char title[BTLEN + 1];
  char BM[IDLEN * 3 + 3];
  int  money;
};
typedef struct boardinfo boardinfo;

boardinfo
board[MAXBOARD];

int k_cmp(b, a)
boardinfo *b,*a;
{
//  return ((a->k/100 + a->ndir + a->nfile) - (b->k/100 + b->ndir + b->nfile));
  return (a->money - b->money);
}

#define FUNCTION ( Gold-(Step*c) > 10 ? Gold-(Step*c) : 10 )

int getuser(userid)
  char *userid;
{
  int uid;
  if (uid = searchuser(userid))
   {
    rec_get(pwdfile, &xuser, sizeof(xuser), uid);
   }
  return uid;
}

int inumoney(char *tuser,int money, char mode[10])
{
  int unum;
  time_t now;
  time(&now);
  if (unum = getuser(tuser))
      if (((now - xuser.lastlogin)/60/60/24) < 7)
      {
         if (strcmp(mode,"+g")==0)
         {
            xuser.goldmoney += money;
            substitute_record(pwdfile, &xuser, sizeof(userec), unum);
         }
         else if (strcmp(mode,"-g")==0)
         {
            xuser.goldmoney -= money;
            substitute_record(pwdfile, &xuser, sizeof(userec), unum);
         }
         return 1;
      }  
  return -1;
}

int
 main(int argc, char **argv)
{

  FILE *fp=fopen(BBSHOME "/log/mandex.log","r");
  char buf[201],bname[20],BM[90],*ch, mode[10];
//  fileheader mymail; char genbuf[256];
  boardheader *bptr;
  int nBM,Gold,Step,money,total=0;
  
  resolve_boards();
  if(!fp) return ;

  if (argc > 1)
     strcpy(mode, argv[1]);
  else
     strcpy(mode, "nothing");

  if (argc > 2)
     Gold = atoi(argv[2]);
  else
     Gold = 49;
     
  if (argc > 3)
     Step = atoi(argv[3]);
  else
     Step = 1;

  nb=0;
  while(fgets(buf,200,fp)!=NULL)
  {
    sscanf(buf,"%s %d",&bname,&money);
    for (n = 0; n < numboards; n++)
    {
       bptr = &bcache[n];
       if(!strcmp(bptr->brdname, bname)) break;
    }
    if(n== numboards) continue;
    if(bptr->brdattr & BRD_GROUPBOARD || bptr->brdattr & BRD_CLASS)
       continue;
    strcpy(BM,bptr->BM);
    if(BM[0] == 0 || BM[0] == ' ' || strstr(BM, "¼x¨D"))
       continue;
       
    strcpy(board[nb].bname,bname);
    strcpy(board[nb].BM, bptr->BM);
    strcpy(board[nb].title, bptr->title);
    money=sqrt(money);
    if(!(bptr->brdattr & BRD_PERSONAL))
      money = money + ((money > 30) ? 30 : money);
    if(bptr->brdattr & BRD_OUTGOING && bptr->brdattr & BRD_INGOING)
      money = money + 30;
    board[nb].money = money + 30;
    if(!belong(BBSHOME"/dec/nobmmoney", bname)) nb++;
  }
  qsort(board, nb, sizeof(boardinfo), k_cmp);
     
  printf("[1;37;42m¦W¦¸. ¬ÝªO         ¬ÝªO ¼ÐÃD                    "
         "     Á~¤ô ªO¥D                [40m\n");

  total=0;
  for(c=0 ; c<nb ; c++)
  {
    strcpy(BM,board[c].BM);
    money=board[c].money;
    printf("[1;36;40m%4d. %-13.13s[37m%-32.32s",c+1,
      board[c].bname, board[c].title);
    printf("[33m%6d [32m%s\n",money,BM);
    ch=BM;
    for(nBM=1;(ch = strchr(ch,'/'))!=NULL;nBM++) {ch++;};
    ch=BM;
        for(n=0;n<nBM;n++)
        {
           char *ch1;
           if(ch1 = strchr(ch,'/'))
              *ch1 = 0;
           total=total+money;
           if(inumoney(ch, money, mode) != -1)
           {
              if (strcmp(mode,"+g")==0)
              {
              /*
                sprintf(genbuf,BBSHOME"/home/%s", ch);
                stampfile(genbuf, &mymail);
                strcpy(mymail.owner, "[Á~¤ô³U]");
                sprintf(mymail.title,"[32m %s [m ¬ÝªOªºÁ~¤ô ¢C[33m%d[37m",
                bname,money);
                mymail.savemode = 0;
                unlink(genbuf);
                link(BBSHOME "/etc/BM_money", genbuf);
                sprintf(genbuf,BBSHOME"/home/%s/.DIR", ch);
                rec_add(genbuf, &mymail, sizeof(mymail));
                */
              }
           }
           ch=ch1+1;
        }
    }

  printf("                                              "
         "[1;33mtotal=%5d[m\n",total);

  if(strcmp(mode,"+g")==0)
  {
    money=0;
    fp = fopen(logfile, "r");

    if (fp != NULL)
    {
       fscanf(fp,"%10d",&money);
       fclose(fp);
    }
    money = money + total;

    fp = fopen(logfile,"w");
    fprintf(fp,"%d",money);
    fclose(fp);
  }  
  return 1;  
}
