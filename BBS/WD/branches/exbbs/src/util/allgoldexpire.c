
#include "bbs.h"
#include "bbs_util.h"
#include "record.c"
#include "cache.c"

#define pwdfile fn_passwds
#define taxfile BBSHOME"/log/counter/tax_income"

#define gold_lower_bound 50	// 低於 50 金者不扣稅
#define exp_lower_bound 1000    
#define tax_rate_a 500		// 稅率 A  
#define tax_rate_b 50		// 稅率 B  百分之１金 (太久沒上站者)

#define CHECK_LEVEL(x,level)     ((x)?level&(x):1)

userec auser;
userec xuser;

int getuser(userid)
  char *userid;
{
  int uid;
  if (uid = searchuser(userid))
    rec_get(pwdfile, &xuser, sizeof(xuser), uid);
  return uid;
}

int taxtax(char *tuser,int gold, int exp)
{
  int unum;
  if (unum = getuser(tuser))
    {
      if(xuser.goldmoney<gold)
        xuser.silvermoney -= (10000 * (gold-xuser.goldmoney));
      xuser.goldmoney -= gold;
      xuser.exp -= exp;
      if(xuser.goldmoney<0) xuser.goldmoney=0;
      if(xuser.exp<0) xuser.exp=0;
      substitute_record(pwdfile, &xuser, sizeof(userec), unum);
      return xuser.goldmoney;
    }
  return -1;
}


int
bad_user_id(userid)
  char *userid;
{
  register char ch;
  if (strlen(userid) < 2)    return 1;
  if (not_alpha(*userid))    return 1;
  while (ch = *(++userid))
    if (not_alnum(ch))      return 1;
  return 0;
}

int main(int argc, char *argv[])
{
  FILE *inf;
  time_t now;
  int i, j=0, t=0, expire, p_money, p_exp, total=0, usermoney, userexp;
  char u='s', l='u', mode[10], name[20];
                                                                                
  inf = fopen(pwdfile,"rb");   
                                                                           
  if (inf == NULL)
  {
    printf("Sorry, user data is not ready.\n");
    exit(0);
  }

  if (argc != 2)
  {
     printf("\nusage: allgoldexpire mode \n\n");
     printf(" -l: list all users who have more than 100 golds\n");
     printf(" -e: list users who need to degold\n");
     printf(" -g: get tax from all users and degold!\n\n");
     exit(0);
  }
   else
     strcpy(mode,argv[1]);

  time(&now);
  for (i = 0; fread(&auser, sizeof(userec), 1, inf); i++)
  {
    if (bad_user_id(auser.userid) || strchr(auser.userid,'.') )
        i--;
    else
    {
       t=now-auser.lastlogin;
       p_exp=0; p_money=0; expire=0;
       usermoney=auser.goldmoney + auser.silvermoney/10000;
       userexp=auser.exp;
       
       u='s';
       if(t>60) {          t=t/60;          u='m';       }
       if(t>60) {          t=t/60;          u='h';       }
       if(t>24)
       {
          t=t/24;
          u='d';
          if (t>=7)   expire=1;		// 超過7天加倍
          if (t>=28)  expire=2;		// 超過28天再加倍
          if (t>=56)  expire=3;		// 依此類推
          if (t>=112) expire=4;
          if (t>=224) expire=5;
          if (t>=336) expire=7;
       }
       l='u';
       if CHECK_LEVEL(PERM_BM, auser.userlevel) l='b';
       if CHECK_LEVEL(PERM_BBSADM, auser.userlevel) l='a';
       if CHECK_LEVEL(PERM_SYSOP, auser.userlevel)
       {
          expire=0;      l='s';
       }

       strcpy(name, auser.userid);

       p_exp=1 + (auser.exp/tax_rate_a);
       p_money=1 + (usermoney/tax_rate_a);
       
       if(expire>0)
       {
          p_exp=p_exp + (expire * (auser.exp/tax_rate_b));
          p_money=p_money + (expire * (usermoney/tax_rate_b));
       }
       if (auser.exp < exp_lower_bound) p_exp=0;
       if (usermoney < gold_lower_bound) p_money=0;

       if (strcmp(mode, "-l")==0)
          printf("%04d.%-12s %c g:%-10d e:%-10d pm:%-5d pp:%5d t:%5d%c\n",
             ++j, name, l, usermoney, userexp, p_money, p_exp, t, u);
       else
       {
          if((expire>0) && (p_money>0))
          {
             printf("%04d. %12s (%c) %5d 天沒上站了，徵收稅金：%5d枚\n",
                ++j, name, l, t, p_money);
          }
          if((strcmp(mode, "-g")==0))
          {
             total = total + p_money;
             taxtax(name, p_money, p_exp);
          }
       }
    }

  }
  fclose(inf);

  t=0;  
  inf = fopen(taxfile, "r");  /* 稅收記錄檔  累積稅收金幣 -> 國庫 */
  
  if (inf != NULL)
  {
     fscanf(inf,"%d",&t);
     fclose(inf);
  }
  t = t + total;
  
  inf = fopen(taxfile,"w+");
  fprintf(inf,"%d",t);
  fclose(inf);
  
  return 1;
}
