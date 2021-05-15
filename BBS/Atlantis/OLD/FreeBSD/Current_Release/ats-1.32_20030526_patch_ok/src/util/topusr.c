/*-------------------------------------------------------*/
/* util/topusr.c        ( NTHU CS MapleBBS Ver 2.36 )    */
/*-------------------------------------------------------*/
/* target : ¨Ï¥ÎªÌ ¤W¯¸°O¿ý/¤å³¹½g¼Æ ±Æ¦æº]              */
/* create : 95/03/29                                     */
/* update : 95/12/15                                     */
/*-------------------------------------------------------*/


#define REAL_INFO
#include "bbs.h"

struct manrec
{
  char userid[IDLEN+1];
  char username[23];
  ushort numlogins;
  ushort numposts;
};
typedef struct manrec manrec;
struct manrec allman[MAXUSERS];

userec aman;
int num;
FILE *fp;

#define TYPE_POST       0
#define TYPE_LOGIN      1


int
login_cmp(b, a)
  struct manrec *a, *b;
{
  return (a->numlogins - b->numlogins);
}


int
post_cmp(b, a)
  struct manrec *a, *b;
{
  return (a->numposts - b->numposts);
}


void
top(type)
{
  static char *str_type[2] = {"Äé¤ô±í", "¤U®ü¦¸"};
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
      (type ? allman[i].numlogins : allman[i].numposts));
    j = i + rows;
    sprintf(buf2, "[%2d] %-11.11s%-16.16s%4d",
      j + 1, allman[j].userid, allman[j].username,
      (type ? allman[j].numlogins : allman[j].numposts));
    if (i < 3)
      fprintf(fp, "\n [1;%dm%-40s[0;37m%s", 31 + i, buf1, buf2);
    else
      fprintf(fp, "\n %-40s%s", buf1, buf2);
  }
}


#ifdef  HAVE_TIN
int
post_in_tin(char *name)
{
  char buf[256];
  FILE *fh;
  int counter = 0;

  sprintf(buf, "%s/home/%s/.tin/posted", home_path, name);
  fh = fopen(buf, "r");
  if (fh == NULL)
    return 0;
  else
  {
    while (fgets(buf, 255, fh) != NULL)
      counter++;
    fclose(fh);
    return counter;
  }
}
#endif                          /* HAVE_TIN */


main(argc, argv)
  int argc;
  char **argv;
{
  FILE *inf;
  int i;

  if (argc < 2)
  {
    printf("Usage: %s num_top\n", argv[0]);
    exit(1);
  }

  num = atoi(argv[1]);
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
    strcpy(allman[i].userid, aman.userid);
    strncpy(allman[i].username, aman.username,23);
    allman[i].numlogins = aman.numlogins;
    allman[i].numposts = aman.numposts;

#ifdef  HAVE_TIN
    allman[i].numposts += post_in_tin(allman[i].userid);
#endif
  }

  if ((fp = fopen(BBSHOME "/etc/topusr", "w")) == NULL)
  {
    printf("cann't open topusr\n");
    return 0;
  }

  qsort(allman, i, sizeof(manrec), post_cmp);
  top(TYPE_POST);

  qsort(allman, i, sizeof(manrec), login_cmp);
  top(TYPE_LOGIN);

  fclose(fp);
}
