#include "bbs.h"

char *betname[8] = {"疲倦野貓","沒力咩咩","遊魂小風","魔力小夢",
                    "勤奮大魚","碼頭阿倫","一堆螞蟻","嘰哩咕嚕"};
/* 黑傑克遊戲 */
int BlackJack()
{
  char buf[256];
  int    num[52]={11,11,11,11,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5,5,6,6,6,6,
                   7,7,7,7,8,8,8,8,9,9,9,9,10,10,10,10,10,10,10,10,
                  10,10,10,10,10,10,10,10};
  int cardlist[52]={0};
  int i,j,m,tmp=0,tmp2,ch;
  int win=2,win_jack=5; /* win 為贏時的倍率, win_jack 為前兩張就 21 點倍率 */
  int six=10, seven=20, aj=10, super_jack=20; /* 777, A+J, spade A+J 的倍率 */
  int host_count=2, guest_count=1, card_count=3, A_count=0, AA_count=0;
  int host_point=0, guest_point=0, mov_y=4;
  int host_card[12]={0}, guest_card[12]={0};
  long int money;

  int CHEAT=0; /* 做弊參數, 1 就作弊, 0 就不作 */

  time_t now = time(0);
  time(&now);

  clear();
  setutmpmode(BLACKJACK);
  do{
  move(0,0);prints("您身上還有 [1;44;33m%d[m 元",cuser.money);
  getdata(1, 0, "要押注多少錢(1 - 250000)? ", buf, 7, DOECHO,0);
  money=atoi(buf);
  if(!buf[0])return 0;
  if(check_money(money)) return 0;
  }while((money<1)||(money>250000));
  demoney(money);
  move(2,0);prints("(按 y 續牌, n 不續牌, d double)");
  move(0,0);clrtoeol();prints("您還有 [1;44;33m%d[m 銀兩",cuser.money);
  for(i=1;i<=52;i++){
    m=0;
    do{
    j=(time(0)+cuser.money+rand())%52;
    if (cardlist[j]==0){cardlist[j]=i;m=1;}
    }while(m==0);
  };
  for(i=0;i<52;i++)cardlist[i]--; /* 洗牌 */

  if(money>=20000)CHEAT=1;
  if(CHEAT==1){
    if(cardlist[1]<=3){
      tmp2=cardlist[50];
      cardlist[50]=cardlist[1];
      cardlist[1]=tmp2;
    }
  }                             /* 作弊碼 */

  host_card[0]=cardlist[0];
  if(host_card[0]<4)AA_count++;
  guest_card[0]=cardlist[1];

  if(guest_card[0]<4)A_count++;
  host_card[1]=cardlist[2];
  if(host_card[1]<4)AA_count++; /* 發前三張牌 */

  move(5,0);  prints("╭───╮");
  move(6,0);  prints("│      │");
  move(7,0);  prints("│      │");
  move(8,0);  prints("│      │");
  move(9,0);  prints("│      │");
  move(10,0); prints("│      │");
  move(11,0); prints("╰───╯");
  print_card(host_card[1],5,4);
  print_card(guest_card[0],15,0);  /* 印出前三張牌 */

  host_point=num[host_card[1]];
  guest_point=num[guest_card[0]];

  do{
    m=1;
    guest_card[guest_count]=cardlist[card_count];
    if(guest_card[guest_count]<4)A_count++;
    print_card(guest_card[guest_count],15,mov_y);
    guest_point+=num[guest_card[guest_count]];

    if((guest_card[0]>=24&&guest_card[0]<=27)&&(guest_card[1]>=24&&guest_card[1]<=27)&&(guest_card[2]>=24&&guest_card[2]<=27)){
      move(18,3);prints("[1;41;33m     ７７７     [m");
      move(3,0);prints("[1;41;33m７７７ !!! 得獎金 %d 銀兩[m",money*seven);
      inmoney(money*seven);
      move(0,0);clrtoeol();prints("您還有 [1;44;33m%d[m 銀兩",cuser.money);
      sprintf(buf,"[1;32m%12s[m 中了 [1;33m%7d[m 元的 "COLOR1"[1m  ７７７   [m %s ",cuser.userid,money*seven,Cdate(&now));
      f_cat("game/blackjack.dat",buf);
      pressanykey(NULL);
      return 0;
    }

    if((guest_card[0]==40&&guest_card[1]==0)||(guest_card[0]==0&&guest_card[1]==40)){
      move(18,3);prints("[1;41;33m 超級正統 BLACK JACK  [m");
      move(3,0);prints("[1;41;33m超級正統 BLACK JACK !!! 得獎金 %d 銀兩[m",money*super_jack);
      inmoney(money*super_jack);
      move(0,0);clrtoeol();prints("您還有 [1;44;33m%d[m 銀兩",cuser.money);
      sprintf(buf,"[1;32m%12s[m 中了 [1;33m%7d[m 元的 [1;41;33m 正統 ＡＪ [m %s ",cuser.userid,money*super_jack,Cdate(&now));
      f_cat("game/blackjack.dat",buf);
      pressanykey(NULL);
      return 0;
    }

    if((guest_card[0]<=3&&guest_card[0]>=0)&&(guest_card[1]<=43&&guest_card[1]>=40))tmp=1;

if((tmp==1)||((guest_card[1]<=3&&guest_card[1]>=0)&&(guest_card[0]<=43&&guest_card[0]>=40))){
      move(18,3);prints("[1;41;33m SUPER BLACK JACK  [m");
      move(3,0);prints("[1;41;33mSUPER BLACK JACK !!! 得獎金 %d 銀兩[m",money*aj);
      inmoney(money*aj);
      move(0,0);clrtoeol();prints("您還有 [1;44;33m%d[m 銀兩",cuser.money);
      sprintf(buf,"[1;32m%12s[m 中了 [1;33m%7d[m 元的 [1;44;33m SuperＡＪ [m %s ",cuser.userid,money*aj,Cdate(&now));
      f_cat("game/blackjack.dat",buf);
      pressanykey(NULL);
      return 0;
    }

    if(guest_point==21&&guest_count==1){
      move(18,3);prints("[1;41;33m  BLACK JACK  [m");
      move(3,0);prints("[1;41;33mBLACK JACK !!![44m 得獎金 %d 銀兩[m",money*win_jack);
      inmoney(money*win_jack);
      move(0,0);clrtoeol();prints("您還有 [1;44;33m%d[m 銀兩",cuser.money);
    if(money*win_jack>=500000){
      sprintf(buf,"[1;32m%12s[m 中了 [1;33m%7d[m 元的 [1;47;30m BlackJack [m %s ",cuser.userid,money*win_jack,Cdate(&now));
      f_cat("game/blackjack.dat",buf);
    }

      pressanykey(NULL);
      return 0;
    }                        /* 前兩張就 21 點 */

    if(guest_point>21){
      if(A_count>0){guest_point-=10;A_count--;};
    }
    move(12,0); clrtoeol();prints("[1;32m點數: [33m%d[m",host_point);
    move(14,0); clrtoeol();prints("[1;32m點數: [33m%d[m",guest_point);
    if(guest_point>21){
      pressanykey("  爆掉啦~~~  ");
      return 0;
    }

    if(guest_count==5){
      move(18,3);prints("[1;41;33m            過六關            [m");
      move(3,0);prints("[1;41;33m過六關 !!! 得獎金 %d 銀兩[m",money*six);
      inmoney(money*six);
      move(0,0);clrtoeol();prints("您還有 [1;44;33m%d[m 銀兩",cuser.money);
      sprintf(buf,"[1;32m%12s[m 中了 [1;33m%7d[m 元的 [1;44;33m  過六關   [m %s ",cuser.userid,money*six,Cdate(&now));
      f_cat("game/blackjack.dat",buf);
      pressanykey(NULL);
      return 0;
    }

    guest_count++;
    card_count++;
    mov_y+=4;

    do{
      if(ch=='d')m=0;
      if(m!=0)ch=igetkey();
    }while(ch!='y'&&ch!='n'&&ch!='d'&&m!=0); /* 抓 key */

    if(ch=='d'&&m!=0&&guest_count==2){
      if(cuser.money>=money){
        demoney(money);
        money*=2;
      }
      else ch='n';
      move(0,0);clrtoeol();prints("您還有 [1;44;33m%d[m 銀兩",cuser.money);
    }                                      /* double */

    if(ch=='d'&&guest_count>2)ch='n';
    if(guest_point==21)ch='n';
  }while(ch!='n'&&m!=0);

  mov_y=8;

  print_card(host_card[0],5,0);
  print_card(host_card[1],5,4);
  host_point+=num[host_card[0]];

  do{

    if(host_point<guest_point){
      host_card[host_count]=cardlist[card_count];
      print_card(host_card[host_count],5,mov_y);
      if(host_card[host_count]<4)AA_count++;
      host_point+=num[host_card[host_count]];
    }
    if(host_point>21){
      if(AA_count>0){host_point-=10;AA_count--;};
    }
    move(12,0); clrtoeol();prints("[1;32m點數: [33m%d[m",host_point);
    move(14,0); clrtoeol();prints("[1;32m點數: [33m%d[m",guest_point);
    if(host_point>21){
      move(14,0); clrtoeol(); prints("[1;32m點數: [33m%d [1;41;33m WINNER [m",guest_point);

      move(3,0);prints("[1;44;33m你贏了~~~~ 得獎金 %d 銀兩[m",money*win);
      inmoney(money*win);
      move(0,0);clrtoeol();prints("您還有 [1;44;33m%d[m 銀兩",cuser.money);
      pressanykey(NULL);
      return 0;
    }
    host_count++;
    card_count++;
    mov_y+=4;
  }while(host_point<guest_point);

  pressanykey("你輸了~~~~ 銀兩沒收!");

return 0;
}


int print_card(int card,int x,int y)
{
  char *flower[4]={"Ｓ","Ｈ","Ｄ","Ｃ"};
  char *poker[52]={"Ａ","Ａ","Ａ","Ａ","２","２","２","２","３","３","３","３",
                   "４","４","４","４","５","５","５","５","６","６","６","６",
                   "７","７","７","７","８","８","８","８","９","９","９","９",
                   "10","10","10","10","Ｊ","Ｊ","Ｊ","Ｊ","Ｑ","Ｑ","Ｑ","Ｑ",
                   "Ｋ","Ｋ","Ｋ","Ｋ"};

move(x,y);   prints("╭───╮");
move(x+1,y); prints("│%s    │",poker[card]);
move(x+2,y); prints("│%s    │",flower[card%4]);
move(x+3,y); prints("│      │");
move(x+4,y); prints("│      │");
move(x+5,y); prints("│      │");
move(x+6,y); prints("╰───╯");
return 0;
}

int
gagb()
{
  int tmoney,money;
  char genbuf[200],buf[80];
  char ans[5]="";
  /* 倍率        0  1   2   3   4   5   6   7   8   9   10 */
  float bet[11]={0,100, 50, 10, 3 ,1.5,1.2,0.9, 0.8 ,0.5,0.1};
  int a,b,c,count;  
  FILE *fp;
  time_t now;
  
  setutmpmode(XAXB);
  srand(time(0));
  clear();
  showtitle("猜數字！", BoardName);
  move(1,0);
  prints("[1m現在你身上共有 [33m%d [37m元。[m",cuser.money);
  move(6,0);
  prints("[36m%s[m",msg_seperator);
  move(17,0);
  prints("[36m%s[m",msg_seperator);
  fp = fopen("game/money", "r");
  fscanf(fp, "%d", &tmoney);
  fclose(fp);

  move(20,0);
  prints("現在遊樂場內共有 %d  元",tmoney);

  getdata(3, 0, "請問你要下注多少(1 ~ 10000)？ ", genbuf, 6, 1, 0);
  money=atoi(genbuf);
  if(money<=0||money>cuser.money||money>10000) 
    return 0;
  demoney(money); 

  move(1,15); 
  prints("%d",cuser.money);
  tmoney+=money; 

  move(20,17); 
  prints("%d",tmoney);

  do{
    itoa(rand()%10000,ans);
    for (a=0;a<3;a++)
    for (b=a+1;b<4;b++)
    if(ans[a]==ans[b]) ans[0]=0;
  }while (!ans[0]);

  for(count=1;count<11;count++)
  {
    do{
      getdata(5,0,"請猜 → ",genbuf,5,1,0);
      if(!strcmp(genbuf,"貓老大"))
      {
        prints("%s",ans);
        igetch();
      }
      c=atoi(genbuf);
      itoa(c,genbuf);
      for (a=0;a<3;a++)
      for (b=a+1;b<4;b++)
      if(genbuf[a]==genbuf[b]) 
        genbuf[0]=0;
      if(!genbuf[0])
      {
        pressanykey("輸入數字有問題!!");
      }
    }while (!genbuf[0]);
    move(count+6,0);
    prints("  [1;31m第 [37m%2d [31m次： [37m%s  ->  [33m%dA [36m%dB [m",count,genbuf,an(genbuf,ans),bn(genbuf,ans));
    if(an(genbuf,ans)==4) break;
  }
  now=time(0);

  fp=fopen("log/gagb.log","a+");
  sprintf(genbuf, "%s %-13s ", Cdate(&now),cuser.userid);
  if(count > 10)
  {
    sprintf(buf,"你輸了呦！正確答案是 %s，下次再加油吧!!",ans);
    fprintf(fp,"[1;31m%s 可憐沒猜到，輸了 %d 元！[m\n", genbuf, money);
  }
  else
  {
  int oldmoney=money;
    money *= bet[count];
    inmoney(money);
    tmoney-=money;
    if(money-oldmoney > 0)
      sprintf(buf,"恭喜！總共猜了 %d 次，淨賺獎金 %d 元",count,money-oldmoney);
    if(money-oldmoney == 0)
      sprintf(buf,"唉∼∼總共猜了 %d 次，沒輸沒贏！",count);
    if(money-oldmoney < 0)
      sprintf(buf,"啊∼∼總共猜了 %d 次，賠錢 %d 元！",count,oldmoney-money);
    move(1,15); 
    prints("%d",cuser.money);
    move(20,17); 
    prints("%d",tmoney);
    if(money-oldmoney > 0)
      fprintf(fp,"[1;33m%s 猜了 %d 次，淨賺 %d 元！[m\n", genbuf,count,money-oldmoney);
    if(money-oldmoney < 0)
      fprintf(fp,"[1;31m%s 猜了 %d 次，虧了 %d 元！[m\n", genbuf,count,oldmoney-money);
  }
  fclose(fp);

  fp = fopen("game/money", "w");
  fprintf(fp, "%d", tmoney);
  fclose(fp);

  pressanykey(buf);
  return 0;
}

itoa(i,a)
int i;
char *a;
{
 int j,k,l=1000;
// prints("itoa: i=%d ",i);

 for(j=3;j>0;j--)
 {
  k=i-(i%l);
  i-=k;
  k=k/l+48;
  a[3-j]=k;
  l/=10;
 }
 a[3]=i+48;
 a[4]=0;

// prints(" a=%s\n",a);
// igetch();
}

int
an(a,b)
char *a,*b;
{
 int i,k=0;
 for(i=0;i<4;i++)
  if(*(a+i)==*(b+i)) k++;
 return k;
}

int
bn(a,b)
char *a,*b;
{
 int i,j,k=0;
 for(i=0;i<4;i++)
  for(j=0;j<4;j++)
   if(*(a+i)==*(b+j)) k++;
 return (k-an(a,b));
}

/*
  程式設計：wsyfish
  自己評語：寫得很爛，亂寫一通，沒啥深度:)
  相容程度：Ptt板本應該都行吧，就用inmoney和demoney，其他如Sob就要改一下囉
  附帶一提：這程式裡面有耍賤，期望值不是 0
*/
char *dice[6][3] = {"        ",
                    "   ●   ",
                    "        ",
                    "   ●   ",
                    "        ",
                    "   ●   ",
                    "   ●   ",
                    "   ●   ",
                    "   ●   ",
                    "●    ●",
                    "        ",
                    "●    ●",
                    "●    ●",
                    "   ●   ",
                    "●    ●",
                    "●    ●",
                    "●    ●",
                    "●    ●"
};

int
x_dice()
{
  char choice[11],buf[60],buf2[80];
  int  i, money; 
  char tmpchar;    /* 紀錄選項 */
  char tmpdice[3]; /* 三個骰子的值 */
  char totaldice;
  time_t now = time(0);

  time(&now);

  setutmpmode(DICE);
  while(1)
  {
    stand_title("ㄒㄧ ㄅㄚ ㄌㄚ 賭場");
    getdata(1, 0, "請問要下注多少呢呢？(最多 250000 元) ",
            choice, 7, LCECHO, 0);
    money = atoi(choice);
    if(money <= 0 || money > 250000 || money > cuser.money)
    {
      pressanykey("下注金額輸入有誤！離開賭場");
      break;
    }
    outs("\n┌────────────────────────────────────┐\n"
         "│ ２倍  1. 大      2. 小                                                 │\n"
         "│ ５倍  3. 三點    4. 四點     5. 五點    6. 六點    7. 七點    8. 八點  │\n"
         "│       9. 九點   10. 十點    11. 十一點 12. 十二點 13. 十三點 14. 十四點│\n"
         "│      15. 十五點 16. 十六點  17. 十七點 18. 十八點                      │\n"
         "│ ９倍 19. 一一一 20. 二二二  21. 三三三 22. 四四四 23. 五五五 24. 六六六│\n"
         "└────────────────────────────────────┘\n");
    getdata(11, 0, "要押哪一項呢？(請輸入號碼) ", choice, 3, LCECHO, 0);
    tmpchar = atoi(choice);
    if(tmpchar <= 0 || tmpchar > 24)
    {
      pressanykey("要押的項目輸入有誤！離開賭場");
      break;
    }
    demoney(money);
    outs("\n按任一鍵擲出骰子....\n");
    igetkey();

    do
    {
      totaldice = 0;
      for(i = 0; i < 3; i++)
      {
        tmpdice[i] = rand() % 6 + 1;
        totaldice += tmpdice[i];
      }

      if (((tmpchar == 1) && totaldice > 10) ||
          ((tmpchar == 2) && totaldice <= 10))
      {
        if ((rand() % 10) < 7) /* 作弊用，中獎率為原來之70% */
          break;
      }
      else
        break;

    }while(tmpchar <= 2);

    outs("\n╭────╮╭────╮╭────╮\n");

    for(i = 0; i < 3; i++)
      prints("│%s││%s││%s│\n",
             dice[tmpdice[0] - 1][i],
             dice[tmpdice[1] - 1][i],
             dice[tmpdice[2] - 1][i]);

    outs("╰────╯╰────╯╰────╯\n\n");

    if((tmpchar == 1 && totaldice > 10)
       || (tmpchar == 2 && totaldice <= 10)) /* 處理大小 */
      sprintf(buf,"中了！得到２倍獎金 %d 元，總共有 %d 元",
              money * 2, inmoney(money * 2));
    else if(tmpchar <= 18 && totaldice == tmpchar) /* 處理總和 */
      sprintf(buf,"中了！得到５倍獎金 %d 元，總共有 %d 元",
              money * 5, inmoney(money * 5));
    else if((tmpchar - 18) == tmpdice[0] && (tmpdice[0] == tmpdice[1])
            && (tmpdice[1] == tmpdice[2])) /* 處理三個一樣總和 */
      sprintf(buf,"中了！得到９倍獎金 %d 元，總共有 %d 元",
            money * 9, inmoney(money * 9));

    else /* 處理沒中 */
      sprintf(buf,"很可惜沒有押中！");
    pressanykey(buf);
    sprintf(buf2,"%s %s %s",cuser.userid,buf,Cdate(&now));
    f_cat("log/dice.log",buf2);
  }
  return 0;
 }

/* ----------------------------------- */
/* gamble.c 原作者: Ptt 改寫者: yychen */
/*                                     */
/* ----------------------------------- */
show_bet()
{
  FILE *fp = fopen(FN_TICKET_RECORD,"r");
  int i, total=0, ticket[8] = {0,0,0,0,0,0,0,0};

  if(fp)
        {
          fscanf(fp,"%9d %9d %9d %9d %9d %9d %9d %9d\n",
                 &ticket[0],&ticket[1],&ticket[2],&ticket[3],
                 &ticket[4],&ticket[5],&ticket[6],&ticket[7]);
          for(i = 0; i < 8; i++)
                total += ticket[i];
          fclose(fp);
        }
  prints("[1;33m1.%-8s:%-9d2.%-8s:%-9d3.%-8s:%-9d4.%-8s:%-9d[m\n"
         "[1;33m5.%-8s:%-9d6.%-8s:%-9d7.%-8s:%-9d8.%-8s:%-9d[m\n"
         "[1;37;44m 下注所有金額: [33m%d00[36m 金 [m",
         betname[0], ticket[0], betname[1], ticket[1],
         betname[2], ticket[2], betname[3], ticket[3],
         betname[4], ticket[4], betname[5], ticket[5],
         betname[6], ticket[6], betname[7], ticket[7], total);
}


show_ticket_data()
{
 clear();
 showtitle("對對樂賭盤", BoardName);
 move(2,0);
 outs(
        "[1;32m規則:[m 1.可購買八種不同類型的彩票。每張要花100元。\n"
        "      2.每十二小時開獎一次(0:30 12:30)。\n"
        "      3.開獎時會抽出一種彩票, 有購買該彩票者, 則可依購買的張數均分總賭金。\n"
        "      4.每筆獎金由系統抽取5%之稅金。\n\n"
        "[1;32m前幾次開獎結果:[m" );
 show_file(FN_TICKET, 8, 8,NO_RELOAD);
 move(15,0);
 outs("[1;36m目前下注狀況:[m\n");
 show_bet();
 move(20,0);
 prints(COLOR1"[1m你身上有： %-10d 元 [m           [m\n",
   cuser.money);
 prints("[1m請選擇要購買的種類(1~8)[Q:離開]:[m");
}

int
append_ticket_record(ch,n)
{
  FILE *fp;
  int ticket[8] = {0,0,0,0,0,0,0,0};
  if(fp = fopen(FN_TICKET_USER,"a"))
        {
          fprintf(fp,"%s %d %d\n",cuser.userid,ch,n);
          fclose(fp);
        }

  if(fp = fopen(FN_TICKET_RECORD,"r+"))
        {
          fscanf(fp,"%9d %9d %9d %9d %9d %9d %9d %9d\n",
                    &ticket[0],&ticket[1],&ticket[2],&ticket[3],
                    &ticket[4],&ticket[5],&ticket[6],&ticket[7]);
          ticket[ch] += n;
          rewind(fp);
          fprintf(fp,"%9d %9d %9d %9d %9d %9d %9d %9d\n",
                    ticket[0],ticket[1],ticket[2],ticket[3],
                    ticket[4],ticket[5],ticket[6],ticket[7]);
          fclose(fp);
        }
  else if(fp = fopen(FN_TICKET_RECORD,"w"))
        {
          ticket[ch] += n;
          fprintf(fp,"%9d %9d %9d %9d %9d %9d %9d %9d\n",
                    ticket[0],ticket[1],ticket[2],ticket[3],
                    ticket[4],ticket[5],ticket[6],ticket[7]);
          fclose(fp);
        }
}

void
show_picture(char *filename)
{
 FILE *fp;
 char buf[256];
 move(5,0);
 if((fp=fopen(filename,"r")))
  {
    while(fgets(buf,256,fp))
        {
          outs(buf);
        }
    fclose(fp);
  }
}
void
clear_picture()
{
 char i;
 for (i=5;i<18;i++)
  {
        move(i,0);
        clrtoeol();
  }
}

void
ch_buyitem(money,picture,item)
int money;
char *picture;
int *item;
{
  int num=0;
  char buf[5];
  getdata(b_lines-1,0,"要買多少份呢:",buf,4,DOECHO,0);
  num = atoi(buf);
  if(num<0) num=0;
  if (cuser.money > money*num)
        {
          *item += num;
          demoney(money*num);
          clear_picture();
          show_picture(picture);
        }
  else
        {
          move(b_lines-1,0);
          clrtoeol();
          outs("現金不夠 !!!");
        }
  pressanykey(NULL);
}


int
ticket_main()
{
 int ch,n;

 if (lockutmpmode(TICKET))  return 0;

 setutmpmode(TICKET);
 while(1)
  {
   show_ticket_data();
   ch = igetch();
   if(ch=='q' || ch == 'Q') break;
   ch -= '1';
   if(ch > 7 || ch < 0) continue;
   n = 0;
   ch_buyitem(100,"etc/buyticket",&n);
   if(n>0)  append_ticket_record(ch,n);
  }
 unlockutmpmode();
 return 0;
}

int
query_ticket()
{
  FILE *fp;
  int tickets[8] = {0,0,0,0,0,0,0,0};
  int num1, num2, i=0;
  char userid[IDLEN+1];

  if (fp = fopen(FN_TICKET_USER, "r"))
  {
      while ((fscanf(fp, "%s %d %d", userid, &num1, &num2))!=EOF)
      {
        if (!strcasecmp(userid, cuser.userid))
        {
          if (!i)
            i = 1;
          tickets[num1] += num2;
        }
      }
      fclose(fp);
  }
  else
  {
    pressanykey("櫃台小姐: 並沒有任何一個人(包括您)押注.");
    return;
  }
  if (!i)
  {
    pressanykey("櫃台小姐: 抱歉，您並沒有押任何一項！");
    return;
  }
  clear();
  prints("[1;33m櫃台小姐給您一張清單：[m\n");
  prints("[32m森林賭盤：[1;32m%s[0;32m 下注一覽表[m\n\n",
    cuser.userid);
  for(i = 0; i < 8; i++)
  {
    prints("[1;3%dm您(%s) 押了 [%d. %-4s]：%d 張[m\n", 
      i+1 <= 7 ? i+1 : i-6,
      cuser.userid, i+1, betname[i],
      tickets[i]);
  }
  pressanykey("櫃台小姐: 以上是您的押注狀況。");
}

#define PICTURE BBSHOME"/game/marie"
#define GMONEY BBSHOME"/game/money"

unsigned long int totalmoney;

int mary_m()
{
  FILE *fs;
  unsigned int total=0;

  fs = fopen(GMONEY,"r");
  fscanf(fs,"%d",&totalmoney);
  fclose(fs);
  
  total=cuser.money + cuser.deposit;
  setutmpmode(MARIE);
  clear();
  show_file(PICTURE,0,24,ONLY_COLOR);
  totalmoney = show_m(totalmoney);
  
  fs = fopen(GMONEY,"w");
  fprintf(fs,"%d",totalmoney);
  fclose(fs);
  
  return;
}

int show_m(totalmoney)
{
   static int x[9]={0,0,0,0,0,0,0,0,0},w=0,bet_flag=0;
   int c_flag[10]={1,5,10,20,100,1000,10000,100000,1000000,10000000}; /* 倍率 */
   int i; 
   int ch;
   clear();
   show_file(PICTURE,0,24,ONLY_COLOR);
   move(8,44);
   prints("[1m本機臺內有現金: %-ld[m",totalmoney);
   move(9,44);
   prints("[1m您身上現有銀兩: %-d圓[m",cuser.money);
   move(10,44);
   prints("[1m目前押注的倍率是 [46m%6d倍[m",c_flag[w]);
   move(b_lines-4,0);

   for(i=0;i<9;i++)
      prints("  %5d",x[i]);

   ch=igetkey();
   switch(ch)
   {
   case 'w':
      w=(w+1)%10;
      break;
   case 's':
      if (bet_flag)
      {
         int i,j,seed;
         int g[10]={5,40,30,25,50,20,15,10,2,0};
         int gold=0;
         int ran;
         time_t now = time(0);
         time(&now);
         seed=1000;
         ran=rand()%seed;

         if(ran<=400) /* 2 */
            j=8;
         else if(ran<=520) /* 5 */
            j=0;
         else if(ran<=580) /* 10 */
            j=7;
         else if(ran<=620) /* 15 */
            j=6;
         else if(ran<=650) /* 20 */
            j=5;
         else if(ran<=663) /* 25 */
            j=3;
         else if(ran<=678) /* 30 */
            j=2;
         else if(ran<=686) /* 40 */
            j=1;
         else if(ran<=690) /* 50 */
            j=4;
         else
         {
            move(11,44);
            clrtoeol();
            prints("[1;31mSorry[m:~~~~[1;36m銘謝惠顧[m");
            j=9;
         }

         gold=x[j]*g[j];

         move(14,0);
         clrtoeol();

         for(i=0;i<9+j;i++)
         {
            move(14,5+14*((i+9)%9));
            prints("[0;33m●");
            if(j!=9)
            {
              move(14,5+14*((i+10)%9) );
              prints("[1;31m●");
            }
         }
         if(j!=9)
         {
            move(11,44);
            clrtoeol();
            prints("[32;40m你可得[33;41m %d [32;40m銀兩[m",gold);
            inmoney(gold);
            totalmoney-=gold;
         }
         for(i=0;i<9;i++)
            x[i]=0;
         bet_flag=0;
         pressanykey(NULL);
         break;
      }
      else
         pressanykey("請先押注!");
      break;

   case 'a':
      if(check_money(9*c_flag[w]))
        clear();
      else
      {
         demoney(9*c_flag[w]);
         totalmoney += 9*c_flag[w];
         for(i=0;i<=8;i++)
            x[i]+=c_flag[w];
         bet_flag=1;
      }
      break;
   case 'q':
      if(bet_flag)
         pressanykey("您已經下注, 請先玩完此局.");
      else
         return totalmoney;
      break;
   case '1':
   case '2':
   case '3':
   case '4':
   case '5':
   case '6':
   case '7':
   case '8':
   case '9':
      i=(ch-'0');
      if(check_money(c_flag[w]))
         clear();
      else
      {
         demoney(c_flag[w]);
         totalmoney += c_flag[w];
         bet_flag=1;
         x[i-1]+=c_flag[w];
      }
      break;
   default:
     break;
   }
   show_file(PICTURE,0,24,ONLY_COLOR);
   show_m(totalmoney);
}

#define MAX_SCORE       20                     
struct tetrisrec          /*俄羅斯方塊*/       
{                                              
char userid[IDLEN + 1];                      
time_t playtime;                           
char lasthost[16];                       
int u_score;                           
int u_level;                         
int u_rkill;                       
};                                 
typedef struct tetrisrec tetrisrec;

x_tetris()
{
  char buf[64];
  int save_pager;
  clear();
  counter(BBSHOME"/counter/俄羅斯方塊","玩俄羅斯方塊");
  pressanykey(NULL);
  save_pager = currutmp->pager;
  currutmp->pager = 2;
  setutmpmode(TETRIS);
  restore_tty();
  sprintf(buf, BBSHOME"/bin/tetris.sh");
  do_exec(buf, NULL);
  restore_tty();
  refresh();
  game_score(0);
  currutmp->pager = save_pager;
  return 0;
}

int
tetris_cmp(b, a)
struct tetrisrec *a, *b;
{
  return (a->u_score - b->u_score);
}


game_score(swi)
int swi;
{

  FILE *fp;
  int score=0,level=0,rkill=0;
  int i,j;
  struct tetrisrec alltetris[MAX_SCORE+1];

  switch(swi){

    case 0:
       if(fp=fopen(BBSHOME"/etc/tetris.tmp","r"))
       {
          fscanf(fp,"%d %d %d ",&score,&level,&rkill);
          fclose(fp);
       }
       unlink(BBSHOME"/etc/tetris.tmp");
       break;

  }


  switch(swi){
     char genbuf[100];

     case 0:
             if((fp=fopen(BBSHOME"/etc/tetris.score","r+")) == NULL )
               fp=fopen(BBSHOME"/etc/tetris.score","w");
             for(i=0;i<MAX_SCORE;i++)
                if(fread(&alltetris[i],sizeof(struct tetrisrec),1,fp) == 0)
                    break;

             strcpy(alltetris[i].userid, cuser.userid);
             alltetris[i].playtime = time(0);
             strcpy(alltetris[i].lasthost, cuser.lasthost);
             alltetris[i].u_score = score;
             alltetris[i].u_level = level;
             alltetris[i].u_rkill = rkill;

             qsort(alltetris, i+1, sizeof(struct tetrisrec), tetris_cmp);
             rewind(fp);
             for(j=0;j<i+1 && j<MAX_SCORE;j++)
               fwrite(&alltetris[j],sizeof(struct tetrisrec),1,fp);
             fclose(fp);
             clear();
             prints("yours final score: %d  level: %d  row: %d\n",score,level,rkill);
             prints("%-12s%-9.9s %-10.10s %-5.5s%-17.16s%s\n",
             "    使用者","    Score","    level","row","from","時間");
            for(j=0;j<i+1 && j<MAX_SCORE;j++)
             {
               sprintf(genbuf,"%-12s%-9.9d %-3.3d    %-4.4d %-17.16s%s",alltetris[j].userid,
               alltetris[j].u_score,alltetris[j].u_level,alltetris[j].u_rkill,
               alltetris[j].lasthost,ctime(&alltetris[j].playtime));
               prints("%02d. %s",j+1,genbuf);
             }
             pressanykey(NULL);
             break;


  }
  return 0;
}

/* write by dsyan               */
/* 87/10/24                     */
/* 天長地久 Forever.twbbs.org   */

#include "bbs.h"

#undef  GP_DEBUG

char card[52],mycard[5],cpucard[5],sty[100],now;

show_money(int m)
{
  move(19,0);
  clrtoeol();
  prints("[1;37;44m你現有現金: [36m%-18d[37m押注金額: [36m%-20d[m",
  cuser.money,m);
}

static int
forq(a,b)
char *a,*b;
{
 char c=(*a)%13;
 char d=(*b)%13;
 if(!c) c=13; if(!d) d=13;
 if(c==1) c=14; if(d==1) d=14;
 if(c==d) return *a-*b;
 return c-d;
}

int
p_gp()
{
 char genbuf[200],hold[5];
 char ans[5];
 time_t n;
 int bet,i,j,k,tmp,x,xx,doub,gw=0,cont=0;

 srandom(time(0));
 n = time(0);
 more("etc/game/gp.welcome",YEA);

 setutmpmode(GP);bet=0;
 while(-1)
 {
  clear();
  showtitle("金撲克梭哈", BoardName);
  ans[0]=0;
  show_money(bet);
  if(cont==0)
  do
  {
   getdata(21, 0,"要下注多少呢(上限1000)? 按 Enter 離開>", genbuf, 5, 1, 0);
   bet=atoi(genbuf);
   if(bet<0) bet=0;
  } while(bet>1000);
  if(!bet) return 0;
  if(check_money(bet)) return 0;
  move(21,0); clrtoeol();
  if(cont>0)
  prints("[33;1m (←)(→)改變選牌  (SPCAE)改變換牌  (Enter)確定[m");
  else
  prints("[33;1m (←)(→)改變選牌  (d)Double  (SPCAE)改變換牌  (Enter)確定[m");
  demoney(bet);
  show_money(bet);

  for(i=0;i<52;i++) card[i]=i; /* 0~51 ..黑傑克是 1~52 */

  for(i=0;i<1000;i++)
  {
   j=random()%52; k=random()%52;
   tmp=card[j]; card[j]=card[k]; card[k]=tmp;
  }

  now=doub=0;
  for(i=0;i<5;i++)
  {
   mycard[i]=card[now++];
   hold[i]=1;
  }
  qsort(mycard,5,sizeof(char),forq);

  for(i=0;i<5;i++)
   show_card(0,mycard[i],i);

  x=xx=tmp=0;
  while(tmp!=13)
  {
   for(i=0;i<5;i++)
   {
    move(16, i*4+2); outs(hold[i]<0?"保":"  ");
    move(17, i*4+2); outs(hold[i]<0?"留":"  ");
   }
   move(11,xx*4+2); outs("  ");
   move(11,x*4+2); outs("↓");
   move(b_lines-1,0);
   refresh();
   xx=x;

   tmp=igetkey();
   switch(tmp)
   {
#ifdef GP_DEBUG
    case KEY_UP:
     getdata(21, 0,"把牌換成> ", genbuf, 3, 1, 0);
     mycard[x]=atoi(genbuf);
     qsort(mycard,5,sizeof(char),forq);
     for(i=0;i<5;i++)
      show_card(0,mycard[i],i);
     break;
#endif
    case KEY_LEFT: case 'l': x=x?x-1:4; break;
    case KEY_RIGHT: case 'r': x=(x==4)?0:x+1; break;
    case ' ': hold[x]*=-1; break;
    case 'd':
     if(!cont&&!doub&&cuser.money>=bet)
     {
      doub++;
      move(21,0); clrtoeol();
      prints("[33;1m (←)(→)改變選牌  (SPCAE)改變換牌  (Enter)確定[m");
      demoney(bet);
      bet*=2;
      show_money(bet);
     }
     break;
   }
  }

  for(i=0;i<5;i++)
   if(hold[i]==1)
    mycard[i]=card[now++];
  qsort(mycard,5,sizeof(char),forq);
  for(i=0;i<5;i++)
   show_card(0,mycard[i],i);
  move(11,x*4+2); outs("  ");

  cpu();
#ifdef GP_DEBUG
for(x=0;x<5;x++)
{
     getdata(21, 0,"把牌換成> ", genbuf, 3, 1, 0);
     cpucard[x]=atoi(genbuf);
}
     qsort(cpucard,5,sizeof(char),forq);
     for(i=0;i<5;i++)
      show_card(1,cpucard[i],i);
#endif
  i=gp_win();

  if(i<0)
  {
   inmoney(bet*2);
   sprintf(genbuf,"哇!!好棒喔!!淨賺 %d 元咧.. :DDD",bet);
   pressanykey( genbuf );
   if (cont>0)
   sprintf(genbuf,"%s ,%s 連勝 %d 次, 贏了 %d 元",
        Cdate(&n),cuser.userid,cont+1,bet);
   else
   sprintf(genbuf,"%s ,%s 贏了 %d 元",Cdate(&now),cuser.userid,bet);
   f_cat("log/gp.log",genbuf);
   bet = ( bet > 50000 ? 100000 : bet * 2 );gw=1;
  }
  else if(i>1000)
  {
   switch(i)
   {
    case 1001: doub=15; break;
    case 1002: doub=10; break;
    case 1003: doub=5; break;
   }
   inmoney(bet*2*doub);
   sprintf(genbuf,"哇!!好棒喔!!淨賺 %d 元咧.. :DDD",bet*2*doub-bet);
   pressanykey( genbuf );
   if(cont>0)
   sprintf(genbuf,"%s ,%s 連勝 %d 次, 贏了 %d 元",
        sty,cuser.userid,cont+1,bet*doub);
   else
   sprintf(genbuf,"%s ,%s 贏了 %d 元",Cdate(&now),cuser.userid,bet*doub);
   f_cat("log/gp.log",genbuf);
   bet = ( bet > 50000 ? 100000 : bet * 2 * doub ); gw=1;
   bet = ( bet >= 100000 ? 100000 : bet );
  }
  else
  {
   pressanykey("輸了..:~~~");
   if(cont>1)
   sprintf(genbuf,"%s ,%s 中止 %d 連勝, 輸了 %d 元",
        sty,cuser.userid,cont,bet);
   else
   sprintf(genbuf,"%s ,%s 輸了 %d 元",Cdate(&now),cuser.userid,bet);
   f_cat("log/gp.log",genbuf);
   cont=0;bet=0;
  }

  if ( gw==1 )
  {
    gw=0;
    getdata(21, 0,"您要把獎金繼續壓注嗎 (y/n)?", ans, 2, 1, 0);
    if ( ans[0]=='y' || ans[0]=='Y' )
       cont++;
    else
    {
       cont=0;bet=0;
    }
  }
 }
}

show_card(isDealer,c,x)
int isDealer,c,x;
{
  int beginL;
  char *suit[4] = {"Ｃ","Ｄ","Ｈ","Ｓ"};
  char *num[13] = {"Ｋ","Ａ","２","３","４","５","６",
                   "７","８","９","10","Ｊ","Ｑ"};

  beginL=(isDealer)? 2: 12;
  move(beginL, x*4);      outs("╭───╮");
  move(beginL+1, x*4);  prints("│%2s    │", num[c%13]);
  move(beginL+2, x*4);  prints("│%2s    │", suit[c/13]); /* ←這裡跟黑傑克 */
#ifdef GP_DEBUG
  move(beginL+3, x*4);  prints("│%2d    │",c);              /*   有點不同喔!! */
#else
  move(beginL+3, x*4);    outs("│      │");              /*   有點不同喔!! */
#endif
  move(beginL+4, x*4);    outs("│      │");
  move(beginL+5, x*4);    outs("│      │");
  move(beginL+6, x*4);    outs("╰───╯");
}

cpu()
{
 char i,j,hold[5];
 char p[13],q[5],r[4];
 char a[5],b[5];

 for(i=0;i<5;i++)
 {
  cpucard[i]=card[now++];
  hold[i]=0;
 }
 qsort(cpucard,5,sizeof(char),forq);
 for(i=0;i<5;i++)
  show_card(1,cpucard[i],i);

 tran(a,b,cpucard);
 check(p,q,r,cpucard);

 for(i=0;i<13;i++)
  if(p[i]>1)
   for(j=0;j<5;j++)
    if(i==cpucard[j]%13) hold[j]=-1;

 for(i=0;i<5;i++)
 {
  if(a[i]==13 || a[i]==1) hold[i]=-1;
  move(6, i*4+2); outs(hold[i]<0?"保":"  ");
  move(7, i*4+2); outs(hold[i]<0?"留":"  ");
 }

 pressanykey("電腦換牌前..");

 for(i=0;i<5;i++)
  if(!hold[i]) cpucard[i]=card[now++];
 qsort(cpucard,5,sizeof(char),forq);
 for(i=0;i<5;i++)
  show_card(1,cpucard[i],i);
}

int
gp_win()
{
 int my,cpu,ret;
 char myx,myy,cpux,cpuy;

 my=complex(mycard,&myx,&myy);
 cpu=complex(cpucard,&cpux,&cpuy);
 show_style(my,cpu);

 if(my!=cpu)
  ret=my-cpu;
 else if (myx==1 && cpux!=1)
  ret=-1;
 else if (myx!=1 && cpux==1)
  ret=1;
 else if(myx!=cpux)
  ret=cpux-myx;
 else if (myy!=cpuy)
  ret=cpuy-myy;

 if(ret<0)
  switch(my)
  {
   case 1: ret=1001; break;
   case 2: ret=1002; break;
   case 3: ret=1003; break;
  }

 return ret;
}

// 同花順、鐵枝、葫、同花、順、三條、兔胚、胚、一隻
int
complex(cc,x,y)
char *cc,*x,*y;
{
 char p[13],q[5],r[4];
 char a[5],b[5],c[5],d[5];
 int i,j,k;

 tran(a,b,cc);
 check(p,q,r,cc);

/* 同花順 */
 if((a[0]==a[1]-1 && a[1]==a[2]-1 && a[2]==a[3]-1 && a[3]==a[4]-1) &&
    (b[0]==b[1] && b[1]==b[2] && b[2]==b[3] &&b[3]==b[4]))
 { *x=a[4]; *y=b[4]; return 1;}

 if(a[4]==1 && a[0]==2 && a[1]==3 && a[2]==4 && a[3]==5 &&
    (b[0]==b[1] && b[1]==b[2] && b[2]==b[3] && b[3]==b[4]))
 { *x=a[3]; *y=b[4]; return 1;}

 if(a[4]==1 && a[0]==10 && a[1]==11 && a[2]==12 && a[3]==13 &&
    (b[0]==b[1] && b[1]==b[2] && b[2]==b[3] && b[3]==b[4]))
 { *x=1; *y=b[4]; return 1;}

/* 四張 */
 if(q[4]==1)
 {
   for(i=0;i<13;i++) if(p[i]==4) *x=i?i:13;
   return 2;
 }

/* 葫蘆 */
 if(q[3]==1&&q[2]==1)
 {
  for(i=0;i<13;i++) if(p[i]==3) *x=i?i:13;
  return 3;
 }

/* 同花 */
 for(i=0;i<4;i++) if(r[i]==5) {*x=i; return 4;}

/* 順子 */
 memcpy(c,a,5); memcpy(d,b,5);
 for(i=0;i<4;i++)
  for(j=i;j<5;j++)
  if(c[i]>c[j])
  {
   k=c[i]; c[i]=c[j]; c[j]=k;
   k=d[i]; d[i]=d[j]; d[j]=k;
  }

 if(10==c[1] && c[1]==c[2]-1 && c[2]==c[3]-1 && c[3]==c[4]-1 && c[0]==1)
 { *x=1; *y=d[0]; return 5;}

 if(c[0]==c[1]-1 && c[1]==c[2]-1 && c[2]==c[3]-1 && c[3]==c[4]-1)
 { *x=c[4]; *y=d[4]; return 5;}

/* 三條 */
 if(q[3]==1)
  for(i=0;i<13;i++)
   if(p[i]==3) { *x=i?i:13; return 6;}

/* 兔胚 */
 if(q[2]==2)
 {
  for(*x=0,i=0;i<13;i++)
   if(p[i]==2)
   {
    if((i>1?i:i+13)>(*x==1?14:*x))
    {
     *x=i?i:13; *y=0;
     for(j=0;j<5;j++) if(a[j]==i&&b[j]>*y) *y=b[j];
    }
   }
  return 7;
 }

/* 胚 */
 if(q[2]==1)
  for(i=0;i<13;i++)
   if(p[i]==2)
   {
    *x=i?i:13; *y=0;
    for(j=0;j<5;j++) if(a[j]==i&&b[j]>*y) *y=b[j];
    return 8;
   }

/* 一張 */
 *x=0; *y=0;
 for(i=0;i<5;i++)
  if((a[i]=a[i]?a[i]:13>*x||a[i]==1)&&*x!=1)
  { *x=a[i]; *y=b[i];}

 return 9;
}

/* a 是點數 .. b 是花色 */
tran(a,b,c)
char *a,*b,*c;
{
 int i;
 for(i=0;i<5;i++)
 {
  a[i]=c[i]%13;
  if(!a[i]) a[i]=13;
 }

 for(i=0;i<5;i++)
  b[i]=c[i]/13;
}

check(p,q,r,cc)
char *p,*q,*r,*cc;
{
 char i;

 for(i=0;i<13;i++) p[i]=0;
 for(i=0;i<5;i++) q[i]=0;
 for(i=0;i<4;i++) r[i]=0;

 for(i=0;i<5;i++)
  p[cc[i]%13]++;

 for(i=0;i<13;i++)
  q[p[i]]++;

 for(i=0;i<5;i++)
  r[cc[i]/13]++;
}

// 同花順、鐵枝、葫、同花、順、三條、兔胚、胚、一隻
show_style(my,cpu)
char my,cpu;
{
 char *style[9]={"同花順","四張","葫蘆","同花","順子",
              "三條","兔胚","單胚","一張"};
 move(5,26); prints("[41;37;1m%s[m",style[cpu-1]);
 move(15,26); prints("[41;37;1m%s[m",style[my-1]);
 sprintf(sty,"我的牌[44;1m%s[m..電腦的牌[44;1m%s[m",
  style[my-1],style[cpu-1]);
}

/********************************/
/* BBS 站內遊戲–天地九九       */
/*                     11/26/98 */
/* dsyan.bbs@Forever.twbbs.org  */
/********************************/

#undef  NINE_DEBUG

//               0  1  2  3  4  5  6  7  8  9 10 11 12 /* 電腦 AI 所在 */
// char cp[13]={ 9, 8, 7, 6, 3, 2, 1, 0,11, 5, 4,10,12};
char tb[13]=  { 7, 6, 5, 4,10, 9, 3, 2, 1, 0,11, 8,12};
char *tu[4] = {"↓","→","↑","←"};
char card[52],hand[4][5],now,dir,turn,live;
char buf[255];
int sum;

static int
forqp(a,b)
char *a,*b;
{
 return tb[(*a)%13]-tb[(*b)%13];
}

p_nine()
{
 char genbuf[200];
 int bet,i,j,k,tmp,x,xx;
 FILE *fs;
 srandom(time(0));
 more("game/99.welcome",YEA);
 while(-1)
 {
  setutmpmode(NINE);
  showtitle("天地九九", BoardName);
  show_money(bet=0);
  do
  {
   getdata(21, 0,"要下注多少呢(上限5000)? 按 Enter 離開>", genbuf, 5, 1, 0);
   bet=atoi(genbuf);
   if(bet<0) bet=0;
  } while(bet>5000);
  if(!bet) return 0;
  if(check_money(bet))return 0;

  move(1,0);
  fs=fopen("game/99","r");
  while (fgets(genbuf, 255, fs)) prints(genbuf);

  move(21,0); clrtoeol();
  prints("[33;1m (←)(→)改變選牌  (↓)查詢該牌作用 (SPCAE)(Enter)打牌  [m");
  demoney(bet);
  show_money(bet);

  for(i=0;i<52;i++) card[i]=1;

  for(i=0;i<4;i++)
  {
   for(j=0;j<5;j++)
   {
    while(-1)
    {
     k=random()%52;
     if(card[k])
     {
      hand[i][j]=k;
      card[k]=0;
      break;
  }}}}

  qsort(hand[0],5,sizeof(char),forqp);
  x=xx=now=turn=sum=tmp=0;
  dir=1; live=3;
  show_mycard(100);

  while(-1)
  {
   move(9,52); prints(tu[turn]); refresh();
   sum++;
   if(turn) //電腦
   {
    qsort(hand[turn],5,sizeof(char),forqp);
    for(i=0;i<5;i++)
    {
     tmp=hand[turn][i]%13;
     if(tmp==0 || tmp==4 || tmp==5 || tmp>9) break;
     if(now+tmp<=99 && now+tmp>=0) break;
    }
    if(i<2) if (tmp==0 || tmp==4 || tmp==5 || tmp>9) i+=random()%(5-i);
    if(i==5) cpu_die();
     else add(&(hand[turn][i]));
    if(random()%5==0) mmsg();
    continue;
   }

   if(!live)
   {
//    game_log(NINE, "[32;1m在 %d 張牌贏了.. :)) [m %d",sum,bet);
    if(sum<25) live=20;
    else if(sum<50) live=15;
    else if(sum<100) live=10;
    else if(sum<150) live=7;
    else if(sum<200) live=5;
    else live=3;
    inmoney(bet*(live+1));
    sprintf(buf,"贏了 %d ... :D",bet*live);
    pressanykey(buf);
    break;
   }

   tmp=hand[0][4]%13;
   if(tmp!=0 && tmp!=4 && tmp!=5 && tmp<10 && now+tmp>99)
   {
    pressanykey("嗚嗚嗚..被電爆了!!.. :~");
//    game_log(NINE, "[31;1m在 %d 張牌被電腦電爆掉了.. :~ %d[m %d",sum,live,bet);
    break;
   }

   while(tmp!=13 && tmp!=32) //人類
   {
    move(18,xx*4+30); outs("  ");
    move(18,(xx=x)*4+30);

    if(tb[hand[0][x]%13]<7)
    {
     if(hand[0][x]%13+now>99) outs("！");
      else outs("○");
    }
    else outs("★");

    move(18,x*4+31);
    refresh();

    switch(tmp=igetkey())
    {
#ifdef NINE_DEBUG
     case KEY_UP:
      getdata(22, 0,"把牌換成> ", genbuf, 3, 1, 0);
      card[hand[0][x]]=3;
      hand[0][x]=atoi(genbuf);
      card[hand[0][x]]=0;
      qsort(hand[0],5,sizeof(char),forqp);
      show_mycard(100);
      break;
#endif
     case KEY_DOWN:
      nhelp(hand[0][x]);
      break;
     case KEY_LEFT: case 'l':
      x=x?x-1:4;
      break;
     case KEY_RIGHT: case 'r':
      x=(x==4)?0:x+1;
      break;
     case 'q':
      break;
    }
    if(tmp=='q') return;
   }

   move(18,xx*4+30); outs("  ");
   if(add(&(hand[0][x])))
   {
    pressanykey("嗚嗚嗚..白爛爆了!!.. :~");
//    game_log(NINE, "[31;1m在 %d 張牌白爛爆了.. :~ %d[m %d",sum,live,bet);
    break;
   }
   qsort(hand[0],5,sizeof(char),forqp);
   show_mycard(100);
}}}

show_mycard(t)
char t;
{
  char i;
#ifdef NINE_DEBUG
  char j;
#endif
  char *suit[4] = {"Ｃ","Ｄ","Ｈ","Ｓ"};
  char *num[13] = {"Ｋ","Ａ","２","３","４","５","６",
                   "７","８","９","10","Ｊ","Ｑ"};
  char coorx[4] = {30,38,30,22};
  char coory[4] = {8,6,4,6};

#ifdef NINE_DEBUG
  move(22,0);
  for(i=3;i>0;i--)
  {
   if(hand[i][0]==-1) continue;
   qsort(hand[i],5,sizeof(char),forqp);
   for(j=0;j<5;j++)
    prints(num[hand[i][j]%13]);
   prints("  ");
  }
#endif

  if(t==100)
  {
   for(i=0;i<5;i++)
   {
    move(16, 30+i*4); prints(num[hand[0][i]%13]);
    move(17, 30+i*4); prints(suit[hand[0][i]/13]);
   }
   return;
  }

  move(coory[turn], coorx[turn]);
  prints("╭───╮");
  move(coory[turn]+1,coorx[turn]);
  prints("│%s    │",num[t%13]);
  move(coory[turn]+2,coorx[turn]);
  prints("│%s    │",suit[t/13]);
  move(coory[turn]+3,coorx[turn]);
  prints("│      │");
  move(coory[turn]+4,coorx[turn]);
  prints("│      │");
//  prints("│    %s│",num[t%13]);
  move(coory[turn]+5,coorx[turn]);
  prints("│      │");
//  prints("│    %s│",suit[t/13]);
  move(coory[turn]+6, coorx[turn]);
  prints("╰───╯");

  move(8,50); prints("%s  %s",dir==1?"↙":"↗",dir==1?"↖":"↘");
  move(10,50); prints("%s  %s",dir==1?"↘":"↖",dir==1?"↗":"↙");

  move(13,46); prints("點數：%c%c%c%c",(now/10)?162:32,
   (now/10)?(now/10+175):32,162,now%10+175);
  move(14,46); prints("張數：%d",sum);
  refresh();
  sleep(1);
  move(21,0); clrtoeol(); refresh();
  prints("[33;1m (←)(→)改變選牌  (↓)查詢該牌作用 (SPCAE)(Enter)打牌 [m");
}

int add(t)
char *t;
{
  int k=0,change=0;

   switch(*t%13)
   {
    case 4:  //4 迴轉
     dir=-dir;
     break;

    case 5:  //5 指定
     move(21,0); clrtoeol();

     prints("指定那一家？ ");
     for(change=3;change>=0;change--)
      if(turn!=change && hand[change][0]!=-1)
       prints("(%s) ",tu[change]);

     change=0;

     while(!change)
     {
      int tmp;
      if(turn || live==1) tmp=random()%4+3;
       else tmp=igetkey();

      if((turn!=3 && hand[3][0]!=-1) && (tmp==KEY_LEFT || tmp==6) )
       change=6;
      else if((turn!=2 && hand[2][0]!=-1) && (tmp==KEY_UP || tmp==5) )
       change=5;
      else if((turn!=1 && hand[1][0]!=-1) && (tmp==KEY_RIGHT || tmp==4) )
       change=4;
      else if((turn!=0 && hand[0][0]!=-1) && tmp==3)
       change=3;
     }

     prints("[32;1m(%s)[m",tu[change-3]);
     break;

    case 10: //10 加或減10
     ten_or_twenty(10);
     break;

    case 11: //J PASS
     break;

    case 12: //Q 加或減20
     ten_or_twenty(20);
     break;

    case 0:  //K 馬上變99
     now=99;
     break;

    default:
     if((now+(*t%13))>99) return -1;
      else now+=(*t%13);
     break;
   }

   refresh();
   show_mycard(*t);
   while(-1)
   {
    k=random()%52;
    if(card[k]==1 && card[k]!=0)
    {
     card[*t]=3; *t=k; card[k]=0;
     break;
    }
    else card[k]--;
   }

   while(-1)
   {
    if(change)
    { turn=change-3; break;}
    else
    {
     turn=(turn+4+dir)%4;
     if(hand[turn][0]>-1) break;
   }}
   return 0;
}

ten_or_twenty(t)
int t;
{
     if(now<t-1) now+=t;
     else if(now>99-t) now-=t;
     else
     {
      int tmp=0;
      move(21,0); clrtoeol();
      prints("(←)(+)加%d  (→)(-)減%d  ",t,t);

      while(!tmp)
      {
       if(turn) tmp=random()%2+5;
        else tmp=igetkey();
       switch(tmp)
       {
        case KEY_LEFT: case '+': case 5:
         now+=t; prints("[32;1m加 %d[m",t); break;
        case KEY_RIGHT: case '-': case 6:
         now-=t; prints("[32;1m減 %d[m",t); break;
        default:
         tmp=0;
       }
      }
     }
}

cpu_die()
{
 switch(turn)
 {
  case 1: move(9,55); break;
  case 2: move(7,52); break;
  case 3: move(9,49); break;
 }
 prints("  ");
 live--;
 sprintf(buf,"電腦 %d 爆了!!! .. :DD",turn);
 pressanykey(buf);
 hand[turn][0]=-1;
 while(-1)
 {
  turn=(turn+4+dir)%4;
  if(hand[turn][0]>-1) break;
 }
}

nhelp(t)
int t;
{
 t%=13;
 switch(t)
 {
  case 0: pressanykey("九九：點數馬上變成９９"); break;
  case 4: pressanykey("迴轉：遊戲進行方向相反"); break;
  case 5: pressanykey("指定：自由指定下一個玩家"); break;
  case 11: pressanykey("PASS：可PASS一次"); break;
  case 10: pressanykey("點數加或減 10"); break;
  case 12: pressanykey("點數加或減 20"); break;
  default:
    sprintf(buf,"點數加 %d",t);
    pressanykey(buf); break;
 }
}

mmsg()
{
char *msg[12]={
"喂…打快一點啦！",
"打牌專心一點，不要流口水…",
"不要看美眉…",
"大爺行行好，放過我吧…",
"接招受死吧！！！",
"哇，你好厲害喔！竟然能打出這張牌！",
"你一定是賭徒轉世的！",
"last hand…",
"再玩電爆你！",
"哇…你有胸肌咧..",
"你的肚子跑出來了咧…",
"dsyan是好人…gwen是帥哥…"};
 move(21,0); clrtoeol(); refresh();
 prints("[%d;1m%s[m",random()%7+31,msg[random()%12]);
}
