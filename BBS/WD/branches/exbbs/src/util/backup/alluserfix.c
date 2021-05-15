#include "bbs.h"
#include "record.c"
#include "cache.c"
#include "bbs_util.h"

#define pwdfile fn_passwds
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

int userfix(char *tuser)
{
  int unum;
  if (unum = getuser(tuser))
    {
//      xuser.habit |= HABIT_SAYALOHA;
//      if(xuser.exp<0) xuser.exp=0;
//      if(xuser.goldmoney<0) xuser.goldmoney=0;
//      if(xuser.silvermoney<0) xuser.silvermoney=0;
      xuser.habit |= HABIT_LIGHTBAR;
      substitute_record(pwdfile, &xuser, sizeof(userec), unum);
      return 1;
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
  int i;
  char name[32];
                                                                                
  if (argc != 2)
  {
     printf("usage: alluserfix input_pwdfile\n");
     return 0;
  }

  inf = fopen(argv[1],"rb");

  if (inf == NULL)
  {
    printf("Sorry, user data is not ready.\n");
    exit(0);
  }
         

  time(&now);
  for (i = 0; fread(&auser, sizeof(userec), 1, inf); i++)
  {
    if (bad_user_id(auser.userid) || strchr(auser.userid,'.') )
        i--;
    else
    {
       strcpy(name, auser.userid);
       
//       if(strcmp(name,"SYSOP") && strcmp(name,"guest")
//          && !CHECK_LEVEL(PERM_SYSOP, auser.userlevel))
          userfix(name);
    }

  }
  fclose(inf);

  printf("fixed user: %d\n",i);
  return 1;
}
