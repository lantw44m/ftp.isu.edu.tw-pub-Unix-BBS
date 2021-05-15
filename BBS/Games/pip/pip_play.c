/*---------------------------------------------------------------------------*/
/* 玩樂選單:散步 旅遊 運動 約會 猜拳                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include <time.h>
#include "bbs.h"
#include "pip.h"
extern struct chicken d;
extern time_t start_time;
extern time_t lasttime;
#define getdata(a, b, c , d, e, f, g) getdata(a,b,c,d,e,f,NULL,g)

int pip_play_stroll()	/*散步*/
{
    int lucky;
    count_tired(3,3,"Y",100,0);
    lucky=rand()%7;
    if(lucky==2)
     {
      d.happy+=rand()%3+rand()%3+9;
      d.satisfy+=rand()%3+rand()%3+3;
      d.shit+=rand()%3+3;
      d.hp-=(rand()%3+5);
      move(4,0);
      if(rand()%2>0)
        show_play_pic(1);
      else
        show_play_pic(2);
      pressanykey("遇到朋友囉  真好.... ^_^");
     }
    else if(lucky==3)
     {
      d.money+=100;
      d.happy+=rand()%3+6;
      d.satisfy+=rand()%3+4;
      d.shit+=rand()%3+3;
      d.hp-=(rand()%3+4);
      move(4,0);
      show_play_pic(3);
      pressanykey("撿到了100元了..耶耶耶....");
     }

    else if(lucky==4)
     {
      if(rand()%2>0)
      {
       d.happy-=(rand()%2+5);
       move(4,0);
       d.hp-=(rand()%3+3);
       show_play_pic(4);
       if(d.money>=50)
        {
         d.money-=50;
         pressanykey("掉了50元了..嗚嗚嗚....");
        }
       else
        {
         d.money=0;
         d.hp-=(rand()%3+3);
         pressanykey("錢掉光光了..嗚嗚嗚....");
        }
       d.shit+=rand()%3+2;
      }
      else
      {
       d.happy+=rand()%3+5;
       move(4,0);
       show_play_pic(5);
       if(d.money>=50)
        {
         d.money-=50;
         d.hp-=(rand()%3+3);
         pressanykey("用了50元了..不可以罵我喔....");
        }
       else
        {
         d.money=0;
         d.hp-=(rand()%3+3);
         pressanykey("錢被我偷用光光了..:p");
        }
       d.shit+=rand()%3+2;
      }
     }
    else if(lucky==5)
     {
      d.happy+=rand()%3+6;
      d.satisfy+=rand()%3+5;
      d.shit+=2;
      move(4,0);
      if(rand()%2>0)
        show_play_pic(6);
      else
        show_play_pic(7);
      pressanykey("好棒喔撿到玩具了說.....");
     }
    else if(lucky==6)
     {
      d.happy-=(rand()%3+10);
      d.shit+=(rand()%3+20);
      move(4,0);
      show_play_pic(9);
      pressanykey("真是倒楣  可以去買愛國獎券");
     }
    else
    {
      d.happy+=rand()%3+3;
      d.satisfy+=rand()%2+1;
      d.shit+=rand()%3+2;
      d.hp-=(rand()%3+2);
      move(4,0);
      show_play_pic(8);
      pressanykey("沒有特別的事發生啦.....");
    }
    return 0;
}

int pip_play_sport()	/*運動*/
{
    count_tired(3,8,"Y",100,1);
    d.weight-=(rand()%3+2);
    d.satisfy+=rand()%2+3;
    if(d.satisfy>100)
      d.satisfy=100;
    d.shit+=rand()%5+10;
    d.hp-=(rand()%2+8);
    d.maxhp+=rand()%2;
    d.speed+=(2+rand()%3);
    move(4,0);
    show_play_pic(10);
    pressanykey("運動好處多多啦...");
    return 0;
}

int pip_play_date()	/*約會*/
{
    if(d.money<150)
    {
     pressanykey("你錢不夠多啦! 約會總得花點錢錢");
    }
    else
    {
     count_tired(3,6,"Y",100,1);
     d.happy+=rand()%5+12;
     d.shit+=rand()%3+5;
     d.hp-=rand()%4+8;
     d.satisfy+=rand()%5+7;
     d.character+=rand()%3+1;
     d.money=d.money-150;
     move(4,0);
     show_play_pic(11);
     pressanykey("約會去  呼呼");
    }
    return 0;
}
int pip_play_outing()	/*郊遊*/
{
    int lucky;
    char buf[256];
    
    if(d.money<250)
    {
     pressanykey("你錢不夠多啦! 旅遊總得花點錢錢");
    }
    else
    { 
      d.weight+=rand()%2+1;
      d.money-=250;   
      count_tired(10,45,"N",100,0);
      d.hp-=rand()%10+20;
      if(d.hp>=d.maxhp)
           d.hp=d.maxhp;
      d.happy+=rand()%10+12;
      d.character+=rand()%5+5;
      d.satisfy+=rand()%10+10;
      lucky=rand()%4;
      if(lucky==0)
      {
       d.maxmp+=rand()%3;
       d.art+=rand()%2;
       show_play_pic(12);
       if(rand()%2>0)
         pressanykey("心中有一股淡淡的感覺  好舒服喔....");
       else
         pressanykey("雲水 閑情 心情好多了.....");
      }
      else if(lucky==1)
      {
       d.art+=rand()%3;
       d.maxmp+=rand()%2;
       show_play_pic(13);
       if(rand()%2>0)
         pressanykey("有山有水有落日  形成一幅美麗的畫..");
       else
         pressanykey("看著看著  全身疲憊都不見囉..");
      }
      else if(lucky==2)
      {
       d.love+=rand()%3;
       show_play_pic(14);
       if(rand()%2>0)
         pressanykey("看  太陽快沒入水中囉...");
       else
         pressanykey("聽說這是海邊啦  你說呢?");
      }      
      else if(lucky==3)
      {
       d.maxhp+=rand()%3;
       show_play_pic(15);
       if(rand()%2>0)
         pressanykey("讓我們瘋狂在夜裡的海灘吧....呼呼..");
       else
         pressanykey("涼爽的海風迎面襲來  最喜歡這種感覺了....");
      }
      if((rand()%301+rand()%200)%100==12)
      {
        lucky=0;
        clear();
        sprintf(buf,"[1;41m  星空戰鬥雞 ∼ %-10s                                                    [0m",d.name); 	 
        show_play_pic(0);
        move(17,10);
        prints("[1;36m親愛的 [1;33m%s ∼[0m",d.name);
        move(18,10);
        prints("[1;37m看到你這樣努力的培養自己的能力  讓我心中十分的高興喔..[m");
        move(19,10);
        prints("[1;36m小天使我決定給你獎賞鼓勵鼓勵  偷偷地幫助你一下....^_^[0m");
        move(20,10);
        lucky=rand()%7;
        if(lucky==6)
        {
          prints("[1;33m我將幫你的各項能力全部提升百分之五喔......[0m");
          d.maxhp=d.maxhp*105/100;
          d.hp=d.maxhp;
          d.maxmp=d.maxmp*105/100;
          d.mp=d.maxmp;          
          d.attack=d.attack*105/100;
          d.resist=d.resist*105/100;          
          d.speed=d.speed*105/100;          
          d.character=d.character*105/100;          
          d.love=d.love*105/100;          
          d.wisdom=d.wisdom*105/100;     
          d.art=d.art*105/100;               
          d.brave=d.brave*105/100;          
          d.homework=d.homework*105/100;          
        }
        
        else if(lucky<=5 && lucky>=4)
        {
          prints("[1;33m我將幫你的戰鬥能力全部提升百分之十喔.......[0m");        
          d.attack=d.attack*110/100;
          d.resist=d.resist*110/100;          
          d.speed=d.speed*110/100;        
          d.brave=d.brave*110/100;                              
        } 
               
        else if(lucky<=3 && lucky>=2)
        {
          prints("[1;33m我將幫你的魔法能力和生命力全部提升百分之十喔.......[0m");        
          d.maxhp=d.maxhp*110/100;
          d.hp=d.maxhp;
          d.maxmp=d.maxmp*110/100;
          d.mp=d.maxmp;                  
        }
        else if(lucky<=1 && lucky>=0)
        {
          prints("[1;33m我將幫你的感受能力全部提升百分之二十喔....[0m");                
          d.character=d.character*110/100;          
          d.love=d.love*110/100;          
          d.wisdom=d.wisdom*110/100;     
          d.art=d.art*110/100;               
          d.homework=d.homework*110/100;                  
        }        
        
        pressanykey("請繼續加油喔..."); 
      }
    }
    return 0;
}

int pip_play_kite()	/*風箏*/
{
    count_tired(4,4,"Y",100,0);
    d.weight+=(rand()%2+2);
    d.satisfy+=rand()%3+12;
    if(d.satisfy>100)
      d.satisfy=100;
    d.happy+=rand()%5+10;
    d.shit+=rand()%5+6;
    d.hp-=(rand()%2+7);
    d.affect+=rand()%4+6;
    move(4,0);
    show_play_pic(16);
    pressanykey("放風箏真好玩啦...");
    return 0;
}

int pip_play_KTV()	/*KTV*/
{
    if(d.money<250)
    {
     pressanykey("你錢不夠多啦! 唱歌總得花點錢錢");
    }
    else
    {
     count_tired(10,10,"Y",100,0);
     d.satisfy+=rand()%2+20;
     if(d.satisfy>100)
       d.satisfy=100;
     d.happy+=rand()%3+20;
     d.shit+=rand()%5+6;
     d.money-=250;
     d.hp+=(rand()%2+6);
     d.art+=rand()%4+3;
     move(4,0);
     show_play_pic(17);
     pressanykey("你說你  想要逃...");
    }
    return 0;
}

int pip_play_guess()   /* 猜拳程式 */
{
   int ch,com;
   int pipkey;
   char inbuf[10];
   struct tm *qtime;
   time_t now;

   time(&now);
   qtime = localtime(&now);
   d.satisfy+=(rand()%3+2);
   count_tired(2,2,"Y",100,1);
   d.shit+=rand()%3+2;
   do
   {
    if(d.death==1 || d.death==2 || d.death==3)
      return 0;  
    if(pip_mainmenu(0)) return 0;
    move(b_lines-2,0);
    clrtoeol();  
    move(b_lines, 0);
    clrtoeol();
    move(b_lines,0);
    prints("[1;44;37m  猜拳選單  [46m[1]我出剪刀 [2]我出石頭 [3]我出布啦 [4]猜拳記錄 [Q]跳出：         [m");   
    move(b_lines-1, 0);
    clrtoeol();
    pipkey=egetch();
    switch(pipkey)
    {
#ifdef MAPLE
      case Ctrl('R'):
	     if (currutmp->msgs[0].last_pid)
	     {
	      show_last_call_in();
	      my_write(currutmp->msgs[0].last_pid, "水球丟回去：");
	     }
	     break;
#endif  // END MAPLE
      case '4':
             situ();
             break;
     }
   }while((pipkey!='1')&&(pipkey!='2')&&(pipkey!='3')&&(pipkey !='q')&&(pipkey !='Q'));

    com=rand()%3;
    move(18,0);
    clrtobot();
    switch(com){
        case 0:
          outs("小雞：剪刀\n");
         break;
        case 1:
          outs("小雞：石頭\n");
         break;
        case 2:
          outs("小雞：布\n");
         break;
     }

    move(17,0);

    switch(pipkey){
    case '1':
      outs("你  ：剪刀\n");
      if (com==0)
        tie();
      else  if (com==1)
        lose();
      else if (com==2)
        win();
      break;
    case '2':
      outs("你　：石頭\n");
      if (com==0)
        win();
      else if (com==1)
        tie();
      else if (com==2)
        lose();
      break;
    case '3':
      outs("你　：布\n");
      if (com==0)
        lose();
      else if (com==1)
        win();
      else if (com==2)
        tie();
      break;
    case 'q':
      break;
  }

}

int win()
{
    d.winn++;
    d.hp-=rand()%2+3;
    move(4,0);
    show_guess_pic(2);
    move(b_lines,0);
    pressanykey("小雞輸了....~>_<~");
    return;
}
int tie()
{
    d.hp-=rand()%2+3;
    d.happy+=rand()%3+5;
    move(4,0);
    show_guess_pic(3);
    move(b_lines,0);
    pressanykey("平手........-_-");
        return;
}
int lose()
{
    d.losee++;
    d.happy+=rand()%3+5;
    d.hp-=rand()%2+3;
    move(4,0);
    show_guess_pic(1);
    move(b_lines,0);
    pressanykey("小雞贏囉....*^_^*");
    return;
}

int situ()
{
        clrchyiuan(19,21);
        move(19,0);
        prints("你:[44m %d勝 %d負[m                     \n",d.winn,d.losee);
        move(20,0);
        prints("雞:[44m %d勝 %d負[m                     \n",d.losee,d.winn);

       if (d.winn>=d.losee)
       {
        move(b_lines,0);
        pressanykey("哈..贏小雞也沒多光榮");
       }
       else
       {
        move(b_lines,0);
        pressanykey("笨蛋..竟輸給了雞....ㄜ...");
       }
       return;
}
