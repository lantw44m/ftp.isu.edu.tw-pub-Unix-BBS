/*---------------------------------------------------------------------------*/
/* 基本選單:餵食 清潔 親親 休息                                              */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include <time.h>
#include "bbs.h"
#include "pip.h"
extern struct chicken d;
extern time_t start_time;
extern time_t lasttime;
#define getdata(a, b, c , d, e, f, g) getdata(a,b,c,d,e,f,NULL,g)
int pip_basic_takeshower() /*洗澡*/
{
     int lucky;
     d.shit-=20;
     if(d.shit<0) d.shit=0;
     d.hp-=rand()%2+3;
     move(4,0);
     lucky=rand()%3;
     if(lucky==0)
       {
         show_usual_pic(1);
         pressanykey("我是乾淨的小雞  cccc....");
       }
     else if(lucky==1)
       {
         show_usual_pic(7);
         pressanykey("馬桶 嗯∼∼");       
       }
     else
       {
         show_usual_pic(2);
         pressanykey("我愛洗澡 lalala....");
       }
     return 0;
}

int pip_basic_takerest() /*休息*/
{
//     count_tired(5,20,"Y",100,0);
     d.tired-=rand() % 15;
     d.tired = d.tired > 0 ? d.tired : 0;
     
     if(d.hp>d.maxhp)
       d.hp=d.maxhp;
     d.shit+=1;
     move(4,0);
     show_usual_pic(5);
     pressanykey("再按一下我就起床囉....");
     show_usual_pic(6);
     pressanykey("喂喂喂..該起床囉......");
     return 0;
}

int pip_basic_kiss()/*親親*/
{
     if(rand()%2>0)
       {
          d.happy+=rand()%3+4;
          d.satisfy+=rand()%2+1;
       }
     else
       {
          d.happy+=rand()%2+1;
          d.satisfy+=rand()%3+4;
       }
     count_tired(1,2,"N",100,1);
     d.shit+=rand()%5+4;
     d.relation+=rand()%2;
     move(4,0);
     show_usual_pic(3);
     if(d.shit<60)
      {
       pressanykey("來嘛! 啵一個.....");
      }
     else
      {
       pressanykey("親太多也是會髒死的喔....");
      }
     return 0;
}
int pip_basic_feed()     /* 餵食*/
{
  time_t now;
  char inbuf[80];
  char genbuf[200];
  char buf[256];
  long smoney;
  int pipkey;

  d.nodone=1;
  
  do
  {
   if(d.death==1 || d.death==2 || d.death==3)
     return 0;   
   if(pip_mainmenu(1)) return 0;
   move(b_lines-2, 0);
   clrtoeol();
   move(b_lines-2, 1);  
   sprintf(buf,"%s該做什麼事呢?",d.name);    
   prints(buf);   
   now=time(0);   
   move(b_lines, 0);
   clrtoeol();   
   move(b_lines, 0);
   prints("[1;44;37m  飲食選單  [46m[1]吃飯 [2]零食 [3]補丸 [4]靈芝 [5]人蔘 [6]雪蓮 [Q]跳出：         [m");   
   pip_time_change(now);
   pipkey=egetch();
   pip_time_change(now);

   switch(pipkey)
   {
   case '1':
    if(d.food<=0)
     {
      move(b_lines,0);
      pressanykey("沒有食物囉..快去買吧！");
      break;
     }
    move(4,0);
    if((d.bbtime/60/30)<3)
       show_feed_pic(0);
    else
       show_feed_pic(1);
    d.food--;
    d.hp+=50;
    if(d.hp >=d.maxhp)
     {
       d.hp=d.maxhp;
       d.weight+=rand()%2;
     }
    d.nodone=0;
    pressanykey("每吃一次食物會恢復體力50喔!");
    break;

   case '2':
    if(d.cookie<=0)
    {
      move(b_lines,0);
      pressanykey("零食吃光囉..快去買吧！");
      break;
    }
    move(4,0);    
    d.cookie--;
    d.hp+=100;
    if(d.hp >=d.maxhp)
     {
       d.hp=d.maxhp;
       d.weight+=(rand()%2+2);
     }
    else
     {
       d.weight+=(rand()%2+1);
     }
    if(rand()%2>0)
       show_feed_pic(2);
    else
       show_feed_pic(3);
    d.happy+=(rand()%3+4);
    d.satisfy+=rand()%3+2;
    d.nodone=0;
    pressanykey("吃零食容易胖喔...");
    break;

   case '3':
    if(d.bighp<=0)
    {
      move(b_lines,0);
      pressanykey("沒有大補丸了耶! 快買吧..");
      break;
    }
    d.bighp--;
    d.hp+=600;
    d.tired-=20;
    d.weight+=rand()%2;
    move(4,0);
    show_feed_pic(4);
    d.nodone=0;
    pressanykey("補丸..超極棒的唷...");
    break;

   case '4':
    if(d.medicine<=0)
     {
      move(b_lines,0);
      pressanykey("沒有靈芝囉..快去買吧！");
      break;
     }
    move(4,0);
    show_feed_pic(1);
    d.medicine--;
    d.mp+=50;
    if(d.mp >=d.maxmp)
     {
       d.mp=d.maxmp;
     }
    d.nodone=0;
    pressanykey("每吃一次靈芝會恢復法力50喔!");
    break;

   case '5':
    if(d.ginseng<=0)
    {
      move(b_lines,0);
      pressanykey("沒有千年人蔘耶! 快買吧..");
      break;
    }
    d.ginseng--;
    d.mp+=500;
    d.tired-=20;
    move(4,0);
    show_feed_pic(1);
    d.nodone=0;
    pressanykey("千年人蔘..超極棒的唷...");
    break;

   case '6':
    if(d.snowgrass<=0)
    {
      move(b_lines,0);
      pressanykey("沒有天山雪蓮耶! 快買吧..");
      break;
    }
    d.snowgrass--;
    d.mp=d.maxmp;
    d.hp=d.maxhp;
    d.tired-=0;
    d.sick=0;
    move(4,0);
    show_feed_pic(1);
    d.nodone=0;
    pressanykey("天山雪蓮..超極棒的唷...");
    break;

#ifdef MAPLE   
   case Ctrl('R'):
     if (currutmp->msgs[0].last_pid)
     {
     show_last_call_in();
     my_write(currutmp->msgs[0].last_pid, "水球丟回去：");
     }
    d.nodone=0;
    break;
#endif  // END MAPLE
   }
  }while((pipkey!='Q')&&(pipkey!='q')&&(pipkey!=KEY_LEFT));
  
  return 0;
}
