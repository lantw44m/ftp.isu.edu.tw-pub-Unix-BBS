
/* 使用錢的函數 */

#include "bbs.h"

/* 花錢選單 */
unsigned setuperm(pbits,nb,money)  /* 買權限用 */
  unsigned pbits;
  char nb;
  int money;
{
  register int i;
    i = nb - 'a';
    if(!((pbits>>i)&1))
    {
     pbits ^= (1 << i);
     degold(money);
    }
    return(pbits);
}

/*買賣記錄*/
void
tradelog(userid,i)
char *userid;
int i;
{
  time_t now = time(0);
  char genbuf[200];
  char *item[5]={"信箱上限提昇","終極隱身大法","故鄉修改寶典","信件無上限","透視鏡"};
  now = time(NULL) - 6 * 60;
  sprintf(genbuf,"在 %s 使用者 [1;32m%s[m 購買了[1;36m%s[m的權限",Cdate(&now),cuser.userid,item[i]);
  f_cat("log/trade.log",genbuf);
}

void
p_cloak()
{
  char buf[4];
  move(b_lines-2,0);
  if(check_money(BUY_C,GOLD))      return;
  prints("金額：%d    ",BUY_C); // cost
  getdata(b_lines-1, 0,
    (currutmp->invisible==1) ? "確定要現身?[y/N]" : "確定要隱身?[y/N]",buf,3,LCECHO,"N");
  if(buf[0]!='y')    return;
  degold(BUY_C);

  if(currutmp->invisible & 1)
    currutmp->invisible &= ~1;
  else
    currutmp->invisible |= 1;
            
  pressanykey((currutmp->invisible==1) ? MSG_CLOAKED : MSG_UNCLOAK);
  return;
}

void
p_scloak()
{
  char buf[4];
  move(b_lines-2,0);
  if(check_money(BUY_C,GOLD))      return;
  prints("金額：%d    ",BUY_S); // cost
  getdata(b_lines-1, 0,"確定要購買透視鏡? (y/N) ",buf,3,LCECHO,"N");
  if(buf[0]!='y')    return;
  degold(BUY_S);
  currutmp->invisible |= SEE;
  pressanykey(MSG_SEECLOAK);
  return;
}


void
p_fcloak()
{
  register int i;
  char ans[4];
  if(check_money(BUY_SC,GOLD) || HAS_PERM(PERM_CLOAK))
  {
    if(HAS_PERM(PERM_CLOAK))
      pressanykey("你已經可以隱形了還來買，嫌錢太多啊？");
    return;
  }
  move(b_lines-3,0);
  prints("金額：%d    ",BUY_SC); // cost
  getdata(b_lines-2, 0, "確定要購買終極隱身大法？[y/N]",ans,3,LCECHO,0);
  if(ans[0]!='y')
    return;
  rec_get(fn_passwd, &xuser, sizeof(xuser), usernum);
  i=setuperm(cuser.userlevel,'g',BUY_SC);
  update_data();
  cuser.userlevel=i;
  substitute_record(fn_passwd, &cuser, sizeof(userec), usernum);
  tradelog(cuser.userid,1);
  pressanykey("恭喜您已經學會了終極隱身大法!!");
  return;
}

void
p_from()
{
  char ans[4], genbuf[32];
  move(b_lines-3,0);
  if(check_money(BUY_F,GOLD)) return;
  prints("金額：%d    ",BUY_F); // cost
  getdata(b_lines-2, 0, "確定要改故鄉?[y/N]",ans,3,LCECHO,"N");
  if(ans[0]!='y') return;
  if (getdata(b_lines-1, 0, "請輸入新故鄉：", genbuf, 17, DOECHO,0))
        {
           degold(BUY_F);
           strip_ansi(genbuf, genbuf, 0);
           strncpy(currutmp->from, genbuf, 24);
           currutmp->from_alias=0;
        }
  return;
}

void
p_ffrom()
{
  register int i;
  char ans[4];
  if(check_money(BUY_SF,GOLD) || HAS_PERM(PERM_FROM) || HAS_PERM(PERM_SYSOP))
  {
    if(HAS_PERM(PERM_FROM) || HAS_PERM(PERM_SYSOP))
      pressanykey("你已經可以修改故鄉了還來買，嫌錢太多啊？");
    return;
  }
  move(b_lines-3,0);
  prints("金額：%d    ",BUY_SF); // cost
  getdata(b_lines-2, 0, "確定要購買修改故鄉寶典？[y/N]",ans,3,LCECHO,0);
  if(ans[0]!='y') return;
    rec_get(fn_passwd, &xuser, sizeof(xuser), usernum);
  i=setuperm(cuser.userlevel,'t',BUY_SF);
  update_data();
  cuser.userlevel=i;
  substitute_record(fn_passwd, &cuser, sizeof(userec), usernum);
  tradelog(cuser.userid,2);
  pressanykey("恭喜你得到了故鄉修改寶典");
  return;
}

void
p_exmail()
{
  char ans[4],buf[100];
  int  n;
  if(cuser.exmailbox >= MAXEXKEEPMAIL )
  {
    pressanykey("容量最多增加 %d 封，不能再買了。", MAXEXKEEPMAIL);
    return;
  }
  move(b_lines-3,0);
  prints("金額：%d    ",BUY_M); // cost  
  sprintf(buf,"您曾增購 %d 封容量，還要再買多少?",cuser.exmailbox);
  getdata(b_lines-2, 0, buf,ans,3,LCECHO,"10");
  n = atoi(ans);
  if(!ans[0] || !n )
    return;
  if(n+cuser.exmailbox > MAXEXKEEPMAIL )
     n = MAXEXKEEPMAIL - cuser.exmailbox;
  if(check_money(n*BUY_M,GOLD))
    return;
  degold(BUY_M*n);
  inmailbox(n);
  return;
}

/*
void
p_ulmail()
{
  register int i;
  char ans[4];
  if(check_money(100000,GOLD) || HAS_PERM(PERM_MAILLIMIT))
  {
    if(HAS_PERM(PERM_MAILLIMIT))
      pressanykey("你的信箱已經沒有限制了還來買，嫌錢太多啊？");
    return;
  }
  getdata(b_lines-2, 0, "確定要花 $100000 購買無上限信箱?[y/N]",ans,3,LCECHO,0);
  if(ans[0]!='y')
    return;
  rec_get(fn_passwd, &xuser, sizeof(xuser), usernum);
  i=setuperm(cuser.userlevel,'f',100000);
  update_data();
  cuser.userlevel=i;
  substitute_record(fn_passwd, &cuser, sizeof(userec), usernum);
  tradelog(cuser.userid,3);
  pressanykey("恭喜您已經得到了無上限的信箱!!");
  return;
}
*/

void
p_give()
{
   int mymoney, money, mode=0, rc, rate=50;
   char id[IDLEN+1],buf[256],reason[60];
   FILE *fp=fopen("tmp/givemoney","w");
   fileheader mymail;
   time_t now;
   
   time(&now);
   move(15,0);
   update_data();
   usercomplete("  請輸入對方的ID：", id);
   if (!id[0] || !ci_strcmp(id,"guest"))
      return;
      
   getdata(17, 0, "  0. 銀幣  1.金幣：",buf, 2, LCECHO, "0");
   if(buf[0]=='1') mode=1;
   if(buf[0]=='0') mode=0;

   getdata(19, 0, "  要轉多少過去？", buf, 9, LCECHO,0);
   
   mymoney = atoi(buf);
   if(mymoney<100)
   {
     pressanykey("抱歉，每次轉帳交易之數值下限為100");
     return;
   }
   
   if(check_money(mymoney, mode)) return;

   money = mymoney - mymoney/rate;

   if(mode==1)
   {
     degold(mymoney);
     rc=inugold(id, money);
   }
   else
   {
     demoney(mymoney);
     rc=inumoney(id, money);
   }
   
   if(rc>0)
   {
     sprintf(buf,"作者: %s \n"
                 "標題:[轉帳通知] 送你 %d 枚 %s ！\n"
                 "時間: %s\n",
                 cuser.userid,
                 money,
                 (mode ? "金幣" : "銀幣"),
                 ctime(&now));
     fputs(buf,fp);
     while(!getdata(21,0,"  請輸入一個理由：",reason,60,DOECHO ,"錢太多"));
     
     sprintf(buf,"[1;32m%s[37m 轉帳 [33m%d [37m 枚 %s 到你的帳戶裡。\n"
                 "他的理由是：[33m %s [m",
                 cuser.userid,
                 money,
                 (mode ? "金幣" : "銀幣"),
                 reason);
     fputs(buf,fp);

     sprintf(buf,"home/%s/mailbox", id);
     stampfile(buf, &mymail);
     strcpy(mymail.owner, cuser.userid);
     rename ("tmp/givemoney",buf);
     
     sprintf(mymail.title,"[轉帳通知] %d 枚 %s 進帳了！",
     money,
     (mode ? "金幣" : "銀幣")) ;
     sprintf(buf,"home/%s/mailbox/.DIR",id);
     rec_add(buf, &mymail, sizeof(mymail));
     sprintf(buf,"[1;33m%s %s [37m把 %s [33m%d 枚 [37m轉帳給[33m %s[37m",
     Cdate(&now),
     cuser.userid,
     (mode ? "金幣" : "銀幣"),
     money,
     id);
     f_cat("log/bank.log",buf);
   }
   else
   {
     if(mode==1)
       ingold(mymoney);
     else
       inmoney(mymoney);
       
     pressanykey("轉帳失敗!");
   }
   fclose(fp);
   return;
}

void
water_exchange()
{
  char buf[128], ans[10];
  int Money, rate=1;
//  time_t now = time(0);

  move(13,0);
  clrtobot();
  prints("  你收集到的水球有 %d 個\n\n",cuser.receivemsg);
  prints("  今日匯兌比率：  水球：銀幣 = 1：%d\n",rate);

  getdata(17,0,"  要拿多少水球來換？ ",ans,10,LCECHO,0);
  if(!ans[0]) return;
  
  Money = atol(ans);
  if(Money<1 || Money > cuser.receivemsg) return;
  
  sprintf(buf,"  是否要將水球 %d 個 換成銀幣 %d 枚 ? [y/N] ",
          Money, Money);
  
  getdata(19,0,buf,ans,2,LCECHO,"y");
  
  if(ans[0] == 'y')
  {
    rec_get(fn_passwd, &xuser, sizeof(xuser), usernum);
    cuser.receivemsg = cuser.receivemsg - Money;
    substitute_record(fn_passwd, &cuser,sizeof(userec), usernum);
    inmoney(Money);
    pressanykey("OK! 您目前的銀幣為 %d 枚", cuser.silvermoney);
  }
  else
    pressanykey("取消.....");
  
}


void
exchange()
{
  char buf[100],ans[10];
  int i, mymoney, money, rate=50;
  time_t now = time(0);

  move(13,0);
  clrtobot();
  prints("  你身上有金幣 %d 元,銀幣 %d 元\n\n",cuser.goldmoney,cuser.silvermoney);
  prints("  今日匯兌比率：  金幣：銀幣 = 1：10000\n");
  
  if(!getdata(17,0,"  0.銀幣換金幣  1.金幣換銀幣 ",ans,2,LCECHO,0)) return;
  if(ans[0] < '0' || ans[0] > '1') return;
  i = atoi(ans);
  while(1)
  {
    if(i == 0)
      getdata(19,0,"  要拿多少銀幣來換？ ",ans,10,LCECHO,0);
    else
      getdata(19,0,"  要拿多少金幣來換？ ",ans,10,LCECHO,0);
    if(!ans[0]) return;
    mymoney = atol(ans);
    if(i == 0)
    {
       if(mymoney<10000 || mymoney > cuser.silvermoney)
          continue;
    }
    else
    {
       if(mymoney<=0 || mymoney > cuser.goldmoney)
          continue;
    }
    break;
  }
  
  if(i == 0)
     money = mymoney/10000;
  else
     money = mymoney*10000 - mymoney*10000/rate;
  
  if(i == 0)
    sprintf(buf,"  是否要將銀幣 %d 枚 換為金幣 %d 枚 ? [y/N]",mymoney,money);
  else
    sprintf(buf,"  是否要將金幣 %d 枚 換為銀幣 %d 枚 ? [y/N]",mymoney,money);
    
  getdata(21,0,buf,ans,2,LCECHO,0);
  if(ans[0] == 'y')
  {
    if(i == 0)
    {
      demoney(mymoney); ingold(money);
      sprintf(buf,"[1;36m%s %s [37m把銀幣 [33m%d 元 [37m轉換為金幣 %d 元",
        Cdate(&now),cuser.userid,mymoney, money);
    }
    else
    {
      degold(mymoney); inmoney(money);
      sprintf(buf,"[1;32m%s %s [37m把金幣 [33m%d 元 [37m轉換為銀幣 %d 元",
        Cdate(&now),cuser.userid,mymoney, money);
    }
    f_cat("log/bank.log",buf);
    pressanykey("OK! 您目前的金幣 %d 枚, 銀幣 %d 枚",
                cuser.goldmoney,cuser.silvermoney);
  }
  else
    pressanykey("取消.....");
}

/* 金庫 */
void
bank()
{
  char buf[10];
  if(lockutmpmode(BANK)) return;
  setutmpmode(BANK);
  stand_title("電子銀行");

  if (count_multi() > 1)
  {
    pressanykey("您不能派遣分身進入銀行呦 !");
    unlockutmpmode();
    return;
  }

  counter(BBSHOME"/log/counter/bank","使用銀行",0);
  move(2, 0);
  update_data();
  prints("%12s 您好呀！歡迎光臨本銀行。
[1;36m??????????????????????????????????????????????????????????????
??[32m您現在有銀幣:[33m %-12d [32m枚，金幣:[33m %-12d[32m 枚[36m      ??
??[32m您的銀幣上限:[33m %-12d [32m枚。[36m                           ??
?僓???????????????????????????????????????????????????????????
?? 目前銀行提供下列三項服務：                               ??",
    cuser.userid, cuser.silvermoney, cuser.goldmoney,
    (cuser.totaltime*1) + (cuser.numlogins*10) + (cuser.numposts*1000) + 10000 );
    move(7, 0);
      outs("\
??[33m1.[36m 轉帳服務[37m：1000枚銀幣以上 (手續費： 2%)                [36m ??
??[33m2.[33m 匯兌服務[37m：銀幣 <--> 金幣 (手續費： 2%)                [36m ??
??[33m3.[32m 交換服務[37m：水球 ---> 銀幣 (手續費： 0%)                [36m ??
??[33mQ.[37m 離開銀行[36m                                               ??
??????????????????????????????????????????????????????????????[m");

  getdata(13,0,"  請輸入您需要的服務：", buf, 2, DOECHO, 0);
  if (buf[0] == '1')
    p_give();
  else if (buf[0] == '2')
    exchange();
  else if (buf[0] == '3')
    water_exchange();

  update_data();
  pressanykey("謝謝光臨，下次再來！");
  unlockutmpmode();
}

void
goldwish()
{
  int i,m;
//  time_t now = time(0);

  move(2,0);
  clrtobot();
  degold(1);

  i=rand() % 3000 - 2000;
  move(13,26);

  if (i>0)
     if (i%100 ==0)
     {
       m=i/100;
       cuser.goldmoney+=m;
       prints("天下掉下來金幣 %d 枚!",m);
     }
     else
     {
       cuser.exp+=i;
       prints("經驗值上升 %d 點!",i);
     }

  else
     if (i%10==0)
     {
       m = (cuser.goldmoney > (-i/10) ? (-i/10) : cuser.goldmoney);
       cuser.goldmoney-=m;
       prints("%d 枚金幣變成了石頭...",m);
     }
     else
     {
       m = (cuser.exp > (-i/10) ? (-i/10) : cuser.exp);
       cuser.exp-=m;
       prints("經驗值下降 %d 點...",m);
     }
  substitute_record(fn_passwd, &cuser, sizeof(userec), usernum);

  pressanykey("願望小惡魔：恭喜你....喝喝喝...");
}

void
silverwish()
{
  int i;
//  time_t now = time(0);
  move(2,0);
  clrtobot();
  demoney(1000);
  i=rand() % 200 - 100;
  i= (i<0) ? 1 : i;
  move(13,26);

  if ((i+1)%25==0)
  {
    cuser.bequery = xuser.bequery + (i%2);
    prints("人氣指數上升 %d 點!",(i%2));
  }
  else
  {
    cuser.exp = xuser.exp + i;
    prints("經驗值上升 %d 點!",i);
  }
  substitute_record(fn_passwd, &cuser, sizeof(userec), usernum);

  pressanykey("願望小惡魔：恭喜你....喝喝喝...");
}

void
darkwish()
{
  char buf[10];
  if(lockutmpmode(WISH)) return;
  setutmpmode(WISH);
  stand_title("黑暗許\願池");

  if (count_multi() > 1)
  {
    pressanykey("您不能派遣分身進入許\願池哦 !");

    unlockutmpmode();
    return;
  }

  counter(BBSHOME"/log/counter/darkwish","黑暗許\願池",0);

  do{
  move(3, 0);
  prints("%12s 嘿...您好..歡迎來到 黑暗許\願池...

  我是[1;31;40m願望小惡魔[1;37;40m，只要您肯花小錢，本魔就達成你的願望..

  嘿嘿，先讓我看看你有多少錢... [1;33;40m金子[1;37;40m:%-12d [1;36;40m銀兩[1;37;40m:%-12d",

  cuser.userid, cuser.goldmoney, cuser.silvermoney);
  move(9, 0);

  outs("
[1;33;40m  1. 一枚金幣許\一願 -- 好運得好願，壞運得壞願， 全靠您的手氣..嘿嘿..

[1;36;40m  2. 千枚銀幣得一願 -- 有本魔罩你，有好無壞啦..

[1;37;40m  Q. 離開

");
  getdata(16,0,"  請選擇您要的吧，嘿嘿..：", buf, 3, DOECHO, 0);
  if (buf[0] == '1' && cuser.goldmoney >= 1)
    goldwish();
  else if (buf[0] == '2' && cuser.silvermoney >= 1000)
    silverwish();
  else
      break;
//  update_data();
  }while(1);

  pressanykey("嘿嘿嘿....下次再來呀...");
  unlockutmpmode();
}

void
time_machine()
{
  char genbuf[128], pwdfile[128], keyword[128], ans[10];
  int id=0;

  if(lockutmpmode(TIME_MACHINE)) return;

  setutmpmode(TIME_MACHINE);
  stand_title("黑暗時光機");
  if (count_multi() > 1)
  {
    pressanykey("您不能派遣分身進入!");
    
    unlockutmpmode();
    return;
  }
  if (!HAS_PERM(PERM_LOGINOK))
  {
    pressanykey("您的權限不足!");
    unlockutmpmode();
    return;
  }

  counter(BBSHOME"/log/counter/time_machine","時光機",0);

  update_data();
  pressanykey("時光機啟動中...");
      
  ans[0]='1';
  while(ans[0]!='3')
  {
    stand_title("黑暗時光機");
    move(1,0);
    clrtobot();
    move(1,0);
    prints("


                      ????????????????????????????
                      ?灡犮?機操作選單          ??
                      ?僓?????????????????????????
                      ??1. 時光記錄簿  $ 1 金   ??
                      ??2. 時光回溯器  $ 10銀   ??
                      ??3. 離開                 ??
                      ????????????????????????????

");
    getdata(12,0,"  請輸入您需要的服務：", ans, 2, DOECHO, 0);
    if(ans[0]=='1' && !check_money(1,1))
    {
       sprintf(pwdfile, "cat boards/System_Log/M*");
       getdata(14,0,"  請輸入要查詢的關鍵文字之一：",genbuf, 30, DOECHO, 0);
       genbuf[30]='\0';
       sprintf(keyword, " | grep \"%s\"", genbuf);
       strcat(pwdfile, keyword);

       getdata(15,0,"  請輸入要查詢的關鍵文字之二：",genbuf, 30, DOECHO, 0);
       genbuf[30]='\0';
       sprintf(keyword, " | grep \"%s\"", genbuf);
       strcat(pwdfile, keyword);

       sprintf(keyword, "tmp/%s.log", cuser.userid);
       sprintf(genbuf, "%s > %s", pwdfile, keyword);
       
       system(genbuf);
       more(keyword, YEA);
       degold(1);
    }
    else if(ans[0]=='2' && !check_money(10,0))
    {
      if(HAS_PERM(PERM_SYSOP))
      {
        getdata(14,0,"  請輸入要查詢的ID：",genbuf, IDLEN+1, DOECHO, 0);
        if(genbuf[0])
          strcpy(keyword, genbuf);
      }
      else
      {
        strcpy(keyword, cuser.userid);
      }
        
         getdata(15,0,"  請輸入要回溯的日期：",genbuf, 5, DOECHO, 0);
         genbuf[5]='\0';
         sprintf(pwdfile,"backup/passwds_%s", genbuf);
         if(dashs(pwdfile)==-1)
         {
           pressanykey("沒有此記錄檔案!");
         }
         else
         {
           id = searchuser(keyword);
           rec_get(pwdfile, &xuser, sizeof(userec), id);
           move(1,0);
           user_display(&xuser, 1);
           pressanykey(NULL);
           demoney(10);
         }
         
    }
  }   
  
  pressanykey("時光機關閉中...");    
  unlockutmpmode();
  return;
}
