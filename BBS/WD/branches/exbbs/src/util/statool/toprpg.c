/*-------------------------------------------------------*/
/* util/toprpg.c        ( Wind & Dust BBS 1998.6 )       */
/*-------------------------------------------------------*/
/* target : 使用者 RPG 排行榜                            */
/*-------------------------------------------------------*/

#include "bbs.h"
#define REAL_INFO

char *racename[7] = {"無業遊民","灌水強人","讀文快手","常駐程式","聊天猛將","水球冠軍","遊戲剋星"};

struct manrec
{
  char userid[IDLEN+1];
  char username[23];
  ushort race;
  ushort level;
  usint age; 
  unsigned long int exp;
};
typedef struct manrec manrec;
struct manrec allman[MAXUSERS];

userec aman;
rpgrec rman;
int num=0 ,racenum;
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
  FILE *fp;
  int rc = 0;

  if (fp = fopen(filelist, "r"))
  {
    char buf[STRLEN], *ptr;

    while (fgets(buf, STRLEN, fp))
    {
      if ((ptr = strtok(buf, " \t\n\r")) && !strcasecmp(ptr, key))
      {
        rc = 1;
        break;
      }
    }
    fclose(fp);
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
[1m[44;33m╭○──────────┬→[41m    ＲＰＧ排行榜    [33;44m←┬───────────○╮[m
[1m[44;33m├名次─代號───暱稱─┴───────年齡──職業──等級─────經驗值┤[m");
    for (i = 0; i < rows; i++)
    {
      k = allman[i].race;
      ++toptype[k];
      sprintf(buf1, 
"[1;3%dm%3d. [37m%-11.11s%-20.20s   [3%dm%-3d %-8.8s [1;3%dm%5d %15d[m ",
  allman[i].level%8,i + 1, allman[i].userid,allman[i].username,k,
  (allman[i].age/2880)+10, racename[k],allman[i].level%8,
   allman[i].level,allman[i].exp);
      fprintf(fp, "\n[1m[44;33m│[m%s[1m[44;33m│[m", buf1);
    }
    fprintf(fp,"
[1m[44;33m╰○───────────────────────────────────○╯[m");
    fprintf(fp,"\n[1;37m○ 榜上總人數 ○
[1;31m%s %-3d人  [32m%s %-3d人
[1;33m%s %-3d人  [34m%s %-3d人
[1;35m%s %-3d人  [36m%s %-3d人\n",
     racename[1],toptype[1],racename[2],toptype[2],racename[3],toptype[3],
     racename[4],toptype[4],racename[5],toptype[5],racename[6],toptype[6]);
  }
  else
  {  
    fprintf(fp,"\n[1;37m○ 職業總人數 ○
[1;31m%s %-3d人  [32m%s %-3d人
[1;33m%s %-3d人  [34m%s %-3d人
[1;35m%s %-3d人  [36m%s %-3d人\n",
     racename[1],racetype[1],racename[2],racetype[2],racename[3],racetype[3],
     racename[4],racetype[4],racename[5],racetype[5],racename[6],racetype[6]);
    fprintf(fp,"\n[1;37m○ 經驗值總和 ○
[1;31m%s  %20.0f  [32m%s  %20.0f
[1;33m%s  %20.0f  [34m%s  %20.0f
[1;35m%s  %20.0f  [36m%s  %20.0f\n",
     racename[1],totalexp[1],racename[2],totalexp[2],racename[3],totalexp[3],
     racename[4],totalexp[4],racename[5],totalexp[5],racename[6],totalexp[6]);
    fprintf(fp,"\n[1;37m○ 經驗值平均 ○
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

main(argc, argv)
  int argc;
  char **argv;
{
  FILE *inf,*inf2;
  char buf[64];
  int i;

  if (argc < 4)
  {
    printf("Usage: %s <racenum> <num_top> <out-file>\n", argv[0]);
    exit(1);
  }

  racenum = atoi(argv[1]);  
  num = atoi(argv[2]);
  if (num == 0)
    num = 30;

  inf = fopen(BBSHOME "/.PASSWDS", "rb");

  if (inf == NULL)
  {
    printf("Sorry, the data is not ready.\n");
    exit(0);
  }

  for (i = 0; fread(&aman, sizeof(userec), 1, inf); i++)
  {
    sprintf(buf, BBSHOME"/home/%s/.RPG",aman.userid);
    inf2 = fopen(buf, "rb");
    
    fread(&rman,sizeof(rpgrec),1,inf2);
    
    if(belong("etc/nontop",aman.userid) || bad_user_id(aman.userid)
       || strchr(aman.userid,'.') || (rman.race != racenum && racenum < 7))
    {
        i--;
    }
    else
    {
       unsigned long int tmp; 
       strcpy(allman[i].userid, aman.userid);
       strncpy(allman[i].username, aman.username,23);
       allman[i].race = rman.race;
       allman[i].age = rman.age;
       allman[i].level = rman.level;
       allman[i].exp = aman.exp;
       ++racetype[rman.race];
       tmp = rman.level-1;
       totalage[rman.race] += rman.age/2880;
       totalexp[rman.race] += ((tmp*(tmp+1)*((tmp*2)+1)) / 6)*10000 + aman.exp; 
    }
    fclose(inf2);
  }

  if ((fp = fopen(argv[3], "w")) == NULL)
  {
    printf("cann't open toprpg\n");
    return 0;
  }

  qsort(allman, i, sizeof(manrec), level_cmp);
  top();
  fclose(inf);
  fclose(fp);
}
