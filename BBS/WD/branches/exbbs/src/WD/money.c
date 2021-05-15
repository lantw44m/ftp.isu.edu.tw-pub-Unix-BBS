/*-------------------------------------------------------*/
/* money.c       ( WD-BBS Version 1.54 )                 */
/*-------------------------------------------------------*/
/* target : money control function                       */
/* create : 98/08/16                                     */
/* update : 99/03/05                                     */
/*-------------------------------------------------------*/

#include "bbs.h"

int
inumoney(char *tuser,int money)
{
  int unum;
  if (unum = getuser(tuser))
    {
      xuser.silvermoney += money; 
      substitute_record(fn_passwd, &xuser,sizeof(userec), unum); 
      return xuser.silvermoney;
    }
  else
      return -1;
}

int
inugold(char *tuser,int money)
{
  int unum;
  if (unum = getuser(tuser))
    {
      xuser.goldmoney += money; 
      substitute_record(fn_passwd, &xuser,sizeof(userec), unum); 
      return xuser.goldmoney;
    }
  else
      return -1;
}

int
inmoney(int money)
{
      update_data();
      cuser.silvermoney = xuser.silvermoney + money;
      substitute_record(fn_passwd, &cuser, sizeof(userec), usernum);
      return cuser.silvermoney;
}

int
ingold(unsigned long int money)
{
      update_data();
      cuser.goldmoney = xuser.goldmoney + money;
      substitute_record(fn_passwd, &cuser, sizeof(userec), usernum);
      return cuser.goldmoney;
}

int
inmailbox(int m)
{
      update_data();
      cuser.exmailbox = xuser.exmailbox + m;
      substitute_record(fn_passwd, &cuser, sizeof(userec), usernum);
      return cuser.exmailbox;
}

int
deumoney(char *tuser, int money)
{
  int unum;
  if (unum = getuser(tuser))
    {
      if((unsigned long int)xuser.silvermoney <=
         (unsigned long int) money) xuser.silvermoney=0;
      else xuser.silvermoney -= money;
      substitute_record(fn_passwd, &xuser, sizeof(userec), unum);
      return xuser.silvermoney;
    }
  else
      return -1;
}

int
demoney(unsigned long int money)
{
  update_data();
  if(xuser.silvermoney <= money) cuser.silvermoney=0;
  else
    cuser.silvermoney = xuser.silvermoney - money;
  substitute_record(fn_passwd, &cuser, sizeof(userec), usernum);
  return cuser.silvermoney;
}

int
degold(unsigned long int money)
{
  update_data();
  if(xuser.goldmoney <= money) cuser.goldmoney=0;
  else
    cuser.goldmoney = xuser.goldmoney - money;
  substitute_record(fn_passwd, &cuser, sizeof(userec), usernum);
  return cuser.goldmoney;
}

/* ¨¾°ô Multi play */
int
count_multiplay(int unmode)
{
  register int i, j;
  register user_info *uentp;
  extern struct UTMPFILE *utmpshm;

  resolve_utmp();
  for (i = j = 0; i < USHM_SIZE; i++)
  {
    uentp = &(utmpshm->uinfo[i]);
    if (uentp->uid == usernum)
     if(uentp->lockmode == unmode)
      j++;
  }
  return j;
}

