/*---------------------------------------------------------------------------*/
/* �򥻿��:���� �M�� �˿� ��                                              */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include <time.h>
#include "bbs.h"
#include "pip.h"
extern struct chicken d;
extern time_t start_time;
extern time_t lasttime;
#define getdata(a, b, c , d, e, f, g) getdata(a,b,c,d,e,f,NULL,g)
int pip_basic_takeshower() /*�~��*/
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
         pressanykey("�ڬO���b���p��  cccc....");
       }
     else if(lucky==1)
       {
         show_usual_pic(7);
         pressanykey("���� ����");       
       }
     else
       {
         show_usual_pic(2);
         pressanykey("�ڷR�~�� lalala....");
       }
     return 0;
}

int pip_basic_takerest() /*��*/
{
//     count_tired(5,20,"Y",100,0);
     d.tired-=rand() % 15;
     d.tired = d.tired > 0 ? d.tired : 0;
     
     if(d.hp>d.maxhp)
       d.hp=d.maxhp;
     d.shit+=1;
     move(4,0);
     show_usual_pic(5);
     pressanykey("�A���@�U�ڴN�_���o....");
     show_usual_pic(6);
     pressanykey("�޳޳�..�Ӱ_���o......");
     return 0;
}

int pip_basic_kiss()/*�˿�*/
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
       pressanykey("�ӹ�! �q�@��.....");
      }
     else
      {
       pressanykey("�ˤӦh�]�O�|ż������....");
      }
     return 0;
}
int pip_basic_feed()     /* ����*/
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
   sprintf(buf,"%s�Ӱ�����ƩO?",d.name);    
   prints(buf);   
   now=time(0);   
   move(b_lines, 0);
   clrtoeol();   
   move(b_lines, 0);
   prints("[1;44;37m  �������  [46m[1]�Y�� [2]�s�� [3]�ɤY [4]�F�� [5]�H�x [6]���� [Q]���X�G         [m");   
   pip_time_change(now);
   pipkey=egetch();
   pip_time_change(now);

   switch(pipkey)
   {
   case '1':
    if(d.food<=0)
     {
      move(b_lines,0);
      pressanykey("�S�������o..�֥h�R�a�I");
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
    pressanykey("�C�Y�@�������|��_��O50��!");
    break;

   case '2':
    if(d.cookie<=0)
    {
      move(b_lines,0);
      pressanykey("�s���Y���o..�֥h�R�a�I");
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
    pressanykey("�Y�s���e���D��...");
    break;

   case '3':
    if(d.bighp<=0)
    {
      move(b_lines,0);
      pressanykey("�S���j�ɤY�F�C! �ֶR�a..");
      break;
    }
    d.bighp--;
    d.hp+=600;
    d.tired-=20;
    d.weight+=rand()%2;
    move(4,0);
    show_feed_pic(4);
    d.nodone=0;
    pressanykey("�ɤY..�W���Ϊ���...");
    break;

   case '4':
    if(d.medicine<=0)
     {
      move(b_lines,0);
      pressanykey("�S���F���o..�֥h�R�a�I");
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
    pressanykey("�C�Y�@���F�۷|��_�k�O50��!");
    break;

   case '5':
    if(d.ginseng<=0)
    {
      move(b_lines,0);
      pressanykey("�S���d�~�H�x�C! �ֶR�a..");
      break;
    }
    d.ginseng--;
    d.mp+=500;
    d.tired-=20;
    move(4,0);
    show_feed_pic(1);
    d.nodone=0;
    pressanykey("�d�~�H�x..�W���Ϊ���...");
    break;

   case '6':
    if(d.snowgrass<=0)
    {
      move(b_lines,0);
      pressanykey("�S���Ѥs�����C! �ֶR�a..");
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
    pressanykey("�Ѥs����..�W���Ϊ���...");
    break;

#ifdef MAPLE   
   case Ctrl('R'):
     if (currutmp->msgs[0].last_pid)
     {
     show_last_call_in();
     my_write(currutmp->msgs[0].last_pid, "���y��^�h�G");
     }
    d.nodone=0;
    break;
#endif  // END MAPLE
   }
  }while((pipkey!='Q')&&(pipkey!='q')&&(pipkey!=KEY_LEFT));
  
  return 0;
}
