/*
Phoenix to Maple (sob-version): .PASSWDS
woju.bbs@freebsd.ee.ntu.edu.tw
*/

#include <stdio.h>
#include <time.h>

#define PNPASSWDS  BBSHOME "/.PASSWDS"
#define DOTPASSWDS BBSHOME "/.PASSWDS"
#define PASSWDSBAK BBSHOME "/PASSWDS"
#define TMPFILE    BBSHOME "/tmpfile"
#define LOGFILE    "pn2sobpass.log"


#define IDLEN    12             /* Length of bid/uid */
#define PASSLEN  14             /* Length of encrypted passwd field */
#define REGLEN   38             /* Length of registration data */
#define NAMELEN  40             /* Length of username/realname */
#define STRLEN   80             /* Length of most string data */

struct
{
  char userid[IDLEN + 1];
  char realname[20];
  char username[24];
  char passwd[PASSLEN];
  unsigned char uflag;
  unsigned int userlevel;
  unsigned short numlogins;
  unsigned short numposts;
  time_t firstlogin;
  time_t lastlogin;
  char lasthost[16];
  char termtype[8];
  char email[50];
  char address[50];
  char justify[REGLEN + 1];
  unsigned char month;
  unsigned char day;
  unsigned char year;
  unsigned char sex;
  unsigned char state;
} sobcuser;


struct {                  /* Structure used to hold information in */
        char            userid[IDLEN+2];   /* PASSFILE */
        char            fill[30];
        time_t          firstlogin;
        char            lasthost[16];
        unsigned int    numlogins;
        unsigned int    numposts;
        char            flags[2];
        char            passwd[PASSLEN];
        char            username[NAMELEN];
        char            justify[NAMELEN];
        char            termtype[STRLEN];
        unsigned        userlevel;
        time_t          lastlogin;
        time_t          unused_time;
        char            realname[NAMELEN];
        char            address[STRLEN];
        char            email[STRLEN];
} pncuser;

main()
{
    FILE *foo1, *foo2, *flog;
    int cnum, i, match, total;

    if (!(flog = fopen(LOGFILE, "w"))) {
       puts("log file opening error");
       exit(1);
    }
    if( ((foo1=fopen(PNPASSWDS, "r")) == NULL)
                || ((foo2=fopen(TMPFILE,"w"))== NULL) ){
        puts("file opening error");
        exit(1);
    }

    #define STR_PN2SOB(field) \
       strncpy(sobcuser.field, pncuser.field, sizeof(sobcuser.field) - 1); \
       sobcuser.field[sizeof(sobcuser.field) - 1] = 0; \
       if (strlen(pncuser.field) > sizeof(sobcuser.field) - 1) \
          fprintf(flog, \
             "Waring[%d]: %s '%s' too long\n", i, #field, pncuser.field);

    #define INT_PN2SOB(field) sobcuser.field = pncuser.field

    i = 0;
    fseek(foo1, 0, 2);
    total = ftell(foo1) / sizeof(pncuser);
    rewind(foo1);
    while( (cnum=fread( &pncuser, sizeof(pncuser), 1, foo1))>0 ) {
       ++i;
       memset(&sobcuser, 0, sizeof(sobcuser));
       STR_PN2SOB(userid);
       STR_PN2SOB(realname);
       STR_PN2SOB(username);
       STR_PN2SOB(passwd);
       INT_PN2SOB(userlevel);
       INT_PN2SOB(numlogins);
       INT_PN2SOB(numposts);
       INT_PN2SOB(firstlogin);
       INT_PN2SOB(lastlogin);
       STR_PN2SOB(lasthost);
       STR_PN2SOB(termtype);
       STR_PN2SOB(email);
       STR_PN2SOB(address);
       STR_PN2SOB(justify);
       sobcuser.uflag = 0x40 | 0x80;
       if (bad_user_id(sobcuser.userid)) {
          if (*sobcuser.userid)
             fprintf(flog,
                "Warning[%d]: bad userid '%s'\n", i, sobcuser.userid);
          continue;
       }
       fwrite(&sobcuser, sizeof(sobcuser), 1, foo2);
       if (i % 100 == 1) {
          printf("%d / %d\r", i, total);
          fflush(stdout);
       }
    }
    fclose(foo1);
    fclose(foo2);
    fclose(flog);
    printf("\r\n\rFinished, please check %s\n", LOGFILE);

    if(rename(DOTPASSWDS,PASSWDSBAK)==-1){
        perror("replace fails");
        exit(1);
    }
    unlink(DOTPASSWDS);
    rename(TMPFILE,DOTPASSWDS);
    unlink("tmpfile");

    exit(0);
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

