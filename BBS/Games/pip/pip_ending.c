/*---------------------------------------------------------------------------*/
/* �����禡                                                                  */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include <time.h>
#include "bbs.h"
#include "pip.h"
extern struct chicken d;
extern time_t start_time;
extern time_t lasttime;
#define getdata(a, b, c , d, e, f, g) getdata(a,b,c,d,e,f,NULL,g)

/*--------------------------------------------------------------------------*/
/*  �����ѼƳ]�w                                                            */
/*--------------------------------------------------------------------------*/
struct newendingset
{
  char *girl;		/*�k�͵�����¾�~*/ 
  char *boy;		/*�k�͵�����¾�~*/
  int grade;		/*����*/
};
typedef struct newendingset newendingset;

/*�U��*/
struct newendingset endmodeall_purpose[] = {
"�k��¾�~",		"�k��¾�~",		0,
"�����o�Ӱ�a�s�k��",	"�����o�Ӱ�a�s���",	500,
"������a���_��",	"������a���_��",	400,
"�����з|�����j�D��",	"�����з|�����j�D��",	350,
"������a���j��",	"������a���j��",	320,
"�����@��դh",		"�����@��դh",		300,
"�����з|�����פk",	"�����з|��������",	150,
"�����k�x�W���j�k�x",   "�����k�x�W���k�x",	200,
"�������W���Ǫ�",	"�������W���Ǫ�",	120,
"�����@�W�k�x",		"�����@�W�k�x",		100,
"�b�|���|�u�@",		"�b�|���|�u�@",		100,
"�b���]�u�@",		"�b���]�u�@",		100,
"�b�A���u�@",		"�b�A���u�@",		100,
"�b�\\�U�u�@",		"�b�\\�U�u�@",		100,
"�b�а�u�@",		"�b�а�u�@",		100,
"�b�a�u�u�@",		"�b�a�u�u�@",		100,
"�b�����u�@",		"�b�����u�@",		100,
"�b���e�|�u�@",		"�b���e�|�u�@",		100,
"�b���y�Ϥu�@",		"�b���y�Ϥu�@",		100,
"�b�u�a�u�@",		"�b�u�a�u�@",		100,
"�b�Ӷ�u�@",		"�b�Ӷ�u�@",		100,
"����a�x�Юv�u�@",	"����a�x�Юv�u�@",	100,
"�b�s�a�u�@",		"�b�s�a�u�@",		100,
"�b�s���u�@",		"�b�s���u�@",		100,
"�b�j�]�`�|�u�@",	"�b�j�]�`�|�u�@",	100,
"�b�a������",		"�b�a������",		50,
"�b�|���|�ݮt",		"�b�|���|�ݮt",		50,
"�b���]�ݮt",		"�b���]�ݮt",		50,
"�b�A���ݮt",		"�b�A���ݮt",		50,
"�b�\\�U�ݮt",		"�b�\\�U�ݮt",		50,
"�b�а�ݮt",		"�b�а�ݮt",		50,
"�b�a�u�ݮt",		"�b�a�u�ݮt",		50,
"�b�����ݮt",		"�b�����ݮt",		50,
"�b���e�|�ݮt",		"�b���e�|�ݮt",		50,
"�b���y�ϭݮt",		"�b���y�ϭݮt",		50,
"�b�u�a�ݮt",		"�b�u�a�ݮt",		50,
"�b�Ӷ�ݮt",		"�b�Ӷ�ݮt",		50,
"����a�x�Юv�ݮt",	"����a�x�Юv�ݮt",	50,
"�b�s�a�ݮt",		"�b�s�a�ݮt",		50,
"�b�s���ݮt",		"�b�s���ݮt",		50,
"�b�j�]�`�|�ݮt",	"�b�j�]�`�|�ݮt",	50,
NULL,		NULL,	0
};

/*�԰�*/
struct newendingset endmodecombat[] = {
"�k��¾�~",		"�k��¾�~",			0,
"�Q�ʬ��i�� �Ԥh��",  	"�Q�ʬ��i�� �Ԥh��",		420,
"�Q���¦����@�ꪺ�N�x",	"�Q���¦����@�ꪺ�N�x",		300,
"��W�F��a��ö�����",	"��W�F��a��ö�����",		200,
"��F�Z�N�Ѯv",		"��F�Z�N�Ѯv",			150,
"�ܦ��M�h���İ�a",	"�ܦ��M�h���İ�a",		160,
"�먭�x�ȥͬ��A�����h�L","�먭�x�ȥͬ��A�����h�L",	80,
"�ܦ������y�H",		"�ܦ������y�H",			0,
"�H�ħL�u�@����",	"�H�ħL�u�@����",		0,
NULL,           NULL,   0
};

/*�]�k*/
struct newendingset endmodemagic[] = {
"�k��¾�~",	     	"�k��¾�~",		0,
"�Q�ʬ��i�� �]�k��",	"�Q�ʬ��i�� �]�k��",	420,
"�Q�u�����c�]�k�v",	"�Q�u�����x�]�k�v",	280,
"��F�]�k�Ѯv",		"��F�]�k�Ѯv",		160,
"�ܦ��@���]�ɤh",	"�ܦ��@���]�ɤh",	180,
"��F�]�k�v",		"��F�]�k�v",		120,
"�H�e�R�v���H��R����",	"�H�e�R�v���H��R����",	40,
"�����@���]�N�v",	"�����@���]�N�v",	20,
"�������Y���H",		"�������Y���H",		10,
NULL,           NULL	,0
};

/*����*/
struct newendingset endmodesocial[] = {
"�k��¾�~",     	"�k��¾�~",		0,
"����������d�m",	"�����k�����t����",	170,
"�Q�D�令�����m",	"�Q�襤��k�����ҴB",	260,
"�Q�B��ݤ��A�����ҤH",	"�����F�k�B�諸�ҴB",	130,
"�����I�����d�l",	"�����k�I�����ҴB",	100,
"�����ӤH���d�l",	"�����k�ӤH���ҴB",	80,
"�����A�H���d�l",	"�����k�A�H���ҴB",	80,
"�����a�D������",	"�����k�a�D������",	-40,
NULL,           NULL,	0
};
/*���N*/
struct newendingset endmodeart[] = {
"�k��¾�~",		"�k��¾�~",	0,
"�����F�p��",		"�����F�p��",	100,
"�����F�@�a",		"�����F�@�a",	100,
"�����F�e�a",		"�����F�e�a",	100,
"�����F�R�Юa",		"�����F�R�Юa",	100,
NULL,           NULL,	0
};

/*�t��*/
struct newendingset endmodeblack[] = {
"�k��¾�~",     	"�k��¾�~",		0,
"�ܦ��F�]��",		"�ܦ��F�]��",		-1000,
"�V���F�өf",		"�V���F�y�]",		-350,
"���F[��ۤk��]���u�@",	"���F[��۰��]���u�@",	-150,
"��F�µ󪺤j�j",	"��F�µ󪺦Ѥj",	-500,
"�ܦ����ű@��",		"�ܦ����ű���",		-350,
"�ܦ��B�ۮv�B�ۧO�H",	"�ܦ��������F�O�H��",	-350,
"�H�y�a���u�@�ͬ�",	"�H�������u�@�ͬ�",	-350,
NULL,		NULL,	0
};

/*�a��*/
struct newendingset endmodefamily[] = {
"�k��¾�~",     	"�k��¾�~",		0,
"���b�s�Q�צ�",		"���b�s���צ�",		50,
NULL,		NULL,	0
};


int /*�����e��*/
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
  prints("[1;33m������������������������������������������������������������[0m");
  move(2,9);
  prints("[1;37m��      ����    ������      ����      ����    ������      ��[0m");
  move(3,9);
  prints("[0;37m��    ������    ������  ������������������    ������  ������[0m");
  move(4,9);
  prints("[0;37m��    ������  ��  ����  ������������������  ��  ����  ������[0m");
  move(5,9);
  prints("[1;37m��      ����  ��  ����      ����      ����  ��  ����      ��[0m");
  move(6,9);
  prints("[1;35m������������������������������������������������������������[0m");
  move(7,8);
  prints("[1;31m�w�w�w�w�w�w�w�w�w�w[41;37m �P�ž԰����������i [0;1;31m�w�w�w�w�w�w�w�w�w�w�w[0m");
  move(9,10);
  prints("[1;36m�o�Ӯɶ�������ı�a�٬O���{�F...[0m");
  move(11,10);
  prints("[1;37m[33m%s[37m �o���}�A���ŷx�h��A�ۤv�@�����b�~���D�ͦs�F.....[0m",d.name);
  move(13,10);
  prints("[1;36m�b�A���U�оɥL���o�q�ɥ��A���L��Ĳ�F�ܦh���A���i�F�ܦh����O....[0m");
  move(15,10);
  prints("[1;37m�]���o�ǡA���p�� [33m%s[37m ���᪺�ͬ��A�ܱo��h���h���F........[0m",d.name);
  move(17,10);
  prints("[1;36m���A�����ߡA�A���I�X�A�A�Ҧ����R......[0m");
  move(19,10);
  prints("[1;37m[33m%s[37m �|�û����ʰO�b�ߪ�....[0m",d.name);
  pressanykey("���U�Ӭݥ��ӵo�i");
  clrchyiuan(7,19);
  move(7,8);
  prints("[1;34m�w�w�w�w�w�w�w�w�w�w[44;37m �P�ž԰������ӵo�i [0;1;34m�w�w�w�w�w�w�w�w�w�w�w[0m");
  move(9,10);
  prints("[1;36m�z�L�����y�A���ڭ̤@�_�Ӭ� [33m%s[36m �����ӵo�i�a.....[0m",d.name);
  move(11,10);
  prints("[1;37m�p�� [33m%s[37m ���%s....[0m",d.name,endbuf1);
  move(13,10);
  prints("[1;36m�]���L�����e���V�O�A�ϱo�L�b�o�@�譱%s....[0m",endbuf2);
  move(15,10);
  prints("[1;37m�ܩ�p�����B�ê��p�A�L���%s�A�B�ú�O�ܬ���.....[0m",endbuf3);
  move(17,10);
  prints("[1;36m��..�o�O�@�Ӥ�����������..........[0m");
  pressanykey("�ڷQ  �A�@�w�ܷP�ʧa.....");
  show_ending_pic(0);
  pressanykey("�ݤ@�ݤ����o");
  endgrade=pip_game_over(endgrade);
  pressanykey("�U�@���O�p�����  ����copy�U�Ӱ�����");
  pip_data_list();
  pressanykey("�w��A�ӬD��....");
  /*�O���}�l*/
  now=time(0);
  sprintf(buf, "[1;35m�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w[0m\n");
  pip_log_record(buf);
  sprintf(buf, "[1;37m�b [33m%s [37m���ɭԡA[36m%s [37m���p�� [32m%s[37m �X�{�F����[0m\n", Cdate(&now), cuser.userid,d.name);
  pip_log_record(buf);
  sprintf(buf, "[1;37m�p�� [32m%s [37m�V�O�[�j�ۤv�A���%s[0m\n[1;37m�]�����e���V�O�A�ϱo�b�o�@�譱%s[0m\n",d.name,endbuf1,endbuf2);
  pip_log_record(buf);
  sprintf(buf, "[1;37m�ܩ�B�ê��p�A�L���%s�A�B�ú�O�ܬ���.....[0m\n\n[1;37m�p�� [32n%s[37m ���`�n���� [33m%d[0m\n",endbuf3,d.name,endgrade);
  pip_log_record(buf);
  sprintf(buf, "[1;35m�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w[0m\n");
  pip_log_record(buf);  
  /*�O���פ�*/
  d.death=3;
  pipdie("[1;31m�C�������o...[m  ",3);
  return 0;
}

int 
pip_ending_decide(endbuf1,endbuf2,endbuf3,endmode,endgrade)
char *endbuf1,*endbuf2,*endbuf3;
int *endmode,*endgrade;
{
  char *name[8][2]={{"�k��","�k��"},
  	           {"�������l","���F���D"},
  	           {"�����A","���F�A"},
                   {"�����ӤH��","���F�k�ӤH��"},
                   {"�����ӤH��","���F�k�ӤH��"},
                   {"�����ӤH��","���F�k�ӤH��"},
                   {"�����ӤH��","���F�k�ӤH��"},
                   {"�����ӤH��","���F�k�ӤH��"}}; 
  int m=0,n=0,grade=0;
  int modeall_purpose=0;
  char buf1[256];
  char buf2[256];
  
  *endmode=pip_future_decide(&modeall_purpose);
  switch(*endmode)
  {
  /*1:�t�� 2:���N 3:�U�� 4:�Ԥh 5:�]�k 6:���� 7:�a��*/
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
    sprintf(buf2,"�D�`�����Q..");
  }
  else if(n==2)
  {
    *endgrade=grade+100;
    sprintf(buf2,"��{�٤���..");
  }
  else if(n==3)
  {
    *endgrade=grade-10;
    sprintf(buf2,"�`�J��ܦh���D....");
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
      sprintf(buf2,"���F�P�檺�k��");
    else
      sprintf(buf2,"�����F�P�檺�k��");  
  } 
  strcpy(endbuf3, buf2);  
  return 0;
}
/*�����P�_*/
/*1:�t�� 2:���N 3:�U�� 4:�Ԥh 5:�]�k 6:���� 7:�a��*/
int
pip_future_decide(modeall_purpose)
int *modeall_purpose;
{
  int endmode;
  /*�t��*/
  if((d.etchics==0 && d.offense >=100) || (d.etchics>0 && d.etchics<50 && d.offense >=250))
     endmode=1;
  /*���N*/
  else if(d.art>d.hexp && d.art>d.mexp && d.art>d.hskill && d.art>d.mskill &&
          d.art>d.social && d.art>d.family && d.art>d.homework && d.art>d.wisdom &&
          d.art>d.charm && d.art>d.belief && d.art>d.manners && d.art>d.speech &&
          d.art>d.cookskill && d.art>d.love)
     endmode=2;
  /*�԰�*/
  else if(d.hexp>=d.social && d.hexp>=d.mexp && d.hexp>=d.family)
  {
     *modeall_purpose=1;
     if(d.hexp>d.social+50 || d.hexp>d.mexp+50 || d.hexp>d.family+50)
        endmode=4;
     else
        endmode=3;     
  }
  /*�]�k*/
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
/*���B���P�_*/
int
pip_marry_decide()
{
  int grade;
  if(d.lover!=0)
  {  
     /* 3 4 5 6 7:�ӤH */
     d.lover=d.lover;
     grade=80;
  }
  else
  {
     if(d.royalJ>=d.relation && d.royalJ>=100)
     {
        d.lover=1;  /*���l*/
        grade=200;
     }
     else if(d.relation>d.royalJ && d.relation>=100)
     {
        d.lover=2;  /*���˩Υ���*/
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
pip_endingblack(buf,m,n,grade) /*�t��*/
char *buf;
int *m,*n,*grade;
{
 if(d.offense>=500 && d.mexp>=500) /*�]��*/
 {
   *m=1;
   if(d.mexp>=1000)
     *n=1;
   else if(d.mexp<1000 && d.mexp >=800)
     *n=2;
   else
     *n=3;
 }

else if(d.hexp>=600)  /*�y�]*/
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
 else if(d.hexp>=320 && d.character>200 && d.charm< 200)	/*�µ�Ѥj*/
 {
   *m=4;
   if(d.hexp>=400)
     *n=1;
   else if(d.hexp<400 && d.hexp>=360)
     *n=2;
   else 
     *n=3;  
 }
 else if(d.character>=200 && d.charm >=200 && d.speech>70 && d.toman >70)  /*���ű@��*/
 {
   *m=5;
   if(d.charm>=300)
     *n=1;
   else if(d.charm<300 && d.charm>=250)
     *n=2;
   else 
     *n=3;  
 }
 
 else if(d.wisdom>=450)  /*�B�F�v*/
 {
   *m=6;
   if(d.wisdom>=550)
     *n=1;
   else if(d.wisdom<550 && d.wisdom>=500)
     *n=2;
   else 
     *n=3;  
 }
 
 else /*�y�a*/
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
pip_endingsocial(buf,m,n,grade) /*����*/
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
pip_endingmagic(buf,m,n,grade) /*�]�k*/
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
pip_endingcombat(buf,m,n,grade) /*�԰�*/
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
pip_endingfamily(buf,m,n,grade) /*�a��*/
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
pip_endingall_purpose(buf,m,n,grade,mode) /*�U��*/
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
pip_endingart(buf,m,n,grade) /*���N*/
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
	prints("[1;36m�P�±z������ӬP�Ťp�����C��.....[0m");
	move(10,17);
	prints("[1;37m�g�L�t�έp�⪺���G�G[0m");
	move(12,17);
	prints("[1;36m�z���p�� [37m%s [36m�`�o���� [1;5;33m%d [0m",d.name,gradeall);
	return gradeall;
}

int pip_divine() /*�e�R�v�ӳX*/
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
  prints("[1;33;5m�n�n�n...[0;1;37m��M�ǨӰ}�}���V���n.........[0m");
  pressanykey("�h�@�@�O�֧a......");
  clrchyiuan(0,24);
  move(10,14);
  prints("[1;37;46m    ��ӬO���C�|�����e�R�v�ӳX�F.......    [0m");
  pressanykey("�}�����L�i�ӧa....");
  if(d.money>=money)
  {
    randvalue=rand()%5;
    sprintf(buf,"�A�n�e�R��? �n��%d����...[Y/n]",money);
#ifdef MAPLE
    getdata(12,14,buf, ans, 2, 1, 0);
#else
    getdata(12,14,buf, ans, 2, DOECHO, YEA);
#endif  // END MAPLE
    if(ans[0]!='N' && ans[0]!='n')
    {
      pip_ending_decide(endbuf1,endbuf2,endbuf3,&endmode,&endgrade);
      if(randvalue==0)
      		sprintf(buf,"[1;37m  �A���p��%s�H��i�઺�����O%s  [0m",d.name,endmodemagic[2+rand()%5].girl);
      else if(randvalue==1)
      		sprintf(buf,"[1;37m  �A���p��%s�H��i�઺�����O%s  [0m",d.name,endmodecombat[2+rand()%6].girl);
      else if(randvalue==2)
      		sprintf(buf,"[1;37m  �A���p��%s�H��i�઺�����O%s  [0m",d.name,endmodeall_purpose[6+rand()%15].girl);
      else if(randvalue==3)
      		sprintf(buf,"[1;37m  �A���p��%s�H��i�઺�����O%s  [0m",d.name,endmodeart[2+rand()%6].girl);
      else if(randvalue==4)
      		sprintf(buf,"[1;37m  �A���p��%s�H��i�઺�����O%s  [0m",d.name,endbuf1);
      d.money-=money;
      clrchyiuan(0,24);
      move(10,14);
      prints("[1;33m�b�ڥe�R���G�ݨ�....[0m");
      move(12,14);
      prints(buf);
      pressanykey("���´f�U�A���t�A�����F.(���Ǥ���ǧڳ�)");
    }
    else
    {
      pressanykey("�A���Q�e�R��?..�u�i��..���u�����U���a...");
    }
  }
  else
  {
    pressanykey("�A����������..�u�O�i��..���U���a...");
  }
  return 0;
}
