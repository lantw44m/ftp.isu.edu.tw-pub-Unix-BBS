
/* �ϥο������ */

#include "bbs.h"

/* ������ */
unsigned setuperm(pbits,nb,money)  /* �R�v���� */
  unsigned pbits;
  char nb;
  int money;
{
  register int i;
    i = nb - 'a';
    if(!((pbits>>i)&1))
    {
     pbits ^= (1 << i);
     degold(money);
    }
    return(pbits);
}

/*�R��O��*/
void
tradelog(userid,i)
char *userid;
int i;
{
  time_t now = time(0);
  char genbuf[200];
  char *item[5]={"�H�c�W�����@","�׷������j�k","�G�m�ק��_��","�H��L�W��","�z����"};
  now = time(NULL) - 6 * 60;
  sprintf(genbuf,"�b %s �ϥΪ� [1;32m%s[m �ʶR�F[1;36m%s[m���v��",Cdate(&now),cuser.userid,item[i]);
  f_cat("log/trade.log",genbuf);
}

void
p_cloak()
{
  char buf[4];
  move(b_lines-2,0);
  if(check_money(BUY_C,GOLD))      return;
  prints("���B�G%d    ",BUY_C); // cost
  getdata(b_lines-1, 0,
    (currutmp->invisible==1) ? "�T�w�n�{��?[y/N]" : "�T�w�n����?[y/N]",buf,3,LCECHO,"N");
  if(buf[0]!='y')    return;
  degold(BUY_C);

  if(currutmp->invisible & 1)
    currutmp->invisible &= ~1;
  else
    currutmp->invisible |= 1;
            
  pressanykey((currutmp->invisible==1) ? MSG_CLOAKED : MSG_UNCLOAK);
  return;
}

void
p_scloak()
{
  char buf[4];
  move(b_lines-2,0);
  if(check_money(BUY_C,GOLD))      return;
  prints("���B�G%d    ",BUY_S); // cost
  getdata(b_lines-1, 0,"�T�w�n�ʶR�z����? (y/N) ",buf,3,LCECHO,"N");
  if(buf[0]!='y')    return;
  degold(BUY_S);
  currutmp->invisible |= SEE;
  pressanykey(MSG_SEECLOAK);
  return;
}


void
p_fcloak()
{
  register int i;
  char ans[4];
  if(check_money(BUY_SC,GOLD) || HAS_PERM(PERM_CLOAK))
  {
    if(HAS_PERM(PERM_CLOAK))
      pressanykey("�A�w�g�i�H���ΤF�٨ӶR�A�����Ӧh�ڡH");
    return;
  }
  move(b_lines-3,0);
  prints("���B�G%d    ",BUY_SC); // cost
  getdata(b_lines-2, 0, "�T�w�n�ʶR�׷������j�k�H[y/N]",ans,3,LCECHO,0);
  if(ans[0]!='y')
    return;
  rec_get(fn_passwd, &xuser, sizeof(xuser), usernum);
  i=setuperm(cuser.userlevel,'g',BUY_SC);
  update_data();
  cuser.userlevel=i;
  substitute_record(fn_passwd, &cuser, sizeof(userec), usernum);
  tradelog(cuser.userid,1);
  pressanykey("���߱z�w�g�Ƿ|�F�׷������j�k!!");
  return;
}

void
p_from()
{
  char ans[4], genbuf[32];
  move(b_lines-3,0);
  if(check_money(BUY_F,GOLD)) return;
  prints("���B�G%d    ",BUY_F); // cost
  getdata(b_lines-2, 0, "�T�w�n��G�m?[y/N]",ans,3,LCECHO,"N");
  if(ans[0]!='y') return;
  if (getdata(b_lines-1, 0, "�п�J�s�G�m�G", genbuf, 17, DOECHO,0))
        {
           degold(BUY_F);
           strip_ansi(genbuf, genbuf, 0);
           strncpy(currutmp->from, genbuf, 24);
           currutmp->from_alias=0;
        }
  return;
}

void
p_ffrom()
{
  register int i;
  char ans[4];
  if(check_money(BUY_SF,GOLD) || HAS_PERM(PERM_FROM) || HAS_PERM(PERM_SYSOP))
  {
    if(HAS_PERM(PERM_FROM) || HAS_PERM(PERM_SYSOP))
      pressanykey("�A�w�g�i�H�ק�G�m�F�٨ӶR�A�����Ӧh�ڡH");
    return;
  }
  move(b_lines-3,0);
  prints("���B�G%d    ",BUY_SF); // cost
  getdata(b_lines-2, 0, "�T�w�n�ʶR�ק�G�m�_��H[y/N]",ans,3,LCECHO,0);
  if(ans[0]!='y') return;
    rec_get(fn_passwd, &xuser, sizeof(xuser), usernum);
  i=setuperm(cuser.userlevel,'t',BUY_SF);
  update_data();
  cuser.userlevel=i;
  substitute_record(fn_passwd, &cuser, sizeof(userec), usernum);
  tradelog(cuser.userid,2);
  pressanykey("���ߧA�o��F�G�m�ק��_��");
  return;
}

void
p_exmail()
{
  char ans[4],buf[100];
  int  n;
  if(cuser.exmailbox >= MAXEXKEEPMAIL )
  {
    pressanykey("�e�q�̦h�W�[ %d �ʡA����A�R�F�C", MAXEXKEEPMAIL);
    return;
  }
  move(b_lines-3,0);
  prints("���B�G%d    ",BUY_M); // cost  
  sprintf(buf,"�z���W�� %d �ʮe�q�A�٭n�A�R�h��?",cuser.exmailbox);
  getdata(b_lines-2, 0, buf,ans,3,LCECHO,"10");
  n = atoi(ans);
  if(!ans[0] || !n )
    return;
  if(n+cuser.exmailbox > MAXEXKEEPMAIL )
     n = MAXEXKEEPMAIL - cuser.exmailbox;
  if(check_money(n*BUY_M,GOLD))
    return;
  degold(BUY_M*n);
  inmailbox(n);
  return;
}

/*
void
p_ulmail()
{
  register int i;
  char ans[4];
  if(check_money(100000,GOLD) || HAS_PERM(PERM_MAILLIMIT))
  {
    if(HAS_PERM(PERM_MAILLIMIT))
      pressanykey("�A���H�c�w�g�S������F�٨ӶR�A�����Ӧh�ڡH");
    return;
  }
  getdata(b_lines-2, 0, "�T�w�n�� $100000 �ʶR�L�W���H�c?[y/N]",ans,3,LCECHO,0);
  if(ans[0]!='y')
    return;
  rec_get(fn_passwd, &xuser, sizeof(xuser), usernum);
  i=setuperm(cuser.userlevel,'f',100000);
  update_data();
  cuser.userlevel=i;
  substitute_record(fn_passwd, &cuser, sizeof(userec), usernum);
  tradelog(cuser.userid,3);
  pressanykey("���߱z�w�g�o��F�L�W�����H�c!!");
  return;
}
*/

void
p_give()
{
   int mymoney, money, mode=0, rc, rate=50;
   char id[IDLEN+1],buf[256],reason[60];
   FILE *fp=fopen("tmp/givemoney","w");
   fileheader mymail;
   time_t now;
   
   time(&now);
   move(15,0);
   update_data();
   usercomplete("  �п�J��誺ID�G", id);
   if (!id[0] || !ci_strcmp(id,"guest"))
      return;
      
   getdata(17, 0, "  0. �ȹ�  1.�����G",buf, 2, LCECHO, "0");
   if(buf[0]=='1') mode=1;
   if(buf[0]=='0') mode=0;

   getdata(19, 0, "  �n��h�ֹL�h�H", buf, 9, LCECHO,0);
   
   mymoney = atoi(buf);
   if(mymoney<100)
   {
     pressanykey("��p�A�C����b������ƭȤU����100");
     return;
   }
   
   if(check_money(mymoney, mode)) return;

   money = mymoney - mymoney/rate;

   if(mode==1)
   {
     degold(mymoney);
     rc=inugold(id, money);
   }
   else
   {
     demoney(mymoney);
     rc=inumoney(id, money);
   }
   
   if(rc>0)
   {
     sprintf(buf,"�@��: %s \n"
                 "���D:[��b�q��] �e�A %d �T %s �I\n"
                 "�ɶ�: %s\n",
                 cuser.userid,
                 money,
                 (mode ? "����" : "�ȹ�"),
                 ctime(&now));
     fputs(buf,fp);
     while(!getdata(21,0,"  �п�J�@�Ӳz�ѡG",reason,60,DOECHO ,"���Ӧh"));
     
     sprintf(buf,"[1;32m%s[37m ��b [33m%d [37m �T %s ��A���b��̡C\n"
                 "�L���z�ѬO�G[33m %s [m",
                 cuser.userid,
                 money,
                 (mode ? "����" : "�ȹ�"),
                 reason);
     fputs(buf,fp);

     sprintf(buf,"home/%s/mailbox", id);
     stampfile(buf, &mymail);
     strcpy(mymail.owner, cuser.userid);
     rename ("tmp/givemoney",buf);
     
     sprintf(mymail.title,"[��b�q��] %d �T %s �i�b�F�I",
     money,
     (mode ? "����" : "�ȹ�")) ;
     sprintf(buf,"home/%s/mailbox/.DIR",id);
     rec_add(buf, &mymail, sizeof(mymail));
     sprintf(buf,"[1;33m%s %s [37m�� %s [33m%d �T [37m��b��[33m %s[37m",
     Cdate(&now),
     cuser.userid,
     (mode ? "����" : "�ȹ�"),
     money,
     id);
     f_cat("log/bank.log",buf);
   }
   else
   {
     if(mode==1)
       ingold(mymoney);
     else
       inmoney(mymoney);
       
     pressanykey("��b����!");
   }
   fclose(fp);
   return;
}

void
water_exchange()
{
  char buf[128], ans[10];
  int Money, rate=1;
//  time_t now = time(0);

  move(13,0);
  clrtobot();
  prints("  �A�����쪺���y�� %d ��\n\n",cuser.receivemsg);
  prints("  ����קI��v�G  ���y�G�ȹ� = 1�G%d\n",rate);

  getdata(17,0,"  �n���h�֤��y�Ӵ��H ",ans,10,LCECHO,0);
  if(!ans[0]) return;
  
  Money = atol(ans);
  if(Money<1 || Money > cuser.receivemsg) return;
  
  sprintf(buf,"  �O�_�n�N���y %d �� �����ȹ� %d �T ? [y/N] ",
          Money, Money);
  
  getdata(19,0,buf,ans,2,LCECHO,"y");
  
  if(ans[0] == 'y')
  {
    rec_get(fn_passwd, &xuser, sizeof(xuser), usernum);
    cuser.receivemsg = cuser.receivemsg - Money;
    substitute_record(fn_passwd, &cuser,sizeof(userec), usernum);
    inmoney(Money);
    pressanykey("OK! �z�ثe���ȹ��� %d �T", cuser.silvermoney);
  }
  else
    pressanykey("����.....");
  
}


void
exchange()
{
  char buf[100],ans[10];
  int i, mymoney, money, rate=50;
  time_t now = time(0);

  move(13,0);
  clrtobot();
  prints("  �A���W������ %d ��,�ȹ� %d ��\n\n",cuser.goldmoney,cuser.silvermoney);
  prints("  ����קI��v�G  �����G�ȹ� = 1�G10000\n");
  
  if(!getdata(17,0,"  0.�ȹ�������  1.�������ȹ� ",ans,2,LCECHO,0)) return;
  if(ans[0] < '0' || ans[0] > '1') return;
  i = atoi(ans);
  while(1)
  {
    if(i == 0)
      getdata(19,0,"  �n���h�ֻȹ��Ӵ��H ",ans,10,LCECHO,0);
    else
      getdata(19,0,"  �n���h�֪����Ӵ��H ",ans,10,LCECHO,0);
    if(!ans[0]) return;
    mymoney = atol(ans);
    if(i == 0)
    {
       if(mymoney<10000 || mymoney > cuser.silvermoney)
          continue;
    }
    else
    {
       if(mymoney<=0 || mymoney > cuser.goldmoney)
          continue;
    }
    break;
  }
  
  if(i == 0)
     money = mymoney/10000;
  else
     money = mymoney*10000 - mymoney*10000/rate;
  
  if(i == 0)
    sprintf(buf,"  �O�_�n�N�ȹ� %d �T �������� %d �T ? [y/N]",mymoney,money);
  else
    sprintf(buf,"  �O�_�n�N���� %d �T �����ȹ� %d �T ? [y/N]",mymoney,money);
    
  getdata(21,0,buf,ans,2,LCECHO,0);
  if(ans[0] == 'y')
  {
    if(i == 0)
    {
      demoney(mymoney); ingold(money);
      sprintf(buf,"[1;36m%s %s [37m��ȹ� [33m%d �� [37m�ഫ������ %d ��",
        Cdate(&now),cuser.userid,mymoney, money);
    }
    else
    {
      degold(mymoney); inmoney(money);
      sprintf(buf,"[1;32m%s %s [37m����� [33m%d �� [37m�ഫ���ȹ� %d ��",
        Cdate(&now),cuser.userid,mymoney, money);
    }
    f_cat("log/bank.log",buf);
    pressanykey("OK! �z�ثe������ %d �T, �ȹ� %d �T",
                cuser.goldmoney,cuser.silvermoney);
  }
  else
    pressanykey("����.....");
}

/* ���w */
void
bank()
{
  char buf[10];
  if(lockutmpmode(BANK)) return;
  setutmpmode(BANK);
  stand_title("�q�l�Ȧ�");

  if (count_multi() > 1)
  {
    pressanykey("�z���ଣ�������i�J�Ȧ��� !");
    unlockutmpmode();
    return;
  }

  counter(BBSHOME"/log/counter/bank","�ϥλȦ�",0);
  move(2, 0);
  update_data();
  prints("%12s �z�n�r�I�w����{���Ȧ�C
[1;36m��������������������������������������������������������������
��[32m�z�{�b���ȹ�:[33m %-12d [32m�T�A����:[33m %-12d[32m �T[36m      ��
��[32m�z���ȹ��W��:[33m %-12d [32m�T�C[36m                           ��
��������������������������������������������������������������
�� �ثe�Ȧ洣�ѤU�C�T���A�ȡG                               ��",
    cuser.userid, cuser.silvermoney, cuser.goldmoney,
    (cuser.totaltime*1) + (cuser.numlogins*10) + (cuser.numposts*1000) + 10000 );
    move(7, 0);
      outs("\
��[33m1.[36m ��b�A��[37m�G1000�T�ȹ��H�W (����O�G 2%)                [36m ��
��[33m2.[33m �קI�A��[37m�G�ȹ� <--> ���� (����O�G 2%)                [36m ��
��[33m3.[32m �洫�A��[37m�G���y ---> �ȹ� (����O�G 0%)                [36m ��
��[33mQ.[37m ���}�Ȧ�[36m                                               ��
��������������������������������������������������������������[m");

  getdata(13,0,"  �п�J�z�ݭn���A�ȡG", buf, 2, DOECHO, 0);
  if (buf[0] == '1')
    p_give();
  else if (buf[0] == '2')
    exchange();
  else if (buf[0] == '3')
    water_exchange();

  update_data();
  pressanykey("���¥��{�A�U���A�ӡI");
  unlockutmpmode();
}

void
goldwish()
{
  int i,m;
//  time_t now = time(0);

  move(2,0);
  clrtobot();
  degold(1);

  i=rand() % 3000 - 2000;
  move(13,26);

  if (i>0)
     if (i%100 ==0)
     {
       m=i/100;
       cuser.goldmoney+=m;
       prints("�ѤU���U�Ӫ��� %d �T!",m);
     }
     else
     {
       cuser.exp+=i;
       prints("�g��ȤW�� %d �I!",i);
     }

  else
     if (i%10==0)
     {
       m = (cuser.goldmoney > (-i/10) ? (-i/10) : cuser.goldmoney);
       cuser.goldmoney-=m;
       prints("%d �T�����ܦ��F���Y...",m);
     }
     else
     {
       m = (cuser.exp > (-i/10) ? (-i/10) : cuser.exp);
       cuser.exp-=m;
       prints("�g��ȤU�� %d �I...",m);
     }
  substitute_record(fn_passwd, &cuser, sizeof(userec), usernum);

  pressanykey("�@��p�c�]�G���ߧA....�ܳܳ�...");
}

void
silverwish()
{
  int i;
//  time_t now = time(0);
  move(2,0);
  clrtobot();
  demoney(1000);
  i=rand() % 200 - 100;
  i= (i<0) ? 1 : i;
  move(13,26);

  if ((i+1)%25==0)
  {
    cuser.bequery = xuser.bequery + (i%2);
    prints("�H����ƤW�� %d �I!",(i%2));
  }
  else
  {
    cuser.exp = xuser.exp + i;
    prints("�g��ȤW�� %d �I!",i);
  }
  substitute_record(fn_passwd, &cuser, sizeof(userec), usernum);

  pressanykey("�@��p�c�]�G���ߧA....�ܳܳ�...");
}

void
darkwish()
{
  char buf[10];
  if(lockutmpmode(WISH)) return;
  setutmpmode(WISH);
  stand_title("�·t�\\�@��");

  if (count_multi() > 1)
  {
    pressanykey("�z���ଣ�������i�J�\\�@���@ !");

    unlockutmpmode();
    return;
  }

  counter(BBSHOME"/log/counter/darkwish","�·t�\\�@��",0);

  do{
  move(3, 0);
  prints("%12s �K...�z�n..�w��Ө� �·t�\\�@��...

  �ڬO[1;31;40m�@��p�c�][1;37;40m�A�u�n�z�֪�p���A���]�N�F���A���@��..

  �K�K�A�����ڬݬݧA���h�ֿ�... [1;33;40m���l[1;37;40m:%-12d [1;36;40m�Ȩ�[1;37;40m:%-12d",

  cuser.userid, cuser.goldmoney, cuser.silvermoney);
  move(9, 0);

  outs("
[1;33;40m  1. �@�T�����\\�@�@ -- �n�B�o�n�@�A�a�B�o�a�@�A ���a�z�����..�K�K..

[1;36;40m  2. �d�T�ȹ��o�@�@ -- �����]�n�A�A���n�L�a��..

[1;37;40m  Q. ���}

");
  getdata(16,0,"  �п�ܱz�n���a�A�K�K..�G", buf, 3, DOECHO, 0);
  if (buf[0] == '1' && cuser.goldmoney >= 1)
    goldwish();
  else if (buf[0] == '2' && cuser.silvermoney >= 1000)
    silverwish();
  else
      break;
//  update_data();
  }while(1);

  pressanykey("�K�K�K....�U���A�ӧr...");
  unlockutmpmode();
}

void
time_machine()
{
  char genbuf[128], pwdfile[128], keyword[128], ans[10];
  int id=0;

  if(lockutmpmode(TIME_MACHINE)) return;

  setutmpmode(TIME_MACHINE);
  stand_title("�·t�ɥ���");
  if (count_multi() > 1)
  {
    pressanykey("�z���ଣ�������i�J!");
    
    unlockutmpmode();
    return;
  }
  if (!HAS_PERM(PERM_LOGINOK))
  {
    pressanykey("�z���v������!");
    unlockutmpmode();
    return;
  }

  counter(BBSHOME"/log/counter/time_machine","�ɥ���",0);

  update_data();
  pressanykey("�ɥ����Ұʤ�...");
      
  ans[0]='1';
  while(ans[0]!='3')
  {
    stand_title("�·t�ɥ���");
    move(1,0);
    clrtobot();
    move(1,0);
    prints("


                      ����������������������������
                      ���ɥ����ާ@���          ��
                      ����������������������������
                      ��1. �ɥ��O��ï  $ 1 ��   ��
                      ��2. �ɥ��^����  $ 10��   ��
                      ��3. ���}                 ��
                      ����������������������������

");
    getdata(12,0,"  �п�J�z�ݭn���A�ȡG", ans, 2, DOECHO, 0);
    if(ans[0]=='1' && !check_money(1,1))
    {
       sprintf(pwdfile, "cat boards/System_Log/M*");
       getdata(14,0,"  �п�J�n�d�ߪ������r���@�G",genbuf, 30, DOECHO, 0);
       genbuf[30]='\0';
       sprintf(keyword, " | grep \"%s\"", genbuf);
       strcat(pwdfile, keyword);

       getdata(15,0,"  �п�J�n�d�ߪ������r���G�G",genbuf, 30, DOECHO, 0);
       genbuf[30]='\0';
       sprintf(keyword, " | grep \"%s\"", genbuf);
       strcat(pwdfile, keyword);

       sprintf(keyword, "tmp/%s.log", cuser.userid);
       sprintf(genbuf, "%s > %s", pwdfile, keyword);
       
       system(genbuf);
       more(keyword, YEA);
       degold(1);
    }
    else if(ans[0]=='2' && !check_money(10,0))
    {
      if(HAS_PERM(PERM_SYSOP))
      {
        getdata(14,0,"  �п�J�n�d�ߪ�ID�G",genbuf, IDLEN+1, DOECHO, 0);
        if(genbuf[0])
          strcpy(keyword, genbuf);
      }
      else
      {
        strcpy(keyword, cuser.userid);
      }
        
         getdata(15,0,"  �п�J�n�^��������G",genbuf, 5, DOECHO, 0);
         genbuf[5]='\0';
         sprintf(pwdfile,"backup/passwds_%s", genbuf);
         if(dashs(pwdfile)==-1)
         {
           pressanykey("�S�����O���ɮ�!");
         }
         else
         {
           id = searchuser(keyword);
           rec_get(pwdfile, &xuser, sizeof(userec), id);
           move(1,0);
           user_display(&xuser, 1);
           pressanykey(NULL);
           demoney(10);
         }
         
    }
  }   
  
  pressanykey("�ɥ���������...");    
  unlockutmpmode();
  return;
}
