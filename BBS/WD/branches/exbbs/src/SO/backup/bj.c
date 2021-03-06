#include "bbs.h"

#define ba rpguser.race == 6 ? 10*rpguser.level : 5
/* 堵城笴栏 */

int print_card(int card,int x,int y)
{
  char *flower[4]={"⑨","⒅","⒁","⒀"};
  char *poker[52]={"⑾","⑾","⑾","⑾","⒈","⒈","⒈","⒈","⒉","⒉","⒉","⒉",
                   "⒊","⒊","⒊","⒊","⒋","⒋","⒋","⒋","⒌","⒌","⒌","⒌",
                   "⒍","⒍","⒍","⒍","⒎","⒎","⒎","⒎","⒏","⒏","⒏","⒏",
                   "10","10","10","10","⒇","⒇","⒇","⒇","⑦","⑦","⑦","⑦",
                   "①","①","①","①"};

move(x,y);   prints("ⅰ");
move(x+1,y); prints("%s    ",poker[card]);
move(x+2,y); prints("%s    ",flower[card%4]);
move(x+3,y); prints("      ");
move(x+4,y); prints("      ");
move(x+5,y); prints("      ");
move(x+6,y); prints("ⅱⅲ");
return 0;
}


void
BlackJack()
{
  char buf[256];
  int    num[52]={11,11,11,11,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5,5,6,6,6,6,
                   7,7,7,7,8,8,8,8,9,9,9,9,10,10,10,10,10,10,10,10,
                  10,10,10,10,10,10,10,10};
  int cardlist[52]={0};
  int i,j,m,tmp=0,tmp2,ch;
  int win=2,win_jack=5; /* win 墓瞯, win_jack 玡ㄢ眎碞 21 翴瞯 */
  int six=10, seven=20, aj=10, super_jack=20; /* 777, A+J, spade A+J 瞯 */
  int host_count=2, guest_count=1, card_count=3, A_count=0, AA_count=0;
  int host_point=0, guest_point=0, mov_y=4;
  int host_card[12]={0}, guest_card[12]={0};
  long int money;

  int CHEAT=0; /* 暗国把计, 1 碞国, 0 碞ぃ */

  time_t now = time(0);
  time(&now);

  clear();
  setutmpmode(BLACKJACK);
  do{
  move(0,0);prints("眤ō临Τ [1;44;33m%d[m じ",cuser.silvermoney);
  getdata(1, 0, "璶┿猔ぶ窥(1 - 250000)? ", buf, 7, DOECHO,0);
  money=atoi(buf);
  if(!buf[0])return;
  if(check_money(money,SILVER)) return;
  }while((money<1)||(money>250000));
  demoney(money);
  move(2,0);prints("( y 尿礟, n ぃ尿礟, d double)");
  move(0,0);clrtoeol();prints("眤临Τ [1;44;33m%d[m 蝗ㄢ",cuser.silvermoney);
  for(i=1;i<=52;i++){
    m=0;
    do{
    j=(time(0)+cuser.silvermoney+rand())%52;
    if (cardlist[j]==0){cardlist[j]=i;m=1;}
    }while(m==0);
  };
  for(i=0;i<52;i++)cardlist[i]--; /* 瑍礟 */

  if(money>=20000)CHEAT=1;
  if(CHEAT==1){
    if(cardlist[1]<=3){
      tmp2=cardlist[50];
      cardlist[50]=cardlist[1];
      cardlist[1]=tmp2;
    }
  }                             /* 国絏 */

  host_card[0]=cardlist[0];
  if(host_card[0]<4)AA_count++;
  guest_card[0]=cardlist[1];

  if(guest_card[0]<4)A_count++;
  host_card[1]=cardlist[2];
  if(host_card[1]<4)AA_count++; /* 祇玡眎礟 */

  move(5,0);  prints("ⅰ");
  move(6,0);  prints("      ");
  move(7,0);  prints("      ");
  move(8,0);  prints("      ");
  move(9,0);  prints("      ");
  move(10,0); prints("      ");
  move(11,0); prints("ⅱⅲ");
  print_card(host_card[1],5,4);
  print_card(guest_card[0],15,0);  /* 玡眎礟 */

  host_point=num[host_card[1]];
  guest_point=num[guest_card[0]];

  do{
    m=1;
    guest_card[guest_count]=cardlist[card_count];
    if(guest_card[guest_count]<4)A_count++;
    print_card(guest_card[guest_count],15,mov_y);
    guest_point+=num[guest_card[guest_count]];

    if((guest_card[0]>=24&&guest_card[0]<=27)&&(guest_card[1]>=24&&guest_card[1]<=27)&&(guest_card[2]>=24&&guest_card[2]<=27)){
      move(18,3);prints("[1;41;33m     ⒍⒍⒍     [m");
      move(3,0);prints("[1;41;33m⒍⒍⒍ !!! 眔贱 %d 蝗ㄢ[m",money*seven);
      inmoney(money*seven);
      inexp(ba*7);
      game_log(BLACKJACK,"い [1;33m%7d[m じ "COLOR1"[1m  ⒍⒍⒍   [m"
        ,money*seven);
      pressanykey("眤临Τ [1;44;33m%d[m 蝗ㄢ",cuser.silvermoney);
      return;
    }

    if((guest_card[0]==40&&guest_card[1]==0)||(guest_card[0]==0&&guest_card[1]==40)){
      move(18,3);prints("[1;41;33m 禬タ参 BLACK JACK  [m");
      move(3,0);prints("[1;41;33m禬タ参 BLACK JACK !!! 眔贱 %d 蝗ㄢ[m",money*super_jack);
      inmoney(money*super_jack);
      inexp(ba*5);
      game_log(BLACKJACK,"い [1;33m%7d[m じ [1;41;33m タ参 ⑾⒇ [m"
        ,money*super_jack);
      pressanykey("眤临Τ [1;44;33m%d[m 蝗ㄢ",cuser.silvermoney);
      return;
    }

    if((guest_card[0]<=3&&guest_card[0]>=0)&&(guest_card[1]<=43&&guest_card[1]>=40))tmp=1;

if((tmp==1)||((guest_card[1]<=3&&guest_card[1]>=0)&&(guest_card[0]<=43&&guest_card[0]>=40))){
      move(18,3);prints("[1;41;33m SUPER BLACK JACK  [m");
      move(3,0);prints("[1;41;33mSUPER BLACK JACK !!! 眔贱 %d 蝗ㄢ[m",money*aj);
      inmoney(money*aj);
      inexp(ba*5);
      game_log(BLACKJACK,"い [1;33m%7d[m じ [1;44;33m Super⑾⒇ [m",money*aj);
      pressanykey("眤临Τ [1;44;33m%d[m 蝗ㄢ",cuser.silvermoney);
      return;
    }

    if(guest_point==21&&guest_count==1){
      move(18,3);prints("[1;41;33m  BLACK JACK  [m");
      move(3,0);prints("[1;41;33mBLACK JACK !!![44m 眔贱 %d 蝗ㄢ[m",money*win_jack);
      inmoney(money*win_jack);
      inexp(ba*3);
      move(0,0);clrtoeol();prints("眤临Τ [1;44;33m%d[m 蝗ㄢ",cuser.silvermoney);
    if(money*win_jack>=500000){
      game_log(BLACKJACK,"い [1;33m%7d[m じ [1;47;30m BlackJack [m %s ",money*win_jack);
    }

      pressanykey(NULL);
      return;
    }                        /* 玡ㄢ眎碞 21 翴 */

    if(guest_point>21){
      if(A_count>0){guest_point-=10;A_count--;};
    }
    move(12,0); clrtoeol();prints("[1;32m翴计: [33m%d[m",host_point);
    move(14,0); clrtoeol();prints("[1;32m翴计: [33m%d[m",guest_point);
    if(guest_point>21){
      pressanykey("  脄奔罢~~~  ");
      return;
    }

    if(guest_count==5){
      move(18,3);prints("[1;41;33m            筁せ闽            [m");
      move(3,0);prints("[1;41;33m筁せ闽 !!! 眔贱 %d 蝗ㄢ[m",money*six);
      inmoney(money*six);
      inexp(ba*5);
      game_log(BLACKJACK,"い [1;33m%7d[m じ [1;44;33m  筁せ闽   [m",money*six);
      pressanykey("眤临Τ %d 蝗刽",cuser.silvermoney);
      return;
    }

    guest_count++;
    card_count++;
    mov_y+=4;

    do{
      if(ch=='d')m=0;
      if(m!=0)ch=igetkey();
    }while(ch!='y'&&ch!='n'&&ch!='d'&&m!=0); /* ъ key */

    if(ch=='d'&&m!=0&&guest_count==2){
      if(cuser.silvermoney>=money){
        demoney(money);
        money*=2;
      }
      else ch='n';
      move(0,0);clrtoeol();prints("眤临Τ [1;44;33m%d[m 蝗ㄢ",cuser.silvermoney);
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
    move(12,0); clrtoeol();prints("[1;32m翴计: [33m%d[m",host_point);
    move(14,0); clrtoeol();prints("[1;32m翴计: [33m%d[m",guest_point);
    if(host_point>21){
      move(14,0); clrtoeol(); prints("[1;32m翴计: [33m%d [1;41;33m WINNER [m",guest_point);

      move(3,0);prints("[1;44;33m墓~~~~ 眔贱 %d 蝗ㄢ[m",money*win);
      inmoney(money*win);
      inexp(ba);
      move(0,0);clrtoeol();prints("眤临Τ [1;44;33m%d[m 蝗ㄢ",cuser.silvermoney);
      pressanykey(NULL);
      return;
    }
    host_count++;
    card_count++;
    mov_y+=4;
  }while(host_point<guest_point);

  pressanykey("块~~~~ 蝗ㄢ⊿Μ!");
  return;
}
