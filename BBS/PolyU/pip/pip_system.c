/*---------------------------------------------------------------------------*/
/* �t�ο��:�ӤH���  �p�����  �S�O�A��                                     */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include <time.h>
#include "bbs.h"
#include "pip.h"
extern struct chicken d;
extern time_t start_time;
extern time_t lasttime;
#define getdata(a, b, c , d, e, f, g) getdata(a,b,c,d,e,f,NULL,g)

char weaponhead[7][10]={
"�S���˳�",
"�콦�U�l", 
"���֤p�U",
"���w���U",
"���K�Y��",
"�]�k�v�T",
"�����t��"};


char weaponrhand[10][10]={
"�S���˳�",
"�j���",  
"���ݧ��",
"�C�ɼC",  
"���p�C", 
"���l�M", 
"�ѱ��C", 
"���Y�_�M",
"�O�s�M",  
"�����t��"
};  

char weaponlhand[8][10]={
"�S���˳�",
"�j���", 
"���ݧ��",
"���",
"���ÿ���",
"�ժ�����",
"�]�k��",
"�����t��"
};


char weaponbody[7][10]={
"�S���˳�",
"�콦�`��",
"�S�ť֥�",
"���K����",
"�]�k�ܭ�",
"�ժ�����",
"�����t��"};

char weaponfoot[8][12]={
"�S���˳�",
"�콦��c",
"�F�v��j",
"�S�ūB�c",
"NIKE�B�ʾc",
"�s���ֹu",
"�����]�u",
"�����t�u"
};

int 
pip_system_freepip()
{
      char buf[256];
      move(b_lines-1, 0);
      clrtoeol();
#ifdef MAPLE
      getdata(b_lines-1,1, "�u���n��ͶܡH(y/N)", buf, 2, 1, 0);
#else
      getdata(b_lines-1,1, "�u���n��ͶܡH(y/N)", buf, 2, DOECHO, YEA);
#endif  // END MAPLE
      if (buf[0]!='y'&&buf[0]!='Y') return 0;
      sprintf(buf,"%s �Q���ߪ� %s �ᱼ�F~",d.name,cuser.userid);
      pressanykey(buf);
      d.death=2;
      pipdie("[1;31m�Q���ߥ��:~~[0m",2);
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
     prints("[1;44m  �A�ȶ���  [46m[1]�R�W�j�v [2]�ܩʤ�N [3]�����]��                                [0m");
     pipkey=egetch();
     
     switch(pipkey)
     {
     case '1':
       move(b_lines-1,0);
       clrtobot();
#ifdef MAPLE
       getdata(b_lines-1, 1, "���p�����s���Ӧn�W�r�G", buf, 11, DOECHO,NULL);
#else
       getdata(b_lines-1, 1, "���p�����s���Ӧn�W�r�G", buf, 11, DOECHO,YEA);
#endif  // END MAPLE
       if(!buf[0])
       {
         pressanykey("���@�U�Q�n�A�Ӧn�F  :)");
         break;
       }
       else
       {
        strcpy(oldname,d.name);
        strcpy(d.name,buf);
        /*��W�O��*/
        now=time(0);
        sprintf(buf, "[1;37m%s %-11s��p�� [%s] ��W�� [%s] [0m\n", Cdate(&now), cuser.userid,oldname,d.name);
        pip_log_record(buf);
        pressanykey("���  ���@�ӷs���W�r��...");
       }
       break;
       
     case '2':  /*�ܩ�*/
       move(b_lines-1,0);
       clrtobot();
       /*1:�� 2:�� */
       if(d.sex==1)
       { 
         oldchoice=2; /*��-->��*/
         move(b_lines-1, 0);
         prints("[1;33m�N�p����[32m��[33m�ܩʦ�[35m��[33m���ܡH [37m[y/N][0m");
       }
       else
       { 
         oldchoice=1; /*��-->��*/
         move(b_lines-1, 0); 
         prints("[1;33m�N�p����[35m��[33m�ܩʦ�[35m��[33m���ܡH [37m[y/N][0m");
       }
       move(b_lines,0);
       prints("[1;44m  �A�ȶ���  [46m[1]�R�W�j�v [2]�ܩʤ�N [3]�����]��                                [0m");
       pipkey=egetch();
       if(pipkey=='Y' || pipkey=='y')
       {
         /*��W�O��*/
         now=time(0);
         if(d.sex==1)
           sprintf(buf,"[1;37m%s %-11s��p�� [%s] �ѡ��ܩʦ���F[0m\n",Cdate(&now), cuser.userid,d.name);
         else
           sprintf(buf,"[1;37m%s %-11s��p�� [%s] �ѡ��ܩʦ���F[0m\n",Cdate(&now), cuser.userid,d.name);           
         pip_log_record(buf);
         pressanykey("�ܩʤ�N����...");       
         d.sex=oldchoice;
       }  
       break;
       
     case '3':
       move(b_lines-1,0);
       clrtobot();
       /*1:���n�B���B 4:�n�B���B */
       oldchoice=d.wantend;
       if(d.wantend==1 || d.wantend==2 || d.wantend==3)
       { 
         oldchoice+=3; /*�S��-->��*/
         move(b_lines-1, 0); 
         prints("[1;33m�N�p���C���令[32m[��20������][33m? [37m[y/N][0m");
	 sprintf(buf,"�p���C���]�w��[��20������]..");         
       }
       else
       { 
         oldchoice-=3; /*��-->�S��*/
         move(b_lines-1, 0); 
         prints("[1;33m�N�p���C���令[32m[�S��20������][33m? [37m[y/N][0m");
         sprintf(buf,"�p���C���]�w��[�S��20������]..");
       }
       move(b_lines,0);
       prints("[1;44m  �A�ȶ���  [46m[1]�R�W�j�v [2]�ܩʤ�N [3]�����]��                                [0m");
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

int
pip_data_list()  /*�ݤp���ӤH�ԲӸ��*/
{
  char buf[256];
  char inbuf1[20];
  char inbuf2[20];
  int tm;
  int pipkey;
  int page=1;
  
  tm=(time(0)-start_time+d.bbtime)/60/30;

  clear();  
  move(1,0);
  prints("       [1;33m����������������������������������������[m\n");
  prints("       [0;37m������  ���� ��   �������������� ��   ��[m\n");
  prints("       [1;37m������  ��������  ��  ����    ������  ��[m\n");
  prints("       [1;34m��������������������  ����    ����������[32m......................[m");
  do
  { clrchyiuan(5,23);
    switch(page)
    {
     case 1:
       move(5,0);
       sprintf(buf,
       "[1;31m �~�t[41;37m �򥻸�� [0;1;31m�u�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w��[m\n");  
       prints(buf);
  
       sprintf(buf,
       "[1;31m �x[33m�̩m    �W :[37m %-10s [33m�̥�    �� :[37m %02d/%02d/%02d   [33m�̦~    �� :[37m %-2d         [31m�x[m\n",
       d.name,d.year,d.month,d.day,tm);
       prints(buf);  
  
       sprintf(inbuf1,"%d/%d",d.hp,d.maxhp);  
       sprintf(inbuf2,"%d/%d",d.mp,d.maxmp);  
       sprintf(buf,
       "[1;31m �x[33m����    �� :[37m %-5d(�̧J)[33m����    �O :[37m %-11s[33m�̪k    �O :[37m %-11s[31m�x[m\n",
       d.weight,inbuf1,inbuf2);
       prints(buf);  
  
       sprintf(buf,
       "[1;31m �x[33m�̯h    �� :[37m %-3d        [33m�̯f    �� :[37m %-3d        [33m��ż    ż :[37m %-3d        [31m�x[m\n",
       d.tired,d.sick,d.shit);
       prints(buf);  
   
       sprintf(buf,  
       "[1;31m �x[33m�̵�    �O :[37m %-7d    [33m�̿ˤl���Y :[37m %-7d    [33m�̪�    �� :[37m %-11d[31m�x[m\n",
       d.wrist,d.relation,d.money);
       prints(buf);  
  
       sprintf(buf,  
       "[1;31m �u�t[41;37m ��O��� [0;1;31m�u�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t[m\n");
       prints(buf);  
   
       sprintf(buf,   
       "[1;31m �x[33m�̮�    �� :[37m %-10d [33m�̴�    �O :[37m %-10d [33m�̷R    �� :[37m %-10d [31m�x[m\n",
       d.character,d.wisdom,d.love);
       prints(buf);  
   
       sprintf(buf, 
       "[1;31m �x[33m����    �N :[37m %-10d [33m�̹D    �w :[37m %-10d [33m�̮a    �� :[37m %-10d [31m�x[m\n",
       d.art,d.etchics,d.homework);
       prints(buf);  
 
       sprintf(buf, 
       "[1;31m �x[33m��§    �� :[37m %-10d [33m����    �� :[37m %-10d [33m�̲i    �� :[37m %-10d [31m�x[m\n",
       d.manners,d.speech,d.cookskill);
       prints(buf);    
 
       sprintf(buf,  
       "[1;31m �u�t[41;37m ���A��� [0;1;31m�u�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t[m\n");
       prints(buf);  
 
       sprintf(buf, 
       "[1;31m �x[33m�̧�    �� :[37m %-10d [33m�̺�    �N :[37m %-10d [33m�̤H    �� :[37m %-10d [31m�x[m\n",
       d.happy,d.satisfy,d.toman);
       prints(buf);
  
       sprintf(buf, 
       "[1;31m �x[33m�̾y    �O :[37m %-10d [33m�̫i    �� :[37m %-10d [33m�̫H    �� :[37m %-10d [31m�x[m\n",
       d.charm,d.brave,d.belief);
       prints(buf);  

       sprintf(buf, 
       "[1;31m �x[33m�̸o    �^ :[37m %-10d [33m�̷P    �� :[37m %-10d [33m            [37m            [31m�x[m\n",
       d.offense,d.affect);
       prints(buf);  

       sprintf(buf, 
       "[1;31m �u�t[41;37m ������� [0;1;31m�u�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t[m\n");
       prints(buf);  

       sprintf(buf, 
       "[1;31m �x[33m�̪������ :[37m %-10d [33m�̾԰����� :[37m %-10d [33m���]�k���� :[37m %-10d [31m�x[m\n",
       d.social,d.hexp,d.mexp);
       prints(buf);  

       sprintf(buf, 
       "[1;31m �x[33m�̮a�Ƶ��� :[37m %-10d [33m            [37m            [33m            [37m            [31m�x[m\n",
       d.family);
       prints(buf);  
  
       sprintf(buf, 
       "[1;31m ���w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w��[m\n");
       prints(buf);  
       
       move(b_lines-1,0);       
       sprintf(buf, 
       "                                                              [1;36m�Ĥ@��[37m/[36m�@�G��[m\n");
       prints(buf);  
       break;

     case 2:
       move(5,0);
       sprintf(buf, 
       "[1;31m �~�t[41;37m ���~��� [0;1;31m�u�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w��[m\n");
       prints(buf);  
  
       sprintf(buf, 
       "[1;31m �x[33m�̭�    �� :[37m %-10d [33m�̹s    �� :[37m %-10d [33m�̤j �� �Y :[37m %-10d [31m�x[m\n",
       d.food,d.cookie,d.bighp);
       prints(buf);  
  
       sprintf(buf, 
       "[1;31m �x[33m����    �� :[37m %-10d [33m�̮�    �� :[37m %-10d [33m�̪�    �� :[37m %-10d [31m�x[m\n",
       d.medicine,d.book,d.playtool);
       prints(buf);  
  
       sprintf(buf, 
       "[1;31m �u�t[41;37m �C����� [0;1;31m�u�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t[m\n");
       prints(buf);  
  
       sprintf(buf, 
       "[1;31m �x[33m�̲q �� Ĺ :[37m %-10d [33m�̲q �� �� :[37m %-10d                         [31m�x[m\n",
       d.winn,d.losee);
       prints(buf);  
  
       sprintf(buf, 
       "[1;31m �u�t[41;37m �Z�O��� [0;1;31m�u�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t[m\n");
       prints(buf);  
  
       sprintf(buf, 
       "[1;31m �x[33m�̧� �� �O :[37m %-10d [33m�̨� �m �O :[37m %-10d [33m�̳t �� �� :[37m %-10d [31m�x[m\n",
       d.attack,d.resist,d.speed);
       prints(buf);  
       sprintf(buf, 
       "[1;31m �x[33m�̧��]��O :[37m %-10d [33m�̾԰��޳N :[37m %-10d [33m���]�k�޳N :[37m %-10d [31m�x[m\n",
       d.mresist,d.hskill,d.mskill);
       prints(buf);  
  
       sprintf(buf, 
       "[1;31m �x[33m���Y���˳� :[37m %-10s [33m�̥k��˳� :[37m %-10s [33m�̥���˳� :[37m %-10s [31m�x[m\n",
       weaponhead[d.weaponhead],weaponrhand[d.weaponrhand],weaponlhand[d.weaponlhand]);
       prints(buf);  
  
       sprintf(buf, 
       "[1;31m �x[33m�̨���˳� :[37m %-10s [33m�̸}���˳� :[37m %-10s [33m            [37m            [31m�x[m\n",
       weaponbody[d.weaponbody],weaponfoot[d.weaponfoot]);
       prints(buf);  
  
       sprintf(buf, 
       "[1;31m ���w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w��[m\n");
       prints(buf); 

       move(b_lines-1,0);
       sprintf(buf, 
       "                                                              [1;36m�ĤG��[37m/[36m�@�G��[m\n");
       prints(buf);          
       break;
    }
    move(b_lines,0);
    sprintf(buf,"[1;44;37m  ��ƿ��  [46m  [��/PAGE UP]���W�@�� [��/PAGE DOWN]���U�@�� [Q]���}:            [m");
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
