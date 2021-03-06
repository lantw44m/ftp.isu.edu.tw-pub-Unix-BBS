/*-------------------------------------------------------*/
/* util/account.c       ( NTHU CS MapleBBS Ver 2.36 )    */
/*-------------------------------------------------------*/
/* target : 上站人次統計、系統資料備份                   */
/* create : 95/03/29                                     */
/* update : 95/12/15                                     */
/*-------------------------------------------------------*/
#include "bbs.h"
#include "bbs_util.h"

#include <time.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "record.c"

#define MAX_LINE        16
#define ADJUST_M        6       /* adjust back 5 minutes */

struct UCACHE *uidshm;
struct FILMCACHE *ptt;
struct FROMCACHE *fcache;

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
void
resolve_ucache()
{
  if (uidshm == NULL)
  {
    uidshm = attach_shm(UIDSHM_KEY, sizeof(*uidshm));
  }
}
void
reset_garbage()
{
  if (ptt == NULL)
  {
    ptt = attach_shm(FILMSHM_KEY, sizeof(*ptt));
    if (ptt->touchtime == 0)
      ptt->touchtime = 1;
  }
  ptt->uptime =0;
}

void
resolve_fcache()
{
  if (fcache == NULL)
  {
    fcache = attach_shm(FROMSHM_KEY, sizeof(*fcache));
    if (fcache->touchtime == 0)
      fcache->touchtime = 1;
  }
  fcache->uptime =0;
}

void
keeplog(fpath, board, title)
char *fpath;
char *board;
char *title;
{
  fileheader fhdr;
  char genbuf[256];

  if(dashf(fpath)<1)
    return;

  if(!board)
    board = "Security";

  printf("%s\n",title);
  
  sprintf(genbuf, "boards/%s", board);
  stampfile(genbuf, &fhdr);
  fhdr.savemode = fhdr.filemode = 0;
  rename(fpath, genbuf);

  strcpy(fhdr.title, title);
  strcpy(fhdr.owner, "[歷史軌跡]");
  sprintf(genbuf, "boards/%s/.DIR", board);
  rec_add(genbuf, &fhdr, sizeof(fhdr));
  return;
}


void
outs(fp, buf, mode)
  FILE *fp;
  char buf[], mode;
{
  static char state = '0';

  if (state != mode)
    fprintf(fp, "[3%cm", state = mode);
  if (buf[0])
  {
    fprintf(fp, buf);
    buf[0] = 0;
  }
}


void
gzip(source, target, stamp)
  char *source, *target, *stamp;
{
  char buf[128];
  sprintf(buf, "/bin/gzip -9n adm/%s%s", target, stamp);
  rename(source, &buf[14]);
//  system(buf);
}

main()
{
  int hour, max, item, total, i,
       j,mo,da,max_user=0,max_login=0,max_reg=0,mahour,k;
  char *act_file = ".act";
  char *log_file = "usies";
  char buf[256],buf1[256], *p;
  FILE *fp,*fp1;
  int act[27];                  /* 次數/累計時間/pointer */
  time_t now;
  struct tm *ptime;
  chdir(BBSHOME);
  now = time(NULL) - ADJUST_M * 60;     /* back to ancent */
  ptime = localtime(&now);

  memset(act, 0, sizeof(act));
  
//  printf("count...\n");
  
  if ((ptime->tm_hour != 0) && (fp = fopen(act_file, "r")))
  {
    fread(act, sizeof(act), 1, fp);
    fclose(fp);
  }
  if ((fp = fopen(log_file, "r")) == NULL)
  {
    printf("cann't open usies\n");
    return 1;
  }
  if (act[26])
    fseek(fp, (off_t)(act[26]), 0);
    
  while (fgets(buf, 256, fp))
  {
    hour = atoi(buf + 9);
    if (hour < 0 || hour > 23)
    {
      continue;
    }
    if (!strncmp(buf + 22, "ENTER", 5))
    {
      act[hour]++;
      continue;
    }
    if (p = (char *) strstr(buf + 40, "Stay:"))
    {
      if (hour = atoi(p + 5))
      {
        act[24] += hour;
        act[25]++;
      }
      continue;
    }
  }
  act[26] = ftell(fp);
  fclose(fp);
  
  for (i = max = total = 0; i < 24; i++)
  {
    total += act[i];
    if (act[i] > max)
     {
      max_user = max = act[i];
      mahour = i;
     }
  }
  item = max / MAX_LINE + 1;

  if (!ptime->tm_hour)
    keeplog("etc/today", "Record", "[塵埃紀錄] 上站人次統計");

  if ((fp = fopen("etc/today", "w")) == NULL)
  {
    printf("cann't open etc/today\n");
    return 1;
  }
  
  fprintf(fp, "\t\t\t[1m[33;46m 每小時上站人次統計 [%02d/%02d/%02d] "
    "[40m\n\n", ptime->tm_year, ptime->tm_mon + 1, ptime->tm_mday);
  
  for (i = MAX_LINE + 1; i > 0; i--)
  {
    strcpy(buf, "   ");
    for (j = 0; j < 24; j++)
    {
      max = item * i;
      hour = act[j];
      if (hour && (max > hour) && (max - item <= hour))
      {
        outs(fp, buf, '7');
        fprintf(fp, "%-3d", hour);
      }
      else if (max <= hour)
      {
        outs(fp, buf, '6');
        fprintf(fp, "█ ");
      }
      else
        strcat(buf, "   ");
    }
    fprintf(fp, "\n");
  }
  
  fprintf(fp, "   [33m"
  "0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19 20 21 22 23\n\n"
    "\t[33m總共上站人次：[37m%-9d[33m平均使用人數："
    "[37m%-5d[33m平均使用時間：[37m %d [33m分[m \n"
    ,total, total/24, (int) (act[24]/act[25]) +1);
  fclose(fp);

  if (fp = fopen(act_file, "w"))
  {
    fwrite(act, sizeof(act), 1, fp);
    fclose(fp);
  }

  /* -------------------------------------------------------------- */

    sprintf(buf, "-%02d%02d%02d",
      ptime->tm_year, ptime->tm_mon + 1, ptime->tm_mday);

  now += ADJUST_M * 60;         /* back to future */



   /* Ptt 歷史事件處理 */
    if(fp = fopen("etc/history.data","r"))
     {                                  /*最多同時上線*/
      if(fscanf(fp,"%d %d %d %d",&max_login,&max,&max_reg,&k))
        {
	 int a;
         resolve_fcache();
         printf("此時段最多同時上線:%d 過去:%d",a=fcache->max_user,k);
         fclose(fp);
         if(a > k)
                {
                  ptime=localtime(&fcache->max_time);
                  if(fp1=fopen("etc/history", "a"))
                    {
                      fprintf(fp1,
                      "◎ 【%02d/%02d/%02d %02d:%02d】"
                      "[32m同時在站內人數[m首次達到 [1;36m%d[m 人次\n",
                        ptime->tm_mon+1, ptime->tm_mday, ptime->tm_year,
                        ptime->tm_hour,ptime->tm_min,a);
                      fclose(fp1);
                    }
                   if(fp=fopen("etc/history.data","w"))
                    {
                     fprintf(fp,"%d %d %d %d",max_login,max,max_reg,a);
                    }
                }
        }
        fclose(fp);
     }
  ptime = localtime(&now);

if(ptime->tm_hour)
  {
  printf("多個節日處理\n");
   /* Ptt 多個節日處理 */

      if(fp1 = fopen("etc/today_is","r"));
        {
          char tod[20][100];
          for(i=0;fgets(tod[i],20,fp1) && i<100;i++);
          fclose(fp1);

          fp1 = fopen("etc/today_is","w");
          for(j=1;j<=i;j++)
             {
               fputs(tod[j%i],fp1);
             }
          fclose(fp1);
        }
  /* Ptt reset Ptt's share memory */
  reset_garbage();
  }

if(!ptime->tm_hour)
  {
//    keeplog("log/trade.log", "Security","[金融中心] 商品販賣狀況");
    keeplog("usies",         "System_Log", "[系統日誌] 使用者上線紀錄");
    keeplog("log/bank.log",  "Security", "[金融中心] 金錢流動紀錄");
    keeplog("log/admin.log", "Security", "[系統日誌] 系統狀態報告");
    keeplog("log/board.log", "Security", "[系統日誌] 看板狀態報告");
    keeplog("log/board_edit.log","Security","[系統日誌] 板主指令報告");

    keeplog(".note",          "Record", "[塵埃紀錄] 心情留言版");
    keeplog("etc/topgold",    "Record", "[塵埃記錄] 金幣收集排行榜");
    keeplog("etc/topmoney",   "Record", "[塵埃記錄] 銀幣收集排行榜");
    keeplog("etc/toprpg",     "Record", "[塵埃記錄] ＲＰＧ排行榜");
    keeplog("log/gnp.log",    "Record", "[金融中心] 生產毛額統計");
    
    keeplog("log/pip.log",    "Record", "[遊樂場] 電子雞養育記錄");
    keeplog("log/chessmj.log","Record", "[遊樂場] 象棋麻將記錄");
    keeplog("log/marie.log",  "Record", "[遊樂場] 小瑪麗記錄");
    keeplog("log/bj.log",     "Record", "[遊樂場] 黑傑克記錄");
    keeplog("log/gp.log",   "Record", "[遊樂場] 金撲克記錄");
    keeplog("log/ab.log",   "Record", "[遊樂場] 猜數字記錄");
    keeplog("log/seven.log","Record", "[遊樂場] 賭城七張記錄");
    keeplog("log/dice.log", "Record", "[遊樂場] 西巴啦記錄");
    keeplog("log/nine.log", "Record", "[遊樂場] 九九記錄");
    keeplog("log/fightnum.log","Record", "[遊樂場] 對戰猜數字記錄");
    keeplog("log/bingo.log",   "Record", "[遊樂場] 賓果園記錄");
    keeplog("log/race.log",    "Record", "[遊樂場] 賽豬場記錄");
//    keeplog("log/five.log",  "Record", "[遊樂場] 五子棋對戰記錄");

    system("/bin/cp etc/today etc/yesterday");
    system("rm -f note.dat");
    gzip(log_file, "usies", buf);

/* Ptt 歷史事件處理 */
    now = time(NULL) - ADJUST_M * 60;     /* back to ancent */
    ptime = localtime(&now);

    resolve_ucache();
    if(fp = fopen("etc/history.data","r"))
     {                                  /* 單日最多次人次,同時上線,註冊*/
      if(fscanf(fp,"%d %d %d %d",&max_login,&max,&max_reg,&k))
       {
        fp1 = fopen("etc/history","r+");
        fseek(fp1,(off_t)0,2);
        if ( max_user > max )
          {
      fprintf(fp1,"◇ 【%02d/%02d/%02d %02d】   "
            "[1;32m單一小時上線人次[m首次達到 [1;35m%d[m 人次 \n"
             ,ptime->tm_mon+1,ptime->tm_mday,ptime->tm_year,mahour,max_user);
             max = max_user;
          }
        if ( total > max_login )
          {
      fprintf(fp1,"◆ 【%02d/%02d/%02d】      "
            "[1;32m單日上線人次[m首次達到[1;33m %d[m 人次   \n"
             ,ptime->tm_mon+1,ptime->tm_mday,ptime->tm_year,total);
              max_login = total;
          }

        if ( uidshm->number > max_reg + max_reg/10)
          {
      fprintf(fp1,"★ 【%02d/%02d/%02d】      "
            "[1;32m總註冊人數[m提升到[1;31m %d[m 人   \n"
             ,ptime->tm_mon+1,ptime->tm_mday,ptime->tm_year,uidshm->number);
              max_reg = uidshm->number;
          }

        fclose(fp1);
       }
      fclose(fp);
      fp = fopen("etc/history.data","w");
      fprintf(fp,"%d %d %d %d",max_login,max,max_reg,k);
      fclose(fp);
     }
  now += ADJUST_M * 60;         /* back to future */
  ptime = localtime(&now);

/* Ptt 節日處理 */
    i=0;
  if(fp1 = fopen("etc/today_is","w"))
    {
     if(fp = fopen("etc/feast","r"))
      {
       while (fscanf(fp,"%d %d %s\n",&mo,&da,buf1) != EOF)
        {
         if(ptime->tm_mday == da && ptime->tm_mon + 1 == mo)
           {
              i =1;
              fprintf(fp1,"%-14s\n",buf1);
           }
       }
      fclose(fp);
     }
   if(i==0)
     {
       if(fp = fopen("etc/today_boring","r"))
       {
         while (fgets(buf1,250,fp))
         {
          if(strlen(buf)>3)
           {
            buf1[strlen(buf1)-1]=0;
            fprintf(fp1,"%-14s\n",buf1);
           }
         }
         fclose(fp);
       }
       else
       {
         fprintf(fp1,"本日節日徵求中");
       }
     }
     fclose(fp1);
    }

/* Ptt 歡迎畫面處理 */
/*
    if(fp = fopen("etc/Welcome.date","r"))
    {
     char temp[50];
      while (fscanf(fp,"%d %d %s\n",&mo,&da,buf1) != EOF)
      {
        if(ptime->tm_mday == da && ptime->tm_mon + 1 == mo)
          {
                strcpy(temp,buf1);
                sprintf(buf1,"cp -f etc/Welcomes/%s etc/Welcome",temp);
                system(buf1);
                break;
          }
      }
     fclose(fp);
    }
*/
    if (ptime->tm_wday == 0)
    {
      keeplog("log/week", "Record", "[塵埃紀錄] 本週熱門話題");

//      gzip("bbslog", "bntplink", buf);
//      gzip("innd/bbslog", "innbbsd", buf);
//      gzip("etc/mailog", "mailog", buf);
    }

    if (ptime->tm_mday == 1)
    {
      keeplog("log/month", "Record", "[塵埃紀錄] 本月熱門話題");
      keeplog("etc/topboard", "Record", "[塵埃紀錄] 本月看板排行");
    }
    if (ptime->tm_yday == 1)
      keeplog("log/year", "Record", "[塵埃紀錄] 年度熱門話題");
  }
  else if (ptime->tm_hour == 3 && ptime->tm_wday == 6)
  {
    char *fn1 = "tmp";
    char *fn2 = "suicide";
    rename(fn1, fn2);
    mkdir(fn1, 0755);
    sprintf(buf, "/bin/gtar cfz adm/%s-%02d%02d%02d.tgz %s",
      fn2, ptime->tm_year, ptime->tm_mon + 1, ptime->tm_mday, fn2);
//    system(buf);
//    sprintf(buf, "/bin/rm -fr %s", fn2);
//    system(buf);
  }
}
