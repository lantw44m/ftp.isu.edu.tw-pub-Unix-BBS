/*-------------------------------------------------------*/
/* record.c     ( NTHU CS MapleBBS Ver 2.36 )            */
/*-------------------------------------------------------*/
/* target : binary record file I/O routines              */
/* create : 95/03/29                                     */
/* update : 03/04/28 (Dopin)                             */
/*-------------------------------------------------------*/

#include "bbs.h"

#undef  HAVE_MMAP
#ifdef  HAVE_MMAP
#include <sys/mman.h>
#endif

#define BUFSIZE 1024

#ifdef SYSV
int flock(int fd, int op) {
  switch(op) {
    case LOCK_EX: return lockf(fd, F_LOCK, 0);
    case LOCK_UN: return lockf(fd, F_ULOCK, 0);
    default: return -1;
  }
}
#endif

#ifdef  POSTBUG
char bigbuf[10240];
int numtowrite;
int bug_possible = 0;

saverecords(char *fpath, int size, int pos) {
  int fd;

  if(!bug_possible) return 0;
  if((fd = open(fpath, O_RDONLY)) == -1) return -1;
  if(pos > 5) numtowrite = 5;
  else numtowrite = 4;
  lseek(fd, (pos - numtowrite - 1) * size, L_SET);
  read(fd, bigbuf, numtowrite * size);
}

restorerecords(char *fpath, int size, int pos) {
  int fd;

  if(!bug_possible) return 0;
  if((fd = open(fpath, O_WRONLY)) == -1) return -1;
  flock(fd, LOCK_EX);
  lseek(fd, (pos - numtowrite - 1) * size, L_SET);
  safewrite(fd, bigbuf, numtowrite * size);

#ifdef  HAVE_REPORT
  report("post bug poison set out!");
#endif

  flock(fd, LOCK_UN);
  bigbuf[0] = '\0';
  close(fd);
}


/* 每次寫一點點，以策安全 */

static int safewrite(int fd, char *buf, int size) {
  int cc, sz = size, origsz = size;
  char *bp = buf;

#ifdef POSTBUG
  if(size == sizeof(fileheader)) {
    char foo[80];
    struct stat stbuf;
    fileheader *fbuf = (fileheader *) buf;

    setbpath(foo, fbuf->filename);
    if(!isalpha(fbuf->filename[0]) || stat(foo, &stbuf) == -1)
      if(fbuf->filename[0] != 'M' || fbuf->filename[1] != '.') {
#ifdef  HAVE_REPORT
        report("safewrite: foiled attempt to write bugged record\n");
#endif
        return origsz;
      }
  }
#endif

  do {
    cc = write(fd, bp, sz);
    if((cc < 0) && (errno != EINTR)) {
#ifdef  HAVE_REPORT
      report("safewrite failed!");
#endif
      return -1;
    }
    if(cc > 0) {
      bp += cc;
      sz -= cc;
    }
  } while (sz > 0);
  return origsz;
}
#else

#define safewrite       write
#endif                          /* POSTBUG */

#if !defined(_BBS_UTIL_C_)
long get_num_records(char *fpath, int size) {
  struct stat st;

  if (stat(fpath, &st) == -1)
    return 0;
  return (st.st_size / size);
}

getindex(char* fpath, char* fname) {
   int fd;

   if((fd = open(fpath, O_RDONLY, 0)) != -1) {
      struct stat st;
      fileheader* fhdr;
      int num, i;

      fstat(fd, &st);
      if(fhdr = malloc(st.st_size)) {
         num = read(fd, fhdr, st.st_size) / sizeof(fileheader);
         close(fd);
      }
      else {
         close(fd);
         return 0;
      }

      for(i = 0; i < num; i++)
        if(!strcasecmp(fhdr[i].filename, fname)) {
           free(fhdr);
           return ++i;
        }

      free(fhdr);
   }
   return 0;
}

get_sum_records(char* fpath, int size) {
   struct stat st;
   long ans = 0;
   FILE* fp;
   fileheader fhdr;
   char buf[200], *p;

   if (!(fp = fopen(fpath, "r")))
      return -1;

   strcpy(buf, fpath);
   p = strrchr(buf, '/') + 1;

   while (fread(&fhdr, size, 1, fp) == 1) {
      strcpy(p, fhdr.filename);
      if (stat(buf, &st) == 0 && S_ISREG(st.st_mode) && st.st_nlink == 1)
         ans += st.st_size;
   }
   fclose(fp);
   return ans / 1024;

}

int get_record(char *fpath, char *rptr, int size, int id) {
  int fd;

  if(!id) return -1; /* Dopin: by itoc, 當讀入的 id index 值為 0 時 不執行 */
  if((fd = open(fpath, O_RDONLY, 0)) != -1) {
    if(lseek(fd, size * (id - 1), SEEK_SET) != -1) {
      if(read(fd, rptr, size) == size) {
        close(fd);
        return 0;
      }
    }
    close(fd);
  }

  return -1;
}

int get_records(char *fpath, char *rptr, int size, int id, int number) {
  int fd;

  if((fd = open(fpath, O_RDONLY, 0)) == -1) return -1;

#ifdef  HAVE_MMAP
  {
    int offset;
    caddr_t fimage;
    struct stat stbuf;

    fstat(fd, &stbuf);
    fimage = mmap(NULL, stbuf.st_size, PROT_READ, MAP_SHARED, fd, 0);
    close(fd);

    if(fimage == (char *) -1) return -1;
    offset = size * (id - 1);
    id = size * number;
    number = stbuf.st_size - offset;

    if(id > number) id = number;
    memcpy(rptr, fimage + offset, n);
    munmap(fimage, stbuf.st_size);
  }
#else
  if(lseek(fd, size * (id - 1), SEEK_SET) == -1) {
    close(fd);
    return 0;
  }

  if((id = read(fd, rptr, size * number)) == -1) {
    close(fd);
    return -1;
  }

  close(fd);
#endif

  return (id / size);
}

int substitute_record(char *fpath, char *rptr, int size, int id) {
  int fd;

#ifdef POSTBUG
  if (size == sizeof(fileheader) && (id > 1) && ((id - 1) % 4 == 0))
    saverecords(fpath, size, id);
#endif

  if(!id) return -1; /* Dopin: by itoc, 當寫入的 id index 值為 0 時 不寫入 */
  if((fd = open(fpath, O_WRONLY | O_CREAT, 0664)) == -1) return -1;
  flock(fd, LOCK_EX);

#ifdef  HAVE_REPORT
  if(lseek(fd, size * (id - 1), SEEK_SET) == -1)
    report("substitute_record failed!!! (lseek)");
  if(safewrite(fd, rptr, size) != size)
    report("substitute_record failed!!! (safewrite)");
#else
  lseek(fd, size * (id - 1), SEEK_SET);
  safewrite(fd, rptr, size);
#endif

  flock(fd, LOCK_UN);
  close(fd);

#ifdef POSTBUG
  if(size == sizeof(fileheader) && (id > 1) && ((id - 1) % 4 == 0))
    restorerecords(fpath, size, id);
#endif

  return 0;
}

/* ---------------------------- */
/* new/old/lock file processing */
/* ---------------------------- */

typedef struct {
  char newfn[MAXPATHLEN];
  char oldfn[MAXPATHLEN];
  char lockfn[MAXPATHLEN];
}      nol;

static void nolfilename(nol *n, char *fpath) {
  sprintf(n->newfn, "%s.new", fpath);
  sprintf(n->oldfn, "%s.old", fpath);
  sprintf(n->lockfn, "%s.lock", fpath);
}

int delete_record(char *fpath, int size, int id) {
  nol my;
  char abuf[BUFSIZE+4];
  int fdr, fdw, fd;
  int count;

  nolfilename(&my, fpath);

  if((fd = open(my.lockfn, O_RDWR | O_CREAT | O_APPEND, 0664)) == -1)
     return -1;
  flock(fd, LOCK_EX);

  if((fdr = open(fpath, O_RDONLY, 0)) == -1) {
#ifdef  HAVE_REPORT
    report("delete_record failed!!! (open)");
#endif
    flock(fd, LOCK_UN);
    close(fd);
    return -1;
  }

  if((fdw = open(my.newfn, O_WRONLY | O_CREAT | O_EXCL, 0664)) == -1) {
    flock(fd, LOCK_UN);
#ifdef  HAVE_REPORT
    report("delete_record failed!!! (open tmpfile)");
#endif
    close(fd);
    close(fdr);
    return -1;
  }

  count = 1;
  while(read(fdr, abuf, size) == size) {
    if(id != count++ && (safewrite(fdw, abuf, size) == -1)) {
#ifdef  HAVE_REPORT
      report("delete_record failed!!! (safewrite)");
#endif
      unlink(my.newfn);
      close(fdr);
      close(fdw);
      flock(fd, LOCK_UN);
      close(fd);
      return -1;
    }
  }
  close(fdr);
  close(fdw);

  if(Rename(fpath, my.oldfn) == -1 || Rename(my.newfn, fpath) == -1) {
#ifdef  HAVE_REPORT
    report("delete_record failed!!! (Rename)");
#endif
    flock(fd, LOCK_UN);
    close(fd);
    return -1;
  }

  flock(fd, LOCK_UN);
  close(fd);
  return 0;
}

char *title_body(char *title) {
  if(!strncasecmp(title, str_reply, 3)) {
    title += 3;
    if(*title == ' ') title++;
  }
  return title;
}

int delete_range(char *fpath, int id1, int id2, char *title) {
  fileheader fhdr;
  nol my;
  char fullpath[STRLEN], *t;
  int fdr, fdw, fd;
  int count;

  nolfilename(&my, fpath);

  if((fd = open(my.lockfn, O_RDWR | O_CREAT | O_APPEND, 0664)) == -1)
     return -1;
  flock(fd, LOCK_EX);

  if((fdr = open(fpath, O_RDONLY, 0)) == -1) {
    flock(fd, LOCK_UN);
    close(fd);
    return -1;
  }

  if((fdw = open(my.newfn, O_WRONLY | O_CREAT | O_EXCL, 0664)) == -1) {
    close(fdr);
    flock(fd, LOCK_UN);
    close(fd);
    return -1;
  }

  count = 1;
  strcpy(fullpath, fpath);
  t = (char *) strrchr(fullpath, '/') + 1;

  while(read(fdr, &fhdr, sizeof(fileheader)) == sizeof(fileheader)) {
    if((count < id1 || count > id2 ||
       fhdr.filemode & FILE_MARKED ||
       fhdr.filemode & FILE_DIGEST ) &&
       (!title || strcmp(title_body(title), title_body(fhdr.title)))) {
      if((safewrite(fdw, &fhdr, sizeof(fileheader)) == -1)) {
        close(fdr);
        close(fdw);
        unlink(my.newfn);
        flock(fd, LOCK_UN);
        close(fd);
        return -1;
      }
    }
    else {
      strcpy(t, fhdr.filename);
      unlink(fullpath);
    }
    count++;
  }
  close(fdr);
  close(fdw);

  if(Rename(fpath, my.oldfn) == -1 || Rename(my.newfn, fpath) == -1) {
    flock(fd, LOCK_UN);
    close(fd);
    return -1;
  }

  flock(fd, LOCK_UN);
  close(fd);
  return 0;
}

int update_file(dirname, size, ent, filecheck, fileupdate)
  char *dirname;
  int size, ent;
  int (*filecheck) ();
  void (*fileupdate) ();
{
  char abuf[BUFSIZE+4];
  int fd;

  if((fd = open(dirname, O_RDWR)) == -1) return -1;
  flock(fd, LOCK_EX);

  if(lseek(fd, size * (ent - 1), SEEK_SET) != -1) {
    if(read(fd, abuf, size) == size)
      if((*filecheck) (abuf)) {
        lseek(fd, -(off_t)size, SEEK_CUR);
        (*fileupdate) (abuf);
        if(safewrite(fd, abuf, size) != size) {
#ifdef  HAVE_REPORT
          report("update_file failed!!! (safewrite)");
#endif
          flock(fd, LOCK_UN);
          close(fd);
          return -1;
        }

        flock(fd, LOCK_UN);
        close(fd);
        return 0;
      }
  }

  lseek(fd, 0, SEEK_SET);
  while(read(fd, abuf, size) == size) {
    if((*filecheck) (abuf)) {
      lseek(fd, -(off_t)size, SEEK_CUR);
      (*fileupdate) (abuf);
      if(safewrite(fd, abuf, size) == size) {
#ifdef  HAVE_REPORT
        report("update_file failed!!! (safewrite)");
#endif
        flock(fd, LOCK_UN);
        close(fd);
        return 0;
      }
      break;
    }
  }

  flock(fd, LOCK_UN);
  close(fd);
  return -1;
}

/* woju */
int search_rec(char* dirname, int (*filecheck)()) {
   fileheader fhdr;
   FILE *fp;
   int ans = 0;

   if(!(fp = fopen(dirname, "r"))) return 0;

   while(fread(&fhdr, sizeof(fhdr), 1, fp)) {
     ans++;
     if((*filecheck) (&fhdr)) {
        fclose(fp);
        return ans;
     }
   }

   fclose(fp);
   return 0;
}

int delete_files(char* dirname, int (*filecheck)()) {
   fileheader fhdr;
   FILE *fp, *fptmp;
   int ans = 0;
   char tmpfname[100];
   char genbuf[200];

   if(!(fp = fopen(dirname, "r"))) return ans;

   strcpy(tmpfname, dirname);
   strcat(tmpfname, "_tmp");

   if(!(fptmp = fopen(tmpfname, "w"))) {
      fclose(fp);
      return ans;
   }

   while(fread(&fhdr, sizeof(fhdr), 1, fp))
     if((*filecheck) (&fhdr)) {
        ans++;
        setdirpath(genbuf, dirname, fhdr.filename);
        unlink(genbuf);
     }
     else fwrite(&fhdr, sizeof(fhdr), 1, fptmp);

   fclose(fp);
   fclose(fptmp);
   unlink(dirname);
   Rename(tmpfname, dirname);

   return ans;
}

int delete_file(dirname, size, ent, filecheck)
  char *dirname;
  int size, ent;
  int (*filecheck) ();
{
  char abuf[BUFSIZE+4];
  int fd;
  struct stat st;
  long numents;

  if((fd = open(dirname, O_RDWR)) == -1) return -1;
  flock(fd, LOCK_EX);
  fstat(fd, &st);
  numents = ((long) st.st_size) / size;

  if(((long) st.st_size) % size) fprintf(stderr, "align err\n");
  if(lseek(fd, size * (ent - 1), SEEK_SET) != -1) {
    if(read(fd, abuf, size) == size)
      if((*filecheck) (abuf)) {
        int i;

        for(i = ent; i < numents; i++) {
          if(lseek(fd, i * size, SEEK_SET) == -1) break;
          if(read(fd, abuf, size) != size) break;
          if(lseek(fd, (i - 1) * size, SEEK_SET) == -1) break;
          if(safewrite(fd, abuf, size) != size) break;
        }

        ftruncate(fd, (off_t) size * (numents - 1));
        flock(fd, LOCK_UN);
        close(fd);
        return 0;
      }
  }
  lseek(fd, 0, SEEK_SET);
  ent = 1;

  while(read(fd, abuf, size) == size) {
    if((*filecheck) (abuf)) {
      int i;

      for(i = ent; i < numents; i++) {
        if(lseek(fd, (i + 1) * size, SEEK_SET) == -1) break;
        if(read(fd, abuf, size) != size) break;
        if(lseek(fd, i * size, SEEK_SET) == -1) break;
        if(safewrite(fd, abuf, size) != size) break;
      }

      ftruncate(fd, (off_t) size * (numents - 1));
      flock(fd, LOCK_UN);
      close(fd);
      return 0;
    }
    ent++;
  }
  flock(fd, LOCK_UN);
  close(fd);

  return -1;
}

int search_record(fpath, rptr, size, fptr, farg)
  char *fpath;
  char *rptr;
  int size;
  int (*fptr) ();
  int farg;
{
  int fd;
  int id = 1;

  if((fd = open(fpath, O_RDONLY, 0)) == -1) return 0;
  while(read(fd, rptr, size) == size) {
    if((*fptr) (farg, rptr)) {
      close(fd);
      return id;
    }
    id++;
  }
  close(fd);
  return 0;
}
#endif                          /* !defined(_BBS_UTIL_C_) */

/* woju */
int apply_record(fpath, fptr, size)
  char *fpath;
  int(*fptr)();
  int size;
{
  char abuf[BUFSIZE+4];
  FILE* fp;

  if(!(fp = fopen(fpath, "r"))) return -1;

  while(fread(abuf, 1, size, fp) == size)
    if((*fptr) (abuf) == QUIT) {
      fclose(fp);
      return QUIT;
    }

  fclose(fp);
  return 0;
}

/* ------------------------------------------ */
/* mail / post 時，依據時間建立檔案，加上郵戳 */
/* ------------------------------------------ */
/* Input: fpath = directory; Output: fpath = full path; */

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
  do {
    sprintf(ip, "M.%d.A", ++dtime);
  } while ((fp = open(fpath, O_CREAT | O_EXCL | O_WRONLY, 0664)) == -1);

  close(fp);
  memset(fh, 0, sizeof(fileheader));
  strcpy(fh->filename, ip);
  ptime = localtime(&dtime);
  sprintf(fh->date, "%2d/%02d", ptime->tm_mon + 1, ptime->tm_mday);
}

/*
    ===== Added by mgtsai, Sep 10th, '96 =====
*/

void stampdir(char *fpath, fileheader *fh) {
  register char *ip = fpath;
  time_t dtime;
  struct tm *ptime;

  if(access(fpath, X_OK | R_OK | W_OK)) mkdir(fpath, 0755);

  time(&dtime);
  while (*(++ip));
  *ip++ = '/';

  do {
    sprintf(ip, "D.%d.A", ++dtime);
  } while (mkdir(fpath, 0755) == -1);
  memset(fh, 0, sizeof(fileheader));
  strcpy(fh->filename, ip);
  ptime = localtime(&dtime);
  sprintf(fh->date, "%2d/%02d", ptime->tm_mon + 1, ptime->tm_mday);
}

void stamplink(char *fpath, fileheader *fh) {
  register char *ip = fpath;
  time_t dtime;
  struct tm *ptime;

  if(access(fpath, X_OK | R_OK | W_OK)) mkdir(fpath, 0755);

  time(&dtime);
  while (*(++ip));
  *ip++ = '/';

  do {
    sprintf(ip, "S.%d.A", ++dtime);
  } while (symlink("temp", fpath) == -1);

  memset(fh, 0, sizeof(fileheader));
  strcpy(fh->filename, ip);
  ptime = localtime(&dtime);
  sprintf(fh->date, "%2d/%02d", ptime->tm_mon + 1, ptime->tm_mday);
}
/* ===== end ===== */

int append_record(char *fpath, char *record, int size) {
  int fd;

#ifdef POSTBUG
  int numrecs = (int) get_num_records(fpath, size);

  bug_possible = 1;
  if (size == sizeof(fileheader) && numrecs && (numrecs % 4 == 0))
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
  if(size == sizeof(fileheader) && numrecs && (numrecs % 4 == 0))
    restorerecords(fpath, size, numrecs + 1);
  bug_possible = 0;
#endif

  return 0;
}
