/*-------------------------------------------------------*/
/* talk.c       ( NTHU CS MapleBBS Ver 2.36 )            */
/*-------------------------------------------------------*/
/* target : talk/quety/friend routines                   */
/* create : 95/03/29                                     */
/* update : 02/09/10 (Dopin)                             */
/*-------------------------------------------------------*/

#define UPDATE_USEREC   (currmode |= MODE_DIRTY)
#define _MODES_C_

#include "bbs.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <math.h>
#include <time.h>

void check_listname();
extern void get_msg_1(void);

#ifdef USE_MULTI_TITLE
extern char title_color;
#endif
extern int station_num;
extern screenline* big_picture;

extern char station_limit[];
extern char station_list[][13];
extern char station_list_cn[][15];

extern int b_perm_edit(void);
extern int check_links(void);

/* Dopin 11/24/98 */
usint power(int i, int j) {
   usint res=1, cnt;

   for(cnt = 0 ; cnt < j ; cnt ++)
      res *= i;

   return res;
}

/* woju */
int cmpuids(int uid, user_info *urec) {
  return (uid == urec->uid);
}

int cmppids(pid_t pid, user_info *urec) {
  return (pid == urec->pid);
}

static int cmpunums(int unum, user_info *up) {
  if(up->pid) return (unum == up->destuid);
  return 0;
}

struct msg {
   char userid[IDLEN+1];
   char message[77];
   char time[6];
};
typedef struct msg msg;

int online_message(char *uident, char *msg) {
   int tuid;
   user_info *uentp;

   uschar mode0 = currutmp->mode;
   char c0 = currutmp->chatid[0];
   int currstat0 = currstat;

   if((tuid = searchuser(uident)) && tuid != usernum) {
      uentp = (user_info *) search_ulistn(cmpuids, tuid, 1);
      // Dopin : 如果對象不在線上 跳出此程式
      if(!uentp) return DONOTHING;

      if(uentp->mode != XMODE)
         if(HAS_PERM(PERM_SYSOP) || !(is_rejected(uentp) & 2))
            my_write(uentp->pid, msg);
   }

   return DONOTHING;
}

#ifdef CHOICE_RMSG_USER
int choice_water_id(void) {
   char callinid[IDLEN+1];
   char message[30];

   if(!HAS_PERM(PERM_LOGINOK)) return DONOTHING;

   move(0, 0);
   usercomplete("傳訊給 ", callinid, 0);

   if(callinid[0]) {
      move(b_lines, 0);
      sprintf(message, "%-12s : ",  callinid);

      online_message(callinid, message);
   }

   return FULLUPDATE;
}
#endif

unsigned long int file_seed(char *filename) {
   FILE *fp;
   long seed_name;
   char ci;
   char buf[20];
   char list_num[10] = "0123456789";
   char seed_array[9] = { 0, 0, 0, 0, 0, 0, 0, 0 };
   long int li;

   if((fp = fopen(filename, "r")) == NULL) sprintf(buf, "%d", (int)time(NULL));
   else {
     fgets(buf, 20, fp);
     fclose(fp);
   }
   li = (long)atoi(buf);

   for(ci = 0 ; ci < 48 ; ci++) seed_array[ci/6] = list_num[rand()%10];

   fp = fopen(filename, "w+");
   fprintf(fp, "%d", (long)atoi(seed_array));
   fclose(fp);

   return (unsigned long)li;
}

void buy_item_menu(void);
void buy_item_gold(void);
void buy_item_other(void);
void buy_weapon_menu(void);
void buy_weapon_sward(void);
void buy_weapon_wing(void);
void buy_weapon_ken(void);
void buy_other_menu(void);
void buy_orin_menu(void);
void buy_orin_stone(void);
void item_update_passwd(char);
void check_spc_eff(char *);

char *fn_board_favors   = "favor_boards";
char *fn_call_msg = "aloha";

int find_item_no(int);
int t_weapon(void);

extern int brdnum;
extern boardstat *nbrd;

#ifdef RPG_STATUS
char itemtable1[40][20] = {
"邦聯統治旗",
"亞特蘭提斯紀念金幣",
"亞特蘭提斯紀念銀幣",
"招財貓",
"會員卡",
"黑曜石",
"紅寶石",
"綠寶石",
"貓眼石",
"藍寶石",   /* 10 */
"鑽石",
"白羊座聖石",
"金牛座聖石",
"雙子座聖石",
"巨蟹座聖石",
"獅子座聖石",
"天秤座聖石",
"天蠍座聖石",
"摩羯座聖石",
"寶瓶座聖石",   /* 20 */
"雙魚座聖石",
"忍者刀",
"武士刀",
"短劍",
"寬刃短刀",
"聖十字劍",
"長矛",
"騎士槍",
"戰斧",
"雙刃斧",    /* 30 */
"十字弓",
"迴力鏢",
"飛刀",
"手裡劍",
"水晶",
"凌晨之星",
"TEDDY BEAR",
"石中劍",
"正宗刀",
"神聖矛" };    /* 40 */

char itemtable2[40][25] = {
"雷神錘",
"艾魯美絲之靴",
"雅典娜之盾",
"海神之戟",
"ＧＲＡＭ",
"ＧＵＮＧＮＩＲ",
"星塵劍",
"鳳凰劍",
"掃帚",
"魔法藥劑",  /* 50 */
"聖靈藥艾里庫沙",
"胥林克絲",
"月神弓",
"蛇杖",
"美杜莎之首",
"治療藥草",
"毒草",
"火鳳凰之羽",
"精靈之淚\",
"長劍",   /* 60 */
"寬刃劍",
"聖水",
"所羅門王指環",
"米斯里魯礦石",
"奧利哈岡礦石",
"光之劍",
"鏡",
"吸血劍(個人)",
"吸魔劍(個人)",
"變化之杖(個人)",    /* 70 */
"風之弓(個人)",
"不死鳥之劍(個人)",
"心劍(個人)",
"名馬奧克利凱布(個人)",
"琥珀(個人)",
"日記(個人)",
"紅帽小熊(個人)",
"星光乙女(個人)",
"含光",
"創世寶典(個人)" };   /* 80 */

char itemtable3[120][25] = {
"拉格那洛克(個人)",
"帕特雷多(個人)",
"魔羯之淚\(個人)",
"愛麗絲之劍(個人)",
"星之砂",
"飛馬西雲飛鴻(個人)",
"歌姬麥克風(個人)",
"魔法矛(個人)",
"風之盾(個人)",
"星塵鳳凰劍(個人)",   /* 90 */
"菊一文字(個人)",
"龍之劍(個人)",
"星之杖(個人)",
"諾亞的琵\琶琴",
"青釭劍(個人)",
"血之誓約 -- 那卡",
"那卡之書",
"神之加護",
"乙女座聖石",
"血之誓約 -- 歐德",   /* 100 */
"神劍巴魯孟克",
"獨角獸之角",
"名馬便便藏",
"    ",
"拖把",
"劍法之書",
"微風",
"泰迪熊",
"鈴鐺",
"拳套",   /* 110 */
"勇者之劍",
"勇者之弓",
"勇者之斧",
"鸚鵡",
"義肢",
"力量指環",
"技巧指環",
"速度指環",
"防禦指環",
"魔防指環",   /* 120 */
"生命腕輪",
"鐵爪",
"光劍",
"絨毛娃娃熊",
"變身棒",
"血之誓約 -- 赫玆爾",
"米斯特路汀",
"諾迪歐 (普通馬)",
"菜頭",
"忍法之書",   /* 130 */
"短刀",
"馬",
"木杖",
"菜刀",
"轟雷",
"竹劍",
"護手",
"聖經",
"拉克尼安劍",
"暴風雪",        /* 140 */
"狗爪",
"炎",
"微風",
"雷",
"光明之證",
"神龍石",
"無限核融合",
"羅盤",
"狂風",
"惡",            /* 150 */
"閃電",
"利賽亞",
"光之戒",
"聖杯",
"啟示錄",
"GRAM",
"破天罡\劍",
"長笛",
"邪惡之像",
"闇之魔法陣",    /* 160 */
"賢者之杖",
"美樂蒂",
"公主之冕",
"白百合之髮飾",
"闇之戒",
"恆星腕輪",
"珍珠墜飾",
"預言之書",
"水晶球",
"制裁之雷",
"勇者之證",      /* 171 */
"倫石",
"地契",
"核融合",
"太陽風",
"水晶之杖",
"狙擊之弓",
"風來人之劍",    /* 178 */
"星之砂(使用)",
"妮伊劍",
"冰矛",
""   };
#endif

int nitoa3(int num, char *ptr) {
   char num_buf[4] = { '0', '0', '0', 0 };

   if(num < 0)
      return -1;

   if(num >= 100) {
      num_buf[0] = (char)(num / 100) + '0';
      num -= (num_buf[0] - '0') * 100;
   }

   if(num >= 10) {
      num_buf[1] = (char)(num / 10) + '0';
      num -= (num_buf[1] - '0') * 10;
   }

   num_buf[2] = (char)num + '0';
   strcpy(ptr, num_buf);

   return 0;
}

void blank(void) {
   clear();
   move(10, 30);
   prints("系統尚未更新完成");
   pressanykey(NULL);
   return;
}

#ifdef RPG
char appendstatus[7],appendstatus1[7];
#endif

#ifdef RPG_FIGHT
userec p1,p2;

char fightround;
char hitflag[61];
char hitskl[61];
char p1wepname[25],p2wepname[25];
char uident1[STRLEN],uident2[STRLEN];
char skl[2][50];
usint p1status,p2status;
uschar p1hit;
uschar p2hit;
uschar p1maxhp;
uschar p2maxhp;

char askpk = 0;
/* Dopin */
#endif

#ifdef lint
#include <sys/uio.h>
#endif


#define M_INT 15                /* monitor mode update interval */
#define P_INT 20                /* interval to check for page req. in
                                 * talk/chat */

#define FRIEND_OVERRIDE 0
#define FRIEND_REJECT   1
#define BOARD_FAVOR     2
#define CALL_MSG        3

#define IRH 1
#define HRM 2

struct talk_win
{
  int curcol, curln;
  int sline, eline;
};


typedef struct
{
  user_info *ui;
  time_t idle;
  usint friend;
}      pickup;

extern int bind( /* int,struct sockaddr *, int */ );
extern bad_user(char* name);
extern char* getuserid();
extern struct UTMPFILE *utmpshm;

/* -------------------------- */
/* 記錄 friend 的 user number */
/* -------------------------- */

#define PICKUP_WAYS     5
int pickup_way = 0;
int friendcount;
int friends_number;
int override_number;
int rejected_number;
int aloha_number;
char count_number;
char *fcolor[7] = {"", "[1;32m", "[1;33m", "[1;37m", "[31m", "[1;35m", "[1;36m"};

char *talk_uent_buf;
char save_page_requestor[40];
char page_requestor[40];
static FILE* flog;

void friend_load();

int is_rejected(user_info *ui);

char *modestring(user_info *uentp, int simple) {
  static char modestr[40];
  static char *notonline="不在境內";
  register int mode = uentp->mode;
  register char *word;

  word = ModeTypeTable[mode];

  if (!(PERM_HIDE(uentp) && is_rejected(uentp) & HRM && is_friend(uentp) & 2))
     if (!(HAS_PERM(PERM_SYSOP) || HAS_PERM(PERM_SEECLOAK)) &&
         (uentp->invisible ||
         (is_rejected(uentp) & HRM && !(is_friend(uentp) & 2))))
       return (notonline);
/* woju */
  if (mode == EDITING) {
        sprintf(modestr, "E:%s",
           ModeTypeTable[uentp->destuid < EDITING ? uentp->destuid : EDITING]);
     word = modestr;
  }
  else if (!mode && *uentp->chatid == 1)
  {
     if (!simple)
        sprintf(modestr, "回應 %s", getuserid(uentp->destuid));
     else
        sprintf(modestr, "回應呼叫");
  }
  else if (!mode && *uentp->chatid == 2)
     sprintf(modestr, "被魔法擊中");
  else if (!mode && *uentp->chatid == 3)
     sprintf(modestr, "魔法頌唱中");

#ifdef TAKE_IDLE
  else if (!mode) {
/* Dopin */
    return (uentp->destuid == 6) ? uentp->chatid :
      IdleTypeTable[(0 <= uentp->destuid && uentp->destuid < 6) ?
                    uentp->destuid: 0];

     sprintf(modestr, "Idle: %s",uentp->destuid == 6 ? uentp->chatid :
      IdleTypeTable[(0 <= uentp->destuid && uentp->destuid < 6) ?
         uentp->destuid: 0]);
     return(modestr);
    }
#endif

  else if (simple)
    return (word);

  else if (uentp->in_chat && mode == CHATING)
    sprintf(modestr, "%s (%s)", word, uentp->chatid);
  else if (mode != TALK && mode != PAGE && mode != QUERY)
    return (word);
  else
    sprintf(modestr, "%s %s", word, getuserid(uentp->destuid));

  return (modestr);
}

/* ------------------------------------- */
/* routines for Talk->Friend             */
/* ------------------------------------- */

static int can_override(userid, whoasks)
  char *userid;
  char *whoasks;
{
  char buf[STRLEN];

  sethomefile(buf, userid, fn_overrides);
  return belong(buf, whoasks);
}

int is_aloha(user_info *ui) {
   register ushort unum, hit=0, *isaloha;

   isaloha = ui->aloha;
   while (unum = *isaloha++) {
      if(unum == currutmp->uid) {
         if(!ui->invisible && !PERM_HIDE(ui)) {
            hit = 1;
            aloha_number++;
            break;
         }
         break;
      }
   }
   return hit;
}

int is_friend(ui)
  user_info *ui;
{
  register ushort unum, hit, *myfriends;

  /* 判斷對方是否為我的朋友 ? */

  unum = ui->uid;
  myfriends = currutmp->friend;
  while(hit = *myfriends++)
    if(unum == hit) {
      hit = 1;
      if(count_number && !ui->invisible && !PERM_HIDE(ui)) friends_number++;
      break;
    }

  /* 判斷我是否為對方的朋友 ? */

  myfriends = ui->friend;
  while(unum = *myfriends++)
    if(unum == usernum) {
      if(count_number && !ui->invisible && !PERM_HIDE(ui)) override_number++;
      hit |= 2;
      break;
    }

  return hit;
}

static int be_rejected(char *userid) {
  char buf[STRLEN];

  sethomefile(buf, userid, fn_reject);
  return belong(buf, cuser.userid);
}

  /* 被拒絕 */

int is_rejected(user_info *ui) {
  register ushort unum, hit, *myrejects;

  /* 判斷對方是否為我的仇人 ? */

  unum = ui->uid;
  myrejects = currutmp->reject;
  while(hit = *myrejects++)
    if(unum == hit) {
      hit = 1;
      if(count_number) rejected_number++;
      break;
    }

  /* 判斷我是否為對方的仇人 ? */

  myrejects = ui->reject;
  while(unum = *myrejects++)
    if(unum == usernum) {
      if(hit & IRH && count_number) --rejected_number;
      hit |= 2;
      break;
    }

  return hit;
}

/* ------------------------------------- */
/* 真實動作                              */
/* ------------------------------------- */

static char *friend_file[4];
static char *friend_desc[2] = { "友誼描述：", "惡形惡狀：" };
static char *friend_list[4] =
            { "好友名單", "壞人名單", "我的最愛", "上線通知" };

static void friend_add(uident, type)
  char *uident;
  int type;
{
  char fpath[80];

  setuserfile(fpath, friend_file[type]);
  if ((uident[0] > ' ') && !belong(fpath, uident))
  {
    FILE *fp;
    char buf[40];
    char t_uident[IDLEN + 1];
    strcpy(t_uident, uident);   /* Thor: avoid uident run away when get data */

    if(type < 2)
       getdata(2, 0, friend_desc[type], buf, 40, DOECHO, 0);

    if (fp = fopen(fpath, "a"))
    {
      flock(fileno(fp), LOCK_EX);
      if(buf[0])
         fprintf(fp, "%-13s%s\n", t_uident, buf);
      else
         fprintf(fp, "%-13s\n", t_uident);
      flock(fileno(fp), LOCK_UN);
      fclose(fp);
    }
  }
#ifdef CHECK_LIST_ID_SELF
    check_listname();
#endif
}


static void friend_delete(uident, type)
  char *uident;
  int type;
{
  FILE *fp, *nfp;
  char fn[80], fnnew[80];
  char genbuf[200];

  setuserfile(fn, friend_file[type]);
  sprintf(fn, "chmod 660 %s", fn);
  system(fn);
  setuserfile(fn, friend_file[type]);

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
#ifdef CHECK_LIST_ID_SELF
    check_listname();
#endif
}

static void friend_query(uident, type)
  char *uident;
  int type;
{
  char fpath[80], name[IDLEN + 2], *desc, *ptr;
  int len;
  FILE *fp;
  char genbuf[200];

  setuserfile(fpath, friend_file[type]);
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
          prints("，%s%s", friend_desc[type], desc);
        break;
      }
    }
    fclose(fp);
  }
}



/* ----------------------------------------------------- */


static void
my_kick(uentp)
  user_info *uentp;
{
  char genbuf[200];

  getdata(1, 0, msg_sure_ny, genbuf, 4, LCECHO, 0);
  clrtoeol();
  if (genbuf[0] == 'y')
  {
    sprintf(genbuf, "%s (%s)", uentp->userid, uentp->username);
    log_usies("KICK ", genbuf);
    if ((kill(uentp->pid, SIGHUP) == -1) && (errno == ESRCH))
      memset(uentp, 0, sizeof(user_info));
    /* purge_utmp(uentp); */
    outs("傳送出去");
  }
  else
    outs(msg_cancel);
  pressanykey(NULL);
}

/* Dopin 10/31/98 */

char skillshow[50][7] = {
"    ",
"大盾",
"憤怒",
"追擊",
"連續",
"援護",   /* 5 */
"見切",
"必殺",
"月光",
"流星",
"太陽",   /* 10 */
"招喚",
"端茶",
"擦地",
"開機",
"黑箱",   /* 15 */
"抹殺",
"倒垃圾",
"吸血",
"吸魔",
"契約",   /* 20 */
"突擊",
"生命",
"原力",
"加速",
"跳躍",   /* 25 */
"視覺",
"推移",
"拉扯",
"半減",
"毒L1",     /* 30 */
"毒L2",
"毒L3",
"反彈",
"撒嬌",
"裝可愛",   /* 35 */
"站立",
"吐舌",
"搖尾",
"汪汪",
"偷盜",     /* 40 */
"詛咒",
"感應",
"韌皮",
"",
"",
"",
"",
"",
"" };

/* Dopin */
/* Dopin 07/26/98 */

my_query(char *uident) {
  extern char currmaildir[];
  char fpath[80];
  char j,dh;

  char *sex[8] = {
"人類",
"獸族",
"妖精",
"地靈",
"????",
"龍族",
"神族",
"魔族" };

/* Dopin */

  FILE *fps;
  char filebuf[100];
  int tuid, i, curs[3];
  user_info *uentp;

/* Dopin */
  if (tuid = getuser(uident))
  {
  clear();
#ifdef RPG_FIGHT
  if(askpk == 1)
     goto SeeStatus;
#endif

    move(1, 0);
    clrtobot();
    move(2, 0);
    setutmpmode(QUERY);
    currutmp->destuid = tuid;

    move(1, 0);
    prints("\033[1;32;40m〔英文帳號〕\033[m%-17s", xuser.userid);
    prints("\033[1;32;40m〔暱稱〕\033[m%-25s", xuser.username);
    prints("\033[1;32;40m〔冒險〕\033[m%d", xuser.numlogins);

    strcpy(filebuf, (char *)Cdate(&xuser.lastlogin));
    move(2, 0);
    prints("\033[1;32;40m〔文章提報〕\033[m\033[1;33;40m%-17d\033[m",
           xuser.good_posts);
    prints("\033[1;32;40m〔時間〕\033[m%-25s",
           (strcmp(xuser.userid, "SYSOP")) ? filebuf : "不詳");
    prints("\033[1;32;40m〔文章〕\033[m%d", xuser.numposts);

    uentp = (user_info *) search_ulist(cmpuids, tuid);
    move(3, 0);
    if(uentp && !(PERM_HIDE(currutmp) ||
      is_rejected(uentp) & HRM && is_friend(uentp) & 2)
      && PERM_HIDE(uentp))
      prints("\033[1;32;40m〔目前動態〕\033[m%-17s ", "未冒險");
    else
       prints("\033[1;32;40m〔目前動態〕\033[m%-17s",
              uentp ? modestring(uentp, 0) : "未冒險");

    sethomedir(currmaildir, xuser.userid);
    prints("\033[1;32;40m〔新信〕\033[m%-11s", chkmail(1) ? "有" : "無");
    sethomedir(currmaildir, cuser.userid);
    chkmail(1);

    if(1) {
    FILE *inftmp;
    int totmp, bytmp;
    char buftmp[100];
    char fromid[13], toid[13];

    bytmp = 0;
    totmp = 0;

    sethomefile(buftmp, xuser.userid, "query");

    /* ----------------------------------------------------------------- */

    /* Dopin: by Bosser (也可能是再轉的 原 patch 者不詳 > <) */
    if(inftmp = fopen(buftmp,"r")) {
       if(fscanf(inftmp,"%d %d %13s %13s",&bytmp, &totmp, &fromid, &toid)) {
          prints("\033[1;32;40m〔人氣〕\033[m%-6.6d", bytmp);
          prints("\033[1;32;40m〔好奇〕\033[m%-6.6d", totmp);
          /* fromid, toid */
       }
       fclose(inftmp);
    }

    if((strcmp(cuser.userid, xuser.userid)) && (strcmp(fromid,cuser.userid)))
       bytmp++;

    if(inftmp = fopen(buftmp,"w")) {
       fprintf(inftmp,"%d %d %13s %13s", bytmp, totmp, cuser.userid, toid);
       fclose(inftmp);
    }

    bytmp = 0;
    totmp = 0;

    sethomefile(buftmp, cuser.userid, "query");
    if(inftmp = fopen(buftmp,"r")) {
       if(fscanf(inftmp,"%d %d %13s %13s",&bytmp, &totmp,&fromid, &toid))
       if (strcmp(toid,xuser.userid)) totmp++;
       fclose(inftmp);
    }


    if(inftmp = fopen(buftmp,"w")) {
       fprintf(inftmp,"%d %d %13s %13s",bytmp,totmp,fromid,xuser.userid);
       fclose(inftmp);
    }
    /* Dopin */
  }

   /* Dopin */
    move(4,0);
    if(!xuser.lasthost[0]) strcpy(filebuf, "來源不明");
    else strcpy(filebuf, (xuser.welcomeflag & 64) && !HAS_PERM(PERM_SYSOP) ?
                         "使用者隱藏" : xuser.lasthost);

    prints("\033[1;32;40m〔最近登錄〕\033[m%-17s", filebuf);

    /* Dopin 062701 */
    curs[0] = curs[1] = 0;
#ifdef RPG_STATUS
    curs[2] = xuser.hp;
#endif

#ifndef MIN_USEREC_STRUCT
    prints("\033[1;32;40m〔性別〕\033[m%-11s", xuser.classsex == 3 ? "不詳" :
           xuser.classsex == 2 ? "女性" : xuser.classsex == 1 ? "男性" :
           "未定義");
#else
    outs("\033[1;32;40m〔備用〕\033[m      ");
#endif

#ifdef RPG_STATUS
    outs("\033[1;32;40m〔生命〕\033[m");
    if(curs[2] < xuser.hp)
       prints("\033[1;33;40m%2d\033[m", curs[2]);
    else
       prints("%2d", curs[2]);
    prints("/%2d\n\n", xuser.hp);
#else
    outs("\n\n");
#endif
    showplans(uident);

/* Dopin 07/26/98 */
#ifdef RPG_STATUS
    pressanykey("請鍵入任意鍵觀看使用者角色狀態");
    goto SeeStatus;
#else
    pressanykey(NULL);
    goto LeaveShow;
#endif

#ifdef RPG_STATUS
HaveWhat:
    if(fpath[0] == 'm') {
       clear();

       if(xuser.havetoolsnumber == 0)
          prints("該冒險者沒有任何物品\n");
       else for(i=1 ; i<=xuser.havetoolsnumber ; i++) {
          if(xuser.havetools[i-1] < 41)
             prints("(%2d) %s\n", i, &itemtable1[xuser.havetools[i-1]-1][0]);
          else if(xuser.havetools[i-1] < 81)
             prints("(%2d) %s\n", i, &itemtable2[xuser.havetools[i-1]-41][0]);
          else
             prints("(%2d) %s\n", i, &itemtable3[xuser.havetools[i-1]-81][0]);
        }

        pressanykey(NULL);
        goto ReturnSee;
    }
    else goto LeaveShow;

SeeStatus:
  {
ReturnSee:
     clear();
     /* Dopin 09/10/98 */
     outs("\n  種族: ");
     outs(sex[xuser.sex]);
     prints("    擁有金錢數: %6d      道具數: %d",
            xuser.havemoney, xuser.havetoolsnumber);

     prints("\n  職業等級: %2d  職業: ", xuser.nowlevel);

     if(xuser.working[0] == 0 || xuser.working[0] == 32) {
        prints("未定義職業");

        if(xuser.userlevel & PERM_SYSOP) prints(" (元老)");
        else if(xuser.userlevel & PERM_BM) prints(" (里長)");
     }
     else prints("%s", xuser.working);

     move(2, 40);
     prints("經驗值: %d\n\n",
            xuser.numlogins + xuser.numposts * 10 + xuser.addexp);

     outs("  個人資料                                                 "
          "裝備武器\n");
     prints("  HP  = %2d\n  STR = %2d\n  MGC = %2d\n  SKL = %2d\n  "
            "SPD = %2d\n  LUK = %2d\n  DEF = %2d\n  MDF = %2d\n\n",
            xuser.hp, xuser.str, xuser.mgc, xuser.skl, xuser.spd, xuser.luk,
            xuser.def, xuser.mdf);

     outs("    狀態                    個人技巧\n");
     outs("    正常\n");

     if(xuser.spcskl[0] == 0) {
        move(15,31);
        prints("無");
     }
     else {
        j = 0;
        for(i=0 ; i<6 ; i++) {
          move(15,17+j);
          prints("%s", &skillshow[xuser.spcskl[i]][0]);

          j += 5;
        }
     }

     move(17, 0);
     outs("  所屬:                     指揮官:                情人:");
     move(17, 8);
     if(xuser.belong[0] == ' ') prints("--");
     else prints("%s", xuser.belong);

     move(17, 36);
     if(xuser.commander > 0)
        for(i=0 ; i<xuser.commander ; i++) prints("☆");
     else if(xuser.cmdrname[0] == ' ') prints("--");
     else prints("%s", xuser.cmdrname);

     move(17, 57);
     if(xuser.lover[0] != ' ') prints("%s", xuser.lover);
     else prints("--");

     move(17, 65);
     outs("機動值    --");
     move(18, 0);
     prints("\n  武器使用等級\n  劍   槍   斧   弓   杖   炎   風   雷   光"
            "   闇   手   忍   銃   ??   ??\n");

     for(i=0, j=0 ; i<10 ; i++) {
        move(21, 2+j);
        if(xuser.wepnlv[0][i] == '-' || xuser.wepnlv[0][i] == 'C' ||
           xuser.wepnlv[0][i] == 'B' || xuser.wepnlv[0][i] == 'A' ||
           xuser.wepnlv[0][i] == '*')
           prints(" %c", xuser.wepnlv[0][i]);
        else prints(" -");

        j += 5;
     }

     j += 2;
     for(i=0 ; i<5 ; i++) {
        move(21, j);
        prints(" -");
        j += 5;
     }

     move(5,55);
     if(xuser.curwepnlv[0][0] > 0 && xuser.curwepnlv[0][0] !=104) {
        if(xuser.curwepnlv[0][0] -1 < 40)
           prints("%s\n", &itemtable1[xuser.curwepnlv[0][0]-1]);
        else if(xuser.curwepnlv[0][0] -1 < 80)
           prints("%s\n", &itemtable2[xuser.curwepnlv[0][0]-41]);
        else
           prints("%s\n", &itemtable3[xuser.curwepnlv[0][0]-81]);

        move(5,51);
        prints("(%c)", xuser.curwepnlv[1][0]);
     }

     appendstatus[0] = xuser.str;
     appendstatus[1] = xuser.mgc;
     appendstatus[2] = xuser.skl;
     appendstatus[3] = xuser.spd;
     appendstatus[4] = xuser.luk;
     appendstatus[5] = xuser.def;
     appendstatus[6] = xuser.mdf;

     if(xuser.curwepnspc[0] > 0) {
        if(!(xuser.curwepnspc[1] & 128)) {
           j = 64;

        for(i=0 ; i<7 ; i++) {
           move(6+i,11);
           if(xuser.curwepnspc[1] & j)
              if(xuser.curwepnspc[2] & j) {
                 prints("+20");
                 appendstatus[i] += 20;
              }
              else {
                 prints("+10");
                 appendstatus[i] += 10;
              }

              j /= 2;
           }
        }
     }

/* Dopin 11/01/98 */
     move(1,59); prints("性別： ");
     if(xuser.classsex == 0) prints("未確認");
     else if(xuser.classsex == 1) prints("男");
     else prints("女");

     move(2,59); outs("鬥技場等級：1");
     move(7,51); outs("攻擊力");
     move(8,51); outs("命中值");
     move(9,51); outs("迴避值");
     move(10,51); outs("防禦力");
     move(11,51); outs("魔防力");
     move(12,51); outs("移動力");

     move(14,51); prints("混沌值 %5d  效果值 %5d", xuser.will_value%100,
                         xuser.effect_value%100);
     move(15,51); prints("領導值 %5d  信賴值 %5d", xuser.leader_value%100,
                         xuser.belive_value%100);

     move(7,60);
     if(xuser.curwepnlv[0][0] == 0 || xuser.curwepnlv[0][0] == 104)
        outs(" --");
     else if(xuser.curwepnspc[3] & 128)
        prints("%3d", appendstatus[1]+xuser.curwepnatt);
     else
        prints("%3d", appendstatus[0]+xuser.curwepnatt);

     move(8,60);
     if(xuser.curwepnlv[0][0] == 0 || xuser.curwepnlv[0][0] == 104)
        prints(" --");
     else prints("%3d", appendstatus[2]*2 + xuser.curwepnhit);

     move(9,60);
     if(xuser.curwepnlv[0][0] == 0)
        prints("%3d", appendstatus[3]*2 + appendstatus[4]);
     else prints("%3d",
                 (appendstatus[3] - xuser.curwepnweg)*2 + appendstatus[4]);

     move(10,60); prints("%3d", appendstatus[5]);
     move(11,60); prints("%3d", appendstatus[6]);
     move(12,60); prints("%3d", xuser.mov);
     move(14,60); prints("  0");
     move(15,60); if(xuser.cmdrname[0] == ' ') prints(" --");

     move(7,66);  outs("----------");
     move(8,66);  outs("-  徵求  -");
     move(9,66);  outs(" 6x10 之 -");
     move(10,66); outs("-組織紋章-");
     move(11,66); outs("-ASCII 圖-");
     move(12,66); outs("----------");

     move(4,16);  outs("---------------------------------");
     move(5,16);  outs("-                               -");
     move(6,16);  outs("-                               -");
     move(7,16);  outs("-                               -");
     move(8,16);  outs("-  徵求 9 X 33 之職業 ASCII 圖  -");
     move(9,16);  outs("-                               -");
     move(10,16); outs("-                               -");
     move(11,16); outs("-                               -");
     move(12,16); outs("---------------------------------");

     getdata(23, 0, "請鍵入 <m> 檢視個人物品, 或其他任意鍵離開 ： ",
             fpath, 2, DOECHO, 0);
     goto HaveWhat;
  }
#endif

LeaveShow:
    return FULLUPDATE;
  }
  return DONOTHING;
}

/* Dopin: 檢查可不可以丟水球或聊天 ... */
int check_pager(user_info *u) {
  if(be_rejected(u->userid)) return 0; /* 壞人 啥事都別想做 */

  if(u->pager == 1) return  1;         /* 打開 傳訊聊天皆可 */
  if(u->pager == 2) return  0;         /* 拔掉 就是防擾模式 */
  if(u->pager == 3) return -1;         /* 防水 任何傳訊都擋 */

  return (is_friend(u) & HRM);         /* 好友 回傳與我為友 */
}

int my_write(pid_t pid, char *hint) {
  int len;
  char msg[80];
  FILE *fp;
  struct tm *ptime;
  time_t now;
  char genbuf[200];
  /* Dopin */
  char write_id[IDLEN+1];
  user_info *uin = (user_info*)search_ulist(cmppids, pid);

  /* woju */
  uschar mode0 = currutmp->mode;
  char c0 = currutmp->chatid[0];
  int currstat0 = currstat;

  /* Dopin */
  if(!uin) {
     my_outmsg("[1;33;41m糟糕! 對方已經停止冒險了 [37m~>_<~[m");
     if(isprint2(*hint)) sleep(1);
     return 0;
  }
  else {
     strcpy(write_id, uin->userid);
#ifdef RECORD_NEW_MSG
     uin->msgs.newmsg |= 1;
#endif
  }

  currutmp->mode = 0;
  currutmp->chatid[0] = 3;
  currstat = XMODE;

  time(&now);
  ptime = localtime(&now);

  if(isprint2(*hint)) {
    char inputbuf[4];

    if (!(len = getdata(0, 0, hint, msg, 65, DOECHO, 0))) {
      my_outmsg("[1;33;42m算了! 保存能力...[m");
      sleep(1);
      currutmp->chatid[0] = c0;
      currutmp->mode = mode0;
      currstat = currstat0;
      return 0;
    }
    else if(len == 255) return len;

    /* Dopin */
    if(!*uin->userid || strcmp(write_id, uin->userid)) {
       my_outmsg("[1;33;41m糟糕! 對方已經停止冒險了 [37m~>_<~[m");
       if(*hint) sleep(1);

       currutmp->chatid[0] = c0;
       currutmp->mode = mode0;
       currstat = currstat0;

       return 0;
    }

    sprintf(genbuf, "向%s報復:%.40s....?[Y] ", uin->userid, msg);

#ifdef DENY_MESSAGE_WHILE_PURPLE
    /* Dopin: 防止站長現身時被鎖定紫隱後中水球 */
    if(!HAS_PERM(PERM_SYSOP))
       if((uin->userlevel & PERM_SYSOP) && (uin->userlevel & PERM_DENYPOST)) {
          my_outmsg("[1;33;41m糟糕! 對方不在站上 [37m~>_<~[m");
          sleep(1);
          return 0;
       }
#endif

    getdata(0, 0, genbuf, inputbuf, 3, LCECHO, 0);
    genbuf[0] = '\0';

    if(inputbuf[0] == 'n') {
      currutmp->chatid[0] = c0;
      currutmp->mode = mode0;
      currstat = currstat0;
      return 0;
    }

/* Dopin */
    if (!*uin->userid || strcmp(write_id, uin->userid))  {
       my_outmsg("[1;33;41m糟糕! 對方已經停止冒險了 [37m~>_<~[m");
       if(isprint2(*hint)) sleep(1);

       currutmp->chatid[0] = c0;
       currutmp->mode = mode0;
       currstat = currstat0;
       return 0;
    }
  }
  else {
     strcpy(msg, hint + 1);
     len = strlen(msg);
  }

  now = time(0);

  /* Dopin: 上站通知不受傳訊模式影響 需在喜好設定中變更 */
  if(strstr(msg, "☆") && strstr(msg, "冒險了!")) goto NO_Write_Message;

  if(*hint != 1) {
    if(!HAS_PERM(PERM_SYSOP) && check_pager(uin) < 1) {

      my_outmsg("[1;33;41m糟糕! 對方防禦了! [37m~>_<~[m");
    }
    else {
      sethomefile(genbuf, uin->userid, fn_writelog);
      if(fp = fopen(genbuf, "a+")) {
        fprintf(fp, "[1;33;46m★ %s %s[37;45m %s [0m[%s]\n",
               cuser.userid, (*hint == 2) ? cuser.uflag & FRIEND_FLAG ?
               "[1;33;43m好友" : "[1;33;41m廣播" : "", msg,
               Cdatelite(&now));
        fclose(fp);
      }

      sethomefile(genbuf, cuser.userid, fn_writelog);
      if(fp = fopen(genbuf, "a+")) {
        fprintf(fp, "To %s: %s [%s]\n", uin->userid, msg, Cdatelite(&now));
        fclose(fp);
      }

NO_Write_Message:
      uin->msgs.last_pid = currpid;
      strcpy(uin->msgs.last_userid, cuser.userid);
      strcpy(uin->msgs.last_call_in, msg);

      if(kill(uin->pid, SIGUSR2) == -1)
         my_outmsg("[1;33;41m糟糕! 沒打中! [37m~>_<~[m");
      else my_outmsg("[1;33;44mPerfect! [37mb^(OO)^d[m");
    }
  }

  if(isprint2(*hint)) sleep(1);

  currutmp->chatid[0] = c0;
  currutmp->mode = mode0;
  currstat = currstat0;

  return 1;
}

/* Thor: for ask last call-in message */

int t_display() {
  char ans[4];
  char genbuf[200];

  setuserfile(genbuf, fn_writelog);
  strcpy(vetitle, "[傳訊記錄]");
  if(more(genbuf, YEA) != -1) return DONOTHING;
  else return FULLUPDATE;
}

/* ----------------------------------------------------- */

static int dotalkuent(user_info *uentp) {
  char buf[STRLEN];
  char mch;

  if(HAS_PERM(PERM_SYSOP) || HAS_PERM(PERM_SEECLOAK) || !uentp->invisible) {
    switch (uentp->mode) {
    case CLASS:
      mch = 'S';
      break;
    case TALK:
      mch = 'T';
      break;
    case CHATING:
      mch = 'C';
      break;
    case READNEW:
    case READING:
      mch = 'R';
      break;
    case POSTING:
      mch = 'P';
      break;
    case SMAIL:
    case RMAIL:
    case MAIL:
      mch = 'M';
      break;
    case EDITING:
      mch = 'E';
      break;
    default:
      mch = '-';
    }
    sprintf(buf, "%s%s(%c), ",
            uentp->invisible ? "*" : "", uentp->userid, mch);
    strcpy(talk_uent_buf, buf);
    talk_uent_buf += strlen(buf);
  }
  return 0;
}

static void do_talk_nextline(struct talk_win *twin) {
   twin->curcol = 0;
   if(twin->curln < twin->eline) ++(twin->curln);
   else region_scroll_up(twin->sline, twin->eline);
   move(twin->curln, twin->curcol);
}

static void do_talk_char(struct talk_win *twin, int ch) {
  extern int dumb_term;
  screenline* line;
  int i;
  char ch0;
  char buf[81];

  if (isprint2(ch)) {
     ch0 = big_picture[twin->curln].data[twin->curcol + mbbsd];
     if(big_picture[twin->curln].len < 79) move(twin->curln, twin->curcol);
     else do_talk_nextline(twin);

     outc(ch);
     ++(twin->curcol);
     line =  big_picture + twin->curln;
     if(twin->curcol < line->len) {      /* insert */
        ++(line->len);
        memcpy(buf, line->data + twin->curcol + mbbsd, 80);
        save_cursor();
        do_move(twin->curcol, twin->curln);
        ochar(line->data[twin->curcol + mbbsd] = ch0);

        for (i = twin->curcol + 1; i < line->len; i++)
           ochar(line->data[i + mbbsd] = buf[i - twin->curcol - 1]);
        restore_cursor();
     }
     line->data[line->len + mbbsd] = 0;
     return;
  }

  switch (ch) {
  case Ctrl('H'):
  case '\177':
    if(twin->curcol == 0) return;

    line =  big_picture + twin->curln;
    --(twin->curcol);
    if (twin->curcol < line->len) {
       --(line->len);
       save_cursor();
       do_move(twin->curcol, twin->curln);
       for (i = twin->curcol; i < line->len; i++)
          ochar(line->data[i + mbbsd] = line->data[i + 1 + mbbsd]);
       line->data[i + mbbsd] = 0;
       ochar(' ');
       restore_cursor();
    }
    move(twin->curln, twin->curcol);
    return;

  case Ctrl('D'):
     line =  big_picture + twin->curln;
     if(twin->curcol < line->len) {
        --(line->len);
        save_cursor();
        do_move(twin->curcol, twin->curln);
        for (i = twin->curcol; i < line->len; i++)
           ochar(line->data[i + mbbsd] = line->data[i + 1 + mbbsd]);
        line->data[i + mbbsd] = 0;
        ochar(' ');
        restore_cursor();
     }
     return;

  case Ctrl('b'):
     if (twin->curcol > 0) {
        --(twin->curcol);
        move(twin->curln, twin->curcol);
     }
     return;

  case Ctrl('F'):
     if (twin->curcol < 79) {
        ++(twin->curcol);
        move(twin->curln, twin->curcol);
     }
     return;

  case Ctrl('A'):
     twin->curcol = 0;
     move(twin->curln, twin->curcol);
     return;

  case Ctrl('K'):
     clrtoeol();
     return;

  case Ctrl('Y'):
     twin->curcol = 0;
     move(twin->curln, twin->curcol);
     clrtoeol();
     return;

  case Ctrl('E'):
     twin->curcol = big_picture[twin->curln].len;
     move(twin->curln, twin->curcol);
     return;

  case Ctrl('M'):
  case Ctrl('J'):
     line =  big_picture + twin->curln;
     strncpy(buf, line->data + mbbsd, line->len);
     buf[line->len] = 0;
     if(dumb_term) outc('\n');
     do_talk_nextline(twin);
     break;

  case Ctrl('P'):
     line =  big_picture + twin->curln;
     strncpy(buf, line->data + mbbsd, line->len);
     buf[line->len] = 0;
     if (twin->curln > twin->sline) {
        --(twin->curln);
        move(twin->curln, twin->curcol);
     }
     break;

  case Ctrl('N'):
     line =  big_picture + twin->curln;
     strncpy(buf, line->data + mbbsd, line->len);
     buf[line->len] = 0;
     if (twin->curln < twin->eline) {
        ++(twin->curln);
        move(twin->curln, twin->curcol);
     }
     break;
  }

  trim(buf);
  if(*buf) fprintf(flog, "%s%s: %s%s\n",
                   (twin->eline == b_lines - 1) ? "[1;35m" : "",
                   (twin->eline == b_lines - 1) ?
                   getuserid(currutmp->destuid) : cuser.userid, buf,
                   (ch == Ctrl('P')) ? "[37;45m(Up)[m" : "[m");
}

static void do_talk_string(struct talk_win *twin, char *str) {
  while (*str) do_talk_char(twin, *str++);
}

static void dotalkuserlist(struct talk_win *twin) {
  char bigbuf[MAXACTIVE * 20];
  int savecolumns;

  do_talk_string(twin, "*** 上線網友 ***\n");
  savecolumns = (t_columns > STRLEN ? t_columns : 0);
  talk_uent_buf = bigbuf;

  if(apply_ulist(dotalkuent) == -1) strcpy(bigbuf, "沒有任何使用者上線\n");
  strcpy(talk_uent_buf, "\n");
  do_talk_string(twin, bigbuf);
  if(savecolumns) t_columns = savecolumns;
}

static do_talk(int fd) {
   struct talk_win mywin, itswin;
   char mid_line[128], data[200], ans[4];
   int i, ch, datac;
   int im_leaving = 0;
   FILE *log;
   struct tm *ptime;
   time_t now;
   FILE* fp;
   char genbuf[200], fpath[100];
   int phone_mode = 0;
   char* pstr;
   extern char* phone_char();

   time(&now);
   ptime = localtime(&now);

   sethomepath(fpath, cuser.userid);
   strcpy(fpath, tempnam(fpath, "talk_"));
   flog = fopen(fpath, "w");

   setuserfile(genbuf, fn_talklog); /* Kaede */
   if(!is_watched(cuser.userid)) log = NULL;
   else if (log = fopen(genbuf, "a+"))
     fprintf(log, "[%d/%e %d:%02d] & %s\n",
             ptime->tm_mon + 1, ptime->tm_mday, ptime->tm_hour, ptime->tm_min,
             save_page_requestor);
   setutmpmode(TALK);

   ch = 58 - strlen(save_page_requestor);
   sprintf(genbuf, "%s【%s", cuser.userid, cuser.username);
   i = ch - strlen(genbuf);
   if(i >= 0) i = (i >> 1) + 1;
   else {
     genbuf[ch] = '\0';
     i = 1;
   }
   memset(data, ' ', i);
   data[i] = '\0';

   sprintf(mid_line, "[1;46;37m  鵲橋細語  [45m%s%s】 ◆  %s%s[0m",
           data, genbuf, save_page_requestor,  data);

   memset(&mywin, 0, sizeof(mywin));
   memset(&itswin, 0, sizeof(itswin));

   i = b_lines >> 1;
   mywin.eline = i - 1;
   itswin.curln = itswin.sline = i + 1;
   itswin.eline = b_lines - 1;

   clear();
   move(i, 0);
   outs(mid_line);
   move(0, 0);

   add_io(fd, 0);

   while(1) {
     ch = igetkey();

     if (ch == I_OTHERDATA) {
       datac = recv(fd, data, sizeof(data), 0);
       if(datac <= 0) break;
       for (i = 0; i < datac; i++) do_talk_char(&itswin, data[i]);
     }
     else {
       if(ch == Ctrl('C')) {
         if(im_leaving) break;
         move(b_lines, 0);
         clrtoeol();
         outs("再按一次 Ctrl-C 就正式中止談話囉！");
         im_leaving = 1;
         continue;
       }

       if(im_leaving) {
         move(b_lines, 0);
         clrtoeol();
         im_leaving = 0;
       }

#ifdef CTRL_G_REVIEW
       if(ch == Ctrl('G')) {
         add_io(0, 0);
         scr_exec(get_msg_1);
         add_io(fd, 0);
         continue;
       }
#endif

       if (ch == KEY_LEFT) ch = Ctrl('b');
       else if (ch == KEY_RIGHT) ch = Ctrl('F');
       else if (ch == KEY_UP) ch = Ctrl('P');
       else if (ch == KEY_DOWN) ch = Ctrl('N');

       if(phone_mode && (pstr = phone_char(ch))
         || isprint2(ch) || ch == Ctrl('H') || ch == '\177'
         || ch == Ctrl('M') || ch == Ctrl('J') || ch == Ctrl('F')
         || ch == Ctrl('b') || ch == Ctrl('D') || ch == Ctrl('A')
         || ch == Ctrl('E') || ch == Ctrl('K') || ch == Ctrl('Y')
         || ch == Ctrl('P') || ch == Ctrl('N')) {
         if (phone_mode && pstr) data[0] = (char)pstr[0];
         else data[0] = (char) ch;
         if (send(fd, data, 1, 0) != 1) break;
         if (log) fprintf(log, "%c", (ch == Ctrl('M'))? '\n' : (char) *data);

         do_talk_char(&mywin, *data);
         if (phone_mode && pstr) {
            data[0] = (char) pstr[1];
            if(send(fd, data, 1, 0) != 1) break;
            if(log)
              fprintf(log, "%c", (ch == Ctrl('M'))? '\n' : (char) *data);
            do_talk_char(&mywin, *data);
         }
       }
       else if (ch == KEY_ESC)
          switch (KEY_ESC_arg) {
          case 'p':
             phone_mode ^= 1;
             break;
          case 'c':
             capture_screen();
             break;
          case 'n': {
             add_io(0, 0);
             scr_exec(edit_note);
             add_io(fd, 0);
             }
             break;
          }
       else if (ch == Ctrl('U')) {
         add_io(0, 0);
         scr_exec(t_users);
         add_io(fd, 0);
       }
#ifdef TAKE_IDLE
       else if (ch == Ctrl('I')) {
         add_io(0, 0);
         scr_exec(t_idle);
         add_io(fd, 0);
       }
#endif
       else if (ch == Ctrl('B')) {
         add_io(0, 0);
         scr_exec(m_read);
         add_io(fd, 0);
       }
       else if (ch == Ctrl('R')) {
         add_io(0, 0);
         DL_func("bin/message.so:get_msg", 0);
         add_io(fd, 0);
       }
     }
   }

   if(log) fclose(log);

   add_io(0, 0);
   close(fd);

   if (flog) {
      char ans[4];
      extern uschar scr_lns;
      int i;

      time(&now);
      fprintf(flog, "\n[33;44m離別畫面 [%s] ...     [m\n", Cdatelite(&now));
      for(i = 0; i < scr_lns; i++)
         fprintf(flog, "%.*s\n", big_picture[i].len,
                 big_picture[i].data + mbbsd);
      fclose(flog);
      more(fpath, NA);

      getdata(b_lines - 1, 0, "清除(C) 移至備忘錄(M) (C/M)?[C]", ans, 4,
              LCECHO, 0);
      if(*ans == 'm') {
         fileheader mymail;
         char title[128];

         sethomepath(genbuf, cuser.userid);
         stampfile(genbuf, &mymail);
         mymail.savemode = 'H';        /* hold-mail flag */
         mymail.filemode = FILE_READ;
         strcpy(mymail.owner, "[備.忘.錄]");
         sprintf(mymail.title, "對話記錄 [1;36m(%s)[m",
                 getuserid(currutmp->destuid));
         sethomedir(title, cuser.userid);
         append_record(title, &mymail, sizeof(mymail));
         Rename(fpath, genbuf);
      }
      else unlink(fpath);
      flog = 0;
   }

   setutmpmode(XINFO);
}

static void my_talk(user_info *uin) {
  int sock, msgsock, length, ch;
  struct sockaddr_in server;
  pid_t pid;
  char c;
  char genbuf[4];
  uschar mode0 = currutmp->mode;

  ch = uin->mode;
  strcpy(currauthor, uin->userid);

  if(!(pid = uin->pid) || (kill(pid, 0) == -1)) {
    resetutmpent();
    outs(msg_usr_left);
  }
  else if(ch == EDITING || ch == TALK || ch == CHATING || ch == PAGE ||
         ch == MAILALL || ch == MONITOR || !ch && (uin->chatid[0] == 1 ||
         uin->chatid[0] == 3)) outs("人家在忙啦");
  else if(!check_pager(uin)) outs("對方禁止打擾中");  /* Dopin 03/05/13 */
  else {
    showplans(uin->userid);
    getdata(2, 0, "確認要 談天/取消 嗎 (y/N) ? ", genbuf, 4, LCECHO, 0);
    if(*genbuf != 'y') goto i_return_talk;

ComfirmPK:
#ifdef RPG_FIGHT
    clear();
    if(askpk == 1) {
       pressanykey("抱歉,該函式未完成 ::::>_<::::");
       askpk = 0;
    }

    return;
#endif

BeginTalk:
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0) {
      perror("sock err");
      return;
    }

    server.sin_family = PF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = 0;
    if(bind(sock, (struct sockaddr *) & server, sizeof server) < 0) {
      close(sock);
      perror("bind err");
      return;
    }

    length = sizeof server;
    if(getsockname(sock, (struct sockaddr *) & server, &length) < 0) {
      close(sock);
      perror("sock name err");
      return;
    }

    currutmp->sockactive = YEA;
    currutmp->sockaddr = server.sin_port;
    currutmp->destuid = uin->uid;
    setutmpmode(PAGE);
    uin->destuip = currutmp;
    kill(pid, SIGUSR1);
    clear();

#ifdef RPG_FIGHT
    if(askpk == 1) outs("提出挑戰書中.....\n鍵入 Ctrl-D 中止....");
    else
#endif
       prints("正呼叫 %s.....\n鍵入 Ctrl-D 中止....", uin->userid);

    listen(sock, 1);
    add_io(sock, 5);
    while(1) {
      ch = igetch();
      if (ch == I_TIMEOUT) {
         ch = uin->mode;
         if (!ch && uin->chatid[0] == 1 && uin->destuip == currutmp) {
             bell();
#ifdef RPG_FIGHT
             if(askpk == 1) outmsg("對方思考中...");
             else
#endif
                outmsg("對方回應中...");
             refresh();
         }
         else if(ch == EDITING || ch == TALK || ch == CHATING
              || ch == PAGE || ch == MAILALL || ch == MONITOR
              || !ch && (uin->chatid[0] == 1 || uin->chatid[0] == 3)) {
                 add_io(0, 0);
                 close(sock);
                 currutmp->sockactive = currutmp->destuid = 0;
                 pressanykey("人家在忙啦");
                 return;
         }
         else {
#ifdef LINUX
           add_io(sock, 20);       /* added 4 linux... achen */
#endif
           move(0, 0);
           outs("再");
           bell();

           uin->destuip = currutmp;
           if (kill(pid, SIGUSR1) == -1) {
#ifdef LINUX
             add_io(sock, 20);     /* added 4 linux... achen */
#endif

             outmsg(msg_usr_left);
             refresh();
             pressanykey(NULL);
             return;
           }
           continue;
         }
      }

      if(ch == I_OTHERDATA) break;

      if(ch == '\004') {
        add_io(0, 0);
        close(sock);
        currutmp->sockactive = currutmp->destuid = 0;
        return;
      }
    }

    msgsock = accept(sock, (struct sockaddr *) 0, (int *) 0);
    if(msgsock == -1) {
      perror("accept");
      return;
    }
    add_io(0, 0);
    close(sock);
    currutmp->sockactive = NA;
    read(msgsock, &c, sizeof c);

    if (c == 'y') {
#ifdef RPG_FIGHT
      if(HAS_PERM(PERM_SYSOP) && askpk == 1) {
         setutmpmode(CHICKENTALK);
         clear();
         pressanykey("程式尚未完成,以下結果對方看不到");
         strcpy(uident1, cuser.userid);
         strcpy(uident2, uin->userid);
         player_input();
         fightround = 0;
         beginfig();
      }
      else {
#endif
         sprintf(save_page_requestor, "%s (%s)", uin->userid, uin->username);
         do_talk(msgsock);
#ifdef RPG_FIGHT
      }
#endif
    }
    else {
      move(9, 9);
      outs("【回音】 ");
      switch (c) {
      case 'a':
#ifdef RPG_FIGHT
        if(askpk == 1) outs("我現在很忙，請等一會兒再找我對戰，好嗎？");
        else
#endif
           outs("我現在很忙，請等一會兒再 call 我，好嗎？");
        break;

      case 'b':
#ifdef RPG_FIGHT
        if(askpk == 1) outs("對不起，我有事情不能跟你對戰.....");
        else
#endif
           outs("對不起，我有事情不能跟你 Talk");
        break;

      case 'd':
        outs("ㄟ你有完沒完？像蒼蠅一樣嗡嗡叫，真討厭....");
        break;

      case 'c':
        outs("請不要吵我好嗎？");
        break;

      case 'e':
#ifdef RPG_FIGHT
        if(askpk == 1) outs("唉....你不是我的對手....還是別打了...");
        else
#endif
           outs("找我有事嗎？請先來信唷....");
        break;

      case 'f': {
          char msgbuf[60];
          read(msgsock, msgbuf, 60);
          outs("對不起，我現在不能跟你 talk，因為\n");
          move(10,18);
          outs(msgbuf);
        }
        break;

      default:
#ifdef RPG_FIGHT
        if(askpk == 1) outs("我現在不想對戰啦.....:)");
        else
#endif
           outs("我現在不想 talk 啦.....:)");
      }
      close(msgsock);
    }
  }

i_return_talk:
  currutmp->mode = mode0;
  currutmp->destuid = 0;
  pressanykey(NULL);
}

/* ------------------------------------- */
/* 選單式聊天介面                        */
/* ------------------------------------- */

#define US_PICKUP       1234
#define US_RESORT       1233
#define US_ACTION       1232
#define US_REDRAW       1231

static int search_pickup(int num, int actor, pickup pklist[]) {
  char genbuf[IDLEN + 2];

  getdata(b_lines - 1, 0, "請輸入使用者姓名：", genbuf, IDLEN + 1, DOECHO, 0);
  move(b_lines - 1, 0);
  clrtoeol();

  if(genbuf[0]) {
    int n = (num + 1) % actor;

    str_lower(genbuf, genbuf);
    while(n != num) {
      if (strstr_lower(pklist[n].ui->userid, genbuf))
        return n;
      if (++n >= actor)
        n = 0;
    }
  }

  return -1;
}

static int pickup_cmp(i, j)
pickup *i, *j; {
  if(!strcmp(i->ui->userid, cuser.userid)) return -1;
  if(!strcmp(j->ui->userid, cuser.userid)) return  1;

  switch (pickup_way) {
    case 0: {
      register int friend;
      if(friend = j->friend - i->friend) return friend;
    }

    case 1: return strcasecmp(i->ui->userid, j->ui->userid);
    case 2: return (i->ui->mode - j->ui->mode);
    case 3: return (i->idle - j->idle);
    case 4: return strcasecmp(i->ui->from, j->ui->from);
  }
}

/* Kaede show friend description */
static char *friend_descript(char *uident) {
  static char *space_buf="                    ";
  static char desc_buf[80];
  char fpath[80], name[IDLEN + 2], *desc, *ptr;
  int len, flag;
  FILE *fp;
  char genbuf[200];

  setuserfile(fpath, friend_file[0]);
  if(fp = fopen(fpath, "r")) {
    sprintf(name, "%s ", uident);
    len = strlen(name);
    desc = genbuf + 13;

    while ((flag = (int)fgets(genbuf, STRLEN, fp))) {
      if(!memcmp(genbuf, name, len)) {
        if(ptr = strchr(desc, '\n')) ptr[0] = '\0';
        if (desc) break;
      }
    }
    fclose(fp);

    if(desc && flag) strcpy(desc_buf, desc);
    else return space_buf;

    return desc_buf;
  }
  else return space_buf;
}

#ifdef TRANS_FROMHOST
#ifdef FROMHOST_BY_FILE
/* Dopin 02/11/20 */
char *where(char *from) {
   int i;
   FILE *fp;
   fh2st st;

   if((fp = fopen("etc/transhost.db", "rb")) == NULL) goto RT_OF_WHERE;

   while(fread(&st, sizeof(st), 1, fp)) {
      i = st.host_type ? (int)(!strstr(from, st.fromhost)) :
                         strcmp(from, st.fromhost);

      if(!i) {
         char *trans = (char *)malloc(sizeof(char) * 21);

         fclose(fp);
         strcpy(trans, st.transto);
         return trans;
      }
   }

   fclose(fp);
RT_OF_WHERE:
   return from;
}
#else
/* 原作者 buggy, Izero (zoo.twbbs.org) 1996 */
char *where(char *from) { /* 更改 UserList 上的故鄉 用 where 做判斷再回傳  */
   #define NUM_HOSTS1 26    /* Dopin: 更動資料記得要改這裡 */
   char compare1[NUM_HOSTS1][2][30] = {
       /* 站長群的電腦 */
      {"localhost",           "站台管理處"    },
      {"210.68.39.15",        "豆餅個人工作室"},
      {"ym66189.ym.edu.tw",   "亞特蘭提斯"    },
      {"IAN.m3.ntu.edu.tw",   "來自魔界"      },
      {"Pahn.m1.ntu.edu",     "牧場售票口"    },
      {"140.112.213.207",     "波特多之家"    },
       /* 好友的電腦 */
      {"kk.f2.ntu.edu.tw",    "貓貓貓貓貓貓"  },
      {"Luzern.m3.ntu.edu",   "白癡白痴白癡"  },
      {"Elfstone.m1.ntu",     "小遠的家"      },
      {"Turtle.m1.ntu.edu",   "忍者龜的下水道"},
      {"KYO.m1.ntu.edu.tw",   "龍域"          },
      {"Dolphin.m3.ntu.edu",  "海豚樂園"      },
      {"140.112.169.187",     "萱萱的窩"      },
      {"polymer.m1.ntu.ed",   "臥龍居"        },
      {"140.112.169.223",     "粉色水晶內"    },
      {"x.m1.ntu.edu.tw",     "小花貓"        },
      {"ccstudent.ee.ntu",    "台大電機工作站"},
      {"m7070.m7.ntu.edu.",   "Unknown"       },
      {"WZ.m1.ntu.edu.tw",    "wz的家"        },
      {"140.112.213.176",     "雁鳥的故鄉"    },
      {"m7201.m7.ntu.edu.",   "邪惡小小容的家"},
      {"140.119.138.28",      "文山區牧場"    },
      {"140.112.169.233",     "風有故鄉嗎？"  },
       /* 大大們的主機 */
      {"freebsd.ee.ntu.edu",  "神祕深奧的地方"},
      {"woju.g1.ntu.edu.tw",  "詭異之域"      },
      {"Power.m7.ntu.edu.t",  "既magic又Power"}
   };

   #define NUM_HOSTS2 25    /* Dopin: 更動資料記得要改這裡 */
   char compare2[NUM_HOSTS2][2][30] = {
      {"HINET",               "爆貴的駭＝＝NET" },
      {"dsl-dyn-tpe",         "索尼網路"        },
      {"210.60.60",           "玄奘高級冷氣套房"},
      {"210.60.61",           "『玄』字號房"    },
      {"210.60.62",           "玄奘高級冷氣套房"},
      {"210.60.63",           "『玄』字號房"    },
      {"seed",                "C 網路"          },
      {"giga",                "Gi尬"            },
      {"211.22",              "中滑電信"        },
      {"adsl-dyn-tpe-",       "索尼網路"        },
      {"192.168.",            "內部黑箱網路"    },
      {".ym.edu.tw",          "陽明大學"        },
      {".cc.ntu.edu.",        "台大計中"        },
      {"140.112.20.",         "台大電機系"      },
      {"140.112.52.",         "台大物理系"      },
      {"140.112.145",         "台大普通大樓"    },
      {"168.95.100.",         "HiNet "          },
      {"168.95.101.",         "HiNet "          },
      {"140.112.8.",          "台大計中工作站"  },
      {"dialup.ntu",          "台大撥接專線"    },
      {".nctu.",              "國立交通大學"    },
      {".ncku.ed",            "國立成功\大學"   },
      {"140.116.",            "國立成功\大學"   },
      {"203.64.52.",          "北一女中"        },
      {"168.95.99.",          "HiNet"           }
   };

   int i;

   for(i=0;i<NUM_HOSTS1;i++)
   if(strncmp(compare1[i][0],from,15)==0) {    /* Dopin: 注意這裡是 strncmp */
      char *local=(char *)malloc(sizeof(char)*17);
      strcpy(local,compare1[i][1]);
      return local;
   }

   for(i=0;i<NUM_HOSTS2;i++)
   if(strstr(from,compare2[i][0])) {           /* Dopin: 注意這裡是 strstr  */
      char *local=(char *)malloc(sizeof(char)*17);
      strcpy(local,compare2[i][1]);
      return local; /* Dopin: 兩個分別是 "完全" 與 "部份" 比對 作用並不相同 */
   }

   return from;
}
#endif
#endif

static int pickup_user(void) {
  static int real_name = 0;
  static int show_friend = 0;
  static int show_uid = 0;
  static int show_tty = 0;
  static int show_pid = 0;
  static int num = 0;
  char genbuf[200];

  register user_info *uentp;
  register int state = US_PICKUP, hate, ch;
  register int actor, head, foot;
  int badman;
  int savemode = currstat;
  time_t diff, freshtime;
  pickup pklist[USHM_SIZE];
  FILE *fp;

  struct stat ttystat;
  char buf[20];
  char pagerchar[4] = "* o ";
  char *msg_pickup_way[PICKUP_WAYS] =
  { "嗨！朋友",
    "網友代號",
    "網友動態",
    "發呆時間",
    "來自何方"
  };

  while(1) {
    if(state == US_PICKUP) freshtime = 0;

    if(utmpshm->uptime > freshtime) {
      time(&freshtime);
      friends_number = override_number = rejected_number = actor = ch = 0;

      while(ch < USHM_SIZE) {
        uentp = &(utmpshm->uinfo[ch++]);
        if(uentp->pid > 1) {
          count_number = 1;
          head = is_friend(uentp);
          count_number = 0;

#ifndef NO_USE_MULTI_STATION
/* Dopin 02/09/04 */
#ifdef SYSOP_SEE_ALL /* SYSOP Skip Check Station List */
          if(HAS_PERM(PERM_SYSOP)) goto SKIP_SAME_ST_CHECK;
#endif

#ifdef ONLY_SEE_SAME_STATION
   #ifdef DEFAULT_ST_SEE_ALL
          if(cuser.now_stno)
   #endif
          {
             if(
   #ifdef FRIEND_CAN_SEE_ANYWHERE
                !(head & 3) &&
   #endif
                strcmp(cuser.station, station_list[uentp->now_stno])
               )
                continue;
          }
   #ifdef CANT_SEE_HIDE_STATAION
          else {
             if(strcmp(cuser.station, station_list[uentp->now_stno]) &&
                station_limit[uentp->now_stno]) continue;
          }
   #endif
#else
   #ifdef CANT_SEE_HIDE_STATAION
          if(station_limit[uentp->now_stno]) continue;
   #endif
#endif
SKIP_SAME_ST_CHECK:  /* Dopin */
#endif

          /* Dopin: 一般名單才需計算壞人 好友列表模式不需要 反而會多算一次 */
          if(!(cuser.uflag & FRIEND_FLAG)) count_number = 1;
          if(!((badman = is_rejected(uentp)) & HRM && head & 2 &&
             PERM_HIDE(uentp))) {
             if(badman & (cuser.welcomeflag & 256 ? 1 : 0)) continue;
             if(badman & HRM && !(head & 2) && !HAS_PERM(PERM_SYSOP) ||
               (uentp->invisible && !(HAS_PERM(PERM_SYSOP) ||
                HAS_PERM(PERM_SEECLOAK))))
                continue; /* Thor: can't see anyone who rejects you. */

             if(!PERM_HIDE(currutmp) && PERM_HIDE(uentp)) continue;
             if(cuser.uflag & FRIEND_FLAG && !head) continue;
          }
          count_number = 0;

#ifdef SHOW_IDLE_TIME
          diff = freshtime - uentp->uptime;

  #ifdef DOTIMEOUT
            /* prevent fault /dev mount from kicking out users */
            if(!(PERM_HIDE(uentp) || uentp->mode == MAILALL) &&
              (diff > IDLE_TIMEOUT) && (diff < 60 * 60 * 24 * 5)) {
              if ((kill(uentp->pid, SIGHUP) == -1) && (errno == ESRCH))
                memset(uentp, 0, sizeof(user_info));
              continue;
            }
  #endif
          pklist[actor].idle = diff;
#endif
          pklist[actor].friend = head;
          pklist[actor].ui = uentp;
          actor++;
        }
      }
      badman = rejected_number;
      state = US_PICKUP;

      if(!actor) {
        getdata(b_lines - 1, 0, "你的朋友還沒上站，要看看一般網友嗎(Y/N)？[Y]",
                genbuf, 4, LCECHO, 0);
        if(genbuf[0] != 'n') {
          cuser.uflag &= ~FRIEND_FLAG;
          continue;
        }
        return;
      }
    }

    if(state >= US_RESORT) qsort(pklist, actor, sizeof(pickup), pickup_cmp);

    if(state >= US_ACTION) {
      showtitle((cuser.uflag & FRIEND_FLAG)? "好友列表": "休閒聊天",
#ifndef NO_USE_MULTI_STATION
            station_list_cn[cuser.now_stno]
#else
            BOARDNAME
#endif
      );

      prints(
        "  排序方式：[%s]  上站人數：%-6d[1;32m我的朋友：%-4d"
        "[33m與我為友：%-4d[%d;3%dm壞人：%-3d[0m\n"
#ifdef SHOW_FROMHOST
        "[7m  %s P%c代號        %-18.17s%-15.14s%-11.10s%-9.8s%s[0m\n",
#else
        "[7m  %s P%c代號        %-29.28s%-12.11s%-11.10s%-7.7s[m\n",
#endif
        msg_pickup_way[pickup_way], actor, friends_number, override_number,
        cuser.welcomeflag & 256 ? 0 : 1, cuser.welcomeflag & 256 ? 0 : 1,
        badman, /* Dopin: 加了這裡 不想看的連壞人數都不會計算給你 QQ;;; */

#ifdef SHOWUID
        show_uid ? "UID" :
#endif
        "No.",
        (HAS_PERM(PERM_SEECLOAK) || HAS_PERM(PERM_SYSOP)) ? 'C' : ' ',

#ifdef REALINFO
        real_name ? "姓名" :
#endif

        "暱稱",
#ifdef SHOW_FROMHOST
        show_friend ? "好友描述" : "故鄉",
#endif

#ifdef SHOWTTY
        show_tty ? "TTY " :
#endif
        "動態",
#ifdef SHOWPID
        show_pid ? "       PID" :
#endif
#ifndef NO_USE_MULTI_STATION
        "站台",
#else
        "心情",
#endif
#ifdef SHOW_IDLE_TIME
        "分:秒 "
#else
        ""
#endif

        );
    }
    else {
      move(3, 0);
      clrtobot();
    }

    if(num < 0) num = 0;
    else if(num >= actor) num = actor - 1;

    head = (num / p_lines) * p_lines;
    foot = head + p_lines;
    if(foot > actor) foot = actor;

    for(ch = head; ch < foot; ch++) {
      uentp = pklist[ch].ui;

      if(!uentp->pid) {
        state = US_PICKUP;
        break;
      }

#ifdef SHOW_IDLE_TIME
      diff = pklist[ch].idle;
      if(diff > 0) sprintf(buf, "%3d:%02d", diff / 60, diff % 60);
      else buf[0] = '\0';
#else
      buf[0] = '\0';
#endif

#ifdef SHOWPID
      if(show_pid) sprintf(buf, "%6d", uentp->pid);
#endif

      state = (currutmp == uentp) ? 6 : pklist[ch].friend;

      hate = is_rejected(uentp);
      if(PERM_HIDE(uentp)) {
         if (hate & HRM && is_friend(uentp) & 2) {
            hate = 0;
            state = 5;
         }
         else state = PERM_HIDE(currutmp) ? 5 : 0;
      }
      else if(is_friend(uentp) & 2) hate &= 1;

      /* Dopin */
      if ((hate & IRH) && !(is_friend(uentp) & 1)) state = 4;
      diff = uentp->pager & !(hate & HRM);

#ifdef SHOW_FROMHOST /* Dopin 11/06/02 */
         prints("%5d %c%c%s%-12.11s%-18.17s%s%-15.14s%-11.10s%-9.8s%s\n",
#else
         prints("%5d %c%c%s%-12.11s%-29.28s%s%-12.11s%-11.10s%s\n",
#endif

#ifdef SHOWUID
        show_uid ? uentp->uid :
#endif
        (ch + 1),
        (hate & HRM)? 'X' : (uentp->pager == 3) ? 'W' :
        (uentp->pager == 2) ? '-' : pagerchar[(state & 2) | diff],
        (uentp->invisible ? ')' : ' '),
        (hate & IRH)? fcolor[4] : fcolor[state], uentp->userid,

#ifdef REALINFO
        real_name ? uentp->realname :
#endif
        uentp->username, "[0m",

#ifdef SHOW_FROMHOST
        show_friend ? friend_descript(uentp->userid) :
#ifdef TRANS_FROMHOST
        /* Dopin: 喜好設定不提供來源則不顯示 */
        uentp->welcomeflag & 64 ? "*" : where(uentp->from),
#else
        /* Dopin: 喜好設定不提供來源則不顯示 */
        uentp->welcomeflag & 64 ? "*" : uentp->from,
#endif
#endif

#ifdef SHOWTTY
        show_tty ? uentp->tty :
#endif
        modestring(uentp, 0),
#ifndef NO_USE_MULTI_STATION
        station_list[uentp->now_stno],
#else
        uentp->kimochi,
#endif
        buf);
    }
    if(state == US_PICKUP) continue;

    move(b_lines, 0);
#ifdef USE_MULTI_TITLE
    prints("[1;37;%2dm     [0;31;47m (TAB/f)[30m排序/好友 [31m(t)[30m"
           "聊天 [31m(a/d/o)[30m交友 [31m(q)[30m查詢 [31m(w)[30m扣應 "
           "[31m(m)[30m寄信 [31m(h)[30m線上輔助 [m", title_color);
#else
    outs("[1;37;46m     [0;31;47m (TAB/f)[30m排序/好友 [31m(t)[30m聊天"
         "[31m(a/d/o)[30m交友 [31m(q)[30m查詢 [31m(w)[30m扣應 "
         "[31m(m)[30m寄信 [31m(h)[30m線上輔助 [m");
#endif

    state = 0;
    while(!state) {
      ch = cursor_key(num + 3 - head, 0);
      if(ch == KEY_RIGHT || ch == '\n' || ch == '\r') ch = 't';

      switch (ch) {
      case KEY_LEFT:
      case 'e':
      case 'E': return;

#ifdef CTRL_G_REVIEW
      case Ctrl('G'):
        DL_func("bin/message.so:get_msg", 1);
        state = US_PICKUP;
        break;
#endif

      case KEY_TAB:
        if(++pickup_way >= PICKUP_WAYS) pickup_way = 0;
        state = US_RESORT;
        num = 0;
        break;

      case KEY_DOWN:
      case 'n':
        if(++num < actor) {
          if(num >= foot) state = US_REDRAW;
          break;
        }

      case '0':
      case KEY_HOME:
        num = 0;
        if(head) state = US_REDRAW;
        break;

      case 'N':
         if(HAS_PERM(PERM_BASIC)) {
            char buf[100];

            sprintf(buf, "暱稱 [%s]：", currutmp->username);
            if(!getdata(1, 0, buf, currutmp->username, 24, DOECHO,
               currutmp->username))
               strcpy(currutmp->username, cuser.username);
            else {
               if(strcmp(currutmp->username, cuser.username)) {
                  out2line(1, 1, "");

                  getdata(1, 0, "將更改後的暱稱一併寫入個人資料 y/N ? ", buf,
                          2, LCECHO, 0);
                  if(buf && buf[0] == 'y') {
                     strcpy(cuser.username, currutmp->username);
                     item_update_passwd('l');
                     strcpy(xuser.username, currutmp->username);
                     item_update_passwd('u');
                  }
               }
            }
            state = US_PICKUP;
         }
         break;

      case 'H':
         if(HAS_PERM(PERM_SYSOP)) {
            currutmp->userlevel ^= PERM_DENYPOST;
            state = US_PICKUP;
         }
         break;

      case 'l':
         if(HAS_PERM(PERM_BASIC)) {
           t_display();
           state = US_PICKUP;
         }
         break;

      case 'C':
         if(HAS_PERM(PERM_SYSOP)) {
           char buf[100];

           sprintf(buf, "代號 [%s]：", currutmp->userid);
           if(!getdata(1, 0, buf, currutmp->userid, IDLEN + 1, DOECHO,
             currutmp->userid)) strcpy(currutmp->userid, cuser.userid);

           state = US_PICKUP;
         }
         break;

#ifdef SHOW_FROMHOST
      case 'F':   /* 同理 權限請自行設定 預設可設定為 PERM_LOGINOK */
         if(HAS_PERM(
#ifdef USER_DEFINE_HOST
                      PERM_LOGINOK
#else
                      PERM_SYSOP
#endif
         )) {
           sprintf(buf, "故鄉 [%s]：", currutmp->from);
           getdata(1,0, buf, currutmp->from, 29, DOECHO, currutmp->from);
           state = US_PICKUP;
         }
         break;
#endif

      case ' ':
      case KEY_PGDN:
      case Ctrl('F'):
        if(foot < actor) {
          num += p_lines;
          state = US_REDRAW;
          break;
        }

        if(head) num = 0;
        state = US_PICKUP;
        break;

      case KEY_UP:
        if(--num < head) {
          if(num < 0) {
            num = actor - 1;
            if(actor == foot) break;
          }
          state = US_REDRAW;
        }
        break;

      case KEY_PGUP:
      case Ctrl('B'):
      case 'P':
        if(head) {
          num -= p_lines;
          state = US_REDRAW;
          break;
        }

      case KEY_END:
      case '$':
        num = actor - 1;
        if(foot < actor) state = US_REDRAW;
        break;

      case '/': {
        int tmp;
        if((tmp = search_pickup(num, actor, pklist)) >= 0) num = tmp;
        state = US_REDRAW;
      }
      break;

      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9': {                       /* Thor: 可以打數字跳到該人 */
        int tmp;
        if((tmp = search_num(ch, actor - 1)) >= 0) num = tmp;
        state = US_REDRAW;
      }
      break;

#ifdef  REALINFO
      case 'R':         /* 顯示真實姓名 */
        if(HAS_PERM(PERM_SYSOP)) real_name ^= 1;
        state = US_PICKUP;
        break;
#endif
#ifdef  SHOWUID
      case 'U':
        if(HAS_PERM(PERM_SYSOP)) show_uid ^= 1;
        state = US_PICKUP;
        break;
#endif
#ifdef  SHOWTTY
      case 'Y':
        if(HAS_PERM(PERM_SYSOP)) show_tty ^= 1;
        state = US_PICKUP;
        break;
#endif
#ifdef  SHOWPID
      case 'I':
        if(HAS_PERM(PERM_SYSOP)) show_pid ^= 1;
        state = US_PICKUP;
        break;
#endif

#ifdef  NO_USE_MULTI_STATION
      case 'k':     /* 這裡權限可改成其他的 */
        if(HAS_PERM(PERM_BASIC)) {
          char buf[20];
          sprintf(buf, "心情 [%s]：", currutmp->kimochi);
          getdata(1,0, buf, currutmp->kimochi, 13, DOECHO, currutmp->kimochi);
          state = US_PICKUP;
        }
        break;
#endif

      case 'b':         /* broadcast */
        if(cuser.uflag & FRIEND_FLAG || HAS_PERM(PERM_SYSOP)) {
          int actor_pos = actor;
          char ans[4];

          state = US_PICKUP;
          if(!getdata(0, 0, "廣播訊息:", genbuf + 1, 60, DOECHO, 0)) break;
          if(getdata(0, 0, "確定廣播? [Y]", ans, 4, LCECHO, 0) && *ans == 'n')
             break;

          genbuf[0] = HAS_PERM(PERM_SYSOP) ? 2 : 0;
          while (actor_pos) {
            uentp = pklist[--actor_pos].ui;

            if(uentp->pid && currpid != uentp->pid && kill(uentp->pid, 0) != -1
               && (HAS_PERM(PERM_SYSOP) || uentp->pager < 3))
               my_write(uentp->pid, genbuf);
          }
        }
        break;

      case 's':         /* 顯示好友描述 */
        show_friend ^= 1;
        state = US_PICKUP;
        break;

      /* case 'u': */        /* 線上修改資料 */

      case 'K':         /* 把壞蛋踢出去 */
        if(!HAS_PERM(PERM_SYSOP)
#ifdef USE_ACCOUNTS_PERM
           && !HAS_PERM(PERM_ACCOUNTS)
#endif
        ) continue;

        uentp = pklist[num].ui;
        state = US_ACTION;
        break;

      case 't':
      case 'w':
        if(!(cuser.userlevel & PERM_PAGE)) continue;
        uentp = pklist[num].ui;
        state = US_ACTION;
        break;

      case 'a':
      case 'A':
      case 'd':
      case 'D':
      case 'o':
      case 'O':
      case 'f':
        if(!HAS_PERM(PERM_LOGINOK)) break; /* 註冊才有 Friend */
        if(ch == 'f') {
          cuser.uflag ^= FRIEND_FLAG;
          state = US_PICKUP;
          break;
        }
        uentp = pklist[num].ui;
        state = US_ACTION;
        break;

      case 'q':
      case 'm':
      case 'r': /* guest 只能 query */
        if(bad_user(cuser.userid) || !cuser.userlevel) break;
        uentp = pklist[num].ui;

      case 'h':
        state = US_ACTION;
        break;

#ifdef CHOICE_RMSG_USER
      case Ctrl('V'):
         choice_water_id();
         state = US_PICKUP;
         break;
#endif

      case Ctrl('R'):
         DL_func("bin/message.so:get_msg", 0);
         state = US_PICKUP;
         break;

      case 'p':
         if(HAS_PERM(PERM_BASIC)) {
            t_pager();
            state = US_PICKUP;
         }
         break;

      case KEY_ESC:
         if(KEY_ESC_arg == 'c') capture_screen();
         else if (KEY_ESC_arg == 'n') {
            edit_note();
            state = US_PICKUP;
         }
         break;

      default:          /* refresh user state */
        state = US_PICKUP;
      }
    }

    if(state != US_ACTION) continue;

    if(ch == 'w') {
      if((uentp->pid != currpid) &&
        (HAS_PERM(PERM_SYSOP) || uentp->pager != 3)) {
        cursor_show(num + 3 - head, 0);
        my_write(uentp->pid, "熱線 Call-In：");
      }
      else state = 0;
    }
    else {
      switch(ch) {
      case 'r':
        m_read();
        break;
      case 'a':
      case 'A':
        friend_add(uentp->userid,
                  (ch == 'a') ? FRIEND_OVERRIDE : FRIEND_REJECT);
        friend_load();
        state = US_PICKUP;
        break;

      case 'd':
      case 'D':
        sprintf(genbuf, "確定把 %s 從 [%s] 移除嗎(Y/N)?[N]",
                        uentp->userid, friend_list[(ch == 'd') ? 0 : 1]);
        if(getdata(1, 0, genbuf, genbuf, 4, LCECHO, 0) && *genbuf == 'y') {
          friend_delete(uentp->userid,
                        (ch == 'd') ? FRIEND_OVERRIDE : FRIEND_REJECT);
          friend_load();
        }
        state = US_PICKUP;
        break;

      case 'o':
      case 'O':
        if(ch == 'o') t_override();
        else t_reject();
        state = US_PICKUP;
        break;

      case 'K':
        if(uentp->pid && (kill(uentp->pid, 0) != -1)) {
          move(1, 0);
          clrtobot();
          move(2, 0);
          my_kick(uentp);
          state = US_PICKUP;
        }
        break;

      case 'm':
        stand_title("寄  信");
        prints("收信人：%s", uentp->userid);
        my_send(uentp->userid);
        break;

      case 'q':
        strcpy(currauthor, uentp->userid);
        my_query(uentp->userid);
        break;

      case 'u':         /* Thor: 可線上查看及修改使用者 */
        {
          int id;
          userec muser;
          strcpy(currauthor, uentp->userid);
          stand_title("使用者設定");

          move(1, 0);
          if(id = getuser(uentp->userid)) {
            memcpy(&muser, &xuser, sizeof(muser));
            user_display(&muser, 1);
            uinfo_query(&muser, 1, id);
          }
        }
        break;

      case 'h':  /* Thor: 看 Help */ /* Dopin: 改用外部呼叫 */
        DL_func("bin/help.so:help", CHATING);
        break;

      case 't':
        if(uentp->pid != currpid) {
          move(1, 0);
          clrtobot();
          move(3, 0);
          my_talk(uentp);
          state = US_PICKUP;
        }
      }
    }
    setutmpmode(savemode);
  }
}

int l_post_msg(void) {
   int i, j, k, edit=0;
   char fn[80], fnnew[80] ={0};
   char genbuf[20] = {0};
   char guest_list[257][IDLEN+1] = {0};
   FILE *fp;

   clear();
   stand_title("編輯來賓名單 (上限 256 名");

   if(!is_BM(currBM + 6) && !is_board_manager(currboard)) return FULLUPDATE;

LOAD_PERM:
   sprintf(fn, "boards/%s/permlist", currboard);
   fp = fopen(fn, "r");

   i = 0;
   if(fp) {
      while(!feof(fp) && !ferror(fp) && i < 256) {
         fgets(fnnew, 80, fp);
         if(!feof(fp) && !ferror(fp)) {
            for(k = 0 ; k < IDLEN + 1 && (fnnew[k] != 10 && fnnew[k] != ' ')
                ; k++);
            fnnew[k] = 0;
            strcpy(guest_list[i], fnnew);
            i++;
         }
         else break;
      }
      fclose(fp);
   }

SHOW_PM:
   CreateNameList();
   move(3, 0);
   for(j = 0 ; j <= i ; j++) {
      if(!guest_list[j] || guest_list[j][0] == ' ') {
         continue;
         guest_list[j][0] = 0;
      }

      prints("%-13s", guest_list[j]);
      AddNameList(guest_list[j]);
      if(j % 6 == 5) outc('\n');
   }
   getdata(1, 0, "(A)增加 (D)刪除 (E)編輯 (Q)結束？[Q] ",
           genbuf, 2, DOECHO, 0);

   switch(genbuf[0]) {
      case 'E' :
      case 'e' :
         b_perm_edit();
         edit = 2;
         goto LOAD_PERM;

      case 'A' :
      case 'a' :
         move(1, 0);
         clrtoeol();
         move(1, 0);
         usercomplete(msg_uid, genbuf);
         if(*genbuf) {
            strcpy(guest_list[i], genbuf);
            AddNameList(guest_list[i]);
            i++;
            edit = 1;
         }
         goto SHOW_PM;

      case 'd' :
      case 'D' :
         if(!i) goto SHOW_PM;
         out2line(1, 1, "");

         namecomplete(msg_uid, genbuf);
         if(*genbuf) {
            for(k=0 ; k <= i ; k++) if(!strcmp(genbuf, guest_list[k])) break;

            for(j = k ; j < i ; j++) strcpy(guest_list[j], guest_list[j+1]);
            guest_list[i][0] = 0;
            i--;
            edit = 1;
         }
         goto SHOW_PM;

      default:
         if(edit == 1) {
            sprintf(fnnew, "boards/%s/permlist1", currboard);
            fp = fopen(fnnew, "w+");
            for(j = 0 ; j <= i ; j++)
               if(guest_list[j][0]) fprintf(fp, "%s\n", guest_list[j]);
            fclose(fp);

            Rename(fnnew, fn);
         }
      }

   return FULLUPDATE;
}

static int listcuent(user_info *uentp) {
  if((uentp->uid != usernum) && (!uentp->invisible || HAS_PERM(PERM_SYSOP) ||
     HAS_PERM(PERM_SEECLOAK)))
     AddNameList(uentp->userid);

  return 0;
}

static void creat_list() {
  CreateNameList();
  apply_ulist(listcuent);
}

int t_users() {
  int destuid0 = currutmp->destuid;
  int mode0 = currutmp->mode;
  int stat0 = currstat;
  static int enter;
  char ch[2] = "";

  if(chkmailbox() || enter > 2) return;

  ++enter;
  setutmpmode(LUSERS);
  pickup_user();
  currutmp->mode = mode0;
  currutmp->destuid = destuid0;
  currstat = stat0;
  --enter;
  return 0;
}

int t_pager() {
  currutmp->pager = (currutmp->pager + 1) % 4;
  return 0;
}

#ifdef TAKE_IDLE
int t_idle() {
  int destuid0 = currutmp->destuid;
  int mode0 = currutmp->mode;
  int stat0 = currstat;
  char genbuf[20];

  setutmpmode(IDLE);
  getdata(b_lines - 1, 0, "理由：[0]發呆 (1)接電話 (2)覓食 (3)打瞌睡 "
          "(4)裝死 (5)羅丹 (6)其他 (Q)沒事？", genbuf, 3, DOECHO, 0);
  if(genbuf[0] == 'q' || genbuf[0] == 'Q') {
     currutmp->mode = mode0;
     currstat = stat0;
     return 0;
  }
  else if(genbuf[0] >= '1' && genbuf[0] <= '6')
    currutmp->destuid = genbuf[0] - '0';
  else currutmp->destuid = 0;

  if(currutmp->destuid == 6)
    if(!cuser.userlevel || !getdata(b_lines - 1, 0, "發呆的理由：",
      currutmp->chatid, 11, DOECHO, 0))
      currutmp->destuid = 0;


  {
     char buf[80];

     move(b_lines - 1, 0);
     clrtoeol();
     sprintf(buf, "發呆中: %s", (currutmp->destuid != 6) ?
             IdleTypeTable[currutmp->destuid] : currutmp->chatid);
     outs(buf);
     refresh();
     igetch();
  }
  currutmp->mode = mode0;
  currutmp->destuid = destuid0;
  currstat = stat0;

  return 0;
}
#endif

int t_query() {
  char uident[STRLEN];

  stand_title("查詢網友");
  usercomplete(msg_uid, uident, 0);
  if(uident[0]) my_query(uident);
  return 0;
}

int t_talk() {
  char uident[16];
  int tuid, unum, ucount;
  user_info *uentp;
  char genbuf[4];

  if(count_ulist() <= 1) {
    outs("目前線上只有您一人，快邀請朋友來光臨【 " BOARDNAME " 】吧！");
    return XEASY;
  }
  stand_title("打開話匣子");
  creat_list();
  namecomplete(msg_uid, uident);
  if(uident[0] == '\0') return 0;

  move(3, 0);
  if(!(tuid = searchuser(uident)) || tuid == usernum) {
    pressanykey(err_uid);
    return 0;
  }

  /* ----------------- */
  /* multi-login check */
  /* ----------------- */

  unum = 1;
  while ((ucount = count_logins(cmpuids, tuid, 0)) > 1) {
    outs("(0) 不想 talk 乏...\n");
    count_logins(cmpuids, tuid, 1);
    getdata(1, 33, "請選擇一個聊天對象 [0]：", genbuf, 4, DOECHO, 0);
    unum = atoi(genbuf);
    if(unum == 0) return 0;

    move(3, 0);
    clrtobot();
    if(unum > 0 && unum <= ucount) break;
  }

  if(uentp = (user_info *) search_ulistn(cmpuids, tuid, unum)) my_talk(uentp);

  return 0;
}

/* ------------------------------------- */
/* 有人來串門子了，回應呼叫器            */
/* ------------------------------------- */

user_info *uip;

void talkreply() {
  int a;
  struct hostent *h;
  char hostname[STRLEN], buf[4];
  struct sockaddr_in sin;
  char genbuf[200];
  int i;

  uip = currutmp->destuip;
  sprintf(page_requestor, "%s (%s)", uip->userid, uip->username);
  currutmp->destuid = uip->uid;
  currstat = XMODE;             /* 避免出現動畫 */

  clear();
  outs("\n\n\
       (Y) 讓我們 talk 吧！     (A) 我現在很忙，請等一會兒再 call 我\n\
       (N) 我現在不想 talk      (B) 對不起，我有事情不能跟你 talk\n\
       (C) 請不要吵我好嗎？     (D) 你真的很煩，我實在不想跟你 talk\n\
       (E) 有事嗎？請先來信     (F) 我自己輸入理由好了...\n\n");

  getuser(uip->userid);
  currutmp->msgs.last_pid = uip->pid;
  strcpy(currutmp->msgs.last_userid, uip->userid);
  strcpy(currutmp->msgs.last_call_in, "呼叫、呼叫，聽到請回答 (Ctrl-R)");
  prints("對方來自 [%s]，共來訪 %d 次，文章 %d 篇\n",
    uip->from, xuser.numlogins, xuser.numposts);
  showplans(uip->userid);
  show_last_call_in();

     sprintf(genbuf, "你想跟 %s 聊聊天嗎？請選擇(Y/N/A/B/C/D)[Y] ", page_requestor);
  getdata(0, 0, genbuf, buf, 4, LCECHO, 0);

  if (uip->mode != PAGE) {
     sprintf(genbuf, "%s已停止呼叫，按Enter繼續...", page_requestor);
     getdata(0, 0, genbuf, buf, 4, LCECHO, 0);
     return;
  }

  /* DOPIN REMARK */ /*
  currutmp->msgcount = 0; */
  strcpy(save_page_requestor, page_requestor);
  memset(page_requestor, 0, sizeof(page_requestor));
  gethostname(hostname, STRLEN);

  if (!(h = gethostbyname(hostname)))
  {
    perror("gethostbyname");
    return;
  }
  memset(&sin, 0, sizeof sin);
  sin.sin_family = h->h_addrtype;
  memcpy(&sin.sin_addr, h->h_addr, h->h_length);
  sin.sin_port = uip->sockaddr;
  a = socket(sin.sin_family, SOCK_STREAM, 0);
  if ((connect(a, (struct sockaddr *) & sin, sizeof sin)))
  {
    perror("connect err");
    return;
  }
/* Dopin
  if (!buf[0] || !strchr("abcdn", buf[0]))  */
  if (!buf[0] || !strchr("abcdefn", buf[0]))
    buf[0] = 'y';
  write(a, buf, 1);
   if (buf[0] == 'f' || buf[0] == 'F')
   {
     if (!getdata(b_lines, 0, "不能 talk 的原因：", genbuf, 60, DOECHO,0))
       strcpy(genbuf, "不告訴你咧 !! ^o^");
     write(a, genbuf, 60);
   }
  if (buf[0] == 'y')
     do_talk(a);
  else
     close(a);
  clear();
}


/* ------------------------------------- */
/* 網友動態簡表                          */
/* ------------------------------------- */


int
shortulist(uentp)
  user_info *uentp;
{
  static int lineno, fullactive, linecnt;
  static int moreactive, page, num;
  char uentry[50];
  int state;

  if (!lineno)
  {
    lineno = 3;
    page = moreactive ? (page + p_lines * 3) : 0;
    linecnt = num = moreactive = 0;
    move(1, 70);
    prints("Page: %d", page / (p_lines) / 3 + 1);
    move(lineno, 0);
  }
  if (uentp == NULL)
  {
    int finaltally;

    clrtoeol();
    move(++lineno, 0);
    clrtobot();
    finaltally = fullactive;
    lineno = fullactive = 0;
    return finaltally;
  }
  if ((!HAS_PERM(PERM_SYSOP) && !HAS_PERM(PERM_SEECLOAK) && uentp->invisible) ||
      ((is_rejected(uentp) & HRM) && !HAS_PERM(PERM_SYSOP)))
  {
    if (lineno >= b_lines)
      return 0;
    if (num++ < page)
      return 0;
    memset(uentry, ' ', 25);
    uentry[25] = '\0';
  }
  else
  {
    fullactive++;
    if (lineno >= b_lines)
    {
      moreactive = 1;
      return 0;
    }
    if (num++ < page)
      return 0;

    state = (currutmp == uentp) ? 6 : is_friend(uentp);

    if (PERM_HIDE(uentp))
       if (is_rejected(uentp) & HRM) {
          state = 5;
       }
       else
          state = (is_rejected(uentp) & HRM || PERM_HIDE(currutmp)) ? 5 : 0;

    if (PERM_HIDE(currutmp) && state == 3)
       state = 6;

    sprintf(uentry, "%s%-13s%c%-10s%s ", fcolor[state],
      uentp->userid, uentp->invisible ? '#' : ' ',
      modestring(uentp, 1), state ? "[0m" : "");
  }
  if (++linecnt < 3)
  {
    strcat(uentry, "│");
    outs(uentry);
  }
  else
  {
    outs(uentry);
    linecnt = 0;
    clrtoeol();
    move(++lineno, 0);
  }
  return 0;
}


static void
do_list(modestr)
  char *modestr;
{
  int count;

  showtitle(modestr, BoardName);
  if (currstat == MONITOR)
    prints("每隔 %d 秒更新一次，請按[Ctrl-C]或[Ctrl-D]離開", M_INT);

  outc('\n');
  outs(msg_shortulist);

  friends_number = override_number = 0;
  if (apply_ulist(shortulist) == -1)
  {
    outs(msg_nobody);
  }
  else
  {
    time_t thetime = time(NULL);

    count = shortulist(NULL);
    move(b_lines, 0);
    prints("[1;37;46m  上站總人數：%-7d[32m我的朋友：%-6d"
      "[33m與我為友：%-8d[30m%-23s[37;40;0m",
      count, friends_number, override_number, Cdate(&thetime));
    refresh();
  }
}


int
t_list()
{
  setutmpmode(LUSERS);
  do_list("使用者狀態");
  igetch();
  return 0;
}


/* ------------------------------------- */
/* 監看使用情形                          */
/* ------------------------------------- */

int idle_monitor_time;

static void
sig_catcher()
{
  if (currstat != MONITOR)
  {

#ifdef DOTIMEOUT
    init_alarm();
#else
    signal(SIGALRM, SIG_IGN);
#endif

    return;
  }
  if (signal(SIGALRM, sig_catcher) == SIG_ERR)
  {
    perror("signal");
    exit(1);
  }

#ifdef DOTIMEOUT
  if (!(PERM_HIDE(currutmp) || currutmp->mode == MAILALL))
     idle_monitor_time += M_INT;
  if (idle_monitor_time > MONITOR_TIMEOUT)
  {
    clear();
    fprintf(stderr, "timeout\n");
    abort_bbs();
  }
#endif

  do_list("追蹤站友");
  alarm(M_INT);
}


int
t_monitor()
{
  char c;
  int i;

  setutmpmode(MONITOR);
  alarm(0);
  signal(SIGALRM, sig_catcher);
  idle_monitor_time = 0;

  do_list("追蹤站友");
  alarm(M_INT);
  while (YEA)
  {
    i = read(0, &c, 1);
    if (!i || c == Ctrl('D') || c == Ctrl('C'))
      break;
    else if (i == -1)
    {
      if (errno != EINTR)
      {
        perror("read");
        exit(1);
      }
    }
    else
      idle_monitor_time = 0;
  }
  return 0;
}


/* ------------------------------------- */
/* 好友名單、黑名單                      */
/* ------------------------------------- */


void friend_load() {
  FILE *fp;
  ushort myfriends[MAX_FRIEND];
  ushort myalohas[MAX_ALOHA]; /* Dopin 08/14/02 */
  ushort myrejects[MAX_REJECT];
  char genbuf[200];

  friend_file[0] = fn_overrides;
  friend_file[1] = fn_reject;
  friend_file[2] = fn_board_favors;
  friend_file[3] = fn_call_msg;

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

  memset(myalohas, 0, sizeof(myalohas));
  aloha_number = 0;
  setuserfile(genbuf, fn_call_msg);
  if(fp = fopen(genbuf, "r")) {
     ushort unum;

     while(fgets(genbuf, STRLEN, fp) && aloha_number < MAX_ALOHA - 2)
        if(strtok(genbuf, str_space))
           if(unum = searchuser(genbuf))
              myalohas[aloha_number++] = (ushort) unum;

     fclose(fp);
  }
  memcpy(currutmp->aloha, myalohas, sizeof(myalohas));
}

#ifndef NO_FORCE_LISTS
int maxns[4] = { MAX_FRIEND, MAX_REJECT, MAX_BOARDFAVOR, MAX_ALOHA };
int check[4] = { FRIEND_OVERRIDE, FRIEND_REJECT, BOARD_FAVOR, CALL_MSG };
#endif

void friend_edit(int type) {
  char fpath[80], line[80], uident[20], *msg;
  int count, column, dirty, ct;
  FILE *fp;
  char genbuf[200];

  setuserfile(fpath, friend_file[type]);
  msg = friend_list[type];

  dirty = 0;
  while(1) {
    stand_title(msg);

    count = 0;
    CreateNameList();
    for(ct = 0; ct < brdnum; ct++) AddNameList(nbrd[ct].name);

    if(fp = fopen(fpath, "r")) {
      move(3, 0);
      column = 0;

      while(fgets(genbuf, STRLEN, fp)) {
        if(genbuf[0] <= ' ') continue;

        strtok(genbuf, str_space);
        AddNameList(genbuf);
        prints("%-13s", genbuf);
        count++;

        if(++column > 5) {
          column = 0;
          outc('\n');
        }
      }

      fclose(fp);
    }

    getdata(1, 0, count ? "(A)增加 (D)刪除 (L)詳細列出 (E)編輯 (Q)結束？[Q] "
                   : "(A)增加 (Q)結束？[Q] ", uident, 4, LCECHO, 0);
    if(*uident == 'a') {
      move(1, 0);
      if(type != 2) usercomplete(msg_uid, uident, 0);
      else namecomplete("請輸入看板名稱 : ", uident);

      if(type != 2) {
         if(uident[0] && searchuser(uident) && !InNameList(uident)) {
           friend_add(uident, type);
           dirty = 1;
         }
      }
      else if(uident[0]) {
         friend_add(uident, type);
         dirty = 1;
      }
    }
    else if((*uident == 'd') && count) {
      move(1, 0);
      if(type != 2) namecomplete(msg_uid, uident);
      else namecomplete("請輸入看板名稱 : ", uident);

      if(type != 2) {
         if(uident[0] && InNameList(uident)) {
           friend_delete(uident, type);
           dirty = 1;
         }
      }
      else if(uident[0]) {
         friend_delete(uident, type);
         dirty = 1;
      }
    }
    else if((*uident == 'l') && count) {
      strcpy(vetitle, "[好友名單]");
      more(fpath, YEA);
    }
    else if(*uident == 'e') {
       vedit(fpath, NA);
       dirty = 1;
    }
    else {
#ifndef NO_FORCE_LISTS
       if(check_file_line(fpath) > maxns[type]) {
          pressanykey("%s 超出上限 請適度刪除", friend_list[type]);
          continue;
       }
       else
#endif
          break;
    }

  }
  if(dirty) friend_load();
}

int t_override() {
  friend_edit(FRIEND_OVERRIDE);
#ifdef CHECK_LIST_ID_SELF
    check_listname();
#endif
  return 0;
}

int t_reject() {
  friend_edit(FRIEND_REJECT);
#ifdef CHECK_LIST_ID_SELF
    check_listname();
#endif
  return 0;
}

/*
int l_call_msg(void) {
friend_edit(CALL_MSG);
#ifdef CHECK_LIST_ID_SELF
check_listname();
#endif
return FULLUPDATE;
}
*/

#ifndef NO_FORCE_LISTS
void force_lists(void) {
   char fpath[100];
   char *files[] = { fn_overrides, fn_reject, fn_call_msg };
   int i, j;

   clear();
   for(i = 0 ; i < 3 ; i++) {
      sethomefile(fpath, cuser.userid, files[i]);
      if(check_file_line(fpath) > maxns[i]) {
         pressanykey("%s 人數超過系統上限 請刪除部份名單",
                     friend_list[check[i]]);
         friend_edit(check[i]);
      }
   }
}
#endif

int t_friends() {
  char ch[2] = "";

  if(friendcount) {
    setutmpmode(FRIEND);

    pickup_user();

    if (!(friends_number || override_number)) {
      outs("再等一下吧，也許\他們待會兒就上線了。");
      return XEASY;
    }
    return 0;
  }
  else {
    outs("目前還沒設定好友名單");
    return XEASY;
  }
}

void t_aloha(char mode) {
   int i;
   user_info *uentp;
   pid_t pid;
   char buf[100];

   if(currutmp->invisible) return; /* Dopin: 這樣比較快 免得判斷多浪費時間 */

   sprintf(buf + 1, "☆ %s(%s) %s冒險了!", cuser.userid, cuser.username,
           mode == 'I' ? "開始" : "停止");
   *buf = 0;

   for (i = 0; i < USHM_SIZE; i++) {
      uentp = &utmpshm->uinfo[i];

      if((pid = uentp->pid) && (kill(pid, 0) != -1)) {
         /* Thor: 特別注意, 自己上站不會通知自己... */
         if(!strcmp(uentp->userid, cuser.userid))
            continue;

         /* Dopin 如果關上線通知不送 08/13/02 */
         if(is_aloha(uentp) && !(uentp->welcomeflag & 1) &&
            !(is_rejected(uentp) & 1)) /*add by drange 910415 for 壞人不送*/
              my_write(uentp->pid, buf);
      }
   }
}

void my_outmsg(char* msg)  {
  if(!*msg) return; /* Dopin: 對抗 空字串/空訊息 專用 O.O;;; */
  /* Dopin: 如果有設定不報時 就跳出 */
  if(strstr(msg, " [報時] ")) if(cuser.welcomeflag & 32) return;

  out2line(1, b_lines, msg);
}

#ifdef RPG_FIGHT
/* Dopin 12/01/98 */
void clrp1p2(void) {

   uschar i,j;

for(i=0 ; i<STRLEN-1 ; i++) {
uident1[i] = 0;
uident2[i] = 0;
}

p1.nowlevel = 0;
p1.hp = 0;
p1.str = 0;
p1.mgc = 0;
p1.skl = 0;
p1.spd = 0;
p1.luk = 0;
p1.def = 0;
p1.mdf = 0;

for(i=0 ; i<6 ; i++)
p1.spcskl[i] = 0;

for(i=0 ; i<2 ; i++)
for(j=0 ; j<10; j++)
p1.wepnlv[i][j] = 0;

p1.curwepnlv[0][0] = 0;
p1.curwepnlv[1][0] = 0;
p1.curwepnatt = 0;
p1.curwepnhit = 0;
p1.curwepnweg = 0;

for(i=0 ; i<4 ; i++)
p1.curwepnspc[i] = 0;


p2.nowlevel = 0;
p2.hp = 0;
p2.str = 0;
p2.mgc = 0;
p2.skl = 0;
p2.spd = 0;
p2.luk = 0;
p2.def = 0;
p2.mdf = 0;

for(i=0 ; i<6 ; i++)
p2.spcskl[i] = 0;

for(i=0 ; i<2 ; i++)
for(j=0 ; j<10; j++)
p2.wepnlv[i][j] = 0;

p2.curwepnlv[0][0] = 0;
p2.curwepnlv[1][0] = 0;
p2.curwepnatt = 0;
p2.curwepnhit = 0;
p2.curwepnweg = 0;

for(i=0 ; i<4 ; i++)
p2.curwepnspc[i] = 0;

}

int init(void) {
   clrp1p2();

   stand_title("PK 模擬戰 Beta Testing Ver. 0.21");
   prints("\n[1;37;42m遊戲方法：[m 請輸入欲使之對戰之使用者帳號[m\n");
   prints("           (請確定至少其一已轉職)\n");
   prints("[1;37;45m注意事項：[m 此兩人需至少有一人已裝備武器[m\n\n");
   prints("測試過程若已上兩條件皆符合但程式不當斷線,請寫信給 Dopin\n");

   /* Dopin 071301 */
   usercomplete(msg_uid, uident1, 0);
   /* Dopin */
   usercomplete(msg_uid, uident2, 0);

   /* Dopin 071301 */
   /*
   strcpy(uident2, cuser.userid);
   */
   if(!strcmp(uident1, uident2)) {
      prints("不可與自己對戰 !!\n");
      return 0;
   }
   /* Dopin */

   if (uident1[0] && uident2[0]) {
      player_input();
      return 1;
   }
   else
      return 0;
}

int player_input(void) {
int tuid,i,j;

if (tuid = getuser(&uident1)) {
clrtobot();
currutmp->destuid = tuid;

appendstatus[0] = xuser.str;
appendstatus[1] = xuser.mgc;
appendstatus[2] = xuser.skl;
appendstatus[3] = xuser.spd;
appendstatus[4] = xuser.luk;
appendstatus[5] = xuser.def;
appendstatus[6] = xuser.mdf;

p1maxhp = xuser.hp;
p1.nowlevel = xuser.nowlevel;
p1.hp = xuser.hp;
p1.str = xuser.str;
p1.mgc = xuser.mgc;
p1.skl = xuser.skl;
p1.spd = xuser.spd;
p1.luk = xuser.luk;
p1.def = xuser.def;
p1.mdf = xuser.mdf;
p1.curwepnatt = xuser.curwepnatt;
p1.curwepnhit = xuser.curwepnhit;
p1.curwepnweg = xuser.curwepnweg;

for(i=0 ; i<20 ; i++)
p1.working[i] = xuser.working[i];
for(i=0 ; i<6 ; i++)
p1.spcskl[i] = xuser.spcskl[i];

for(i=0 ; i<2 ; i++)
for(j=0 ; j<10 ; j++)
p1.wepnlv[i][j] = xuser.wepnlv[i][j];

p1.curwepnlv[0][0] = xuser.curwepnlv[0][0];
p1.curwepnlv[1][0] = xuser.curwepnlv[1][0];

for(i=0 ; i<4 ; i++)
p1.curwepnspc[i] = xuser.curwepnspc[i];

p1.commander = xuser.commander;

if(xuser.curwepnspc[0] > 0) {
if(!(p1.curwepnspc[1] & 128)) {

j = 64;
for(i=0 ; i<7 ; i++) {

if(p1.curwepnspc[1] & j)
if(p1.curwepnspc[2] & j)
appendstatus[i] += 20;
else
appendstatus[i] += 10;
j /= 2;
}
p1.str = appendstatus[0];
p1.mgc = appendstatus[1];
p1.skl = appendstatus[2];
p1.spd = appendstatus[3];
p1.luk = appendstatus[4];
p1.def = appendstatus[5];
p1.mdf = appendstatus[6];
}
}
}

if (tuid = getuser(&uident2))
{
clrtobot();
currutmp->destuid = tuid;

appendstatus1[0] = xuser.str;
appendstatus1[1] = xuser.mgc;
appendstatus1[2] = xuser.skl;
appendstatus1[3] = xuser.spd;
appendstatus1[4] = xuser.luk;
appendstatus1[5] = xuser.def;
appendstatus1[6] = xuser.mdf;

p2maxhp = xuser.hp;
p2.nowlevel = xuser.nowlevel;
p2.hp = xuser.hp;
p2.str = xuser.str;
p2.mgc = xuser.mgc;
p2.skl = xuser.skl;
p2.spd = xuser.spd;
p2.luk = xuser.luk;
p2.def = xuser.def;
p2.mdf = xuser.mdf;
p2.curwepnatt = xuser.curwepnatt;
p2.curwepnhit = xuser.curwepnhit;
p2.curwepnweg = xuser.curwepnweg;

for(i=0 ; i<20 ; i++)
p2.working[i] = xuser.working[i];
for(i=0 ; i<6 ; i++)
p2.spcskl[i] = xuser.spcskl[i];

for(i=0 ; i<2 ; i++)
for(j=0 ; j<10 ; j++)
p2.wepnlv[i][j] = xuser.wepnlv[i][j];

p2.curwepnlv[0][0] = xuser.curwepnlv[0][0];
p2.curwepnlv[1][0] = xuser.curwepnlv[1][0];

for(i=0 ; i<4 ; i++)
p2.curwepnspc[i] = xuser.curwepnspc[i];

p2.commander = xuser.commander;

if(p2.curwepnspc[0] > 0) {
if(!(p2.curwepnspc[1] & 128)) {

j = 64;
for(i=0 ; i<7 ; i++) {

if(p2.curwepnspc[1] & j)
if(p2.curwepnspc[2] & j)
appendstatus1[i] += 20;
else
appendstatus1[i] += 10;
j /= 2;
}
p2.str = appendstatus1[0];
p2.mgc = appendstatus1[1];
p2.skl = appendstatus1[2];
p2.spd = appendstatus1[3];
p2.luk = appendstatus1[4];
p2.def = appendstatus1[5];
p2.mdf = appendstatus1[6];
}
}
}
}
/* Dopin */


int rando(int i) {
   FILE *fp1;

   srandom(file_seed(BBSHOME"/src/maple/fpindex"));
   if(i > random()%100)
      return 1;
   else
      return 0;
}
#endif

void bitwr(usint *charvar, int bitnum, char flag) {
   usint i;

   if(bitnum < 0 || bitnum > 31)
      return;

   i = power(2, bitnum);

   if(flag == '+')
      *charvar |= i;
   else
      *charvar &= 0xFFFFFFFF - i;
}

void loop(int i) {
   int j;

   for(j=0 ; j<i ; j++) ;
}

#ifdef RPG_FIGHT
int showprocess(int i, int j, int k, int l) {
   int count,row,result=0,norp1,norp2;
   char getlife,jumpflag=0;

   norp1 = i;
   norp2 = k;

   move(0, 0);
   prints("  第 %3d/100 回合", fightround);

   for(count=1 ; count<16 ; count++) {
      move(count, 0);
      prints("                                                                   ");
   }

   row = 2;

   if(skl[0][23] == 1 && (rando(p1.skl / 2) || jumpflag == 1)) {
      move(row, 2);
      prints("%s 發動原力拉扯 !!!!!, %s 本回合內無法攻擊 !!!", uident1, uident2);
      jumpflag = 1;
      row++;
   }
   else
      if(skl[1][23] == 1 && (rando(p2.skl / 2) || jumpflag == 2)) {
      move(row, 2);
         prints("%s 發動原力拉扯 !!!!!, %s 本回合內無法攻擊 !!!", uident2, uident1);
      jumpflag = 2;
      row++;
   }

   if(jumpflag == 0) {
      if(p1.curwepnlv[0][0] == 81 || skl[1][41])
         if(rando(40 - p1.luk)) {
            move(row, 2);
            prints("%s 因詛咒之故, ", uident1);

            if(random() % 2 == 0) {
               prints("本回合攻擊完全失效 !!!");
               for(count = 0; count < 61; count++)
                  if(hitflag[count] % 2 == 1)
                     hitskl[count] = 0;
            }
            else {
               prints("陷入中毒狀態 LV1 !!");
               bitwr(&p1status, 0, '+');
           }

            row++;
         }

      if(p2.curwepnlv[0][0] == 81 || skl[0][41])
         if(rando(60 - p2.luk)) {
            move(row, 2);
            prints("%s 因詛咒之故, ", uident2);

            if(random() %2 == 0) {
               prints("本回合攻擊完全失效 !!!");
               for(count = 0; count < 61; count++)
                  if(hitflag[count] > 0 && hitflag[count] % 2 == 0)
                     hitskl[count] = 0;
            }
            else {
              prints("陷入中毒狀態 LV1 !!");
              bitwr(&p2status, 0, '+');
           }

           row++;
        }
   }


   for(count=0 ; count < 61 && result == 0 ; count++) {

      if(jumpflag == 2)
         goto JediJump1;

      if(skl[0][2] == 1 && p1.hp <= p1maxhp / 2)
         i = j;
      else
         i = norp1;

      if(skl[1][2] == 1 && p2.hp <= p2maxhp / 2)
         k = l;
      else
         k = norp2;

      if(hitflag[count] % 2 == 1) {
         move(row, 2);
         if(i >= 10 && (hitflag[count-10] == 1 | hitflag[count-10] == 3)) {
            move(row, 2);
            prints("%s 的快速連續攻擊 !!", uident1);
            row++;
         }
         else {
            move(row, 2);
            prints("%s 的攻擊 !", uident1);
            row++;
         }

         move(row, 2);
         if(hitflag[count] == 3 && count % 5 == 0) {
            move(row, 2);
            prints("%s 發動流星 !!!!!", uident1);
            row++;
         }

         move(row, 2);

         if(hitskl[count] == 0) {
            if(skl[0][23] == 1 && rando(p1.skl / 2)) {
              prints("%s 發動原力視覺 !!, 掌握 %s 行動 !!", uident1, uident2);
              row++;
              move(row, 2);
              goto StillHit1;
            }
            else
               prints("%s 攻擊失誤... %s 迴避 !!", uident1, uident2);

            row++;
         }
         else
StillHit1:
            if(skl[1][23] == 1 && rando(p2.skl) || skl[1][1] == 1 &&
               rando(p2.nowlevel / 2)) {
            if(skl[1][23] == 1 && skl[1][1] == 0)
               prints("%s 發動原力推移 !! %s 攻擊失效...", uident2, uident1);
            else
               if(skl[1][1] == 1)
               prints("%s 大盾技巧發動 !! %s 攻擊失效...", uident2 ,uident1);

               row++;
            }
            else
               if(hitskl[count] > 1) {
                  if(skl[1][23] == 1 && rando(p2.spd / 2)) {
                     move(row, 2);
                     prints("%s 發動原力加速 !! %s 攻擊失效...", uident2, uident1);
                     row++;
                     goto JediJump1;
                  }

                  prints("%s 的必殺攻擊 !!!", uident1);
                  row++;
                  move(row, 2);
                  prints("造成 %s %d 點的損傷...", uident2, j);
                  row++;
                  if(p2.hp - j > 0) {
                     p2.hp -= j;

                     if(skl[0][18] == 1 || skl[0][10] == 1 && rando(p1.skl))
                        if(p1.hp < p1maxhp) {
                           if(skl[0][18] == 0) {
                              move(row, 2);
                              prints("%s 發動太陽 !!!", uident1);
                              row++;
                           }

                           if(p1maxhp - p1.hp > j) {
                              p1.hp += j;
                              getlife = j;
                           }
                           else {
                              getlife = p1maxhp - p1.hp;
                              p1.hp += p1maxhp - p1.hp;
                           }
                           move(row, 2);
                           prints("%s 奪取 %s %d 點體力", uident1, uident2, getlife);
                           row++;
                        }
                  }
                  else {
                     move(row, 2);
                     prints("%s 戰鬥不能... %s 獲勝 !!", uident2, uident1);
                     p2.hp = 0;
                     result =1;
                  }
              }
              else {
                 if(skl[1][23] == 1 && rando(p2.spd / 2)) {
                    move(row, 2);
                    prints("%s 發動原力加速 !! %s 攻擊失效...", uident2, uident1);
                    row++;
                    goto JediJump1;
                 }
                 if(skl[0][2] == 1 && p1.hp <= p1maxhp / 2 && (skl[1][6] == 0
                    || skl[1][23] == 0)) {
                    move(row, 2);
                    prints("%s 的憤怒攻擊 !!!", uident1);
                    row++;
                 }
                 prints("造成 %s %d 點的損傷...", uident2, i);
                 row++;

                 if(p2.hp - i > 0) {
                     p2.hp -= i;

                     if(skl[0][18] == 1 || skl[0][10] == 1 && rando(p1.skl))
                        if(p1.hp < p1maxhp) {
                           if(skl[0][18] == 0) {
                              move(row, 2);
                              prints("%s 發動太陽 !!!", uident1);
                              row++;
                           }

                           if(p1maxhp - p1.hp > i) {
                              p1.hp += i;
                              getlife = i;
                           }
                           else {
                              getlife = p1maxhp - p1.hp;
                              p1.hp += p1maxhp - p1.hp;
                           }

                           move(row, 2);
                           prints("%s 奪取 %s %d 點體力", uident1, uident2, getlife);
                           row++;
                        }
                 }
                 else {
                    move(row, 2);
                    prints("%s 戰鬥不能... %s 獲勝 !!", uident2, uident1);
                    p2.hp = 0;
                    result =1;
                 }
          }
      }
      if(jumpflag == 1)
         goto JediJump2;
JediJump1:

      if(hitflag[count] % 2 == 0 && hitflag[count] >= 2) {
         if(count >= 10 && (hitflag[count-10] == 2 | hitflag[count-10] == 4)) {
            move(row, 2);
            prints("%s 的快速連續攻擊 !!", uident2);
            row++;
         }
         else {
            move(row, 2);
            prints("%s 的攻擊 !!", uident2);
            row++;
         }

         if(hitflag[count] == 4 && count % 5 == 0) {
            move(row, 2);
            prints("%s 發動流星 !!!!!", uident2);
            row++;
         }

         move(row, 2);

         if(hitskl[count] == 0) {
            if(skl[1][23] == 1 && rando(p2.skl / 2)) {
               prints("%s 發動原力視覺 !!, 掌握 %s 行動 !!", uident2, uident1);
               row++;
               move(row, 2);
               goto StillHit2;
            }
            else
               prints("%s 攻擊失誤... %s 迴避 !!", uident2, uident1);

            row++;
         }
         else
StillHit2:
            if(skl[0][1] == 1 && rando(p1.nowlevel / 2) || skl[0][23] == 1 &&
               rando(p1.skl)) {
               if(skl[0][23] == 1 && skl[0][1] == 0)
                  prints("%s 發動原力推移 !! %s 攻擊失效...", uident1, uident2);
               else
                  if(skl[0][1] == 1)
                  prints("%s 大盾技巧發動 !! %s 攻擊失效...", uident1 ,uident2);
               row++;
            }
            else
               if(hitskl[count] > 1) {
                  if(skl[0][23] == 1 && rando(p1.spd / 2)) {
                     move(row, 2);
                     prints("%s 發動原力加速 !! %s 攻擊失效...", uident1, uident2);
                     row++;
                     goto JediJump2;
                  }

                  prints("%s 的必殺攻擊 !!!", uident2);
                  row++;
                  move(row, 2);
                  prints("造成 %s %d 點的損傷...", uident1, l);
                  row++;

                  if(p1.hp - l > 0) {
                     p1.hp -= l;

                     if(skl[1][18] == 1 || skl[1][10] == 1 && rando(p2.skl))
                        if(p2.hp < p2maxhp) {
                           if(skl[1][18] == 0) {
                              move(row, 2);
                              prints("%s 發動太陽 !!!", uident2);
                              row++;
                           }

                           if(p2maxhp - p2.hp > l) {
                             p2.hp += l;
                             getlife = l;
                           }
                           else {
                              getlife = p2maxhp - p2.hp;
                              p2.hp += p2maxhp - p2.hp;
                           }

                           move(row, 2);
                           prints("%s 奪取 %s %d 點體力", uident2, uident1, getlife);
                           row++;
                        }
                  }
                  else {
                     move(row, 2);
                     prints("%s 戰鬥不能... %s 獲勝 !!", uident1, uident2);
                     p1.hp = 0;
                     result =2;
                  }
               }
               else {
                  if(skl[0][23] == 1 && rando(p1.spd / 2)) {
                     move(row, 2);
                     prints("%s 發動原力加速 !! %s 攻擊失效...", uident1, uident2);
                     row++;
                     goto JediJump2;
                  }
                  if(skl[1][2] == 1 && p2.hp <= p2maxhp / 2 && (skl[0][23] == 0
                     || skl[0][6] == 0)) {
                     move(row, 2);
                     prints("%s 的憤怒攻擊 !!!", uident2);
                     row++;
                  }
                  prints("造成 %s %d 點的損傷...", uident1, k);
                  row++;

                  if(p1.hp - k > 0) {
                     p1.hp -= k;

                     if(skl[1][18] == 1 || skl[1][10] == 1 && rando(p2.skl))
                        if(p2.hp < p2maxhp) {
                           if(skl[1][18] == 0) {
                              move(row, 2);
                              prints("%s 發動太陽 !!!", uident2);
                              row++;
                           }

                           if(p2maxhp - p2.hp > k) {
                              p2.hp += k;
                              getlife = k;
                           }
                           else {
                              getlife = p2maxhp - p2.hp;
                              p2.hp += p2maxhp - p2.hp;
                           }

                           move(row, 2);
                           prints("%s 奪取 %s %d 點體力", uident2, uident1, getlife);
                           row++;
                        }
                 }
                 else {
                    move(row, 2);
                    prints("%s 戰鬥不能... %s 獲勝 !!", uident1, uident2);
                    p1.hp = 0;
                    result =2;
                 }
             }
      }

JediJump2:

      if(row > 12) {
         row = 2;
         pressanykey(NULL);

         for(count=0 ; count<16 ; count++) {
            move(count, 0);
            clrtoeol();
         }
      }
/*
      move(18, 5);
      prints("%3d", p1.hp);
      move(18, 44);
      prints("%3d", p2.hp);
*/
   }

   if(fightround >= 100) {
       move(row+1, 0);
       prints("雙方無法於 100 回合內分出勝負....記為平手...");
       result=3;
       goto EndFighting;
   }

if(result == 0) {
   if(p1status > 0) {
      if(p1status & 1) {
         move(14, 2);

         if(p1.hp - (int)(p1maxhp * 0.15) > 0) {
            prints("%s 因中毒 L1 之故, 損傷 %d 之體力", uident1, (int)(p1maxhp * 0.15));
            p1.hp -= (int)(p1maxhp * 0.15);
         }
         else {
            prints("%s 因中毒 L1 之故, 損血降至 0, %s 戰鬥不能!!",
                    uident1, uident1);
            p1.hp = 0;
            result = 2;
         }
      }
   }

   if(p2status > 0) {
      if(p2status & 1) {
         move(14, 2);

         if(p2.hp - (int)(p2maxhp * 0.15) > 0) {
            prints("%s 因中毒 L1 之故, 損傷 %d 之體力", uident2, (int)(p2maxhp * 0.15));
            p2.hp -= (int)(p2maxhp * 0.15);
         }
         else {
            prints("%s 因中毒 L1 之故, 損血降至 0, %s 戰鬥不能!!",
                   uident2, uident2);
            p2.hp = 0;
            result = 1;
         }
      }
   }

   if(p1.hp == 0 || p1.hp == 0) {
      move(15, 2);

      if(p1.hp == 0)
         if(p2.hp > 0)
            prints("%s 獲勝!!", uident2);
         else {
            prints("兩敗俱傷....");
            result = 4;
         }
      else
         prints("%s 獲勝!!", uident1);
   }

   if(skl[0][22] == 1 && result == 0)
      if(p1.hp < p1maxhp) {
         move(15, 2);

         if(p1.hp <= p1maxhp - (int)(p1maxhp * 0.15)) {
            p1.hp += (int)(p1maxhp * 0.15);

            prints("%s 回復 %d 點體力", uident1, p1maxhp * 0.15);
         }
         else {
            p1.hp = p1maxhp;
            prints("%s 的體力完全回復 !!", uident1);
         }
      }

   if(skl[1][22] == 1 && result == 0)
      if(p2.hp < p2maxhp) {
         move(15, 2);

         if(p2.hp <= p2maxhp - (int)(p2maxhp * 0.15)) {
            p2.hp += (int)(p2maxhp * 0.15);
            prints("%s 回復 %d 點的體力", uident2, p2maxhp * 0.15);
         }
         else {
            p2.hp = p2maxhp;
            prints("%s 的體力完全回復 !!", uident2);
         }
      }
}

/*
   if(result)
      for(row++ ; row<16 ; row++) {
         move(row, 0);
         prints("                                                   ");
      }
*/
EndFighting:
   move(18, 5);
   prints("%3d", p1.hp);
   move(18, 44);
   prints("%3d", p2.hp);

   pressanykey(NULL);
   return (int)result;
}


int beginfig(void) {

uschar p1relatt,p1reldef,p2relatt,p2reldef;
char status,p1nordmg,p1spcdmg,p2nordmg,p2spcdmg;
char fitspd,temp;
int i,j,k,l, win[2], lost[2], chp[2];
FILE *pk1, *pk2;
char fbuf[80];

   clear();
   srandom(file_seed(BBSHOME"/src/maple/fpindex"));

   for(i=0 ; i<80 ; i++)
      fbuf[i] = abs(random()%256);

   for(i=0 ; i<50 ; i++) {
      skl[0][i] = 0;
      skl[1][i] = 0;
   }

p1status = p2status = 0;

for(i=0 ; i<24 ; i++) {
p1wepname[i] = ' ';
p2wepname[i] = ' ';
}

if(p1.curwepnlv[0][0] == 0 || p1.curwepnlv[0][0] == 104)
for(i=0 ; i<3 ; i++)
p1wepname[i] = '-';
else
if(p1.curwepnlv[0][0] < 41)
for(i=0 ; i<24 ; i++)
p1wepname[i] = itemtable1[p1.curwepnlv[0][0]-1][i];
else
if(p1.curwepnlv[0][0]-1 < 81)
for(i=0 ; i<24 ; i++)
p1wepname[i] = itemtable2[p1.curwepnlv[0][0]-41][i];
else
for(i=0 ; i<24 ; i++)
p1wepname[i] = itemtable3[p1.curwepnlv[0][0]-81][i];

if(p2.curwepnlv[0][0] == 0 || p2.curwepnlv[0][0] == 104)
for(i=0 ; i<3 ; i++)
p2wepname[i] = '-';
else
if(p2.curwepnlv[0][0] < 41)
for(i=0 ; i<24 ; i++)
p2wepname[i] = itemtable1[p2.curwepnlv[0][0]-1][i];
else
if(p2.curwepnlv[0][0]-1 < 81)
for(i=0 ; i<24 ; i++)
p2wepname[i] = itemtable2[p2.curwepnlv[0][0]-41][i];
else
for(i=0 ; i<24 ; i++)
p2wepname[i] = itemtable3[p2.curwepnlv[0][0]-81][i];


for(i = 0; i< 24 ; i++)
if(p1wepname[i] == 0) {
j = i;
break;
}
for( ; j<24 ; j++)
p1wepname[j] = ' ';

for(i=0 ; i<24 ; i++)
if(p2wepname[i] == 0) {
j = i;
break;
}

for( ; j<24 ; j++)
p2wepname[j] = ' ';

p1wepname[24] = 0;
p2wepname[24] = 0;

if(p1.curwepnlv[0][0] == 0 || p1.curwepnlv[0][0] == 104) {
p1relatt = 0;
p2reldef = p2.def;
}
else if(p1.curwepnspc[3] & 128) {
if(appendstatus[1]+p1.curwepnatt >= 255)
p1relatt = 255;
else
p1relatt = appendstatus[1]+p1.curwepnatt;

p2reldef = appendstatus1[6];
}
else {
if(appendstatus[0]+p1.curwepnatt >= 255)
p1relatt = 255;
else
p1relatt = appendstatus[0]+p1.curwepnatt;

p2reldef = appendstatus1[5];
}

if((p1.skl*2+p1.curwepnhit) - ((p2.spd-p2.curwepnweg)*2+p2.luk) >= 100)
p1hit = 100;
else
if((p1.skl*2+p1.curwepnhit) - ((p2.spd-p2.curwepnweg)*2+p2.luk) <= 0)
p1hit = 0;
else
p1hit = (p1.skl*2+p1.curwepnhit) - ((p2.spd-p2.curwepnweg)*2+p2.luk);

if((p2.skl*2+p2.curwepnhit) - ((p1.spd-p1.curwepnweg)*2+p1.luk) >= 100)
p2hit = 100;
else
if((p2.skl*2+p2.curwepnhit) - ((p1.spd-p1.curwepnweg)*2+p1.luk) <= 0)
p2hit = 0;
else
p2hit = (p2.skl*2+p2.curwepnhit) - ((p1.spd-p1.curwepnweg)*2+p1.luk);

if(p2.curwepnlv[0][0] == 0 || p2.curwepnlv[0][0] == 104) {
p2relatt = 0;
p1reldef = p1.def;
}
else if(p2.curwepnspc[3] & 128) {
if(appendstatus1[1]+p2.curwepnatt >= 255)
p2relatt = 255;
else
p2relatt = appendstatus1[1]+p2.curwepnatt;

p1reldef = appendstatus[6];
}
else {
if(appendstatus1[0]+p2.curwepnatt >= 255)
p2relatt = 255;
else
p2relatt = appendstatus1[0]+p2.curwepnatt;

p1reldef = appendstatus[5];
}

for(j=0 ; j<6 ; j++) {
i = p1.spcskl[j];
skl[0][i] = 1;
i = p2.spcskl[j];
skl[1][i] = 1;
}

if(skl[0][29] == 1)
   p2relatt /= 2;

if(skl[1][29] == 1)
   p1relatt /= 2;

if(p1.commander > 1)
   p1.commander -= 1;
if(p2.commander > 1)
   p2.commander -= 1;

if(p1.commander > 0 || p2.commander > 0)
   if(p1.commander - p2.commander != 0)
      if(p1.commander - p2.commander > 0) {
         if(p1hit + (p1.commander - p2.commander) * 10 > 100)
            p1hit = 100;
         else
            p1hit += (p1.commander - p2.commander) * 10;
         if(p2hit - (p1.commander - p2.commander) * 10 < 0)
            p2hit = 0;
         else
            p2hit += (p2.commander - p1.commander) * 10;
      }
      else {
         if(p2hit + (p2.commander - p1.commander) * 10 > 100)
            p2hit = 100;
         else
            p2hit += (p2.commander - p1.commander) * 10;
         if(p1hit - (p2.commander - p1.commander) * 10 < 0)
            p1hit = 0;
         else
            p1hit -= (p2.commander - p1.commander) * 10;
      }

if(skl[0][5] == 1 && p1hit < 100)
   if(p1hit <= 90)
      p1hit += 10;
   else
      p1hit = 100;

if(skl[1][5] == 1 && p2hit < 100)
   if(p2hit <=90)
      p2hit += 10;
   else
      p2hit = 100;

if(skl[0][5] == 1 && p2hit > 0)
   if(p2hit >= 10 && p2hit < 100)
      p2hit -= 10;
   else
      if(p2hit < 10)
         p2hit = 0;
      else {
         p2hit = (p2.skl*2+p2.curwepnhit) - ((p1.spd-p1.curwepnweg)*2+p1.luk);
         p2hit -= 10;
         if(p2hit > 100)
            p2hit = 100;
      }

if(skl[1][5] == 1 && p2hit > 0)
   if(p1hit >= 10 && p1hit < 100)
      p1hit -= 10;
   else
      if(p1hit < 10)
         p1hit = 0;
      else {
         p1hit = (p1.skl*2+p1.curwepnhit) - ((p2.spd-p2.curwepnweg)*2+p2.luk);
         p1hit -= 10;
         if(p1hit > 100)
            p1hit = 100;
      }

k = 3;

   sprintf(fbuf, BBSHOME"/home/%s/pkstatus", uident1);
   /*
   if(!(pk1 = fopen(fbuf, "rw+"))) {
   */
      win[0] = lost[0] = 0;
      p1.hp = p1maxhp;
   /*
   }
   else {
      fscanf(pk1, "%d %d %d", &win[0], &lost[0], &chp[0]);
      p1.hp = chp[0];
   }
   fclose(pk1);
   */

   sprintf(fbuf, BBSHOME"/home/%s/pkstatus", uident2);
   /*
   if(!(pk2 = fopen(fbuf, "rw+"))) {
   */
      win[1] = lost[1] = 0;
      p2.hp = p2maxhp;
   /*
   }
   else {
      fscanf(pk2, "%d %d %d", &win[1], &lost[1], &chp[1]);
      p2.hp = chp[1];
   }
   fclose(pk2);
   */

for(i=0 ; i<13 ; i++) {
if(uident1[i] == 0)
uident1[i] = ' ';
if(uident2[i] == 0)
uident2[i] = ' ';
}
uident1[STRLEN-1] = 0;
uident2[STRLEN-1] = 0;

move (16, 0);

prints("\
===============================================================================\n");
prints("\
= %s                        | %s                        =\n\
= HP %3d                               | HP %3d                               =\n\
= %s             | %s             =\n",
uident1, uident2, p1.hp, p2.hp, p1wepname, p2wepname);
prints("\
= HIT %3d           DEF %3d            | HIT %3d           DEF %3d            =\n",
p1hit, p1reldef, p2hit, p2reldef);
prints("\
= ATT %3d           LEV %3d            | ATT %3d           LEV %3d            =\n",
p1relatt, p1.nowlevel, p2relatt, p2.nowlevel);
prints("\
===============================================================================\n");

if(p1relatt > p2reldef)
   if(p1relatt - p2reldef >= 99)
      p1nordmg = 99;
   else
      p1nordmg = p1relatt - p2reldef;
else
   if(p1relatt > 0)
      p1nordmg = 1;
   else
      p1nordmg = 0;

if(p1relatt * 2 > p2reldef)
   if(p1relatt * 2 - p2relatt >= 99)
      p1spcdmg = 99;
   else
      p1spcdmg = p1relatt * 2 - p2reldef;
else
   if(p1relatt > 0)
      p1spcdmg = 1;
   else
      p1spcdmg = 0;

if(p2relatt > p1reldef)
   if(p2relatt - p1reldef >= 99)
      p2nordmg = 99;
   else
      p2nordmg = p2relatt - p1reldef;
else
   if(p2relatt > 0)
      p2nordmg = 1;
   else
      p2nordmg = 0;

if(p2relatt * 2 > p1reldef)
   if(p2relatt * 2 - p1reldef >= 99)
      p2spcdmg = 99;
   else
      p2spcdmg = p2relatt * 2 - p1reldef;
else
   if(p2relatt > 0)
      p2spcdmg = 1;
   else
      p2spcdmg = 0;

if(p1nordmg == 0 && p2nordmg == 0) {
   move(8, 6);
   prints("兩位職業皆為無業遊民或未裝備攻擊性武器,無法進行戰鬥...程式中斷 !!");
   pressanykey(NULL);
   goto TerminalEnd;
}

   fitspd = (p1.spd-p1.curwepnweg) - (p2.spd-p2.curwepnweg);
   p1maxhp = p1.hp;
   p2maxhp = p2.hp;

   pressanykey(NULL);

BeginOneRun:

fightround++;

for(i=0 ; i<61 ; i++) {
   hitflag[i] = 0;
   hitskl[i] = 0;
}
/*
for(j=0 ; j<6 ; j++) {
i = p1.spcskl[j];
skl[0][i] = 1;
i = p2.spcskl[j];
skl[1][i] = 1;
}
*/
if(fitspd > 0) {
   hitflag[0] = 1;
   if(skl[0][4] == 1) {
      if(rando(p1.spd))
         hitflag[10] = 1;
   }
   if(skl[0][3] == 1) {
      hitflag[40] = 1;
      if(skl[0][4] == 1)
         if(rando(p1.spd))
            hitflag[50] = 1;
   }
   hitflag[20] = 2;
   if(skl[1][4] == 1)
      if(rando(p2.spd))
         hitflag[30] = 2;
}
else {
if(fitspd < 0) {
    hitflag[0] = 2;
   if(skl[1][4] == 1) {
      if(rando(p2.spd))
          hitflag[10] = 2;
   }
   if(skl[1][3] == 1) {
      hitflag[40] = 2;
      if(skl[1][4] == 1)
         if(rando(p2.spd))
            hitflag[50] = 2;

   }
   hitflag[20] = 1;
   if(skl[0][4] == 1)
      if(rando(p1.spd))
         hitflag[30] = 1;
}
else {
   hitflag[0] = 1;
   hitflag[20] = 2;
   if(skl[0][4] == 1)
      if(rando(p2.spd))
         hitflag[10] = 1;
   if(skl[1][4] == 1)
      if(rando(p1.spd))
         hitflag[30] = 2;
}
}
/*
   move(2, 0);
   prints("判斷攻擊進程 '1' 為 P1, '2' 為 P2(追擊,連續)\n");
   for(i=0; i<61 ; i++)
      prints("%d", hitflag[i]);
*/
   for(i=0 ; i<61 ; i+=10) {
      if((p1.curwepnlv[0][0] == 111 | p1.curwepnlv[0][0] == 112 |
         p1.curwepnlv[0][0] == 113) && hitflag[i] == 1)
         hitflag[i+5] = 1;

      if((p2.curwepnlv[0][0] == 111 | p2.curwepnlv[0][0] == 112 |
         p2.curwepnlv[0][0] == 113) && hitflag[i] == 2)
         hitflag[i+5] = 2;
   }
/*
   move(4, 0);
   prints("判斷有無勇者系武器\n");
   for(i=0; i<61 ; i++)
      prints("%d", hitflag[i]);
*/
   for(i=0 ; i<61 ; i+=5) {
      if(skl[0][9] == 1  && hitflag[i] == 1) {
         if(rando(p1.skl))
            for(j=0 ; j<5 ; j++)
               hitflag[i+j] = 3;
      }

      if(skl[1][9] == 1 && hitflag[i] == 2) {
         if(rando(p2.skl))
            for(j=0 ; j<5 ; j++)
               hitflag[i+j] = 4;
      }
   }
/*
   move(6, 0);
   prints("判斷有無特殊連續攻擊(ex:流星)\n");
   for(i=0; i<61 ; i++)
      prints("%d", hitflag[i]);
*/
   for(i=0 ; i<61 ; i++) {
      if(hitflag[i] > 0 && hitflag[i] % 2 == 0)
         if(rando(p2hit))
            hitskl[i] = 1;

      if(hitflag[i] % 2 == 1)
         if(rando(p1hit))
            hitskl[i] = 1;
   }
/*
   move(8, 0);
   prints("判斷命中與否...\n");
   for(i=0; i<61 ; i++)
      prints("%d", hitskl[i]);
*/
   for(i=0 ; i < 61 ; i++) {

       if(skl[1][6] == 0 && skl[1][23] == 0 && skl[0][7] == 1 &&
          hitflag[i] % 2 == 1 && hitskl[i] == 1)
          if(rando(p1.skl))
             hitskl[i] = 2;

       if(skl[0][6] == 0 && skl[0][23] == 0 && skl[1][7] == 1 &&
          hitflag[i] > 0 && hitflag[i] % 2 == 0 &&  hitskl[i] == 1)
          if(rando(p2.skl))
             hitskl[i] = 2;
   }
/*
   move(10, 0);
   prints("判斷是否發動必殺攻擊...\n");
   for(i=0; i<61 ; i++)
      prints("%d", hitskl[i]);

   pressanykey(NULL);
*/

   i =   showprocess(p1nordmg, p1spcdmg, p2nordmg, p2spcdmg);

   if(i == 0)
      goto BeginOneRun;
   else
      if(i == 1) {
         chp[0] = p1.hp;
         chp[1] = 1;
         win[0]++;
         lost[1]++;
      }
      else
         if(i == 2) {
            chp[0] = 1;
            chp[1] = p2.hp;
            win[1]++;
            lost[0]++;
         }
         else
            if(i == 3) {
               chp[0] = p1.hp;
               chp[1] = p2.hp;
            }
            else {
               chp[0] = chp[1] = 1;
               lost[0]++;
               lost[1]++;
            }

   for(i=0 ; i<13 ; i++) {
      if(uident1[i] == ' ')
         uident1[i] = 0;
      if(uident2[i] == ' ')
         uident2[i] = 0;
   }

   /*
   pk2 = fopen(fbuf, "w+");
   sprintf(fbuf, BBSHOME"/home/%s/pkstatus", uident1);
   pk1 = fopen(fbuf, "w+");

   fprintf(pk1, "%d %d %d", win[0], lost[0], p1maxhp);
   fprintf(pk2, "%d %d %d", win[1], lost[1], p2maxhp);
   */
TerminalEnd:
   /*
   fclose(pk1);
   fclose(pk2);
   */

/*
bitwr(&hitflag[0], 7, '+');
*/
   askpk = 0;
}


int t_fight(void) {

   fightround = 0;

   if(init())
      beginfig();
   else {
      pressanykey("輸入錯誤!!程式中斷....");
   }

   return FULLUPDATE;
}

void popsort(usint *a, uschar *b, uschar n) {
   uschar i, j, bt;
   usint temp;

   for(i=0 ; i<n ; i++) {
      j = i + 1;

      while(j > 0 ) {
         if(a[j] < a[j-1]) {
            bt = b[j];
            temp = a[j];
            b[j] = b[j-1];
            a[j] = a[j-1];
            b[j-1] = bt;
            a[j-1] = temp;
         }

         j--;
      }
   }
}
#endif

void item_update_passwd(char ch) {
   if(ch == 'l')
      get_record(fn_passwd, &xuser, sizeof(xuser), usernum);

   if(ch == 'u') {
      UPDATE_USEREC;
      substitute_record(fn_passwd, &xuser, sizeof(xuser), usernum);
   }
}

int check_money(char mode, int money) {
   item_update_passwd('l');

   if(money < 0) return -1;

   if(mode == '-' || mode == '+') {
      if(mode == '-') {
         if(xuser.havemoney < money) return -1;
         xuser.havemoney -= money;
      }
      else xuser.havemoney += money;

      item_update_passwd('u');
      item_update_passwd('l');
   }

   return xuser.havemoney;
}

int b_favor_set(char flag, char mode, int boardnum) {
   char ci, cj;
   FILE *fp;

   clear();
   if(brdnum <= 1) {
      move(10, 15);
      outs("[1;37;41m未載入看板資料 請先進入分組討論區 檢視一次所有看板[m");
      pressanykey(NULL);
      return FULLUPDATE;
   }
   friend_edit(BOARD_FAVOR);

   return FULLUPDATE;
}

#ifdef DRAW
int do_draw(int *co, int nd, char buf) {
   FILE *fp;
   struct draw_s {
      char id[IDLEN+1];
      int no;
   } draw;
   int i, j, dr_n[4] = { 0 }, ch = 0;
   char genbuf[2];
   char dr_id[4][200][IDLEN+1] = { 0 };
   char *draw_l[4] = {
"板主部份\n"
"特獎   一名 打造個人武器 或 任選 A 級以下武器一把\n"
"            EXP +10000  $ +30000 招財貓一隻\n"
"            頭獎   一名 EXP +10000  $ +20000 招財貓一隻\n"
"            貳獎   三名 EXP + 8000  $ +20000\n"
"            三獎   五名 EXP + 5000  $ +10000\n"
"            肆獎   十名             $ + 5000\n"
"            參加獎                  $ + 2000\n"
,
"註冊滿 100 天\n"
"頭獎   一名 EXP +20000  $ +10000  亞特蘭提斯紀念金幣一枚\n"
"二獎   五名 EXP + 5000  $ + 8000  亞特蘭提斯紀念銀幣一枚\n"
"三獎   十名 EXP + 5000  $ + 5000\n"
"肆獎 二十名 EXP + 3000  $ + 3000\n"
,
"優良站友\n"
"特獎   一名 創造個人物品(條件式) 或任選 80000 以下物品一個\n"
"            EXP +20000  $ +50000 招財貓一隻\n"
"            頭獎   一名 EXP +10000  $ +50000 招財貓一隻\n"
"            貳獎   三名 EXP + 5000  $ +20000 招財貓一隻\n"
"            三獎   五名 EXP + 5000\n"
"            所有參加者皆可得到紀念金幣一枚\n"
,
"通過認證\n"
"頭獎   一名 EXP + 5000  $ + 5000  亞特蘭提斯紀念金幣一枚\n"
"二獎   五名 EXP + 5000  $ + 3000  亞特蘭提斯紀念銀幣一枚\n"
"三獎   十名 EXP + 3000  $ + 3000  火鳳凰之羽一枚\n"
"肆獎 二十名 EXP + 2000  $ + 1000\n"
};

   clear();
   move(1, 0);

   if(buf == 'l' || buf == 'L') {
      fp = fopen(BBSHOME"/draw/list_all", "rb");
      if(!fp) return -1;
      i = j = 0;
      prints("%-7s[1;31;40m%-s[m", "帳號" , "參加項目");
      move(2, 0);
      while (fread(&draw, sizeof(draw), 1, fp)) {
         prints("%-13s[1;31;40m%-2d[m", draw.id, draw.no+1);

         if(j == 4) {
            i++;
            j = 0;

            if(i == 22) {
               move(b_lines-1, 5);
               outs("請按任意鍵繼續 或 Q 離開");
               ch = igetkey();
               if(ch == 'Q' || ch == 'q')
                  goto DONE_D;

               ch = 0;
               i = 0;
               clear();
               move(2, 0);
            }
            outc('\n');
         }
         else
            j++;
      }
      fclose(fp);

      fp = fopen(BBSHOME"/adm/drp_list", "w+");
      if(!fp) goto PUPU_L;
      for(i = 0 ; i < 4 ; i++) {
         for(j = 0 ; j <= dr_n[i] ; j++)
            fprintf(fp, "%s\n", dr_id[i][j]);
      fprintf(fp, "\n");
   }
   fclose(fp);

PUPU_L:
   }
   else {
      i = (int)(buf - '1');
      if(!co[i]) {
         if(i != 1) {
            outs("     抱歉...您無法參加該項目...");
         }
         goto DONE_D;
      }
DO_D:
      move(1, 0);
      outs("本項目之獎項如下 :\n\n");
      outs(draw_l[i]);
      fp = fopen(BBSHOME"/draw/list_all", "rb");
      if(!fp) return -1;
      ch = 0;
      while (fread(&draw, sizeof(draw), 1, fp)) {
         if(!strcmp(draw.id, cuser.userid)) {
            ch = 1;
            break;
         }
      }
      fclose(fp);

      if(ch) {
         outs("\n\n   您已經報名過本次活動了...");
         goto DONE_D;
      }

      getdata(15, 25, "確定參加 y/N ? ", genbuf, 2, LCECHO, 0);
      if(genbuf[0] == 'y' || genbuf[0] == 'Y') {
         fp = fopen(BBSHOME"/draw/list_all", "ab+");
         if(!fp) return -1;
         strcpy(draw.id, cuser.userid);
         draw.no = i;
         fwrite(&draw, sizeof(draw), 1, fp);
         fclose(fp);
         move(17, 25);
         outs("報名處理完畢...");
      }
   }
DONE_D:
   return DONOTHING;
}

int show_me() {
   int cont[4] = {0};
   char buf[2];

   cont[0] = belong("etc/NO_1", cuser.userid);
   cont[1] = ((cuser.firstlogin - 989971200) <= 0);
   cont[2] = belong("etc/NO_3", cuser.userid);
   cont[3] = (cuser.userlevel & PERM_LOGINOK);

   clear();
   move(1, 5);
   if(!strcmp(cuser.userid, "guest")) {
      outs("Guest 帳號無法參加抽獎...");
      goto OK_D;
   }

   prints("您於 08/24/01 之前%s板主權限 (具有者方可參加第一項抽獎)",
          cont[0] ? "具有" : "不具有");
   move(3, 5);
   prints("您的第一次登錄本站時間為 %s", Cdate(&cuser.firstlogin));
   move(4, 5);
   prints("您%s參加本次抽獎之第二項", cont[1] ?
           "可以" : "不能");

   move(6, 5);
   prints("您於 08/24/01 之前被提報之文章%s 50 篇 (達到者方可參加第三項抽獎)",
           cont[2] ? "達到" : "未達");

   move(8, 5);
   prints("您%s (通過註冊認證權限者方可參加第四項抽獎)",
     cont[3] ? "已通過註冊認證" : "未通過註冊認證");

   move(10, 5);
   outs("關於抽獎之各事項請參考 Announce 板最新之公告,有任何問題請寫信給 Dopin");

   if(!strcmp(cuser.userid, "guest")) {
      move(12, 5);
      outs("Guest 帳號無法參加抽獎...");
      pressanykey(NULL);
      return FULLUPDATE;
   }

   if(cont[0] || cont[1] || cont[2] || cont[3]) {
      getdata(12, 5, "請選擇要報名的項目或檢視報名名單 [1/2/3/4/(L)ist/(Q)uit] : ",
                     buf, 2, DOECHO, 0);
      if(buf[0] == '1' || buf[0] == '2' || buf[0] == '3' || buf[0] == '4' ||
         buf[0] == 'L' || buf[0] == 'l')
         do_draw(cont, 4, buf[0]);

      cul_data();
   }

OK_D:
   pressanykey(NULL);
   return FULLUPDATE;
}
#endif

#ifdef CHECK_LIST_ID_SELF
void check_listname(void) {   /* Dopin: 檢查名單中是否含有自己的 ID */
   char buf[100];
   int i;

   for(i = 0 ; i < 4 ; i++)
      if(i != 2) {
         setuserfile(buf, friend_file[i]); /* 取 static 變數 */
         while(belong(buf, cuser.userid)) {
            pressanykey("%s 名單中含你自己的 ID 關鍵字 請將移除 ...",
                        friend_list[i]);
            friend_edit(i);
         }
      }
}
#endif
