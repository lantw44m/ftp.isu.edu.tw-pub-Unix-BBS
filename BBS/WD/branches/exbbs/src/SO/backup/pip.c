/* ----------------------------------- */
/* pip.c  養小雞程式                   */
/* 原作者: dsyan   改寫者: fennet      */
/* 圖圖 by tiball.bbs@bbs.nhctc.edu.tw */
/* ----------------------------------- */
#define ba rpguser.race == 6 ? 10*rpguser.level : 5

#include "bbs.h"
#include <time.h>
#include "pip.h"
struct chicken d, other;
time_t start_time;
time_t lasttime;

/*遊戲主程式*/
int p_pipple()
{
 FILE *fs;
 int pipkey;
 char genbuf[200];

 setutmpmode(CHICKEN);
 more("src/maple/pipgame/pip.welcome",YEA);
 showtitle("電子養小雞", BoardName);
 srandom(time(0));
 sprintf(genbuf,"home/%s/chicken",cuser.userid);
 
 pip_read_file(cuser.userid);
 if((fs=fopen(genbuf, "r")) == NULL)
 {
   show_system_pic(11);
   move(b_lines,0);
   pipkey=egetch();   
   if(pipkey=='Q' || pipkey=='q')
     return 0;
   if(d.death!=0 || !d.name[0])
   {
       pip_new_game();
   }      
 }
 else
 {
   show_system_pic(12);
   move(b_lines,0);
   pipkey=egetch();   
   if(pipkey=='R' || pipkey=='r')
     pip_read_backup();
   else if(pipkey=='Q' || pipkey=='q')
     return 0;
   if(d.death!=0 || !d.name[0])
     {
       pip_new_game();
     }
 }
 
 lasttime=time(0);
 start_time=time(0);
 /*pip_do_menu(0,0,pipmainlist);*/
 if(d.death!=0 || !d.name[0])  return 0;
 pip_main_menu();
 d.bbtime+=time(0)-start_time;
 pip_write_file();
 return 0;
}

/*時間表示法*/
char*
dsyan_time(const time_t *t)
{
  struct tm *tp;
  static char ans[9];

  tp = localtime(t);
  sprintf(ans, "%02d/%02d/%02d", tp->tm_year % 100, tp->tm_mon + 1,tp->tm_mday);
  return ans;
}

/*新遊戲的設定*/
int
pip_new_game()
{
  char buf[256];
  time_t now;
  char *pipsex[3]={"？","♂","♀"};
  struct tm *ptime;
  ptime = localtime(&now);
  
  if(d.death==1 && !(!d.name[0]))
  {
     clear();
     showtitle("外星戰鬥雞", BoardName); 
     move(4,6);
     prints("歡迎來到 [1;5;33m星空生物科技研究院[0m");
     move(6,6);
     prints("經我們調查顯示  先前你有養過小雞喔  可是被你養死了...");
     move(8,6);
     if(d.liveagain<4)
     {
       prints("我們可以幫你幫小雞復活  但是需要付出一點代價");
       getdata(10, 6, "你要我們讓他重生嗎? [y/N]:", buf, 2, 1, 0);
       if(buf[0]=='y' || buf[0]=='Y')
       {
         pip_live_again();
       }
     }
     else if(d.liveagain>=4)
     {
       prints("可是你復活手術太多次了  小雞身上都是開刀痕跡");     
       move(10,6);
       prints("我們找不到可以手術的地方了  所以....");
       pressanykey("重新再來吧....唉....");    
     }
  }
  if(d.death!=0 || !d.name[0])
  {
    clear();
    showtitle("外星戰鬥雞", BoardName);   
    /*小雞命名*/
    getdata(2, 3, "幫小雞取個好聽的名字吧(請不要有空格):", buf, 11, 1, 0);
    if(!buf[0]) return 0;
    strcpy(d.name,buf);
    /*1:公 2:母 */
    getdata(4, 3, "[Boy]小公雞♂ or [Girl]小母雞♀ [b/G]", buf, 2, 1, 0);  
    if(buf[0]=='b' || buf[0]=='B')
    {
      d.sex=1;
    }  
    else
    {
      d.sex=2; 
    }        
    move(6,3);
    prints("星空戰鬥雞的遊戲現今分成兩種玩法");
    move(7,3);
    prints("選有結局會在小雞20歲時結束遊戲，並告知小雞後續的發展");
    move(8,3);
    prints("選沒有結局則一直養到小雞死亡才結束遊戲....");
    /*1:不要且未婚 4:要且未婚 */
    getdata(9, 3, "你希望小雞遊戲是否要有20歲結局? [Y/n]", buf, 2, 1, 0);
    if(buf[0]=='n' || buf[0]=='N')
    {
      d.wantend=1;
    }  
    else
    {
      d.wantend=4; 
    }        
    /*開頭畫面*/
    show_basic_pic(0);
    pressanykey("小雞終於誕生了，請好好愛他....");

    /*開頭設定*/
    now=time(0);
    strcpy(d.birth,dsyan_time(&now));
    d.bbtime=0;

    /*基本資料*/
    d.year=ptime->tm_year%100;
    d.month=ptime->tm_mon + 1;
    d.day=ptime->tm_mday;
    d.death=d.nodone=d.relation=0;
    d.liveagain=d.dataB=d.dataC=d.dataD=d.dataE=0;
          
    /*身體參數*/
    d.hp=rand()%15+20;
    d.maxhp=rand()%20+20;
    if(d.hp>d.maxhp) d.hp=d.maxhp;
    d.weight=rand()%10+50;
    d.tired=d.sick=d.shit=d.wrist=0;
    d.bodyA=d.bodyB=d.bodyC=d.bodyD=d.bodyE=0;
  
    /*評價參數*/
    d.social=d.family=d.hexp=d.mexp=0;
    d.tmpA=d.tmpB=d.tmpC=d.tmpD=d.tmpE=0;
         
    /*戰鬥參數*/
    d.mp=d.maxmp=d.attack=d.resist=d.speed=d.hskill=d.mskill=d.mresist=0;
    d.magicmode=d.fightB=d.fightC=d.fightD=d.fightE=0;
  
    /*武器參數*/
    d.weaponhead=d.weaponrhand=d.weaponlhand=d.weaponbody=d.weaponfoot=0;
    d.weaponA=d.weaponB=d.weaponC=d.weaponD=d.weaponE=0;
    
    /*能力參數*/
    d.toman=d.character=d.love=d.wisdom=d.art=d.etchics=0;
    d.brave=d.homework=d.charm=d.manners=d.speech=d.cookskill=0;
    d.learnA=d.learnB=d.learnC=d.learnD=d.learnE=0;
  
    /*狀態數值*/
    d.happy=rand()%10+20;
    d.satisfy=rand()%10+20;
    d.fallinlove=d.belief=d.offense=d.affect=0;
    d.stateA=d.stateB=d.stateC=d.stateD=d.stateE=0;

    /*食物參數:食物 零食 藥品 大補丸*/
    d.food=10;
    d.medicine=d.cookie=d.bighp=2;
    d.ginseng=d.snowgrass=d.eatC=d.eatD=d.eatE=0;

    /*物品參數:書 玩具*/
    d.book=d.playtool=0;
    d.money=1500;
    d.thingA=d.thingB=d.thingC=d.thingD=d.thingE=0;

    /*猜拳參數:贏 負*/
    d.winn=d.losee=0;

    /*參見王臣*/
    d.royalA=d.royalB=d.royalC=d.royalD=d.royalE=0;
    d.royalF=d.royalG=d.royalH=d.royalI=d.royalJ=0;
    d.seeroyalJ=1;
    d.seeA=d.seeB=d.seeC=d.seeD=d.seeE;
    /*接受求婚愛人*/        
    d.lover=0;
    /*0:沒有 1:魔王 2:龍族 3:A 4:B 5:C 6:D 7:E */
    d.classA=d.classB=d.classC=d.classD=d.classE=0;
    d.classF=d.classG=d.classH=d.classI=d.classJ=0;
    d.classK=d.classL=d.classM=d.classN=d.classO=0;

    d.workA=d.workB=d.workC=d.workD=d.workE=0;
    d.workF=d.workG=d.workH=d.workI=d.workJ=0;
    d.workK=d.workL=d.workM=d.workN=d.workO=0;
    d.workP=d.workQ=d.workR=d.workS=d.workT=0;
    d.workU=d.workV=d.workW=d.workX=d.workY=d.workZ=0;
    /*養雞記錄*/
    now=time(0);
    sprintf(buf, "[1;36m%s %-11s養了一隻叫 [%s] 的 %s 小雞 [0m\n", Cdate(&now), cuser.userid,d.name,pipsex[d.sex]);
    pip_log_record(buf);
  }  
  pip_write_file();
}

/*小雞死亡函式*/
pipdie(msg,mode)
char *msg;
int mode;
{
 char genbuf[200];
 time_t now;
 clear();
 showtitle("電子養小雞", BoardName); 
 if(mode==1)
 {
   show_die_pic(1);
   pressanykey("死神來帶走小雞了");
   clear();
   showtitle("電子養小雞", BoardName); 
   show_die_pic(2);
   move(14,20);
   prints("可憐的小雞[1;31m%s[m",msg);
   pressanykey("星空哀悼中....");
 }
 else if(mode==2)
 {
   show_die_pic(3);
   pressanykey("嗚嗚嗚..我被丟棄了.....");
 } 
 else if(mode==3)
 {
   show_die_pic(0);
   pressanykey("遊戲結束囉.."); 
 }
 
 now=time(0);
 sprintf(genbuf, "[1;31m%s %-11s的小雞 [%s] %s[m\n", Cdate(&now), cuser.userid,d.name, msg);
 pip_log_record(genbuf);
 pip_write_file();
}


/*pro:機率 base:底數 mode:類型 mul:加權100=1 cal:加減*/
int
count_tired(prob,base,mode,mul,cal)
int prob,base;
char *mode;
int mul;
int cal;
{
 int tiredvary=0;
 int tm;
 /*time_t now;*/
 tm=(time(0)-start_time+d.bbtime)/60/30;
 if(!strcmp(mode,"Y"))
 {
  if(tm>=0 && tm <=3)
  {
     if(cal==1)
        tiredvary=(rand()%prob+base)*d.maxhp/(d.hp+0.8*d.hp)*120/100;
     else if(cal==0)
        tiredvary=(rand()%prob+base)*4/3;
  }
  else if(tm >=4 && tm <=7)
  {
     if(cal==1)
        tiredvary=(rand()%prob+base)*d.maxhp/(d.hp+0.8*d.hp);
     else if(cal==0)
        tiredvary=(rand()%prob+base)*3/2;
  }
  else if(tm >=8 && tm <=10)
  {
     if(cal==1)
        tiredvary=(rand()%prob+base)*d.maxhp/(d.hp+0.8*d.hp)*110/100;
     else if(cal==0)
        tiredvary=(rand()%prob+base)*5/4;
  }
  else if(tm >=11)
  {
     if(cal==1)
        tiredvary=(rand()%prob+base)*d.maxhp/(d.hp+0.8*d.hp)*150/100;
     else if(cal==0)
        tiredvary=(rand()%prob+base)*1;
  }
 }
 else if(!strcmp(mode,"N"))
 {
  tiredvary=rand()%prob+base;
 }

 if(cal==1)
 {
   d.tired+=(tiredvary*mul/100);
   if(d.tired>100)
     d.tired=100;
 }
 else if(cal==0)
 {
   d.tired=d.tired-tiredvary;
   if(d.tired<=0)
     {d.tired=0;}
 }
 tiredvary=0;
 return;
}

/*---------------------------------------------------------------------------*/
/*主畫面和選單                                                               */
/*---------------------------------------------------------------------------*/

char *menuname[8][2]={
 {"             ",
  "[1;44;37m 選單 [46m[1]基本 [2]逛街 [3]修行 [4]玩樂 [5]打工 [6]特殊 [7]系統 [Q]離開          [0m"},
 
 {"             ",
  "[1;44;37m  基本選單  [46m[1]餵食 [2]清潔 [3]休息 [4]親親 [Q]跳出：                          [m"},

 {"[1;44;37m 逛街 [46m【日常用品】[1]便利商店 [2]星空藥鋪 [3]夜裡書局                          [m",
  "[1;44;37m 選單 [46m【武器百貨】[A]頭部武器 [B]右手武器 [C]左手武器 [D]身體武器 [E]腳部武器  [m"},
 
 {"[1;44;37m 修行 [46m[A]科學(%d) [B]詩詞(%d) [C]神學(%d) [D]軍學(%d) [E]劍術(%d)                   [m",
  "[1;44;37m 選單 [46m[F]格鬥(%d) [G]魔法(%d) [H]禮儀(%d) [I]繪畫(%d) [J]舞蹈(%d) [Q]跳出：         [m"},
  
 {"   ",
  "[1;44;37m  玩樂選單  [46m[1]散步 [2]運動 [3]約會 [4]猜拳 [5]旅遊 [6]郊外 [7]唱歌 [Q]跳出：  [m"},

 {"[1;44;37m 打工 [46m[A]家事 [B]保姆 [C]旅館 [D]農場 [E]餐\廳 [F]教堂 [G]地攤 [H]伐木         [m",
  "[1;44;37m 選單 [46m[I]美髮 [J]獵人 [K]工地 [L]守墓 [M]家教 [N]酒家 [O]酒店 [P]夜總會 [Q]跳出[m"},
 
 {"   ",
  "[1;44;37m  特殊選單  [46m[1]星空醫院 [2]媚登峰 [3]戰鬥修行 [4]拜訪朋友 [5]皇宮 [6]找人單挑  [m"},
  
 {"   ",
  "[1;44;37m  系統選單  [46m[1]詳細資料 [2]小雞自由 [3]特別服務 [4]儲存進度 [5]讀取進度 [Q]跳出[m"}
};

/*主選單*/
int pip_basic_menu(),pip_store_menu(),pip_practice_menu();
int pip_play_menu(),pip_job_menu(),pip_special_menu(),pip_system_menu();
static struct pipcommands pipmainlist[] =
{
pip_basic_menu,		'1',	'1',
pip_store_menu,		'2',	'2',
pip_practice_menu,	'3',	'3',
pip_play_menu,		'4',	'4',
pip_job_menu,		'5',	'5',
pip_special_menu,	'6',	'6',
pip_system_menu,	'7',	'7',
NULL,			'\0',	'\0'
};

/*基本選單*/
int pip_basic_feed(),pip_basic_takeshower(),pip_basic_takerest(),pip_basic_kiss();
static struct pipcommands pipbasiclist[] =
{
pip_basic_feed,		'1',	'1',
pip_basic_takeshower,	'2',	'2',
pip_basic_takerest,	'3',	'3',
pip_basic_kiss,		'4',	'4',
NULL,			'\0',	'\0'
};

/*商店選單*/
int pip_store_food(),pip_store_medicine(),pip_store_other();
int pip_store_weapon_head(),pip_store_weapon_rhand(),pip_store_weapon_lhand();
int pip_store_weapon_body(),pip_store_weapon_foot();
static struct pipcommands pipstorelist[] =
{
pip_store_food,		'1',	'1',
pip_store_medicine,	'2',	'2',
pip_store_other,	'3',	'3',
pip_store_weapon_head,	'a',	'A',
pip_store_weapon_rhand,	'b',	'B',
pip_store_weapon_lhand,	'c',	'C',
pip_store_weapon_body,	'd',	'D',
pip_store_weapon_foot,	'e',	'E',
NULL,			'\0',	'\0'
};
/*修行選單*/
int pip_practice_classA(),pip_practice_classB(),pip_practice_classC();
int pip_practice_classD(),pip_practice_classE(),pip_practice_classF();
int pip_practice_classG(),pip_practice_classH(),pip_practice_classI();
int pip_practice_classJ();
static struct pipcommands pippracticelist[] =
{
pip_practice_classA,	'a',	'A',
pip_practice_classB,	'b',	'B',
pip_practice_classC,	'c',	'C',
pip_practice_classD,	'd',	'D',
pip_practice_classE,	'e',	'E',
pip_practice_classF,	'f',	'F',
pip_practice_classG,	'g',	'G',
pip_practice_classH,	'h',	'H',
pip_practice_classI,	'i',	'I',
pip_practice_classJ,	'j',	'J',
NULL,			'\0',	'\0'
};

/*玩樂選單*/
int pip_play_stroll(),pip_play_sport(),pip_play_date(),pip_play_guess();
int pip_play_outing(),pip_play_kite(),pip_play_KTV();
static struct pipcommands pipplaylist[] =
{
pip_play_stroll,	'1',	'1',
pip_play_sport,		'2',	'2',
pip_play_date,		'3',	'3',
pip_play_guess,		'4',	'4',
pip_play_outing,	'5',	'5',
pip_play_kite,		'6',	'6',
pip_play_KTV,		'7',	'7',
NULL,			'\0',	'\0'
};
/*打工選單*/
int pip_job_workA(),pip_job_workB(),pip_job_workC(),pip_job_workD();
int pip_job_workE(),pip_job_workF(),pip_job_workG(),pip_job_workH();
int pip_job_workI(),pip_job_workJ(),pip_job_workK(),pip_job_workL();
int pip_job_workM(),pip_job_workN(),pip_job_workO(),pip_job_workP();
static struct pipcommands pipjoblist[] =
{
pip_job_workA,		'a',	'A',
pip_job_workB,		'b',	'B',
pip_job_workC,		'c',	'C',
pip_job_workD,		'd',	'D',
pip_job_workE,		'e',	'E',
pip_job_workF,		'f',	'F',
pip_job_workG,		'g',	'G',
pip_job_workH,		'h',	'H',
pip_job_workI,		'i',	'I',
pip_job_workJ,		'j',	'J',
pip_job_workK,		'k',	'K',
pip_job_workL,		'l',	'L',
pip_job_workM,		'm',	'M',
pip_job_workN,		'n',	'N',
pip_job_workO,		'o',	'O',
pip_job_workP,		'p',	'P',
NULL,			'\0',	'\0'
};

/*特殊選單*/
int t_talk(),pip_see_doctor(),pip_change_weight(),pip_meet_vs_man(),pip_query(),pip_go_palace();
static struct pipcommands pipspeciallist[] =
{
pip_see_doctor,		'1',	'1',
pip_change_weight,	'2',	'2',
pip_meet_vs_man,	'3',	'3',
pip_query,		'4',	'4',
pip_go_palace,		'5',	'5',
t_talk,			'6',	'6',
NULL,			'\0',	'\0'
};

/*系統選單*/
int pip_data_list(),pip_system_freepip(),pip_system_service();
int pip_write_backup(),pip_read_backup();
int pip_divine(),pip_results_show();
static struct pipcommands pipsystemlist[] =
{
pip_data_list,		'1',	'1',
pip_system_freepip,	'2',	'2',
pip_system_service,	'3',	'3',
pip_write_backup,	'4',	'4',
pip_read_backup,	'5',	'5',
pip_divine,		'o',	'O',
pip_results_show,	's',	'S',
NULL,			'\0',	'\0'
};



/*類似menu.c的功能*/
int
pip_do_menu(menunum,menumode,cmdtable)
int menunum,menumode;
struct pipcommands cmdtable[];
{
	time_t now;
	int key1,key2;
	int pipkey;
	int goback=0,ok=0;
	int class1=0,class2=0,class3=0,class4=0,class5=0;
	int class6=0,class7=0,class8=0,class9=0,class10=0;
	struct pipcommands *cmd1;
	struct pipcommands *cmd2;

	do
	{
	   ok=0;
	   /*判斷是否死亡  死掉即跳回上一層*/
	   if(d.death==1 || d.death==2 || d.death==3)
	     return 0;
	   /*經pip_mainmenu判定後是否死亡*/
	   if(pip_mainmenu(menumode)) 
	     return 0;

	   class1=d.wisdom/200+1;			/*科學*/
	   if(class1>5)  class1=5;
	   class2=(d.affect*2+d.wisdom+d.art*2+d.character)/400+1; /*詩詞*/
	   if(class2>5)  class2=5;  
	   class3=(d.belief*2+d.wisdom)/400+1;		/*神學*/
	   if(class3>5)  class3=5;   
	   class4=(d.hskill*2+d.wisdom)/400+1;		/*軍學*/
	   if(class4>5)  class4=5; 
	   class5=(d.hskill+d.attack)/400+1;		/*劍術*/
	   if(class5>5)  class5=5; 
	   class6=(d.hskill+d.resist)/400+1;		/*格鬥*/
	   if(class6>5)  class6=5; 
	   class7=(d.mskill+d.maxmp)/400+1;		/*魔法*/
	   if(class7>5)  class7=5;    
	   class8=(d.manners*2+d.character)/400+1;	/*禮儀*/
	   if(class8>5)  class8=5; 
	   class9=(d.art*2+d.character)/400+1; 		/*繪畫*/
	   if(class9>5)  class9=5;      
	   class10=(d.art*2+d.charm)/400+1;		/*舞蹈*/
	   if(class10>5) class10=5; 
	   
	   clrchyiuan(22,24);
	   move(b_lines-1,0);
	   prints(menuname[menunum][0],class1,class2,class3,class4,class5);
	   move(b_lines,0);
	   prints(menuname[menunum][1],class6,class7,class8,class9,class10);
	   
	   now=time(0);   
	   pip_time_change(now);
	   pipkey=egetch();
	   now=time(0);
	   pip_time_change(now);
	
	   cmd1=cmdtable;
	   cmd2=cmdtable;
	   switch(pipkey)
	   {
	     case KEY_ESC:
		if (KEY_ESC_arg == 'c')
		    capture_screen();
		else if (KEY_ESC_arg == 'n') 
		    edit_note();
		break;
			
	     case KEY_LEFT:
	     case 'q':
	     case 'Q':		
	        goback=1;
		break;
		
	     default:
		for(cmd1; key1 = cmd1->key1; cmd1++)
		/*if(key == tolower(pipkey))*/
		if(key1 == pipkey)
		{
		    cmd1->fptr();
		    ok=1;
		}
		for(cmd2; key2 = cmd2->key2; cmd2++)
		if(ok==0 && key2 == pipkey)
		{
		    cmd2->fptr();
		}
		break;
	   }  
	}while(goback==0);

	return 0;
}


/*---------------------------------------------------------------------------*/
/* 基本選單:餵食 清潔 親親 休息                                              */
/*                                                                           */
/*---------------------------------------------------------------------------*/
int pip_main_menu()
{
 pip_do_menu(0,0,pipmainlist);
 return 0;
}

/*---------------------------------------------------------------------------*/
/* 基本選單:餵食 清潔 親親 休息                                              */
/*                                                                           */
/*---------------------------------------------------------------------------*/
int pip_basic_menu()
{
 pip_do_menu(1,0,pipbasiclist);
 return 0;
}

/*---------------------------------------------------------------------------*/
/* 商店選單:食物 零食 大補丸 玩具 書本                                       */
/*                                                                           */
/*---------------------------------------------------------------------------*/
int pip_store_menu()
{
   pip_do_menu(2,1,pipstorelist);
   return 0;
}

/*---------------------------------------------------------------------------*/
/* 修行選單:念書 練武 修行                                                   */
/*                                                                           */
/*---------------------------------------------------------------------------*/
int pip_practice_menu()
{
   pip_do_menu(3,3,pippracticelist);
   return 0;
}

     
/*---------------------------------------------------------------------------*/
/* 玩樂選單:散步 旅遊 運動 約會 猜拳                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
int pip_play_menu()
{
  pip_do_menu(4,0,pipplaylist);
  return 0;
}

/*---------------------------------------------------------------------------*/
/* 打工選單:家事 苦工 家教 地攤                                              */
/*                                                                           */
/*---------------------------------------------------------------------------*/
int pip_job_menu()
{
   pip_do_menu(5,2,pipjoblist);
   return 0;
}

/*---------------------------------------------------------------------------*/
/* 特殊選單:看病 減肥 戰鬥 拜訪 朝見                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
int pip_special_menu()
{
  pip_do_menu(6,0,pipspeciallist);
  return 0;
}

/*---------------------------------------------------------------------------*/
/* 系統選單:個人資料  小雞放生  特別服務                                     */
/*                                                                           */
/*---------------------------------------------------------------------------*/
int pip_system_menu()
{
  pip_do_menu(7,0,pipsystemlist);
  return;
}


int
pip_mainmenu(mode)
int mode;
{
 char genbuf[200];
 char inbuf1[20];
 char inbuf2[20];
 char buf[256];
 time_t now;

 int tm,m,color,tm1,m1;
 int age;
 int color1,color2,color3,color4;
 int anynum;
 float pc;
 char yo[12][5]={"誕生","嬰兒","幼兒","兒童","少年","青年",
                 "成年","壯年","更年","老年","古稀","神仙"};

 color1=color2=color3=color4=37;
 move(1,0);
 tm=(time(0)-start_time+d.bbtime)/60/30; /* 一歲 */
 tm1=(time(0)-start_time+d.bbtime)/60;
 m=d.bbtime/60/30;
 m1 = d.bbtime/60;
 /*長大一歲時的增加改變值*/
 if(m!=tm)
 {
  d.wisdom+=10;
  d.happy+=rand()%5+5;
  if(d.happy>100)
     d.happy=100;
  d.satisfy+=rand()%5;
  if(d.satisfy>100)
     d.satisfy=100;
  if(tm<13) d.maxhp+=rand()%5+5; else d.maxhp-=rand()%15;
  d.character+=rand()%5;
  d.money+=500;
  d.seeroyalJ=1;
  count_tired(1,7,"N",100,0);
  d.bbtime+=time(0)-start_time;
  start_time=time(0);
  pip_write_file();

  /*記錄開始*/
  now=time(0);
  sprintf(genbuf, "[1;37m%s %-11s的小雞 [%s] 滿 %d 歲了 [0m\n", Cdate(&now), cuser.userid,d.name,m+1);
  pip_log_record(genbuf);
  /*記錄終止*/
  clear();
  showtitle("電子養小雞", BoardName);  
  show_basic_pic(20); /*生日快樂*/
  pressanykey("小雞長大一歲了..");
  /*結局*/
  if(tm%2==0)
     pip_results_show();
  if(tm>=21 && (d.wantend==4 || d.wantend==5 || d.wantend==6))
     pip_ending_screen(); 
  
  clrtobot();
  refresh();
 }
 color=37;
 m=tm;
 
 if((rand()%30==29) && tm>=15 && d.charm>=300 && d.character >=300)
    pip_marriage_offer();
    
 if(mode!=1 && rand()%71==69)
    pip_divine();

 /*武官*/
 if((time(0)-start_time)>=900 )
 {
  d.seeroyalJ=0;
 }
 
 if(m==0) /*誕生*/
     age=0;
 else if( m==1) /*嬰兒*/
     age=1;
 else if( m>=2 && m<=5 ) /*幼兒*/
     age=2;
 else if( m>=6 && m<=12 ) /*兒童*/
     age=3;
 else if( m>=13 && m<=15 ) /*少年*/
     age=4;     
 else if( m>=16 && m<=18 ) /*青年*/
     age=5;     
 else if( m>=19 && m<=35 ) /*成年*/
     age=6;
 else if( m>=36 && m<=45 ) /*壯年*/
     age=7;
 else if( m>=45 && m<=60 ) /*更年*/
     age=8;
 else if( m>=60 && m<=70 ) /*老年*/
     age=9;
 else if( m>=70 && m<=100 ) /*古稀*/
     age=10;
 else if( m>100 ) /*神仙*/
     age=11;
 clear(); 
 /*showtitle("電子養小雞", BoardName);*/
 move(0,0);
 if(d.sex==1)
   sprintf(buf,"[1;41m  星空戰鬥雞 ∼ [32m♂ [37m%-15s                                             [0m",d.name); 	 
 else if(d.sex==2)
   sprintf(buf,"[1;41m  星空戰鬥雞 ∼ [33m♀ [37m%-15s                                             [0m",d.name); 	 
 else 
   sprintf(buf,"[1;41m  星空戰鬥雞 ∼ [34m？ [37m%-15s                                             [0m",d.name); 	 
 prints(buf); 
 
 move(1,0);
 if(d.money<=100)
     color1=31;
 else if(d.money>100 && d.money<=500)
     color1=33;
 else
     color1=37;
 sprintf(inbuf1,"%02d/%02d/%02d",d.year,d.month,d.day);
 sprintf(buf
 ," [1;32m[狀  態][37m %-5s     [32m[生  日][37m %-9s [32m[年  齡][37m %-5d     [32m[金  錢][%dm %-8d [m"
 ,yo[age],inbuf1,tm,color1,d.money);
 prints(buf);
   
 move(2,0);   
 
 if((d.hp*100/d.maxhp)<=20)
     color1=31;
 else if((d.hp*100/d.maxhp)<=40 && (d.hp*100/d.maxhp)>20)
     color1=33;
 else
     color1=37;   
 if(d.maxmp==0)
     color2=37;
 else if((d.mp*100/d.maxmp)<=20)
     color2=31;
 else if((d.mp*100/d.maxmp)<=40 && (d.mp*100/d.maxmp)>20)
     color2=33;
 else
     color2=37;   
   
 if(d.tired>=80)
     color3=31;
 else if(d.tired<80 && d.tired >=60)
     color3=33;
 else
     color3=37;      
     
 sprintf(inbuf1,"%d/%d",d.hp,d.maxhp);  
 sprintf(inbuf2,"%d/%d",d.mp,d.maxmp);       
 sprintf(buf
 ," [1;32m[生  命][%dm %-10s[32m[法  力][%dm %-10s[32m[體  重][37m %-5d     [32m[疲  勞][%dm %-4d[0m "
 ,color1,inbuf1,color2,inbuf2,d.weight,color3,d.tired);
 prints(buf);
   
 move(3,0);
 if(d.shit>=80)
     color1=31;
 else if(d.shit<80 && d.shit >=60)
     color1=33;
 else
     color1=37;         
 if(d.sick>=75)
     color2=31;
 else if(d.sick<75 && d.sick >=50)
     color2=33;
 else
     color2=37;           
 if(d.happy<=20)
     color3=31;
 else if(d.happy>20 && d.happy <=40)
     color3=33;
 else
     color3=37;           
 if(d.satisfy<=20)
     color4=31;
 else if(d.satisfy>20 && d.satisfy <=40)
     color4=33;
 else
     color4=37;           
 sprintf(buf
 ," [1;32m[髒  髒][%dm %-4d      [32m[病  氣][%dm %-4d      [32m[快樂度][%dm %-4d      [32m[滿意度][%dm %-4d[0m"
 ,color1,d.shit,color2,d.sick,color3,d.happy,color4,d.satisfy);
 prints(buf);              
 if(mode==0)  /*主要畫面*/
 {
   anynum=0;
   anynum=rand()%4;
   move(4,0);
   if(anynum==0)
     sprintf(buf," [1;35m[站長曰]:[31m紅色[36m表示危險  [33m黃色[36m表示警告  [37m白色[36m表示安全[0m");
   else if(anynum==1)
     sprintf(buf," [1;35m[站長曰]:[37m要多多注意小雞的疲勞度和病氣  以免累死病死[0m");     
   else if(anynum==2)
     sprintf(buf," [1;35m[站長曰]:[37m隨時注意小雞的生命數值唷![0m");                  
   else if(anynum==3)
     sprintf(buf," [1;35m[站長曰]:[37m快快樂樂的小雞才是幸福的小雞.....[0m");                       
   prints(buf);               
 }
 else if(mode==1)/*餵食*/
 {
   move(4,0);
   if(d.food==0)
     color1=31;
   else if(d.food<=5 && d.food>0)
     color1=33;
   else
     color1=37;         
   if(d.cookie==0)
     color2=31;
   else if(d.cookie<=5 && d.cookie>0)
     color2=33;
   else
     color2=37;           
   if(d.bighp==0)
     color3=31;
   else if(d.bighp<=2 && d.bighp >0)
     color3=33;
   else
     color3=37;           
   if(d.medicine==0)
     color4=31;
   else if(d.medicine<=5 && d.medicine>0)
     color4=33;
   else
     color4=37;           
   sprintf(buf
   ," [1;36m[食物][%dm%-7d[36m[零食][%dm%-7d[36m[補丸][%dm%-7d[36m[靈芝][%dm%-7d[36m[人參][37m%-7d[36m[雪蓮][37m%-7d[0m"
   ,color1,d.food,color2,d.cookie,color3,d.bighp,color4,d.medicine,d.ginseng,d.snowgrass);
   prints(buf);
   
 }  
 else if(mode==2)/*打工*/
 {
   move(4,0);
   sprintf(buf
   ," [1;36m[愛心][37m%-5d[36m[智慧][37m%-5d[36m[氣質][37m%-5d[36m[藝術][37m%-5d[36m[道德][37m%-5d[36m[勇敢][37m%-5d[36m[家事][37m%-5d[0m"   
   ,d.love,d.wisdom,d.character,d.art,d.etchics,d.brave,d.homework);
   prints(buf);
   
 }  
 else if(mode==3)/*修行*/
 {
   move(4,0);
   sprintf(buf
   ," [1;36m[智慧][37m%-5d[36m[氣質][37m%-5d[36m[藝術][37m%-5d[36m[勇敢][37m%-5d[36m[攻擊][37m%-5d[36m[防禦][37m%-5d[36m[速度][37m%-5d[0m"   
   ,d.wisdom,d.character,d.art,d.brave,d.attack,d.resist,d.speed);
   prints(buf);
   
 }  
  move(5,0);
  prints("[1;%dm┌─────────────────────────────────────┐[m",color);  
  move(6,0);
  switch(age)
  {
     case 0:       
     case 1:
     case 2:
       if(d.weight<=(60+10*tm-30))
          show_basic_pic(1);
       else if(d.weight>(60+10*tm-30) && d.weight<(60+10*tm+30))
          show_basic_pic(2);
       else if(d.weight>=(60+10*tm+30))
          show_basic_pic(3);
       break;
     case 3:
     case 4:
       if(d.weight<=(60+10*tm-30))
          show_basic_pic(4);
       else if(d.weight>(60+10*tm-30) && d.weight<(60+10*tm+30))
          show_basic_pic(5);
       else if(d.weight>=(60+10*tm+30))
          show_basic_pic(6);
       break;
     case 5:
     case 6:
       if(d.weight<=(60+10*tm-30))
          show_basic_pic(7);
       else if(d.weight>(60+10*tm-30) && d.weight<(60+10*tm+30))
          show_basic_pic(8);
       else if(d.weight>=(60+10*tm+30))
          show_basic_pic(9);
       break;     
     case 7:
     case 8:
       if(d.weight<=(60+10*tm-30))
          show_basic_pic(10);
       else if(d.weight>(60+10*tm-30) && d.weight<(60+10*tm+30))
          show_basic_pic(11);
       else if(d.weight>=(60+10*tm+30))
          show_basic_pic(12);
       break;     
     case 9:
       show_basic_pic(13);
       break;
     case 10:
     case 11:
       show_basic_pic(16);
       break;
  }
  

 move(18,0);
 prints("[1;%dm└─────────────────────────────────────┘[m",color);
 move(19,0);
 prints(" [1;34m─[37;44m  狀 態  [0;1;34m─[0m");
 move(20,0);
 prints(" ");
 if(d.shit==0)
     prints("乾淨小雞  ");
 if(d.shit>40&&d.shit<60)
     prints("有點臭臭  ");
 if(d.shit>=60&&d.shit<80)
     prints("[1;33m很臭了說[m  ");
 if(d.shit>=80&&d.shit<100)
  {
     prints("[1;35m快臭死了[m  ");
     d.sick+=4;
     d.character-=(rand()%3+3);
  }
 if(d.shit>=100)
  {
     d.death=1;
     pipdie("[1;31m哇∼臭死了[m  ",1);
     return -1;
  }

 if(d.hp<=0)
   pc=0;
 else
   pc=d.hp*100/d.maxhp;
 if(pc==0)
  {
     d.death=1;
     pipdie("[1;31m嗚∼餓死了[m  ",1);
     return -1;
  }
 if(pc<20)
  {
     prints("[1;35m快餓昏了[m  ");
     d.sick+=3;
     d.happy-=5;
     d.satisfy-=3;
  }
 if(pc<40&&pc>=20)
     prints("[1;33m想吃東西[m  ");
 if(pc<=100&&pc>=90)
     prints("肚子飽飽  ");
 if(pc<110&&pc>100)
     prints("[1;33m撐撐的說[m  ");

 pc=d.tired;
 if(pc<20)
     prints("精神很好  ");
 if(pc<80&&pc>=60)
     prints("[1;33m有點小累[m  ");
 if(pc<100&&pc>=80)
  {
     prints("[1;35m真的很累[m  ");
     d.sick+=5;
  }
 if(pc>=100)
  {
     d.death=1;
     pipdie("[1;31mㄚ∼累死了[m  ",1);
     return -1;
  }

 pc=60+10*tm;
 if(d.weight<(pc+30) && d.weight>=(pc+10))
     prints("[1;33m有點小胖[m  ");
 if(d.weight<(pc+50) && d.weight>=(pc+30))
  {
     prints("[1;35m太胖了啦[m  ");
     d.sick+=3;
     if(d.speed>=2)
        d.speed-=2;
     else
        d.speed=0;
     
  }
 if(d.weight>(pc+50))
  {
     d.death=1;
     pipdie("[1;31m嗚∼肥死了[m  ",1);
     return -1;
  }

 if(d.weight<(pc-50))
  {
     d.death=1;
     pipdie("[1;31m:~~ 瘦死了[m  ",1);
     return -1;
  }
 if(d.weight>(pc-30) && d.weight<=(pc-10))
     prints("[1;33m有點小瘦[m  ");
 if(d.weight>(pc-50) && d.weight<=(pc-30))
     prints("[1;35m太瘦了喔[m ");

 if(d.sick<75 &&d.sick>=50)
  {
     prints("[1;33m生病了啦[m  ");
     count_tired(1,8,"Y",100,1);
  }
 if(d.sick<100&&d.sick>=75)
  {
     prints("[1;35m正病重中[m  ");
     d.sick+=5;
     count_tired(1,15,"Y",100,1);
  }
 if(d.sick>=100)
  {
     d.death=1;
     pipdie("[1;31m病死了啦 :~~[m  ",1);
     return -1;
  }

 pc=d.happy;
 if(pc<20)
     prints("[1;35m很不快樂[m  ");
 if(pc<40&&pc>=20)
     prints("[1;33m不太快樂[m  ");
 if(pc<95&&pc>=80)
     prints("快樂啦..  ");
 if(pc<=100 &&pc>=95)
     prints("很快樂..  ");

 pc=d.satisfy;
 if(pc<20) prints("[1;35m很不滿足..[m  ");
 if(pc<40&&pc>=20) prints("[1;33m不太滿足[m  ");
 if(pc<95&&pc>=80) prints("滿足啦..  ");
 if(pc<=100 && pc>=95) prints("很滿足..  ");

 prints("\n");

 pip_write_file();
 return 0;
}

/*固定時間作的事 */
int
pip_time_change(cnow)
time_t cnow;
{
  int stime=60;
  int stired=2;
  while ((time(0)-lasttime)>=stime) /* 固定時間做的事 */
  {
   /*不做事  還是會變髒的*/
   if((time(0)-cnow)>=stime)
      d.shit+=(rand()%3+3);
   /*不做事  疲勞當然減低啦*/
   if(d.tired>=stired) d.tired-=stired; else d.tired=0;
   /*不做事  肚子也會餓咩 */
   d.hp-=rand()%2+2;
   if(d.mexp<0)
      d.mexp=0;
   if(d.hexp<0)
      d.hexp=0;
   /*體力會因生病降低一點*/
   d.hp-=d.sick/10;
   /*病氣會隨機率增加減少少許*/
   if(rand()%3>0)
    {
       d.sick-=rand()%2;
       if(d.sick<0)
         d.sick=0;
    }
   else
      d.sick+=rand()%2;
   /*隨機減快樂度*/
   if(rand()%4>0)
    {
       d.happy-=rand()%2+2;
    }
   else
       d.happy+=2;
   if(rand()%4>0)
    {
       d.satisfy-=(rand()%4+5);
    }
   else
       d.satisfy+=2;
   lasttime+=stime;
  };
   /*快樂度滿意度最大值設定*/
   if(d.happy>100)
     d.happy=100;
   else if(d.happy<0)
     d.happy=0;
   if(d.satisfy>100)
     d.satisfy=100;
   else if(d.satisfy<0)
     d.satisfy=0;  
   /*評價*/
   if(d.social<0)
     d.social=0;
   if(d.tired<0)
     d.tired=0;
   if(d.hp>d.maxhp)
     d.hp=d.maxhp;
   if(d.mp>d.maxmp)
     d.mp=d.maxmp;
   if(d.money<0)
     d.money=0;
   if(d.charm<0)
     d.charm=0;
}

/*---------------------------------------------------------------------------*/
/* 基本選單:餵食 清潔 親親 休息                                              */
/*                                                                           */
/*---------------------------------------------------------------------------*/

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
     count_tired(5,20,"Y",100,0);
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
  char buf[256];
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
   
   }
  }while((pipkey!='Q')&&(pipkey!='q')&&(pipkey!=KEY_LEFT));
  
  return 0;
}

/*遊戲寫資料入檔案*/
pip_write_file()
{
 FILE *ff;
 char buf[200];
 sprintf(buf,"home/%s/chicken",cuser.userid);

 if(ff=fopen(buf,"w"))
 {
  fprintf(ff, "%lu\n", d.bbtime);
  fprintf(ff,
  "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %s %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
  d.year,d.month,d.day,d.sex,d.death,d.nodone,d.relation,d.liveagain,d.dataB,d.dataC,d.dataD,d.dataE,
  d.hp,d.maxhp,d.weight,d.tired,d.sick,d.shit,d.wrist,d.bodyA,d.bodyB,d.bodyC,d.bodyD,d.bodyE,
  d.social,d.family,d.hexp,d.mexp,d.tmpA,d.tmpB,d.tmpC,d.tmpD,d.tmpE,
  d.mp,d.maxmp,d.attack,d.resist,d.speed,d.hskill,d.mskill,d.mresist,d.magicmode,d.fightB,d.fightC,d.fightD,d.fightE,
  d.weaponhead,d.weaponrhand,d.weaponlhand,d.weaponbody,d.weaponfoot,d.weaponA,d.weaponB,d.weaponC,d.weaponD,d.weaponE,
  d.toman,d.character,d.love,d.wisdom,d.art,d.etchics,d.brave,d.homework,d.charm,d.manners,d.speech,d.cookskill,d.learnA,d.learnB,d.learnC,d.learnD,d.learnE,
  d.happy,d.satisfy,d.fallinlove,d.belief,d.offense,d.affect,d.stateA,d.stateB,d.stateC,d.stateD,d.stateE,
  d.food,d.medicine,d.bighp,d.cookie,d.ginseng,d.snowgrass,d.eatC,d.eatD,d.eatE,
  d.book,d.playtool,d.money,d.thingA,d.thingB,d.thingC,d.thingD,d.thingE,
  d.winn,d.losee,
  d.royalA,d.royalB,d.royalC,d.royalD,d.royalE,d.royalF,d.royalG,d.royalH,d.royalI,d.royalJ,d.seeroyalJ,d.seeA,d.seeB,d.seeC,d.seeD,d.seeE,
  d.wantend,d.lover,d.name,
  d.classA,d.classB,d.classC,d.classD,d.classE,
  d.classF,d.classG,d.classH,d.classI,d.classJ,
  d.classK,d.classL,d.classM,d.classN,d.classO,
  d.workA,d.workB,d.workC,d.workD,d.workE,
  d.workF,d.workG,d.workH,d.workI,d.workJ,
  d.workK,d.workL,d.workM,d.workN,d.workO,
  d.workP,d.workQ,d.workR,d.workS,d.workT,
  d.workU,d.workV,d.workW,d.workX,d.workY,d.workZ
  );
  fclose(ff);
 }
}

/*遊戲讀資料出檔案*/
int
pip_read_file(userid)
  char *userid;
{
 FILE *fs;
 char buf[200];
 sprintf(buf,"home/%s/chicken",userid);

 if(fs=fopen(buf,"r"))
 {
  fgets(buf, 80, fs);
  d.bbtime = (time_t) atol(buf);

  fscanf(fs,
  "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %s %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
  &(d.year),&(d.month),&(d.day),&(d.sex),&(d.death),&(d.nodone),&(d.relation),&(d.liveagain),&(d.dataB),&(d.dataC),&(d.dataD),&(d.dataE),
  &(d.hp),&(d.maxhp),&(d.weight),&(d.tired),&(d.sick),&(d.shit),&(d.wrist),&(d.bodyA),&(d.bodyB),&(d.bodyC),&(d.bodyD),&(d.bodyE),
  &(d.social),&(d.family),&(d.hexp),&(d.mexp),&(d.tmpA),&(d.tmpB),&(d.tmpC),&(d.tmpD),&(d.tmpE),
  &(d.mp),&(d.maxmp),&(d.attack),&(d.resist),&(d.speed),&(d.hskill),&(d.mskill),&(d.mresist),&(d.magicmode),&(d.fightB),&(d.fightC),&(d.fightD),&(d.fightE),
  &(d.weaponhead),&(d.weaponrhand),&(d.weaponlhand),&(d.weaponbody),&(d.weaponfoot),&(d.weaponA),&(d.weaponB),&(d.weaponC),&(d.weaponD),&(d.weaponE),
  &(d.toman),&(d.character),&(d.love),&(d.wisdom),&(d.art),&(d.etchics),&(d.brave),&(d.homework),&(d.charm),&(d.manners),&(d.speech),&(d.cookskill),&(d.learnA),&(d.learnB),&(d.learnC),&(d.learnD),&(d.learnE),
  &(d.happy),&(d.satisfy),&(d.fallinlove),&(d.belief),&(d.offense),&(d.affect),&(d.stateA),&(d.stateB),&(d.stateC),&(d.stateD),&(d.stateE),
  &(d.food),&(d.medicine),&(d.bighp),&(d.cookie),&(d.ginseng),&(d.snowgrass),&(d.eatC),&(d.eatD),&(d.eatE),
  &(d.book),&(d.playtool),&(d.money),&(d.thingA),&(d.thingB),&(d.thingC),&(d.thingD),&(d.thingE),
  &(d.winn),&(d.losee),
  &(d.royalA),&(d.royalB),&(d.royalC),&(d.royalD),&(d.royalE),&(d.royalF),&(d.royalG),&(d.royalH),&(d.royalI),&(d.royalJ),&(d.seeroyalJ),&(d.seeA),&(d.seeB),&(d.seeC),&(d.seeD),&(d.seeE),
  &(d.wantend),&(d.lover),d.name,
  &(d.classA),&(d.classB),&(d.classC),&(d.classD),&(d.classE),
  &(d.classF),&(d.classG),&(d.classH),&(d.classI),&(d.classJ),
  &(d.classK),&(d.classL),&(d.classM),&(d.classN),&(d.classO),
  &(d.workA),&(d.workB),&(d.workC),&(d.workD),&(d.workE),
  &(d.workF),&(d.workG),&(d.workH),&(d.workI),&(d.workJ),
  &(d.workK),&(d.workL),&(d.workM),&(d.workN),&(d.workO),
  &(d.workP),&(d.workQ),&(d.workR),&(d.workS),&(d.workT),
  &(d.workU),&(d.workV),&(d.workW),&(d.workX),&(d.workY),&(d.workZ)
  );  

  fclose(fs);
 }
 else
 {
   pressanykey("我沒有養小雞啦 !");
   return 0;
 }
 
 return 1;
}

/*記錄到pip.log檔*/
int 
pip_log_record(msg)
char *msg;
{
  FILE *fs;
  
  fs=fopen("log/pip.log","a+");
  fprintf(fs,"%s",msg);
  fclose(fs);
}

/*小雞進度儲存*/
int
pip_write_backup()
{
 char *files[4]={"沒有","進度一","進度二","進度三"};
 char buf[200],buf1[200];
 char ans[3];
 int num=0;
 int pipkey;

 show_system_pic(21);
 pip_write_file();
 do{ 
    move(b_lines-2, 0);
    clrtoeol();
    move(b_lines-1, 0);
    clrtoeol();
    move(b_lines-1,1);
    prints("儲存 [1]進度一 [2]進度二 [3]進度三 [Q]放棄 [1/2/3/Q]：");
    pipkey=egetch();
    
    if (pipkey=='1')
      num=1;
    else if(pipkey=='2') 
      num=2;
    else if(pipkey=='3') 
      num=3;     
    else
      num=0;
    
 }while(pipkey!='Q' && pipkey!='q' && num!=1 && num!=2 && num!=3);
 if(pipkey=='q' ||pipkey=='Q')
 {
    pressanykey("放棄儲存遊戲進度");
    return 0;
 }  
 move(b_lines-2, 1);
 prints("儲存檔案會覆蓋\原儲存於 [%s] 的小雞的檔案喔！請考慮清楚...",files[num]);
 sprintf(buf1,"確定要儲存於 [%s] 檔案嗎？ [y/N]:",files[num]);
 getdata(b_lines-1, 1,buf1, ans, 2, 1, 0);
 if (ans[0]!='y'&&ans[0]!='Y') 
 {
    pressanykey("放棄儲存檔案");
    return 0;
 }
 
 move(b_lines-1,0);
 clrtobot();
 sprintf(buf1,"儲存 [%s] 檔案完成了",files[num]);
 pressanykey(buf1);
 sprintf(buf,"/bin/cp home/%s/chicken home/%s/chicken.bak%d",cuser.userid,cuser.userid,num);
 system(buf);
 return 0;
}

int
pip_read_backup()
{
 char buf[200],buf1[200],buf2[200];
 char *files[4]={"沒有","進度一","進度二","進度三"};
 char ans[3];
 int pipkey;
 int num=0;
 int ok=0;
 FILE *fs;
 show_system_pic(22);
 do{ 
    move(b_lines-2, 0);
    clrtoeol();
    move(b_lines-1, 0);
    clrtoeol();
    move(b_lines-1,1);
    prints("讀取 [1]進度一 [2]進度二 [3]進度三 [Q]放棄 [1/2/3/Q]：");
    pipkey=egetch();
    
    if (pipkey=='1')
      num=1;
    else if(pipkey=='2') 
      num=2;
    else if(pipkey=='3') 
      num=3;     
    else
      num=0;
    
    if(num>0)
    {
      sprintf(buf,"home/%s/chicken.bak%d",cuser.userid,num);
      if((fs=fopen(buf,"r")) == NULL)
      {
        sprintf(buf,"檔案 [%s] 不存在",files[num]);
        pressanykey(buf);
        ok=0;
      }
      else 
      {
         
	 move(b_lines-2, 1);
	 prints("讀取出檔案會覆蓋\現在正在玩的小雞的檔案喔！請考慮清楚...");
	 sprintf(buf,"確定要讀取出 [%s] 檔案嗎？ [y/N]:",files[num]);
	 getdata(b_lines-1, 1,buf, ans, 2, 1, 0);
	 if (ans[0]!='y'&&ans[0]!='Y') 
	    pressanykey("讓我再決定一下...");
	 else ok=1;
      }
    }
 }while(pipkey!='Q' && pipkey!='q' && ok!=1);
 if(pipkey=='q' ||pipkey=='Q')
 {
    pressanykey("還是玩原本的遊戲");
    return 0;
 }
 
 move(b_lines-1,0);
 clrtobot();
 sprintf(buf,"讀取 [%s] 檔案完成了",files[num]);
 pressanykey(buf);
 
 sprintf(buf1,"/bin/touch home/%s/chicken.bak%d",cuser.userid,num);
 sprintf(buf2,"/bin/cp home/%s/chicken.bak%d home/%s/chicken",cuser.userid,num,cuser.userid);
 system(buf1);
 system(buf2);
 pip_read_file(cuser.userid);
 return 0;
}



int
pip_live_again()
{
   char genbuf[80];
   time_t now;
   int tm;
   
   tm=(d.bbtime)/60/30;

   clear();
   showtitle("小雞復活手術中", BoardName);

   now = time(0);
   sprintf(genbuf, "[1;33m%s %-11s的小雞 [%s二代] 復活了！[m\n", Cdate(&now), cuser.userid,d.name);
   pip_log_record(genbuf);
   
   /*身體上的設定*/
   d.death=0;
   d.maxhp=d.maxhp*3/4+1;
   d.hp=d.maxhp/2+1;
   d.tired=20;
   d.shit=20;
   d.sick=20;   
   d.wrist=d.wrist*3/4;
   d.weight=45+10*tm;
   
   /*錢減到五分之一*/
   d.money=d.money/5;
   
   /*戰鬥能力降一半*/
   d.attack=d.attack*3/4;
   d.resist=d.resist*3/4;
   d.maxmp=d.maxmp*3/4;
   d.mp=d.maxmp/2;
   
   /*變的不快樂*/
   d.happy=0;
   d.satisfy=0;
   
   /*評價減半*/
   d.social=d.social*3/4;
   d.family=d.family*3/4;
   d.hexp=d.hexp*3/4;
   d.mexp=d.mexp*3/4;

   /*武器掉光光*/   
   d.weaponhead=0;
   d.weaponrhand=0;
   d.weaponlhand=0;
   d.weaponbody=0;
   d.weaponfoot=0;
   
   /*食物剩一半*/
   d.food=d.food/2;
   d.medicine=d.medicine/2;
   d.bighp=d.bighp/2;
   d.cookie=d.cookie/2;

   d.liveagain+=1;
   
   pressanykey("小雞器官重建中！");
   pressanykey("小雞體質恢復中！");
   pressanykey("小雞能力調整中！");
   pressanykey("恭禧您，你的小雞又復活囉！");
   pip_write_file();
   return 0;
}

/*---------------------------------------------------------------------------*/
/* 小雞圖形區                                                                */
/*                                                                           */
/*---------------------------------------------------------------------------*/
int
show_basic_pic(int i)
{
 char buf[256];
 clrchyiuan(6,18);
 sprintf(buf,BBSHOME"/game/pipgame/basic/pic%d",i);
 show_file(buf,6,12,ONLY_COLOR);
}

int
show_feed_pic(int i)  /*吃東西*/
{
 char buf[256];
 clrchyiuan(6,18);
 sprintf(buf,BBSHOME"/game/pipgame/feed/pic%d",i);
 show_file(buf,6,12,ONLY_COLOR);
}

int
show_buy_pic(int i)  /*購買東西*/
{
 char buf[256];
 clrchyiuan(6,18);
 sprintf(buf,BBSHOME"/game/pipgame/buy/pic%d",i);
 show_file(buf,6,12,ONLY_COLOR);
}



int
show_usual_pic(int i)  /* 平常狀態 */
{
 char buf[256];
 clrchyiuan(6,18);
 sprintf(buf,BBSHOME"/game/pipgame/usual/pic%d",i);
 show_file(buf,6,12,ONLY_COLOR);

}

int
show_special_pic(int i)  /* 特殊選單 */
{
 char buf[256];
 clrchyiuan(6,18);
 sprintf(buf,BBSHOME"/game/pipgame/special/pic%d",i);
 show_file(buf,6,12,ONLY_COLOR);

}

int
show_practice_pic(int i)  /*修行用的圖 */
{
 char buf[256];
 clrchyiuan(6,18);
 sprintf(buf,BBSHOME"/game/pipgame/practice/pic%d",i);
 show_file(buf,6,12,ONLY_COLOR);
}

int
show_job_pic(int i)    /* 打工的show圖 */
{
 char buf[256];
 clrchyiuan(6,18);
 sprintf(buf,BBSHOME"/game/pipgame/job/pic%d",i);
 show_file(buf,6,12,ONLY_COLOR);

}


int
show_play_pic(int i)  /*休閒的圖*/
{
 char buf[256];
 clrchyiuan(6,18);
 sprintf(buf,BBSHOME"/game/pipgame/play/pic%d",i);
 if(i==0)
    show_file(buf,2,16,ONLY_COLOR);
 else
    show_file(buf,6,12,ONLY_COLOR);
}

int
show_guess_pic(int i)  /* 猜拳用 */
{
 char buf[256];
 clrchyiuan(6,18);
 sprintf(buf,BBSHOME"/game/pipgame/guess/pic%d",i);
 show_file(buf,6,12,ONLY_COLOR);
}

int
show_weapon_pic(int i)  /* 武器用 */
{
 char buf[256];
 clrchyiuan(1,10);
 sprintf(buf,BBSHOME"/game/pipgame/weapon/pic%d",i);
 show_file(buf,1,10,ONLY_COLOR);
}

int
show_palace_pic(int i)  /* 參見王臣用 */
{
 char buf[256];
 clrchyiuan(0,13);
 sprintf(buf,BBSHOME"/game/pipgame/palace/pic%d",i);
 show_file(buf,0,11,ONLY_COLOR);

}

int
show_badman_pic(int i)  /* 壞人 */
{
 char buf[256];
 clrchyiuan(6,18);
 sprintf(buf,BBSHOME"/game/pipgame/badman/pic%d",i);
 show_file(buf,6,14,ONLY_COLOR);
}

int
show_fight_pic(int i)  /* 打架 */
{
 char buf[256];
 clrchyiuan(6,18);
 sprintf(buf,BBSHOME"/game/pipgame/fight/pic%d",i);
 show_file(buf,6,14,ONLY_COLOR);
}

int
show_die_pic(int i)  /*死亡*/
{
 char buf[256];
 clrchyiuan(0,23);
 sprintf(buf,BBSHOME"/game/pipgame/die/pic%d",i);
 show_file(buf,0,23,ONLY_COLOR);
}

int
show_system_pic(int i)  /*系統*/
{
 char buf[256];
 clrchyiuan(1,23);
 sprintf(buf,BBSHOME"/game/pipgame/system/pic%d",i);
 show_file(buf,4,16,ONLY_COLOR);
}

int
show_ending_pic(int i)  /*結束*/
{
 char buf[256];
 clrchyiuan(1,23);
 sprintf(buf,BBSHOME"/game/pipgame/ending/pic%d",i);
 show_file(buf,4,16,ONLY_COLOR);
}

int 
show_resultshow_pic(int i)	/*收穫季*/
{
 char buf[256];
 clrchyiuan(0,24);
 sprintf(buf,BBSHOME"/game/pipgame/resultshow/pic%d",i);
 show_file(buf,0,24,ONLY_COLOR);
}

/*---------------------------------------------------------------------------*/
/* 商店選單:食物 零食 大補丸 玩具 書本                                       */
/*                                                                           */
/*---------------------------------------------------------------------------*/

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
			getdata(b_lines-1,1,inbuf,genbuf, 3, LCECHO,"0");
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
					getdata(b_lines-1,1,inbuf,genbuf,6, 1, 0); 
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
					getdata(b_lines-1,1,inbuf,genbuf, 2, 1, 0); 
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
			getdata(b_lines-1,1,inbuf,genbuf, 3, LCECHO,"0");
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
				getdata(b_lines-1,1,inbuf,genbuf,6, 1, 0); 
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
					getdata(b_lines-1,1,inbuf,genbuf, 2, 1, 0); 
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
     getdata(b_lines-1,1,shortbuf,choicekey, 4, LCECHO,"0");
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
          getdata(b_lines-1,1,shortbuf, ans, 2, 1, 0); 
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
        getdata(b_lines-1,1,shortbuf, ans, 2, 1, 0); 
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
     pip_data_list;   
     break;    
   }
  }while((pipkey!='Q')&&(pipkey!='q')&&(pipkey!=KEY_LEFT));
    
  return variance;
}

/*---------------------------------------------------------------------------*/
/* 打工選單:家事 苦工 家教 地攤                                              */
/*                                                                           */
/*---------------------------------------------------------------------------*/

int pip_job_workA()
{
/*  ├────┼──────────────────────┤*/
/*  │家庭管理│待人接物 + N , 掃地洗衣 + N , 烹飪技巧 + N  │*/
/*  │        │和父親的關係 + N , 疲勞 + 1 , 感受 - 2      │*/
/*  ├────┼──────────────────────┤*/
/*  │家庭管理│若 體    力 - RND (疲勞) >=   5 則工作成功  │*/
/*  ├────┼──────────────────────┤*/  
    float class;
    long workmoney;
    
    workmoney=0;       
    class=(d.hp*100/d.maxhp)-d.tired;        
    d.maxhp+=rand()%2;
    d.shit+=rand()%3+5;
    count_tired(3,7,"Y",100,1);
    d.hp-=(rand()%2+4);
    d.happy-=(rand()%3+4);
    d.satisfy-=rand()%3+4;
    d.affect-=7+rand()%7;
    if(d.affect<=0)
       d.affect=0;
    show_job_pic(11);
    if(class>=75)
    {
      d.cookskill+=rand()%2+7;    
      d.homework+=rand()%2+7;
      d.family+=rand()%3+4;
      d.relation+=rand()%3+4;
      workmoney=80+(d.cookskill*2+d.homework+d.family)/40;
      pressanykey("家事很成功\喔..多一點錢給你..");
    }
    else if(class<75 && class>=50)
    {
      d.cookskill+=rand()%2+5;    
      d.homework+=rand()%2+5;
      d.family+=rand()%3+3;
      d.relation+=rand()%3+3;
      workmoney=60+(d.cookskill*2+d.homework+d.family)/45;
      pressanykey("家事還蠻順利的唷..嗯嗯..");
    }    
    else if(class<50 && class>=25)
    {
      d.cookskill+=rand()%3+3;    
      d.homework+=rand()%3+3;
      d.family+=rand()%3+2;
      d.relation+=rand()%3+2;
      workmoney=40+(d.cookskill*2+d.homework+d.family)/50;
      pressanykey("家事普普通通啦..可以更好的..加油..");      
    }    
    else if(class<25)
    {
      d.cookskill+=rand()%3+1;    
      d.homework+=rand()%3+1;
      d.family+=rand()%3+1;
      d.relation+=rand()%3+1;
      workmoney=20+(d.cookskill*2+d.homework+d.family)/60;
      pressanykey("家事很糟糕喔..這樣不行啦..");      
    }        
    d.money+=workmoney;
    d.workA+=1;
    return 0;
}

int pip_job_workB()
{   
/*  ├────┼──────────────────────┤*/
/*  │育幼院  │母性 + N , 感受 + 1 , 魅力 - 1 , 疲勞 + 3   │*/
/*  ├────┼──────────────────────┤*/
/*  │育幼院  │若 體    力 - RND (疲勞) >=  20 則工作成功  │*/
/*  ├────┼──────────────────────┤*/
    float class;
    long workmoney;
    
    workmoney=0;       
    class=(d.hp*100/d.maxhp)-d.tired;        
    d.maxhp+=rand()%2+1;
    d.shit+=rand()%3+5;
    d.affect+=rand()%3+4;
    
    count_tired(3,9,"Y",100,1);
    d.hp-=(rand()%3+6);
    d.happy-=(rand()%3+4);
    d.satisfy-=rand()%3+4;
    d.charm-=rand()%3+4;
    if(d.charm<=0)
       d.charm=0;
    show_job_pic(21);
    if(class>=90)
    {
      d.love+=rand()%2+7;
      d.toman+=rand()%2+2;
      workmoney=150+(d.love+d.toman)/50;
      pressanykey("當保姆很成功\喔..下次再來喔..");
    }
    else if(class<90 && class>=75)
    {
      d.love+=rand()%2+5;
      d.toman+=rand()%2+2;
      workmoney=120+(d.love+d.toman)/50;
      pressanykey("保姆還當的不錯唷..嗯嗯..");
    }    
    else if(class<75 && class>=50)
    {
      d.love+=rand()%2+3;
      d.toman+=rand()%2+1;
      workmoney=100+(d.love+d.toman)/50;
      pressanykey("小朋友很皮喔..加油..");      
    }    
    else if(class<50)
    {
      d.love+=rand()%2+1;
      d.toman+=rand()%2+1;
      workmoney=80+(d.love+d.toman)/50;
      pressanykey("很糟糕喔..你罩不住小朋友耶...");      
    }        
    d.money+=workmoney;
    d.workB+=1;
    return 0;
}
             
int pip_job_workC()
{
/*  ├────┼──────────────────────┤*/
/*  │旅館    │掃地洗衣 + N , 戰鬥技術 - N , 疲勞 + 2      │*/
/*  ├────┼──────────────────────┤*/
/*  │旅館    │若 體    力 - RND (疲勞) >=  30 則工作成功  │*/
/*  ├────┼──────────────────────┤*/
    float class;
    long workmoney;
    
    workmoney=0;       
    class=(d.hp*100/d.maxhp)-d.tired;        
    d.maxhp+=rand()%2+2;
    d.shit+=rand()%3+5;
    count_tired(5,12,"Y",100,1);
    d.hp-=(rand()%4+8);
    d.happy-=(rand()%3+4);
    d.satisfy-=rand()%3+4;
    show_job_pic(31);
    if(class>=95)
    {
      d.homework+=rand()%2+7;
      d.family+=rand()%2+4;
      d.hskill-=rand()%2+7;
      if(d.hskill<0)
           d.hskill=0;
      workmoney=250+(d.cookskill*2+d.homework*2)/40;
      pressanykey("旅館事業蒸蒸日上..希望你再過來...");
    }
    else if(class<95 && class>=80)
    {
      d.homework+=rand()%2+5;
      d.family+=rand()%2+3;
      d.hskill-=rand()%2+5;
      if(d.hskill<0)
           d.hskill=0;
      workmoney=200+(d.cookskill*2+d.homework*2)/50;    
      pressanykey("旅館還蠻順利的唷..嗯嗯..");
    }    
    else if(class<80 && class>=60)
    {
      d.homework+=rand()%2+3;
      d.family+=rand()%2+3;
      d.hskill-=rand()%2+5;
      if(d.hskill<0)
           d.hskill=0;
      workmoney=150+(d.cookskill*2+d.homework*2)/50;    
      pressanykey("普普通通啦..可以更好的..加油..");      
    }    
    else if(class<60)
    {
      d.homework+=rand()%2+1;
      d.family+=rand()%2+1;
      d.hskill-=rand()%2+1;
      if(d.hskill<0)
           d.hskill=0;
      workmoney=100+(d.cookskill*2+d.homework*2)/50;    
      pressanykey("這個很糟糕喔..你這樣不行啦..");      
    }        
    d.money+=workmoney;
    d.workC+=1;
    return 0; 
}  

int pip_job_workD()
{    
/*  ├────┼──────────────────────┤*/
/*  │農場    │體力 + 1 , 腕力 + 1 , 氣質 - 1 , 疲勞 + 3   │*/
/*  ├────┼──────────────────────┤*/
/*  │農場    │若 體    力 - RND (疲勞) >=  30 則工作成功  │*/
/*  ├────┼──────────────────────┤*/    
    float class;
    long workmoney;
    
    workmoney=0;       
    class=(d.hp*100/d.maxhp)-d.tired;        
    d.maxhp+=rand()%3+2;
    d.wrist+=rand()%2+2;
    d.shit+=rand()%5+10;
    count_tired(5,15,"Y",100,1);
    d.hp-=(rand()%4+10);
    d.happy-=(rand()%3+4);
    d.satisfy-=rand()%3+4;
    d.character-=rand()%3+4;
    if(d.character<0)
       d.character=0;
    show_job_pic(41);
    if(class>=95)
    {
      workmoney=250+(d.wrist*2+d.hp*2)/80;
      pressanykey("牛羊長的好好喔..希望你再來幫忙...");
    }
    else if(class<95 && class>=80)
    {
      workmoney=210+(d.wrist*2+d.hp*2)/80;
      pressanykey("呵呵..還不錯喔..:)");
    }    
    else if(class<80 && class>=60)
    {
      workmoney=160+(d.wrist*2+d.hp*2)/80;
      pressanykey("普普通通啦..可以更好的..");      
    }    
    else if(class<60)
    {
      workmoney=120+(d.wrist*2+d.hp*2)/80;
      pressanykey("你不太適合農場的工作  -_-...");      
    }        
    d.money+=workmoney;
    d.workD+=1;
    return 0;
}   
         
int pip_job_workE()
{
/*  ├────┼──────────────────────┤*/
/*  │餐廳    │料理 + N , 戰鬥技術 - N , 疲勞 + 2          │*/
/*  ├────┼──────────────────────┤*/
/*  │餐廳    │若 烹飪技術 - RND (疲勞) >=  50 則工作成功  │*/
/*  ├────┼──────────────────────┤*/
    float class;
    long workmoney;
    
    workmoney=0;       
    class=d.cookskill-d.tired;        
    d.maxhp+=rand()%2+1;
    d.shit+=rand()%4+12;
    count_tired(5,9,"Y",100,1);
    d.hp-=(rand()%4+8);
    d.happy-=(rand()%3+4);
    d.satisfy-=rand()%3+4;
    show_job_pic(51);
    if(class>=80)
    {
      d.homework+=rand()%2+1;
      d.family+=rand()%2+1;
      d.hskill-=rand()%2+5;
      if(d.hskill<0)
           d.hskill=0;
      d.cookskill+=rand()%2+6;
      workmoney=250+(d.cookskill*2+d.homework*2+d.family*2)/80;        
      pressanykey("客人都說太好吃了..再來一盤吧...");
    }
    else if(class<80 && class>=60)
    {
      d.homework+=rand()%2+1;
      d.family+=rand()%2+1;
      d.hskill-=rand()%2+5;
      if(d.hskill<0)
           d.hskill=0;
      d.cookskill+=rand()%2+4;
      workmoney=200+(d.cookskill*2+d.homework*2+d.family*2)/80;        
      pressanykey("煮的還不錯吃唷..:)");
    }    
    else if(class<60 && class>=30)
    {
      d.homework+=rand()%2+1;
      d.family+=rand()%2+1;
      d.hskill-=rand()%2+5;
      if(d.hskill<0)
           d.hskill=0;
      d.cookskill+=rand()%2+2;
      workmoney=150+(d.cookskill*2+d.homework*2+d.family*2)/80;            
      pressanykey("普普通通啦..可以更好的..");      
    }    
    else if(class<30)
    {
      d.homework+=rand()%2+1;
      d.family+=rand()%2+1;
      d.hskill-=rand()%2+5;
      if(d.hskill<0)
           d.hskill=0;
      d.cookskill+=rand()%2+1;
      workmoney=100+(d.cookskill*2+d.homework*2+d.family*2)/80;            
      pressanykey("你的廚藝待加強喔...");      
    }        
    d.money+=workmoney;
    d.workE+=1;
    return 0;
}     

int pip_job_workF()
{           
/*  ├────┼──────────────────────┤*/
/*  │教堂    │信仰 + 2 , 道德 + 1 , 罪孽 - 2 , 疲勞 + 1   │*/
/*  ├────┼──────────────────────┤*/
    float class;
    long workmoney;
    
    workmoney=0;
    class=(d.hp*100/d.maxhp)-d.tired;
    count_tired(5,7,"Y",100,1);
    d.love+=rand()%3+4;
    d.belief+=rand()%4+7;
    d.etchics+=rand()%3+7;
    d.shit+=rand()%3+3;
    d.hp-=rand()%3+5;
    d.offense-=rand()%4+7;
    if(d.offense<0)
       d.offense=0;
    show_job_pic(61);
    if(class>=75)
    {
      workmoney=100+(d.belief+d.etchics-d.offense)/20;
      pressanykey("錢很少 但看你這麼認真 給你多一點...");
    }
    else if(class<75 && class>=50)
    {
      workmoney=75+(d.belief+d.etchics-d.offense)/20;
      pressanykey("謝謝你的熱心幫忙..:)");
    }    
    else if(class<50 && class>=25)
    {
      workmoney=50+(d.belief+d.etchics-d.offense)/20;
      pressanykey("你真的很有愛心啦..不過有點小累的樣子...");      
    }    
    else if(class<25)
    {
      workmoney=25+(d.belief+d.etchics-d.offense)/20;
      pressanykey("來奉獻不錯..但也不能打混ㄚ....:(");      
    }        
    d.money+=workmoney;
    d.workF+=1;
    return 0;
}               

int pip_job_workG()
{   
/* ├────┼──────────────────────┤*/
/* │地攤    │體力 + 2 , 魅力 + 1 , 疲勞 + 3 ,談吐 +1     │*/
/* ├────┼──────────────────────┤*/   
    long workmoney;
    
    workmoney=0;
    workmoney=200+(d.charm*3+d.speech*2+d.toman)/50;
    count_tired(3,12,"Y",100,1);
    d.shit+=rand()%3+8;
    d.speed+=rand()%2;
    d.weight-=rand()%2;
    d.happy-=(rand()%3+7);
    d.satisfy-=rand()%3+5;
    d.hp-=(rand()%6+6);
    d.charm+=rand()%2+3;
    d.speech+=rand()%2+3;
    d.toman+=rand()%2+3;
    move(4,0);
    show_job_pic(71);
    pressanykey("擺\地攤要躲警察啦..:p");
    d.money+=workmoney;
    d.workG+=1;
    return 0;
}   
    
int pip_job_workH()
{
/*  ├────┼──────────────────────┤*/
/*  │伐木場  │腕力 + 2 , 氣質 - 2 , 疲勞 + 4              │*/
/*  ├────┼──────────────────────┤*/
/*  │伐木場  │若 腕    力 - RND (疲勞) >=  80 則工作成功  │*/
/*  ├────┼──────────────────────┤*/
    float class;
    long workmoney;
    
    if((d.bbtime/60/30)<1) /*一歲才行*/
    {
      pressanykey("小雞太小了,一歲以後再來吧...");
      return 0;
    }
    workmoney=0;       
    class=d.wrist-d.tired;        
    d.maxhp+=rand()%2+3;
    d.shit+=rand()%7+15;
    d.wrist+=rand()%3+4;
    count_tired(5,15,"Y",100,1);
    d.hp-=(rand()%4+10);
    d.happy-=(rand()%3+4);
    d.satisfy-=rand()%3+4;
    d.character-=rand()%3+7;
    if(d.character<0)
       d.character=0;    
    show_job_pic(81);
    if(class>=70)
    {
      workmoney=350+d.wrist/20+d.maxhp/80;        
      pressanykey("你腕力很好唷..:)");
    }
    else if(class<70 && class>=50)
    {
      workmoney=300+d.wrist/20+d.maxhp/80;
      pressanykey("砍了不少樹喔.....:)");
    }    
    else if(class<50 && class>=20)
    {
      workmoney=250+d.wrist/20+d.maxhp/80;
      pressanykey("普普通通啦..可以更好的..");      
    }    
    else if(class<20)
    {
      workmoney=200+d.wrist/20+d.maxhp/80;
      pressanykey("待加強喔..鍛鍊再來吧....");      
    }        
    d.money+=workmoney;
    d.workH+=1;
    return 0;     
}

int pip_job_workI()
{
/*  ├────┼──────────────────────┤*/
/*  │美容院  │感受 + 1 , 腕力 - 1 , 疲勞 + 3              │*/
/*  ├────┼──────────────────────┤*/
/*  │美容院  │若 藝術修養 - RND (疲勞) >=  40 則工作成功  │*/
/*  ├────┼──────────────────────┤*/
    float class;
    long workmoney;

    if((d.bbtime/60/30)<1) /*一歲才行*/
    {
      pressanykey("小雞太小了,一歲以後再來吧...");
      return 0;
    }
    workmoney=0;    
    class=d.art-d.tired;        
    d.maxhp+=rand()%2;
    d.affect+=rand()%2+3;
    count_tired(3,11,"Y",100,1);
    d.shit+=rand()%4+8;
    d.hp-=(rand()%4+10);
    d.happy-=(rand()%3+4);
    d.satisfy-=rand()%3+4;
    d.wrist-=rand()%+3;
    if(d.wrist<0)
       d.wrist=0;
    /*show_job_pic(4);*/
    if(class>=80)
    {
      workmoney=400+d.art/10+d.affect/20;        
      pressanykey("客人都很喜歡讓你做造型唷..:)");
    }
    else if(class<80 && class>=60)
    {
      workmoney=360+d.art/10+d.affect/20;        
      pressanykey("做的不錯喔..頗有天份...:)");
    }    
    else if(class<60 && class>=40)
    {
      workmoney=320+d.art/10+d.affect/20;        
      pressanykey("馬馬虎虎啦..再加油一點..");      
    }    
    else if(class<40)
    {
      workmoney=250+d.art/10+d.affect/20;        
      pressanykey("待加強喔..以後再來吧....");      
    }        
    d.money+=workmoney;
    d.workI+=1;
    return 0;
}

int pip_job_workJ()
{           
/*  ├────┼──────────────────────┤*/
/*  │狩獵區  │體力 + 1 , 氣質 - 1 , 母性 - 1 , 疲勞 + 3   │*/
/*  │        │戰鬥技術 + N                                │*/
/*  ├────┼──────────────────────┤*/
/*  │狩獵區  │若 體    力 - RND (疲勞) >=  80 ＆          │*/
/*  │        │若 智    力 - RND (疲勞) >=  40 則工作成功  │*/
/*  ├────┼──────────────────────┤*/
    float class;
    float class1;
    long workmoney;

    /*兩歲以上才行*/
    if((d.bbtime/60/30)<2)
    {
      pressanykey("小雞太小了,兩歲以後再來吧...");
      return 0;
    }          
    workmoney=0;
    class=(d.hp*100/d.maxhp)-d.tired;            
    class1=d.wisdom-d.tired;
    count_tired(5,15,"Y",100,1);
    d.shit+=rand()%4+13;
    d.weight-=(rand()%2+1);
    d.maxhp+=rand()%2+3;
    d.speed+=rand()%2+3;
    d.hp-=(rand()%6+8);
    d.character-=rand()%3+4;
    d.happy-=rand()%5+8;
    d.satisfy-=rand()%5+6;
    d.love-=rand()%3+4;
    if(d.character<0)
       d.character=0;
    if(d.love<0)
       d.love=0;
    move(4,0);
    show_job_pic(101);    
    if(class>=80 && class1>=80)
    {
       d.hskill+=rand()%2+7;
       workmoney=300+d.maxhp/50+d.hskill/20;
       pressanykey("你是完美的獵人..");
    }
    else if((class<75 && class>=50) && class1>=60 )  
    {
       d.hskill+=rand()%2+5;
       workmoney=270+d.maxhp/45+d.hskill/20;   
       pressanykey("收獲還不錯喔..可以飽餐\一頓了..:)");
    }
    else if((class<50 && class>=25) && class1 >=40 )
    {
       d.hskill+=rand()%2+3;
       workmoney=240+d.maxhp/40+d.hskill/20;
       pressanykey("技術差強人意  再加油喔..");
    }    
    else if((class<25 && class>=0) && class1 >=20)  
    {
       d.hskill+=rand()%2+1;
       workmoney=210+d.maxhp/30+d.hskill/20;    
       pressanykey("狩獵是體力與智力的結合....");
    }    
    else if(class<0)
    {
       d.hskill+=rand()%2;
       workmoney=190+d.hskill/20;
       pressanykey("要多多鍛鍊和增進智慧啦....");
    }    
    d.money=d.money+workmoney;        
    d.workJ+=1;
    return 0;   
}

int pip_job_workK()
{ 
/* ├────┼──────────────────────┤*/
/* │工地    │體力 + 2 , 魅力 - 1 , 疲勞 + 3              │*/
/* ├────┼──────────────────────┤*/
    float class;
    long workmoney;
    
    /*兩歲以上才行*/
    if((d.bbtime/60/30)<2)
    {
      pressanykey("小雞太小了,兩歲以後再來吧...");
      return 0;
    }
    workmoney=0;
    class=(d.hp*100/d.maxhp)-d.tired;            
    count_tired(5,15,"Y",100,1);
    d.shit+=rand()%4+16;
    d.weight-=(rand()%2+2);
    d.maxhp+=rand()%2+1;
    d.speed+=rand()%2+2;
    d.hp-=(rand()%6+10);
    d.charm-=rand()%3+6;
    d.happy-=(rand()%5+10);
    d.satisfy-=rand()%5+6;
    if(d.charm<0)
       d.charm=0;
    move(4,0);
    show_job_pic(111);    
    if(class>=75)
    {
       workmoney=250+d.maxhp/50;
       pressanykey("工程很完美  謝謝你了..");
    }
    else if(class<75 && class>=50)  
    {
       workmoney=220+d.maxhp/45;    
       pressanykey("工程尚稱順利  辛苦你了..");
    }
    else if(class<50 && class>=25)  
    {
       workmoney=200+d.maxhp/40;    
       pressanykey("工程差強人意  再加油喔..");
    }    
    else if(class<25 && class>=0)  
    {
       workmoney=180+d.maxhp/30;    
       pressanykey("ㄜ  待加強待加強....");
    }    
    else if(class<0)
    {
       workmoney=160;
       pressanykey("下次體力好一點..疲勞度低一點再來....");
    }
    
    d.money=d.money+workmoney;
    d.workK+=1;
    return 0;
}

int pip_job_workL()
{   
/*  ├────┼──────────────────────┤*/
/*  │墓園    │抗魔能力 + N , 感受 + 1 , 魅力 - 1          │*/
/*  │        │疲勞 + 2                                    │*/
/*  ├────┼──────────────────────┤*/
    float class;
    float class1;
    long workmoney;
    
    /*三歲才行*/
    if((d.bbtime/60/30)<3)
    {
      pressanykey("小雞現在還太小了,三歲以後再來吧...");
      return 0;
    }
    workmoney=0;
    class=(d.hp*100/d.maxhp)-d.tired;
    class1=d.belief-d.tired;
    d.shit+=rand()%5+8;
    d.maxmp+=rand()%2;
    d.affect+=rand()%2+2;    
    d.brave+=rand()%2+2;    
    count_tired(5,12,"Y",100,1);    
    d.hp-=(rand()%3+7);
    d.happy-=(rand()%4+6);
    d.satisfy-=rand()%3+5;    
    d.charm-=rand()%3+6;         
    if(d.charm<0)
       d.charm=0;
    show_job_pic(121);                
    if(class>=75 && class1>=75)
    {
      d.mresist+=rand()%2+7;
      workmoney=200+(d.affect+d.brave)/40;
      pressanykey("守墓成功\喔  給你多點錢");
    }
    else if((class<75 && class>=50)&& class1 >=50)
    {
      d.mresist+=rand()%2+5;
      workmoney=150+(d.affect+d.brave)/50;      
      pressanykey("守墓還算成功\喔..謝啦..");    
    }    
    else if((class<50 && class>=25)&& class1 >=25)
    {
      d.mresist+=rand()%2+3;
      workmoney=120+(d.affect+d.brave)/60;
      pressanykey("守墓還算差強人意喔..加油..");    
    }    
    else
    {
      d.mresist+=rand()%2+1;
      workmoney=80+(d.affect+d.brave)/70;
      pressanykey("我也不方便說啥了..請再加油..");    
    }           
    if(rand()%10==5)
    {
       pressanykey("真是倒楣  竟遇到死神魔..");
       pip_fight_bad(12);
    }
    d.money+=workmoney;
    d.workL+=1;
    return 0;   
}

int pip_job_workM()
{
/*  ├────┼──────────────────────┤*/
/*  │家庭教師│道德 + 1 , 母性 + N , 魅力 - 1 , 疲勞 + 7   │*/
/*  ├────┼──────────────────────┤*/
    float class;
    long workmoney;    

    if((d.bbtime/60/30)<4)
    {
      pressanykey("小雞太小了,四歲以後再來吧...");
      return 0;
    }    
    workmoney=0;
    class=(d.hp*100/d.maxhp)-d.tired;            
    workmoney=50+d.wisdom/20+d.character/20;
    count_tired(5,10,"Y",100,1);
    d.shit+=rand()%3+8;
    d.character+=rand()%2;
    d.wisdom+=rand()%2;
    d.happy-=(rand()%3+6);
    d.satisfy-=rand()%3+5;
    d.hp-=(rand()%3+8);
    d.money=d.money+workmoney;
    move(4,0);
    show_job_pic(131);
    pressanykey("家教輕鬆 當然錢就少一點囉");
    d.workM+=1;
    return;
}

int pip_job_workN()
{   
/*  ├────┼──────────────────────┤*/
/*  │酒店    │烹飪技巧 + N , 談話技巧 + N , 智力 - 2      │*/
/*  │        │疲勞 + 5                                    │*/
/*  ├────┼──────────────────────┤*/
/*  │酒店    │若 體    力 - RND (疲勞) >=  60 ＆          │*/
/*  │        │若 魅    力 - RND (疲勞) >=  50 則工作成功  │*/
/*  ├────┼──────────────────────┤*/
    float class;
    float class1;
    long workmoney;
    
    if((d.bbtime/60/30)<5)
    {
      pressanykey("小雞太小了,五歲以後再來吧...");
      return 0;
    }         
    workmoney=0;
    class=(d.hp*100/d.maxhp)-d.tired;
    class1=d.charm-d.tired;
    d.shit+=rand()%5+5;
    count_tired(5,14,"Y",100,1);
    d.hp-=(rand()%3+5);
    d.social-=rand()%5+6;
    d.happy-=(rand()%4+6);
    d.satisfy-=rand()%3+5;    
    d.wisdom-=rand()%3+4;         
    if(d.wisdom<0)
       d.wisdom=0;
    /*show_job_pic(6);*/
    if(class>=75 && class1>=75)
    {
      d.cookskill+=rand()%2+7;
      d.speech+=rand()%2+5;
      workmoney=500+(d.charm)/5;
      pressanykey("你很紅唷  :)");
    }
    else if((class<75 && class>=50)&& class1 >=50)
    {
      d.cookskill+=rand()%2+5;
      d.speech+=rand()%2+5;
      workmoney=400+(d.charm)/5;    
      pressanykey("蠻受歡迎的耶....");    
    }    
    else if((class<50 && class>=25)&& class1 >=25)
    {
      d.cookskill+=rand()%2+4;
      d.speech+=rand()%2+3;
      workmoney=300+(d.charm)/5;    
      pressanykey("很平凡啦..但馬馬虎虎...");    
    }    
    else
    {
      d.cookskill+=rand()%2+2;
      d.speech+=rand()%2+2;
      workmoney=200+(d.charm)/5;    
      pressanykey("你的媚力不夠啦..請加油....");    
    }           
    d.money+=workmoney;
    d.workN+=1;
    return 0;        
}

int pip_job_workO()
{         
/*  ├────┼──────────────────────┤*/
/*  │酒家    │魅力 + 2 , 罪孽 + 2 , 道德 - 3 , 信仰 - 3   │*/
/*  │        │待人接物 - N , 和父親的關係 - N , 疲勞 + 12 │*/
/*  ├────┼──────────────────────┤*/
/*  │酒家    │若 魅    力 - RND (疲勞) >=  70 則工作成功  │*/
/*  ├────┼──────────────────────┤*/
    float class;
    long workmoney;
    
    if((d.bbtime/60/30)<4)
    {
      pressanykey("小雞太小了,四歲以後再來吧...");
      return 0;
    }                 
    workmoney=0;
    class=d.charm-d.tired;
    d.shit+=rand()%5+14;
    d.charm+=rand()%3+8;
    d.offense+=rand()%3+8;
    count_tired(5,22,"Y",100,1);
    d.hp-=(rand()%3+8);
    d.social-=rand()%6+12;
    d.happy-=(rand()%4+8);
    d.satisfy-=rand()%3+8;    
    d.etchics-=rand()%6+10;
    d.belief-=rand()%6+10;
    if(d.etchics<0)
       d.etchics=0;
    if(d.belief<0)
       d.belief=0;       
    
    /*show_job_pic(6);*/
    if(class>=75)
    {
      d.relation-=rand()%5+12;
      d.toman-=rand()%5+12;
      workmoney=600+(d.charm)/5;
      pressanykey("你是本店的紅牌唷  :)");
    }
    else if(class<75 && class>=50)
    {
      d.relation-=rand()%5+8;
      d.toman-=rand()%5+8;
      workmoney=500+(d.charm)/5;    
      pressanykey("你蠻受歡迎的耶..:)");    
    }    
    else if(class<50 && class>=25)
    {
      d.relation-=rand()%5+5;
      d.toman-=rand()%5+5;
      workmoney=400+(d.charm)/5;
      pressanykey("你很平凡..但馬馬虎虎啦...");    
    }    
    else
    {
      d.relation-=rand()%5+1;
      d.toman-=rand()%5+1;
      workmoney=300+(d.charm)/5;    
      pressanykey("唉..你的媚力不夠啦....");    
    }           
    d.money+=workmoney;
    if(d.relation<0)
       d.relation=0;
    if(d.toman<0)
       d.toman=0;
    d.workO+=1;
    return 0;
}        
    
int pip_job_workP()
{
/*  ├────┼──────────────────────┤*/
/*  │大夜總會│魅力 + 3 , 罪孽 + 1 , 氣質 - 2 , 智力 - 1   │*/
/*  │        │待人接物 - N , 疲勞 + 8                     │*/
/*  ├────┼──────────────────────┤*/
/*  │大夜總會│若 魅    力 - RND (疲勞) >=  70 ＆          │*/
/*  │        │若 藝術修養 - RND (疲勞) >=  30 則工作成功  │*/
/*  └────┴──────────────────────┘*/
    float class;
    float class1;
    long workmoney;
    
    if((d.bbtime/60/30)<6)
    {
      pressanykey("小雞太小了,六歲以後再來吧...");
      return;
    }                
    workmoney=0;
    class=d.charm-d.tired;
    class1=d.art-d.tired;
    d.shit+=rand()%5+7;
    d.charm+=rand()%3+8;
    d.offense+=rand()%3+8;
    count_tired(5,22,"Y",100,1);
    d.hp-=(rand()%3+8);
    d.social-=rand()%6+12;
    d.happy-=(rand()%4+8);
    d.satisfy-=rand()%3+8;    
    d.character-=rand()%3+8;
    d.wisdom-=rand()%3+5;
    if(d.character<0)
       d.character=0;
    if(d.wisdom<0)
       d.wisdom=0;       
    /*show_job_pic(6);*/
    if(class>=75 && class1>30)
    {
      d.speech+=rand()%5+12;
      d.toman-=rand()%5+12;
      workmoney=1000+(d.charm)/5;
      pressanykey("你是夜總會最閃亮的星星唷  :)");
    }
    else if((class<75 && class>=50) && class1>20)
    {
      d.speech+=rand()%5+8;
      d.toman-=rand()%5+8;
      workmoney=800+(d.charm)/5;    
      pressanykey("嗯嗯..你蠻受歡迎的耶..:)");    
    }    
    else if((class<50 && class>=25) && class1>10)
    {
      d.speech+=rand()%5+5;
      d.toman-=rand()%5+5;
      workmoney=600+(d.charm)/5;
      pressanykey("你要加油了啦..但普普啦...");    
    }    
    else
    {
      d.speech+=rand()%5+1;
      d.toman-=rand()%5+1;
      workmoney=400+(d.charm)/5;    
      pressanykey("唉..你不行啦....");    
    }           
    d.money+=workmoney;
    if(d.toman<0)
       d.toman=0;
    d.workP+=1;
    return;    
}

/*---------------------------------------------------------------------------*/
/* 玩樂選單:散步 旅遊 運動 約會 猜拳                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/

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
   int com;
   int pipkey;
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

/*---------------------------------------------------------------------------*/
/* 修行選單:念書 練武 修行                                                   */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/* 資料庫                                                                    */
/*---------------------------------------------------------------------------*/
char *classrank[6]={"沒有","初級","中級","高級","進階","專業"};
int classmoney[11][2]={{ 0,  0},
		       {60,110},{70,120},{70,120},{80,130},{70,120},
		       {60,110},{90,140},{70,120},{70,120},{80,130}};		     
int classvariable[11][4]={
{0,0,0,0},
{5,5,4,4},{5,7,6,4},{5,7,6,4},{5,6,5,4},{7,5,4,6},
{7,5,4,6},{6,5,4,6},{6,6,5,4},{5,5,4,7},{7,5,4,7}};


char *classword[11][5]=
{
{"課名","成功\一","成功\二","失敗一","失敗二"},
 
{"自然科學","正在用功\讀書中..","我是聰明雞 cccc...",
            "這題怎麼看不懂咧..怪了","唸不完了 :~~~~~~"},
            
{"唐詩宋詞","床前明月光...疑是地上霜...","紅豆生南國..春來發幾枝..",
            "ㄟ..上課不要流口水","你還混喔..罰你背會唐詩三百首"},

{"神學教育","哈雷路亞  哈雷路亞","讓我們迎接天堂之門",
	    "ㄟ..你在幹嘛ㄚ? 還不好好唸","神學很嚴肅的..請好好學..:("},

{"軍學教育","孫子兵法是中國兵法書..","從軍報國，我要帶兵去打仗",
	    "什麼陣形ㄚ?混亂陣形?? @_@","你還以為你在玩三國志ㄚ?"},

{"劍道技術","看我的厲害  獨孤九劍....","我刺 我刺 我刺刺刺..",
	    "劍要拿穩一點啦..","你在刺地鼠ㄚ? 劍拿高一點"},

{"格鬥戰技","肌肉是肌肉  呼呼..","十八銅人行氣散..",
	    "腳再踢高一點啦...","拳頭怎麼這麼沒力ㄚ.."},

{"魔法教育","我變 我變 我變變變..","蛇膽+蟋蜴尾+鼠牙+蟾蜍=??",
	    "小心你的掃帚啦  不要亂揮..","ㄟ∼口水不要流到水晶球上.."},

{"禮儀教育","要當隻有禮貌的雞...","歐嗨唷..ㄚ哩ㄚ豆..",
	    "怎麼學不會ㄚ??天呀..","走起路來沒走樣..天ㄚ.."},

{"繪畫技巧","很不錯唷..有美術天份..","這幅畫的顏色搭配的很好..",
	    "不要鬼畫符啦..要加油..","不要咬畫筆啦..壞壞小雞喔.."},

{"舞蹈技巧","你就像一隻天鵝喔..","舞蹈細胞很好喔..",
            "身體再柔軟一點..","拜託你優美一點..不要這麼粗魯.."}};
/*---------------------------------------------------------------------------*/
/* 修行選單:念書 練武 修行                                                   */
/* 函式庫                                                                    */
/*---------------------------------------------------------------------------*/

int pip_practice_classA()
{
/*  ├────┼──────────────────────┤*/
/*  │自然科學│智力 + 1~ 4 , 信仰 - 0~0 , 抗魔能力 - 0~0   │*/
/*  │        ├──────────────────────┤*/
/*  │        │智力 + 2~ 6 , 信仰 - 0~1 , 抗魔能力 - 0~1   │*/
/*  │        ├──────────────────────┤*/
/*  │        │智力 + 3~ 8 , 信仰 - 0~2 , 抗魔能力 - 0~1   │*/
/*  │        ├──────────────────────┤*/
/*  │        │智力 + 4~12 , 信仰 - 1~3 , 抗魔能力 - 0~1   │*/
/*  ├────┼──────────────────────┤*/
	int body,class;
	int change1,change2,change3,change4,change5;
     
	class=d.wisdom/200+1; /*科學*/
	if(class>5) class=5;

	body=pip_practice_function(1,class,11,12,&change1,&change2,&change3,&change4,&change5);
	if(body==0) return 0;      
	d.wisdom+=change4;
	if(body==1)
	{ 
		d.belief-=rand()%(2+class*2);
		d.mresist-=rand()%4;
	}
	else
	{
		d.belief-=rand()%(2+class*2);
		d.mresist-=rand()%3;      
	}
	pip_practice_gradeup(1,class,d.wisdom/200+1);
	if(d.belief<0)  d.belief=0;
	if(d.mresist<0) d.mresist=0;
	d.classA+=1;
	return 0;
}   

int pip_practice_classB()
{
/*  ├────┼──────────────────────┤*/
/*  │詩詞    │感受 + 1~1 , 智力 + 0~1 , 藝術修養 + 0~1    │*/
/*  │        │氣質 + 0~1                                  │*/
/*  │        ├──────────────────────┤*/
/*  │        │感受 + 1~2 , 智力 + 0~2 , 藝術修養 + 0~1    │*/
/*  │        │氣質 + 0~1                                  │*/
/*  │        ├──────────────────────┤*/
/*  │        │感受 + 1~4 , 智力 + 0~3 , 藝術修養 + 0~1    │*/
/*  │        │氣質 + 0~1                                  │*/
/*  │        ├──────────────────────┤*/
/*  │        │感受 + 2~5 , 智力 + 0~4 , 藝術修養 + 0~1    │*/
/*  │        │氣質 + 0~1                                  │*/
/*  ├────┼──────────────────────┤*/
	int body,class;
	int change1,change2,change3,change4,change5;
     
	class=(d.affect*2+d.wisdom+d.art*2+d.character)/400+1; /*詩詞*/
	if(class>5) class=5;
     
	body=pip_practice_function(2,class,21,21,&change1,&change2,&change3,&change4,&change5);
	if(body==0) return 0;            
	d.affect+=change3;
	if(body==1)
	{ 
		d.wisdom+=rand()%(class+3);
		d.character+=rand()%(class+3);
		d.art+=rand()%(class+3);
	}
	else
	{
		d.wisdom+=rand()%(class+2);
		d.character+=rand()%(class+2);
		d.art+=rand()%(class+2);      
	}
	body=(d.affect*2+d.wisdom+d.art*2+d.character)/400+1;
	pip_practice_gradeup(2,class,body);
	d.classB+=1;
	return 0;
}

int pip_practice_classC()
{
/*  ├────┼──────────────────────┤*/
/*  │神學    │智力 + 1~1 , 信仰 + 1~2 , 抗魔能力 + 0~1    │*/
/*  │        ├──────────────────────┤*/
/*  │        │智力 + 1~1 , 信仰 + 1~3 , 抗魔能力 + 0~1    │*/
/*  │        ├──────────────────────┤*/
/*  │        │智力 + 1~2 , 信仰 + 1~4 , 抗魔能力 + 0~1    │*/
/*  │        ├──────────────────────┤*/
/*  │        │智力 + 1~3 , 信仰 + 1~5 , 抗魔能力 + 0~1    │*/
/*  ├────┼──────────────────────┤*/
	int body,class;
	int change1,change2,change3,change4,change5;
     
	class=(d.belief*2+d.wisdom)/400+1; /*神學*/
	if(class>5) class=5;

	body=pip_practice_function(3,class,31,31,&change1,&change2,&change3,&change4,&change5);
	if(body==0) return 0;            
	d.wisdom+=change2;
	d.belief+=change3;
	if(body==1)
	{ 
		d.mresist+=rand()%5;
	}
	else
	{
		d.mresist+=rand()%3;
	}
	body=(d.belief*2+d.wisdom)/400+1;
	pip_practice_gradeup(3,class,body);
	d.classC+=1;
	return 0;        
}

int pip_practice_classD()
{    
/*  ├────┼──────────────────────┤*/
/*  │軍學    │智力 + 1~2 , 戰鬥技術 + 0~1 , 感受 - 0~1    │*/
/*  │        ├──────────────────────┤*/
/*  │        │智力 + 2~4 , 戰鬥技術 + 0~1 , 感受 - 0~1    │*/
/*  │        ├──────────────────────┤*/
/*  │        │智力 + 3~4 , 戰鬥技術 + 0~1 , 感受 - 0~1    │*/
/*  │        ├──────────────────────┤*/
/*  │        │智力 + 4~5 , 戰鬥技術 + 0~1 , 感受 - 0~1    │*/
/*  ├────┼──────────────────────┤*/
	int body,class;
	int change1,change2,change3,change4,change5;
     
	class=(d.hskill*2+d.wisdom)/400+1;
	if(class>5) class=5;
	body=pip_practice_function(4,class,41,41,&change1,&change2,&change3,&change4,&change5);
	if(body==0) return 0;            
	d.wisdom+=change2;
	if(body==1)
	{ 
		d.hskill+=rand()%3+4;
		d.affect-=rand()%3+6;
	}
	else
	{
		d.hskill+=rand()%3+2;
		d.affect-=rand()%3+6;
	}
	body=(d.hskill*2+d.wisdom)/400+1;
	pip_practice_gradeup(4,class,body);
	if(d.affect<0)  d.affect=0;
	d.classD+=1;
	return 0;    
}

int pip_practice_classE()
{ 
/*  ├────┼──────────────────────┤*/
/*  │劍術    │戰鬥技術 + 0~1 , 攻擊能力 + 1~1             │*/
/*  │        ├──────────────────────┤*/
/*  │        │戰鬥技術 + 0~1 , 攻擊能力 + 1~2             │*/
/*  │        ├──────────────────────┤*/
/*  │        │戰鬥技術 + 0~1 , 攻擊能力 + 1~3             │*/
/*  │        ├──────────────────────┤*/
/*  │        │戰鬥技術 + 0~1 , 攻擊能力 + 1~4             │*/
/*  ├────┼──────────────────────┤*/
	int body,class;
	int change1,change2,change3,change4,change5;
     
	class=(d.hskill+d.attack)/400+1;
	if(class>5) class=5;
     
	body=pip_practice_function(5,class,51,51,&change1,&change2,&change3,&change4,&change5);
	if(body==0) return 0;      
	d.speed+=rand()%3+2;
	d.hexp+=rand()%2+2;
	d.attack+=change4;
	if(body==1)
	{ 
		d.hskill+=rand()%3+5;
	}
	else
	{
		d.hskill+=rand()%3+3;
	}
	body=(d.hskill+d.attack)/400+1;
	pip_practice_gradeup(5,class,body);
	d.classE+=1;
	return 0;        
}

int pip_practice_classF()
{     
/*  ├────┼──────────────────────┤*/
/*  │格鬥術  │戰鬥技術 + 1~1 , 防禦能力 + 0~0             │*/
/*  │        ├──────────────────────┤*/
/*  │        │戰鬥技術 + 1~1 , 防禦能力 + 0~1             │*/
/*  │        ├──────────────────────┤*/
/*  │        │戰鬥技術 + 1~2 , 防禦能力 + 0~1             │*/
/*  │        ├──────────────────────┤*/
/*  │        │戰鬥技術 + 1~3 , 防禦能力 + 0~1             │*/
/*  ├────┼──────────────────────┤*/
	int body,class;
	int change1,change2,change3,change4,change5;
     
	class=(d.hskill+d.resist)/400+1;
	if(class>5) class=5;
     
	body=pip_practice_function(6,class,61,61,&change1,&change2,&change3,&change4,&change5);
	if(body==0) return 0;
	d.hexp+=rand()%2+2;
	d.speed+=rand()%3+2;
	d.resist+=change2;
	if(body==1)
	{ 
		d.hskill+=rand()%3+5;
	}
	else
	{
		d.hskill+=rand()%3+3;
	}
	body=(d.hskill+d.resist)/400+1;
	pip_practice_gradeup(6,class,body);
	d.classF+=1;
	return 0;             
}

int pip_practice_classG()
{     
/*  ├────┼──────────────────────┤*/
/*  │魔法    │魔法技術 + 1~1 , 魔法能力 + 0~2             │*/
/*  │        ├──────────────────────┤*/
/*  │        │魔法技術 + 1~2 , 魔法能力 + 0~3             │*/
/*  │        ├──────────────────────┤*/
/*  │        │魔法技術 + 1~3 , 魔法能力 + 0~4             │*/
/*  │        ├──────────────────────┤*/
/*  │        │魔法技術 + 2~4 , 魔法能力 + 0~5             │*/
/*  ├────┼──────────────────────┤*/
	int body,class;
	int change1,change2,change3,change4,change5;

	class=(d.mskill+d.maxmp)/400+1;
	if(class>5) class=5;

	body=pip_practice_function(7,class,71,72,&change1,&change2,&change3,&change4,&change5);
	if(body==0) return 0;     
	d.maxmp+=change3;
	d.mexp+=rand()%2+2;
	if(body==1)
	{ 
		d.mskill+=rand()%3+7;
	}
	else
	{
		d.mskill+=rand()%3+4;
	}
	
	body=(d.mskill+d.maxmp)/400+1;
	pip_practice_gradeup(7,class,body);	
	d.classG+=1;
	return 0;                  
}

int pip_practice_classH()
{     
/*  ├────┼──────────────────────┤*/
/*  │禮儀    │禮儀表現 + 1~1 , 氣質 + 1~1                 │*/
/*  │        ├──────────────────────┤*/
/*  │        │禮儀表現 + 1~2 , 氣質 + 1~2                 │*/
/*  │        ├──────────────────────┤*/
/*  │        │禮儀表現 + 1~3 , 氣質 + 1~3                 │*/
/*  │        ├──────────────────────┤*/
/*  │        │禮儀表現 + 2~4 , 氣質 + 1~4                 │*/
/*  ├────┼──────────────────────┤*/    
	int body,class;
	int change1,change2,change3,change4,change5;

	class=(d.manners*2+d.character)/400+1;
	if(class>5) class=5;
	
	body=pip_practice_function(8,class,0,0,&change1,&change2,&change3,&change4,&change5);
	if(body==0) return 0;     
	d.social+=rand()%2+2;
	d.manners+=change1+rand()%2;
	d.character+=change1+rand()%2;
	body=(d.character+d.manners)/400+1;
	pip_practice_gradeup(8,class,body);
	d.classH+=1;
	return 0;  
}

int pip_practice_classI()
{          
/*  ├────┼──────────────────────┤*/
/*  │繪畫    │藝術修養 + 1~1 , 感受 + 0~1                 │*/
/*  │        ├──────────────────────┤*/
/*  │        │藝術修養 + 1~2 , 感受 + 0~1                 │*/
/*  │        ├──────────────────────┤*/
/*  │        │藝術修養 + 1~3 , 感受 + 0~1                 │*/
/*  │        ├──────────────────────┤*/
/*  │        │藝術修養 + 2~4 , 感受 + 0~1                 │*/
/*  ├────┼──────────────────────┤*/
	int body,class;
	int change1,change2,change3,change4,change5;

	class=(d.art*2+d.character)/400+1;
	if(class>5) class=5;
     
	body=pip_practice_function(9,class,91,91,&change1,&change2,&change3,&change4,&change5);
	if(body==0) return 0;
	d.art+=change4;
	d.affect+=change2;
	body=(d.affect+d.art)/400+1;
	pip_practice_gradeup(9,class,body);
	d.classI+=1;
	return 0;        
}

int pip_practice_classJ()
{    
/*  ├────┼──────────────────────┤*/
/*  │舞蹈    │藝術修養 + 0~1 , 魅力 + 0~1 , 體力 + 1~1    │*/
/*  │        ├──────────────────────┤*/
/*  │        │藝術修養 + 1~1 , 魅力 + 0~1 , 體力 + 1~1    │*/
/*  │        ├──────────────────────┤*/
/*  │        │藝術修養 + 1~2 , 魅力 + 0~2 , 體力 + 1~1    │*/
/*  │        ├──────────────────────┤*/
/*  │        │藝術修養 + 1~3 , 魅力 + 1~2 , 體力 + 1~1    │*/
/*  └────┴──────────────────────┘*/
	int body,class;
	int change1,change2,change3,change4,change5;

	class=(d.art*2+d.charm)/400+1;
	if(class>5) class=5;

	body=pip_practice_function(10,class,0,0,&change1,&change2,&change3,&change4,&change5);
	if(body==0) return 0;
	d.art+=change2;
	d.maxhp+=rand()%3+2;
	if(body==1)
	{ 
		d.charm+=rand()%(4+class);
	}
	else if(body==2)
	{
		d.charm+=rand()%(2+class);
	}
	body=(d.art*2+d.charm)/400+1;
	pip_practice_gradeup(10,class,body);
	d.classJ+=1;
	return 0;            
}

/*傳入:課號 等級 生命 快樂 滿足 髒髒 傳回:變數12345 return:body*/
int
pip_practice_function(classnum,classgrade,pic1,pic2,change1,change2,change3,change4,change5)
int classnum,classgrade,pic1,pic2;
int *change1,*change2,*change3,*change4,*change5;
{
	int  a,b,body,health;
	char inbuf[256],ans[5];
	long smoney;

	/*錢的算法*/
	smoney=classgrade*classmoney[classnum][0]+classmoney[classnum][1];
	move(b_lines-2, 0);
	clrtoeol();
	sprintf(inbuf,"[%8s%4s課程]要花 $%d ,確定要嗎??[y/N]",classword[classnum][0],classrank[classgrade],smoney);
	getdata(b_lines-2, 1,inbuf, ans, 2, 1, 0);
	if(ans[0]!='y' && ans[0]!='Y')  return 0;
	if(d.money<smoney)
	{
		pressanykey("很抱歉喔...你的錢不夠喔");
		return 0;
	}
	count_tired(4,5,"Y",100,1);
	d.money=d.money-smoney;
	/*成功與否的判斷*/
	health=d.hp*1/2+rand()%20 - d.tired;
	if(health>0) body=1;
	else body=2;

	a=rand()%3+2;
	b=(rand()%12+rand()%13)%2;
	d.hp-=rand()%(3+rand()%3)+classvariable[classnum][0];
	d.happy-=rand()%(3+rand()%3)+classvariable[classnum][1];
	d.satisfy-=rand()%(3+rand()%3)+classvariable[classnum][2];
	d.shit+=rand()%(3+rand()%3)+classvariable[classnum][3];
	*change1=rand()%a+2+classgrade*2/(body+1);	/* rand()%3+3 */
	*change2=rand()%a+4+classgrade*2/(body+1);	/* rand()%3+5 */
	*change3=rand()%a+5+classgrade*3/(body+1);	/* rand()%3+7 */
	*change4=rand()%a+7+classgrade*3/(body+1);	/* rand()%3+9 */
	*change5=rand()%a+9+classgrade*3/(body+1);	/* rand()%3+11 */
	if(rand()%2>0 && pic1>0)
		show_practice_pic(pic1);
	else if(pic2>0)
		show_practice_pic(pic2);
	pressanykey(classword[classnum][body+b]);
	return body;	
}

int pip_practice_gradeup(classnum,classgrade,data)
int classnum,classgrade,data;
{
	char inbuf[256];
	
	if((data==(classgrade+1)) && classgrade<5 )
	{
		sprintf(inbuf,"下次換上 [%8s%4s課程]",
		classword[classnum][0],classrank[classgrade+1]);        
		pressanykey(inbuf);
	}
	return 0;
}

/*---------------------------------------------------------------------------*/
/* 特殊選單:看病 減肥 戰鬥 拜訪 朝見                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/


int pip_see_doctor()	/*看醫生*/
{
    char buf[256];
    long savemoney;
    savemoney=d.sick*25;
    if(d.sick<=0)
    {
    pressanykey("哇哩..沒病來醫院幹嘛..被罵了..嗚~~");
    d.character-=(rand()%3+1);
    if(d.character<0)
      d.character=0;
    d.happy-=(rand()%3+3);
    d.satisfy-=rand()%3+2;
    }    
    else if(d.money < savemoney)
    {
     sprintf(buf,"你的病要花 %d 元喔....你不夠錢啦...",savemoney);    
     pressanykey(buf);
    }
    else if(d.sick>0 && d.money >=savemoney)
    {
    d.tired-=rand()%10+20;
    if(d.tired<0)
       d.tired=0;
    d.sick=0;
    d.money=d.money-savemoney;
    move(4,0);
    show_special_pic(1);
    pressanykey("藥到病除..沒有副作用!!");
    }
    return 0;
}

/*減肥*/
int pip_change_weight()
{
    char genbuf[5];
    char inbuf[256];
    int weightmp;
    
    move(b_lines-1, 0);
    clrtoeol();
    show_special_pic(2);
    getdata(b_lines-1,1, "你的選擇是? [Q]離開:", genbuf, 2, 1, 0);    
    if (genbuf[0]=='1'|| genbuf[0]=='2'|| genbuf[0]=='3'|| genbuf[0]=='4')
    { 
      switch(genbuf[0])
      {
        case '1':
          if(d.money<80)
          {
            pressanykey("傳統增胖要80元喔....你不夠錢啦...");
          }
          else
          {
            getdata(b_lines-1,1, "需花費80元(3∼5公斤)，你確定嗎? [y/N]", genbuf, 2, 1, 0);    
            if(genbuf[0]=='Y' || genbuf[0]=='y')
            {
              weightmp=3+rand()%3;
              d.weight+=weightmp;
              d.money-=80;
              d.maxhp-=rand()%2;
              d.hp-=rand()%2+3;
              show_special_pic(3);
              sprintf(inbuf, "總共增加了%d公斤",weightmp);
              pressanykey(inbuf);
            }
            else
            {
              pressanykey("回心轉意囉.....");
            }
          }
          break;
          
        case '2':
          getdata(b_lines-1,1, "增一公斤要30元，你要增多少公斤呢? [請填數字]:", genbuf, 4, 1, 0);    
          weightmp=atoi(genbuf);
          if(weightmp<=0)
          {
            pressanykey("輸入有誤..放棄囉...");          
          }
          else if(d.money>(weightmp*30))
          {
            sprintf(inbuf, "增加%d公斤，總共需花費了%d元，確定嗎? [y/N]",weightmp,weightmp*30);          
            getdata(b_lines-1,1,inbuf, genbuf, 2, 1, 0);    
            if(genbuf[0]=='Y' || genbuf[0]=='y')
            {
                d.money-=weightmp*30;
                d.weight+=weightmp;
                d.maxhp-=(rand()%2+2);
                count_tired(5,8,"N",100,1);
                d.hp-=(rand()%2+3);
                d.sick+=rand()%10+5;
                show_special_pic(3);
                sprintf(inbuf, "總共增加了%d公斤",weightmp);
                pressanykey(inbuf);
            }
            else
            {
              pressanykey("回心轉意囉.....");
            }
          }
          else
          {
            pressanykey("你錢沒那麼多啦.......");            
          }
          break;        
          
        case '3':
          if(d.money<80)
          {
            pressanykey("傳統減肥要80元喔....你不夠錢啦...");
          }
          else
          {
            getdata(b_lines-1,1, "需花費80元(3∼5公斤)，你確定嗎? [y/N]", genbuf, 2, 1, 0);    
            if(genbuf[0]=='Y' || genbuf[0]=='y')
            {
              weightmp=3+rand()%3;
              d.weight-=weightmp;
              if(d.weight<0)
                   d.weight=0;
              d.money-=100;
              d.maxhp+=rand()%2;
              d.hp-=rand()%2+3;
              show_special_pic(4);
              sprintf(inbuf, "總共減少了%d公斤",weightmp);
              pressanykey(inbuf);
            }
            else
            {
              pressanykey("回心轉意囉.....");
            }
          }        
          break;
        case '4':
          getdata(b_lines-1,1, "減一公斤要30元，你要減多少公斤呢? [請填數字]:", genbuf, 4, 1, 0);    
          weightmp=atoi(genbuf);
          if(weightmp<=0)
          {
            pressanykey("輸入有誤..放棄囉...");
          }          
          else if(d.weight<=weightmp)
          {
            pressanykey("你沒那麼重喔.....");
          }
          else if(d.money>(weightmp*30))
          {
            sprintf(inbuf, "減少%d公斤，總共需花費了%d元，確定嗎? [y/N]",weightmp,weightmp*30);          
            getdata(b_lines-1,1,inbuf, genbuf, 2, 1, 0);    
            if(genbuf[0]=='Y' || genbuf[0]=='y')
            {
                d.money-=weightmp*30;
                d.weight-=weightmp;
                d.maxhp-=(rand()%2+2);
                count_tired(5,8,"N",100,1);
                d.hp-=(rand()%2+3);
                d.sick+=rand()%10+5;
                show_special_pic(4);
                sprintf(inbuf, "總共減少了%d公斤",weightmp);
                pressanykey(inbuf);
            }
            else
            {
              pressanykey("回心轉意囉.....");
            }
          }
          else
          {
            pressanykey("你錢沒那麼多啦.......");            
          }
          break;
      }
    }
    return 0;
}


/*參見*/

int
pip_go_palace()
{
  pip_go_palace_screen(royallist);
  return 0;
}

int
pip_go_palace_screen(p) 
struct royalset *p;
{
  int n;
  int a;
  int b;
  int choice;
  int pipkey;
  int change;
  char buf[256];
  char inbuf1[20];
  char inbuf2[20];
  char *needmode[3]={"      ","禮儀表現＞","談吐技巧＞"};
  int save[11]={0,0,0,0,0,0,0,0,0,0,0};

  d.nodone=0; 
  do
  {
  clear();
  show_palace_pic(0);
  move(13,4);
  sprintf(buf,"[1;31m┌──────┤[37;41m 來到總司令部了  請選擇你欲拜訪的對象 [0;1;31m├──────┐[0m");
  prints(buf);
  move(14,4);
  sprintf(buf,"[1;31m│                                                                  │[0m");
  prints(buf);

  for(n=0;n<5;n++)  
  {
    a=2*n+1;
    b=2*n+2;
    move(15+n,4);
    sprintf(inbuf1,"%-10s%3d",needmode[p[a].needmode],p[a].needvalue);
    if(n==4)
    { 
      sprintf(inbuf2,"%-10s",needmode[p[b].needmode]);
    }
    else
    {
      sprintf(inbuf2,"%-10s%3d",needmode[p[b].needmode],p[b].needvalue);
    }
    if((d.seeroyalJ==1 && n==4)||(n!=4))
      sprintf(buf,"[1;31m│ [36m([37m%s[36m) [33m%-10s  [37m%-14s     [36m([37m%s[36m) [33m%-10s  [37m%-14s[31m│[0m",    
             p[a].num,p[a].name,inbuf1,p[b].num,p[b].name,inbuf2);
    else
      sprintf(buf,"[1;31m│ [36m([37m%s[36m) [33m%-10s  [37m%-14s                                   [31m│[0m",    
             p[a].num,p[a].name,inbuf1);             
    prints(buf);
  }
  move(20,4);
  sprintf(buf,"[1;31m│                                                                  │[0m");
  prints(buf);
  move(21,4);
  sprintf(buf,"[1;31m└─────────────────────────────────┘[0m");
  prints(buf);
  

   if(d.death==1 || d.death==2 || d.death==3)
     return 0;    
  /*將各人務已經給與的數值叫回來*/
   save[1]=d.royalA;            /*from守衛*/
   save[2]=d.royalB;            /*from近衛*/
   save[3]=d.royalC;		/*from將軍*/
   save[4]=d.royalD;            /*from大臣*/
   save[5]=d.royalE;            /*from祭司*/
   save[6]=d.royalF;            /*from寵妃*/
   save[7]=d.royalG;            /*from王妃*/
   save[8]=d.royalH;            /*from國王*/
   save[9]=d.royalI;            /*from小丑*/
   save[10]=d.royalJ;           /*from王子*/

   move(b_lines-1, 0);
   clrtoeol();
   move(b_lines-1,0);
   prints("[1;33m [生命力] %d/%d  [疲勞度] %d [0m",d.hp,d.maxhp,d.tired);
             
   move(b_lines, 0);
   clrtoeol();
   move(b_lines,0);
   prints(
   "[1;37;46m  參見選單  [44m [字母]選擇欲拜訪的人物  [Q]離開星空總司令部：                    [0m");
   pipkey=egetch();
   choice=pipkey-64;
   if(choice<1 || choice>10)
      choice=pipkey-96;

   if((choice>=1 && choice<=10 && d.seeroyalJ==1)||(choice>=1 && choice<=9 && d.seeroyalJ==0))
   {
    d.social+=rand()%3+3;
    d.hp-=rand()%5+6;
    d.tired+=rand()%5+8;
    if(d.tired>=100)
    {
       d.death=1;
       pipdie("[1;31m累死了...[m  ",1);
    }
    if(d.hp<0)
    {
       d.death=1;
       pipdie("[1;31m餓死了...[m  ",1);
    }
    if(d.death==1)
    {
      sprintf(buf,"掰掰了...真是悲情..");
    }
    else
    {
    if((p[choice].needmode==0)||
       (p[choice].needmode==1 && d.manners>=p[choice].needvalue)||
       (p[choice].needmode==2 && d.speech>=p[choice].needvalue))    
    {
      if(choice>=1 && choice<=9 && save[choice] >= p[choice].maxtoman)
      {
        if(rand()%2>0)
            sprintf(buf,"能和這麼偉大的你講話真是榮幸ㄚ...");
        else
            sprintf(buf,"很高興你來拜訪我，但我不能給你什麼了..");
      }
      else
      {
        change=0;
        if(choice>=1 && choice <=8 )
        {
          switch(choice)
          {
            case 1:
              change=d.character/5;
              break;
            case 2:
              change=d.character/8;
              break;
            case 3:
              change=d.charm/5;
              break;              
            case 4:
              change=d.wisdom/10;
              break;
            case 5:
              change=d.belief/10;
              break;
            case 6:
              change=d.speech/10;
              break;
            case 7:
              change=d.social/10;
              break;
            case 8:
              change=d.hexp/10;
              break;            
          }
          /*如果大於每次的增加最大量*/
          if(change > p[choice].addtoman)
             change=p[choice].addtoman;
          /*如果加上原先的之後大於所能給的所有值時*/
          if((change+save[choice])>= p[choice].maxtoman)
             change=p[choice].maxtoman-save[choice];
          save[choice]+=change;
          d.toman+=change;
        }
        else if(choice==9)
        {
          save[9]=0;
          d.social-=13+rand()%4;
          d.affect+=13+rand()%4;
        }
        else if(choice==10 && d.seeroyalJ==1)
        {
            save[10]+=15+rand()%4;
            d.seeroyalJ=0;
        }
        if(rand()%2>0)
            sprintf(buf,"%s",p[choice].words1);
        else    
            sprintf(buf,"%s",p[choice].words2);
      }
    }
    else
    {
      if(rand()%2>0)
            sprintf(buf,"我不和妳這樣的雞談話....");
      else
            sprintf(buf,"你這隻沒教養的雞，再去學學禮儀吧....");    
    
    }
    }    
    pressanykey(buf);
   }
   d.royalA=save[1];
   d.royalB=save[2];
   d.royalC=save[3];
   d.royalD=save[4];
   d.royalE=save[5];
   d.royalF=save[6];
   d.royalG=save[7];
   d.royalH=save[8];
   d.royalI=save[9];
   d.royalJ=save[10];
  }while((pipkey!='Q')&&(pipkey!='q')&&(pipkey!=KEY_LEFT));

  pressanykey("離開星空總司令部.....");  
  return 0;
}

int pip_query()  /*拜訪小雞*/
{
  int id;
  char genbuf[STRLEN];

  stand_title("拜訪同伴");
  usercomplete(msg_uid, genbuf);
  if (genbuf[0])
  {
    move(2, 0);
    if (id = getuser(genbuf))
    {
        pip_read(genbuf);
        pressanykey("觀摩一下別人的小雞...:p");
    }
    else
    {
      outs(err_uid);
      clrtoeol();
    }
  }
  return 0;
}

int
pip_read(genbuf)
{
  FILE *fs;
  char buf[200];
  /*char yo[14][5]={"誕生","嬰兒","幼兒","兒童","青年","少年","成年",
                  "壯年","壯年","壯年","更年","老年","老年","古稀"};*/
  char yo[12][5]={"誕生","嬰兒","幼兒","兒童","少年","青年",
                  "成年","壯年","更年","老年","古稀","神仙"};                  
  int pc1,age1,age=0;
  
  int year1,month1,day1,sex1,death1,nodone1,relation1,liveagain1,dataB1,dataC1,dataD1,dataE1;
  int hp1,maxhp1,weight1,tired1,sick1,shit1,wrist1,bodyA1,bodyB1,bodyC1,bodyD1,bodyE1;
  int social1,family1,hexp1,mexp1,tmpA1,tmpB1,tmpC1,tmpD1,tmpE1;
  int mp1,maxmp1,attack1,resist1,speed1,hskill1,mskill1,mresist1,magicmode1,fightB1,fightC1,fightD1,fightE1;
  int weaponhead1,weaponrhand1,weaponlhand1,weaponbody1,weaponfoot1,weaponA1,weaponB1,weaponC1,weaponD1,weaponE1;
  int toman1,character1,love1,wisdom1,art1,etchics1,brave1,homework1,charm1,manners1,speech1,cookskill1,learnA1,learnB1,learnC1,learnD1,learnE1;
  int happy1,satisfy1,fallinlove1,belief1,offense1,affect1,stateA1,stateB1,stateC1,stateD1,stateE1;
  int food1,medicine1,bighp1,cookie1,ginseng1,snowgrass1,eatC1,eatD1,eatE1;
  int book1,playtool1,money1,thingA1,thingB1,thingC1,thingD1,thingE1;
  int winn1,losee1;
  int royalA1,royalB1,royalC1,royalD1,royalE1,royalF1,royalG1,royalH1,royalI1,royalJ1,seeroyalJ1,seeA1,seeB1,seeC1,seeD1,seeE1;
  int wantend1,lover1;
  char name1[200];
  int classA1,classB1,classC1,classD1,classE1;
  int classF1,classG1,classH1,classI1,classJ1;
  int classK1,classL1,classM1,classN1,classO1;
  int workA1,workB1,workC1,workD1,workE1;
  int workF1,workG1,workH1,workI1,workJ1;
  int workK1,workL1,workM1,workN1,workO1;
  int workP1,workQ1,workR1,workS1,workT1;
  int workU1,workV1,workW1,workX1,workY1,workZ1;

  sprintf(buf,"home/%s/chicken",genbuf);
  currutmp->destuid = genbuf;

  if(fs=fopen(buf,"r"))
  {
    fgets(buf, 80, fs);
    age = ((time_t) atol(buf))/60/30;
  
    if(age==0) /*誕生*/
       age1=0;
    else if( age==1) /*嬰兒*/
       age1=1;
    else if( age>=2 && age<=5 ) /*幼兒*/
       age1=2;
    else if( age>=6 && age<=12 ) /*兒童*/
       age1=3;
    else if( age>=13 && age<=15 ) /*少年*/
       age1=4;     
    else if( age>=16 && age<=18 ) /*青年*/
       age1=5;     
    else if( age>=19 && age<=35 ) /*成年*/
       age1=6;
    else if( age>=36 && age<=45 ) /*壯年*/
       age1=7;
    else if( age>=45 && age<=60 ) /*更年*/
       age1=8;
    else if( age>=60 && age<=70 ) /*老年*/
       age1=9;
    else if( age>=70 && age<=100 ) /*古稀*/
       age1=10;
    else if( age>100 ) /*神仙*/
       age1=11;

    fscanf(fs,
    "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %s %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
    &(year1),&(month1),&(day1),&(sex1),&(death1),&(nodone1),&(relation1),&(liveagain1),&(dataB1),&(dataC1),&(dataD1),&(dataE1),
    &(hp1),&(maxhp1),&(weight1),&(tired1),&(sick1),&(shit1),&(wrist1),&(bodyA1),&(bodyB1),&(bodyC1),&(bodyD1),&(bodyE1),
    &(social1),&(family1),&(hexp1),&(mexp1),&(tmpA1),&(tmpB1),&(tmpC1),&(tmpD1),&(tmpE1),
    &(mp1),&(maxmp1),&(attack1),&(resist1),&(speed1),&(hskill1),&(mskill1),&(mresist1),&(magicmode1),&(fightB1),&(fightC1),&(fightD1),&(fightE1),
    &(weaponhead1),&(weaponrhand1),&(weaponlhand1),&(weaponbody1),&(weaponfoot1),&(weaponA1),&(weaponB1),&(weaponC1),&(weaponD1),&(weaponE1),
    &(toman1),&(character1),&(love1),&(wisdom1),&(art1),&(etchics1),&(brave1),&(homework1),&(charm1),&(manners1),&(speech1),&(cookskill1),&(learnA1),&(learnB1),&(learnC1),&(learnD1),&(learnE1),
    &(happy1),&(satisfy1),&(fallinlove1),&(belief1),&(offense1),&(affect1),&(stateA1),&(stateB1),&(stateC1),&(stateD1),&(stateE1),
    &(food1),&(medicine1),&(bighp1),&(cookie1),&(ginseng1),&(snowgrass1),&(eatC1),&(eatD1),&(eatE1),
    &(book1),&(playtool1),&(money1),&(thingA1),&(thingB1),&(thingC1),&(thingD1),&(thingE1),
    &(winn1),&(losee1),
    &(royalA1),&(royalB1),&(royalC1),&(royalD1),&(royalE1),&(royalF1),&(royalG1),&(royalH1),&(royalI1),&(royalJ1),&(seeroyalJ1),&(seeA1),&(seeB1),&(seeC1),&(seeD1),&(seeE1),
    &(wantend1),&(lover1),
    name1,
    &(classA1),&(classB1),&(classC1),&(classD1),&(classE1),
    &(classF1),&(classG1),&(classH1),&(classI1),&(classJ1),
    &(classK1),&(classL1),&(classM1),&(classN1),&(classO1),
    &(workA1),&(workB1),&(workC1),&(workD1),&(workE1),
    &(workF1),&(workG1),&(workH1),&(workI1),&(workJ1),
    &(workK1),&(workL1),&(workM1),&(workN1),&(workO1),
    &(workP1),&(workQ1),&(workR1),&(workS1),&(workT1),
    &(workU1),&(workV1),&(workW1),&(workX1),&(workY1),&(workZ1)
  );
  fclose(fs);

  move(1,0);
  clrtobot();
  prints("這是%s養的小雞：\n",xuser.userid);

  if (death1==0)
  {
   prints("[1;32mName：%-10s[m  生日：%2d年%2d月%2d日   年齡：%2d歲  狀態：%s  錢錢：%d\n"
          "生命：%3d/%-3d  快樂：%-4d  滿意：%-4d  氣質：%-4d  智慧：%-4d  體重：%-4d\n"
          "大補丸：%-4d   食物：%-4d  零食：%-4d  疲勞：%-4d  髒髒：%-4d  病氣：%-4d\n",
        name1,year1-11,month1,day1,age,yo[age1],money1,
        hp1,maxhp1,happy1,satisfy1,character1,wisdom1,weight1,
        bighp1,food1,cookie1,tired1,shit1,sick1);

    move(5,0);
    switch(age1)
    {
     case 0:       
     case 1:
     case 2:
       if(weight1<=(60+10*age-30))
          show_basic_pic(1);
       else if(weight1>(60+10*age-30) && weight1<(60+10*age+30))
          show_basic_pic(2);
       else if(weight1>=(60+10*age+30))
          show_basic_pic(3);
       break;
     case 3:
     case 4:
       if(weight1<=(60+10*age-30))
          show_basic_pic(4);
       else if(weight1>(60+10*age-30) && weight1<(60+10*age+30))
          show_basic_pic(5);
       else if(weight1>=(60+10*age+30))
          show_basic_pic(6);
       break;
     case 5:
     case 6:
       if(weight1<=(60+10*age-30))
          show_basic_pic(7);
       else if(weight1>(60+10*age-30) && weight1<(60+10*age+30))
          show_basic_pic(8);
       else if(weight1>=(60+10*age+30))
          show_basic_pic(9);
       break;     
     case 7:
     case 8:
       if(weight1<=(60+10*age-30))
          show_basic_pic(10);
       else if(weight1>(60+10*age-30) && weight1<(60+10*age+30))
          show_basic_pic(11);
       else if(weight1>=(60+10*age+30))
          show_basic_pic(12);
       break;     
     case 9:
       show_basic_pic(13);
       break;
     case 10:
     case 11:
       show_basic_pic(13);
       break;
    }
   move(18,0);
   if(shit1==0) prints("很乾淨..");
   if(shit1>40&&shit1<60) prints("臭臭的..");
   if(shit1>=60&&shit1<80) prints("好臭喔..");
   if(shit1>=80&&shit1<100) prints("[1;34m快臭死了..[m");
   if(shit1>=100) {prints("[1;31m臭死了..[m"); return -1;}

   pc1=hp1*100/maxhp1;
   if(pc1==0) {prints("餓死了.."); return -1;}
   if(pc1<20) prints("[1;35m全身無力中.快餓死了.[m");
   if(pc1<40&&pc1>=20) prints("體力不太夠..想吃點東西..");
   if(pc1<100&&pc1>=80) prints("嗯∼肚子飽飽有體力..");
   if(pc1>=100) prints("[1;34m快撐死了..[m");

   pc1=tired1;
   if(pc1<20) prints("精神抖抖中..");
   if(pc1<80&&pc1>=60) prints("[1;34m有點小累..[m");
   if(pc1<100&&pc1>=80) {prints("[1;31m好累喔，快不行了..[m"); }
   if(pc1>=100) {prints("累死了..."); return -1;}

   pc1=60+10*age;
   if(weight1<(pc1+30) && weight1>=(pc1+10)) prints("有點小胖..");
   if(weight1<(pc1+50) && weight1>=(pc1+30)) prints("太胖了..");
   if(weight1>(pc1+50)) {prints("胖死了..."); return -1;}

   if(weight1<(pc1-50)) {prints("瘦死了.."); return -1;}
   if(weight1>(pc1-30) && weight1<=(pc1-10)) prints("有點小瘦..");
   if(weight1>(pc1-50) && weight1<=(pc1-30)) prints("太瘦了..");

   if(sick1<75&&sick1>=50) prints("[1;34m生病了..[m");
   if(sick1<100&&sick1>=75) {prints("[1;31m病重!!..[m"); }
   if(sick1>=100) {prints("病死了.!."); return -1;}

   pc1=happy1;
   if(pc1<20) prints("[1;31m很不快樂..[m");
   if(pc1<40&&pc1>=20) prints("不快樂..");
   if(pc1<95&&pc1>=80) prints("快樂..");
   if(pc1<=100&&pc1>=95) prints("很快樂..");

   pc1=satisfy1;
   if(pc1<40) prints("[31;1m不滿足..[m");
   if(pc1<95&&pc1>=80) prints("滿足..");
   if(pc1<=100&&pc1>=95) prints("很滿足..");
  }
  else if(death1==1)
  {
     show_die_pic(2);
     move(14,20);
     prints("可憐的小雞嗚呼哀哉了");
  } 
  else if(death1==2)
  {
     show_die_pic(3);
  }
  else if(death1==3)
  {
    move(5,0);
    outs("遊戲已經玩到結局囉....");
  }
  else
  {
    pressanykey("檔案損毀了....");
  }
 }   /* 有養小雞 */
 else
 {
   move(1,0);
   clrtobot();
   pressanykey("這一家的人沒有養小雞......");
 }
}

/*---------------------------------------------------------------------------*/
/* 系統選單:個人資料  小雞放生  特別服務                                     */
/*                                                                           */
/*---------------------------------------------------------------------------*/

char weaponhead[7][10]={
"沒有裝備",
"塑膠帽子", 
"牛皮小帽",
"㊣安全帽",
"鋼鐵頭盔",
"魔法髮箍",
"黃金聖盔"};


char weaponrhand[10][10]={
"沒有裝備",
"大木棒",  
"金屬扳手",
"青銅劍",  
"晴雷劍", 
"蟬翼刀", 
"忘情劍", 
"獅頭寶刀",
"屠龍刀",  
"黃金聖杖"
};  

char weaponlhand[8][10]={
"沒有裝備",
"大木棒", 
"金屬扳手",
"木盾",
"不鏽鋼盾",
"白金之盾",
"魔法盾",
"黃金聖盾"
};


char weaponbody[7][10]={
"沒有裝備",
"塑膠冑甲",
"特級皮甲",
"鋼鐵盔甲",
"魔法披風",
"白金盔甲",
"黃金聖衣"};

char weaponfoot[8][12]={
"沒有裝備",
"塑膠拖鞋",
"東洋木屐",
"特級雨鞋",
"NIKE運動鞋",
"鱷魚皮靴",
"飛天魔靴",
"黃金聖靴"
};

int 
pip_system_freepip()
{
      char buf[256];
      move(b_lines-1, 0);
      clrtoeol();
      getdata(b_lines-1,1, "真的要放生嗎？(y/N)", buf, 2, 1, 0);
      if (buf[0]!='y'&&buf[0]!='Y') return 0;
      sprintf(buf,"%s 被狠心的 %s 丟掉了~",d.name,cuser.userid);
      pressanykey(buf);
      d.death=2;
      pipdie("[1;31m被狠心丟棄:~~[0m",2);
      return 0;
}


int
pip_system_service()
{
     int pipkey;
     int oldchoice;
     char buf[200];
     char oldname[21];
     time_t now;     

     move(b_lines, 0);
     clrtoeol();
     move(b_lines,0);
     prints("[1;44m  服務項目  [46m[1]命名大師 [2]變性手術 [3]結局設局                                [0m");
     pipkey=egetch();
     
     switch(pipkey)
     {
     case '1':
       move(b_lines-1,0);
       clrtobot();
       getdata(b_lines-1, 1, "幫小雞重新取個好名字：", buf, 11, DOECHO,NULL);
       if(!buf[0])
       {
         pressanykey("等一下想好再來好了  :)");
         break;
       }
       else
       {
        strcpy(oldname,d.name);
        strcpy(d.name,buf);
        /*改名記錄*/
        now=time(0);
        sprintf(buf, "[1;37m%s %-11s把小雞 [%s] 改名成 [%s] [0m\n", Cdate(&now), cuser.userid,oldname,d.name);
        pip_log_record(buf);
        pressanykey("嗯嗯  換一個新的名字喔...");
       }
       break;
       
     case '2':  /*變性*/
       move(b_lines-1,0);
       clrtobot();
       /*1:公 2:母 */
       if(d.sex==1)
       { 
         oldchoice=2; /*公-->母*/
         move(b_lines-1, 0);
         prints("[1;33m將小雞由[32m♂[33m變性成[35m♀[33m的嗎？ [37m[y/N][0m");
       }
       else
       { 
         oldchoice=1; /*母-->公*/
         move(b_lines-1, 0); 
         prints("[1;33m將小雞由[35m♀[33m變性成[35m♂[33m的嗎？ [37m[y/N][0m");
       }
       move(b_lines,0);
       prints("[1;44m  服務項目  [46m[1]命名大師 [2]變性手術 [3]結局設局                                [0m");
       pipkey=egetch();
       if(pipkey=='Y' || pipkey=='y')
       {
         /*改名記錄*/
         now=time(0);
         if(d.sex==1)
           sprintf(buf,"[1;37m%s %-11s把小雞 [%s] 由♂變性成♀了[0m\n",Cdate(&now), cuser.userid,d.name);
         else
           sprintf(buf,"[1;37m%s %-11s把小雞 [%s] 由♀變性成♂了[0m\n",Cdate(&now), cuser.userid,d.name);           
         pip_log_record(buf);
         pressanykey("變性手術完畢...");       
         d.sex=oldchoice;
       }  
       break;
       
     case '3':
       move(b_lines-1,0);
       clrtobot();
       /*1:不要且未婚 4:要且未婚 */
       oldchoice=d.wantend;
       if(d.wantend==1 || d.wantend==2 || d.wantend==3)
       { 
         oldchoice+=3; /*沒有-->有*/
         move(b_lines-1, 0); 
         prints("[1;33m將小雞遊戲改成[32m[有20歲結局][33m? [37m[y/N][0m");
	 sprintf(buf,"小雞遊戲設定成[有20歲結局]..");         
       }
       else
       { 
         oldchoice-=3; /*有-->沒有*/
         move(b_lines-1, 0); 
         prints("[1;33m將小雞遊戲改成[32m[沒有20歲結局][33m? [37m[y/N][0m");
         sprintf(buf,"小雞遊戲設定成[沒有20歲結局]..");
       }
       move(b_lines,0);
       prints("[1;44m  服務項目  [46m[1]命名大師 [2]變性手術 [3]結局設局                                [0m");
       pipkey=egetch();
       if(pipkey=='Y' || pipkey=='y')
       {
         d.wantend=oldchoice;
         pressanykey(buf);
       }  
       break;     
     } 
     return 0;
}

#include<stdarg.h>
void 
pip_data_list_va(va_list pvar)
{
  char *userid;
  
  userid = va_arg(pvar, char *);
  pip_data_list(userid);
}


int
pip_data_list()  /*看小雞個人詳細資料*/
//  char *userid;
{
  char buf[256];
  char inbuf1[20];
  char inbuf2[20];
  int tm;
  int pipkey;
  int page=1;
  struct chicken chicken;
  FILE *fs;

//  if (!isprint(userid[0]))
    sprintf(buf,BBSHOME"/home/%s/chicken", cuser.userid);
//  else
//    sprintf(buf,BBSHOME"/home/%s/chicken", userid);

  if (fs = fopen(buf, "r"))
  {
    fgets(buf, 80, fs);
    chicken.bbtime = (time_t) atol(buf);

    fscanf(fs,
  "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %s %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
  &(chicken.year),&(chicken.month),&(chicken.day),&(chicken.sex),&(chicken.death),&(chicken.nodone),&(chicken.relation),&(chicken.liveagain),&(chicken.dataB),&(chicken.dataC),&(chicken.dataD),&(chicken.dataE),
  &(chicken.hp),&(chicken.maxhp),&(chicken.weight),&(chicken.tired),&(chicken.sick),&(chicken.shit),&(chicken.wrist),&(chicken.bodyA),&(chicken.bodyB),&(chicken.bodyC),&(chicken.bodyD),&(chicken.bodyE),
  &(chicken.social),&(chicken.family),&(chicken.hexp),&(chicken.mexp),&(chicken.tmpA),&(chicken.tmpB),&(chicken.tmpC),&(chicken.tmpD),&(chicken.tmpE),
  &(chicken.mp),&(chicken.maxmp),&(chicken.attack),&(chicken.resist),&(chicken.speed),&(chicken.hskill),&(chicken.mskill),&(chicken.mresist),&(chicken.magicmode),&(chicken.fightB),&(chicken.fightC),&(chicken.fightD),&(chicken.fightE),
  &(chicken.weaponhead),&(chicken.weaponrhand),&(chicken.weaponlhand),&(chicken.weaponbody),&(chicken.weaponfoot),&(chicken.weaponA),&(chicken.weaponB),&(chicken.weaponC),&(chicken.weaponD),&(chicken.weaponE),
  &(chicken.toman),&(chicken.character),&(chicken.love),&(chicken.wisdom),&(chicken.art),&(chicken.etchics),&(chicken.brave),&(chicken.homework),&(chicken.charm),&(chicken.manners),&(chicken.speech),&(chicken.cookskill),&(chicken.learnA),&(chicken.learnB),&(chicken.learnC),&(chicken.learnD),&(chicken.learnE),
  &(chicken.happy),&(chicken.satisfy),&(chicken.fallinlove),&(chicken.belief),&(chicken.offense),&(chicken.affect),&(chicken.stateA),&(chicken.stateB),&(chicken.stateC),&(chicken.stateD),&(chicken.stateE),
  &(chicken.food),&(chicken.medicine),&(chicken.bighp),&(chicken.cookie),&(chicken.ginseng),&(chicken.snowgrass),&(chicken.eatC),&(chicken.eatD),&(chicken.eatE),
  &(chicken.book),&(chicken.playtool),&(chicken.money),&(chicken.thingA),&(chicken.thingB),&(chicken.thingC),&(chicken.thingD),&(chicken.thingE),
  &(chicken.winn),&(chicken.losee),
  &(chicken.royalA),&(chicken.royalB),&(chicken.royalC),&(chicken.royalD),&(chicken.royalE),&(chicken.royalF),&(chicken.royalG),&(chicken.royalH),&(chicken.royalI),&(chicken.royalJ),&(chicken.seeroyalJ),&(chicken.seeA),&(chicken.seeB),&(chicken.seeC),&(chicken.seeD),&(chicken.seeE),
  &(chicken.wantend),&(chicken.lover),chicken.name,
  &(chicken.classA),&(chicken.classB),&(chicken.classC),&(chicken.classD),&(chicken.classE),
  &(chicken.classF),&(chicken.classG),&(chicken.classH),&(chicken.classI),&(chicken.classJ),
  &(chicken.classK),&(chicken.classL),&(chicken.classM),&(chicken.classN),&(chicken.classO),
  &(chicken.workA),&(chicken.workB),&(chicken.workC),&(chicken.workD),&(chicken.workE),
  &(chicken.workF),&(chicken.workG),&(chicken.workH),&(chicken.workI),&(chicken.workJ),
  &(chicken.workK),&(chicken.workL),&(chicken.workM),&(chicken.workN),&(chicken.workO),
  &(chicken.workP),&(chicken.workQ),&(chicken.workR),&(chicken.workS),&(chicken.workT),
  &(chicken.workU),&(chicken.workV),&(chicken.workW),&(chicken.workX),&(chicken.workY),&(chicken.workZ)
  );  

    fclose(fs);
  }
  else
  {
    pressanykey("我沒有養小雞啦 !");
    return 0;
  }


//  tm=(time(0)-start_time+chicken.bbtime)/60/30;
  tm=chicken.bbtime/60/30;
  
  clear();  
  move(1,0);
  prints("       [1;33m?歈??????????????????歈??????裐?????????[m\n");
  prints("       [0;37m?矙???  ?矙? ??   ?矙裺裐歈??潁? ??   ??[m\n");
  prints("       [1;37m?矙???  ?矙矙歈?  ??  ?矙?    ?矙歈?  ??[m\n");
  prints("       [1;34m?裺????????裺潁裺???  ?裺?    ?裺潁裺???[32m......................[m");
  do
  { clrchyiuan(5,23);
    switch(page)
    {
     case 1:
       move(5,0);
       sprintf(buf,
       "[1;31m ╭┤[41;37m 基本資料 [0;1;31m├─────────────────────────────╮[m\n");  
       prints(buf);
  
       sprintf(buf,
       "[1;31m │[33m﹟姓    名 :[37m %-10s [33m﹟生    日 :[37m %02d/%02d/%02d   [33m﹟年    紀 :[37m %-2d         [31m│[m\n",
       chicken.name,chicken.year,chicken.month,chicken.day,tm);
       prints(buf);  
  
       sprintf(inbuf1,"%d/%d",chicken.hp,chicken.maxhp);  
       sprintf(inbuf2,"%d/%d",chicken.mp,chicken.maxmp);  
       sprintf(buf,
       "[1;31m │[33m﹟體    重 :[37m %-5d(米克)[33m﹟體    力 :[37m %-11s[33m﹟法    力 :[37m %-11s[31m│[m\n",
       chicken.weight,inbuf1,inbuf2);
       prints(buf);  
  
       sprintf(buf,
       "[1;31m │[33m﹟疲    勞 :[37m %-3d        [33m﹟病    氣 :[37m %-3d        [33m﹟髒    髒 :[37m %-3d        [31m│[m\n",
       chicken.tired,chicken.sick,chicken.shit);
       prints(buf);  
   
       sprintf(buf,  
       "[1;31m │[33m﹟腕    力 :[37m %-7d    [33m﹟親子關係 :[37m %-7d    [33m﹟金    錢 :[37m %-11d[31m│[m\n",
       chicken.wrist,chicken.relation,chicken.money);
       prints(buf);  
  
       sprintf(buf,  
       "[1;31m ├┤[41;37m 能力資料 [0;1;31m├─────────────────────────────┤[m\n");
       prints(buf);  
   
       sprintf(buf,   
       "[1;31m │[33m﹟氣    質 :[37m %-10d [33m﹟智    力 :[37m %-10d [33m﹟愛    心 :[37m %-10d [31m│[m\n",
       chicken.character,chicken.wisdom,chicken.love);
       prints(buf);  
   
       sprintf(buf, 
       "[1;31m │[33m﹟藝    術 :[37m %-10d [33m﹟道    德 :[37m %-10d [33m﹟家    事 :[37m %-10d [31m│[m\n",
       chicken.art,chicken.etchics,chicken.homework);
       prints(buf);  
 
       sprintf(buf, 
       "[1;31m │[33m﹟禮    儀 :[37m %-10d [33m﹟應    對 :[37m %-10d [33m﹟烹    飪 :[37m %-10d [31m│[m\n",
       chicken.manners,chicken.speech,chicken.cookskill);
       prints(buf);    
 
       sprintf(buf,  
       "[1;31m ├┤[41;37m 狀態資料 [0;1;31m├─────────────────────────────┤[m\n");
       prints(buf);  
 
       sprintf(buf, 
       "[1;31m │[33m﹟快    樂 :[37m %-10d [33m﹟滿    意 :[37m %-10d [33m﹟人    際 :[37m %-10d [31m│[m\n",
       chicken.happy,chicken.satisfy,chicken.toman);
       prints(buf);
  
       sprintf(buf, 
       "[1;31m │[33m﹟魅    力 :[37m %-10d [33m﹟勇    敢 :[37m %-10d [33m﹟信    仰 :[37m %-10d [31m│[m\n",
       chicken.charm,chicken.brave,chicken.belief);
       prints(buf);  

       sprintf(buf, 
       "[1;31m │[33m﹟罪    孽 :[37m %-10d [33m﹟感    受 :[37m %-10d [33m            [37m            [31m│[m\n",
       chicken.offense,chicken.affect);
       prints(buf);  

       sprintf(buf, 
       "[1;31m ├┤[41;37m 評價資料 [0;1;31m├─────────────────────────────┤[m\n");
       prints(buf);  

       sprintf(buf, 
       "[1;31m │[33m﹟社交評價 :[37m %-10d [33m﹟戰鬥評價 :[37m %-10d [33m﹟魔法評價 :[37m %-10d [31m│[m\n",
       chicken.social,chicken.hexp,chicken.mexp);
       prints(buf);  

       sprintf(buf, 
       "[1;31m │[33m﹟家事評價 :[37m %-10d [33m            [37m            [33m            [37m            [31m│[m\n",
       chicken.family);
       prints(buf);  
  
       sprintf(buf, 
       "[1;31m ╰────────────────────────────────────╯[m\n");
       prints(buf);  
       
       move(b_lines-1,0);       
       sprintf(buf, 
       "                                                              [1;36m第一頁[37m/[36m共二頁[m\n");
       prints(buf);  
       break;

     case 2:
       move(5,0);
       sprintf(buf, 
       "[1;31m ╭┤[41;37m 物品資料 [0;1;31m├─────────────────────────────╮[m\n");
       prints(buf);  
  
       sprintf(buf, 
       "[1;31m │[33m﹟食    物 :[37m %-10d [33m﹟零    食 :[37m %-10d [33m﹟大 補 丸 :[37m %-10d [31m│[m\n",
       chicken.food,chicken.cookie,chicken.bighp);
       prints(buf);  
  
       sprintf(buf, 
       "[1;31m │[33m﹟藥    草 :[37m %-10d [33m﹟書    本 :[37m %-10d [33m﹟玩    具 :[37m %-10d [31m│[m\n",
       chicken.medicine,chicken.book,chicken.playtool);
       prints(buf);  
  
       sprintf(buf, 
       "[1;31m ├┤[41;37m 遊戲資料 [0;1;31m├─────────────────────────────┤[m\n");
       prints(buf);  
  
       sprintf(buf, 
       "[1;31m │[33m﹟猜 拳 贏 :[37m %-10d [33m﹟猜 拳 輸 :[37m %-10d                         [31m│[m\n",
       chicken.winn,chicken.losee);
       prints(buf);  
  
       sprintf(buf, 
       "[1;31m ├┤[41;37m 武力資料 [0;1;31m├─────────────────────────────┤[m\n");
       prints(buf);  
  
       sprintf(buf, 
       "[1;31m │[33m﹟攻 擊 力 :[37m %-10d [33m﹟防 禦 力 :[37m %-10d [33m﹟速 度 值 :[37m %-10d [31m│[m\n",
       chicken.attack,chicken.resist,chicken.speed);
       prints(buf);  
       sprintf(buf, 
       "[1;31m │[33m﹟抗魔能力 :[37m %-10d [33m﹟戰鬥技術 :[37m %-10d [33m﹟魔法技術 :[37m %-10d [31m│[m\n",
       chicken.mresist,chicken.hskill,chicken.mskill);
       prints(buf);  
  
       sprintf(buf, 
       "[1;31m │[33m﹟頭部裝備 :[37m %-10s [33m﹟右手裝備 :[37m %-10s [33m﹟左手裝備 :[37m %-10s [31m│[m\n",
       weaponhead[chicken.weaponhead],weaponrhand[chicken.weaponrhand],weaponlhand[chicken.weaponlhand]);
       prints(buf);  
  
       sprintf(buf, 
       "[1;31m │[33m﹟身體裝備 :[37m %-10s [33m﹟腳部裝備 :[37m %-10s [33m            [37m            [31m│[m\n",
       weaponbody[chicken.weaponbody],weaponfoot[chicken.weaponfoot]);
       prints(buf);  
  
       sprintf(buf, 
       "[1;31m ╰────────────────────────────────────╯[m\n");
       prints(buf); 

       move(b_lines-1,0);
       sprintf(buf, 
       "                                                              [1;36m第二頁[37m/[36m共二頁[m\n");
       prints(buf);          
       break;
    }
    move(b_lines,0);
    sprintf(buf,"[1;44;37m  資料選單  [46m  [↑/PAGE UP]往上一頁 [↓/PAGE DOWN]往下一頁 [Q]離開:            [m");
    prints(buf);    
    pipkey=egetch();
    switch(pipkey)
    {
      case KEY_UP:
      case KEY_PGUP:
      case KEY_DOWN:
      case KEY_PGDN:
        if(page==1)
           page=2;
        else if(page==2)
           page=1;
        break;
    }
  }while((pipkey!='Q')&&(pipkey!='q')&&(pipkey!=KEY_LEFT));
  return 0;
}

/*---------------------------------------------------------------------------*/
/* 戰鬥特區                                                                  */
/*                                                                           */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* 戰鬥人物決定函式                                                          */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/*0~1:2
  2~4:3
  5~9:5
  10~12:3
  13~14:1
  15~19:5
  20~23:4
  24~27:4
  28~33:6
  34~45:12
*/
int
pip_meet_vs_man()
{
  int class;
  int man;
  int lucky;
  class=(d.maxhp*30+d.maxmp*20+d.attack*20+d.resist*15+d.mexp*5+d.hexp*5+d.speed*10)/8500+1;

  if(class==1)
  {
    man=rand()%2;
  }
  else if(class==2)
  { 
    if(rand()%4>0)
    {
      man=rand()%3+2;
    }
    else
    {
      man=rand()%2;
    }
  }
  else if(class==3)
  {
    lucky=rand()%9;
    if(lucky>3)
    {
      man=rand()%5+5;
    }
    else if(lucky<=3 && lucky>0)
    {
      man=rand()%3+2;
    }
    else
    {
      man=rand()%2;
    }
  }  
  else if(class==4)
  {
    lucky=rand()%15;
    if(lucky>5)
    {
      man=rand()%3+10;
    }
    else if(lucky<=5 && lucky>3)
    {
      man=rand()%5+5;
    }
    else 
    {
      man=rand()%5;
    }
  }
  else if(class==5)
  {
    lucky=rand()%20;
    if(lucky>6)
    {
      man=13+rand()%2;
    }
    else if(lucky<=6 && lucky>3)
    {
      man=rand()%3+10;
    }
    else
    {
      man=rand()%10;
    }
  }
  else if(class==6)
  {
    lucky=rand()%25;
    if(lucky>8)
    {
      man=15+rand()%6;
    }
    else if(lucky<=8 &&lucky>4)
    {
      man=13+rand()%2;
    }
    else
    {
      man=rand()%13;
    }
  }  
  else if(class==7)
  {
    lucky=rand()%40;
    if(lucky>12)
    {
      man=21+rand()%3;    
    }
    else if(lucky<=12 &&lucky>4)
    {
      man=15+rand()%6;
    }
    else
    {
      man=rand()%15;
    }
  }  
  else if(class==8)
  {
    lucky=rand()%50;
    if(lucky>25)
    {
      man=24+rand()%4;
    }
    else if(lucky<=25 && lucky>20)
    {
      man=21+rand()%3;    
    }
    else 
    {
      man=rand()%21;
    }
  }        
  else if(class==9)
  {
    lucky=rand()%75;
    if(lucky>20)
    {
      man=28+rand()%6;
    }
    else if(lucky<=20 && lucky>10)    
    {
      man=24+rand()%4;
    }
    else if(lucky<=10 && lucky>5)
    {
      man=21+rand()%3;    
    }
    else
    {
      man=rand()%21;
    }
  }          
  else if(class>=9)
  {
    lucky=rand()%100;
    if(lucky>20)
    {
      man=34+rand()%12;
    }
    else if(lucky<=20 && lucky>10)
    {
      man=28+rand()%6;
    }
    else if(lucky<=10 && lucky>5)    
    {
      man=24+rand()%4;
    }
    else
    {
      man=rand()%24;    
    }
  }            
  pip_fight_bad(man);
  return;
}

int 
pip_fight_bad(n)
int n;
{
  pip_fight_main(n,badmanlist,1);
  return;
}


int
pip_fight_main(n,list,mode)
int n;
struct playrule list[];   
int mode;
{
  pip_vs_man(n,list,mode);
  return 0;
}

/*---------------------------------------------------------------------------*/
/* 戰鬥戰鬥函式                                                              */
/*                                                                           */
/*---------------------------------------------------------------------------*/
int 
pip_vs_man(n,p,mode)
int n;
struct playrule *p;  
int mode;
{
 /* p[n].name hp mp speed attack resist money special map */ 
 struct playrule m; 
 char buf[256];
 char inbuf1[20];
 char inbuf2[20]; 
 int pipkey; 
 int mankey;
 int lucky;
 int dinjure=0;		/*小雞傷害力*/
 int minjure=0;		/*對方傷害力*/
 int dresistmore=0;	/*小雞加強防禦*/
 int mresistmore=0;	/*對方加強防禦*/
 int oldhexp;		/*未戰鬥前格鬥經驗*/
 int oldmexp;		/*未戰鬥前魔法經驗*/
 int oldbrave;		/*未戰鬥前勇敢*/
 int oldhskill;		/*未戰鬥前戰鬥技術*/
 int oldmskill;		/*未戰鬥前魔法技術*/
 int oldetchics;	/*未戰鬥前道德*/
 int oldmoney;		/*未戰鬥前金錢*/
 int oldtired;
 int oldhp;
 int winorlose=0;		/*1:you win 0:you loss*/
  
 /*隨機產生人物 並且存好戰鬥前的一些數值*/
 oldhexp=d.hexp;
 oldmexp=d.mexp;
 oldbrave=d.brave;
 oldhskill=d.hskill;
 oldmskill=d.mskill;
 oldetchics=d.etchics;
 oldmoney=d.money;
 if(mode==1)
 {
	 m.hp=p[n].hp-rand()%10;
	 m.maxhp=(m.hp+p[n].hp)/2;
	 m.mp=p[n].mp-rand()%10;
	 m.maxmp=(m.mp+p[n].mp)/2;
	 m.speed=p[n].speed-rand()%4-1;
	 m.attack=p[n].attack-rand()%10;
	 m.resist=p[n].resist-rand()%10;
	 m.money=p[n].money-rand()%50;
	 m.death=p[n].death;
 }
 else
 {
 	 m.maxhp=d.maxhp*(80+rand()%50)/100+20;;
 	 m.hp=m.maxhp-rand()%10+20;
 	 m.maxmp=d.maxmp*(80+rand()%50)/100+10;
 	 m.mp= m.maxmp-rand()%20+10;
 	 m.speed=d.speed*(80+rand()%50)/100+10;
 	 m.attack=d.attack*(80+rand()%50)/100+10;
 	 m.resist=d.resist*(80+rand()%50)/100+10;
 	 m.money=0;
 	 m.death=0;
 }
 /*d.tired+=rand()%(n+1)/4+2;*/
 /*d.shit+=rand()%(n+1)/4+2;*/
 do
 { 
   if(m.hp<=0) /*敵人死掉了*/
   {
     m.hp=0;
     d.money+=m.money;
     m.death=1;
     d.brave+=rand()%4+3;
   }
   if(d.hp<=0 || d.tired>=100)  /*小雞陣亡*/
   {
     if(mode==1)
     {
       d.hp=0;
       d.tired=0;
       d.death=1;
     }
     else
     {
       d.hp=d.maxhp/3+10;
       d.hexp-=rand()%3+2;
       d.mexp-=rand()%3+2;
       d.tired=50;
       d.death=1;
     }
   }        
   clear(); 
   /*showtitle("電子養小雞", BoardName);*/
   move(0,0);
   if(d.sex==1)
     sprintf(buf,"[1;41m  星空戰鬥雞 ∼ [32m♂ [37m%-10s                                                  [0m",d.name); 	 
   else if(d.sex==2)
     sprintf(buf,"[1;41m  星空戰鬥雞 ∼ [33m♀ [37m%-10s                                                  [0m",d.name); 	 
   else 
     sprintf(buf,"[1;41m  星空戰鬥雞 ∼ [34m？ [37m%-10s                                                  [0m",d.name); 	 
   prints(buf);    
   move(6,0);
   if(mode==1)
	   show_badman_pic(n);
   move(1,0);
   sprintf(buf,"[1;31m┌─────────────────────────────────────┐[m");
   prints(buf);
   move(2,0);
   /* lucky拿來當color用*/
   if(d.tired>=80)
      lucky=31;
   else if(d.tired>=60 && d.tired< 80)
      lucky=33;
   else
      lucky=37;
   sprintf(inbuf1,"%d/%d",d.hp,d.maxhp);  
   sprintf(inbuf2,"%d/%d",d.mp,d.maxmp);    
   sprintf(buf,"[1;31m│[33m生  命:[37m%-12s[33m法  力:[37m%-12s[33m疲  勞:[%dm%-12d[33m金  錢:[37m%-10d[31m│[m",
  	 inbuf1,inbuf2,lucky,d.tired,d.money);
   prints(buf);
   move(3,0);
   sprintf(inbuf1,"%d/%d",d.hexp,d.mexp);  
   sprintf(buf,"[1;31m│[33m攻  擊:[37m%-10d  [33m防  禦:[37m%-10d  [33m速  度:[37m%-5d       [33m經  驗:[37m%-10s[31m│[m",
 	 d.attack,d.resist,d.speed,inbuf1);
   prints(buf);
   move(4,0);
   sprintf(buf,"[1;31m│[33m食  物:[37m%-5d       [33m大補丸:[37m%-5d       [33m零  食:[37m%-5d       [33m藥  草:[37m%-5d     [31m│[m",
 	 d.food,d.bighp,d.cookie,d.medicine);
   prints(buf);	 
   move(5,0);
   sprintf(buf,"[1;31m└─────────────────────────────────────┘[m");
   prints(buf); 
   move(19,0);
   sprintf(buf,"[1;34m┌─────────────────────────────────────┐[m");
   prints(buf);
   move(20,0);
   sprintf(inbuf1,"%d/%d",m.hp,m.maxhp);  
   sprintf(inbuf2,"%d/%d",m.mp,m.maxmp);     
   sprintf(buf,"[1;34m│[32m姓  名:[37m%-10s  [32m生  命:[37m%-11s [32m法  力:[37m%-11s                  [34m│[m",
 	 p[n].name,inbuf1,inbuf2);
   prints(buf);
   move(21,0);
   sprintf(buf,"[1;34m│[32m攻  擊:[37m%-6d      [32m防  禦:[37m%-6d      [32m速  度:[37m%-6d      [32m金  錢:[37m%-6d    [34m│[m",
 	 m.attack,m.resist,m.speed,m.money);
   prints(buf);
   move(22,0);
   sprintf(buf,"[1;34m└─────────────────────────────────────┘[m");
   prints(buf);
   move(b_lines,0);   
   sprintf(buf,"[1;44;37m  戰鬥命令  [46m  [1]普通  [2]全力  [3]魔法  [4]防禦  [5]補充  [6]逃命         [m");
   prints(buf);    
   
   if(m.death==0 && d.death==0)
   {
    dresistmore=0;
    d.nodone=0;
    pipkey=egetch(); 
    switch(pipkey)
    {
     case '1': 
      if(rand()%9==0)
      {
        pressanykey("竟然沒打中..:~~~"); 
      }
      else
      { 
      	if(mresistmore==0)
          dinjure=(d.hskill/100+d.hexp/100+d.attack/9-m.resist/12+rand()%12+2-m.speed/30+d.speed/30);
        else
	  dinjure=(d.hskill/100+d.hexp/100+d.attack/9-m.resist/8+rand()%12+2-m.speed/30+d.speed/30);            
	if(dinjure<=0)
	  dinjure=9;
        m.hp-=dinjure;
        d.hexp+=rand()%2+2;
        d.hskill+=rand()%2+1;  
        sprintf(buf,"普通攻擊,對方生命力減低%d",dinjure);
        pressanykey(buf);
      }
      d.tired+=rand()%(n+1)/15+2;
      break;
     
     case '2':
      show_fight_pic(2);
      if(rand()%11==0)
      { 
        pressanykey("竟然沒打中..:~~~");
      }     
      else 
      { 
        if(mresistmore==0)      
          dinjure=(d.hskill/100+d.hexp/100+d.attack/5-m.resist/12+rand()%12+6-m.speed/50+d.speed/30);
        else
          dinjure=(d.hskill/100+d.hexp/100+d.attack/5-m.resist/8+rand()%12+6-m.speed/40+d.speed/30);                  
        if(dinjure<=15)
          dinjure=20;  
        if(d.hp>5)
        { 
          m.hp-=dinjure;
          d.hp-=5;
          d.hexp+=rand()%3+3;
          d.hskill+=rand()%2+2; 
          d.tired+=rand()%(n+1)/10+3; 
          sprintf(buf,"全力攻擊,對方生命力減低%d",dinjure);
          pressanykey(buf);
        }
        else
        { 
          d.nodone=1;
          pressanykey("你的HP小於5啦..不行啦...");
        }
      }
      break;
     
     case '3':
      oldtired=d.tired;
      oldhp=d.hp;     
      d.magicmode=0;
      dinjure=pip_magic_menu(); 
      if(dinjure<0)
        dinjure=5;
      if(d.nodone==0)
      {
        if(d.magicmode==1)
        {
          oldtired=oldtired-d.tired;
          oldhp=d.hp-oldhp;
          sprintf(buf,"治療後,生命力提高%d 疲勞降低%d",oldhp,oldtired);
          pressanykey(buf);
        }
        else
        {
          if(rand()%15==0)
             pressanykey("竟然沒打中..:~~~");  
          else
          {  
             if(d.mexp<=100)
             {
                if(rand()%4>0)
                    dinjure=dinjure*60/100;
                else
                    dinjure=dinjure*80/100;
             }
             else if(d.mexp<=250 && d.mexp>100)
             {
                if(rand()%4>0)
                    dinjure=dinjure*70/100;
                else
                    dinjure=dinjure*85/100;           
             }
             else if(d.mexp<=500 && d.mexp>250)
             {
                if(rand()%4>0)
                    dinjure=dinjure*85/100;
                else
                    dinjure=dinjure*95/100;           
             }
             else if(d.mexp>500)
             {
                if(rand()%10>0)
                    dinjure=dinjure*90/100;
                else
                    dinjure=dinjure*99/100;           
             }
             if((p[n].special[d.magicmode-2]-48)==1)
             {
                if(rand()%2>0)
                {
                  dinjure=dinjure*125/100;
                }
                else
                {
                  dinjure=dinjure*110/100;
                }
             }
             else
             {
                if(rand()%2>0)
                {
                  dinjure=dinjure*60/100;
                }
                else
                {  
                  dinjure=dinjure*75/100;           
                }
             }
             d.tired+=rand()%(n+1)/12+2;
             m.hp-=dinjure; 
             /*d.mexp+=rand()%2+2;*/
             d.mskill+=rand()%2+2;  
             sprintf(buf,"魔法攻擊,對方生命力減低%d",dinjure);
             pressanykey(buf);           
          }
        }
      }
      break;    
     case '4':
      dresistmore=1;
      d.tired+=rand()%(n+1)/20+1;
      pressanykey("小雞加強防禦啦....");
      break;

     case '5':
      
      pip_basic_feed();
      break;
      
     case '6':
      d.money-=(rand()%100+30);
      d.brave-=(rand()%3+2);      
      if(d.money<0)
      	d.money=0;
      if(d.hskill<0)
      	d.hskill=0;
      if(d.brave<0)
        d.brave=0;
      clear(); 
      showtitle("電子養小雞", BoardName); 
      move(10,0);
      prints("            [1;31m┌──────────────────────┐[m");
      move(11,0);
      prints("            [1;31m│  [37m實力不強的小雞 [33m%-10s                 [31m│[m",d.name);
      move(12,0);
      prints("            [1;31m│  [37m在與對手 [32m%-10s [37m戰鬥後落跑啦          [31m│[m",p[n].name);
      move(13,0);
      sprintf(inbuf1,"%d/%d",d.hexp-oldhexp,d.mexp-oldmexp);  
      prints("            [1;31m│  [37m評價增加了 [36m%-5s [37m點  技術增加了 [36m%-2d/%-2d [37m點  [31m│[m",inbuf1,d.hskill-oldhskill,d.mskill-oldmskill);
      move(14,0);
      sprintf(inbuf1,"%d [37m元",oldmoney-d.money);
      prints("            [1;31m│  [37m勇敢降低了 [36m%-5d [37m點  金錢減少了 [36m%-13s  [31m│[m",oldbrave-d.brave,inbuf1);
      move(15,0);
      prints("            [1;31m└──────────────────────┘[m");             
      pressanykey("三十六計 走為上策...");
      winorlose=0;
      break;
     }   
   }
   clear(); 
   move(0,0);
   if(d.sex==1)
     sprintf(buf,"[1;41m  星空戰鬥雞 ∼ [32m♂ [37m%-10s                                                  [0m",d.name); 	 
   else if(d.sex==2)
     sprintf(buf,"[1;41m  星空戰鬥雞 ∼ [33m♀ [37m%-10s                                                  [0m",d.name); 	 
   else 
     sprintf(buf,"[1;41m  星空戰鬥雞 ∼ [34m？ [37m%-10s                                                  [0m",d.name); 	 
   prints(buf);    
   move(1,0);
   sprintf(buf,"[1;31m┌─────────────────────────────────────┐[m");
   prints(buf);
   move(2,0);
   /* lucky拿來當color用*/
   if(d.tired>=80)
      lucky=31;
   else if(d.tired>=60 && d.tired< 80)
      lucky=33;
   else
      lucky=37;
   sprintf(inbuf1,"%d/%d",d.hp,d.maxhp);  
   sprintf(inbuf2,"%d/%d",d.mp,d.maxmp);    
   sprintf(buf,"[1;31m│[33m生  命:[37m%-12s[33m法  力:[37m%-12s[33m疲  勞:[%dm%-12d[33m金  錢:[37m%-10d[31m│[m",
  	 inbuf1,inbuf2,lucky,d.tired,d.money);
   prints(buf);
   
   move(3,0);
   sprintf(inbuf1,"%d/%d",d.hexp,d.mexp);     
   sprintf(buf,"[1;31m│[33m攻  擊:[37m%-6d      [33m防  禦:[37m%-6d      [33m速  度:[37m%-5d       [33m經  驗:[37m%-10s[31m│[m",
 	 d.attack,d.resist,d.speed,inbuf1);
   prints(buf);
   move(4,0);
   sprintf(buf,"[1;31m│[33m食  物:[37m%-5d       [33m大補丸:[37m%-5d       [33m零  食:[37m%-5d       [33m藥  草:[37m%-5d     [31m│[m",
 	 d.food,d.bighp,d.cookie,d.medicine);
   prints(buf);	 
   move(5,0);
   sprintf(buf,"[1;31m└─────────────────────────────────────┘[m");
   prints(buf);    
   move(6,0);
   if(mode==1)
	   show_badman_pic(n);
   move(19,0);
   sprintf(buf,"[1;34m┌─────────────────────────────────────┐[m");
   prints(buf);
   move(20,0);
   sprintf(inbuf1,"%d/%d",m.hp,m.maxhp);  
   sprintf(inbuf2,"%d/%d",m.mp,m.maxmp);     
   sprintf(buf,"[1;34m│[32m姓  名:[37m%-10s  [32m生  命:[37m%-11s [32m法  力:[37m%-11s                  [34m│[m",
 	 p[n].name,inbuf1,inbuf2);
   prints(buf);
   move(21,0);
   sprintf(buf,"[1;34m│[32m攻  擊:[37m%-6d      [32m防  禦:[37m%-6d      [32m速  度:[37m%-6d      [32m金  錢:[37m%-6d    [34m│[m",
 	 m.attack,m.resist,m.speed,m.money);
   prints(buf);
   move(22,0);
   sprintf(buf,"[1;34m└─────────────────────────────────────┘[m");
   prints(buf);
   move(b_lines,0);   
   sprintf(buf,"[1;41;37m  [37m攻擊命令  [47m  [31m[1][30m普通  [31m[2][30m全力  [31m[3][30m魔法  [31m[4][30m防禦  [31m[5][30m逃命                     [m");
   prints(buf);       
   
   if((m.hp>0)&&(pipkey!='6')&& (pipkey=='1'||pipkey=='2'||pipkey=='3'||pipkey=='4'||pipkey=='5')&&(d.death==0)&&(d.nodone==0))
   {
     mresistmore=0;
     lucky=rand()%100;
     if(lucky>=0 && lucky<=50)
        mankey=1;
     else if(lucky>=51 && lucky<=84)
        mankey=2;  
     else if(lucky>=85 && lucky<=97)
        mankey=3;
     else if(lucky>=98)
        mankey=4;   
     switch(mankey)
     {    
      case 1:
       if(rand()%6==5)
       { 
         pressanykey("對方沒打中..:~~~");
       }     
       else
       {
        if(dresistmore==0)      
          minjure=(m.attack/9-d.resist/12+rand()%15+4-d.speed/30+m.speed/30-d.hskill/200-d.hexp/200);
        else
          minjure=(m.attack/9-d.resist/8+rand()%12+4-d.speed/50+m.speed/20-d.hskill/200-d.hexp/200);
        if(minjure<=0)
          minjure=8;  
        d.hp-=minjure;
        d.tired+=rand()%3+2;
        sprintf(buf,"對方普通攻擊,生命力減低%d",minjure);
        pressanykey(buf);
       }             
       break;
       
      case 2:       
       if(rand()%11==10)
       { 
         pressanykey("對方沒打中..:~~~");
       }     
       else
       {
        if(m.hp>5)
        {
          if(dresistmore==0)      
            minjure=(m.attack/5-d.resist/12+rand()%12+6-d.speed/30+m.speed/30-d.hskill/200-d.hexp/200);
          else
            minjure=(m.attack/5-d.resist/8+rand()%12+6-d.speed/30+m.speed/30-d.hskill/200-d.hexp/200);                  
	  if(minjure<=15)
	    minjure=20;
          d.hp-=minjure;
          m.hp-=5;
          sprintf(buf,"對方全力攻擊, 生命力減低%d",minjure);
          d.tired+=rand()%4+4;
          pressanykey(buf);
        }
        else
        {
          if(dresistmore==0)      
            minjure=(m.attack/9-d.resist/12+rand()%12+4-d.speed/30+m.speed/25-d.hexp/200-d.hskill/200);
          else
            minjure=(m.attack/9-d.resist/8+rand()%12+3-d.speed/30+m.speed/25-d.hexp/200-d.hskill/200);        
          if(minjure<=0)
            minjure=4;
          d.hp-=minjure;
          d.tired+=rand()%3+2;
          sprintf(buf,"對方普通攻擊,生命力減低%d",minjure);
          pressanykey(buf);        
        }
       }             
       break;             
       
      case 3:
       if(rand()%5>0 && m.mp>20)
       {
        if(rand()%6>0 && m.mp>=50)
        {
         if(m.mp>=1000)
         {
           minjure=500;
           m.mp-=(500+rand()%300);
           if(rand()%2)
	      sprintf(inbuf1,"熱火魔");
	   else
	      sprintf(inbuf1,"寒氣鬼");
         }
         else if(m.mp<1000 && m.mp>=500)
         {
           minjure=300;
           m.mp-=(300+rand()%200);
           if(rand()%2)
	      sprintf(inbuf1,"狂水怪");
	   else
	      sprintf(inbuf1,"怒土虫");
         }
         else if(m.mp<500 && m.mp>=200)
         {
           minjure=100;
           m.mp-=(100+rand()%100);
           if(rand()%2)
	      sprintf(inbuf1,"迷魂鬼差");
	   else
	      sprintf(inbuf1,"石怪");
         }
         else if(m.mp<200 && m.mp>=50)
         {
           minjure=50;
           m.mp-=50;
           if(rand()%2)
	      sprintf(inbuf1,"鬼木魂");
	   else
	      sprintf(inbuf1,"風妖");
         }
         minjure=minjure-d.resist/50-d.mresist/10-d.mskill/200-d.mexp/200+rand()%10;
         if(minjure<0)
         	minjure=15;
         d.hp-=minjure;
         d.mresist+=rand()%2+1;
         sprintf(buf,"對方招換了%s,你受傷了%d點",inbuf1,minjure);
         pressanykey(buf);
        }
        else
        {
         m.mp-=20;
         m.hp+=130+rand()%20;
         if(m.hp>m.maxhp)
            m.hp=m.maxhp;
         pressanykey("對方使用魔法治療了自己...");
        }
       }
       else
       {
         mresistmore=1;
         pressanykey("對方加強防禦....");
       }
       break;
      
      case 4:
       d.money+=(m.money+m.money/2)/3+rand()%10;
       d.hskill+=rand()%4+3;
       d.brave+=rand()%3+2;
       m.death=1;
       sprintf(buf,"對方先閃了..但掉了一些錢給你...");
       pressanykey(buf);       
       break;      
     }
   }
   
   if(m.death==1)
   {
     clear();
     showtitle("電子養小雞", BoardName);
     if(mode==1)
     { 
       move(10,0);
       prints("            [1;31m┌──────────────────────┐[m");
       move(11,0);
       prints("            [1;31m│  [37m英勇的小雞 [33m%-10s                     [31m│[m",d.name);
       move(12,0);
       prints("            [1;31m│  [37m打敗了邪惡的怪物 [32m%-10s               [31m│[m",p[n].name);
     }  
     else
     {
       move(10,0);
       prints("            [1;31m┌──────────────────────┐[m");
       move(11,0);
       prints("            [1;31m│  [37m武術大會的小雞 [33m%-10s                 [31m│[m",d.name);
       move(12,0);
       prints("            [1;31m│  [37m打敗了強勁的對手 [32m%-10s               [31m│[m",p[n].name);
     }
     move(13,0);
     sprintf(inbuf1,"%d/%d",d.hexp-oldhexp,d.mexp-oldmexp);  
     prints("            [1;31m│  [37m評價提升了 %-5s 點  技術增加了 %-2d/%-2d 點  [31m│[m",inbuf1,d.hskill-oldhskill,d.mskill-oldmskill);
     move(14,0);
     sprintf(inbuf1,"%d 元",d.money-oldmoney);
     prints("            [1;31m│  [37m勇敢提升了 %-5d 點  金錢增加了 %-9s [31m│[m",d.brave-oldbrave,inbuf1);
     move(15,0);
     prints("            [1;31m└──────────────────────┘[m");     
     
     if(m.hp<=0)     
        pressanykey("對方死掉囉..所以你贏囉..");   
     else if(m.hp>0)
        pressanykey("對方落跑囉..所以算你贏囉.....");   
     winorlose=1;
   }
   if(d.death==1 && mode==1)
   {   
       clear();
       showtitle("電子養小雞", BoardName);       
       move(10,0);
       prints("            [1;31m┌──────────────────────┐[m");
       move(11,0);
       prints("            [1;31m│  [37m可憐的小雞 [33m%-10s                     [31m│[m",d.name);
       move(12,0);
       prints("            [1;31m│  [37m在與 [32m%-10s [37m的戰鬥中，                [31m│[m",p[n].name);
       move(13,0);
       prints("            [1;31m│  [37m不幸地陣亡了，在此特別默哀..........      [31m│[m");
       move(14,0);
       prints("            [1;31m└──────────────────────┘[m");        
       pressanykey("小雞陣亡了....");      
       pipdie("[1;31m戰鬥中被打死了...[m  ",1);       
   }
   else if(d.death==1 && mode==2)
   {
       clear();
       showtitle("電子養小雞", BoardName);       
       move(10,0);
       prints("            [1;31m┌──────────────────────┐[m");
       move(11,0);
       prints("            [1;31m│  [37m可憐的小雞 [33m%-10s                     [31m│[m",d.name);
       move(12,0);
       prints("            [1;31m│  [37m在與 [32m%-10s [37m的戰鬥中，                [31m│[m",p[n].name);
       move(13,0);
       prints("            [1;31m│  [37m不幸地打輸了，記者現場特別報導.........   [31m│[m");
       move(14,0);
       prints("            [1;31m└──────────────────────┘[m");        
       pressanykey("小雞打輸了....");     
   }
 }while((pipkey!='6')&&(d.death!=1)&&(m.death!=1)&&(mankey!=8)); 
 return winorlose;    
}                 


/*---------------------------------------------------------------------------*/
/* 戰鬥魔法函式                                                              */
/*                                                                           */
/*---------------------------------------------------------------------------*/

/*進入使用魔法選單*/
int
pip_magic_menu()   /*戰鬥中法術的應用*/
{
  char buf[256];
  int injure;		/*傷害力*/
  int pipkey;
  
  do
  {
   move(b_lines, 0);
   clrtoeol();
   move(b_lines,0);
   sprintf(buf,
   "[1;44;37m  魔法選單  [46m  [1]治療 [2]雷系 [3]冰系 [4]火系 [5]土系 [6]風系 [Q]放棄: [m");
   move(b_lines,0);
   prints(buf);   
   pipkey=egetch();
   switch(pipkey)
   {
    case '1':  /*治療法術*/
     d.magicmode=1;
     injure=pip_magic_doing_menu(treatmagiclist);
     break;

    case '2':  /*雷系法術*/
     d.magicmode=2;
     injure=pip_magic_doing_menu(thundermagiclist);
     break;

    case '3': /*冰系法術*/
     d.magicmode=3;
     injure=pip_magic_doing_menu(icemagiclist);
     break;

    case '4': /*炎系法術*/
     d.magicmode=4;
     injure=pip_magic_doing_menu(firemagiclist);
     show_fight_pic(341);
     pressanykey("小雞使用了炎系法術");
     break;

    case '5': /*土系法術*/
     d.magicmode=5;
     injure=pip_magic_doing_menu(earthmagiclist);
     break;

    case '6': /*風系法術*/
     d.magicmode=6;
     injure=pip_magic_doing_menu(windmagiclist);
     break;
   }     
  }while((pipkey!='1')&&(pipkey!='2')&&(pipkey!='3')&&(pipkey!='4')&&(pipkey!='5')&&(pipkey!='6')&&(pipkey!='Q')&&(pipkey!='q')&&(d.nodone==0));

  if((pipkey=='Q')||(pipkey=='q'))
  {
   d.nodone=1;
  }
  return injure;    
}

/*魔法視窗*/
int
pip_magic_doing_menu(p)   /*魔法畫面*/
struct magicset *p;
{
 register int n=1;
 register char *s;
 char buf[256];
 char ans[5];
 int pipkey;
 int injure=0;

 d.nodone=0; 

 clrchyiuan(6,18);
 move(7,0);
 sprintf(buf,"[1;31m┤[37;41m   可用[%s]一覽表   [0;1;31m├────────────[m",p[0].name);
 prints(buf);
 while ((s = p[n].name)&& (p[n].needmp<=d.mp))
 {
  move(7+n,4);
  if(p[n].hpmode==1)
  {   
      sprintf(buf,
      "[1;37m[[36m%d[37m] [33m%-10s  [37m需要法力: [32m%-6d  [37m恢復體力: [32m%-6d [37m恢復疲勞: [32m%-6d[m   ",n,p[n].name,p[n].needmp,p[n].hp,p[n].tired);
      prints(buf);
  }
  else if(p[n].hpmode==2)
  {
      sprintf(buf,
      "[1;37m[[36m%d[37m] [33m%-10s  [37m需要法力: [32m%-6d  [37m恢復體力到[35m最大值[37m 恢復疲勞到[35m最小值[m  ",n,p[n].name,p[n].needmp);
      prints(buf);
  }  
  else if(p[n].hpmode==0)
  {
      sprintf(buf,
      "[1;37m[[36m%d[37m] [33m%-10s  [37m需要法力: [32m%-6d [m             ",n,p[n].name,p[n].needmp);
      prints(buf);
  }
  n++;
 }  
 n-=1;
 
 do{
   move(16,4);
   sprintf(buf,"你想使用那一個%8s呢?  [Q]放棄:",p[0].name);
   getdata(16,4,buf, ans, 2, 1, 0);    
   if( ans[0]!='q' && ans[0]!='Q')
   {
     pipkey=atoi(ans);
   }
 }while( ans[0]!='q' && ans[0]!='Q' && (pipkey>n || pipkey <=0));
 
 if(ans[0]!='q' && ans[0]!='Q')
 {
   getdata(17,4,"確定使用嗎? [Y/n]", ans, 2, 1, 0);
   if(ans[0]!='n' && ans[0]!='N')
   {
     if(p[pipkey].hpmode==1)
     {
      d.hp+=p[pipkey].hp;
      d.tired-=p[pipkey].tired;      
      d.mp-=p[pipkey].needmp;
      if(d.hp>d.maxhp)
        d.hp=d.maxhp;
      if(d.tired<0)
        d.tired=0;
      injure=0;
     }
     else if(p[pipkey].hpmode==2)
     {
      d.hp=d.maxhp;
      d.mp-=p[pipkey].needmp;
      d.tired=0;
      injure=0;
     } 
     else
     {
      injure=(p[pipkey].hp-rand()%5);               
      d.mp-=p[pipkey].needmp;
     }
     d.mexp+=rand()%3+pipkey;
   }
   else
   {
     d.nodone=1;
     injure=0;
   }
 }
 else
 {
   d.nodone=1;
   injure=0; 
 }
 return injure;
}

/*---------------------------------------------------------------------------*/
/* 函式特區                                                                  */
/*                                                                           */
/*---------------------------------------------------------------------------*/

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
   getdata(b_lines-1, 1,buf, ans, 2, 1, 0);
   if(ans[0]=='y' || ans[0]=='Y')
   {
     if(d.wantend!=1 && d.wantend!=4)
     {
       sprintf(buf,"ㄚ∼之前已經有婚約了，您確定要解除舊婚約，改定立婚約嗎？[y/N]");
       getdata(b_lines-1, 1,buf, ans, 2, 1, 0);
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


/*---------------------------------------------------------------------------*/
/* 結局函式                                                                  */
/*                                                                           */
/*---------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/*  結局參數設定                                                            */
/*--------------------------------------------------------------------------*/

int /*結局畫面*/
pip_ending_screen()
{
  time_t now;
  char buf[256];
  char endbuf1[50];
  char endbuf2[50];
  char endbuf3[50];
  int endgrade=0;
  int endmode=0;
  clear();
  pip_ending_decide(endbuf1,endbuf2,endbuf3,&endmode,&endgrade);
  move(1,9); 
  prints("[1;33m?歈??????裐歈??????裐歈????????歈??????裐歈??????裐?????????[0m");
  move(2,9);
  prints("[1;37m??      ?矙?    ?矙矙?      ?矙?      ?矙?    ?矙矙?      ??[0m");
  move(3,9);
  prints("[0;37m??    ???禊?    ?矙矙?  ?????矙裺??裐歈潁?    ?矙矙?  ?歈???[0m");
  move(4,9);
  prints("[0;37m??    ???禊?  ??  ?矙?  ?????矙歈??潁裺裐?  ??  ?矙?  ??????[0m");
  move(5,9);
  prints("[1;37m??      ?矙?  ??  ?矙?      ?矙?      ?矙?  ??  ?矙?      ??[0m");
  move(6,9);
  prints("[1;35m?裺??????潁裺??????潁裺????????裺??????潁裺??????潁?????????[0m");
  move(7,8);
  prints("[1;31m──────────[41;37m 星空戰鬥雞結局報告 [0;1;31m───────────[0m");
  move(9,10);
  prints("[1;36m這個時間不知不覺地還是到臨了...[0m");
  move(11,10);
  prints("[1;37m[33m%s[37m 得離開你的溫暖懷抱，自己一隻雞在外面求生存了.....[0m",d.name);
  move(13,10);
  prints("[1;36m在你照顧教導他的這段時光，讓他接觸了很多領域，培養了很多的能力....[0m");
  move(15,10);
  prints("[1;37m因為這些，讓小雞 [33m%s[37m 之後的生活，變得更多采多姿了........[0m",d.name);
  move(17,10);
  prints("[1;36m對於你的關心，你的付出，你所有的愛......[0m");
  move(19,10);
  prints("[1;37m[33m%s[37m 會永遠都銘記在心的....[0m",d.name);
  pressanykey("接下來看未來發展");
  clrchyiuan(7,19);
  move(7,8);
  prints("[1;34m──────────[44;37m 星空戰鬥雞未來發展 [0;1;34m───────────[0m");
  move(9,10);
  prints("[1;36m透過水晶球，讓我們一起來看 [33m%s[36m 的未來發展吧.....[0m",d.name);
  move(11,10);
  prints("[1;37m小雞 [33m%s[37m 後來%s....[0m",d.name,endbuf1);
  move(13,10);
  prints("[1;36m因為他的之前的努力，使得他在這一方面%s....[0m",endbuf2);
  move(15,10);
  prints("[1;37m至於小雞的婚姻狀況，他後來%s，婚姻算是很美滿.....[0m",endbuf3);
  move(17,10);
  prints("[1;36m嗯..這是一個不錯的結局唷..........[0m");
  pressanykey("我想  你一定很感動吧.....");
  show_ending_pic(0);
  pressanykey("看一看分數囉");
  endgrade=pip_game_over(endgrade);
  inmoney(endgrade*10*ba);
  inexp(endgrade*ba);
  sprintf(buf,"/bin/rm home/%s/chicken home/%s/chicken.bak*",cuser.userid,cuser.userid);
  system(buf);
  pressanykey("得到 %d 元,%d 點經驗值",endgrade*10*ba,endgrade*10); 
  pressanykey("下一頁是小雞資料  趕快copy下來做紀念");
  pip_data_list(cuser.userid);
  pressanykey("歡迎再來挑戰....");
  /*記錄開始*/
  now=time(0);
  sprintf(buf, "[1;35m───────────────────────────────────────[0m\n");
  pip_log_record(buf);
  sprintf(buf, "[1;37m在 [33m%s [37m的時候，[36m%s [37m的小雞 [32m%s[37m 出現了結局[0m\n", Cdate(&now), cuser.userid,d.name);
  pip_log_record(buf);
  sprintf(buf, "[1;37m小雞 [32m%s [37m努力加強自己，後來%s[0m\n[1;37m因為之前的努力，使得在這一方面%s[0m\n",d.name,endbuf1,endbuf2);
  pip_log_record(buf);
  sprintf(buf, "[1;37m至於婚姻狀況，他後來%s，婚姻算是很美滿.....[0m\n\n[1;37m小雞 [32n%s[37m 的總積分＝ [33m%d[0m\n",endbuf3,d.name,endgrade);
  pip_log_record(buf);
  sprintf(buf, "[1;35m───────────────────────────────────────[0m\n");
  pip_log_record(buf);  
  /*記錄終止*/
  d.death=3;
  pipdie("[1;31m遊戲結束囉...[m  ",3);
  return 0;
}

int 
pip_ending_decide(endbuf1,endbuf2,endbuf3,endmode,endgrade)
char *endbuf1,*endbuf2,*endbuf3;
int *endmode,*endgrade;
{
  char *name[8][2]={{"男的","女的"},
  	           {"嫁給王子","娶了公主"},
  	           {"嫁給你","娶了你"},
                   {"嫁給商人Ａ","娶了女商人Ａ"},
                   {"嫁給商人Ｂ","娶了女商人Ｂ"},
                   {"嫁給商人Ｃ","娶了女商人Ｃ"},
                   {"嫁給商人Ｄ","娶了女商人Ｄ"},
                   {"嫁給商人Ｅ","娶了女商人Ｅ"}}; 
  int m=0,n=0,grade=0;
  int modeall_purpose=0;
  char buf1[256];
  char buf2[256];
  
  *endmode=pip_future_decide(&modeall_purpose);
  switch(*endmode)
  {
  /*1:暗黑 2:藝術 3:萬能 4:戰士 5:魔法 6:社交 7:家事*/
    case 1:
      pip_endingblack(buf1,&m,&n,&grade);    
      break;
    case 2:
      pip_endingart(buf1,&m,&n,&grade);
      break;
    case 3:
      pip_endingall_purpose(buf1,&m,&n,&grade,modeall_purpose);
      break;
    case 4:
      pip_endingcombat(buf1,&m,&n,&grade);
      break;
    case 5:
      pip_endingmagic(buf1,&m,&n,&grade);
      break;
    case 6:
      pip_endingsocial(buf1,&m,&n,&grade);
      break;
    case 7:
      pip_endingfamily(buf1,&m,&n,&grade);
      break;
  }
  
  grade+=pip_marry_decide();
  strcpy(endbuf1, buf1);  
  if(n==1)
  {
    *endgrade=grade+300;
    sprintf(buf2,"非常的順利..");
  }
  else if(n==2)
  {
    *endgrade=grade+100;
    sprintf(buf2,"表現還不錯..");
  }
  else if(n==3)
  {
    *endgrade=grade-10;
    sprintf(buf2,"常遇到很多問題....");
  }  
  strcpy(endbuf2, buf2);  
  if(d.lover>=1 && d.lover <=7)
  {
    if(d.sex==1)
      sprintf(buf2,"%s",name[d.lover][1]);
    else
      sprintf(buf2,"%s",name[d.lover][0]);
  }
  else if(d.lover==10)
    sprintf(buf2,"%s",buf1);
  else if(d.lover==0)
  {
    if(d.sex==1)
      sprintf(buf2,"娶了同行的女孩");
    else
      sprintf(buf2,"嫁給了同行的男生");  
  } 
  strcpy(endbuf3, buf2);  
  return 0;
}
/*結局判斷*/
/*1:暗黑 2:藝術 3:萬能 4:戰士 5:魔法 6:社交 7:家事*/
int
pip_future_decide(modeall_purpose)
int *modeall_purpose;
{
  int endmode;
  /*暗黑*/
  if((d.etchics==0 && d.offense >=100) || (d.etchics>0 && d.etchics<50 && d.offense >=250))
     endmode=1;
  /*藝術*/
  else if(d.art>d.hexp && d.art>d.mexp && d.art>d.hskill && d.art>d.mskill &&
          d.art>d.social && d.art>d.family && d.art>d.homework && d.art>d.wisdom &&
          d.art>d.charm && d.art>d.belief && d.art>d.manners && d.art>d.speech &&
          d.art>d.cookskill && d.art>d.love)
     endmode=2;
  /*戰鬥*/
  else if(d.hexp>=d.social && d.hexp>=d.mexp && d.hexp>=d.family)
  {
     *modeall_purpose=1;
     if(d.hexp>d.social+50 || d.hexp>d.mexp+50 || d.hexp>d.family+50)
        endmode=4;
     else
        endmode=3;     
  }
  /*魔法*/
  else if(d.mexp>=d.hexp && d.mexp>=d.social && d.mexp>=d.family)
  {  
     *modeall_purpose=2;
     if(d.mexp>d.hexp || d.mexp>d.social || d.mexp>d.family)
        endmode=5;
     else
        endmode=3;
  }   
  else if(d.social>=d.hexp && d.social>=d.mexp && d.social>=d.family)
  {
     *modeall_purpose=3;
     if(d.social>d.hexp+50 || d.social>d.mexp+50 || d.social>d.family+50)
        endmode=6;
     else
        endmode=3;
  }

  else 
  {  
     *modeall_purpose=4;
     if(d.family>d.hexp+50 || d.family>d.mexp+50 || d.family>d.social+50)
        endmode=7;
     else
        endmode=3;     
  }     
  return endmode;
}
/*結婚的判斷*/
int
pip_marry_decide()
{
  int grade;
  if(d.lover!=0)
  {  
     /* 3 4 5 6 7:商人 */
     d.lover=d.lover;
     grade=80;
  }
  else
  {
     if(d.royalJ>=d.relation && d.royalJ>=100)
     {
        d.lover=1;  /*王子*/
        grade=200;
     }
     else if(d.relation>d.royalJ && d.relation>=100)
     {
        d.lover=2;  /*父親或母親*/
        grade=0;
     }
     else
     {
        d.lover=0;
        grade=40;
     }
  }
  return grade;
}


int
pip_endingblack(buf,m,n,grade) /*暗黑*/
char *buf;
int *m,*n,*grade;
{
 if(d.offense>=500 && d.mexp>=500) /*魔王*/
 {
   *m=1;
   if(d.mexp>=1000)
     *n=1;
   else if(d.mexp<1000 && d.mexp >=800)
     *n=2;
   else
     *n=3;
 }

else if(d.hexp>=600)  /*流氓*/
 {
   *m=2;
   if(d.wisdom>=350)
     *n=1;
   else if(d.wisdom<350 && d.wisdom>=300)
     *n=2;
   else 
     *n=3;  
 } 
 else if(d.speech>=100 && d.art>=80) /*SM*/
 {
   *m=3;
   if(d.speech>150 && d.art>=120)
     *n=1;
   else if(d.speech>120 && d.art>=100)
     *n=2;
   else   
     *n=3;
 }
 else if(d.hexp>=320 && d.character>200 && d.charm< 200)	/*黑街老大*/
 {
   *m=4;
   if(d.hexp>=400)
     *n=1;
   else if(d.hexp<400 && d.hexp>=360)
     *n=2;
   else 
     *n=3;  
 }
 else if(d.character>=200 && d.charm >=200 && d.speech>70 && d.toman >70)  /*高級娼婦*/
 {
   *m=5;
   if(d.charm>=300)
     *n=1;
   else if(d.charm<300 && d.charm>=250)
     *n=2;
   else 
     *n=3;  
 }
 
 else if(d.wisdom>=450)  /*詐騙師*/
 {
   *m=6;
   if(d.wisdom>=550)
     *n=1;
   else if(d.wisdom<550 && d.wisdom>=500)
     *n=2;
   else 
     *n=3;  
 }
 
 else /*流鶯*/
 {
   *m=7;
   if(d.charm>=350)
     *n=1;
   else if(d.charm<350 && d.charm>=300)
     *n=2;
   else 
     *n=3;  
 }
 if(d.sex==1)
   strcpy(buf, endmodeblack[*m].boy);
 else
   strcpy(buf, endmodeblack[*m].girl);
 *grade=endmodeblack[*m].grade;
 return 0; 
}


int
pip_endingsocial(buf,m,n,grade) /*社交*/
char *buf;
int *m,*n,*grade;
{
 int class;
 if(d.social>600) class=1;
 else if(d.social>450) class=2;
 else if(d.social>380) class=3;
 else if(d.social>250) class=4;
 else class=5;

 switch(class)
 {
   case 1:
     if(d.charm>500)
     {
       *m=1;
       d.lover=10;
       if(d.character>=700)
        *n=1;
       else if(d.character<700 && d.character>=500)
        *n=2;
       else   
        *n=3;
     }
     else
     {
       *m=2;
       d.lover=10;
       if(d.character>=700)
        *n=1;
       else if(d.character<700 && d.character>=500)
        *n=2;
       else   
        *n=3;
     }
     break;
     
   case 2:
     *m=1;
     d.lover=10;
     if(d.character>=700)
        *n=1;
     else if(d.character<700 && d.character>=500)
        *n=2;
     else   
        *n=3;   
     break;
     
   case 3:
     if(d.character>=d.charm)
     {
       *m=3;
       d.lover=10;
       if(d.toman>=250)
         *n=1;
       else if(d.toman<250 && d.toman>=200)
         *n=2;
       else   
         *n=3;     
     }
     else
     {
       *m=4;
       d.lover=10;
       if(d.character>=400)
         *n=1;
       else if(d.character<400 && d.character>=300)
         *n=2;
       else   
         *n=3;     
     }
     break;
     
   case 4:
     if(d.wisdom>=d.affect)	
     {
	   *m=5;
	   d.lover=10;
	   if(d.toman>120 && d.cookskill>300 && d.homework>300)
	     *n=1;
	   else if(d.toman<120 && d.cookskill<300 && d.homework<300 &&d.toman>100 && d.cookskill>250 && d.homework>250)
	     *n=2;
	   else   
	     *n=3;     	
     }
     else
     {
	   *m=6;
	   d.lover=10;
	   if(d.hp>=400)
	     *n=1;
	   else if(d.hp<400 && d.hp>=300)
	     *n=2;
	   else   
	     *n=3;     
     }
     break;
   
   case 5:
     *m=7;
     d.lover=10;
     if(d.charm>=200)
       *n=1;
     else if(d.charm<200 && d.charm>=100)
       *n=2;
     else   
       *n=3;
     break;
 }
 if(d.sex==1)
   strcpy(buf, endmodesocial[*m].boy);
 else
   strcpy(buf, endmodesocial[*m].girl);
 *grade=endmodesocial[*m].grade;
 return 0; 
}

int
pip_endingmagic(buf,m,n,grade) /*魔法*/
char *buf;
int *m,*n,*grade;
{
 int class;
 if(d.mexp>800) class=1;
 else if(d.mexp>600) class=2;
 else if(d.mexp>500) class=3;
 else if(d.mexp>300) class=4;
 else class=5;

 switch(class)
 {
    case 1:
      if(d.affect>d.wisdom && d.affect>d.belief && d.etchics>100)
      {
	   *m=1;
	   if(d.etchics>=800)
	     *n=1;
	   else if(d.etchics<800 && d.etchics>=400)
	     *n=2;
	   else   
	     *n=3;      
      }
      else if(d.etchics<50)
      {
	   *m=4;
	   if(d.hp>=400)
	     *n=1;
	   else if(d.hp<400 && d.hp>=200)
	     *n=2;
	   else   
	     *n=3;    
      }
      else
      {
	   *m=2;
	   if(d.wisdom>=800)
	     *n=1;
	   else if(d.wisdom<800 && d.wisdom>=400)
	     *n=2;
	   else   
	     *n=3;      
      }
      break;
      
    case 2:
      if(d.etchics>=50)
      {
	   *m=3;
	   if(d.wisdom>=500)
	     *n=1;
	   else if(d.wisdom<500 && d.wisdom>=200)
	     *n=2;
	   else   
	     *n=3;     
      }
      else
      {
	   *m=4;
	   if(d.hp>=400)
	     *n=1;
	   else if(d.hp<400 && d.hp>=200)
	     *n=2;
	   else   
	     *n=3;          
      }
      break;
    
    case 3:
      *m=5;
      if(d.mskill>=300)
	*n=1;
      else if(d.mskill<300 && d.mskill>=150)
	*n=2;
      else   
	*n=3;
      break;

    case 4:
      *m=6;
      if(d.speech>=150)
	*n=1;
      else if(d.speech<150 && d.speech>=60)
	*n=2;
      else   
	*n=3;
      break; 
     
    case 5:
      if(d.character>=200)
      {
	*m=7;
        if(d.speech>=150)
 	  *n=1;
        else if(d.speech<150 && d.speech>=60)
      	  *n=2;
        else   
	  *n=3;        
      }
      else
      {
        *m=8;
        if(d.speech>=150)
 	  *n=1;
        else if(d.speech<150 && d.speech>=60)
      	  *n=2;
        else   
	  *n=3;      
      }
      break;
    
 }

 if(d.sex==1)
   strcpy(buf, endmodemagic[*m].boy);
 else
   strcpy(buf, endmodemagic[*m].girl); 
 *grade=endmodemagic[*m].grade;
 return 0; 
}

int
pip_endingcombat(buf,m,n,grade) /*戰鬥*/
char *buf;
int *m,*n,*grade;
{
 int class;
 if(d.hexp>1500) class=1;
 else if(d.hexp>1000) class=2;
 else if(d.hexp>800) class=3;
 else class=4;

 switch(class)
 {
    case 1:
      if(d.affect>d.wisdom && d.affect>d.belief && d.etchics>100)
      {
	   *m=1;
	   if(d.etchics>=800)
	     *n=1;
	   else if(d.etchics<800 && d.etchics>=400)
	     *n=2;
	   else   
	     *n=3;      
      }
      else if(d.etchics<50)
      {

      }
      else
      {
	   *m=2;
	   if(d.wisdom>=800)
	     *n=1;
	   else if(d.wisdom<800 && d.wisdom>=400)
	     *n=2;
	   else   
	     *n=3;      
      }
      break;	
    
    case 2:
      if(d.character>=300 && d.etchics>50)
      {
	   *m=3;
	   if(d.etchics>=300 && d.charm >=300)
	     *n=1;
	   else if(d.etchics<300 && d.charm<300 && d.etchics>=250 && d.charm >=250)
	     *n=2;
	   else   
	     *n=3;      
      }
      else if(d.character<300 && d.etchics>50)
      {
	   *m=4;
	   if(d.speech>=200)
	     *n=1;
	   else if(d.speech<150 && d.speech>=80)
	     *n=2;
	   else   
	     *n=3;      
      }
      else
      {
	   *m=7;
	   if(d.hp>=400)
	     *n=1;
	   else if(d.hp<400 && d.hp>=200)
	     *n=2;
	   else   
	     *n=3;          
      }
      break;
    
    case 3:
      if(d.character>=400 && d.etchics>50)
      {
	   *m=5;
	   if(d.etchics>=300)
	     *n=1;
	   else if(d.etchics<300 && d.etchics>=150)
	     *n=2;
	   else   
	     *n=3;      
      }
      else if(d.character<400 && d.etchics>50)
      {
	   *m=4;
	   if(d.speech>=200)
	     *n=1;
	   else if(d.speech<150 && d.speech>=80)
	     *n=2;
	   else   
	     *n=3;      
      }
      else
      {
	   *m=7;
	   if(d.hp>=400)
	     *n=1;
	   else if(d.hp<400 && d.hp>=200)
	     *n=2;
	   else   
	     *n=3;          
      }
      break;    
    
    case 4:
      if(d.etchics>=50)
      {
	   *m=6;
      }
      else
      {
	   *m=8;
      }
      if(d.hskill>=100)
        *n=1;
      else if(d.hskill<100 && d.hskill>=80)
        *n=2;
      else   
        *n=3;       
      break;
 }

 if(d.sex==1)
   strcpy(buf, endmodecombat[*m].boy);
 else
   strcpy(buf, endmodecombat[*m].girl);
 *grade=endmodecombat[*m].grade;
 return 0;
}


int
pip_endingfamily(buf,m,n,grade) /*家事*/
char *buf;
int *m,*n,*grade;
{
 *m=1;
 if(d.charm>=200)
   *n=1;
 else if(d.charm<200 && d.charm>100)
   *n=2;
 else 
   *n=3;
   
 if(d.sex==1)
   strcpy(buf, endmodefamily[*m].boy);
 else
   strcpy(buf, endmodefamily[*m].girl);
 *grade=endmodefamily[*m].grade;
 return 0;
}


int
pip_endingall_purpose(buf,m,n,grade,mode) /*萬能*/
char *buf;
int *m,*n,*grade;
int mode;
{
 int data;
 int class;
 int num=0;
 
 if(mode==1)
    data=d.hexp;
 else if(mode==2)
    data=d.mexp;
 else if(mode==3)
    data=d.social;
 else if(mode==4)
    data=d.family;
 if(class>1000) class=1;
 else if(class>800) class=2;
 else if(class>500) class=3;
 else if(class>300) class=4;
 else class=5;

 data=pip_max_worktime(&num);
 switch(class)
 {
   case 1:
	if(d.character>=1000)
	{
	   *m=1;
	   if(d.etchics>=900)
	     *n=1;
	   else if(d.etchics<900 && d.etchics>=600)
	     *n=2;
	   else   
	     *n=3;	
	}
	else
	{
	   *m=2;
	   if(d.etchics>=650)
	     *n=1;
	   else if(d.etchics<650 && d.etchics>=400)
	     *n=2;
	   else   
	     *n=3;	
	}
	break;
   
   case 2:
	if(d.belief > d.etchics && d.belief > d.wisdom)
	{
	   *m=3;
	   if(d.etchics>=500)
	     *n=1;
	   else if(d.etchics<500 && d.etchics>=250)
	     *n=2;
	   else   
	     *n=3;	
	}
	else if(d.etchics > d.belief && d.etchics > d.wisdom)
	{
	   *m=4;
	   if(d.wisdom>=800)
	     *n=1;
	   else if(d.wisdom<800 && d.wisdom>=600)
	     *n=2;
	   else   
	     *n=3;	
	}
	else
	{
	   *m=5;
	   if(d.affect>=800)
	     *n=1;
	   else if(d.affect<800 && d.affect>=400)
	     *n=2;
	   else   
	     *n=3;	
	}
	break;

   case 3:
	if(d.belief > d.etchics && d.belief > d.wisdom)
	{
	   *m=6;
	   if(d.belief>=400)
	     *n=1;
	   else if(d.belief<400 && d.belief>=150)
	     *n=2;
	   else   
	     *n=3;	
	}
	else if(d.etchics > d.belief && d.etchics > d.wisdom)
	{
	   *m=7;
	   if(d.wisdom>=700)
	     *n=1;
	   else if(d.wisdom<700 && d.wisdom>=400)
	     *n=2;
	   else   
	     *n=3;	
	}
	else
	{
	   *m=8;
	   if(d.affect>=800)
	     *n=1;
	   else if(d.affect<800 && d.affect>=400)
	     *n=2;
	   else   
	     *n=3;	
	}
	break;   

   case 4:
	if(num>=2)
	{
	   *m=8+num;
	   switch(num)
	   {
	   	case 2:
	   		if(d.love>100)	*n=1;
	   		else if(d.love>50) *n=2;
	   		else *n=3;
	   		break;
	   	case 3:
	   		if(d.homework>100) *n=1;
	   		else if(d.homework>50) *n=2;
	   		else *n=3;
	   		break;
	   	case 4:
	   		if(d.hp>600) *n=1;
	   		else if(d.hp>300) *n=2;
	   		else *n=3;
	   		break;
	   	case 5:
	   		if(d.cookskill>200) *n=1;
	   		else if(d.cookskill>100) *n=2;
	   		else *n=3;
	   		break;
	   	case 6:
	   		if((d.belief+d.etchics)>600) *n=1;
	   		else if((d.belief+d.etchics)>200) *n=2;
	   		else *n=3;
	   		break;
	   	case 7:
	   		if(d.speech>150) *n=1;
	   		else if(d.speech>50) *n=2;
	   		else *n=3;
	   		break;
	   	case 8:
	   		if((d.hp+d.wrist)>900) *n=1;
	   		else if((d.hp+d.wrist)>600) *n=2;
	   		else *n=3;
	   		break;
	   	case 9:
	   	case 11:
	   		if(d.art>250) *n=1;
	   		else if(d.art>100) *n=2;
	   		else *n=3;
	   		break;
	   	case 10:
	   		if(d.hskill>250) *n=1;
	   		else if(d.hskill>100) *n=2;
	   		else *n=3;
	   		break;
	   	case 12:
	   		if(d.belief>500) *n=1;
	   		else if(d.belief>200) *n=2;
	   		else *n=3;
	   		break;
	   	case 13:
	   		if(d.wisdom>500) *n=1;
	   		else if(d.wisdom>200) *n=2;
	   		else *n=3;
	   		break;	
	   	case 14:
	   	case 16:
	   		if(d.charm>1000) *n=1;
	   		else if(d.charm>500) *n=2;
	   		else *n=3;
	   		break;
	   	case 15:
	   		if(d.charm>700) *n=1;
	   		else if(d.charm>300) *n=2;
	   		else *n=3;
	   		break;	   	
	   }
	}
	else
	{
	   *m=9;
	   if(d.etchics > 400)
	     *n=1;
	   else if(d.etchics >200)
	     *n=2;
	   else
	     *n=3;
	}
	break;
   case 5:
	if(num>=2)
	{
	   *m=24+num;
	   switch(num)
	   {
	   	case 2:
	   	case 3:
	   		if(d.hp>400) *n=1;
	   		else if(d.hp>150) *n=2;
	   		else *n=3;
	   		break;
	   	case 4:
	   	case 10:
	   	case 11:
	   		if(d.hp>600) *n=1;
	   		else if(d.hp>300) *n=2;
	   		else *n=3;
	   		break;
	   	case 5:
	   		if(d.cookskill>150) *n=1;
	   		else if(d.cookskill>80) *n=2;
	   		else *n=3;
	   		break;
	   	case 6:
	   		if((d.belief+d.etchics)>600) *n=1;
	   		else if((d.belief+d.etchics)>200) *n=2;
	   		else *n=3;
	   		break;
	   	case 7:
	   		if(d.speech>150) *n=1;
	   		else if(d.speech>50) *n=2;
	   		else *n=3;
	   		break;
	   	case 8:
	   		if((d.hp+d.wrist)>700) *n=1;
	   		else if((d.hp+d.wrist)>300) *n=2;
	   		else *n=3;
	   		break;
	   	case 9:
	   		if(d.art>100) *n=1;
	   		else if(d.art>50) *n=2;
	   		else *n=3;
	   		break;
	   	case 12:
	   		if(d.hp>300) *n=1;
	   		else if(d.hp>150) *n=2;
	   		else *n=3;
	   		break;
	   	case 13:
	   		if(d.speech>100) *n=1;
	   		else if(d.speech>40) *n=2;
	   		else *n=3;
	   		break;	
	   	case 14:
	   	case 16:
	   		if(d.charm>1000) *n=1;
	   		else if(d.charm>500) *n=2;
	   		else *n=3;
	   		break;
	   	case 15:
	   		if(d.charm>700) *n=1;
	   		else if(d.charm>300) *n=2;
	   		else *n=3;
	   		break;	   	
	   }
	}
	else
	{
	   *m=25;
	   if(d.relation > 100)
	     *n=1;
	   else if(d.relation >50)
	     *n=2;
	   else
	     *n=3;
	}
	break; 
 } 

 if(d.sex==1)
   strcpy(buf, endmodeall_purpose[*m].boy);
 else
   strcpy(buf, endmodeall_purpose[*m].girl);
 *grade=endmodeall_purpose[*m].grade;
 return 0;
}

int
pip_endingart(buf,m,n,grade) /*藝術*/
char *buf;
int *m,*n,*grade;
{
 if(d.speech>=100)
 {
   *m=1;
   if(d.hp>=300 && d.affect>=350)
     *n=1;
   else if(d.hp<300 && d.affect<350 && d.hp>=250 && d.affect>=300)
     *n=2;
   else   
     *n=3;
 } 
 else if(d.wisdom>=400)
 {
   *m=2;
   if(d.affect>=500)
     *n=1;
   else if(d.affect<500 && d.affect>=450)
     *n=2;
   else   
     *n=3;
 }
 else if(d.classI>=d.classJ)
 {
   *m=3;
   if(d.affect>=350)
     *n=1;
   else if(d.affect<350 && d.affect>=300)
     *n=2;
   else   
     *n=3;
 }
 else 
 {
   *m=4;
   if(d.affect>=200 && d.hp>150)
     *n=1;
   else if(d.affect<200 && d.affect>=180 && d.hp>150)
     *n=2;
   else   
     *n=3;
 }
 if(d.sex==1)
   strcpy(buf, endmodeart[*m].boy);
 else
   strcpy(buf, endmodeart[*m].girl);
 *grade=endmodeart[*m].grade;  
 return 0;
}

int
pip_max_worktime(num)
int *num;
{
  int data=20;
  if(d.workA>data)
  {
     data=d.workA;
     *num=1;
  }
  if(d.workB>data)
  {
     data=d.workB;
     *num=2;
  }
  if(d.workC>data)
  {
     data=d.workC;
     *num=3;
  }
  if(d.workD>data)
  {
     data=d.workD;
     *num=4;
  }
  if(d.workE>data)
  {
     data=d.workE;
     *num=5;
  }

  if(d.workF>data)
  {
     data=d.workF;
     *num=6;
  }
  if(d.workG>data)
  {
     data=d.workG;
     *num=7;
  }
  if(d.workH>data)
  {
     data=d.workH;
     *num=8;
  }
  if(d.workI>data)
  {
     data=d.workI;
     *num=9;
  }
  if(d.workJ>data)
  {
     data=d.workJ;
     *num=10;
  }
  if(d.workK>data)
  {
     data=d.workK;
     *num=11;
  }
  if(d.workL>data)
  {
     data=d.workL;
     *num=12;
  }
  if(d.workM>data)
  {
     data=d.workM;
     *num=13;
  }
  if(d.workN>data)
  {
     data=d.workN;
     *num=14;
  }
  if(d.workO>data)
  {
     data=d.workO;
     *num=16;
  }
  if(d.workP>data)
  {
     data=d.workP;
     *num=16;
  }  

  return data;
}

int pip_game_over(endgrade)
int endgrade;
{
	long gradebasic;
	long gradeall;
	
	gradebasic=(d.maxhp+d.wrist+d.wisdom+d.character+d.charm+d.etchics+d.belief+d.affect)/10-d.offense;
	clrchyiuan(1,23);
	gradeall=gradebasic+endgrade;
	move(8,17);
	prints("[1;36m感謝您玩完整個星空小雞的遊戲.....[0m");
	move(10,17);
	prints("[1;37m經過系統計算的結果：[0m");
	move(12,17);
	prints("[1;36m您的小雞 [37m%s [36m總得分＝ [1;5;33m%d [0m",d.name,gradeall);
	return gradeall;
}

int pip_divine() /*占卜師來訪*/
{
  char buf[256];
  char ans[4];
  char endbuf1[50];
  char endbuf2[50];
  char endbuf3[50];
  int endgrade=0;
  int endmode=0;
  long money;
  int tm;
  int randvalue;
  
  tm=d.bbtime/60/30;
  move(b_lines-2,0);
  money=300*(tm+1);
  clrchyiuan(0,24);
  move(10,14);
  prints("[1;33;5m叩叩叩...[0;1;37m突然傳來陣陣的敲門聲.........[0m");
  pressanykey("去瞧瞧是誰吧......");
  clrchyiuan(0,24);
  move(10,14);
  prints("[1;37;46m    原來是雲遊四海的占卜師來訪了.......    [0m");
  pressanykey("開門讓他進來吧....");
  if(d.money>=money)
  {
    randvalue=rand()%5;
    sprintf(buf,"你要占卜嗎? 要花%d元喔...[Y/n]",money);
    getdata(12,14,buf, ans, 2, 1, 0);  
    if(ans[0]!='N' && ans[0]!='n')
    {
      pip_ending_decide(endbuf1,endbuf2,endbuf3,&endmode,&endgrade);
      if(randvalue==0)
      		sprintf(buf,"[1;37m  你的小雞%s以後可能的身份是%s  [0m",d.name,endmodemagic[2+rand()%5].girl);
      else if(randvalue==1)
      		sprintf(buf,"[1;37m  你的小雞%s以後可能的身份是%s  [0m",d.name,endmodecombat[2+rand()%6].girl);
      else if(randvalue==2)
      		sprintf(buf,"[1;37m  你的小雞%s以後可能的身份是%s  [0m",d.name,endmodeall_purpose[6+rand()%15].girl);
      else if(randvalue==3)
      		sprintf(buf,"[1;37m  你的小雞%s以後可能的身份是%s  [0m",d.name,endmodeart[2+rand()%6].girl);
      else if(randvalue==4)
      		sprintf(buf,"[1;37m  你的小雞%s以後可能的身份是%s  [0m",d.name,endbuf1);
      d.money-=money;
      clrchyiuan(0,24);
      move(10,14);
      prints("[1;33m在我占卜結果看來....[0m");
      move(12,14);
      prints(buf);
      pressanykey("謝謝惠顧，有緣再見面了.(不準不能怪我喔)");
    }
    else
    {
      pressanykey("你不想占卜啊?..真可惜..那只有等下次吧...");
    }
  }
  else
  {
    pressanykey("你的錢不夠喔..真是可惜..等下次吧...");
  }
  return 0;
}

void
pip_money()
{
  char buf[100],ans[10];
  int money = -1;

  pip_read_file(cuser.userid);
  if(!d.name[0] || d.death) return;
  move(12,0);
  clrtobot();
  prints("你身上有 %d 元,雞金 %d 元\n",cuser.silvermoney,d.money);
  outs("\n十元換一雞金唷!!\n");
  while(money < 0 || money > cuser.silvermoney)
  {
    getdata(18,0,"要換多少錢? ",ans,10,LCECHO,0);
    if(!ans[0]) return;
    money = atol(ans);
  }
  sprintf(buf,"是否要轉換 %d 元 為 %d 雞金? [y/N]",money,money/10);
  getdata(19,0,buf,ans,3,LCECHO,0);
  if(ans[0] == 'y')
  {
    demoney(money);
    d.money += (money/10);
    pip_write_file();
    sprintf(buf,"你身上有 %d 元,雞金 %d 元",cuser.silvermoney,d.money);
  }
  else
    sprintf(buf,"取消.....");

  pressanykey(buf);
}
                
