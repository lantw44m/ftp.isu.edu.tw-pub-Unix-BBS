#include "bbs.h"
#include "rpg.h" 

/* 大頭照 */
void
showpicture(char *uid)
{
  char genbuf[200];

  sethomefile(genbuf, uid, "picture");
  if (!show_file(genbuf, 11, 12, ONLY_COLOR))
    prints("[1;32m【大頭照】[m%s 目前沒有大頭照", uid);
}

int
u_editpic()
{
  char genbuf[200];
  stand_title("編輯大頭照");
  showpicture(cuser.userid);
  getdata(b_lines - 1, 0, "大頭照 (D)刪除 (E)編輯 [Q]取消？[Q] ", genbuf, 3, LCECHO,0);

  if (genbuf[0] == 'e')
  {
    int aborted;

    setutmpmode(EDITPIC);
    setuserfile(genbuf, "picture");
    aborted = vedit(genbuf, NA);
    if (aborted != -1)
    pressanykey("大頭照更新完畢");
    return 0;
  }
  else if (genbuf[0] == 'd')
  {
    setuserfile(genbuf, "picture");
    unlink(genbuf);
    outmsg("大頭照刪除完畢");
  }
  return 0;
}

/* 計算各項攻防值 */

int
c_att(char *id)
{
  int i;
  getuser(id);
  i = (((rpgtmp.str*10)+(rpgtmp.dex*7)+(rpgtmp.kar*5))/3);
  return i;
}

int
c_def(char *id)
{
  int i;
  getuser(id);
  i = (((rpgtmp.con*10)+(rpgtmp.dex*7)+(rpgtmp.kar*5))/3);
  return i;
}

int
c_ma(char *id)
{
  int i;
  getuser(id);
  i = (((rpgtmp.wis*10)+(rpgtmp.kar*5))/2);
  return i;
}

int
c_md(char *id)
{
  int i;
  getuser(id);
  i = (((rpgtmp.con*10)+(rpgtmp.dex*7)+(rpgtmp.wis*7)+(rpgtmp.kar*5))/4);
  return i;
}

int
rpg_udisplay(char *id)
{
  int MHP,MMP,Att,MA,Def,MD;
  if(!getuser(id)) return 0;
  MHP = rpgtmp.con*30;
  MMP = rpgtmp.wis*10;
  Att = c_att(id);
  Def = c_def(id);
  MA = c_ma(id);
  MD = c_md(id);

  setutmpmode(RINFO);
  clear();
  outs(COLOR1"[1m ○?????????????????????????? 使用者　ＲＰＧ　資料 ?????????????????????????龤? [m\n");
  prints("[1;32m【姓名】[m%-16.16s[1;32m【職業】[m%-15.15s[1;32m【等級】[m%-5d[1;32m【經驗值】[m%d\n"
       ,rpgtmp.userid,rname[rpgtmp.race],rpgtmp.level,xuser.exp);
  prints("[1;32m【年齡】[m%6d 歲  [1;32m【體力】[m%6d/%-6d [1;32m【法力】[m%6d/%-6d\n",
       (rpgtmp.age/2880)+10,rpgtmp.hp,MHP,rpgtmp.mp,MMP);
  prints("[1;32m【屬性】[m[1;33m力量(STR)[m%-3d [1;33m智慧(WIS)[m%-3d\
 [1;33m敏捷(DEX)[m%-3d [1;33m體質(CON)[m%-3d [1;33m運氣(KAR)[m%-3d\n"
       ,rpgtmp.str,rpgtmp.wis,rpgtmp.dex,rpgtmp.con,rpgtmp.kar);
prints("[1;32m【攻擊力】[31m 一般 [m%3d [1;36m魔法[m %3d    \
[1;32m【防禦力】[31m 一般 [m%3d [1;36m魔法[m %3d\n"
       ,Att,MA,Def,MD);
  outs("\n\n\n\n");
  outs(COLOR1"[1m ○?????????????????????????????????????????????????????????????????????????龤? [m\n");
  showpicture(rpgtmp.userid);
  pressanykey(NULL);
  return 1;
}

void
rpg_uinfo()
{
  rpg_udisplay(cuser.userid);
  return;
}

void
rpg_race_c()
{
  int strlv1[7] = {0,5,4,3,5,4,4};
  int wislv1[7] = {0,4,5,4,3,3,4};
  int dexlv1[7] = {0,4,4,4,5,5,4};
  int conlv1[7] = {0,4,4,5,3,4,3};
  int karlv1[7] = {0,3,3,4,4,4,5};
  int ans;
  char buf[2],ans2[2];

  setutmpmode(RCHOOSE);
  more(BBSHOME"/game/rpg/choose_race",YEA);
  stand_title("挑選職業");
  if(rpguser.race)
  {
    move(2,4);
    outs("你已經有職業囉,轉換職業會從等級一重新開始唷！");
    getdata(10,5,"是否確定？ (y/N)",buf,3,DOECHO,"n");
    if(!buf[0] || buf[0] == 'n' || buf[0] == 'N') return;
  }
  move(4,0);
  outs(       "挑選職業： (1)灌水強人 (2)讀文快手 (3)常駐程式");
  getdata(5,0,"           (4)聊天猛將 (5)水球冠軍 (6)遊戲剋星 (0)放棄",buf,3,DOECHO,0);
  if(!buf[0] || buf[0] < '1' || buf[0] > '7') return;
  else ans = atoi(buf);
  if(ans == 7) ans =0;
  clear();
  prints("\n你選擇了 %s ！ 其各項數值如下：\n",rname[ans]);
  prints("??????????????????????????????????\n");
  prints("??      等級一時各項數值        ?鱋n");
  prints("??????????????????????????????????\n");
  prints("  力量：%d\n",strlv1[ans]);
  prints("  智慧：%d\n",wislv1[ans]);
  prints("  敏捷：%d\n",dexlv1[ans]);
  prints("  體質：%d\n",conlv1[ans]);
  prints("  運氣：%d\n",karlv1[ans]);
  prints("  體力：%3d       法力：%d\n",conlv1[ans]*30,wislv1[ans]*10);
  prints("  一般攻擊力：%3d 一般防禦力：%d\n",
  ((strlv1[ans]*10)+(dexlv1[ans]*7+karlv1[ans]*5))/3,((conlv1[ans]*10)+(dexlv1[ans]*7)+(karlv1[ans]*5))/3);
  prints("  魔法攻擊力：%3d 魔法防禦力：%d\n",
  ((wislv1[ans]*10)+(karlv1[ans]*5))/2,((conlv1[ans]*7)+(dexlv1[ans]*5)+(wislv1[ans]*5)+(karlv1[ans]*3))/4);

  getdata(14,5,"是否確定？ (y/N)",ans2,2,LCECHO,0);
  if(ans2[0] == 'y' || ans2[0] == 'Y')
  {
    degold(5);
    rpguser.race = ans;
    rpguser.level = 1;
    rpguser.str=strlv1[ans];
    rpguser.wis=wislv1[ans];
    rpguser.dex=dexlv1[ans];
    rpguser.con=conlv1[ans];
    rpguser.kar=karlv1[ans];
    rpguser.hp=conlv1[ans]*30;
    rpguser.mp=wislv1[ans]*10;
    substitute_record(fn_rpg,&rpguser,sizeof(rpgrec),usernum);
    rpg_uinfo();
  }
  else
    rpg_race_c();
}

/* 職業工會 */

rpg_guild()
{
  char ans[5],buf[200];
  int l = rpguser.level;   
  update_data();
  if (!rpguser.race)
  {
    pressanykey("您沒有職業，請由 Join 加入本遊戲");
    return 0;
  }
  setutmpmode(RGUILD);
  clear();
  prints("[1m[33;42m【 %s 同業工會】[m  你現在等級為 %d，有經驗值 %d 點\n\n"
    ,rname[rpguser.race],rpguser.level,cuser.exp);
  prints("\
    ([1;36m0[m)復活            復活會減去所有屬性一點,經驗值扣一半
    ([1;36m1[m)升級            升級須經驗值 %d
    ([1;36m2[m)恢復體力        體力剩 %d，加滿須花費 %d 元
    ([1;36m3[m)恢復法力        法力剩 %d，加滿須花費 %d 元
    ([1;36m4[m)提昇屬性        您的各項屬性上限為 %d 點(每點需 %d 經驗值)
    ([1;36m5[m)資歷查詢中心    查詢別人資料，每次 1000 exp
    ([1;36m6[m)照像館          本館只負責洗照片，底片請自行提供"
  ,LVUP,rpguser.hp,((rpguser.con*30)-rpguser.hp)*10
  ,rpguser.mp,((rpguser.wis*10)-rpguser.mp)*20,l*3,LVUP/10);
  getdata(10,0,"請選擇你需要的服務： ",ans,3,LCECHO,0);
  if(!ans[0]) return 0;
  switch(ans[0])
  {
    default:
      break;
    case '!':
      if(HAS_PERM(PERM_SYSOP))
      {
        rpguser.hp = rpguser.con*30;
        rpguser.mp = rpguser.wis*10;
      }
      break;  
    case '0':
      if(rpguser.hp > 0)
      {
        pressanykey("瘋啦？沒死還來復活！");
        break;
      }
      update_data();
      if(rpguser.str > 1)
        rpguser.str -= 1;
      if(rpguser.wis > 1)
        rpguser.wis -= 1;
      if(rpguser.dex > 1)
        rpguser.dex -= 1;
      if(rpguser.con > 1)
        rpguser.con -= 1;
      if(rpguser.kar > 1)
        rpguser.kar -= 1;
      cuser.exp /= 2;
      rpguser.hp = rpguser.con * 15;
      {
        time_t now = time(0);
        sprintf(buf," %s 花去了 %d 點經驗值，從死亡的邊緣被拖了回來。%s"
          ,cuser.userid,cuser.exp,Cdate(&now));
        f_cat(BBSHOME"/game/rpg/resurge.log",buf);
      }
      sleep(5);
      pressanykey("一道光芒籠罩在你身體的周圍,你慢慢有了知覺....");  
      break; 
    case '1':
    {
      if(rpguser.hp <= 0)
      {
        pressanykey("你已經陣亡了,要先復活才能升級");
        break;
      }
      if(rpguser.level >= 25)
      {
        pressanykey("你已經升到目前的最高等級了");
        break;
      }  
      if(check_exp(LVUP)) break;
      else deexp(LVUP);
      rpguser.str += lvup[0][rpguser.race];
      rpguser.wis += lvup[1][rpguser.race];
      rpguser.dex += lvup[2][rpguser.race];
      rpguser.con += lvup[3][rpguser.race];
      rpguser.kar += lvup[4][rpguser.race];
      rpguser.hp = rpguser.con*30;
      rpguser.mp = rpguser.wis*10;
      if(rpguser.str > (rpguser.level*3)+5)
        rpguser.str = (rpguser.level*3)+5;
      if(rpguser.dex > (rpguser.level*3)+5)
        rpguser.dex = (rpguser.level*3)+5;
      if(rpguser.wis > (rpguser.level*3)+5)
        rpguser.wis = (rpguser.level*3)+5;
      if(rpguser.con > (rpguser.level*3)+5)
        rpguser.con = (rpguser.level*3)+5;
      if(rpguser.kar > (rpguser.level*3)+5)
        rpguser.kar = (rpguser.level*3)+5;
      rpguser.level++;
      substitute_record(fn_passwd, &cuser, sizeof(userec), usernum);
      substitute_record(fn_rpg, &rpguser, sizeof(rpgrec), usernum);
      pressanykey("等級提升至 %d",rpguser.level);
      rpg_uinfo();
    }
    break;

    case '2':
      if(rpguser.hp <= 0)
      {
        pressanykey("你已經陣亡了,要先復活才能補體力");
        break;
      }  
      if(check_money(((rpguser.con*30)-rpguser.hp)*10,SILVER)) return 0;
      demoney(((rpguser.con*30)-rpguser.hp)*10);
      rpguser.hp = rpguser.con*30;
      pressanykey("體力恢復至全滿狀態！");
      break;

    case '3':
      if(check_money(((rpguser.wis*10)-rpguser.mp)*20,SILVER)) return 0;
      demoney(((rpguser.wis*10)-rpguser.mp)*20);
      rpguser.mp = rpguser.wis*10;
      pressanykey("法力恢復至全滿狀態！");
      break;

    case '4':
    {
      char ans2[3],genbuf[100] = "輸入錯誤或是已達到最大值";
      if(rpguser.hp <= 0)
      {
        pressanykey("你已經陣亡了,要先復活才能提昇屬性");
        break;
      }  
      getdata(15,0,
        "要提昇 : [1]力量 [2]智慧 [3]敏捷 [4]體質 [5]運氣 ?",ans2,2,LCECHO,0);
      if(ans2[0] < '1' || ans2[0] > '5') break;
      if(check_exp(LVUP/10)) break;
      switch(ans2[0])
      {
        case '1':
          if(rpguser.str < rpguser.level*3)
          {
            deexp(LVUP/10);
            sprintf(genbuf,"力量提升為 %d 點",++rpguser.str);
          }
          break;
        case '2':
          if(rpguser.wis < rpguser.level*3)
          {
            deexp(LVUP/10);
            sprintf(genbuf,"智慧提升為 %d 點",++rpguser.wis); 
          }
          break;
        case '3':
          if(rpguser.dex < rpguser.level*3)
          {        
            deexp(LVUP/10);
            sprintf(genbuf,"敏捷提升為 %d 點",++rpguser.dex); 
          }
          break;
        case '4':
          if(rpguser.con < rpguser.level*3)
          {        
            deexp(LVUP/10);
            sprintf(genbuf,"體質提升為 %d 點",++rpguser.con);
          }
          break;
        case '5':
          if(rpguser.kar < rpguser.level*3)
          {        
            deexp(LVUP/10);
            sprintf(genbuf,"運氣提升為 %d 點",++rpguser.kar);
          }
          break;
      }
      pressanykey(genbuf);
      break;  
    }        

    case '5':
    {
      char genbuf[20];
      if(check_exp(1000)) break;
      stand_title("查詢別人的資料");
      usercomplete(msg_uid,genbuf);
      if(genbuf[0])
      {
        deexp(1000);
        rpg_udisplay(genbuf);
      }
    }
    break;

    case '6':
      u_editpic();
      break;

  }
  substitute_record(fn_passwd,&cuser,sizeof(userec),usernum);
  substitute_record(fn_rpg,&rpguser,sizeof(rpgrec),usernum);
  rpg_guild();
}

void
rpg_top()
{
  int i; 
  char buf[5],genbuf[200];
  stand_title("RPG 排行榜");
  setutmpmode(RTOPLIST); 
  move(4,0); 
  outs(       "要看什麼職業的排行：(1)灌水強人 (2)讀文快手 (3)常駐程式");
  getdata(5,0,"(t)統計  (a)全部    (4)聊天猛將 (5)水球冠軍 (6)遊戲剋星",buf,2,DOECHO,0);
  if(buf[0] == 'a' || buf[0] == 'A') buf[0] = '7';
  if(buf[0] == 't' || buf[0] == 'T') buf[0] = '8';
  if(!buf[0] || buf[0] < '0' || buf[0] > '8') return;
  i = atoi(buf);
  sprintf(genbuf,"/bbs/bin/toprpg %d %d /bbs/etc/toprpg%d",
    i,i == 7 ? 100 : 50,i);
  system(genbuf);
  sprintf(genbuf,"/bbs/etc/toprpg%s",buf);  
  more(genbuf); 
}

/* 練功 */
int
rpg_kmob(int mode)
{
  mobattr m; 
  int MHP = rpguser.con*30,MMP= rpguser.wis*10;  /* 自己的最大mp,hp */
  int Att = c_att(cuser.userid);  /* 攻擊力 */
  int Def = c_def(cuser.userid);  /* 防禦力 */
  int MA = c_ma(cuser.userid);    /* 魔法攻擊 */
  int MD = c_md(cuser.userid);    /* 魔法防禦 */
  int money,exp;  /* 打完後會增加or減少的$$跟exp */
  char ans[4],buf[256]= "\0"; /* ans給user input用,buf給螢幕output用 */
  int j=0,k=0,l=0,over=0,attack=0; /* over結束戰鬥,attack敵人攻擊 */

  if(rpguser.hp <= 0)
  {
    pressanykey("你已經陣亡了∼∼∼");
    return 0;
  }
//  if(lockutmpmode(RTRAIN)) return 0;  /* 不讓user multi 玩 */
  setutmpmode(RTRAIN); 
  if(mode == hard)
    m.LV = rpguser.level + rand()%10;
  if(mode == medium)
    m.LV = rand()%2 ? rpguser.level + rand()%3 : rpguser.level - rand()%3;
  if(mode == easy)
    m.LV = rand()%3 < 1 ? rpguser.level : rpguser.level - rand()%5;
  if(m.LV < 1) m.LV = 1;  
  m.maxhp = m.hp = m.LV * 100;
  m.Att = m.LV * (10 + rand()%15);
  m.Def = m.LV * (5 + rand()%10);
  m.MA = m.LV * (20 + rand()%5);
  m.MD = m.LV * (5 + rand()%5); 
  m.EXP = (m.LV*m.LV + mode) * ((rand()%5) + 5);
  m.money = (m.LV*m.LV + mode) * ((rand()%2) + 5);
  while(!over){
    update_data();
    if(mode == easy)  
      strcpy(m.name,"肉腳怪物");
    else if(mode == medium)
      strcpy(m.name,"一般怪物");
    else
      strcpy(m.name,"變態怪物");
    clear();
    if(!HAS_PERM(PERM_SYSOP))
    {
      show_file(buf,1,10,ONLY_COLOR);  /* 敵人圖片 */
      move(1,0); 
      prints("等級 : %d",m.LV);
    }
    else
    {
      move(1,0); 
      prints(
"等級 : %d\n生命 : %d\n攻防 : %d / %d\n魔法 : %d / %d\n經驗 : %d\n獎金 : %d"
,m.LV,m.maxhp,m.Att,m.Def,m.MA,m.MD,m.EXP,m.money);
    } 
    move(11,0);
    prints("[36m%s[m",msg_seperator);  /* 分隔線 */
    move(13,0);
    prints("[36m%s[m",msg_seperator);  /* 分隔線 */
    over = 0;
    while(over == 0 && rpguser.hp > 0 && m.hp >0)  /* 戰鬥...直到 over */
    {
      /* 不讓user知道怪物有多少血，所以用身體狀態表示 */
      j = (m.hp/(m.maxhp/100) >= 100 ? 0 :
           m.hp/(m.maxhp/100) >= 75  ? 1 :
           m.hp/(m.maxhp/100) >= 50 ? 2 :
           m.hp/(m.maxhp/100) >= 25 ? 3 : 4);   
      move(0,0);
      clrtoeol();
      if(!HAS_PERM(PERM_SYSOP))
        prints("[1m[33;46m  【 戰 鬥 中 】                       \
[44m◇ 敵人： %8s [40m健康狀態： %s [m",m.name,health[j]);
      else
        prints("[1m[33;46m  【 戰 鬥 中 】          \
[44m◇ 敵人： %8s [40m生命點數：%d/%d [m",m.name,m.hp,m.maxhp);
            move(14,0);
      prints("【自己的狀態】
 ○?????????????????????????????????????????????????稙??????????????????????龤?
 ?齱i體  力】 %-4d / %4d  【法  力】 %-4d / %4d  ?? 【裝備武器】%-8s   ??
 ?齱i攻擊力】 一般 %4d  魔法 %4d                 ?? 【經 驗 值】%-10d ??
 ?齱i防禦力】 一般 %4d  魔法 %4d                 ?? 【身上現金】%-10d ??
 ○?????????????????????????????????????????????????嘵??????????????????????龤?
      ",rpguser.hp,MHP,rpguser.mp,MMP,"空無一物",
        Att,MA,cuser.exp,Def,MD,cuser.silvermoney);
      sprintf(buf,"[1m[46;33m\
 (1)[37m一般攻擊 [33m(2)[37m使用魔法 [33m(3)[37m增強防禦\
 [33m(4)[37m使用道具 [33m(5)[37m投降 [33m(6)[37m逃跑 [33m(7)[37m使用者資料 [m");
      move(20,0);
      outs(buf);
      getdata(22,0,"要做什麼呢？",ans,3,DOECHO,0);

      switch(ans[0])
      {
        default:
          attack = 0;
        break;

        case '!':
          if(HAS_PERM(PERM_SYSOP))
            m.hp -= 10000;  
          break;   
        case '4':
          pressanykey("施工中");
          attack = 0;
          break;

        /* 一般攻擊 */
        case '1':
          j = (rpguser.kar*10)/((rand()%rpguser.kar*2)+2);
          if(j > 50)
          {
            sprintf(buf," [1;31m你用樹枝戳起一陀便便，用力往 [33m%s [31m砸過去！！[m ",m.name);
            j = 5;
          }
          else if(j > 35)
          {
            sprintf(buf," [1;31m你努力的在身上搓搓....搓出一個黑丸子，用力往 [33m%s [31m丟過去！！[m ",m.name);
            j = 3;
          }
          else if(j > 20)  /* 運氣好使出大絕招 */
          {
            sprintf(buf," [1;31m你聚精會神，努力積了一口痰往 [33m%s [31m吐去！！[m ",m.name);
            j = 2;
          }
          else if(j <= 20)
          {
            sprintf(buf," [1;37m你狠狠的往 [33m%s [37m的 [31m%s [37m打下去！！[m "
            ,m.name,body[rand()%5]);
            /* 亂數跑攻擊部位 */
            j = 1;
          }
          move(12,0);
          outs(buf);
          k = (rand()%((Att-m.Def)>=5 ? Att - m.Def : 5))*j;
          sprintf(buf," %s 失血 %d 點。",m.name,k);
          m.hp -= k;
          pressanykey(buf);
          attack = (m.hp > 0 ? 1 : 0);  /* 敵人掛了不會再反擊 */
          break;

        /* 使用魔法 */
          case '2':
          sprintf(buf,"[1m[33;46m\
 (1)[37m攻擊魔法 [33m(2)[37m治療魔法 [33m(3)[37m狂暴魔法[33m\
 (4)[37m石化魔法 [33m(5)[37m傳送魔法 [33m(0)[37m回上層選單    [m");
          move(20,0);
          prints("%79s"," ");
          move(20,0);
          outs(buf);
          getdata(22,0,"要做什麼呢？",ans,3,DOECHO,0);
          switch(ans[0])
          {
            default:
              attack = 0;
              break;

            case '1':
              if(rpguser.mp >= 20)
              {
                rpguser.mp -= 20;
                move(12,0);
                prints(" [1;32m你對著 [33m%s [36m喵喵∼[32m的叫個不停...[33m%s 聽的都快暈去了...[m",m.name,m.name);
                k = rand()%((MA-m.MD)>=30 ? MA - m.MD : 30);
                sprintf(buf," %s 失血 %d 點。",m.name,k);
                m.hp -= k;
                attack = (m.hp > 0 ? 1 : 0);  /* 敵人掛了不會再反擊 */
              }
              else
              {
                sprintf(buf,"法力不夠！");
                attack = 0;
              }
              pressanykey(buf);
              break;

            case '2':
              if(rpguser.mp >= 30 && rpguser.hp < MHP)
              {
                rpguser.mp -= 30;
                rpguser.hp += MHP/5;
                if(rpguser.hp > MHP)
                  rpguser.hp = MHP;
                move(12,0);
                outs(" [1;32m你喃喃念道：「[33m阿拉釋迦玉皇大帝請治療我的傷口，阿門！[32m」  [m");
                sprintf(buf,"體力恢復至 %d 點",rpguser.hp);
                attack = 1;
              }
              else
              {
                if(rpguser.hp >= MHP)
                  sprintf(buf,"你現在精力充沛，不需要治療");
                if(rpguser.mp < 30)
                  sprintf(buf,"法力不夠！");
                attack = 0;
              }
              pressanykey(buf);
              break;

            case '3':
              if(rpguser.mp >= 30)
              {
                rpguser.mp -= 30;
                move(12,0);
                outs(" [1;32m從你手掌中變出幾粒[36m威而鋼[32m，你二話不說馬上吞下去...[m");
                if(Att >= (c_att(cuser.userid)*3/2)) /* 上限 */
                {
                   sprintf(buf,"你的攻擊力已經提升到極限了");
                   attack = 0;  /* 沒升到敵人不攻擊 */
                }
                else
                {
                  Att += rand()%((rpguser.kar*2+rpguser.con*3+rpguser.dex*5)/10);
                  sprintf(buf,"你的攻擊能力提升到 %d 點。",Att);
                  attack = 1;  /* 提升後敵人先打一下 */
                }
              }
              else
              {
                sprintf(buf,"法力不夠！");
                attack = 0;
              }
              pressanykey(buf);
              break;

            case '4':
              if(rpguser.mp >= 10)
              {
                rpguser.mp -= 10;
                move(12,0);
                prints(" 你口中喃喃念咒，變出一罐瘋狂瞬間膠，往 %s 身上潑過去",m.name);
                if((rand()%MD) - m.MD > 0)
                {
                  sprintf(buf,"%s 被潑到了！定在那邊一動也不能動...",m.name);
                  l += 3;
                }
                else
                {
                  sprintf(buf,"%s 拿出一個水桶，把瞬間膠接住了...",m.name);
                  attack = 1;
                }
              }
              else
              {
                sprintf(buf,"法力不夠！");
                attack = 0;
              }
              pressanykey(buf);
              break;

            case '5':
              if(rpguser.mp >= 50)
              {
                rpguser.mp -= 50;
                move(12,0);
                prints(" [1;32m你眼看打不過 [33m%s [32m了，大喊一聲：[31m惡靈退散！！  [m",m.name);
                pressanykey(" 你的腳底下突然颳起一陣怪風，把你吹走了...");
                rpg_guild();
                unlockutmpmode();
                return 0;
              }
              else
                pressanykey("法力不夠！");
              attack = 0;
              break;
          }
          break;

      /* 提升防禦力 */
        case '3':
          move(12,0);
          outs(" [1;33m你大喊：「萬能的天神，請賜予我神奇的力量....」[m");
          if(Def >= (c_def(cuser.userid)*3/2)) /* 上限 */
          {
            sprintf(buf,"你的防禦力已經提升到極限了");
            attack = 0;  /* 沒升到敵人不攻擊 */
          }
          else
          {
            Def += (rand()%(rpguser.kar*2+rpguser.con*3+rpguser.dex*5))/10;
            sprintf(buf,"你的防禦能力提升到 %d 點。",Def);
            attack = 1;  /* 提升後敵人先打一下 */
          }
          pressanykey(buf);
          break;

        /* 投降 */
        case '5':
          j=(rand()%rpguser.kar) - rand()%m.LV;  /* 碰運氣吧 :p */
          move(12,0);
          prints("[1;35m你跪在 %s 前面說：「我有眼不識泰山，拜託饒了我一命吧！」[m",m.name);
          money = m.LV*100/((rand()%rpguser.kar/5)+1);
          if(j > 0 && cuser.silvermoney >= money)
          {
            sprintf(buf,"%s 決定放你一馬，收了你 %d 元的保護費。",m.name,money);
            demoney(money);
            over = 1;
            attack = 0;
          }
          else
          {
            sprintf(buf,"%s 覺得還是把你殺了比較快！",m.name);
            attack = 1;
          }
          pressanykey(buf);
          break;

        /* 嘗試逃跑 */
        case '6':
          j=(rand()%rpguser.kar) - m.LV;  /* 碰運氣吧 :p */
          move(12,0);
          outs(" [1;32m你嘗試逃跑....[m");
          if(j > 0)
          {
            money = m.LV*50/((rand()%rpguser.kar)+1);
            sprintf(buf,"逃跑成功\，但是驚慌中掉了 %d 元。",money);
            demoney(money);
            over = 1;
            attack = 0;
          }
          else
          {
            attack = 1;  /* 逃跑失敗敵人補一拳 */
            sprintf(buf,"逃跑失敗！");
          }
          pressanykey(buf);
          break;

        case '7':
          rpg_uinfo();
          attack = 0;
          break;
      }

      if(l > 0)
      {
        attack = 0;
        sprintf(buf,"%s 被定住了，還要 %d 回合才能動作....",m.name,l--);
        pressanykey(buf);
      }

      /* 敵人的攻擊 */
      if(attack == 1)
      {
        move(12,0);
        prints("%-255s"," ");
        move(12,0);
        prints(" [1;33m%s [31m攻擊！！[m",m.name);
        k = rand()%((m.Att-Def) >=5 ? m.Att - Def :5);  /* 敵人沒特攻 */
        sprintf(buf," 你 失血 %d 點。",k);
        rpguser.hp -= k;
        if(rpguser.hp < 0) rpguser.hp = 0; // 負的斷線後會加回來?
        substitute_record(fn_rpg, &rpguser, sizeof(rpgrec), usernum);
        substitute_record(fn_passwd, &cuser, sizeof(userec), usernum);
        /* 避免User斷線沒紀錄到 */
        pressanykey(buf);
        move(12,0);
        prints("                                                                               ");   
      }

      /* 敵人掛了 */
      if(m.hp <= 0)
      {
        money = m.money + rand()%(20*m.LV);
        exp = m.EXP + rand()%(10*m.LV);
        sprintf(buf,"終於贏了！！得到 %d 點經驗值，以及 %d 元",exp,money);
        pressanykey(buf);
        inmoney(money);
        inexp(exp);
        over = 1;  /* 迴圈結束 */
      }

      /* 敵人逃跑 */
      if((m.hp*100)/m.maxhp < 3 && rand()%10 < 3 && m.hp > 0)
      {
        money = (rand()%m.money)+20;  /* 敵人逃跑賺到的錢跟exp比較少 */
        exp = (rand()%m.EXP)+10;
        sprintf(buf,"%s 打不過你逃走了，得到 %d 經驗值 %d 元",m.name,exp,money);
        pressanykey(buf);
        inmoney(money);
        inexp(exp);
        over = 1;  /* 迴圈結束 */
      }
      /* 陣亡囉 */
      if(rpguser.hp <= 0)
      {
        money = (rand()%(m.LV*100))+100;
        exp = (rand()%(m.LV*m.LV*10)) + 100;
        sprintf(buf,"不幸陣亡了！！損失%d點經驗值,%d元",exp,money);
        pressanykey(buf);
        demoney(money);
        deexp(exp);
        rpguser.hp = 0;   /* hp不會變負的 */
        over = 1;       /* 迴圈結束 */
      }
    }
    substitute_record(fn_passwd, &cuser, sizeof(userec), usernum);
    substitute_record(fn_rpg, &rpguser, sizeof(rpgrec), usernum);
    return 0;
  }
  return 0;
}


void
rpg_train()
{
  char buf[3];
  int mode;

  if(check_money(10*rpguser.level,SILVER)) return;
  demoney(10*rpguser.level);
  stand_title("訓練場");
  show_file(BBSHOME"/game/rpg/Welcome_train", 2, 15, ONLY_COLOR);
  getdata(18,0,"喜好選擇 1.自虐傾向 2.努力不懈 3.輕鬆自如 ",buf,2,DOECHO,0);
  if(buf[0] == '1') mode = hard;
  else if(buf[0] == '2') mode = medium;
  else if(buf[0] == '3') mode = easy;
  else return;
  do{
    rpg_kmob(mode);
    buf[0] = answer("是否繼續？ (y/N)");
  }while(buf[0] == 'y');
  return;
}

/* 修改資料 */

int
rpg_uquery(char *userid)
{
  char buf[80];
  rpgrec rpg;
  int u;

  if(u = getuser(userid))
    memcpy(&rpg , &rpgtmp, sizeof(rpgrec));
  else return RC_NONE;

  getdata(23,0,"是否要修改資料(Y/N)？  [N]",buf,3,DOECHO,"N");
  if(buf[0] == 'y')
  {
    clear();
    sprintf(buf,"%i",rpg.race);
    getdata(0,0,"職業： (1)灌水強人 (2)讀文快手 (3)常駐程式\n"
                "       (4)聊天猛將 (5)水球冠軍 (6)遊戲剋星 ",
    buf,3,LCECHO,buf);
      if(buf[0] < '7' && buf[0] > '0')
        rpg.race= atoi(buf);
    sprintf(buf,"%i",rpg.level);
    getdata(2,0,"輸入等級：",buf,4,DOECHO,buf);
      if(atol(buf) >=0)
        rpg.level = atol(buf);
    sprintf(buf,"%i",rpg.hp);
    getdata(3,0,"體力：",buf,8,DOECHO,buf);
      if(atol(buf) >=0)
        rpg.hp = atol(buf);
    sprintf(buf,"%i",rpg.mp);
    getdata(3,40,"法力：",buf,8,DOECHO,buf);
      if(atol(buf) >=0)
        rpg.mp = atol(buf);
    sprintf(buf,"%i",rpg.str);
    getdata(5,0,"力量：",buf,4,DOECHO,buf);
      if(atol(buf) >=0)
        rpg.str = atol(buf);
  sprintf(buf,"%i",rpg.wis);
  getdata(5,40,"智慧：",buf,4,DOECHO,buf);
      if(atol(buf) >=0)
        rpg.wis = atol(buf);
  sprintf(buf,"%i",rpg.dex);
  getdata(6,0,"敏捷：",buf,4,DOECHO,buf);
      if(atol(buf) >=0)
        rpg.dex = atol(buf);
  sprintf(buf,"%i",rpg.con);
  getdata(6,40,"體質：",buf,4,DOECHO,buf);
      if(atol(buf) >=0)
        rpg.con = atol(buf);
  sprintf(buf,"%i",rpg.kar);
  getdata(7,0,"運氣：",buf,4,DOECHO,buf);
      if(atol(buf) >=0)
        rpg.kar = atol(buf);
  sprintf(buf,"%i",rpg.weapon);
  getdata(9,0,"武器代碼：",buf,4,DOECHO,buf);
      if(atol(buf) >=0)
        rpg.weapon = atol(buf);
  sprintf(buf,"%i",rpg.armor);
  getdata(10,0,"防具代碼：",buf,4,DOECHO,buf);
      if(atol(buf) >=0)
        rpg.armor=atol(buf);
  sprintf(buf,"%i",rpg.age);
  getdata(11,0,"年齡：",buf,9,DOECHO,buf);
      if(atol(buf) >=0)
        rpg.age=atol(buf);
  getdata(20, 0, msg_sure_yn, buf, 3, LCECHO,"y");
    if (buf[0] != 'y')
    {
      pressanykey("放棄修改");
    }
    else
    {
      log_usies("SetRPG", rpg.userid);
      substitute_record(fn_rpg, &rpg, sizeof(rpg), u);
      pressanykey("修改完成");
    }
  }
  return 0;
}

/* 編輯User資料 */
rpg_edit()
{
  int id;
  char genbuf[200];
  stand_title("修改資料");
  usercomplete(msg_uid,genbuf);
  if(*genbuf)
  {
    move(2,0);
    if (id = getuser(genbuf))
    {
      rpg_udisplay(genbuf);
      rpg_uquery(genbuf);
    }
    else
    {
      pressanykey(err_uid);
    }
  }
  return 0;
}

/* PK */

int
rpg_pk(fd,enemyid)
  int fd;
  char enemyid[IDLEN+1];
{
  rpgrec enemy; 
  userec euser; 
  int MHP = rpguser.con*30,MMP= rpguser.wis*10;  /* 自己的最大mp,hp */
  int Att = c_att(cuser.userid);  /* 攻擊力 */
  int Def = c_def(cuser.userid);  /* 防禦力 */
  int MA = c_ma(cuser.userid);    /* 魔法攻擊 */
  int MD = c_md(cuser.userid);    /* 魔法防禦 */
  int Emhp,Emmp,Eatt=c_att(enemyid) ,Edef= c_def(enemyid)
      ,Ema=c_ma(enemyid) ,Emd=c_md(enemyid) ; 
  int money,exp;  /* 打完後會增加or減少的$$跟exp */
  char buf[256]= "\0"; /* ans給user input用,buf給螢幕output用 */
  int j=0,k=0,l=0,over=0,attack=0; /* over結束戰鬥,attack敵人攻擊 */
  int ans;
  user_info *my = currutmp;

  getuser(enemyid);
  enemy = rpgtmp;
  euser = xuser;  
  Emhp = enemy.con*30; Emmp = enemy.wis*10;     
  add_io(fd, 0);

  setutmpmode(RPK); 

  for(;;)
  {
    if(over == 1) 
    {
      add_io(0, 0);
      close(fd);
      break;
    }
    else
    {
      update_data();
      clear();
      if(!HAS_PERM(PERM_SYSOP))
      {
        sethomefile(buf, enemyid, "picture");
        show_file(buf,1,10,ONLY_COLOR);  /* 敵人圖片 */
      }
      else
      {
        move(1,0); 
        prints(
"等級 : %d\n生命 : %d\n攻防 : %d / %d\n魔法 : %d / %d\n經驗 : %d\n銀幣 : %d"
,enemy.level,Emhp,Eatt,Edef,Ema,Emd,euser.exp,euser.silvermoney);
      } 
      move(11,0);
      prints("[36m%s[m",msg_seperator);  /* 分隔線 */
      move(13,0);
      prints("[36m%s[m",msg_seperator);  /* 分隔線 */
      over = 0;
      while(over == 0)  /* 戰鬥...直到 over */
      {
        /* 不讓user知道怪物有多少血，所以用身體狀態表示 */
        j = ((enemy.hp*100)/Emhp >= 100 ? 0 :
            (enemy.hp*100)/Emhp >= 75  ? 1 :
            (enemy.hp*100)/Emhp >= 50 ? 2 :
            (enemy.hp*100)/Emhp >= 25 ? 3 : 4);   
        move(0,0);
        clrtoeol();
        if(!HAS_PERM(PERM_SYSOP))
          prints("[1m[33;46m  【 戰 鬥 中 】                       \
[44m◇ 敵人： %8s [40m健康狀態： %s [m",euser.userid,health[j]);
        else
          prints("[1m[33;46m  【 戰 鬥 中 】          \
[44m◇ 敵人： %8s [40m生命點數：%d/%d [m",euser.userid,enemy.hp,Emhp);
        move(14,0);
        prints("【自己的狀態】
 ○?????????????????????????????????????????????????稙??????????????????????龤?
 ?齱i體  力】 %-4d / %4d  【法  力】 %-4d / %4d  ?? 【裝備武器】%-8s   ??
 ?齱i攻擊力】 一般 %4d  魔法 %4d                 ?? 【經 驗 值】%-10d ??
 ?齱i防禦力】 一般 %4d  魔法 %4d                 ?? 【身上現金】%-10d ??
 ○?????????????????????????????????????????????????嘵??????????????????????龤?
        ",rpguser.hp,MHP,rpguser.mp,MMP,"空無一物",
          Att,MA,cuser.exp,Def,MD,cuser.silvermoney);
        sprintf(buf,"[1m[46;33m (1)[37m一般攻擊");
        move(20,0);
        outs(buf);
        move(23,0);
        outs(my->turn ? "輪到你的攻擊!" : "對方攻擊中...");

        if(my->turn)
        {
          move(22,0);
          outs("要做什麼呢？");
          ans=igetch();
          switch(ans)
          {
            default:
              attack = 0;
              pressanykey("施工中");
              break;
            /* 一般攻擊 */
            case '1':
              sprintf(buf," [1;37m你狠狠的往 [33m%s [37m的 [31m%s [37m打下去！！[m "
              ,euser.userid,body[rand()%5]);
              /* 亂數跑攻擊部位 */
              move(12,0);
              outs(buf);
              k = (rand()%((Att-Edef)>=5 ? Att - Edef : 5))*j;
              sprintf(buf," %s 失血 %d 點。",euser.userid,k);
              enemy.hp -= k;              
              pressanykey(buf);
              my->turn = -1;
              send(fd, &my, sizeof(user_info), 0);
              my->turn = 0;
              attack = (enemy.hp > 0 ? 1 : 0);  /* 敵人掛了不會再反擊 */
              break;
            case 'q':
              over = 1;  /* 迴圈結束 */
              break;
          }
        }
        do{
          if(ans == I_OTHERDATA)
          {
            recv(fd, &my, sizeof(user_info), 0);
            if(my->turn == -1) my->turn = 1;
          }
        }while(!my->turn);
      }
    }
  }
  return 0;
}
