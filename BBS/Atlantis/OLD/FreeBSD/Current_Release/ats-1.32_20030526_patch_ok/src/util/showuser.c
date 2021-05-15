/*-------------------------------------------------------*/
/* util/showuser.c      ( NTHU CS MapleBBS Ver 2.36 )    */
/*-------------------------------------------------------*/
/* target : 使用者資料縱覽                               */
/* create : 95/03/29                                     */
/* update : 95/12/15                                     */
/*-------------------------------------------------------*/
/*      $ showuser .PASSWDS ... : 看 all users           */
/*      $ ln showuser showsysop : 看 sysop               */
/*      $ ln showuser showmgr   : 看 manager/sysop       */
/*-------------------------------------------------------*/


#include "bbs.h"

struct userec aman;
char *MYPASSFILE, field_str[80];
int field_count = 0;
int field_lst_no[17];
int field_lst_size[17];
int field_default_size[17] = {
  4, 12, 16, 4, 4,
  16, 14, 12, 14, 30,
  8, 16, 14, 40, 14, 10, 16
};

char field_idx[] = "dihlpnvramUufjVFP";
char *field_name[] = {
  "No.",
  "UserID",
  "LastHost",
  "Login",
  "Post",
  "Nick",
  "LastVisit",
  "Real",
  "Addr",
  "Email",
  "LastUser",
  "Userlevel",
  "FirstVisit",
  "Justify",
  "LastVisitTime",
  "UserFlag",
  "Password",
  NULL
};


void
set_opt(argc, argv)
  int argc;
  char *argv[];
{
  int i, flag, field, size, *p;
  char *ptr, *field_ptr;

  field_count = 0;

  for (i = 2; i < argc; i++)
  {
    field_ptr = (char *) strchr(field_idx, argv[i][0]);
    if (field_ptr == NULL)
      continue;
    else
      field = field_ptr - field_idx;

    size = atoi(argv[i] + 1);

    field_lst_no[field_count] = field;
    field_lst_size[field_count] = size ? size : field_default_size[field];
    field_count++;
  }
}


void
print_head()
{
  int i, field, size;

  for (i = 0; i < field_count; i++)
  {
    field = field_lst_no[i];
    size = field_lst_size[i];
    printf("%-*.*s ", size, size, field_name[field]);
  }
  printf("\n");
  for (i = 0; i < field_count; i++)
  {
    size = field_lst_size[i];
    while (size--)
      putchar('=');
    putchar(' ');
  }
  printf("\n");
}



char*
my_ctime(t)
  time_t *t;
{
  struct tm *tp;
  tp = localtime(t);
  sprintf(field_str, "%02d%02d%02d%02d%02d%02d", tp->tm_year, tp->tm_mon + 1,
     tp->tm_mday, tp->tm_hour, tp->tm_min, tp->tm_sec);
}



void
print_record(serial_no, p)
  int serial_no;
  struct userec *p;
{
  int i, j, field, size, pat;

  for (i = 0; i < field_count; i++)
  {
    field = field_lst_no[i];
    size = field_lst_size[i];

    switch (field)
    {
    case 0:
      sprintf(field_str, "%d", serial_no);
      break;

    case 1:
      strcpy(field_str, p->userid);
      break;

    case 2:
      strcpy(field_str, p->lasthost);
      break;

    case 3:
      sprintf(field_str, "%d", p->numlogins);
      break;

    case 4:
      sprintf(field_str, "%d", p->numposts);
      break;

    case 5:
      strcpy(field_str, p->username);
      break;

    case 6:
      my_ctime(&p->lastlogin);
      break;

    case 7:
      strncpy(field_str, p->realname, 79);
      break;

    case 8:
      strcpy(field_str, p->address);
      break;

    case 9:
      strcpy(field_str, p->email);
      break;

    case 10:
      strcpy(field_str, p->remoteuser);
      break;

    case 11:
      pat = p->userlevel;
      for (j = 0; j < 31; j++, pat >>= 1)
      {
        field_str[j] = (pat & 1) ? '1' : '0';
      }
      field_str[j] = '\0';
      break;

    case 12:
      my_ctime(&p->firstlogin);
      break;

    case 13:
      strcpy(field_str, p->justify);
      break;

    case 14:
      sprintf(field_str, "%d", p->lastlogin);
    case 15:
      pat = p->uflag;
      for (j = 0; j < 8; j++, pat >>= 1)
      {
        field_str[j] = (pat & 1) ? '1' : '0';
      }
      field_str[j] = '\0';
      break;
    case 16:
      sprintf(field_str, "%.*s", PASSLEN, p->passwd);
      break;
    }

    printf("%-*.*s ", size, size, field_str);
  }
  printf("\n");
}


main(argc, argv)
  int argc;
  char *argv[];
{
  FILE *inf;
  int i, level;
  char *p;

  if (argc < 3)
  {
    printf("Usage: %s %s\n", argv[0], "password_file [XN] ....");
    printf("Example: %s %s\n", argv[0], "d3 i12 e30");
    printf("N is field width, X is one of the following char :\n");

    for (i = 0; field_name[i]; i++)
    {
      printf("\t%c[%2d] - %s\n",
        field_idx[i], field_default_size[i], field_name[i]);
    }
    exit(0);
  }
  else
  {
    set_opt(argc, argv);
    MYPASSFILE = argv[1];
  }

  if (strstr(argv[0], "showsysop"))
    level = PERM_SYSOP;
  else if (strstr(argv[0], "showmgr"))
    level = PERM_BM;
  else
    level = 0;

  inf = fopen(MYPASSFILE, "rb");
  if (inf == NULL)
  {
    printf("Error open %s\n", MYPASSFILE);
    exit(0);
  }

  print_head();
  i = 0;

  while (fread(&aman, sizeof(aman), 1, inf))
  {
    i++;
    if (aman.userlevel >= level)
      print_record(i, &aman);
  }
  fclose(inf);
}
