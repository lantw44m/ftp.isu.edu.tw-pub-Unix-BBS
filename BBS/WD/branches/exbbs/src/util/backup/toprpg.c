/*-------------------------------------------------------*/
/* util/toprpg.c        ( Wind & Dust BBS 1998.6 )       */
/*-------------------------------------------------------*/
/* target : ¨Ï¥ÎªÌ RPG ±Æ¦æº]                            */
/*-------------------------------------------------------*/

#include "bbs.h"
#define REAL_INFO

char *racename[7] = {"µL·~¹C¥Á","Äé¤ô±j¤H","Åª¤å§Ö¤â","±`¾nµ{¦¡","²á¤Ñ²r±N","¤ô²y«a­x","¹CÀ¸«g¬P"};

struct manrec
{
  char userid[IDLEN+1];
  char username[24];
  uschar race;
  ushort level;
  usint age; 
  unsigned long int exp;
};
typedef struct manrec manrec;
struct manrec allman[MAXUSERS];

userec USER;
rpgrec RPG;
int num=0;
uschar racenum;
int racetype[7] = {0,0,0,0,0,0,0};
int toptype[7] = {0,0,0,0,0,0,0};
float totalexp[7] = {0,0,0,0,0,0,0};
int totalage[7] = {0,0,0,0,0,0,0};
FILE *fp;

int
belong(filelist, key)
  char *filelist;
  char *key;
{
  FILE *fp1;
  int rc = 0;

  if (fp1 = fopen(filelist, "r"))
  {
    char buf[STRLEN], *ptr;

    while (fgets(buf, STRLEN, fp1))
    {
      if ((ptr = strtok(buf, " \t\n\r")) && !strcasecmp(ptr, key))
      {
        rc = 1;
        break;
      }
    }
    fclose(fp1);
  }
  return rc;
}


int
level_cmp(b, a)
  struct manrec *a, *b;
{

  if(a->level != b->level)
    return(a->level - b->level);  
  else
   {
    if(a->exp > b->exp)
      return((a->level+1)-b->level);
    else
      return((a->level-1) - b->level);
   }
}


void
top()
{
  int i, k, rows = num;
  char buf1[1024];
  if(racenum != 8)
  {  
    fprintf(fp, "\
[1m[44;33m¢~¡³¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢s¡÷[41m    ¢à¢Þ¢Õ±Æ¦æº]    [33;44m¡ö¢s¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¡³¢¡[m
[1m[44;33m¢u¦W¦¸¢w¥N¸¹¢w¢w¢w¼ÊºÙ¢w¢r¢w¢w¢w¢w¢w¢w¢w¦~ÄÖ¢w¢wÂ¾·~¢w¢wµ¥¯Å¢w¢w¢w¢w¢w¸gÅç­È¢t[m");
    for (i = 0; i < rows; i++)
    {
      k = allman[i].race;
      if( k < 0 || k > 6)
        k=0;
      ++toptype[k];
      sprintf(buf1, 
"[1;3%dm%3d. [37m%-11.11s%-20.20s   [3%dm%-3d %-8.8s [1;3%dm%5d %15d[m ",
  allman[i].level,i + 1, allman[i].userid,allman[i].username,k,
  (allman[i].age/2880)+10, racename[k],allman[i].level%8,
   allman[i].level,allman[i].exp);
      fprintf(fp, "\n[1m[44;33m¢x[m%s[1m[44;33m¢x[m", buf1);
    }
    fprintf(fp,"
[1m[44;33m¢¢¡³¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¡³¢£[m");
    fprintf(fp,"\n[1;37m¡³ º]¤WÁ`¤H¼Æ ¡³
[1;31m%s %-3d¤H  [32m%s %-3d¤H
[1;33m%s %-3d¤H  [34m%s %-3d¤H
[1;35m%s %-3d¤H  [36m%s %-3d¤H\n",
     racename[1],toptype[1],racename[2],toptype[2],racename[3],toptype[3],
     racename[4],toptype[4],racename[5],toptype[5],racename[6],toptype[6]);
  }
  else
  {  
    fprintf(fp,"\n[1;37m¡³ Â¾·~Á`¤H¼Æ ¡³
[1;31m%s %-3d¤H  [32m%s %-3d¤H
[1;33m%s %-3d¤H  [34m%s %-3d¤H
[1;35m%s %-3d¤H  [36m%s %-3d¤H\n",
     racename[1],racetype[1],racename[2],racetype[2],racename[3],racetype[3],
     racename[4],racetype[4],racename[5],racetype[5],racename[6],racetype[6]);
    fprintf(fp,"\n[1;37m¡³ ¸gÅç­ÈÁ`©M ¡³
[1;31m%s  %20.0f  [32m%s  %20.0f
[1;33m%s  %20.0f  [34m%s  %20.0f
[1;35m%s  %20.0f  [36m%s  %20.0f\n",
     racename[1],totalexp[1],racename[2],totalexp[2],racename[3],totalexp[3],
     racename[4],totalexp[4],racename[5],totalexp[5],racename[6],totalexp[6]);
    fprintf(fp,"\n[1;37m¡³ ¸gÅç­È¥­§¡ ¡³
[1;31m%s  %20.0f  [32m%s  %20.0f
[1;33m%s  %20.0f  [34m%s  %20.0f
[1;35m%s  %20.0f  [36m%s  %20.0f\n",
     racename[1],totalexp[1]/racetype[1],
     racename[2],totalexp[2]/racetype[2],
     racename[3],totalexp[3]/racetype[3],
     racename[4],totalexp[4]/racetype[4],
     racename[5],totalexp[5]/racetype[5],
     racename[6],totalexp[6]/racetype[6]);
  }
}

int
not_alpha(ch)
  register char ch;
{
  return (ch < 'A' || (ch > 'Z' && ch < 'a') || ch > 'z');
}

int
not_alnum(ch)
  register char ch;
{
  return (ch < '0' || (ch > '9' && ch < 'A') ||
    (ch > 'Z' && ch < 'a') || ch > 'z');
}

int
bad_user_id(userid)
  char *userid;
{
  register char ch;
  if (strlen(userid) < 2)
    return 1;
  if (not_alpha(*userid))
    return 1;
  while (ch = *(++userid))
  {
    if (not_alnum(ch))
      return 1;
  }
  return 0;
}

int
main(argc, argv)
  int argc;
  char **argv;
{
  int fdu,fdr,i=0;
  char buf[256];

  if (argc < 4)
  {
    printf("Usage: %s <racenum> <num_top> <out-file>\n", argv[0]);
    exit(1);
  }

  racenum = atoi(argv[1]);  
  num = atoi(argv[2]);
  if (num == 0)
    num = 30;

  fdu=open(BBSHOME"/.PASSWDS",O_RDONLY);

  while(read(fdu,&USER,sizeof(userec))==sizeof(userec))
  {
    if (strlen(USER.userid) != 0)
    {
     sprintf(buf, BBSHOME"/home/%s/.RPG",USER.userid);
     fdr=open(buf , O_RDONLY);
     
     if(fdr != 0)
     {
       read(fdr,&RPG,sizeof(rpgrec));
                
      if(belong("etc/nontop",USER.userid) || bad_user_id(USER.userid)
        || strchr(USER.userid,'.') || (RPG.race != racenum && racenum < 7))
      {
        i--;
      }
      else
      {
         unsigned long int tmp; 
//         printf("%3d) user=%-13s race=%2d level=%d exp=%d\n",i,USER.userid,RPG.race,RPG.level,USER.exp);
         strcpy(allman[i].userid, USER.userid);
         strncpy(allman[i].username, USER.username,23);
         allman[i].race = RPG.race;
         allman[i].age = RPG.age;
         allman[i].level = RPG.level;
         allman[i].exp = USER.exp;
         ++racetype[RPG.race];
         tmp = RPG.level-1;
         totalage[RPG.race] += RPG.age/2880;
         totalexp[RPG.race] += ((tmp*(tmp+1)*((tmp*2)+1)) / 6)*10000 + USER.exp; 
       }
      i++; 
    }
    close(fdr);
   }
  }

  if ((fp = fopen(argv[3], "w")) == NULL)
  {
    printf("cann't open toprpg\n");
    return 0;
  }

  qsort(allman, i, sizeof(manrec), level_cmp);
  top();
  close(fdu);
  fclose(fp);
}
