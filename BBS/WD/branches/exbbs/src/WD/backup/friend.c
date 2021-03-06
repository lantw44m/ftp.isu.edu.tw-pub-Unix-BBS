/*-------------------------------------------------------*/
/* friend.c       ( PTTBBS Ver 2.36 )                    */
/*-------------------------------------------------------*/
/* target : Namelist                                     */
/* create : 95/03/29                                     */
/* update : 95/12/15                                     */
/*-------------------------------------------------------*/
#include "bbs.h"

extern int friendcount;
extern int rejected_number;

/* ------------------------------------- */
/* 特別名單                              */
/* ------------------------------------- */

/* Ptt 其他特別名單的檔名*/
static char special_list[] = "list.0";
static char special_des[] =  "ldes.0";

/* 特別名單的上限 */
static unsigned
char friend_max[8]  = {MAX_FRIEND , MAX_REJECT ,MAX_LOGIN_INFO,
   MAX_POST_INFO,MAX_NAMELIST,MAX_NAMELIST,MAX_NAMELIST,MAX_NAMELIST};

/* Ptt 各種特別名單的檔名 */
const char *friend_file[8] = { FN_OVERRIDES, FN_REJECT,
  FN_ALOHAED,FN_POSTLIST, "otherlist", FN_CANVOTE,FN_WATER,FN_VISABLE};

/* Ptt 各種特別名單的補述 */
static char *friend_desc[8] = {"友誼描述：", "惡形惡狀：","","","描述一下：",
   "投票者描述：","惡形惡狀：","看板好友描述"};

/* Ptt 各種特別名單的中文敘述 */
static char *friend_list[8] = {"好友名單","壞人名單",
   "上線通知","新文章通知","其它特別名單",
   "私人投票名單","看板禁聲名單","看板好友名單"};

void
setfriendfile(char *fpath, int type)
{
  if (type <= 4) /* user list Ptt */
     setuserfile(fpath, friend_file[type]);
  else        /* board list */
     setbfile(fpath, currboard, friend_file[type]);
}


void
friend_add(uident, type)
  char *uident;
  int type;
{
  char fpath[80];

  setfriendfile(fpath, type);
  if (friend_count(fpath) > friend_max[type]) 
    return;

  if ((uident[0] > ' ') && !belong(fpath, uident))
  {
    FILE *fp;
    char buf[40]="";
    char t_uident[IDLEN + 1];
    strcpy(t_uident, uident);   /* Thor: avoid uident run away when get data */

    if (type != FRIEND_ALOHA && type != FRIEND_POST)
      getdata(2, 0, friend_desc[type], buf, 40, DOECHO,0);

    if (fp = fopen(fpath, "a"))
    {
      flock(fileno(fp), LOCK_EX);
      fprintf(fp, "%-13s%s\n", t_uident, buf);
      flock(fileno(fp), LOCK_UN);
      fclose(fp);
    }
  }
}


void
friend_special()
{
   char genbuf[70],i,fname[70];

   friend_file[FRIEND_SPECIAL] = special_list;
   for(i=0 ; i<=9 ; i++)
        {
          sprintf(genbuf,"  ([36m%d[m)  .. ",i);
          special_des[5]=i+'0';
          setuserfile(fname, special_des);
          if(dashf(fname))
                {
                  FILE *fp=fopen(fname,"r");
                  fgets(genbuf+15,40,fp);
                  genbuf[47]=0;
                }
          move(i+12,0);
          clrtoeol();
          outs(genbuf);
        }
   getdata(22,0,"請選擇第幾號特別名單 (0~9)[0]?",genbuf,3,LCECHO,"0");
   if(genbuf[0] >='0' && genbuf[0] <='9')
              {  special_list[5]=genbuf[0]; special_des[5]=genbuf[0];}
   else
              {  special_list[5]='0';       special_des[5]='0';}
}

static void
friend_append(int type,int count)
{
  char fpath[80],i,j,buf[80],sfile[80];
  FILE *fp,*fp1;

  setfriendfile(fpath,type);

  do
  {
    int Select();
    move(2,0);
    clrtobot();
    outs("要引入哪一個名單?\n");
    for(i=0,j=0;i<=7;i++)
    {
      if(i==type) continue;
      j++;
      if(i<=4)
        {
         sprintf(buf,"  (%d) %-s\n",j,friend_list[i]);
        }
      else
        {
         sprintf(buf,"  (%d) %s 版的 %s\n",j, currboard,friend_list[i]);
        }
      outs(buf);
    }
         outs(       "  (S) 選擇其他看板的特別名單");
    getdata(11,0,"請選擇 或 直接[Enter] 放棄:",buf,3,LCECHO,0);
    if(!buf[0]) return;
    if(buf[0] == 's') Select();
  }while(buf[0] < '0' || buf[0]>'9');

  j = buf[0] - '1';
  if (j>=type) j++;
  if (j==FRIEND_SPECIAL) friend_special();

  setfriendfile(sfile,j);

  fp = fopen(sfile,"r");
  while (fgets(buf, 80, fp) && count <= friend_max[type])
  {
    char the_id[15];

    sscanf(buf,"%s",the_id);
    if (!belong(fpath,the_id))
     {
      if (fp1 = fopen(fpath, "a"))
      {
        flock(fileno(fp1), LOCK_EX);
        fputs(buf ,fp1);
        flock(fileno(fp1), LOCK_UN);
        fclose(fp1);
      }
     }
  }
  fclose(fp);
}


void
friend_delete(uident, type)
  char *uident;
  int type;
{
  FILE *fp, *nfp;
  char fn[80], fnnew[80];
  char genbuf[200];

  setfriendfile(fn,type);

  sprintf(fnnew, "%s-", fn);
  if ((fp = fopen(fn, "r")) && (nfp = fopen(fnnew, "w")))
  {
    int length = strlen(uident);

    while (fgets(genbuf, STRLEN, fp))
    {
      if ((genbuf[0] > ' ') && strncmp(genbuf, uident, length))
        fputs(genbuf, nfp);
    }
    fclose(fp);
    fclose(nfp);
    Rename(fnnew, fn);
  }
}


void
friend_query(uident, type)
  char *uident;
  int type;
{
  char fpath[80], name[IDLEN + 2], *desc, *ptr;
  int len;
  FILE *fp;
  char genbuf[200];

  setfriendfile(fpath,type);

  if (fp = fopen(fpath, "r"))
  {
    sprintf(name, "%s ", uident);
    len = strlen(name);
    desc = genbuf + 13;

    while (fgets(genbuf, STRLEN, fp))
    {
      if (!memcmp(genbuf, name, len))
      {
        if (ptr = strchr(desc, '\n'))
          ptr[0] = '\0';
        if (desc)
          prints("[37;43m%s[40;37m%s", friend_desc[type], desc);
        break;
      }
    }
    fclose(fp);
  }
}

void
friend_load()
{
  FILE *fp;
  ushort myfriends[MAX_FRIEND];
  ushort myrejects[MAX_REJECT];
  char genbuf[200];

  memset(myfriends, 0, sizeof(myfriends));
  friendcount = 0;
  setuserfile(genbuf, fn_overrides);
  if (fp = fopen(genbuf, "r"))
  {
    ushort unum;

    while (fgets(genbuf, STRLEN, fp) && friendcount < MAX_FRIEND - 2)
    {
      if (strtok(genbuf, str_space))
      {
        if (unum = searchuser(genbuf))
        {
          myfriends[friendcount++] = (ushort) unum;
        }
      }
    }
    fclose(fp);
  }
  memcpy(currutmp->friend, myfriends, sizeof(myfriends));

  memset(myrejects, 0, sizeof(myrejects));
  rejected_number = 0;
  setuserfile(genbuf, fn_reject);
  if (fp = fopen(genbuf, "r"))
  {
    ushort unum;

    while (fgets(genbuf, STRLEN, fp) && rejected_number < MAX_REJECT - 2)
    {
      if (strtok(genbuf, str_space))
      {
        if (unum = searchuser(genbuf))
        {
          myrejects[rejected_number++] = (ushort) unum;
        }
      }
    }
    fclose(fp);
  }
  memcpy(currutmp->reject, myrejects, sizeof(myrejects));
}


void
friend_water(char *message, int type)   /* 群體水球 added by Ptt*/
{
  char fpath[80],line[80],userid[IDLEN + 1];
  FILE *fp;

  setfriendfile(fpath,type);

  if(fp = fopen(fpath,"r"))
   while(fgets(line,80,fp))
      {
          user_info *uentp;
          int tuid;
          extern cmpuids();
          sscanf(line,"%s",userid);

          if ((tuid = searchuser(userid))  && tuid != usernum &&
              (uentp = (user_info *) search_ulistn(cmpuids, tuid, 1)) && !(
              is_rejected(uentp) & 2 && !HAS_PERM(PERM_SYSOP) ||
       uentp->invisible && !HAS_PERM(PERM_SEECLOAK) && !HAS_PERM(PERM_SYSOP) ||
              !PERM_HIDE(currutmp) && PERM_HIDE(uentp) ||
              cuser.uflag & FRIEND_FLAG && !is_friend(uentp)
             ))
             my_write(uentp->pid, message);
       }

  fclose(fp);
}

void
friend_edit(type)
  int type;
{
  char fpath[80], line[80], uident[20];
  int count, column, dirty;
  FILE *fp;
  char genbuf[200];

  if(type == FRIEND_SPECIAL) friend_special();
  setfriendfile(fpath,type);

  if(type == FRIEND_ALOHA || type == FRIEND_POST)
    {
     if(dashf(fpath))
        {
         sprintf(genbuf, "/bin/cp %s %s.old", fpath, fpath);
         system(genbuf);
        }
    }
  dirty = 0;
  while (1)
  {
    stand_title(friend_list[type]);
    move(0,40);
    sprintf(line,"(名單上限:%d個人)",friend_max[type]);
    outs(line);
    count = 0;
    CreateNameList();

    if (fp = fopen(fpath, "r"))
    {
      move(3, 0);
      column = 0;
      while (fgets(genbuf, STRLEN, fp))
      {
        if (genbuf[0] <= ' ')
          continue;
        strtok(genbuf, str_space);
        AddNameList(genbuf);
        prints("%-13s", genbuf);
        count++;
        if (++column > 5)
        {
          column = 0;
          outc('\n');
        }
      }
      fclose(fp);
    }
    getdata(1,0, (count ?
"(A)增加(D)刪除(P)引入其他名單(L)詳細列出(K)刪除整個名單(W)丟水球(Q)結束？[Q]" :
        "(A)增加 (P)引入其他名單 (Q)結束？[Q]"), uident,3, LCECHO,0);
    if (*uident == 'a')
    {
      move(1, 0);
      usercomplete(msg_uid, uident);
      if (uident[0] && searchuser(uident) && !InNameList(uident))
      {
        friend_add(uident, type);
        dirty = 1;
      }
    }
    else if (*uident == 'p')
    {
        friend_append(type,count);
        dirty = 1;
    }
    else if (*uident == 'd'  && count)
        {
              move(1, 0);
              namecomplete(msg_uid, uident);
              if (uident[0] && InNameList(uident))
              {
                friend_delete(uident, type);
                dirty = 1;
              }
        }
    else if (*uident == 'l'  && count)  more(fpath, YEA);

    else if (*uident == 'k'  && count)
        {
           getdata(2,0,"整份名單將會被刪除,您確定嗎 (a/N)?",uident,3,LCECHO,"N");
           if(*uident == 'a') unlink(fpath);
           dirty = 1;
        }
    else if (*uident == 'w'  && count)
        {
         *uident = 0;
         if (!getdata(0, 0, "群體水球:", uident + 1 , 60, DOECHO,0))
             continue;
         if (getdata(0, 0, "確定丟出群體水球? [Y]", line, 4, LCECHO,0) &&
             *line == 'n')
             continue;

         friend_water(uident,type);
        }
    else  break;
  }
  if (dirty)
    {
     move(2,0);
     outs("更新資料中..請稍候.....");
     refresh();
     if(type == FRIEND_ALOHA || type == FRIEND_POST)
        {
         sprintf(genbuf,"%s.old",fpath);
         if (fp = fopen(genbuf, "r")) {
             while (fgets(line, 80, fp)) {
                 sscanf(line,"%s",uident);
                 sethomefile(genbuf, uident,
                        type == FRIEND_ALOHA ? "aloha" : "postnotify.ok");
                 del_distinct(genbuf, cuser.userid);
              }
             fclose(fp);
          }
         sprintf(genbuf,"%s",fpath);
         if (fp = fopen(genbuf, "r")) {
             while (fgets(line, 80, fp)) {
                 sscanf(line,"%s",uident);
                 sethomefile(genbuf, uident,
                         type == FRIEND_ALOHA ? "aloha" : "postnotify");
                 add_distinct(genbuf, cuser.userid);
              }
             fclose(fp);
          }
        }
     else if(type == FRIEND_SPECIAL)
        {
          genbuf[0]=0;
          setuserfile(line,special_des);
          if(fp = fopen(line,"r") )
                {
                  fgets(genbuf,30,fp);
                  fclose(fp);
                }
          getdata(2,0," 請為此特別名單取一個簡短名稱:",genbuf,30,DOECHO,0);
          if(fp = fopen(line,"w") )
              {  fprintf(fp,"%s",genbuf);
                  fclose(fp);
                }
        }
     friend_load();
    }
}


int
t_override()
{
  friend_edit(FRIEND_OVERRIDE);
  return 0;
}


int
t_reject()
{
  friend_edit(FRIEND_REJECT);
  return 0;
}
