/*---------------------------------------------------------------------------*/
/* 商店選單:食物 零食 大補丸 玩具 書本                                       */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include <time.h>
#include "bbs.h"
#include "pip.h"
extern struct chicken d;
extern time_t start_time;
extern time_t lasttime;

#ifndef MAPLE
extern char BoardName[];
#endif  // END MAPLE
#define getdata(a, b, c , d, e, f, g) getdata(a,b,c,d,e,f,NULL,g)

/*---------------------------------------------------------------------------*/
/* 商店選單:食物 零食 大補丸 玩具 書本                                       */
/* 資料庫                                                                    */
/*---------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*  物品參數設定                                                            */
/*--------------------------------------------------------------------------*/
struct goodsofpip
{
   int num;		/*編號*/
   char *name;		/*名字*/
   char *msgbuy;	/*功用*/
   char *msguse;	/*說明*/
   int money;		/*金錢*/
   int change;		/*改變量*/
   int pic1;
   int pic2;
};
typedef struct goodsofpip goodsofpip;

struct goodsofpip pipfoodlist[] = {
0,"物品名",	"說明buy",	"說明feed",			0,	0,	0,0,
1,"好吃的食物",	"體力恢復50",	"每吃一次食物會恢復體力50喔!",	50,	50,	1,1,
2,"美味的零食",	"體力恢復100",	"除了恢復體力，小雞也會更快樂",	120,	100,	2,3,
0,NULL,NULL,NULL,0,0,0,0
};

struct goodsofpip pipmedicinelist[] = {
0,"物品名",	"說明buy",	"說明feed",			0,	0,	0,0,
1,"好用大補丸",	"體力恢復600",	"恢復大量流失體力的良方",	500,	600,	4,4,
2,"珍貴的靈芝",	"法力恢復50",	"每吃一次靈芝會恢復法力50喔!",	100,	50,	7,7,
3,"千年人參王",	"法力恢復500",	"恢復大量流失法力的良方",	800,	500,	7,7,
4,"天山雪蓮",	"法力體力最大",	"這個  好貴......",		10000,	0,	7,7,
0,NULL,NULL,NULL,0,0,0,0
};

struct goodsofpip pipotherlist[] = {
0,"物品名",	"說明buy",	"說明feed",			0,	0,	0,0,
1,"樂高玩具組",	"快樂滿意度",	"玩具讓小雞更快樂啦...",	50,	0,	5,5,
2,"百科全書",	"知識的來源",	"書本讓小雞更聰明更有氣質啦...",100,	0,	6,6,
0,NULL,NULL,NULL,0,0,0,0
};

/*--------------------------------------------------------------------------*/
/*  武器參數設定                                                            */
/*--------------------------------------------------------------------------*/
struct weapon
{
  char *name;           /*名字*/  
  int needmaxhp;	/*需要hp*/
  int needmaxmp;	/*需要mp*/
  int needspeed;	/*需要的speed*/
  int attack;		/*攻擊*/
  int resist;		/*防護*/
  int speed;		/*速度*/
  int cost;		/*買價*/
  int sell;		/*賣價*/
  int special;		/*特別*/
  int map;		/*圖檔*/

};
typedef struct weapon weapon;

/*名字,需hp,需mp,需speed,攻擊,防護,速度,買價,賣價,特別,圖檔*/
struct weapon headlist[] = {
"不買裝備",  0,  0,  0,  0,  0,  0,     0,     0,0,0,
"塑膠帽子",  0,  0,  0,  0,  5,  0,   500,   300,0,0,	
"牛皮小帽",  0,  0,  0,  0, 10,  0,  3500,  1000,0,0,
"㊣安全帽", 60,  0,  0,  0, 20,  0,  5000,  3500,0,0,
"鋼鐵頭盔",150, 50,  0,  0, 30,  0, 10000,  6000,0,0,
"魔法髮箍",100,150,  0,  0, 25,  0, 50000, 10000,0,0, 
"黃金聖盔",300,300,300,  0,100,  0,300000,100000,0,0,
NULL,        0,  0,  0,  0,  0,  0,   0,   0,0,0
};

/*名字,需hp,需mp,需speed,攻擊,防護,速度,買價,賣價,特別,圖檔*/
struct weapon rhandlist[] = {
"不買裝備",  0,  0,  0,  0,  0,  0,     0,     0,0,0,
"大木棒",    0,  0,  0,  5,  0,  0,  1000,   700,0,0,	
"金屬扳手",  0,  0,  0, 10,  0,  0,  2500,  1000,0,0,
"青銅劍",   50,  0,  0, 20,  0,  0,  6000,  4000,0,0,
"晴雷劍",   80,  0,  0, 30,  0,  0, 10000,  8000,0,0,
"蟬翼刀",  100, 20,  0, 40,  0,  0, 15000, 10000,0,0, 
"忘情劍",  100, 40,  0, 35, 20,  0, 15000, 10000,0,0,
"獅頭寶刀",150,  0,  0, 60,  0,  0, 35000, 20000,0,0,
"屠龍刀",  200,  0,  0,100,  0,  0, 50000, 25000,0,0,
"黃金聖杖",300,300,300,100, 20,  0,150000,100000,0,0,
NULL,        0,  0,  0,  0,  0,  0,    0,   0,0,0
};

/*名字,需hp,需mp,需speed,攻擊,防護,速度,買價,賣價,特別,圖檔*/
struct weapon lhandlist[] = {
"不買裝備",  0,  0,  0,  0,  0,  0,     0,     0,0,0,
"大木棒",    0,  0,  0,  5,  0,  0,  1000,   700,0,0,	
"金屬扳手",  0,  0,  0, 10,  0,  0,  1500,  1000,0,0,
"木盾",	     0,  0,  0,  0, 10,  0,  2000,  1500,0,0,
"不鏽鋼盾", 60,  0,  0,  0, 25,  0,  5000,  3000,0,0,
"白金之盾", 80,  0,  0, 10, 40,  0, 15000, 10000,0,0,
"魔法盾",   80,100,  0, 20, 60,  0, 80000, 50000,0,0,
"黃金聖盾",300,300,300, 30,100,  0,150000,100000,0,0,
NULL,        0,  0,  0,  0,  0,  0,    0,   0,0,0
};

/*名字,需hp,需mp,需speed,攻擊,防護,速度,買價,賣價,特別,圖檔*/
struct weapon bodylist[] = {
"不買裝備",  0,  0,  0,  0,  0,  0,     0,     0,0,0,
"塑膠冑甲", 40,  0,  0,  0,  5,  0,  1000,   700,0,0,	
"特級皮甲", 50,  0,  0,  0, 10,  0,  2500,  1000,0,0,
"鋼鐵盔甲", 80,  0,  0,  0, 25,  0,  5000,  3500,0,0,
"魔法披風", 80, 40,  0,  0, 20, 20, 15500, 10000,0,0,
"白金盔甲",100, 30,  0,  0, 40, 20, 30000, 20000,0,0, 
"黃金聖衣",300,300,300, 30,100,  0,150000,100000,0,0,
NULL,        0,  0,  0,  0,  0,  0,     0,   0,0,0
};

/*名字,需hp,需mp,需speed,攻擊,防護,速度,買價,賣價,特別,圖檔*/
struct weapon footlist[] = {
"不買裝備",  0,  0,  0,  0,  0,  0,     0,     0,0,0,
"塑膠拖鞋",  0,  0,  0,  0,  0, 10,   800,   500,0,0,
"東洋木屐",  0,  0,  0, 15,  0, 10,  1000,   700,0,0, 	
"特級雨鞋",  0,  0,  0,  0, 10, 10,  1500,  1000,0,0,
"NIKE運動鞋",70, 0,  0,  0, 10, 40,  8000,  5000,0,0,
"鱷魚皮靴", 80, 20,  0, 10, 25, 20, 12000,  8000,0,0,
"飛天魔靴",100,100,  0, 30, 50, 60, 25000, 10000,0,0,
"黃金聖靴",300,300,300, 50,100,100,150000,100000,0,0,
NULL,        0,  0,  0,  0,  0,  0,    0,   0,0,0
};

/*---------------------------------------------------------------------------*/
/* 商店選單:食物 零食 大補丸 玩具 書本                                       */
/* 函式庫                                                                    */
/*---------------------------------------------------------------------------*/

int pip_store_food()
{
    int num[3];
    num[0]=2;
    num[1]=d.food;
    num[2]=d.cookie;
    pip_buy_goods_new(1,pipfoodlist,num);
    d.food=num[1];
    d.cookie=num[2];
    return 0;
}

int pip_store_medicine()
{
    int num[5];
    num[0]=4;
    num[1]=d.bighp;
    num[2]=d.medicine;
    num[3]=d.ginseng;
    num[4]=d.snowgrass;
    pip_buy_goods_new(2,pipmedicinelist,num);
    d.bighp=num[1];
    d.medicine=num[2];
    d.ginseng=num[3];
    d.snowgrass=num[4];
    return 0;
}

int pip_store_other()
{
    int num[3];
    num[0]=2;
    num[1]=d.playtool;
    num[2]=d.book;
    pip_buy_goods_new(3,pipotherlist,num);
    d.playtool=num[1];
    d.book=num[2];
    return 0;
}

int pip_store_weapon_head()	/*頭部武器*/
{
     d.weaponhead=pip_weapon_doing_menu(d.weaponhead,0,headlist);
     return 0; 
}
int pip_store_weapon_rhand()	/*右手武器*/
{
     d.weaponrhand=pip_weapon_doing_menu(d.weaponrhand,1,rhandlist);
     return 0;
}
int pip_store_weapon_lhand()    /*左手武器*/
{
     d.weaponlhand=pip_weapon_doing_menu(d.weaponlhand,2,lhandlist);
     return 0;
}
int pip_store_weapon_body()	/*身體武器*/
{
     d.weaponbody=pip_weapon_doing_menu(d.weaponbody,3,bodylist);
     return 0;
}
int pip_store_weapon_foot()     /*足部武器*/
{
     d.weaponfoot=pip_weapon_doing_menu(d.weaponfoot,4,footlist);
     return 0;
}


int 
pip_buy_goods_new(mode,p,oldnum)
int mode;
int oldnum[];
struct goodsofpip *p;
{
    char *shopname[4]={"店名","便利商店","星空藥鋪","夜裡書局"};
    char inbuf[256];
    char genbuf[20];
    long smoney;
    int oldmoney;
    int i,pipkey,choice;
    oldmoney=d.money;
    do
    {
	    clrchyiuan(6,18);
	    move(6,0);
	    sprintf(inbuf,"[1;31m  ─[41;37m 編號 [0;1;31m─[41;37m 商      品 [0;1;31m──[41;37m 效            能 [0;1;31m──[41;37m 價     格 [0;1;31m─[37;41m 擁有數量 [0;1;31m─[0m  ");
	    prints(inbuf);
	    for(i=1;i<=oldnum[0];i++)
	    {
		    move(7+i,0);
		    sprintf(inbuf,"     [1;35m[[37m%2d[35m]     [36m%-10s      [37m%-14s        [1;33m%-10d   [1;32m%-9d    [0m",
		    p[i].num,p[i].name,p[i].msgbuy,p[i].money,oldnum[i]);
		    prints(inbuf);
	    }
	    clrchyiuan(19,24);
	    move(b_lines,0); 
	    sprintf(inbuf,"[1;44;37m  %8s選單  [46m  [B]買入物品  [S]賣出物品  [Q]跳出：                         [m",shopname[mode]);
	    prints(inbuf);
	    pipkey=egetch(); 
	    switch(pipkey)  
	    {
		case 'B':
		case 'b':      
			move(b_lines-1,1);
			sprintf(inbuf,"想要買入啥呢? [0]放棄買入 [1∼%d]物品商號",oldnum[0]);
#ifdef MAPLE
			getdata(b_lines-1,1,inbuf,genbuf, 3, LCECHO,"0");
#else
                        getdata(b_lines-1,1,inbuf,genbuf, 3, DOECHO,YEA);
                        if ((genbuf[0] >= 'A') && (genbuf[0] <= 'Z'))
                                genbuf[0] = genbuf[0] | 32;
#endif  // END MAPLE

     			choice=atoi(genbuf);
			if(choice>=1 && choice<=oldnum[0])
			{
				clrchyiuan(6,18);
				if(rand()%2>0)
					show_buy_pic(p[choice].pic1);
				else
					show_buy_pic(p[choice].pic2);
				move(b_lines-1,0);
				clrtoeol();  
				move(b_lines-1,1);       
				smoney=0;
				if(mode==3)
					smoney=1;
				else
				{
					sprintf(inbuf,"你要買入物品 [%s] 多少個呢?(上限 %d)",p[choice].name,d.money/p[choice].money);
#ifdef MAPLE
					getdata(b_lines-1,1,inbuf,genbuf,6, 1, 0);
#else
                                        getdata(b_lines-1,1,inbuf,genbuf,6, DOECHO, YEA);
#endif  // END MAPLE
					smoney=atoi(genbuf);
				}
				if(smoney<0)
				{
					pressanykey("放棄買入...");
				}
				else if(d.money<smoney*p[choice].money)
				{
					pressanykey("你的錢沒有那麼多喔..");
				}
				else
				{
					sprintf(inbuf,"確定買入物品 [%s] 數量 %d 個嗎?(店家賣價 %d) [y/N]:",p[choice].name,smoney,smoney*p[choice].money);
#ifdef MAPLE
					getdata(b_lines-1,1,inbuf,genbuf, 2, 1, 0); 
#else
                                        getdata(b_lines-1,1,inbuf,genbuf, 2, DOECHO, YEA);
#endif  // END MAPLE
					if(genbuf[0]=='y' || genbuf[0]=='Y')
					{
						oldnum[choice]+=smoney;
						d.money-=smoney*p[choice].money;
						sprintf(inbuf,"老闆給了你%d個%s",smoney,p[choice].name);
						pressanykey(inbuf);
						pressanykey(p[choice].msguse);
						if(mode==3 && choice==1)
						{
							d.happy+=rand()%10+20*smoney;
							d.satisfy+=rand()%10+20*smoney;
						}
						if(mode==3 && choice==2)
						{
							d.happy+=(rand()%2+2)*smoney;
							d.wisdom+=(2+10/(d.wisdom/100+1))*smoney;
							d.character+=(rand()%4+2)*smoney;
							d.art+=(rand()%2+1)*smoney;
						}
					}
					else
					{
						pressanykey("放棄買入...");
					}
				}
			}
			else
			{
				sprintf(inbuf,"放棄買入.....");
				pressanykey(inbuf);            
			}
			break;     
     
 		case 'S':
 		case 's':
 			if(mode==3)
 			{
 				pressanykey("這些東西不能賣喔....");
 				break;
 			}
			move(b_lines-1,1);
			sprintf(inbuf,"想要賣出啥呢? [0]放棄賣出 [1∼%d]物品商號",oldnum[0]);
#ifdef MAPLE
			getdata(b_lines-1,1,inbuf,genbuf, 3, LCECHO,"0");
#else
                        getdata(b_lines-1,1,inbuf,genbuf, 3, DOECHO,YEA);
                        if ((genbuf[0] >= 'A') && (genbuf[0] <= 'Z'))
                                genbuf[0] = genbuf[0] | 32;
#endif  // END MAPLE
     			choice=atoi(genbuf);
			if(choice>=1 && choice<=oldnum[0])
			{
				clrchyiuan(6,18);
				if(rand()%2>0)
					show_buy_pic(p[choice].pic1);
				else
					show_buy_pic(p[choice].pic2);
				move(b_lines-1,0);
				clrtoeol();  
				move(b_lines-1,1);       
				smoney=0;
				sprintf(inbuf,"你要賣出物品 [%s] 多少個呢?(上限 %d)",p[choice].name,oldnum[choice]);
#ifdef MAPLE
				getdata(b_lines-1,1,inbuf,genbuf,6, 1, 0); 
#else
                                getdata(b_lines-1,1,inbuf,genbuf,6, DOECHO, YEA);
#endif  // END MAPLE
				smoney=atoi(genbuf);
				if(smoney<0)
				{
					pressanykey("放棄賣出...");
				}
				else if(smoney>oldnum[choice])
				{
					sprintf(inbuf,"你的 [%s] 沒有那麼多個喔",p[choice].name);
					pressanykey(inbuf);
				}
				else
				{
					sprintf(inbuf,"確定賣出物品 [%s] 數量 %d 個嗎?(店家買價 %d) [y/N]:",p[choice].name,smoney,smoney*p[choice].money*8/10);
#ifdef MAPLE
					getdata(b_lines-1,1,inbuf,genbuf, 2, 1, 0);
#else
                                        getdata(b_lines-1,1,inbuf,genbuf, 2, DOECHO, YEA);
#endif  // END MAPLE
					if(genbuf[0]=='y' || genbuf[0]=='Y')
					{
						oldnum[choice]-=smoney;
						d.money+=smoney*p[choice].money*8/10;
						sprintf(inbuf,"老闆拿走了你的%d個%s",smoney,p[choice].name);
						pressanykey(inbuf);
					}
					else
					{
						pressanykey("放棄賣出...");
					}
				}
			}
			else
			{
				sprintf(inbuf,"放棄賣出.....");
				pressanykey(inbuf);            
			}
			break;
		case 'Q':
		case 'q':
			sprintf(inbuf,"金錢交易共 %d 元,離開 %s ",d.money-oldmoney,shopname[mode]);
			pressanykey(inbuf);
			break;
#ifdef MAPLE
		case Ctrl('R'):
			if (currutmp->msgs[0].last_pid)
			{
				show_last_call_in();
				my_write(currutmp->msgs[0].last_pid, "水球丟回去：");
			}
			break;
#endif  // END MAPLE
	    }
  }while((pipkey!='Q')&&(pipkey!='q')&&(pipkey!=KEY_LEFT));    
  return 0;
}

int
pip_weapon_doing_menu(variance,type,p)               /* 武器購買畫面 */
int variance;
int type;
struct weapon *p;
{
  time_t now;
  register int n=0;
  register char *s;
  char buf[256];
  char ans[5];
  char shortbuf[100];
  char menutitle[5][11]={"頭部裝備區","右手裝備區","左手裝備區","身體裝備區","足部裝備區"};  
  int pipkey;
  char choicekey[5];
  int choice;
  
  do{
   clear();
   showtitle(menutitle[type], BoardName);
   show_weapon_pic(0);
/*   move(10,2); 
   sprintf(buf,"[1;37m現今能力:體力Max:[36m%-5d[37m  法力Max:[36m%-5d[37m  攻擊:[36m%-5d[37m  防禦:[36m%-5d[37m  速度:[36m%-5d [m",
           d.maxhp,d.maxmp,d.attack,d.resist,d.speed);
   prints(buf);*/
   move(11,2);
   sprintf(buf,"[1;37;41m [NO]  [器具名]  [體力]  [法力]  [速度]  [攻擊]  [防禦]  [速度]  [售  價] [m");
   prints(buf);
   move(12,2);
   sprintf(buf," [1;31m──[37m白色 可以購買[31m──[32m綠色 擁有裝備[31m──[33m黃色 錢錢不夠[31m──[35m紫色 能力不足[31m──[m");
   prints(buf); 

   n=0;
   while (s = p[n].name)
   {   
     move(13+n,2);
     if(variance!=0 && variance==(n))/*本身有的*/
     {
      sprintf(buf, 
      "[1;32m (%2d)  %-10s %4d    %4d    %4d    %4d    %4d    %4d    %6d[m",     
      n,p[n].name,p[n].needmaxhp,p[n].needmaxmp,p[n].needspeed,
      p[n].attack,p[n].resist,p[n].speed,p[n].cost);        
     }
     else if(d.maxhp < p[n].needmaxhp || d.maxmp < p[n].needmaxmp || d.speed < p[n].needspeed )/*能力不足*/
     {
      sprintf(buf, 
      "[1;35m (%2d)  %-10s %4d    %4d    %4d    %4d    %4d    %4d    %6d[m",
      n,p[n].name,p[n].needmaxhp,p[n].needmaxmp,p[n].needspeed,
      p[n].attack,p[n].resist,p[n].speed,p[n].cost);
     }

     else if(d.money < p[n].cost)  /*錢不夠的*/
     {
      sprintf(buf, 
      "[1;33m (%2d)  %-10s %4d    %4d    %4d    %4d    %4d    %4d    %6d[m",     
      n,p[n].name,p[n].needmaxhp,p[n].needmaxmp,p[n].needspeed,
      p[n].attack,p[n].resist,p[n].speed,p[n].cost);    
     }    
     else
     {
      sprintf(buf, 
      "[1;37m (%2d)  %-10s %4d    %4d    %4d    %4d    %4d    %4d    %6d[m",     
      n,p[n].name,p[n].needmaxhp,p[n].needmaxmp,p[n].needspeed,
      p[n].attack,p[n].resist,p[n].speed,p[n].cost);        
     } 
     prints(buf);
     n++;
   }
   move(b_lines,0); 
   sprintf(buf,"[1;44;37m  武器購買選單  [46m  [B]購買武器  [S]賣掉裝備  [W]個人資料  [Q]跳出：            [m");
   prints(buf);   
   now=time(0);
   pip_time_change(now);
   pipkey=egetch();
   pip_time_change(now);

   switch(pipkey)  
   {
    case 'B':      
    case 'b':
     move(b_lines-1,1);
     sprintf(shortbuf,"想要購買啥呢? 你的錢錢[%d]元:[數字]",d.money);
     prints(shortbuf);
#ifdef MAPLE
     getdata(b_lines-1,1,shortbuf,choicekey, 4, LCECHO,"0");
#else
     getdata(b_lines-1,1,shortbuf,choicekey, 4, DOECHO,YEA);
     if ((choicekey[0] >= 'A') && (choicekey[0] <= 'Z'))
             choicekey[0] = choicekey[0] | 32;
#endif  // END MAPLE
     choice=atoi(choicekey);
     if(choice>=0 && choice<=n)
     {
       move(b_lines-1,0);
       clrtoeol();  
       move(b_lines-1,1);       
       if(choice==0)     /*解除*/
       { 
          sprintf(shortbuf,"放棄購買...");
          pressanykey(shortbuf);
       }
      
       else if(variance==choice)  /*早已經有啦*/
       {
          sprintf(shortbuf,"你早已經有 %s 囉",p[variance].name);
          pressanykey(shortbuf);      
       }
      
       else if(p[choice].cost >= (d.money+p[variance].sell))  /*錢不夠*/
       {
          sprintf(shortbuf,"這個要 %d 元，你的錢不夠啦!",p[choice].cost);
          pressanykey(shortbuf);      
       }      
     
       else if(d.maxhp < p[choice].needmaxhp || d.maxmp < p[choice].needmaxmp 
               || d.speed < p[choice].needspeed ) /*能力不足*/
       {
          sprintf(shortbuf,"需要HP %d MP %d SPEED %d 喔",
                p[choice].needmaxhp,p[choice].needmaxmp,p[choice].needspeed);
          pressanykey(shortbuf);            
       }
       else  /*順利購買*/
       {
          sprintf(shortbuf,"你確定要購買 %s 嗎?($%d) [y/N]",p[choice].name,p[choice].cost);
#ifdef MAPLE
          getdata(b_lines-1,1,shortbuf, ans, 2, 1, 0); 
#else
          getdata(b_lines-1,1,shortbuf, ans, 2, DOECHO, YEA);
#endif  // END MAPLE
          if(ans[0]=='y' || ans[0]=='Y')
          {
              sprintf(shortbuf,"小雞已經裝配上 %s 了",p[choice].name);
              pressanykey(shortbuf);
              d.attack+=(p[choice].attack-p[variance].attack);
              d.resist+=(p[choice].resist-p[variance].resist);
              d.speed+=(p[choice].speed-p[variance].speed);
              d.money-=(p[choice].cost-p[variance].sell);
              variance=choice;
          }
          else
          {
              sprintf(shortbuf,"放棄購買.....");
              pressanykey(shortbuf);            
          }
       }
     }       
     break;     
     
   case 'S':
   case 's':
     if(variance!=0)
     { 
        sprintf(shortbuf,"你確定要賣掉%s嗎? 賣價:%d [y/N]",p[variance].name,p[variance].sell);
#ifdef MAPLE
        getdata(b_lines-1,1,shortbuf, ans, 2, 1, 0); 
#else
        getdata(b_lines-1,1,shortbuf, ans, 2, DOECHO, YEA);
#endif  // END MAPLE
        if(ans[0]=='y' || ans[0]=='Y')
        {
           sprintf(shortbuf,"裝備 %s 賣了 %d",p[variance].name,p[variance].sell);
           d.attack-=p[variance].attack;
           d.resist-=p[variance].resist;
           d.speed-=p[variance].speed;
           d.money+=p[variance].sell;
           pressanykey(shortbuf);
           variance=0;
        }
        else
        {
           sprintf(shortbuf,"ccc..我回心轉意了...");
           pressanykey(shortbuf);         
        }
     }
     else if(variance==0)
     {
        sprintf(shortbuf,"你本來就沒有裝備了...");
        pressanykey(shortbuf);
        variance=0;
     }
     break;
                      
   case 'W':
   case 'w':
     pip_data_list();   
     break;    
   
#ifdef MAPLE
   case Ctrl('R'):
     if (currutmp->msgs[0].last_pid)
     {
       show_last_call_in();
       my_write(currutmp->msgs[0].last_pid, "水球丟回去：");
     }
     break;
#endif  // END MAPLE
   }
  }while((pipkey!='Q')&&(pipkey!='q')&&(pipkey!=KEY_LEFT));
    
  return variance;
}
