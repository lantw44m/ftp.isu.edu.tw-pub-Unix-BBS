/*-------------------------------------------------------*/
/* util/message.c                  ( PRO BBS Ver 1.0)    */
/*-------------------------------------------------------*/
/* target : ¨Ï¥ÎªÌ °T®§¼Æ ±Æ¦æº]                         */
/* create : 97/08/08                                     */
/*-------------------------------------------------------*/
#include "bbs.h"

#define TYPE_FROM       0
#define TYPE_TO      1
#undef MONTH

struct manrec
{
  char userid[IDLEN+1];
  char username[23];
  int to_times;
  int from_times;
};
typedef struct manrec manrec;
struct manrec allman[MAXUSERS];

userec aman;
int mannum;
int num;
FILE *fp;

int
record_data(name,type)
char *name;
int type;
{
  int i;
  int n;

  for(i=0;i<mannum;i++)
  {
    if (!strcmp(name, allman[i].userid))
    {
      if (type == 0) allman[i].from_times++;
      else allman[i].to_times++;
      return;
    }
  }
  return ;
}

int
from_cmp(b, a)
struct manrec *a, *b;
{
  return (a->from_times - b->from_times);
}

int
to_cmp(b, a)
struct manrec *a, *b;
{
  return (a->to_times - b->to_times);
}

void
top(type)
{
  static char *str_type[2] = {"µo°T®§", "¦¬°T®§"};
  int i, j, rows = (num + 1) / 2;
  char buf1[80], buf2[80];

  if (type)
    fprintf(fp, "\n\n");

  fprintf(fp, "\t\t\
[1;33m===========[%dm    %s¼Æ±Æ¦æº]    [40m============[m\n\n\
[1;36m ¦W¦¸ ¥N¸¹       ¼ÊºÙ            ¼Æ¥Ø    ¦W¦¸ ¥N¸¹       ¼ÊºÙ            ¼Æ¥Ø[m\n\
¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w  ¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w\
", type + 44, str_type[type]);

  for (i = 0; i < rows; i++)
  {
    sprintf(buf1, "[%2d] %-11.11s%-16.16s%4d",
      i + 1, allman[i].userid, allman[i].username,
      (type ? allman[i].to_times : allman[i].from_times));
    j = i + rows;
    sprintf(buf2, "[%2d] %-11.11s%-16.16s%4d",
      j + 1, allman[j].userid, allman[j].username,
      (type ? allman[j].to_times : allman[j].from_times));
    if (i < 3)
      fprintf(fp, "\n [1;%dm%-40s[0;37m%s", 31 + i, buf1, buf2);
    else
      fprintf(fp, "\n %-40s%s", buf1, buf2);
  }
}

main(argc, argv)
  int argc;
  char **argv;
{
  FILE *inf, *inf2;
  int i=0;
#ifdef MONTH
  int j=0;
#endif
  char buf[256], *p, uname_from[20], uname_to[20];

  if (argc < 2)
  {
    printf("Usage: %s num_top\n", argv[0]);
    exit(1);
  }

  num = atoi(argv[1]);
  if (num == 0)
    num = 20;

  inf = fopen(BBSHOME "/.PASSWDS", "rb");


  if (inf == NULL)
  {
    printf("Sorry, the data is not ready.\n");
    exit(0);
  }

  for (i = 0; fread(&aman, sizeof(userec), 1, inf); i++)
  {
    strcpy(allman[i].userid, aman.userid);
    strncpy(allman[i].username, aman.username,23);
  }

  mannum=i;

  fclose(inf);

#ifdef MONTH
  inf2 = fopen(BBSHOME "/adm/" FN_MESSAGE, "rb");
#else
  inf2 = fopen(BBSHOME "/" FN_MESSAGE , "rb");
#endif

  if (inf2 == NULL)
  {
    printf("Sorry, the data is not ready.\n");
    exit(0);
  }

  while (fgets(buf, 256, inf2))
  {
    if(strlen(buf)<57)
        continue;
    if ( !strncmp(buf+21, "MSG", 3))
    {
      p=strstr(buf,"MSG");
      p+=4;
      p=strtok(p," ");
      strcpy(uname_from,p);
      if (( p = (char *)strstr(buf+46, "TO")) && (strncmp(buf+51," ",1)))
      {
        p+=4;
        p=strtok(p," ");
        if (strstr(p,"\n"))
          p=strtok(p,"\n");
        strcpy(uname_to,p);
      }
    record_data(uname_from, 0);
    record_data(uname_to, 1);
#ifdef MONTH
    j++;
#endif
    }
  }

  fclose(inf2);

  if ((fp = fopen(BBSHOME "/etc/message", "w")) == NULL)
  {
    printf("cann't open message\n");
    return 0;
  }

  qsort(allman, mannum, sizeof(manrec), from_cmp);
  top(TYPE_FROM);

  qsort(allman, mannum, sizeof(manrec), to_cmp);
  top(TYPE_TO);

#ifdef MONTH
  printf("total = %i", j);
#endif

}

