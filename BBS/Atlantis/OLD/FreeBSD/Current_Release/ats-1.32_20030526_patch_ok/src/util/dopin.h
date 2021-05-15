/*-------------------------------------------------------*/
/* util/dopin.h              (SOB 0.22 ATS Version 1.31) */
/*-------------------------------------------------------*/
/* target : ¨È¯SÄõ´£´µ¦U¶µ¤u¨ãµ{¦¡¦@¥Î¨ç¦¡ ÀH®É§ó·s      */
/* create : 02/07/13 by Dopin                            */
/* update : 02/09/12                                     */
/*-------------------------------------------------------*/
/* syntax :                                              */
/*-------------------------------------------------------*/

static int check_ri = 0;
char *str_space         = " \t\n\r";  /* global.h */

struct userID {
   char userid[IDLEN+1];
   int count;
};
typedef struct userID userID;

userID users[MAXUSERS] = {0};

// stuff.c
void setbpath(char *buf, char *boardname) {
  sprintf(buf, "boards/%s", boardname);
}

int dashd(char *fname) {
   struct stat st;
   return (stat(fname, &st) == 0 && S_ISDIR(st.st_mode));
}

// stuff.c
int dashf(char *fname) {
  struct stat st;
  return (stat(fname, &st) == 0 && S_ISREG(st.st_mode));
}

// ats 1.31 append
int check_repeat_id(int mode, userec *u) {
   int i;
   char flag = 0;
   char buf[IDLEN+1];

   if(check_ri >= MAXUSERS) return -1;
   strcpy(buf, u->userid);

   for(i = 0 ; i < IDLEN+1 && buf[i] ; i++)
      if(isalpha(buf[i]))
         buf[i] = tolower(buf[i]);

   for(i = 0 ; i < check_ri; i++)
      if(!strcmp(buf, users[i].userid)) {
         flag = 1;
         if(!mode) users[i].count++;
         break;
      }

   if(i == check_ri && !mode) {
      check_ri++;
      strcpy(users[i].userid, buf);
   }

   return flag;
}

// ats 1.30 append
int check_u_name(userec *u) {
   int i, j;

   i = strlen(u->userid);
   if(i > IDLEN || i < 2) return -1;

   if(!isalpha(u->userid[0])) return 1;

   for(j = 0 ; j < i ; j++)
      if(!isalpha(u->userid[j]) && !isdigit(u->userid[j])) return -1;

   return 0;
}

// ats 1.24 append
int check_b_name(boardheader *b) {
   int i, j;

   i = strlen(b->brdname);
   if(i >= (IDLEN+1)*2 || i < 2) return 1;

   for(j = 0 ; j < i ; j++)
      if(!isalpha(b->brdname[j]) && !isdigit(b->brdname[j]) &&
         b->brdname[j] != '-' && b->brdname[j] != '_' &&
         b->brdname[j] != '.')
      return -1;

   return 0;
}

// ats 1.31 append
int check_exist_board(char *boardname) {
   int fg = 0, k;
   FILE *fs;
   boardheader bh;
   char buf[200];

   sprintf(buf, BBSHOME "/backup/BOARDS.ACCESS");
   fs = fopen(buf, "rb");
   if(!fs) return -1;

   while((fread(&bh, sizeof(bh), 1, fs)) == 1) {
      if(!check_b_name(&bh)) {
         if(!strcmp(boardname, bh.brdname)) {
            fg = 1;
            break;
         }
      }
      else {
         fg = -2;
         break;
      }
   }

   fclose(fs);
   return fg;
}

// ats 1.31 append
check_u_dated(userec *u) {
   time_t now;

   now = time(NULL);
   if(now - u->lastlogin > 365 * 86400)
      printf("-- %s --\n", u->userid);
   return (now - u->lastlogin > 365 * 86400);
}

// record.c
void stampfile(char *fpath, fileheader *fh) {
  register char *ip = fpath;
  time_t dtime;
  struct tm *ptime;
  int fp;

#if 1
  if (access(fpath, X_OK | R_OK | W_OK))
    mkdir(fpath, 0755);
#endif

  time(&dtime);
  while (*(++ip));
  *ip++ = '/';
  do
  {
    sprintf(ip, "M.%d.A", ++dtime);
  } while ((fp = open(fpath, O_CREAT | O_EXCL | O_WRONLY, 0664)) == -1);
  close(fp);
  memset(fh, 0, sizeof(fileheader));
  strcpy(fh->filename, ip);
  ptime = localtime(&dtime);
  sprintf(fh->date, "%2d/%02d", ptime->tm_mon + 1, ptime->tm_mday);
}

// record.c
static int safewrite(int fd, char *buf, int size) {
  int cc, sz = size, origsz = size;
  char *bp = buf;

#ifdef POSTBUG
  if(size == sizeof(fileheader)) {
    char foo[80];
    struct stat stbuf;
    fileheader *fbuf = (fileheader *) buf;

    setbpath(foo, fbuf->filename);
    if (!isalpha(fbuf->filename[0]) || stat(foo, &stbuf) == -1)
      if (fbuf->filename[0] != 'M' || fbuf->filename[1] != '.')
      {
#ifdef  HAVE_REPORT
        report("safewrite: foiled attempt to write bugged record\n");
#endif
        return origsz;
      }
  }
#endif

  do
  {
    cc = write(fd, bp, sz);
    if ((cc < 0) && (errno != EINTR))
    {
#ifdef  HAVE_REPORT
      report("safewrite failed!");
#endif
      return -1;
    }
    if (cc > 0)
    {
      bp += cc;
      sz -= cc;
    }
  } while (sz > 0);
  return origsz;
}

// record.c
int append_record(char *fpath, char *record, int size) {
  int fd;

#ifdef POSTBUG
  int numrecs = (int) get_num_records(fpath, size);

  bug_possible = 1;
  if(size == sizeof(fileheader) && numrecs && (numrecs % 4 == 0))
    saverecords(fpath, size, numrecs + 1);
#endif
  if((fd = open(fpath, O_WRONLY | O_CREAT, 0664)) == -1) {
    perror("open");
    return -1;
  }
  flock(fd, LOCK_EX);
  lseek(fd, 0, SEEK_END);

  safewrite(fd, record, size);

  flock(fd, LOCK_UN);
  close(fd);

#ifdef POSTBUG
  if (size == sizeof(fileheader) && numrecs && (numrecs % 4 == 0))
    restorerecords(fpath, size, numrecs + 1);
  bug_possible = 0;
#endif

  return 0;
}

// kaede.c
Rename(char* src, char* dst) {
   char cmd[200];

   if (rename(src, dst) == 0)
      return 0;

   sprintf(cmd, "/bin/mv %s %s", src, dst);
   return system(cmd);
}

// ats 1.31 append
int update_board(boardheader *abh, char *boardname, char mode) {
   char src[100], tar[100];
   int fg = 0;
   FILE *fs, *ft;
   boardheader bh;

   sprintf(src, BBSHOME "/backup/BOARDS.ACCESS");
   fs = fopen(src, "rb");
   if(!fs) {
      printf("%s Open Error\n", src);
      return -1;
   }
   sprintf(tar, BBSHOME "/backup/BOARDS.UPDATE");
   ft = fopen(tar, "wb+");
   if(!ft) {
      fclose(fs);
      printf("%s Open Error\n", tar);
      return -2;
   }

   while(!feof(fs) && !ferror(fs)) {
      if(fread(&bh, sizeof(bh), 1, fs) == 1) {
         if(!check_b_name(&bh)) {
            if(mode == 'A' && strcmp(abh->brdname, bh.brdname) ||
               mode == 'D' && strcmp(boardname, bh.brdname))
               fwrite(&bh, sizeof(bh), 1, ft);
            else {
               if(mode == 'A') {
                  fg = -3;
                  printf("Board: %s Exist ! Skip Append ...\n", abh->brdname);
                  break;
               }
               if(mode == 'D')
                  continue;
            }
         }
      }
   }

   if(mode == 'A' && !fg) fwrite(abh, sizeof(boardheader), 1, ft);
   fclose(fs); fclose(ft);
   Rename(tar, src);

   return fg;
}

// ats 1.31 append
void get_tm_time(int *year, char *month, char *day, char *hour, char *min) {
   time_t now = time(NULL);
   struct tm *ptime;

   ptime = localtime(&now);

   if(year) *year = (int)ptime->tm_year+1900;
   if(month) *month = (char)ptime->tm_mon+1;
   if(day) *day = (char)ptime->tm_mday;
   if(hour) *hour = (char)ptime->tm_hour;
   if(min) *min = (char)ptime->tm_min;
}

// ats 1.31 append
int check_mmdd(char *date_str, char month, char day) {
   char buf[10], m, d;

   if(strlen(date_str) > 5) return -2;
   strcpy(buf, date_str);
   buf[2] = buf[5] = 0;
   m = (char)atoi(buf);
   d = (char)atoi(&buf[3]);

   if(m < 1 || m > 12 || d < 1 || d > 31) return -2;
   if(month < 1 || month > 12 || day < 1 || day > 31) return -2;

   if(m > month)
      if(m > 8 && month < 4) return 1;  /* ¤ë¥÷°_ÂI¶W¹L¤K¤ë ²×ÂI¤p©ó¥|¤ë */
      else return -1;                   /* ·í§@®É¶¡¥¼¨ì¸ÑÅª */

   if(m < month)
      if(m < 4 && month > 8) return -1;
      else return 1;

   if(d > day) return -1;
   else if(d == day) return 0;
   else return 1;
}

// ats 1.31 append
void void_log(char mode, char *fpath, char *board, char *title) {
   fileheader fhdr = {0};
   char genbuf[256];
   char *flog;

   if(mode == 'B') {
      if(!board)
         board = "SYSOP";
      sprintf(genbuf, "boards/%s", board);
   }
   if(mode == 'S')
      strcpy(genbuf, "man/boards/Note/OrderSongs");

   stampfile(genbuf, &fhdr);
   Rename(fpath, genbuf);

   strcpy(fhdr.title, title);
   strcpy(fhdr.owner, "<SYSTEM>");
   if(mode == 'B')
      sprintf(genbuf, "boards/%s/.DIR", board);
   if(mode == 'S')
      strcpy(genbuf, "man/boards/Note/OrderSongs/.DIR");

   append_record(genbuf, (char *) &fhdr, sizeof(fhdr));
}

void addorg(int mode, FILE *fp) {
     fprintf(fp,
     "\n--\n[1;34;40m¡° [mOrigin: [1;31;40mATLANTIS[m  DN: [1;32;40mats.twbbs.org[m"
     "  IP : [1;33;40m140.129.66.189[m");
}

// stuff.c
int belong(char *filelist, char *key) {
  FILE *fp;
  int rc = 0;

  if (fp = fopen(filelist, "r"))
  {
    char buf[STRLEN], *ptr;

    while (fgets(buf, STRLEN, fp))
    {
      if ((ptr = strtok(buf, str_space)) && !strcasecmp(ptr, key))
      {
        rc = 1;
        break;
      }
    }
    fclose(fp);
  }
  return rc;
}

// stuff.c
char *Cdate(time_t *clock) {
  static char foo[22];
  struct tm *mytm = localtime(clock);

  strftime(foo, 22, "%D %T %a", mytm);
  return (foo);
}

// ATS 1.31 Append
void del_b_anyway(char *boardname) {
   char buf[200];

   sprintf(buf, "rm -fr boards/%s man/boards/%s", boardname, boardname);
   system(buf);

   update_board(NULL, boardname, 'D');

}

// ATS 1.32 Append
#ifndef FROMHOST_BY_FILE
struct fh2st {
   char host_type;     /* 0: strcmp 1:strstr */
   char fromhost[42];  /* ³oÃä®ö¶O¤T¦r¤¸ ¬°ªº¥u¬O ¹ï»ô 64 bytes ¤ñ¸û§Ö */
   char transto[21];
};
typedef struct fh2st fh2st;
#endif

#ifndef TAKE_IDLE
static char *IdleTypeTable[] = {
  "°¸¦bªá§b°Õ",
  "±¡¤H¨Ó¹q",
  "³V­¹¤¤",
  "«ô¨£©P¤½",
  "°²¦ºª¬ºA",
  "§Ú¦b«ä¦Ò"
};
#endif

#ifndef  _MODES_C_
static char *ModeTypeTable[] = {
  "µo§b",                       /* IDLE */
  "¥D¿ï³æ",                     /* MMENU */
  "¨t²ÎºûÅ@",                   /* ADMIN */
  "¶l¥ó¿ï³æ",                   /* MAIL */
  "¥æ½Í¿ï³æ",                   /* TMENU */
  "¨Ï¥ÎªÌ¿ï³æ",                 /* UMENU */
  "XYZ ¿ï³æ",                   /* XMENU */
  "¤ÀÃþ¬ÝªO",                   /* CLASS */
  "¤W¯¸³~¤¤",                   /* LOGIN */
  "¤½§GÄæ",                     /* ANNOUNCE */
  "µoªí¤å³¹",                   /* POSTING */
  "¬ÝªO¦Cªí",                   /* READBRD */
  "¾\\Åª¤å³¹",                  /* READING */
  "·s¤å³¹¦Cªí",                 /* READNEW */
  "¿ï¾Ü¬ÝªO",                   /* SELECT */
  "Åª«H",                       /* RMAIL */
  "¼g«H",                       /* SMAIL */
  "²á¤Ñ«Ç",                     /* CHATING */
  "¨ä¥L",                       /* XMODE */
  "´M§ä¦n¤Í",                   /* FRIEND */
  "¤W½u¨Ï¥ÎªÌ",                 /* LAUSERS */
  "¨Ï¥ÎªÌ¦W³æ",                 /* LUSERS */
  "°lÂÜ¯¸¤Í",                   /* MONITOR */
  "©I¥s",                       /* PAGE */
  "¬d¸ß",                       /* QUERY */
  "¥æ½Í",                       /* TALK  */
  "½s¦W¤ùÀÉ",                   /* EDITPLAN */
  "½sÃ±¦WÀÉ",                   /* EDITSIG */
  "§ë²¼¤¤",                     /* VOTING */
  "³]©w¸ê®Æ",                   /* XINFO */
  "±Hµ¹¯¸ªø",                   /* MSYSOP */
  "¨L¨L¨L",                     /* WWW */
  "¥´¾ôµP",                     /* BRIDGE */
  "¦^À³",                       /* REPLY */
  "³Q¤ô²y¥´¤¤",                 /* HIT */
  "¤ô²y·Ç³Æ¤¤",                 /* DBACK */
  "µ§°O¥»",                     /* NOTE */
  "½s¿è¤å³¹",                   /* EDITING */
  "µo¨t²Î³q§i",                 /* MAILALL */
  "¨È¯¸¥\\¯à°Ï",
  "¬ü¤ÖÂû¹Ú¤u³õ"
};
#endif                          /* _MODES_C_ */
