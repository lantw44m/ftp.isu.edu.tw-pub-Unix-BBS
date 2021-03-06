/******************/
/* 點歌機         */
/* 作者：Ptt      */
/* 修改：wildcat  */
/******************/

#include "bbs.h"

#define MAXSONGS 300
#define SONGPATH BBSHOME"/etc/SONGO"
#define SONGBOOK BBSHOME"/etc/SONGBOOK"
#define KTV BBSHOME"/man/boards/KTV"


typedef
struct songcmp
{
  char name[100];
  char cname[100];
  long int count;
}
songcmp ;

long int totalcount=0;

int
count_cmp(b, a)
 songcmp *a, *b;
{
    return (a->count - b->count);
}
void
topsong()
{
 more(FN_TOPSONG,YEA);
}
     
int strip_blank(char *cbuf, char *buf)
{
  for(;*buf;buf++)  if(*buf != ' ') *cbuf++=*buf;	  
  *cbuf=0;
}
void
sortsong()
{
  FILE *fo,*fp=fopen(BBSHOME "/" FN_USSONG,"r");
  songcmp songs[MAXSONGS + 1];
  int n;
  char buf[256],cbuf[256];

  memset( songs , 0, sizeof(songs));
  if(!fp) return;
  if(!(fo=fopen(FN_TOPSONG,"w"))) {fclose(fp); return;}

  totalcount=0;
  while(fgets(buf,200,fp))
    {
     strtok(buf,"\n");
     strip_blank(cbuf,buf);
     for(n=0;n<MAXSONGS && songs[n].name[0] ;n++)
        {
          if(!strcmp(songs[n].cname,cbuf)) break;
        }
     strcpy(songs[n].name,buf);
     strcpy(songs[n].cname,cbuf);
     songs[n].count++;
     totalcount++;
    }
  qsort(songs, MAXSONGS, sizeof(songcmp), count_cmp);
  fprintf(fo,
"    [1;36m──[37m名次[36m──────[37m歌"
"  名[1;36m───────────[37m次數[36m"
"──[1;32m共%d次[36m──[m\n",totalcount);
  for(n=0;n<100 && songs[n].name[0];n++)
    {
        fprintf(fo,"      %5d. %-38.38s %4d   [1;32m[%.2f][m\n",n+1,
           songs[n].name,songs[n].count,(float) songs[n].count/totalcount);
    }
  fclose(fp);
  fclose(fo);
}

char *onlydate(time_t clock)
{
  static char foo[9];
  struct tm *mytm = localtime(&clock);

  strftime(foo, 9, "%D", mytm);
  return (foo);
}

int
ordersong()
{
  char destid[IDLEN+1],buf[256],genbuf[256],filename[256],say[51];
  char receiver[60];
  FILE *fp,*fp1;
  fileheader mail;
  time_t now=time(NULL);
  int lines = 0;

  if(answer("是否要預先瀏覽歌本? (y/N)") == 'y')
    a_menu("點歌歌本",KTV,0);
  strcpy(buf,onlydate(now));
  if(check_money(5,GOLD)) return 0;
  if(lockutmpmode(OSONG)) return 0;
  move(12,0);
  clrtobot();
  sprintf(buf,"親愛的 %s 歡迎使用風塵點歌系統\n",cuser.userid);
  outs(buf);
  getdata(13, 0,"要點給誰呢:",destid, IDLEN+1, DOECHO,0);

  if(!destid[0]) { unlockutmpmode(); return 0; }

  getdata(14, 0,"想要要對他(她)說..:",say, 51, DOECHO,"我愛妳..");
  sprintf(save_title,"%s:%s",cuser.userid,say);
  getdata(16, 0, "寄到誰的信箱(可用E-mail)?", receiver, 45,LCECHO,destid);

  pressanykey("接著要選歌囉..進入歌本好好的選一首歌吧..^o^");
  a_menu("點歌歌本",SONGBOOK,0);

  if(!trans_buffer[0] || !(fp = fopen(trans_buffer, "r"))) 
    {unlockutmpmode(); return 0;}

  strcpy(filename, SONGPATH);
  stampfile(filename, &mail);
  unlink(filename);
  if(!(fp1 = fopen(filename, "w"))) 
  {
    fclose(fp); 
    unlockutmpmode(); 
    return;
  }

  strcpy(mail.owner, "點歌機");
  sprintf(mail.title,"◇ %s 點給 %s ",cuser.userid,destid);
  mail.savemode = 0;

  while (lines++ <= 11)
  {
     char *po;

     if(!fgets(buf,256,fp)) strcpy(buf,"\n");
     if(lines == 11)   // mdify by wildcat:把這行直接加進歌曲的最後一行 :)
       strcpy(buf,
         "  [1;36m<~Src~>[37m 要對 [33m<~Des~> [37m說：[32m<~Say~>[m\n");
     while (po = strstr(buf, "<~Src~>"))
        {
           po[0] = 0;
           sprintf(genbuf,"%s%s%s",buf,cuser.userid,po+7);
           strcpy(buf,genbuf);
        }
     while (po = strstr(buf, "<~Des~>"))
        {
           po[0] = 0;
           sprintf(genbuf,"%s%s%s",buf,destid,po+7);
           strcpy(buf,genbuf);
        }
     while (po = strstr(buf, "<~Say~>"))
        {
           po[0] = 0;
           sprintf(genbuf,"%s%s%s",buf,say,po+7);
           strcpy(buf,genbuf);
        }        
     fputs(buf,fp1);
   }
  fclose(fp1);
  fclose(fp);

  if(rec_add(SONGPATH"/.DIR", &mail, sizeof(mail))!=-1)
    degold(5);
  strcpy(mail.owner, "點歌機");
  sprintf(save_title,"%s:%s",cuser.userid,say);
  hold_mail(filename,destid);
  if(receiver[0])
    bbs_sendmail(filename, save_title, receiver);
  clear();
  outs(
   "\n\n  恭喜您點歌完成囉..\n"
       "  一小時內動態看板會自動重新更新\n"
       "  大家就可以看到您點的歌囉\n\n"
       "  點歌有任何問題可以到Note板的精華區找答案\n"
       "  也可在Note板精華區看到自己的點歌記錄\n"
       "  有任何保貴的意見也歡迎到Note板留話\n"
       "  讓親切的站長為您服務\n");
  pressanykey(NULL);
  sortsong();
  topsong();
  unlockutmpmode();
  return 1;
}
