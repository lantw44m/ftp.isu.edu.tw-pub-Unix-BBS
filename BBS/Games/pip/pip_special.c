/*---------------------------------------------------------------------------*/
/* �S����:�ݯf ��� �԰� ���X �¨�                                         */
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
"T",	"���X��H",	  0,	0,	 0,	  0,NULL,NULL,
"A",	"�P���M�L�s",	  1,	10,	15,    	100,"�A�u�n�A�ӳ��ڲ��..","�u�ìP�Ū��w���O�ܨ��W��..",
"B",	"�P�Ţ�����",	  1,   100,	25,	200,"�u�O§�����p��..�ڳ��w...","�S�ȴN�O���K�O�@�����w�����H..",
"C",	"���j�N�x",	  1,   200,	30,	250,"�i�D�A��I��~���ӾԧЫܺ�m��..","�A�u�O���Q�u�����p��...",
"D",	"�ѿ��`�Ȫ�",	  1,   300,	35,	300,"���������޲z�o�Ӱ�a��..","�A���n���ܦnť�C..�ګܳ��w��...:)",
"E",	"�p�Ѩϯ���",	  1,   400,	35,	300,"�A�ܦ��оi��I�ܰ����{�ѧA...","�u�����A�A���������A���....",
"F",	"����⯸��",	  1,   500,	40,	350,"�A�n�i�R��..�ڳ��w�A��....","���..�H��n�h�h�өM�ڪ���...",
"G",	"�Ĥp�į���",	  1,   550,	40,	350,"��A���ܫܧֳּ�..���������@�˵L��..","�ӡA���ڽ��\\�W�Ať�����G��..",
"H",	"�p�̧J����",	  1,   600,	50,     400,"�@�������d�����j�r..:)..","���§Ať������..�H��n�h�ӳ�...",
"I",	"�P������s",	  2,    60,	 0,	  0,"������..�Z���F����..�ܥi�R....","��  �ڭ̤@�_������a....",
"J",	"�C�~�ӪZ�x",	  0,	 0,	 0,	  0,"�A�n�A�ڬO�Z�x�A��q�Ȫe��Ҧ^�ӥ�..","�Ʊ�U���ٯਣ��A...:)",
NULL,		NULL,NULL,    NULL,    NULL,NULL,NULL
};

int pip_see_doctor()	/*�����*/
{
    char buf[256];
    long savemoney;
    savemoney=d.sick*25;
    if(d.sick<=0)
    {
    pressanykey("�z��..�S�f����|�F��..�Q�|�F..��~~");
    d.character-=(rand()%3+1);
    if(d.character<0)
      d.character=0;
    d.happy-=(rand()%3+3);
    d.satisfy-=rand()%3+2;
    }    
    else if(d.money < savemoney)
    {
     sprintf(buf,"�A���f�n�� %d ����....�A��������...",savemoney);    
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
    pressanykey("�Ĩ�f��..�S���Ƨ@��!!");
    }
    return 0;
}

/*���*/
int pip_change_weight()
{
    char genbuf[5];
    char inbuf[256];
    int weightmp;
    
    move(b_lines-1, 0);
    clrtoeol();
    show_special_pic(2);
#ifdef MAPLE
    getdata(b_lines-1,1, "�A����ܬO? [Q]���}:", genbuf, 2, 1, 0);    
#else
    getdata(b_lines-1,1, "�A����ܬO? [Q]���}:", genbuf, 2, DOECHO, YEA);
#endif  // END MAPLE
    if (genbuf[0]=='1'|| genbuf[0]=='2'|| genbuf[0]=='3'|| genbuf[0]=='4')
    { 
      switch(genbuf[0])
      {
        case '1':
          if(d.money<80)
          {
            pressanykey("�ǲμW�D�n80����....�A��������...");
          }
          else
          {
#ifdef MAPLE
            getdata(b_lines-1,1, "�ݪ�O80��(3��5����)�A�A�T�w��? [y/N]", genbuf, 2, 1, 0);
#else
            getdata(b_lines-1,1, "�ݪ�O80��(3��5����)�A�A�T�w��? [y/N]", genbuf, 2, DOECHO, YEA);
#endif  // END MAPLE
            if(genbuf[0]=='Y' || genbuf[0]=='y')
            {
              weightmp=3+rand()%3;
              d.weight+=weightmp;
              d.money-=80;
              d.maxhp-=rand()%2;
              d.hp-=rand()%2+3;
              show_special_pic(3);
              sprintf(inbuf, "�`�@�W�[�F%d����",weightmp);
              pressanykey(inbuf);
            }
            else
            {
              pressanykey("�^����N�o.....");
            }
          }
          break;
          
        case '2':
#ifdef MAPLE
          getdata(b_lines-1,1, "�W�@����n30���A�A�n�W�h�֤���O? [�ж�Ʀr]:", genbuf, 4, 1, 0);
#else
          getdata(b_lines-1,1, "�W�@����n30���A�A�n�W�h�֤���O? [�ж�Ʀr]:", genbuf, 4, DOECHO, YEA);
#endif  // END MAPLE
          weightmp=atoi(genbuf);
          if(weightmp<=0)
          {
            pressanykey("��J���~..����o...");          
          }
          else if(d.money>(weightmp*30))
          {
            sprintf(inbuf, "�W�[%d����A�`�@�ݪ�O�F%d���A�T�w��? [y/N]",weightmp,weightmp*30);
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
                sprintf(inbuf, "�`�@�W�[�F%d����",weightmp);
                pressanykey(inbuf);
            }
            else
            {
              pressanykey("�^����N�o.....");
            }
          }
          else
          {
            pressanykey("�A���S����h��.......");            
          }
          break;        
          
        case '3':
          if(d.money<80)
          {
            pressanykey("�ǲδ�έn80����....�A��������...");
          }
          else
          {
#ifdef MAPLE
            getdata(b_lines-1,1, "�ݪ�O80��(3��5����)�A�A�T�w��? [y/N]", genbuf, 2, 1, 0);
#else
            getdata(b_lines-1,1, "�ݪ�O80��(3��5����)�A�A�T�w��? [y/N]", genbuf, 2, DOECHO, YEA);
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
              sprintf(inbuf, "�`�@��֤F%d����",weightmp);
              pressanykey(inbuf);
            }
            else
            {
              pressanykey("�^����N�o.....");
            }
          }        
          break;
        case '4':
#ifdef MAPLE
          getdata(b_lines-1,1, "��@����n30���A�A�n��h�֤���O? [�ж�Ʀr]:", genbuf, 4, 1, 0);
#else
          getdata(b_lines-1,1, "��@����n30���A�A�n��h�֤���O? [�ж�Ʀr]:", genbuf, 4, DOECHO, YEA);
#endif  // END MAPLE
          weightmp=atoi(genbuf);
          if(weightmp<=0)
          {
            pressanykey("��J���~..����o...");
          }          
          else if(d.weight<=weightmp)
          {
            pressanykey("�A�S���򭫳�.....");
          }
          else if(d.money>(weightmp*30))
          {
            sprintf(inbuf, "���%d����A�`�@�ݪ�O�F%d���A�T�w��? [y/N]",weightmp,weightmp*30);
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
                sprintf(inbuf, "�`�@��֤F%d����",weightmp);
                pressanykey(inbuf);
            }
            else
            {
              pressanykey("�^����N�o.....");
            }
          }
          else
          {
            pressanykey("�A���S����h��.......");            
          }
          break;
      }
    }
    return 0;
}


/*�Ѩ�*/

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
  int prince;  /*���l�|���|�X�{*/
  int pipkey;
  int change;
  char buf[256];
  char inbuf1[20];
  char inbuf2[20];
  char ans[5];
  char *needmode[3]={"      ","§����{��","�ͦR�ޥ���"};
  int save[11]={0,0,0,0,0,0,0,0,0,0,0};

  d.nodone=0; 
  do
  {
  clear();
  show_palace_pic(0);
  move(13,4);
  sprintf(buf,"[1;31m�z�w�w�w�w�w�w�t[37;41m �Ө��`�q�O���F  �п�ܧA�����X����H [0;1;31m�u�w�w�w�w�w�w�{[0m");
  prints(buf);
  move(14,4);
  sprintf(buf,"[1;31m�x                                                                  �x[0m");
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
      sprintf(buf,"[1;31m�x [36m([37m%s[36m) [33m%-10s  [37m%-14s     [36m([37m%s[36m) [33m%-10s  [37m%-14s[31m�x[0m",    
             p[a].num,p[a].name,inbuf1,p[b].num,p[b].name,inbuf2);
    else
      sprintf(buf,"[1;31m�x [36m([37m%s[36m) [33m%-10s  [37m%-14s                                   [31m�x[0m",    
             p[a].num,p[a].name,inbuf1);             
    prints(buf);
  }
  move(20,4);
  sprintf(buf,"[1;31m�x                                                                  �x[0m");
  prints(buf);
  move(21,4);
  sprintf(buf,"[1;31m�|�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�}[0m");
  prints(buf);
  

   if(d.death==1 || d.death==2 || d.death==3)
     return 0;    
  /*�N�U�H�Ȥw�g���P���ƭȥs�^��*/
   save[1]=d.royalA;            /*from�u��*/
   save[2]=d.royalB;            /*from���*/
   save[3]=d.royalC;		/*from�N�x*/
   save[4]=d.royalD;            /*from�j��*/
   save[5]=d.royalE;            /*from���q*/
   save[6]=d.royalF;            /*from�d�m*/
   save[7]=d.royalG;            /*from���m*/
   save[8]=d.royalH;            /*from���*/
   save[9]=d.royalI;            /*from�p��*/
   save[10]=d.royalJ;           /*from���l*/

   move(b_lines-1, 0);
   clrtoeol();
   move(b_lines-1,0);
   prints("[1;33m [�ͩR�O] %d/%d  [�h�ҫ�] %d [0m",d.hp,d.maxhp,d.tired);
             
   move(b_lines, 0);
   clrtoeol();
   move(b_lines,0);
   prints(
   "[1;37;46m  �Ѩ����  [44m [�r��]��ܱ����X���H��  [Q]���}�P���`�q�O���G                    [0m");
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
       pipdie("[1;31m�֦��F...[m  ",1);
    }
    if(d.hp<0)
    {
       d.death=1;
       pipdie("[1;31m�j���F...[m  ",1);
    }
    if(d.death==1)
    {
      sprintf(buf,"�T�T�F...�u�O�d��..");
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
            sprintf(buf,"��M�o�򰶤j���A���ܯu�O�a����...");
        else
            sprintf(buf,"�ܰ����A�ӫ��X�ڡA���ڤ��൹�A����F..");
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
          /*�p�G�j��C�����W�[�̤j�q*/
          if(change > p[choice].addtoman)
             change=p[choice].addtoman;
          /*�p�G�[�W���������j��ү൹���Ҧ��Ȯ�*/
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
            sprintf(buf,"�ڤ��M�p�o�˪����͸�....");
      else
            sprintf(buf,"�A�o���S�оi�����A�A�h�Ǿ�§���a....");    
    
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

  pressanykey("���}�P���`�q�O��.....");  
  return 0;
}

int pip_query()  /*���X�p��*/
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

  stand_title("���X�P��");
  usercomplete(msg_uid, genbuf);
  if (genbuf[0])
  {
    move(2, 0);
    if (id = getuser(genbuf))
    {
        pip_read(genbuf);
        pressanykey("�[���@�U�O�H���p��...:p");
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
  /*char yo[14][5]={"�ϥ�","����","����","�ൣ","�C�~","�֦~","���~",
                  "���~","���~","���~","��~","�Ѧ~","�Ѧ~","�j�}"};*/
  char yo[12][5]={"�ϥ�","����","����","�ൣ","�֦~","�C�~",
                  "���~","���~","��~","�Ѧ~","�j�}","���P"};                  
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
  
    if(age==0) /*�ϥ�*/
       age1=0;
    else if( age==1) /*����*/
       age1=1;
    else if( age>=2 && age<=5 ) /*����*/
       age1=2;
    else if( age>=6 && age<=12 ) /*�ൣ*/
       age1=3;
    else if( age>=13 && age<=15 ) /*�֦~*/
       age1=4;     
    else if( age>=16 && age<=18 ) /*�C�~*/
       age1=5;     
    else if( age>=19 && age<=35 ) /*���~*/
       age1=6;
    else if( age>=36 && age<=45 ) /*���~*/
       age1=7;
    else if( age>=45 && age<=60 ) /*��~*/
       age1=8;
    else if( age>=60 && age<=70 ) /*�Ѧ~*/
       age1=9;
    else if( age>=70 && age<=100 ) /*�j�}*/
       age1=10;
    else if( age>100 ) /*���P*/
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
  prints("�o�O%s�i���p���G\n",xuser.userid);
#else
  prints("�o�O%s�i���p���G\n",genbuf);
#endif  // END MAPLE

  if (death1==0)
  {
   prints("[1;32mName�G%-10s[m  �ͤ�G%2d�~%2d��%2d��   �~�֡G%2d��  ���A�G%s  �����G%d\n"
          "�ͩR�G%3d/%-3d  �ּ֡G%-4d  ���N�G%-4d  ���G%-4d  ���z�G%-4d  �魫�G%-4d\n"
          "�j�ɤY�G%-4d   �����G%-4d  �s���G%-4d  �h�ҡG%-4d  żż�G%-4d  �f��G%-4d\n",
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
   if(shit1==0) prints("�ܰ��b..");
   if(shit1>40&&shit1<60) prints("��䪺..");
   if(shit1>=60&&shit1<80) prints("�n���..");
   if(shit1>=80&&shit1<100) prints("[1;34m�֯䦺�F..[m");
   if(shit1>=100) {prints("[1;31m�䦺�F..[m"); return -1;}

   pc1=hp1*100/maxhp1;
   if(pc1==0) {prints("�j���F.."); return -1;}
   if(pc1<20) prints("[1;35m�����L�O��.�־j���F.[m");
   if(pc1<40&&pc1>=20) prints("��O���Ӱ�..�Q�Y�I�F��..");
   if(pc1<100&&pc1>=80) prints("���{�l��������O..");
   if(pc1>=100) prints("[1;34m�ּ����F..[m");

   pc1=tired1;
   if(pc1<20) prints("�믫�ݧݤ�..");
   if(pc1<80&&pc1>=60) prints("[1;34m���I�p��..[m");
   if(pc1<100&&pc1>=80) {prints("[1;31m�n�ֳ�A�֤���F..[m"); }
   if(pc1>=100) {prints("�֦��F..."); return -1;}

   pc1=60+10*age;
   if(weight1<(pc1+30) && weight1>=(pc1+10)) prints("���I�p�D..");
   if(weight1<(pc1+50) && weight1>=(pc1+30)) prints("�ӭD�F..");
   if(weight1>(pc1+50)) {prints("�D���F..."); return -1;}

   if(weight1<(pc1-50)) {prints("�G���F.."); return -1;}
   if(weight1>(pc1-30) && weight1<=(pc1-10)) prints("���I�p�G..");
   if(weight1>(pc1-50) && weight1<=(pc1-30)) prints("�ӽG�F..");

   if(sick1<75&&sick1>=50) prints("[1;34m�ͯf�F..[m");
   if(sick1<100&&sick1>=75) {prints("[1;31m�f��!!..[m"); }
   if(sick1>=100) {prints("�f���F.!."); return -1;}

   pc1=happy1;
   if(pc1<20) prints("[1;31m�ܤ��ּ�..[m");
   if(pc1<40&&pc1>=20) prints("���ּ�..");
   if(pc1<95&&pc1>=80) prints("�ּ�..");
   if(pc1<=100&&pc1>=95) prints("�ܧּ�..");

   pc1=satisfy1;
   if(pc1<40) prints("[31;1m������..[m");
   if(pc1<95&&pc1>=80) prints("����..");
   if(pc1<=100&&pc1>=95) prints("�ܺ���..");
  }
  else if(death1==1)
  {
     show_die_pic(2);
     move(14,20);
     prints("�i�����p����I�s�v�F");
  } 
  else if(death1==2)
  {
     show_die_pic(3);
  }
  else if(death1==3)
  {
    move(5,0);
    outs("�C���w�g���쵲���o....");
  }
  else
  {
    pressanykey("�ɮ׷l���F....");
  }
 }   /* ���i�p�� */
 else
 {
   move(1,0);
   clrtobot();
   pressanykey("�o�@�a���H�S���i�p��......");
 }
}
