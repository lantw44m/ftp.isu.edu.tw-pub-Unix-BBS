#include <stdio.h>
#include "bbs.h"

#define DOTPASSWDS	BBSHOME"/.PASSWDS"
#define PASSWDSBAK	BBSHOME"/PASSWDS"
//#define DOTRPG		BBSHOME"/.RPGS"
//#define RPGBAK		BBSHOME"/RPGS"
#define TMPFILE		BBSHOME"/tmp/tmpfile"

struct userec cuser;
struct rpgrec ruser;

main(int argc, char *argv[])
{
    FILE *foo1, *foo2;
    int cnum,i=0;
    char option[32]="nothing";
    
    if (argc>1)
      strcpy(option, argv[1]);
      
//    setgid(BBSGID);
//    setuid(BBSUID);
    
    if( ((foo1=fopen(DOTPASSWDS, "r")) == NULL)
           || ((foo2=fopen(TMPFILE,"w"))== NULL) ){
        puts("file opening error");
        exit(1);
    }

    while( (cnum=fread( &cuser, sizeof(cuser), 1, foo1))>0 ) {
       if (bad_user_id(cuser.userid))
          continue;
       if (strcmp(option,"clear")==0)
       {
          if(strcmp(cuser.userid,"SYSOP")
             // && strcmp(cuser.userid,"guest")
            )
             continue;
          strcpy(cuser.passwd, genpasswd("bbs"));
          strcpy(cuser.email, "?@?.?");
          strcpy(cuser.address, "??????????");
//          strcpy(cuser.realname, "?");
//          strcpy(cuser.username, "?");
          strcpy(cuser.lasthost, "?");
//          strcpy(cuser.justify, "OK");
          strcpy(cuser.toqid, "?");
          strcpy(cuser.beqid, "?");
          cuser.totaltime=0;
          cuser.numlogins=5;
          cuser.numposts=0;
          cuser.toquery=0;
          cuser.bequery=0;
          cuser.sendmsg=0;
          cuser.receivemsg=0;
          cuser.goldmoney=0;
          cuser.silvermoney=0;
          cuser.exp=0;
          cuser.limitmoney=0;
       }
       printf("%3d save %s\n",i++,cuser.userid);
       fwrite( &cuser, sizeof(cuser), 1, foo2);
    }
    fclose(foo1);
    fclose(foo2);

//    if(rename(DOTPASSWDS,PASSWDSBAK)==-1){
//        puts("replace fails");
//        exit(1);
//    }
//    unlink(DOTPASSWDS);
//    rename(TMPFILE,DOTPASSWDS);
//    unlink("tmpfile");

  rename(TMPFILE,PASSWDSBAK);
  printf("%s -> %s\n",DOTPASSWDS,PASSWDSBAK);
    return 0;
}


bad_user_id(userid)
  char *userid;
{
  register char ch;

  if (strlen(userid) < 2)
    return 1;

  if (!isalpha(*userid))
    return 1;

  if (!strcasecmp(userid, "new"))
    return 1;

  while (ch = *(++userid))
  {
    if (!isalnum(ch))
      return 1;
  }
  return 0;
}
