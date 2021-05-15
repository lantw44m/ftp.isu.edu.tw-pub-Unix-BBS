/*---------------------------------------------------------------------------*/
/* �צ���:���� �m�Z �צ�                                                   */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include <time.h>
#include "bbs.h"
#include "pip.h"
extern struct chicken d;
extern time_t start_time;
extern time_t lasttime;
#define getdata(a, b, c , d, e, f, g) getdata(a,b,c,d,e,f,NULL,g)

/*---------------------------------------------------------------------------*/
/* �צ���:���� �m�Z �צ�                                                   */
/* ��Ʈw                                                                    */
/*---------------------------------------------------------------------------*/
char *classrank[6]={"�S��","���","����","����","�i��","�M�~"};
int classmoney[11][2]={{ 0,  0},
		       {60,110},{70,120},{70,120},{80,130},{70,120},
		       {60,110},{90,140},{70,120},{70,120},{80,130}};		     
int classvariable[11][4]={
{0,0,0,0},
{5,5,4,4},{5,7,6,4},{5,7,6,4},{5,6,5,4},{7,5,4,6},
{7,5,4,6},{6,5,4,6},{6,6,5,4},{5,5,4,7},{7,5,4,7}};


char *classword[11][5]=
{
{"�ҦW","���\\�@","���\\�G","���Ѥ@","���ѤG"},
 
{"�۵M���","���b�Υ\\Ū�Ѥ�..","�ڬO�o���� cccc...",
            "�o�D���ݤ�����..�ǤF","�ᤣ���F :~~~~~~"},
            
{"��֧���","�ɫe�����...�ìO�a�W��...","�����ͫn��..�K�ӵo�X�K..",
            "��..�W�Ҥ��n�y�f��","�A�ٲV��..�@�A�I�|��֤T�ʭ�"},

{"���ǱШ|","���p����  ���p����","���ڭ̪ﱵ�Ѱ󤧪�",
	    "��..�A�b�F����? �٤��n�n��","���ǫ��Y�ª�..�Цn�n��..:("},

{"�x�ǱШ|","�]�l�L�k�O����L�k��..","�q�x����A�ڭn�a�L�h���M",
	    "����}�Σ�?�V�ð}��?? @_@","�A�٥H���A�b���T��ӣ�?"},

{"�C�D�޳N","�ݧڪ��F�`  �W�t�E�C....","�ڨ� �ڨ� �ڨ���..",
	    "�C�n��í�@�I��..","�A�b��a����? �C�����@�I"},

{"�氫�ԧ�","�٦׬O�٦�  �I�I..","�Q�K�ɤH���..",
	    "�}�A�𰪤@�I��...","���Y���o��S�O��.."},

{"�]�k�Ш|","���� ���� ��������..","�D�x+���i��+����+����=??",
	    "�p�ߧA��������  ���n�ô�..","����f�����n�y������y�W.."},

{"§���Ш|","�n����§������...","�ڶ٭�..��������..",
	    "���Ǥ��|��??�ѧr..","���_���ӨS����..�ѣ�.."},

{"ø�e�ޥ�","�ܤ�����..�����N�ѥ�..","�o�T�e���C��f�t���ܦn..",
	    "���n���e�Ű�..�n�[�o..","���n�r�e����..�a�a�p����.."},

{"�R�Чޥ�","�A�N���@�����Z��..","�R�вӭM�ܦn��..",
            "����A�X�n�@�I..","���U�A�u���@�I..���n�o��ʾ|.."}};
/*---------------------------------------------------------------------------*/
/* �צ���:���� �m�Z �צ�                                                   */
/* �禡�w                                                                    */
/*---------------------------------------------------------------------------*/

int pip_practice_classA()
{
/*  �u�w�w�w�w�q�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
/*  �x�۵M��Ǣx���O + 1~ 4 , �H�� - 0~0 , ���]��O - 0~0   �x*/
/*  �x        �u�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
/*  �x        �x���O + 2~ 6 , �H�� - 0~1 , ���]��O - 0~1   �x*/
/*  �x        �u�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
/*  �x        �x���O + 3~ 8 , �H�� - 0~2 , ���]��O - 0~1   �x*/
/*  �x        �u�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
/*  �x        �x���O + 4~12 , �H�� - 1~3 , ���]��O - 0~1   �x*/
/*  �u�w�w�w�w�q�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
	int body,class;
	int change1,change2,change3,change4,change5;
	char inbuf[256];
     
	class=d.wisdom/200+1; /*���*/
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
/*  �u�w�w�w�w�q�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
/*  �x�ֵ�    �x�P�� + 1~1 , ���O + 0~1 , ���N�׾i + 0~1    �x*/
/*  �x        �x��� + 0~1                                  �x*/
/*  �x        �u�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
/*  �x        �x�P�� + 1~2 , ���O + 0~2 , ���N�׾i + 0~1    �x*/
/*  �x        �x��� + 0~1                                  �x*/
/*  �x        �u�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
/*  �x        �x�P�� + 1~4 , ���O + 0~3 , ���N�׾i + 0~1    �x*/
/*  �x        �x��� + 0~1                                  �x*/
/*  �x        �u�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
/*  �x        �x�P�� + 2~5 , ���O + 0~4 , ���N�׾i + 0~1    �x*/
/*  �x        �x��� + 0~1                                  �x*/
/*  �u�w�w�w�w�q�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
	int body,class;
	int change1,change2,change3,change4,change5;
	char inbuf[256];
     
	class=(d.affect*2+d.wisdom+d.art*2+d.character)/400+1; /*�ֵ�*/
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
/*  �u�w�w�w�w�q�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
/*  �x����    �x���O + 1~1 , �H�� + 1~2 , ���]��O + 0~1    �x*/
/*  �x        �u�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
/*  �x        �x���O + 1~1 , �H�� + 1~3 , ���]��O + 0~1    �x*/
/*  �x        �u�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
/*  �x        �x���O + 1~2 , �H�� + 1~4 , ���]��O + 0~1    �x*/
/*  �x        �u�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
/*  �x        �x���O + 1~3 , �H�� + 1~5 , ���]��O + 0~1    �x*/
/*  �u�w�w�w�w�q�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
	int body,class;
	int change1,change2,change3,change4,change5;
	char inbuf[256];
     
	class=(d.belief*2+d.wisdom)/400+1; /*����*/
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
/*  �u�w�w�w�w�q�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
/*  �x�x��    �x���O + 1~2 , �԰��޳N + 0~1 , �P�� - 0~1    �x*/
/*  �x        �u�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
/*  �x        �x���O + 2~4 , �԰��޳N + 0~1 , �P�� - 0~1    �x*/
/*  �x        �u�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
/*  �x        �x���O + 3~4 , �԰��޳N + 0~1 , �P�� - 0~1    �x*/
/*  �x        �u�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
/*  �x        �x���O + 4~5 , �԰��޳N + 0~1 , �P�� - 0~1    �x*/
/*  �u�w�w�w�w�q�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
	int body,class;
	int change1,change2,change3,change4,change5;
	char inbuf[256];
     
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
/*  �u�w�w�w�w�q�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
/*  �x�C�N    �x�԰��޳N + 0~1 , ������O + 1~1             �x*/
/*  �x        �u�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
/*  �x        �x�԰��޳N + 0~1 , ������O + 1~2             �x*/
/*  �x        �u�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
/*  �x        �x�԰��޳N + 0~1 , ������O + 1~3             �x*/
/*  �x        �u�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
/*  �x        �x�԰��޳N + 0~1 , ������O + 1~4             �x*/
/*  �u�w�w�w�w�q�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
	int body,class;
	int change1,change2,change3,change4,change5;
	char inbuf[256];
     
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
/*  �u�w�w�w�w�q�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
/*  �x�氫�N  �x�԰��޳N + 1~1 , ���m��O + 0~0             �x*/
/*  �x        �u�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
/*  �x        �x�԰��޳N + 1~1 , ���m��O + 0~1             �x*/
/*  �x        �u�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
/*  �x        �x�԰��޳N + 1~2 , ���m��O + 0~1             �x*/
/*  �x        �u�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
/*  �x        �x�԰��޳N + 1~3 , ���m��O + 0~1             �x*/
/*  �u�w�w�w�w�q�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
	int body,class;
	int change1,change2,change3,change4,change5;
	char inbuf[256];
     
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
/*  �u�w�w�w�w�q�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
/*  �x�]�k    �x�]�k�޳N + 1~1 , �]�k��O + 0~2             �x*/
/*  �x        �u�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
/*  �x        �x�]�k�޳N + 1~2 , �]�k��O + 0~3             �x*/
/*  �x        �u�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
/*  �x        �x�]�k�޳N + 1~3 , �]�k��O + 0~4             �x*/
/*  �x        �u�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
/*  �x        �x�]�k�޳N + 2~4 , �]�k��O + 0~5             �x*/
/*  �u�w�w�w�w�q�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
	int body,class;
	int change1,change2,change3,change4,change5;
	char inbuf[256];

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
/*  �u�w�w�w�w�q�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
/*  �x§��    �x§����{ + 1~1 , ��� + 1~1                 �x*/
/*  �x        �u�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
/*  �x        �x§����{ + 1~2 , ��� + 1~2                 �x*/
/*  �x        �u�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
/*  �x        �x§����{ + 1~3 , ��� + 1~3                 �x*/
/*  �x        �u�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
/*  �x        �x§����{ + 2~4 , ��� + 1~4                 �x*/
/*  �u�w�w�w�w�q�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/    
	int body,class;
	int change1,change2,change3,change4,change5;
	char inbuf[256];

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
/*  �u�w�w�w�w�q�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
/*  �xø�e    �x���N�׾i + 1~1 , �P�� + 0~1                 �x*/
/*  �x        �u�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
/*  �x        �x���N�׾i + 1~2 , �P�� + 0~1                 �x*/
/*  �x        �u�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
/*  �x        �x���N�׾i + 1~3 , �P�� + 0~1                 �x*/
/*  �x        �u�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
/*  �x        �x���N�׾i + 2~4 , �P�� + 0~1                 �x*/
/*  �u�w�w�w�w�q�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
	int body,class;
	int change1,change2,change3,change4,change5;
	char inbuf[256];
     
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
/*  �u�w�w�w�w�q�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
/*  �x�R��    �x���N�׾i + 0~1 , �y�O + 0~1 , ��O + 1~1    �x*/
/*  �x        �u�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
/*  �x        �x���N�׾i + 1~1 , �y�O + 0~1 , ��O + 1~1    �x*/
/*  �x        �u�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
/*  �x        �x���N�׾i + 1~2 , �y�O + 0~2 , ��O + 1~1    �x*/
/*  �x        �u�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
/*  �x        �x���N�׾i + 1~3 , �y�O + 1~2 , ��O + 1~1    �x*/
/*  �|�w�w�w�w�r�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�}*/
	int body,class;
	int change1,change2,change3,change4,change5;
	char inbuf[256];
     
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

/*�ǤJ:�Ҹ� ���� �ͩR �ּ� ���� żż �Ǧ^:�ܼ�12345 return:body*/
int
pip_practice_function(classnum,classgrade,pic1,pic2,change1,change2,change3,change4,change5)
int classnum,classgrade,pic1,pic2;
int *change1,*change2,*change3,*change4,*change5;
{
	int  a,b,body,health;
	char inbuf[256],ans[5];
	long smoney;

	/*������k*/
	smoney=classgrade*classmoney[classnum][0]+classmoney[classnum][1];
	move(b_lines-2, 0);
	clrtoeol();
	sprintf(inbuf,"[%8s%4s�ҵ{]�n�� $%d ,�T�w�n��??[y/N]",classword[classnum][0],classrank[classgrade],smoney);
#ifdef MAPLE
	getdata(b_lines-2, 1,inbuf, ans, 2, 1, 0);
#else
        getdata(b_lines-2, 1,inbuf, ans, 2, DOECHO, YEA);
#endif  // END MAPLE
	if(ans[0]!='y' && ans[0]!='Y')  return 0;
	if(d.money<smoney)
	{
		pressanykey("�ܩ�p��...�A����������");
		return 0;
	}
	count_tired(4,5,"Y",100,1);
	d.money=d.money-smoney;
	/*���\�P�_���P�_*/
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
		sprintf(inbuf,"�U�����W [%8s%4s�ҵ{]",
		classword[classnum][0],classrank[classgrade+1]);        
		pressanykey(inbuf);
	}
	return 0;
}
