/* �q�l�� �p�X�O..�Xa�Xb�C��.�  */
/* Writed by Birdman From 140.116.102.125 �зN�z����*/
// modified for MagicBBS by Magi



#include "bbs.h"
#include "money.h"
#define DATA_FILE  "chicken"
#define HANDLE     "chicken.handle"
#define MAX_SALE_VALUE (3000000)

char
*cstate[10]={"�ڦb�Y��","���Y�s��","�ԫK�K","�³J..�鵹��?","��..Ĺ�p���]�S�h���a"
            ,"�S������..","�h�ҥ���!"};
char *cage[9]={"�ϥ�","�g��","���~","�֦~","�C�~","���O","���~","���~"};
char *menu[8]={"�C��","�B��","�ձЭp��","�R��u��","�M�z����"};

time_t birth;
int weight,satis,mon,day,age,angery,sick,oo,happy,clean,tiredstrong,play;
int winn,losee,last,ate=0,agetmp,food,zfood;
int hdc;

extern time_t login_start_time;
char Name[20];
FILE *cfp;

int
get_handle()
{
   FILE *fp;
   int h; 
   char fname[50];
   
   setuserfile(fname, HANDLE);
   
   fp = fopen(fname, "r");
   fscanf(fp,"%d",&h);
   fclose(fp);
   return h;
}

int
new_handle()
{
   FILE *fp;
   char fname[50];
   int h;
   
   setuserfile(fname, HANDLE);
   
   h=time(NULL);
   fp = fopen(fname, "w");
   fprintf(fp,"%d",h);
   fclose(fp);
   return h;
}

int check_multigame()
{
  if(hdc!=get_handle()) { clear();
                          move(10,10);
                          prints("�ФŦP�ɦh�����i��C���C");
                          pressanykey(NULL); return 1; }
  return 0;
}

int
chicken_main()
{
  FILE *fp;
  time_t now = time(0);
  struct tm *ptime;
  char fname[50];
  agetmp=1;

  time(&now);
  ptime = localtime(&now);
  setuserfile(fname, DATA_FILE);
  if ((fp = fopen(fname, "r+")) == NULL)
   {
       creat_a_egg();
       last=1;
       fp = fopen(fname, "r");
        fscanf(fp,"%d %d %d %d %d %d %d %d %d %d %d %d %d %d %s "
         ,&weight,&mon,&day,&satis,&age,&oo,&happy,&clean,&tiredstrong,&play
         ,&winn,&losee,&food,&zfood,Name);
        fclose(fp);

    }
   else
   {
       last=0;
        fscanf(fp,"%d %d %d %d %d %d %d %d %d %d %d %d %d %d %s "
         ,&weight,&mon,&day,&satis,&age,&oo,&happy,&clean,&tiredstrong,&play
         ,&winn,&losee,&food,&zfood,Name);
        fclose(fp);
   }
/*��*/
  if(ptime->tm_mon+1 < day)
   ptime->tm_mon+=12;
  if(day<(ptime->tm_mon+1))
   { age=ptime->tm_mday;
    age=age+31-mon;}
  else
    age=ptime->tm_mday-mon;

  

  show_chicken();
  hdc=new_handle();  
  select_menu();
                          
    fp = fopen(fname, "r+");
        fprintf(fp,"%d %d %d %d %d %d %d %d %d %d %d %d %d %d %s "
          ,weight,mon,day,satis,age,oo,happy,clean,tiredstrong,play
          ,winn,losee,food,zfood,Name);

  fclose(fp);
  return 0;
}

int creat_a_egg()
{
  char name[20];
  char fname[50];
  struct tm *ptime;
  FILE *fp;
  time_t now;

  time(&now);
  ptime = localtime(&now);
  move(3,0);
  clrtobot();
  setuserfile(fname, DATA_FILE);
  unlink(fname);
  while(strlen(Name)<1)
   getdata(4, 0, "���p�����Ӧn�W�r�G", Name, 21, DOECHO,NULL,YEA);
  setuserfile(fname, DATA_FILE);
  fp = fopen(fname, "w");
  fprintf(fp,"%d %d %d %d %d %d %d %d %d %d %d %d %d %d %s "
      ,ptime->tm_hour*2,ptime->tm_mday,ptime->tm_mon+1
        ,0,0,0,0,0,0,0,0,0,10,5,Name);
  fclose(fp);

  if((fp != fopen("etc/chicken", "a"))==NULL);
  fprintf(fp,"[32m%s[m �b [34;43m[%d/%d  %d:%02d][m  �i�F�@���s [33m%s[m ���p��\n",
          currentuser.userid,ptime->tm_mon + 1, ptime->tm_mday,
          ptime->tm_hour, ptime->tm_min,Name);
  fclose(fp);
  return;
}

int show_chicken()
{
  int diff;

  diff = (time(0) - login_start_time) / 120;

  if(ate>5 && tiredstrong>8)
      {
        weight-=3;
        clean+=3;
        tiredstrong-=2;
        ate=0;
      }
  if(weight<0)
    death();
/*  if((diff-age)>1 && agetmp) �٬O�����D
   {age++;
    agetmp=0;}
*/
  move(1,0);
  clrtobot();
  prints(
     "[33mName:%s[m"
     "  [45mAge :%d��[m"
     "  ���q:%d"
     "  ����:%d"
     "  �s��:%d"
     "  �h��:%d"
     "  �K�K:%d\n"
     "�ͤ�:%d��%d��"
     "   �}�}:$%d"
     "   �ּ֫�:%d"
     "   ���N��:%d"
     "  �j�ɤY:%d\n",
     Name,age,weight,food,zfood,tiredstrong,clean,day
     ,mon,currentuser.money,happy,satis,oo);

  move(3,0);
 if(age<=16){
  switch(age)
  {
     case 0:
     case 1:
      outs("  ��������
��  ����  ��
������������
����    ����
������������
  ��������   ");

      break;
     case 2:
     case 3:
      outs("    ������������
  ��            ��
��    ��    ��    ��
��                ��
��      ����      ��
��                ��
��                ��
  ��            ��
    ������������   ");

       break;
     case 4:
     case 5:

        outs("      ������������
    ��            ��
  ��  ��        ��  ��
  ��                ��
  ��      ����      ��
������    ����      ����
  ��                ��
  ��                ��
    ��  ��������  ��
      ��      ��  ��
                ��    ");
        break;
        case 6:
        case 7:
         outs("   ��    ������������
��  ����  ��        ��
��              ��    ��
  ������                ��
��                      ��
��  ����                ��
  ��  ��                ��
      ��                ��
        ��            ��
          ������������        ");
        break;

        case 8:
        case 9:
        case 10:
         outs("    ����          ����
  ��������      ��������
  ����������������������
  ��                  ��
  ��    ��      ��    ��
��                      ��
��        ������        ��
  ��                  ��
��    ��          ��  ��
  ����            ������
  ��                  ��
    ��              ��
      ��  ������  ��
      ��  ��    ��
        ��               ");

        break;

        case 11:
        case 12:
        outs("        ������������
      ��    ��    ����
    ��  ��      ��  ����
  ����              ������
��              ��    ����
����������������      ����
  ��                  ����
    ��        ��  ��    ��
    ��        ��  ��    ��
    ��          ��      ��
      ��              ��
        ��  ������  ��
        ��  ��  ��  ��
          ��      ��             ");

        break;
        case 13:
        case 14:
        outs("              ��������
      ����������������
    ��������������������
  ������������������������
  ��    ������������������
����    ��            ����
��������                ��
  ��                    ��
    ����            ����
  ��    ������������  ��
  ��                  ��
    ��                  ��
      ��                ��
    ������            ������        ");
        break;
        case 15:
        case 16:
        outs("  ��    ������������
��  ����  ��        ��
��              ��    ��
  ������                ��
��                      ��
��  ����                ��
  ��  ��                ��
      ��        ��  ��    ��
      ��          ��      ��
      ��                  ��
        ��              ��
        ��  ����  ������
        ��  ����  ��
          ��    ��             ");

       break;
        }
    }
     else{
        outs("          ��������������
        ��          ������
      ��    ��    ��  ������
  ��������������        ����
  ��          ��          ��
  ��������������          ��            [1;5;31m�ڬO�j�ǳ�[m
  ��        ��            ��
  ������������            ��
  ��                    ��
  ��                    ��
    ��                ��
����  ��            ��
��      ������������  ����
  ��                      ��
������    �ڬO�j�ǳ�       ������ ");

   }
  if(clean>10)
    {
        move(10,30);
        outs("�K�K�n�h..���...");
       if(clean>15)
          death();
        pressanykey(NULL);
     }

   move(17,0);
   outs("[32m[1]-�Y��    [2]-�Y�s��   [3]-�M�z����   [4]-��p���q��  [5]-�ثe���Z[m");
   outs("\n[32m[6]-�R���}��$20 [7]-�R�s��$30 [8]-�Y�j�ɤY [9]-�R�j�ɤY$50 [0]-������[m");
  /*pressanykey(NULL);*/
  return;
}

int select_menu()
{
  extern struct commands chiclist[];
  char inbuf[80];
  int diff;
  struct tm *ptime;
  time_t now;

  time(&now);
  ptime = localtime(&now);
  diff = (time(0) - login_start_time) / 60;

  while(1){
      if(check_multigame()) return 0;
   getdata(t_lines-1, 0, "�n���Ǥ���O?�G[0]", inbuf, 4, DOECHO,NULL,YEA);
        if(tiredstrong>(20+age) && inbuf[0]!='9' && inbuf[0]!='0' &&inbuf[0]!='8' )
          {
           clear();
           move(15,30);
           outs("��~~~�p���|���a��...�n���h�𮧤@�U..");
           refresh(); sleep(1);
           move(17,30);outs("��    �i     ��");
           pressanykey(NULL);
           return;
          }
   switch(inbuf[0])
   { case '1':
        if (food<=0){
         pressany(5);
         break;}
        move(10,0);
        ate+=2;
        outs("       �b�b�b�b���b
         ��� �i  �m
              �o  �m                             �m�m�m�m  �m
              �m  �m     ��              ��      �m�m�m�m�m�m�m
         �Ѣ���� �m    _�c�c�c�c�c�c�c�c�c_    ����������������
             �m   �m     �H�H�H�H�H�H�H�H�H       �~�w���w��
            �o    �m     �g�g�g�g�g�g�g�g�g       �xMcdonald�x      �@�@�@�@
           �i     �m     �������������������@     ���w�w�w�w��
       ���g�g�g�g��      ���i�i�i�i�i�i�i��     ���i�i�i�i�i�i�� ");

        pressany(0);
        food--;
        tiredstrong++;
        satis++;
        if(age<5)
          weight=weight+(5-age);
        else
          weight++;
        if(weight>100)
          {move(9,30);
           outs("�ӭ��F��..����~~");
           pressanykey(NULL);
           }
        if(weight>150)
          {move(9,30);
           outs("�����w�F~~");
           pressanykey(NULL);
           }
        if(weight>200)
           death();
                /* if(weight>100)
                    {  move(15,40);
                       outs("�A�Q�������ڡH....�z�������ѡ�");
                       pressanykey(NULL);
                       food=100;
                      }*/
        break;
     case '2':
        if (zfood<=0){
         pressany(5);
         break;}
        move(10,0);
        ate++;
        outs("             ��
       [33;1m��[m�~��
       [37;42m����[m
       [32m�i�i[m
       [32;40;1m�i�i[m
       [31m �i [m
      [31m �v�v[m   [32;1m���G�s�B�N�OĬ��[m   ��!�n��!   ");
        pressany(1);
        zfood--;
        tiredstrong++;
        happy++;
        weight+=2;
        if(weight>100)
          {move(9,30);
           outs("�ӭ��F��..����~~");
           pressanykey(NULL);
           }
        if(weight>200)

          death();
        break;
     case '3':
        move(10,0);
        outs("[1;36m                              ����������[m
[1;33m                             �y[37m�i�i�i�i[m
[1;37m                               �i�i�i�i[m
[1;37m             �b�b�b�b�b�b�b�b[32m��[37m�i�i�i�i[m
[37m             �g�g�g�g�g�g�g�g�p[1;37m�i�i�i�i[m
[37m             ������������������[1;33m ��[m
[36m                  ���i�i�i�i�i[1;33m����[m
[1;36m                  ������������[m
  [1;37m                �v�v�v�v�v�v[m
                  �C�C�C...�K�K�ԥ���...                              ");

        pressany(2);
        tiredstrong+=5;
        clean=0;
        break;
     case '4':
        guess();
        satis+= (ptime->tm_sec%2);
        tiredstrong++;
        break;
     case '5':
        old_situ();
        break;
     case '6':
          move(t_lines-3,0);
        if(currentuser.money<20)
          {    outs("�}�G����!!");
        pressanykey(NULL);
        break;}
        food+=5;
        prints("\n������ [33;41m %d [m��",food);
        prints("   �ѤU [33;41m %d [m�}",demoney(m_calcvip(20)));
        pressanykey(NULL);
        break;
     case '7':
          move(t_lines-3,0);
        if(currentuser.money<30)
          {    outs("�}�G����!!");
        pressanykey(NULL);
        break;}
        zfood+=5;
        prints("\n�s���� [33;41m %d [m��",zfood);
        prints("  �ѤU [33;41m %d [m�}",demoney(m_calcvip(30)));
        pressanykey(NULL);
        break;
     case '8':
       move(t_lines-2,0);
       if(oo<=0)
          {
        outs("�S�j�ɤY��!!");
        pressanykey(NULL);
        break;}
        move(10,0);
        outs("
               ���i�i��
               �i�i�i�i
               ���i�i��
                          ���Y�T�Ĥj�ɤY.....");
        tiredstrong = 0;
        happy+=3;
        oo--;
        pressany(6);
        break;
     case '9':
  move(t_lines-3,0);
          if(currentuser.money<50)
           {    outs("�S���F��!!");
           pressanykey(NULL);
          break;}
          oo++;
          prints("\n�j�ɤY�� [33;41m %d [m��",oo);
          prints("  �ѤU [33;41m %d [m��",demoney(m_calcvip(50)));
           pressanykey(NULL);
         break;
     case '0':
               move(t_lines-2,0);
        if(age<5)
        {prints("�Ӥp�F...���o�c�⥼���~�p��.....");
          pressanykey(NULL);
          break;
        }
        sell();
        break;
     case 'k':
        death();
        break;
     case 't':
        tiredstrong = 0;
        break;
     case 'c':
        getdata(t_lines-1, 0, "���p�����Ӧn�W�r�G", Name, 21, DOECHO,NULL,YEA);
        break;
     default:
        return;
     break;
    }
    show_chicken();
   }
  return;
}

int death()
{
  char fname[50];
  FILE *fp;
  struct tm *ptime;

      time_t now;

        time(&now);
          ptime = localtime(&now);
  clear();
  move(5,0);
  clrtobot();
  if((fp = fopen("etc/chicken", "a"))!=NULL);
     /*fp=fopen("etc/chicken,"ab");*/
fprintf(fp,"[32m%s[m �b [34;43m[%d/%d  %d:%02d][m  ���p�� [33m%s  [36m���F~~[m \n",
                 currentuser.userid,ptime->tm_mon + 1, ptime->tm_mday,
                           ptime->tm_hour, ptime->tm_min,Name);
                             fclose(fp);
    outs("��...�p�����F....");
  outs("\n�¥v�F...���X�t��...");
  pressanykey(NULL);
  setuserfile(fname, DATA_FILE);

  unlink(fname);
/*  strcpy(Name,"");
  creat_a_egg();
  chicken_main();*/
  abort_bbs();

}


int
pressany(i)
{
  int ch;
  move(t_lines,0);
  prints("[33;46;1m                           [34m%s[37m                         [0m",cstate[i]);
  do
  {
    ch = egetch();
//    if (ch == KEY_ESC && KEY_ESC_arg == 'c')
//       buf2file();
  } while ((ch != ' ') && (ch != KEY_LEFT) && (ch != '\r') && (ch != '\n'));
  move(t_lines, 0);
  clrtoeol();
  refresh();

}

int guess()
{
   int ch,com;
   struct tm *qtime;
   time_t now;

   time(&now);
   qtime = localtime(&now);

   do
   {
    /*getdata(t_lines-1, 0, "[1]-�ŤM [2]-���Y [3]-���G", inbuf, 4,
DOECHO,NULL);*/
    move(t_lines-1,0);
    clrtoeol();
    outs("[1]-�ŤM [2]-���Y [3]-���G");
    ch = igetkey();
    }while((ch!='1')&&(ch!='2')&&(ch!='3'));

   /* com=qtime->tm_sec%3;*/
    com=rand()%3;
    move(t_lines-4,35);
    switch(com){
        case 0:
          outs("�p��:�ŤM");
         break;
        case 1:
          outs("�p��:���Y");
         break;
        case 2:
          outs("�p��:��");
         break;
     }

    move(t_lines-4,0);


    switch(ch){
    case '1':
      outs("����---�ݧھߨӪ��M---");
      if (com==0)
        old_tie();
     else  if (com==1)
        old_lose();
     else if (com==2)
        old_win();
    break;
    case '2':
      outs("�b��---�{�A�@�����Y!!---");
      if (com==0)
        old_win();
     else if (com==1)
        old_tie();
     else if (com==2)
        old_lose();
    break;
    case '3':
      outs("����---�e�A�@��}��!---");
      if (com==0)
        old_lose();
     else if (com==1)
        old_win();
     else if (com==2)
        old_tie();
    break;
  }
  /* sleep(1);*/
   pressanykey(NULL);

}

int old_win()
{
        winn++;
       /* sleep(1);*/
        move (t_lines-3,0);
        outs("�P�w:�p����F....    >_<~~~~~

                                 ");
        return;
}
int old_tie()
{
       /* sleep(0);*/
         move (t_lines-2,0);
        outs("�P�w:����                    -_-

                                              ");
        return;
}
int old_lose()
{
        losee++;
        happy+=2;
        /*sleep(0);*/
         move (t_lines-2,0);
        outs("�p��Ĺ�o                      ���
                               ��       ");
        return;
}

int old_situ()
{

        move(16,0);
        outs("           ");
        move(17,0);
        prints("�A:[44m %d�� %d�t[m                   ",winn,losee);
        move(18,0);
        prints("��:[44m %d�� %d�t[m                   ",losee,winn);

       if (winn>=losee)
        pressany(4);
       else
        pressany(3);

       return;
}
void
p_bf()
{
  FILE *fp;
  char fname[50];

  move(t_lines-2,0);
  if(currentuser.money<30){
    outs("�}�G����!!");
        pressanykey(NULL);
        return;}
     setuserfile(fname, "chicken");
  if ((fp = fopen(fname, "r+")) == NULL)
   {
      outs("�S�i��..�����A�R..����...");
       pressanykey(NULL);
        return;
    }
   else{
        fp = fopen(fname, "r");
        fscanf(fp,"%d %d %d %d %d %d %d %d %d %d %s %d %d"
,&weight,&mon,&day,&satis,&age,&oo,&happy,&clean,&tiredstrong,&play,Name
         ,&winn,&losee);
        fclose(fp);
      oo++;
      prints("\n�j�ɤY�� %d ��",oo);
      prints("  �ѤU %d �}",demoney(m_calcvip(50)));
      pressanykey(NULL);
    fp = fopen(fname, "r+");
  /*if (last!=1)
    { */
        fprintf(fp,"%d %d %d %d %d %d %d %d %d %d %s %d %d"
          ,weight,mon,day,satis,age,oo,happy,clean,tiredstrong,play,Name
          ,winn,losee);
  fclose(fp);
    }
        return;
}


int year(useri)
{
  FILE *fp;
  char fname[50];
        getuser(useri);
      sethomefile(fname, useri, "chicken");
  if ((fp = fopen(fname, "r+")) == NULL)
   {
       return NULL;
    }
        fscanf(fp,"%d %d %d %d %d %d %d %d %d %d %d %d %d %d %s "
         ,&weight,&mon,&day,&satis,&age,&oo,&happy,&clean,&tiredstrong,&play
         ,&winn,&losee,&food,&zfood,Name);
        fclose(fp);
  return age;

    }
int sell()
{
  int sel=0;
  char ans[5];
  struct tm *ptime;
  FILE *fp;
    time_t now;

      time(&now);
        ptime = localtime(&now);

  getdata(t_lines-2, 0, "�T�w�n�汼�p��?[y/N]",ans,3,DOECHO,NULL,YEA);
  if(ans[0]!='y') return;
  sel+=(happy*10);
  sel+=(satis*7);
  sel+= ((ptime->tm_sec%9)*10);
  sel+= weight;
  sel+=age*10;
  if(sel > MAX_SALE_VALUE) sel=MAX_SALE_VALUE+(sel-MAX_SALE_VALUE)*0.1;
  move(t_lines-3,0);
  prints("�p����[33;45m$$ %d [m�}�}",sel);
    getdata(t_lines-4, 0, "�u���n�汼�p��?[y/N]",ans,3,DOECHO,NULL,YEA);
  if(ans[0]!='y') return;

  if((fp = fopen("etc/chicken", "a"))!=NULL);
fprintf(fp,"[32m%s[m �b [34;43m[%d/%d  %d:%02d][m  ��p�� [33m%s  [31m�H [37;44m%d[m [31m�}�G��F[m\n",
                 currentuser.userid,ptime->tm_mon + 1, ptime->tm_mday,
                           ptime->tm_hour, ptime->tm_min,Name,sel);
                             fclose(fp);
  clear();
  if(check_multigame()) return 0;
  inmoney(sel);
  strcpy(Name,"");
  creat_a_egg();
  chicken_main();

}
int gagb()
{
  char buf[5],buf1[6];
  char ans[5];
  int i,k,flag[11],count=0,GET=0,q,ok=0;
  int l=1,money=0;

  srand(time(NULL));
    clear();
  do{
   /* while(strlen(buf1)<1)*/
  getdata(0, 0, "�n��h�ֿ}�G��(�̤j5000)�G", buf1, 5, DOECHO,NULL,YEA);
  if((money=atoi(buf1))<=0) return 0;
  set_safe_record();
     if(currentuser.money<money){
      outs("����$$");
      pressanykey(NULL);
      return 0;}
     }while((money<=0)||(money>5000));
  demoney(money);
  for(i=0;i<11;i++)
   flag[i]=0;
  for(i=0;i<4;i++){
   do{
     k=rand()%9;
     } while (flag[k]!=0);
   flag[k]=1;
   ans[i]=k+'0';
   }
  while(!GET)
  {
   ga(buf,l);
   if (buf[0]=='q'&&buf[1]=='k'){
     prints("�뭰..�q�F %d��",count);
     prints("\n���׬O:%s",ans);
     pressanykey(NULL);
    /*return 0*/;}
   if(buf[0]=='q'){
     prints("\n���׬O:%s",ans);
     pressanykey(NULL);
    return 0;}
   if(compare(ans,buf,count))
   /* GET=1;*/break;
   if(count>8){
     outs("[1;32m�z��..�q�Q���٤���...�}�}�S��..[m");
     pressanykey(NULL);
     return 0;}
   count++;
   l+=2;
  }
  count++;
  switch(count){
   case 1:
     money*=10;
     break;
   case 2:
   case 3:
     money*=6;
     break;
   case 4:
   case 5:
     money*=3;
     break;
   case 6:
     money*=2;
     break;
   case 7:
     money*=1.5;
     break;
   case 8:
     break;
   default:
     money/=2;
     break;}
   inmoney(money);

  prints("\n�ש��F..�q�F[32m %d[m �� ��}�} [33;45m%d[m ��",count,money);
  pressanykey(NULL);

  return 0;
}

int compare(char ans[],char buf[],int c)
{
 int i,j,A,B;

 A=0;
 B=0;
 for(i=0;i<4;i++)
  if(ans[i]==buf[i])
  A++;
 for(i=0;i<4;i++)
  for(j=0;j<4;j++)
   if((ans[i]==buf[j])&&(i!=j))
    B++;
 prints("%s",buf);
 prints("  ���G: %d A %d B �� %d ��\n",A,B,9-c);
 /*  pressanykey(NULL); */
  if(A==4)
   return 1;
 else
   return 0;
}

int ga(char buf[],int l)
{
  int q,ok=0;

  getdata(l, 0, "��J�Ҳq���Ʀr(�|�줣����)�G", buf, 5, DOECHO,NULL,YEA);
  if (q=(strlen(buf))!=4){
       if (buf[0]=='z'&&buf[1]=='k')
   return 0;
       if (buf[0]=='q')
   return 0;
    outs("�è�..����4��");
   /* pressanykey(NULL);*/
    return 0; }
  if((buf[0]!=buf[1])&&(buf[0]!=buf[2])&&(buf[0]!=buf[3])
   &&(buf[1]!=buf[2])&&
     (buf[1]!=buf[3])&&(buf[2]!=buf[3])) ok=1;
   if(ok!=1){
    outs("�����o");
    /*pressanykey(NULL);*/
    return 0;}

  return 0;
}
char *nam(useri)
{
  FILE *fp;
  char fname[50];
        getuser(useri);
      sethomefile(fname, useri, "chicken");
  if ((fp = fopen(fname, "r+")) == NULL)
   {
       return NULL;
    }
        fscanf(fp,"%d %d %d %d %d %d %d %d %d %d %d %d %d %d %s "
         ,&weight,&mon,&day,&satis,&age,&oo,&happy,&clean,&tiredstrong,&play
         ,&winn,&losee,&food,&zfood,Name);
        fclose(fp);
  return Name;

    }


  int gold,x[9],ran,q_mon,p_mon;
  unsigned long int bank;
  char buf[1],buf1[6];

int mary_m()
{
   FILE *fp;

   memset(x,0,9*sizeof(int));
     if ((fp = fopen("etc/mary", "r")) == NULL){
            fp = fopen("etc/mary", "w");
                 fprintf(fp,"%ld",1000000);
            fclose(fp);}
     fp = fopen("etc/mary", "r");
     fscanf(fp,"%ld",&bank);
     fclose(fp);
   clear();
   clrtobot();
   p_mon=0;
   q_mon=currentuser.money;
   srandom(time(NULL)); // ��Ӫ����S��l�ơA���G�y���@��j�I��..:Q
   show_m();

   fp = fopen("etc/mary", "r+");
   fprintf(fp,"%ld",bank);
   fclose(fp);
   return;
}

int show_m()
{
   int i,j,k,m;

   move(0,0);
   clear();
   outs("              �š�    �š�
            ��    �@��    �@
           �U ���� �U ���� �U
            �@___���@�@�šš�
            �x  ___  ___  �x
          �]�x�y_���y_��  �x�^
        (~~.�x   �@�@��    �x.~~)
       `�@�� �@    ��    �� �@��
   �@     �@   �@�ššš�   ��
   �@       �@���U �U �U�@��
     �@      �x  �v�ݢv  �x
     �@     �y___���ݢ@___�j
       �@      �@__�U__��    [31;40m�w����{�p����..[m");

      move(t_lines-10,0);
   prints("�{���}�G: %-d            �����O���{��: %-d",q_mon,bank);
   move(t_lines-9,0);


prints("[36m�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w[m\n");

   prints("ī�G-1 bar-2  777-3  ���a-4 BAR-5  �a�L-6 ���-7 ��l-8 ��K-9\n");
   prints("x5     x40    x30    x25    x50    x20    x15    x10    x2��\n");
   prints("%-7d%-7d%-7d%-7d%-7d%-7d%-7d%-7d%-7d\n",x[0],x[1],x[2],x[3],x[4],x[5],
           x[6],x[7],x[8]);

prints("\n[36m�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w��a�����w�w�w�w��s�}�l�w�w��q���}�w�w[m");
   getdata(t_lines-3, 0, "�n��X��(�i��h��)�G", buf, 2, DOECHO,NULL,YEA);
   if(!buf[0]) return;
   switch(buf[0]){
   case 's':
        doit();
        return;
        break;
   case 'a':
        getdata(t_lines-2, 0, "�n��h�ֿ}�G", buf1, 8, DOECHO,NULL,YEA);
        for(i=0;i<=8;i++)
         x[i]=atoi(buf1);
         j=(x[0]*9);
         j=abs(j);
        if(q_mon<j)
          {outs("�}�G����");
           pressanykey(NULL);
           for(i=0;i<=8;i++)
                x[i]=0;
           show_m();
           return;
           j=0;
           }
     /*    demoney(j);*/
        q_mon-=j;
        p_mon+=j;
 /*       strcpy(buf," ");*/
        show_m();
        return;
        break;
   case 'q':
     for(i=0;i<=8;i++)
      x[i]=0;
        return;
        break;
   case 't':
        m=10000000;
        for(i=1;i<=5;i++){
         clear();
         move(20,i);
         outs("x");
         if(i%3==0)
          m*=10;
          for(j=1;j<=m;j++)
          k=0;

          refresh();

         }
        return;
        break;
   default:
   i=atoi(buf);
        break;
   }
   k=x[i-1];
   do{
   getdata(t_lines-2, 0, "�n��h�ֿ}�G", buf1, 8, DOECHO,NULL,YEA);
   x[i-1]+=atoi(buf1);
        j=atoi(buf1); }while(x[i-1]<0);

/*   j=x[i-1];*/
   if(j<0)
        j=abs(j);
   if(q_mon<j)
        {outs("�}�G����");
         pressanykey(NULL);
         x[i-1]=k;
         clear();
         j=0;}
      q_mon-=j;
      p_mon+=j;
  /* demoney(j);*/
   show_m();
   return;
}

int doit()
{
   int i,j,k,m,seed,flag=0,flag1=0;
   int g[10]={5,40,30,25,50,20,15,10,2,0};

   clear();
   move(0,0);
   outs("
                       �i�i�i�i�i�i�i�i�i�i�i
                       �i                  �i
                       �i                  �i
                       �i �ܢעܢ�ӢܢҢ� �i
                       �i  ���b���      �i
                       �i      �ۦ�Q��    �i
                       �i                  �i
                       �i�i�i�i�i�i�i�i�i�i�i
                              NINTENDO

                          ��
                        ������           ��
                          ��          ��
                               �b  �b    .....
                                        .......
                                        .....��
                                                              ");

        m=1000000;
        for(i=1;i<=30;i++){
//         clear();
         move(10,i-1);
         outs("  ");
         move(10,i);
         outs("��");
         if(i%23==0)
          m*=10;
          for(j=1;j<=m;j++)
          k=0;

          refresh();

         }
   demoney(p_mon);
   refresh();
   sleep(1);
   clear();
   move(10,31);
   gold=0;
   seed=1000;
   if(p_mon>=50000)
     seed=621;

   do{
   ran=(random()+currentuser.money)%seed;
   flag1=0;

   move(10,31);
   if(ran<=299)
     {  outs("��K");
        j=8;}
    else if(ran<=419){
        outs("ī�G");
        j=0;}
    else if(ran<=479){
        outs("��l");
        j=7;}
    else if(ran<=519){
        outs("���");
        j=6;}
    else if(ran<=549){
        outs("�a�L");
        j=5;}
    else if(ran<=573){
        outs("���a");
        j=3;}
    else if(ran<=593){
        outs("777!");
        j=2;}
    else if(ran<=608){
        outs("bar!");
        j=1;}
    else if(ran<=625){
        outs("BAR!");
        j=4;}
    else {
      /*  outs("test          ���h�k��  �A�]�@��\n");
        for(i=4;i<=8;i++)*/
          outs("���´f�U");
       /* for(i=0;i<=8;i++)
         x[i]=0;*/
        j=9;
          }
   gold=x[j]*g[j];
   if(!flag)
    if(gold>=10000){
       flag=1;
       flag1=1;
     }
            /*    } while( ran>976 || flag1 );*/
                  } while(flag1);
   refresh();
   sleep(1);
   move(11,25);
   prints("[32;40m�A�i�o[33;41m %d [32;40m�}�}[m",gold);
   refresh();
   if (gold>0){
      bank-=gold;
      bank+=p_mon;
      }
   else
      bank+=p_mon;

   inmoney(gold);
   pressanykey(NULL);
   for(i=0;i<=8;i++)
   x[i]=0;
   p_mon=0;
   q_mon=currentuser.money;

   show_m();
   return;
}
