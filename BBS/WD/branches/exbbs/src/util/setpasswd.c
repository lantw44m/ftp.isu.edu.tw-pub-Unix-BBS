
#include "bbs.h"
//#include "bbs_util.h"
#include "record.c"
#include "cache.c"

//#define pwdfile fn_passwds
#define pwdfile BBSHOME "/.PASSWDS"

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

int setpwd(char *tuser, char *passwd)
{
  int unum;
  char oldpwd[PASSLEN], newpwd[PASSLEN];
  
  if (unum = getuser(tuser))
    {
      strcpy(oldpwd, xuser.passwd);
      printf("%s [%s] ", tuser, oldpwd);
      if(strlen(passwd)>0)
      {
        sprintf(newpwd, "%s", crypt(passwd, oldpwd));
        if(strncmp(newpwd, oldpwd, PASSLEN)==0)
          printf("the same as the old password!");
        else
        {
          sprintf(newpwd, "%s", genpasswd(passwd));
          printf("-> <%s> [%s] ", passwd, newpwd);
          strcpy(xuser.passwd, newpwd);
          substitute_record(pwdfile, &xuser, sizeof(userec), unum);
        }
      }
      printf("\n");
      return 1;
    }
  return -1;
}

int main(int argc, char *argv[])
{
//  FILE *inf;
  time_t now;
  char userid[IDLEN + 1], newpwd[PASSLEN]="\0";
                                                                                
  if (argc < 2)
  {
     printf("\nusage: setpasswd userid [password] \n\n");
     exit(0);
  }
  strcpy(userid, argv[1]);
  if(argc>2)
     strcpy(newpwd, argv[2]);

  time(&now);
  return setpwd(userid, newpwd);
}
