/*--------------------------------------------------------------*/
/* 糖果屋賽豬場					 98'12/17	*/
/*--------------------------------------------------------------*/
/* 本程式由小弟我(SugarII)					*/
/* idea取自於 "盈月與繁星" 並參考 badboy 的 source 重新改寫出來 */
/* 的，寫得有點爛，歡迎大家測試並修改，如有任何問題，或任何想法 */
/* 、建議都希望能通知我一聲。	EMail:u861838@Oz.nthu.edu.tw	*/
/*--------------------------------------------------------------*/

#include "bbs.h"

int p[5],i;

int
race_path(int run,int j,int step)
{
  if(step==0) return -1;
  else if(step<0)
  {
    if(j+step<0) j=-step;
    move(run+9,(j+step)*2+8);
    clrtoeol();
    p[run]+=step*100;
    if(p[run]<1) p[run]=1;
    return -1;
  }
  move(run+9,j*2+8);
  prints("[1;3%dm",run+1);
  for(i=0;i<step;i++) prints("■");
  prints("[0m");
  if(p[run]+step*100>3000) return run;
  else return -1;
}

int
p_race()
{
  char bet[1],m1[8],ch,*racename[5]={"糖糖","ㄚ輝","強哥","凱薰","韻怡"};
  int rate[5],flag,stop[5],run,win,ball,money[6],save_pager;
  time_t now;

  setutmpmode(RACE);
  save_pager=currutmp->pager;
  clear();
  more("game/race/race.welcome");
  currutmp->pager =2;
    while(-1)
    {
    	ball=win=-1;flag=0;for(i=0;i<5;i++){p[i]=1;stop[i]=money[i+1]=0;rate[i]=100;}
	clear();
	move(2,0);
	outs("  [1m豬名：[m    ");
	for(i=0;i<5;i++)
	    prints(" %d. [1;3%dm%s[0m ",i+1,i+1,racename[i]);
	outs("\n  [1m速度：[m    \n  [1m賭金：[m    \n");
	outs("（　請按 [1;36mk[m 為你選的小豬加油，按 [1;36mz[m 可丟出保齡球(只有一次機會)　）");
	move(9,0);
	for(i=0;i<5;i++)
	    prints("%d.[1;3%dm%s[m?鱋n",i+1,i+1,racename[i]);
	outs("───?髍w─┴──┴──┴──┴──┴──┴──┴──┴──┴──??");
	do
	{
	    do{
		getdata(0,0,"你要押哪隻(1~5)[S]開始[Q]離開：",bet,2,DOECHO,0);
		i=atoi(bet);
		if(i>0&&i<6)
		{
		    getdata(1,0,"要押多少賭金(1~10000)? 按 Enter 取消：",m1,6,1,0);
		    if(atoi(m1)>cuser.silvermoney)
			pressanykey("你的現金不夠啦!!~~:)");
		    else if(money[i]+atoi(m1)>=0&&money[i]+atoi(m1)<=10000)
		    {
			 money[i]+=atoi(m1);
			 demoney(atoi(m1));
		    }
		    move(1,0);clrtoeol();
		}
		move(4,12);clrtoeol();
		for(i=1;i<6;i++)
		    prints("   [1;3%dm%5d[m ",i,money[i]);
	    } while( !( ((bet[0]=='s'||bet[0]=='S')&&money[1]+money[2]+money[3]+money[4]+money[5]>0)||bet[0]=='q'||bet[0]=='Q') );
	    if(bet[0]=='q'||bet[0]=='Q')
	    {
		if(money[1]+money[2]+money[3]+money[4]+money[5]>0)
		    for(i=1;i<6;i++)
			inmoney(money[i]);
	    	currutmp->pager = save_pager;
		return 0;
	    }
	} while(bet[0]!='s'&&bet[0]!='S');
	while(win==-1)
	{
	    move(3,12);clrtoeol();
	    for(i=0;i<5;i++)
	    {
		now=random();
		if(stop[i]<1)
		    rate[i]+=now%20-(rate[i]+170)/30;
		if(rate[i]<0) rate[i]=0;
		prints("    [1;3%dm%4d[m ",i+1,rate[i]);
	    }
	    do ch=igetch();
	    while(!((ch=='k'||ch=='K')||((ch=='z'||ch=='Z')&&ball==-1)));
            if(ch=='z'||ch=='Z')
            {
                now=random();
                run=now%5;
                stop[run]=3;
                if(flag<6)      move(15+flag,0);
                else            move(9+flag,42);
                prints("[1m保齡球砸到[3%dm%s[37m停止前進三次，速度=0[m",run+1,racename[run]);
                rate[run]=0;flag++;ball=-2;
            }
            else
            {
		now=random();
		run=now%5;
		now=random();
		if( now%12==0 && (flag<11||(flag<12&&ball==-2)) )
		{
		    if(flag<6)  move(15+flag,0);
		    else        move(9+flag,42);
		    prints("[1;3%dm%s[36m",run+1,racename[run]);
		    now=random();
		    switch(now%14)
		    {
			case 0:
			    prints("服下威而剛，速度x1.5[m");
			    rate[run]*=1.5;
			    break;
			case 1:
			    prints("使出熊的爆發力，前進五格[m");
			    win=race_path(run,p[run]/100,5);
			    p[run]+=500;
			    break;
			case 2:
			    prints("踩到地雷，速度減半[m");
			    rate[run]/=2;
			    break;
			case 3:
			    prints("踩到香蕉皮滑倒，暫停二次[m");
			    stop[run]+=2;
			    break;			
			case 4:
			    prints("請神上身，暫停四次，速度加倍[m");
			    stop[run]+=4;rate[run]*=2;
			    break;
			case 5:
			    if(p[0]+p[1]+p[2]+p[3]+p[4]>8000)
			    {
				prints("由於 SugarII 作弊，直達終點[m");
				win=race_path(run,p[run]/100,(3100-p[run])/100);
				p[run]=3001;
			    }
			    else
			    {
				prints("唱出大魔法咒，使其他人暫停三次[m");
				for(i=0;i<5&&i!=run;i++)
				stop[i]+=3;
			    }
			    break;
			case 6:
			    prints("聽見 badboy 的加油聲，速度+100[m");
			    rate[run]+=100;
			    break;
			case 7:
			    prints("使出鋼鐵變身，前進三格，速度+30[m");
			    win=race_path(run,p[run]/100,3);
			    rate[run]+=30;
			    break;
			case 8:
			    prints("衰神上身速度減半，旁邊暫停二次[m");
			    rate[run]/=2;
			    if(run>0) stop[run-1]+=2;
			    if(run<4) stop[run+1]+=2;
			    break;
			case 9:
			    prints("被 SugarII 詛咒，回到起點[m");
			    win=race_path(run,p[run]/100,-30);
			    break;
			case 10:
			    if(p[0]+p[1]+p[2]+p[3]+p[4]>6000)
			    {
				prints("[5m使出凱化薰陶，所有人回到起點[m");
				for(i=0;i<5;i++)
				win=race_path(i,p[i]/100,-30);
			    }
			    else
			    {
				prints("使出企鵝彈跳，速度x1.3，其他人減半[m");
				for(i=0;i<5&&i!=run;i++) rate[i]/=2;
				rate[run]*=1.3;
			    }
			    break;
			case 11:
			    if(money[run+1]>0)
			    {
				prints("撿到一千元，暫停一次[m");
				inmoney(1000);
				stop[run]+=1;
			    }
			    else
			    {
				prints("整隻豬爽起來了，速度+50[m");
				rate[run]+=50;
			    }
			    break;
			case 12:
			    while(run==now%5)
				now=random();
			    prints("愛上了[3%dm%s[36m，速度跟牠一樣",now%5+1,racename[now%5]);
			    rate[run]=rate[now%5];
			    break;
			case 13:
			    if(money[run+1]>0)
			    {
				prints("的賭金x1.5，賺啦!!~[m");
				money[run+1]*=1.5;
				move(4,12);clrtoeol();
				for(i=1;i<6;i++)
				    prints("   [1;3%dm%5d[m ",i,money[i]);
			    }
			    else
			    {
				prints("鞋子掉了，退後三格[m");
				race_path(run,p[run]/100,-3);
			    }
			    break;
		    }
		    flag++;
		}
		else
		{
		    move(7,2);
		    clrtoeol();
		    if(stop[run]>0)
		    {
			prints("[1;3%dm%s[37m 爬不起來[m",run+1,racename[run]);
			stop[run]--;
		    }
		    else
		    {
			prints("[1;3%dm%s[37m 拚命奔跑[m",run+1,racename[run]);
			i=p[run]/100;
			win=race_path(run,i,(p[run]+rate[run])/100-i);
			p[run]+=rate[run];
		    }
		}
	    }
	}
	move(22,0);
	prints("  [1;35m★[37m遊戲結束[35m★[37m  獲勝的是[3%dm%s    [37m",win+1,racename[win]);
	if(money[win+1]>0) 
	{
	    money[win+1]+=money[win+1]*(p[win]-(p[0]+p[1]+p[2]+p[3]+p[4])/5)/500;
	    prints("恭喜你押中了，獲得獎金 [32m%d[37m 元[m",money[win+1]);
	    inmoney(money[win+1]);
	    game_log(RACE,"[1;33m壓中了 %s 賺了 %d 元",racename[win],money[win+1]);
	}
	else
	{
	    outs("抱歉...你沒押中ㄛ~~~[m");
	    game_log(RACE,"[1;31m一隻都沒有壓中啦!!");
	}
	pressanykey(NULL);
    }
}
