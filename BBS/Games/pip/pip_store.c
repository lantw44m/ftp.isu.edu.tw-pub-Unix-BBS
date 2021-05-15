/*---------------------------------------------------------------------------*/
/* �ө����:���� �s�� �j�ɤY ���� �ѥ�                                       */
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
/* �ө����:���� �s�� �j�ɤY ���� �ѥ�                                       */
/* ��Ʈw                                                                    */
/*---------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*  ���~�ѼƳ]�w                                                            */
/*--------------------------------------------------------------------------*/
struct goodsofpip
{
   int num;		/*�s��*/
   char *name;		/*�W�r*/
   char *msgbuy;	/*�\��*/
   char *msguse;	/*����*/
   int money;		/*����*/
   int change;		/*���ܶq*/
   int pic1;
   int pic2;
};
typedef struct goodsofpip goodsofpip;

struct goodsofpip pipfoodlist[] = {
0,"���~�W",	"����buy",	"����feed",			0,	0,	0,0,
1,"�n�Y������",	"��O��_50",	"�C�Y�@�������|��_��O50��!",	50,	50,	1,1,
2,"�������s��",	"��O��_100",	"���F��_��O�A�p���]�|��ּ�",	120,	100,	2,3,
0,NULL,NULL,NULL,0,0,0,0
};

struct goodsofpip pipmedicinelist[] = {
0,"���~�W",	"����buy",	"����feed",			0,	0,	0,0,
1,"�n�Τj�ɤY",	"��O��_600",	"��_�j�q�y����O���}��",	500,	600,	4,4,
2,"�öQ���F��",	"�k�O��_50",	"�C�Y�@���F�۷|��_�k�O50��!",	100,	50,	7,7,
3,"�d�~�H�Ѥ�",	"�k�O��_500",	"��_�j�q�y���k�O���}��",	800,	500,	7,7,
4,"�Ѥs����",	"�k�O��O�̤j",	"�o��  �n�Q......",		10000,	0,	7,7,
0,NULL,NULL,NULL,0,0,0,0
};

struct goodsofpip pipotherlist[] = {
0,"���~�W",	"����buy",	"����feed",			0,	0,	0,0,
1,"�ְ������",	"�ֺּ��N��",	"�������p����ְּ�...",	50,	0,	5,5,
2,"�ʬ����",	"���Ѫ��ӷ�",	"�ѥ����p�����o���󦳮���...",100,	0,	6,6,
0,NULL,NULL,NULL,0,0,0,0
};

/*--------------------------------------------------------------------------*/
/*  �Z���ѼƳ]�w                                                            */
/*--------------------------------------------------------------------------*/
struct weapon
{
  char *name;           /*�W�r*/  
  int needmaxhp;	/*�ݭnhp*/
  int needmaxmp;	/*�ݭnmp*/
  int needspeed;	/*�ݭn��speed*/
  int attack;		/*����*/
  int resist;		/*���@*/
  int speed;		/*�t��*/
  int cost;		/*�R��*/
  int sell;		/*���*/
  int special;		/*�S�O*/
  int map;		/*����*/

};
typedef struct weapon weapon;

/*�W�r,��hp,��mp,��speed,����,���@,�t��,�R��,���,�S�O,����*/
struct weapon headlist[] = {
"���R�˳�",  0,  0,  0,  0,  0,  0,     0,     0,0,0,
"�콦�U�l",  0,  0,  0,  0,  5,  0,   500,   300,0,0,	
"���֤p�U",  0,  0,  0,  0, 10,  0,  3500,  1000,0,0,
"���w���U", 60,  0,  0,  0, 20,  0,  5000,  3500,0,0,
"���K�Y��",150, 50,  0,  0, 30,  0, 10000,  6000,0,0,
"�]�k�v�T",100,150,  0,  0, 25,  0, 50000, 10000,0,0, 
"�����t��",300,300,300,  0,100,  0,300000,100000,0,0,
NULL,        0,  0,  0,  0,  0,  0,   0,   0,0,0
};

/*�W�r,��hp,��mp,��speed,����,���@,�t��,�R��,���,�S�O,����*/
struct weapon rhandlist[] = {
"���R�˳�",  0,  0,  0,  0,  0,  0,     0,     0,0,0,
"�j���",    0,  0,  0,  5,  0,  0,  1000,   700,0,0,	
"���ݧ��",  0,  0,  0, 10,  0,  0,  2500,  1000,0,0,
"�C�ɼC",   50,  0,  0, 20,  0,  0,  6000,  4000,0,0,
"���p�C",   80,  0,  0, 30,  0,  0, 10000,  8000,0,0,
"���l�M",  100, 20,  0, 40,  0,  0, 15000, 10000,0,0, 
"�ѱ��C",  100, 40,  0, 35, 20,  0, 15000, 10000,0,0,
"���Y�_�M",150,  0,  0, 60,  0,  0, 35000, 20000,0,0,
"�O�s�M",  200,  0,  0,100,  0,  0, 50000, 25000,0,0,
"�����t��",300,300,300,100, 20,  0,150000,100000,0,0,
NULL,        0,  0,  0,  0,  0,  0,    0,   0,0,0
};

/*�W�r,��hp,��mp,��speed,����,���@,�t��,�R��,���,�S�O,����*/
struct weapon lhandlist[] = {
"���R�˳�",  0,  0,  0,  0,  0,  0,     0,     0,0,0,
"�j���",    0,  0,  0,  5,  0,  0,  1000,   700,0,0,	
"���ݧ��",  0,  0,  0, 10,  0,  0,  1500,  1000,0,0,
"���",	     0,  0,  0,  0, 10,  0,  2000,  1500,0,0,
"���ÿ���", 60,  0,  0,  0, 25,  0,  5000,  3000,0,0,
"�ժ�����", 80,  0,  0, 10, 40,  0, 15000, 10000,0,0,
"�]�k��",   80,100,  0, 20, 60,  0, 80000, 50000,0,0,
"�����t��",300,300,300, 30,100,  0,150000,100000,0,0,
NULL,        0,  0,  0,  0,  0,  0,    0,   0,0,0
};

/*�W�r,��hp,��mp,��speed,����,���@,�t��,�R��,���,�S�O,����*/
struct weapon bodylist[] = {
"���R�˳�",  0,  0,  0,  0,  0,  0,     0,     0,0,0,
"�콦�`��", 40,  0,  0,  0,  5,  0,  1000,   700,0,0,	
"�S�ť֥�", 50,  0,  0,  0, 10,  0,  2500,  1000,0,0,
"���K����", 80,  0,  0,  0, 25,  0,  5000,  3500,0,0,
"�]�k�ܭ�", 80, 40,  0,  0, 20, 20, 15500, 10000,0,0,
"�ժ�����",100, 30,  0,  0, 40, 20, 30000, 20000,0,0, 
"�����t��",300,300,300, 30,100,  0,150000,100000,0,0,
NULL,        0,  0,  0,  0,  0,  0,     0,   0,0,0
};

/*�W�r,��hp,��mp,��speed,����,���@,�t��,�R��,���,�S�O,����*/
struct weapon footlist[] = {
"���R�˳�",  0,  0,  0,  0,  0,  0,     0,     0,0,0,
"�콦��c",  0,  0,  0,  0,  0, 10,   800,   500,0,0,
"�F�v��j",  0,  0,  0, 15,  0, 10,  1000,   700,0,0, 	
"�S�ūB�c",  0,  0,  0,  0, 10, 10,  1500,  1000,0,0,
"NIKE�B�ʾc",70, 0,  0,  0, 10, 40,  8000,  5000,0,0,
"�s���ֹu", 80, 20,  0, 10, 25, 20, 12000,  8000,0,0,
"�����]�u",100,100,  0, 30, 50, 60, 25000, 10000,0,0,
"�����t�u",300,300,300, 50,100,100,150000,100000,0,0,
NULL,        0,  0,  0,  0,  0,  0,    0,   0,0,0
};

/*---------------------------------------------------------------------------*/
/* �ө����:���� �s�� �j�ɤY ���� �ѥ�                                       */
/* �禡�w                                                                    */
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

int pip_store_weapon_head()	/*�Y���Z��*/
{
     d.weaponhead=pip_weapon_doing_menu(d.weaponhead,0,headlist);
     return 0; 
}
int pip_store_weapon_rhand()	/*�k��Z��*/
{
     d.weaponrhand=pip_weapon_doing_menu(d.weaponrhand,1,rhandlist);
     return 0;
}
int pip_store_weapon_lhand()    /*����Z��*/
{
     d.weaponlhand=pip_weapon_doing_menu(d.weaponlhand,2,lhandlist);
     return 0;
}
int pip_store_weapon_body()	/*����Z��*/
{
     d.weaponbody=pip_weapon_doing_menu(d.weaponbody,3,bodylist);
     return 0;
}
int pip_store_weapon_foot()     /*�����Z��*/
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
    char *shopname[4]={"���W","�K�Q�ө�","�P���ľQ","�]�̮ѧ�"};
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
	    sprintf(inbuf,"[1;31m  �w[41;37m �s�� [0;1;31m�w[41;37m ��      �~ [0;1;31m�w�w[41;37m ��            �� [0;1;31m�w�w[41;37m ��     �� [0;1;31m�w[37;41m �֦��ƶq [0;1;31m�w[0m  ");
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
	    sprintf(inbuf,"[1;44;37m  %8s���  [46m  [B]�R�J���~  [S]��X���~  [Q]���X�G                         [m",shopname[mode]);
	    prints(inbuf);
	    pipkey=egetch(); 
	    switch(pipkey)  
	    {
		case 'B':
		case 'b':      
			move(b_lines-1,1);
			sprintf(inbuf,"�Q�n�R�Jԣ�O? [0]���R�J [1��%d]���~�Ӹ�",oldnum[0]);
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
					sprintf(inbuf,"�A�n�R�J���~ [%s] �h�֭өO?(�W�� %d)",p[choice].name,d.money/p[choice].money);
#ifdef MAPLE
					getdata(b_lines-1,1,inbuf,genbuf,6, 1, 0);
#else
                                        getdata(b_lines-1,1,inbuf,genbuf,6, DOECHO, YEA);
#endif  // END MAPLE
					smoney=atoi(genbuf);
				}
				if(smoney<0)
				{
					pressanykey("���R�J...");
				}
				else if(d.money<smoney*p[choice].money)
				{
					pressanykey("�A�����S������h��..");
				}
				else
				{
					sprintf(inbuf,"�T�w�R�J���~ [%s] �ƶq %d �Ӷ�?(���a��� %d) [y/N]:",p[choice].name,smoney,smoney*p[choice].money);
#ifdef MAPLE
					getdata(b_lines-1,1,inbuf,genbuf, 2, 1, 0); 
#else
                                        getdata(b_lines-1,1,inbuf,genbuf, 2, DOECHO, YEA);
#endif  // END MAPLE
					if(genbuf[0]=='y' || genbuf[0]=='Y')
					{
						oldnum[choice]+=smoney;
						d.money-=smoney*p[choice].money;
						sprintf(inbuf,"���󵹤F�A%d��%s",smoney,p[choice].name);
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
						pressanykey("���R�J...");
					}
				}
			}
			else
			{
				sprintf(inbuf,"���R�J.....");
				pressanykey(inbuf);            
			}
			break;     
     
 		case 'S':
 		case 's':
 			if(mode==3)
 			{
 				pressanykey("�o�ǪF�褣����....");
 				break;
 			}
			move(b_lines-1,1);
			sprintf(inbuf,"�Q�n��Xԣ�O? [0]����X [1��%d]���~�Ӹ�",oldnum[0]);
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
				sprintf(inbuf,"�A�n��X���~ [%s] �h�֭өO?(�W�� %d)",p[choice].name,oldnum[choice]);
#ifdef MAPLE
				getdata(b_lines-1,1,inbuf,genbuf,6, 1, 0); 
#else
                                getdata(b_lines-1,1,inbuf,genbuf,6, DOECHO, YEA);
#endif  // END MAPLE
				smoney=atoi(genbuf);
				if(smoney<0)
				{
					pressanykey("����X...");
				}
				else if(smoney>oldnum[choice])
				{
					sprintf(inbuf,"�A�� [%s] �S������h�ӳ�",p[choice].name);
					pressanykey(inbuf);
				}
				else
				{
					sprintf(inbuf,"�T�w��X���~ [%s] �ƶq %d �Ӷ�?(���a�R�� %d) [y/N]:",p[choice].name,smoney,smoney*p[choice].money*8/10);
#ifdef MAPLE
					getdata(b_lines-1,1,inbuf,genbuf, 2, 1, 0);
#else
                                        getdata(b_lines-1,1,inbuf,genbuf, 2, DOECHO, YEA);
#endif  // END MAPLE
					if(genbuf[0]=='y' || genbuf[0]=='Y')
					{
						oldnum[choice]-=smoney;
						d.money+=smoney*p[choice].money*8/10;
						sprintf(inbuf,"���󮳨��F�A��%d��%s",smoney,p[choice].name);
						pressanykey(inbuf);
					}
					else
					{
						pressanykey("����X...");
					}
				}
			}
			else
			{
				sprintf(inbuf,"����X.....");
				pressanykey(inbuf);            
			}
			break;
		case 'Q':
		case 'q':
			sprintf(inbuf,"��������@ %d ��,���} %s ",d.money-oldmoney,shopname[mode]);
			pressanykey(inbuf);
			break;
#ifdef MAPLE
		case Ctrl('R'):
			if (currutmp->msgs[0].last_pid)
			{
				show_last_call_in();
				my_write(currutmp->msgs[0].last_pid, "���y��^�h�G");
			}
			break;
#endif  // END MAPLE
	    }
  }while((pipkey!='Q')&&(pipkey!='q')&&(pipkey!=KEY_LEFT));    
  return 0;
}

int
pip_weapon_doing_menu(variance,type,p)               /* �Z���ʶR�e�� */
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
  char menutitle[5][11]={"�Y���˳ư�","�k��˳ư�","����˳ư�","����˳ư�","�����˳ư�"};  
  int pipkey;
  char choicekey[5];
  int choice;
  
  do{
   clear();
   showtitle(menutitle[type], BoardName);
   show_weapon_pic(0);
/*   move(10,2); 
   sprintf(buf,"[1;37m�{����O:��OMax:[36m%-5d[37m  �k�OMax:[36m%-5d[37m  ����:[36m%-5d[37m  ���m:[36m%-5d[37m  �t��:[36m%-5d [m",
           d.maxhp,d.maxmp,d.attack,d.resist,d.speed);
   prints(buf);*/
   move(11,2);
   sprintf(buf,"[1;37;41m [NO]  [����W]  [��O]  [�k�O]  [�t��]  [����]  [���m]  [�t��]  [��  ��] [m");
   prints(buf);
   move(12,2);
   sprintf(buf," [1;31m�w�w[37m�զ� �i�H�ʶR[31m�w�w[32m��� �֦��˳�[31m�w�w[33m���� ��������[31m�w�w[35m���� ��O����[31m�w�w[m");
   prints(buf); 

   n=0;
   while (s = p[n].name)
   {   
     move(13+n,2);
     if(variance!=0 && variance==(n))/*��������*/
     {
      sprintf(buf, 
      "[1;32m (%2d)  %-10s %4d    %4d    %4d    %4d    %4d    %4d    %6d[m",     
      n,p[n].name,p[n].needmaxhp,p[n].needmaxmp,p[n].needspeed,
      p[n].attack,p[n].resist,p[n].speed,p[n].cost);        
     }
     else if(d.maxhp < p[n].needmaxhp || d.maxmp < p[n].needmaxmp || d.speed < p[n].needspeed )/*��O����*/
     {
      sprintf(buf, 
      "[1;35m (%2d)  %-10s %4d    %4d    %4d    %4d    %4d    %4d    %6d[m",
      n,p[n].name,p[n].needmaxhp,p[n].needmaxmp,p[n].needspeed,
      p[n].attack,p[n].resist,p[n].speed,p[n].cost);
     }

     else if(d.money < p[n].cost)  /*��������*/
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
   sprintf(buf,"[1;44;37m  �Z���ʶR���  [46m  [B]�ʶR�Z��  [S]�汼�˳�  [W]�ӤH���  [Q]���X�G            [m");
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
     sprintf(shortbuf,"�Q�n�ʶRԣ�O? �A������[%d]��:[�Ʀr]",d.money);
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
       if(choice==0)     /*�Ѱ�*/
       { 
          sprintf(shortbuf,"����ʶR...");
          pressanykey(shortbuf);
       }
      
       else if(variance==choice)  /*���w�g����*/
       {
          sprintf(shortbuf,"�A���w�g�� %s �o",p[variance].name);
          pressanykey(shortbuf);      
       }
      
       else if(p[choice].cost >= (d.money+p[variance].sell))  /*������*/
       {
          sprintf(shortbuf,"�o�ӭn %d ���A�A����������!",p[choice].cost);
          pressanykey(shortbuf);      
       }      
     
       else if(d.maxhp < p[choice].needmaxhp || d.maxmp < p[choice].needmaxmp 
               || d.speed < p[choice].needspeed ) /*��O����*/
       {
          sprintf(shortbuf,"�ݭnHP %d MP %d SPEED %d ��",
                p[choice].needmaxhp,p[choice].needmaxmp,p[choice].needspeed);
          pressanykey(shortbuf);            
       }
       else  /*���Q�ʶR*/
       {
          sprintf(shortbuf,"�A�T�w�n�ʶR %s ��?($%d) [y/N]",p[choice].name,p[choice].cost);
#ifdef MAPLE
          getdata(b_lines-1,1,shortbuf, ans, 2, 1, 0); 
#else
          getdata(b_lines-1,1,shortbuf, ans, 2, DOECHO, YEA);
#endif  // END MAPLE
          if(ans[0]=='y' || ans[0]=='Y')
          {
              sprintf(shortbuf,"�p���w�g�˰t�W %s �F",p[choice].name);
              pressanykey(shortbuf);
              d.attack+=(p[choice].attack-p[variance].attack);
              d.resist+=(p[choice].resist-p[variance].resist);
              d.speed+=(p[choice].speed-p[variance].speed);
              d.money-=(p[choice].cost-p[variance].sell);
              variance=choice;
          }
          else
          {
              sprintf(shortbuf,"����ʶR.....");
              pressanykey(shortbuf);            
          }
       }
     }       
     break;     
     
   case 'S':
   case 's':
     if(variance!=0)
     { 
        sprintf(shortbuf,"�A�T�w�n�汼%s��? ���:%d [y/N]",p[variance].name,p[variance].sell);
#ifdef MAPLE
        getdata(b_lines-1,1,shortbuf, ans, 2, 1, 0); 
#else
        getdata(b_lines-1,1,shortbuf, ans, 2, DOECHO, YEA);
#endif  // END MAPLE
        if(ans[0]=='y' || ans[0]=='Y')
        {
           sprintf(shortbuf,"�˳� %s ��F %d",p[variance].name,p[variance].sell);
           d.attack-=p[variance].attack;
           d.resist-=p[variance].resist;
           d.speed-=p[variance].speed;
           d.money+=p[variance].sell;
           pressanykey(shortbuf);
           variance=0;
        }
        else
        {
           sprintf(shortbuf,"ccc..�ڦ^����N�F...");
           pressanykey(shortbuf);         
        }
     }
     else if(variance==0)
     {
        sprintf(shortbuf,"�A���ӴN�S���˳ƤF...");
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
       my_write(currutmp->msgs[0].last_pid, "���y��^�h�G");
     }
     break;
#endif  // END MAPLE
   }
  }while((pipkey!='Q')&&(pipkey!='q')&&(pipkey!=KEY_LEFT));
    
  return variance;
}
