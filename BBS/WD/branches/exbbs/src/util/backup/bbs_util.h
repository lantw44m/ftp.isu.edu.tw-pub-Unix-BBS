
// WDBBS UPGRADE 20010710 modified by Robert Liu



#ifndef BRD_OUTGOING
#define BRD_OUTGOING    00004
#endif

#ifndef BRD_INGOING
#define BRD_INGOING     04000
#endif

#ifndef BRD_BRDMAIL
#define BRD_BRDMAIL	00100
#endif

#define fn_passwds	BBSHOME"/.PASSWDS"

#define BOARDS_REC      BBSHOME "/.BOARDS"
#define BOARDS_TMP      BBSHOME "/.BOARDS.TMP"
#define BOARDS_OLD      BBSHOME "/.BOARDS.OLD"
#define BOARDS_NEW	BBSHOME "/.BOaRDS.NEW"
#define BOARDS_HTTP	BBSHOME "/.BOARDS.HTTP"

#define EXPIRE_CONF     BBSHOME "/etc/expire.conf"
#define NOKEEP_CONF     BBSHOME "/etc/nokeep.conf"

#define BOARDS_PATH     BBSHOME "/boards"
#define MAN_BRD_PATH	BBSHOME "/man/boards"
#define MAN_HIST_PATH   BBSHOME "/man/log"


int
belong(filelist, key)
  char *filelist;
  char *key;
{
  FILE *fp;
  char *ptr, buf[STRLEN];
  int rc = 0;
  if (fp = fopen(filelist, "r"))
  {
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

dashf(fname)
  char *fname;
{
  struct stat st;

  return (stat(fname, &st) == 0 && S_ISREG(st.st_mode));
}


int
dashd(fname)
  char *fname;
{
  struct stat st;

  return (stat(fname, &st) == 0 && S_ISDIR(st.st_mode));
}

long
dasht(fname)
  char *fname;
{
  struct stat st;

  if (!stat(fname, &st))
        return (st.st_mtime);
  else
        return -1;
}

