/*---------------------------------------------------------------------------*/
/* ���u���:�a�� �W�u �a�� �a�u                                              */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include <time.h>
#include "bbs.h"
#include "pip.h"
extern struct chicken d;
extern time_t start_time;
extern time_t lasttime;
#define getdata(a, b, c , d, e, f, g) getdata(a,b,c,d,e,f,NULL,g)

int pip_job_workA()
{
/*  �u�w�w�w�w�q�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
/*  �x�a�x�޲z�x�ݤH���� + N , ���a�~�� + N , �i���ޥ� + N  �x*/
/*  �x        �x�M���˪����Y + N , �h�� + 1 , �P�� - 2      �x*/
/*  �u�w�w�w�w�q�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
/*  �x�a�x�޲z�x�Y ��    �O - RND (�h��) >=   5 �h�u�@���\  �x*/
/*  �u�w�w�w�w�q�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/  
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
      pressanykey("�a�ƫܦ��\\��..�h�@�I�����A..");
    }
    else if(class<75 && class>=50)
    {
      d.cookskill+=rand()%2+5;    
      d.homework+=rand()%2+5;
      d.family+=rand()%3+3;
      d.relation+=rand()%3+3;
      workmoney=60+(d.cookskill*2+d.homework+d.family)/45;
      pressanykey("�a�����Z���Q����..���..");
    }    
    else if(class<50 && class>=25)
    {
      d.cookskill+=rand()%3+3;    
      d.homework+=rand()%3+3;
      d.family+=rand()%3+2;
      d.relation+=rand()%3+2;
      workmoney=40+(d.cookskill*2+d.homework+d.family)/50;
      pressanykey("�a�ƴ����q�q��..�i�H��n��..�[�o..");      
    }    
    else if(class<25)
    {
      d.cookskill+=rand()%3+1;    
      d.homework+=rand()%3+1;
      d.family+=rand()%3+1;
      d.relation+=rand()%3+1;
      workmoney=20+(d.cookskill*2+d.homework+d.family)/60;
      pressanykey("�a�ƫ��V�|��..�o�ˤ����..");      
    }        
    d.money+=workmoney;
    d.workA+=1;
    return 0;
}

int pip_job_workB()
{   
/*  �u�w�w�w�w�q�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
/*  �x�|���|  �x���� + N , �P�� + 1 , �y�O - 1 , �h�� + 3   �x*/
/*  �u�w�w�w�w�q�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
/*  �x�|���|  �x�Y ��    �O - RND (�h��) >=  20 �h�u�@���\  �x*/
/*  �u�w�w�w�w�q�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
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
      pressanykey("��O�i�ܦ��\\��..�U���A�ӳ�..");
    }
    else if(class<90 && class>=75)
    {
      d.love+=rand()%2+5;
      d.toman+=rand()%2+2;
      workmoney=120+(d.love+d.toman)/50;
      pressanykey("�O�i�ٷ�������..���..");
    }    
    else if(class<75 && class>=50)
    {
      d.love+=rand()%2+3;
      d.toman+=rand()%2+1;
      workmoney=100+(d.love+d.toman)/50;
      pressanykey("�p�B�ͫܥֳ�..�[�o..");      
    }    
    else if(class<50)
    {
      d.love+=rand()%2+1;
      d.toman+=rand()%2+1;
      workmoney=80+(d.love+d.toman)/50;
      pressanykey("���V�|��..�A�n����p�B�ͭC...");      
    }        
    d.money+=workmoney;
    d.workB+=1;
    return 0;
}
             
int pip_job_workC()
{
/*  �u�w�w�w�w�q�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
/*  �x���]    �x���a�~�� + N , �԰��޳N - N , �h�� + 2      �x*/
/*  �u�w�w�w�w�q�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
/*  �x���]    �x�Y ��    �O - RND (�h��) >=  30 �h�u�@���\  �x*/
/*  �u�w�w�w�w�q�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
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
      pressanykey("���]�Ʒ~�]�]��W..�Ʊ�A�A�L��...");
    }
    else if(class<95 && class>=80)
    {
      d.homework+=rand()%2+5;
      d.family+=rand()%2+3;
      d.hskill-=rand()%2+5;
      if(d.hskill<0)
           d.hskill=0;
      workmoney=200+(d.cookskill*2+d.homework*2)/50;    
      pressanykey("���]���Z���Q����..���..");
    }    
    else if(class<80 && class>=60)
    {
      d.homework+=rand()%2+3;
      d.family+=rand()%2+3;
      d.hskill-=rand()%2+5;
      if(d.hskill<0)
           d.hskill=0;
      workmoney=150+(d.cookskill*2+d.homework*2)/50;    
      pressanykey("�����q�q��..�i�H��n��..�[�o..");      
    }    
    else if(class<60)
    {
      d.homework+=rand()%2+1;
      d.family+=rand()%2+1;
      d.hskill-=rand()%2+1;
      if(d.hskill<0)
           d.hskill=0;
      workmoney=100+(d.cookskill*2+d.homework*2)/50;    
      pressanykey("�o�ӫ��V�|��..�A�o�ˤ����..");      
    }        
    d.money+=workmoney;
    d.workC+=1;
    return 0; 
}  

int pip_job_workD()
{    
/*  �u�w�w�w�w�q�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
/*  �x�A��    �x��O + 1 , �äO + 1 , ��� - 1 , �h�� + 3   �x*/
/*  �u�w�w�w�w�q�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
/*  �x�A��    �x�Y ��    �O - RND (�h��) >=  30 �h�u�@���\  �x*/
/*  �u�w�w�w�w�q�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/    
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
      pressanykey("���Ϫ����n�n��..�Ʊ�A�A������...");
    }
    else if(class<95 && class>=80)
    {
      workmoney=210+(d.wrist*2+d.hp*2)/80;
      pressanykey("����..�٤�����..:)");
    }    
    else if(class<80 && class>=60)
    {
      workmoney=160+(d.wrist*2+d.hp*2)/80;
      pressanykey("�����q�q��..�i�H��n��..");      
    }    
    else if(class<60)
    {
      workmoney=120+(d.wrist*2+d.hp*2)/80;
      pressanykey("�A���ӾA�X�A�����u�@  -_-...");      
    }        
    d.money+=workmoney;
    d.workD+=1;
    return 0;
}   
         
int pip_job_workE()
{
/*  �u�w�w�w�w�q�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
/*  �x�\�U    �x�Ʋz + N , �԰��޳N - N , �h�� + 2          �x*/
/*  �u�w�w�w�w�q�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
/*  �x�\�U    �x�Y �i���޳N - RND (�h��) >=  50 �h�u�@���\  �x*/
/*  �u�w�w�w�w�q�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
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
      pressanykey("�ȤH�����Ӧn�Y�F..�A�Ӥ@�L�a...");
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
      pressanykey("�N���٤����Y��..:)");
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
      pressanykey("�����q�q��..�i�H��n��..");      
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
      pressanykey("�A���p���ݥ[�j��...");      
    }        
    d.money+=workmoney;
    d.workE+=1;
    return 0;
}     

int pip_job_workF()
{           
/*  �u�w�w�w�w�q�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
/*  �x�а�    �x�H�� + 2 , �D�w + 1 , �o�^ - 2 , �h�� + 1   �x*/
/*  �u�w�w�w�w�q�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
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
      pressanykey("���ܤ� ���ݧA�o��{�u ���A�h�@�I...");
    }
    else if(class<75 && class>=50)
    {
      workmoney=75+(d.belief+d.etchics-d.offense)/20;
      pressanykey("���§A����������..:)");
    }    
    else if(class<50 && class>=25)
    {
      workmoney=50+(d.belief+d.etchics-d.offense)/20;
      pressanykey("�A�u���ܦ��R�߰�..���L���I�p�֪��ˤl...");      
    }    
    else if(class<25)
    {
      workmoney=25+(d.belief+d.etchics-d.offense)/20;
      pressanykey("�ө^�m����..���]���ॴ�V��....:(");      
    }        
    d.money+=workmoney;
    d.workF+=1;
    return 0;
}               

int pip_job_workG()
{   
/* �u�w�w�w�w�q�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
/* �x�a�u    �x��O + 2 , �y�O + 1 , �h�� + 3 ,�ͦR +1     �x*/
/* �u�w�w�w�w�q�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/   
    float class;
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
    pressanykey("�\\�a�u�n��ĵ���..:p");
    d.money+=workmoney;
    d.workG+=1;
    return 0;
}   
    
int pip_job_workH()
{
/*  �u�w�w�w�w�q�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
/*  �x����  �x�äO + 2 , ��� - 2 , �h�� + 4              �x*/
/*  �u�w�w�w�w�q�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
/*  �x����  �x�Y ��    �O - RND (�h��) >=  80 �h�u�@���\  �x*/
/*  �u�w�w�w�w�q�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
    float class;
    long workmoney;
    
    if((d.bbtime/60/30)<1) /*�@���~��*/
    {
      pressanykey("�p���Ӥp�F,�@���H��A�ӧa...");
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
      pressanykey("�A�äO�ܦn��..:)");
    }
    else if(class<70 && class>=50)
    {
      workmoney=300+d.wrist/20+d.maxhp/80;
      pressanykey("��F���־��.....:)");
    }    
    else if(class<50 && class>=20)
    {
      workmoney=250+d.wrist/20+d.maxhp/80;
      pressanykey("�����q�q��..�i�H��n��..");      
    }    
    else if(class<20)
    {
      workmoney=200+d.wrist/20+d.maxhp/80;
      pressanykey("�ݥ[�j��..����A�ӧa....");      
    }        
    d.money+=workmoney;
    d.workH+=1;
    return 0;     
}

int pip_job_workI()
{
/*  �u�w�w�w�w�q�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
/*  �x���e�|  �x�P�� + 1 , �äO - 1 , �h�� + 3              �x*/
/*  �u�w�w�w�w�q�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
/*  �x���e�|  �x�Y ���N�׾i - RND (�h��) >=  40 �h�u�@���\  �x*/
/*  �u�w�w�w�w�q�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
    float class;
    long workmoney;

    if((d.bbtime/60/30)<1) /*�@���~��*/
    {
      pressanykey("�p���Ӥp�F,�@���H��A�ӧa...");
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
      pressanykey("�ȤH���ܳ��w���A���y����..:)");
    }
    else if(class<80 && class>=60)
    {
      workmoney=360+d.art/10+d.affect/20;        
      pressanykey("����������..�ᦳ�ѥ�...:)");
    }    
    else if(class<60 && class>=40)
    {
      workmoney=320+d.art/10+d.affect/20;        
      pressanykey("��������..�A�[�o�@�I..");      
    }    
    else if(class<40)
    {
      workmoney=250+d.art/10+d.affect/20;        
      pressanykey("�ݥ[�j��..�H��A�ӧa....");      
    }        
    d.money+=workmoney;
    d.workI+=1;
    return 0;
}

int pip_job_workJ()
{           
/*  �u�w�w�w�w�q�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
/*  �x���y��  �x��O + 1 , ��� - 1 , ���� - 1 , �h�� + 3   �x*/
/*  �x        �x�԰��޳N + N                                �x*/
/*  �u�w�w�w�w�q�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
/*  �x���y��  �x�Y ��    �O - RND (�h��) >=  80 ��          �x*/
/*  �x        �x�Y ��    �O - RND (�h��) >=  40 �h�u�@���\  �x*/
/*  �u�w�w�w�w�q�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
    float class;
    float class1;
    long workmoney;

    /*�ⷳ�H�W�~��*/
    if((d.bbtime/60/30)<2)
    {
      pressanykey("�p���Ӥp�F,�ⷳ�H��A�ӧa...");
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
       pressanykey("�A�O�������y�H..");
    }
    else if((class<75 && class>=50) && class1>=60 )  
    {
       d.hskill+=rand()%2+5;
       workmoney=270+d.maxhp/45+d.hskill/20;   
       pressanykey("�����٤�����..�i�H���\\�@�y�F..:)");
    }
    else if((class<50 && class>=25) && class1 >=40 )
    {
       d.hskill+=rand()%2+3;
       workmoney=240+d.maxhp/40+d.hskill/20;
       pressanykey("�޳N�t�j�H�N  �A�[�o��..");
    }    
    else if((class<25 && class>=0) && class1 >=20)  
    {
       d.hskill+=rand()%2+1;
       workmoney=210+d.maxhp/30+d.hskill/20;    
       pressanykey("���y�O��O�P���O�����X....");
    }    
    else if(class<0)
    {
       d.hskill+=rand()%2;
       workmoney=190+d.hskill/20;
       pressanykey("�n�h�h����M�W�i���z��....");
    }    
    d.money=d.money+workmoney;        
    d.workJ+=1;
    return 0;   
}

int pip_job_workK()
{ 
/* �u�w�w�w�w�q�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
/* �x�u�a    �x��O + 2 , �y�O - 1 , �h�� + 3              �x*/
/* �u�w�w�w�w�q�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
    float class;
    long workmoney;
    
    /*�ⷳ�H�W�~��*/
    if((d.bbtime/60/30)<2)
    {
      pressanykey("�p���Ӥp�F,�ⷳ�H��A�ӧa...");
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
       pressanykey("�u�{�ܧ���  ���§A�F..");
    }
    else if(class<75 && class>=50)  
    {
       workmoney=220+d.maxhp/45;    
       pressanykey("�u�{�|�ٶ��Q  ���W�A�F..");
    }
    else if(class<50 && class>=25)  
    {
       workmoney=200+d.maxhp/40;    
       pressanykey("�u�{�t�j�H�N  �A�[�o��..");
    }    
    else if(class<25 && class>=0)  
    {
       workmoney=180+d.maxhp/30;    
       pressanykey("��  �ݥ[�j�ݥ[�j....");
    }    
    else if(class<0)
    {
       workmoney=160;
       pressanykey("�U����O�n�@�I..�h�ҫקC�@�I�A��....");
    }
    
    d.money=d.money+workmoney;
    d.workK+=1;
    return 0;
}

int pip_job_workL()
{   
/*  �u�w�w�w�w�q�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
/*  �x�Ӷ�    �x���]��O + N , �P�� + 1 , �y�O - 1          �x*/
/*  �x        �x�h�� + 2                                    �x*/
/*  �u�w�w�w�w�q�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
    float class;
    float class1;
    long workmoney;
    
    /*�T���~��*/
    if((d.bbtime/60/30)<3)
    {
      pressanykey("�p���{�b�٤Ӥp�F,�T���H��A�ӧa...");
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
      pressanykey("�u�Ӧ��\\��  ���A�h�I��");
    }
    else if((class<75 && class>=50)&& class1 >=50)
    {
      d.mresist+=rand()%2+5;
      workmoney=150+(d.affect+d.brave)/50;      
      pressanykey("�u���ٺ⦨�\\��..�°�..");    
    }    
    else if((class<50 && class>=25)&& class1 >=25)
    {
      d.mresist+=rand()%2+3;
      workmoney=120+(d.affect+d.brave)/60;
      pressanykey("�u���ٺ�t�j�H�N��..�[�o..");    
    }    
    else
    {
      d.mresist+=rand()%2+1;
      workmoney=80+(d.affect+d.brave)/70;
      pressanykey("�ڤ]����K��ԣ�F..�ЦA�[�o..");    
    }           
    if(rand()%10==5)
    {
       pressanykey("�u�O�˷�  ���J�즺���]..");
       pip_fight_bad(12);
    }
    d.money+=workmoney;
    d.workL+=1;
    return 0;   
}

int pip_job_workM()
{
/*  �u�w�w�w�w�q�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
/*  �x�a�x�Юv�x�D�w + 1 , ���� + N , �y�O - 1 , �h�� + 7   �x*/
/*  �u�w�w�w�w�q�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
    float class;
    long workmoney;    

    if((d.bbtime/60/30)<4)
    {
      pressanykey("�p���Ӥp�F,�|���H��A�ӧa...");
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
    pressanykey("�a�л��P ��M���N�֤@�I�o");
    d.workM+=1;
    return;
}

int pip_job_workN()
{   
/*  �u�w�w�w�w�q�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
/*  �x�s��    �x�i���ޥ� + N , �͸ܧޥ� + N , ���O - 2      �x*/
/*  �x        �x�h�� + 5                                    �x*/
/*  �u�w�w�w�w�q�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
/*  �x�s��    �x�Y ��    �O - RND (�h��) >=  60 ��          �x*/
/*  �x        �x�Y �y    �O - RND (�h��) >=  50 �h�u�@���\  �x*/
/*  �u�w�w�w�w�q�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
    float class;
    float class1;
    long workmoney;
    
    if((d.bbtime/60/30)<5)
    {
      pressanykey("�p���Ӥp�F,�����H��A�ӧa...");
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
      pressanykey("�A�ܬ���  :)");
    }
    else if((class<75 && class>=50)&& class1 >=50)
    {
      d.cookskill+=rand()%2+5;
      d.speech+=rand()%2+5;
      workmoney=400+(d.charm)/5;    
      pressanykey("�Z���w�諸�C....");    
    }    
    else if((class<50 && class>=25)&& class1 >=25)
    {
      d.cookskill+=rand()%2+4;
      d.speech+=rand()%2+3;
      workmoney=300+(d.charm)/5;    
      pressanykey("�ܥ��Z��..���������...");    
    }    
    else
    {
      d.cookskill+=rand()%2+2;
      d.speech+=rand()%2+2;
      workmoney=200+(d.charm)/5;    
      pressanykey("�A���A�O������..�Х[�o....");    
    }           
    d.money+=workmoney;
    d.workN+=1;
    return 0;        
}

int pip_job_workO()
{         
/*  �u�w�w�w�w�q�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
/*  �x�s�a    �x�y�O + 2 , �o�^ + 2 , �D�w - 3 , �H�� - 3   �x*/
/*  �x        �x�ݤH���� - N , �M���˪����Y - N , �h�� + 12 �x*/
/*  �u�w�w�w�w�q�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
/*  �x�s�a    �x�Y �y    �O - RND (�h��) >=  70 �h�u�@���\  �x*/
/*  �u�w�w�w�w�q�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
    float class;
    long workmoney;
    
    if((d.bbtime/60/30)<4)
    {
      pressanykey("�p���Ӥp�F,�|���H��A�ӧa...");
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
      pressanykey("�A�O���������P��  :)");
    }
    else if(class<75 && class>=50)
    {
      d.relation-=rand()%5+8;
      d.toman-=rand()%5+8;
      workmoney=500+(d.charm)/5;    
      pressanykey("�A�Z���w�諸�C..:)");    
    }    
    else if(class<50 && class>=25)
    {
      d.relation-=rand()%5+5;
      d.toman-=rand()%5+5;
      workmoney=400+(d.charm)/5;
      pressanykey("�A�ܥ��Z..����������...");    
    }    
    else
    {
      d.relation-=rand()%5+1;
      d.toman-=rand()%5+1;
      workmoney=300+(d.charm)/5;    
      pressanykey("��..�A���A�O������....");    
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
/*  �u�w�w�w�w�q�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
/*  �x�j�]�`�|�x�y�O + 3 , �o�^ + 1 , ��� - 2 , ���O - 1   �x*/
/*  �x        �x�ݤH���� - N , �h�� + 8                     �x*/
/*  �u�w�w�w�w�q�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t*/
/*  �x�j�]�`�|�x�Y �y    �O - RND (�h��) >=  70 ��          �x*/
/*  �x        �x�Y ���N�׾i - RND (�h��) >=  30 �h�u�@���\  �x*/
/*  �|�w�w�w�w�r�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�}*/
    float class;
    float class1;
    long workmoney;
    
    if((d.bbtime/60/30)<6)
    {
      pressanykey("�p���Ӥp�F,�����H��A�ӧa...");
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
      pressanykey("�A�O�]�`�|�̰{�G���P�P��  :)");
    }
    else if((class<75 && class>=50) && class1>20)
    {
      d.speech+=rand()%5+8;
      d.toman-=rand()%5+8;
      workmoney=800+(d.charm)/5;    
      pressanykey("���..�A�Z���w�諸�C..:)");    
    }    
    else if((class<50 && class>=25) && class1>10)
    {
      d.speech+=rand()%5+5;
      d.toman-=rand()%5+5;
      workmoney=600+(d.charm)/5;
      pressanykey("�A�n�[�o�F��..��������...");    
    }    
    else
    {
      d.speech+=rand()%5+1;
      d.toman-=rand()%5+1;
      workmoney=400+(d.charm)/5;    
      pressanykey("��..�A�����....");    
    }           
    d.money+=workmoney;
    if(d.toman<0)
       d.toman=0;
    d.workP+=1;
    return;    
}