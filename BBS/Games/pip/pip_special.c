/*---------------------------------------------------------------------------*/
/* 特殊選單:看病 減肥 戰鬥 拜訪 朝見                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include <time.h>
#include "bbs.h"
#include "pip.h"
extern struct chicken d;
extern time_t start_time;
extern time_t lasttime;
#define getdata(a, b, c , d, e, f, g) getdata(a,b,c,d,e,f,NULL,g)

struct royalset royallist[] = {
"T",	"拜訪對象",	  0,	0,	 0,	  0,NULL,NULL,
"A",	"星空騎兵連",	  1,	10,	15,    	100,"你真好，來陪我聊天..","守衛星空的安全是很辛苦的..",
"B",	"星空００７",	  1,   100,	25,	200,"真是禮貌的小雞..我喜歡...","特務就是秘密保護站長安全的人..",
"C",	"鎮國大將軍",	  1,   200,	30,	250,"告訴你唷！當年那個戰役很精彩喔..","你真是高貴優雅的小雞...",
"D",	"參謀總務長",	  1,   300,	35,	300,"我幫站長管理這個國家唷..","你的聲音很好聽耶..我很喜歡喔...:)",
"E",	"小天使站長",	  1,   400,	35,	300,"你很有教養唷！很高興認識你...","優雅的你，請讓我幫你祈福....",
"F",	"風箏手站長",	  1,   500,	40,	350,"你好可愛喔..我喜歡你唷....","對啦..以後要多多來和我玩喔...",
"G",	"乖小孩站長",	  1,   550,	40,	350,"跟你講話很快樂喔..不像站長一樣無聊..","來，坐我膝蓋\上，聽我講故事..",
"H",	"小米克站長",	  1,   600,	50,     400,"一站之長責任重大呀..:)..","謝謝你聽我講話..以後要多來喔...",
"I",	"星空灌水群",	  2,    60,	 0,	  0,"不錯唷..蠻機靈的喔..很可愛....","來  我們一起來灌水吧....",
"J",	"青年帥武官",	  0,	 0,	 0,	  0,"你好，我是武官，剛從銀河邊境回來休息..","希望下次還能見到你...:)",
NULL,		NULL,NULL,    NULL,    NULL,NULL,NULL
};

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
#ifdef MAPLE
    getdata(b_lines-1,1, "你的選擇是? [Q]離開:", genbuf, 2, 1, 0);    
#else
    getdata(b_lines-1,1, "你的選擇是? [Q]離開:", genbuf, 2, DOECHO, YEA);
#endif  // END MAPLE
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
#ifdef MAPLE
            getdata(b_lines-1,1, "需花費80元(3∼5公斤)，你確定嗎? [y/N]", genbuf, 2, 1, 0);
#else
            getdata(b_lines-1,1, "需花費80元(3∼5公斤)，你確定嗎? [y/N]", genbuf, 2, DOECHO, YEA);
#endif  // END MAPLE
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
#ifdef MAPLE
          getdata(b_lines-1,1, "增一公斤要30元，你要增多少公斤呢? [請填數字]:", genbuf, 4, 1, 0);
#else
          getdata(b_lines-1,1, "增一公斤要30元，你要增多少公斤呢? [請填數字]:", genbuf, 4, DOECHO, YEA);
#endif  // END MAPLE
          weightmp=atoi(genbuf);
          if(weightmp<=0)
          {
            pressanykey("輸入有誤..放棄囉...");          
          }
          else if(d.money>(weightmp*30))
          {
            sprintf(inbuf, "增加%d公斤，總共需花費了%d元，確定嗎? [y/N]",weightmp,weightmp*30);
#ifdef MAPLE
            getdata(b_lines-1,1,inbuf, genbuf, 2, 1, 0);
#else
            getdata(b_lines-1,1,inbuf, genbuf, 2, DOECHO, YEA);
#endif  // END MAPLE
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
#ifdef MAPLE
            getdata(b_lines-1,1, "需花費80元(3∼5公斤)，你確定嗎? [y/N]", genbuf, 2, 1, 0);
#else
            getdata(b_lines-1,1, "需花費80元(3∼5公斤)，你確定嗎? [y/N]", genbuf, 2, DOECHO, YEA);
#endif  // END MAPLE
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
#ifdef MAPLE
          getdata(b_lines-1,1, "減一公斤要30元，你要減多少公斤呢? [請填數字]:", genbuf, 4, 1, 0);
#else
          getdata(b_lines-1,1, "減一公斤要30元，你要減多少公斤呢? [請填數字]:", genbuf, 4, DOECHO, YEA);
#endif  // END MAPLE
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
#ifdef MAPLE
            getdata(b_lines-1,1,inbuf, genbuf, 2, 1, 0);
#else
            getdata(b_lines-1,1,inbuf, genbuf, 2, DOECHO, YEA);
#endif  // END MAPLE
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
  int prince;  /*王子會不會出現*/
  int pipkey;
  int change;
  char buf[256];
  char inbuf1[20];
  char inbuf2[20];
  char ans[5];
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

#ifdef MAPLE
  userec muser;
#endif  // END MAPLE
  int id;
  char genbuf[STRLEN];

#ifndef MAPLE
  char *msg_uid = MSG_UID;
  char *err_uid = ERR_UID;
#endif  // END MAPLE

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
char *genbuf;
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

#ifdef MAPLE
  sprintf(buf,"home/%s/new_chicken",genbuf);
  currutmp->destuid = genbuf;
#else
  sprintf(buf,"home/%c/%s/new_chicken",toupper(genbuf[0]),genbuf);
#endif  // END MAPLE

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
#ifdef MAPLE
  prints("這是%s養的小雞：\n",xuser.userid);
#else
  prints("這是%s養的小雞：\n",genbuf);
#endif  // END MAPLE

  if (death1==0)
  {
   prints("[1;32mName：%-10s[m  生日：%2d年%2d月%2d日   年齡：%2d歲  狀態：%s  錢錢：%d\n"
          "生命：%3d/%-3d  快樂：%-4d  滿意：%-4d  氣質：%-4d  智慧：%-4d  體重：%-4d\n"
          "大補丸：%-4d   食物：%-4d  零食：%-4d  疲勞：%-4d  髒髒：%-4d  病氣：%-4d\n",
        name1,year1,month1,day1,age,yo[age1],money1,
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
