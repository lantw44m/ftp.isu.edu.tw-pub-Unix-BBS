/*---------------------------------------------------------------------------*/
/* 函式特區                                                                  */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include <time.h>
#include "bbs.h"
#include "pip.h"
extern struct chicken d;
extern time_t start_time;
extern time_t lasttime;
#define getdata(a, b, c , d, e, f, g) getdata(a,b,c,d,e,f,NULL,g)

/*名字        體力MAX法力MAX  攻擊   防護     速度    財寶   特別   圖檔*/
struct playrule resultmanlist[] = {
"茱麗葉塔",	 60,0,	20,0,	20,	20,	 20,	150, "11101",	0,0,
"菲歐利娜",	 60,0,	20,0,	30,	30,	 30,	200, "01111",	0,0,
"阿妮斯",	 80,0,	40,0,	50,	35,	 60,	250, "11110",	0,0,
"帕多雷西亞",	 85,0,	30,0,	80,	90,	 80,	500, "10111",	0,0,
"卡美拉美", 	 90,0,  50,0,   75,	70,	 60,	550, "11010",	0,0,
"姍娜麗娃",	 90,0,  40,0,   10,     30,      50,    880, "10100",   0,0,
NULL,		  0,0,	 0,0,	 0,	 0,	  0,	  0,	NULL,	0,0
};
/*求婚*/
int
pip_marriage_offer()
{
   time_t now;
   char buf[256];
   char ans[4];
   int money;
   int who;
   char *name[5][2]={{"女商人Ａ","商人Ａ"},
                     {"女商人Ｂ","商人Ｂ"},
                     {"女商人Ｃ","商人Ｃ"},
                     {"女商人Ｄ","商人Ｄ"},
                     {"女商人Ｅ","商人Ｅ"}};
   do
   { 
     who=rand()%5;
   }while(d.lover==(who+3));
  
   money=rand()%2000+rand()%3000+4000;
   sprintf(buf,"%s帶來了金錢%d，要向你的小雞求婚，您願意嗎？[y/N]",name[who][d.sex-1],money);
#ifdef MAPLE
   getdata(b_lines-1, 1,buf, ans, 2, 1, 0);
#else
   getdata(b_lines-1, 1,buf, ans, 2, DOECHO, YEA);
#endif  // END MAPLE
   if(ans[0]=='y' || ans[0]=='Y')
   {
     if(d.wantend!=1 && d.wantend!=4)
     {
       sprintf(buf,"ㄚ∼之前已經有婚約了，您確定要解除舊婚約，改定立婚約嗎？[y/N]");
#ifdef MAPLE
       getdata(b_lines-1, 1,buf, ans, 2, 1, 0);
#else
       getdata(b_lines-1, 1,buf, ans, 2, DOECHO, YEA);
#endif  // END MAPLE
       if(ans[0]!='y' && ans[0]!='Y')
       {
         d.social+=10;
         pressanykey("還是維持舊婚約好了..");
         return 0;
       }
       d.social-=rand()%50+100;
     }
     d.charm-=rand()%5+20;
     d.lover=who+3;
     d.relation-=20;
     if(d.relation<0)
        d.relation=0;
     if(d.wantend<4)
     	d.wantend=2;
     else
        d.wantend=5;
     pressanykey("我想對方是一個很好的伴侶..");
     now=time(0);
     sprintf(buf, "[1;37m%s %-11s的小雞 [%s] 接受了 %s 的求婚[0m\n", Cdate(&now), cuser.userid,d.name,name[who][d.sex-1]);
     pip_log_record(buf);
   }
   else
   {
     d.charm+=rand()%5+20;
     d.relation+=20;
     if(d.wantend==1 || d.wantend==4)
	     pressanykey("我還年輕  心情還不定...");
     else
     	     pressanykey("我早已有婚約了..對不起...");
   }
   d.money+=money;
   return 0;
}

int pip_results_show()  /*收穫季*/
{
	char *showname[5]={"  ","武鬥大會","藝術大展","皇家舞會","烹飪大賽"};
	char buf[256];
	int pipkey,i=0;
	int winorlost=0;
	int a,b[3][2],c[3];

	clear();
	move(10,14);
	prints("[1;33m叮咚叮咚∼ 辛苦的郵差幫我們送信來了喔...[0m");
	pressanykey("嗯  把信打開看看吧...");
	clear();
	show_resultshow_pic(0);
	sprintf(buf,"[A]%s [B]%s [C]%s [D]%s [Q]放棄:",showname[1],showname[2],showname[3],showname[4]);
	move(b_lines,0);
	prints(buf);	
	do
	{
		pipkey=egetch();
	}
	while(pipkey!='q' && pipkey!='Q' && pipkey!='A' && pipkey!='a' &&
	       pipkey!='B' && pipkey!='b' && pipkey!='C' && pipkey!='c'&&
	       pipkey!='D' && pipkey!='d');
	a=rand()%4+1;
	b[0][0]=a-1;
	b[1][0]=a+1;
	b[2][0]=a;
	switch(pipkey)
	{
		case 'A':
		case 'a':
			pressanykey("今年共有四人參賽∼現在比賽開始");
			for(i=0;i<3;i++)
			{
				a=0;
				b[i][1]=0;
				sprintf(buf,"你的第%d個對手是%s",i+1,resultmanlist[b[i][0]].name);
				pressanykey(buf);
				a=pip_vs_man(b[i][0],resultmanlist,2);
				if(a==1)
					b[i][1]=1;/*對方輸了*/
				winorlost+=a;	
				d.death=0;
			}
			switch(winorlost)
			{
				case 3:
					pip_results_show_ending(3,1,b[1][0],b[0][0],b[2][0]);
					d.hexp+=rand()%10+50;
					break;
				case 2:
					if(b[0][1]!=1) 
					{
						c[0]=b[0][0];
						c[1]=b[1][0];
						c[2]=b[2][0];
					}
					else if(b[1][1]!=1) 
					{
						c[0]=b[1][0];
						c[1]=b[2][0];
						c[2]=b[0][0];
					}
					else if(b[2][1]!=1) 
					{
						c[0]=b[2][0];
						c[1]=b[0][0];
						c[2]=b[1][0];
					}
					pip_results_show_ending(2,1,c[0],c[1],c[2]);
					d.hexp+=rand()%10+30;
					break;
				case 1:
					if(b[0][1]==1) 
					{
						c[0]=b[2][0];
						c[1]=b[1][0];
						c[2]=b[0][0];
					}
					else if(b[1][1]==1) 
					{
						c[0]=b[0][0];
						c[1]=b[2][0];
						c[2]=b[1][0];
					}
					else if(b[2][1]==1) 
					{
						c[0]=b[1][0];
						c[1]=b[0][0];
						c[2]=b[2][0];
					}
					pip_results_show_ending(1,1,c[0],c[1],c[2]);
					d.hexp+=rand()%10+10;
					break;
				case 0:
					pip_results_show_ending(0,1,b[0][0],b[1][0],b[2][0]);
					d.hexp-=rand()%10+10;
					break;
			}
			break;
		case 'B':
		case 'b':
			pressanykey("今年共有四人參賽∼現在比賽開始");
			show_resultshow_pic(21);
			pressanykey("比賽情形");
			if((d.art*2+d.character)/400>=5)
			{
				winorlost=3;
			}
			else if((d.art*2+d.character)/400>=4)
			{
				winorlost=2;
			}
			else if((d.art*2+d.character)/400>=3)
			{
				winorlost=1;
			}
			else
			{
				winorlost=0;
			}
			pip_results_show_ending(winorlost,2,rand()%2,rand()%2+2,rand()%2+4);
			d.art+=rand()%10+20*winorlost;
			d.character+=rand()%10+20*winorlost;
			break;
		case 'C':
		case 'c':
			pressanykey("今年共有四人參賽∼現在比賽開始");
			if((d.art*2+d.charm)/400>=5)
			{				
				winorlost=3;
			}
			else if((d.art*2+d.charm)/400>=4)
			{
				winorlost=2;
			}
			else if((d.art*2+d.charm)/400>=3)
			{
				winorlost=1;
			}
			else
			{
				winorlost=0;
			}
			d.art+=rand()%10+20*winorlost;
			d.charm+=rand()%10+20*winorlost;
			pip_results_show_ending(winorlost,3,rand()%2,rand()%2+4,rand()%2+2);
			break;
		case 'D':
		case 'd':
			pressanykey("今年共有四人參賽∼現在比賽開始");
			if((d.affect+d.cookskill*2)/200>=4)
			{
				winorlost=3;
			}
			else if((d.affect+d.cookskill*2)/200>=3)
			{
				winorlost=2;
			}
			else if((d.affect+d.cookskill*2)/200>=2)
			{
				winorlost=1;
			}
			else
			{
				winorlost=0;
			}
			d.cookskill+=rand()%10+20*winorlost;
			d.family+=rand()%10+20*winorlost;
			pip_results_show_ending(winorlost,4,rand()%2+2,rand()%2,rand()%2+4);
			break;
		case 'Q':
		case 'q':
			pressanykey("今年不參加啦.....:(");
			d.happy-=rand()%10+10;
			d.satisfy-=rand()%10+10;
			d.relation-=rand()%10;
			break;
	}
	if(pipkey!='Q' && pipkey!='q')
	{
		d.tired=0;
		d.hp=d.maxhp;
		d.happy+=rand()%20;
		d.satisfy+=rand()%20;
		d.relation+=rand()%10;
	}
	return 0;
}

int pip_results_show_ending(winorlost,mode,a,b,c)
int winorlost,mode,a,b,c;
{
	char *resultname[4]={"最後一名","季軍","亞軍","冠軍"};
	char *gamename[5]={"  ","武鬥大會","藝術大展","皇家舞會","烹飪大賽"};
	int resultmoney[4]={0,3000,5000,8000};
	char name1[25],name2[25],name3[25],name4[25];
	char buf[256];
	
	if(winorlost==3)
	{
		strcpy(name1,d.name);
		strcpy(name2,resultmanlist[a].name);
		strcpy(name3,resultmanlist[b].name);
		strcpy(name4,resultmanlist[c].name);
	}
	else if(winorlost==2)
	{
		strcpy(name1,resultmanlist[a].name);
		strcpy(name2,d.name);
		strcpy(name3,resultmanlist[b].name);
		strcpy(name4,resultmanlist[c].name);
	}
	else if(winorlost==1)
	{
		strcpy(name1,resultmanlist[a].name);
		strcpy(name2,resultmanlist[b].name);
		strcpy(name3,d.name);
		strcpy(name4,resultmanlist[c].name);
	}
	else
	{
		strcpy(name1,resultmanlist[a].name);
		strcpy(name2,resultmanlist[b].name);
		strcpy(name3,resultmanlist[c].name);
		strcpy(name4,d.name);
	}	
	clear();
	move(6,13);
	prints("[1;37m∼∼∼ [32m本屆 %s 結果揭曉 [37m∼∼∼[0m",gamename[mode]);	
	move(8,15);
	prints("[1;41m 冠軍 [0;1m∼ [1;33m%-10s[36m  獎金 %d[0m",name1,resultmoney[3]);
	move(10,15);
	prints("[1;41m 亞軍 [0;1m∼ [1;33m%-10s[36m  獎金 %d[0m",name2,resultmoney[2]);
	move(12,15);
	prints("[1;41m 季軍 [0;1m∼ [1;33m%-10s[36m  獎金 %d[0m",name3,resultmoney[1]);
	move(14,15);
	prints("[1;41m 最後 [0;1m∼ [1;33m%-10s[36m [0m",name4);	
	sprintf(buf,"今年的%s結束囉 後年再來吧..",gamename[mode]);
	d.money+=resultmoney[winorlost];
	pressanykey(buf);
	return 0;
}
