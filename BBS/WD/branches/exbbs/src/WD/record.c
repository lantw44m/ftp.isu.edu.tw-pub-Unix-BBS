/*-------------------------------------------------------*/
/* record.c     ( NTHU CS MapleBBS Ver 2.36 )            */
/*-------------------------------------------------------*/
/* target : binary record file I/O routines              */
/* create : 95/03/29                                     */
/* update : 95/12/15                                     */
/*-------------------------------------------------------*/

#include "bbs.h"

#ifdef SYSV
int
flock(fd, op)
  int fd, op;
{
  switch (op)
  {
  case LOCK_EX:
    return lockf(fd, F_LOCK, 0);
  case LOCK_UN:
    return lockf(fd, F_ULOCK, 0);
  default:
    return -1;
  }
}
#endif


long
get_num_records(fpath, size)
  char *fpath;
{
  struct stat st;
  if (stat(fpath, &st) == -1)
    return 0;
  return (st.st_size / size);
}


get_sum_records(char* fpath, int size)
{
   struct stat st;
   long ans = 0;
   FILE* fp;
   fileheader fhdr;
   char buf[200], *p;

   if (!(fp = fopen(fpath, "r")))
      return -1;

   strcpy(buf, fpath);
   p = strrchr(buf, '/') + 1;

// �֧�o�̧令 &st ??
   while (fread(&fhdr, size, 1, fp) == 1) {
      strcpy(p, fhdr.filename);
      if (stat(buf, &st) == 0 && S_ISREG(st.st_mode) && st.st_nlink == 1)
         ans += st.st_size;
   }
   fclose(fp);
   return ans / 1024;
}

int
get_records(fpath, rptr, size, id, number)
  char *fpath, *rptr;
  int size, id, number;
{
  int fd;

  if ((fd = open(fpath, O_RDONLY, 0)) == -1)
    return -1;

  if (lseek(fd, (off_t)(size * (id - 1)), SEEK_SET) == -1)
  {
    close(fd);
    return 0;
  }
  if ((id = read(fd, rptr, size * number)) == -1)
  {
    close(fd);
    return -1;
  }
  close(fd);

  return (id / size);
}


int
substitute_record(fpath, rptr, size, id)
  char *fpath;
  char *rptr;
  int size, id;
{
  int fd;
  if ((fd = open(fpath, O_WRONLY | O_CREAT, 0644)) == -1)
    return -1;
  flock(fd, LOCK_EX);

  lseek(fd, (off_t) (size * (id - 1)), SEEK_SET);
  write(fd, rptr, size);

  flock(fd, LOCK_UN);
  close(fd);

  return 0;
}

#if !defined(_BBS_UTIL_C_)
/* ---------------------------- */
/* new/old/lock file processing */
/* ---------------------------- */

typedef struct  /* Ptt*/
{
  char newfn[256];
  char oldfn[256];
  char lockfn[256];
}      nol;


static void
nolfilename(n, fpath)
  nol *n;
  char *fpath;
{
  sprintf(n->newfn, "%s.new", fpath);
  sprintf(n->oldfn, "%s.old", fpath);
  sprintf(n->lockfn, "%s.lock", fpath);
}


int
rec_del(fpath, size, id)
  char fpath[];
  int size, id;
{
  nol my;
  char abuf[1024];
  int fdr, fdw, fd;
  int count;

  nolfilename(&my, fpath);
  if ((fd = open(my.lockfn, O_RDWR | O_CREAT | O_APPEND, 0644)) == -1)
    return -1;
  flock(fd, LOCK_EX);
  if ((fdr = open(fpath, O_RDONLY, 0)) == -1)
  {
    flock(fd, LOCK_UN);
    close(fd);
    return -1;
  }

  if ((fdw = open(my.newfn, O_WRONLY | O_CREAT | O_EXCL, 0644)) == -1)
  {
    flock(fd, LOCK_UN);
    close(fd);
    close(fdr);
    return -1;
  }

  count = 1;
  while (read(fdr, abuf, size) == size)
  {
    if (id != count++ && (write(fdw, abuf, size) == -1))
    {
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
  if (f_mv(fpath, my.oldfn) == -1 || f_mv(my.newfn, fpath) == -1)
  {
    flock(fd, LOCK_UN);
    close(fd);
    return -1;
  }
  flock(fd, LOCK_UN);
  close(fd);
  return 0;
}

#if 0
char *
title_body(title)
  char *title;
{
  if (!strncasecmp(title, str_reply, 3))
  {
    title += 3;
    if (*title == ' ')
      title++;
  }
  return title;
}
#endif

int
delete_range(fpath, id1, id2)
  char *fpath;
  int id1, id2;
{
  fileheader fhdr;
  nol my;
  char fullpath[STRLEN], *t;
  int fdr, fdw, fd, count, number=0, payback=0;

  nolfilename(&my, fpath);

  if ((fd = open(my.lockfn, O_RDWR | O_CREAT | O_APPEND, 0644)) == -1)
    return -1;
  flock(fd, LOCK_EX);

  if ((fdr = open(fpath, O_RDONLY, 0)) == -1)
  {
    flock(fd, LOCK_UN);
    close(fd);
    return -1;
  }

  if ((fdw = open(my.newfn, O_WRONLY | O_CREAT | O_EXCL, 0644)) == -1)
  {
    close(fdr);
    flock(fd, LOCK_UN);
    close(fd);
    return -1;
  }
  count = 1;
  strcpy(fullpath, fpath);
  t = (char *) strrchr(fullpath, '/') + 1;

  while (read(fdr, &fhdr, sizeof(fileheader)) == sizeof(fileheader))
  {
    strcpy(t, fhdr.filename);
    if (count < id1 || count > id2 || 
         fhdr.filemode & FILE_MARKED ||
         fhdr.filemode & FILE_DIGEST || dashd(fullpath))
    {
      if ((write(fdw, &fhdr, sizeof(fileheader)) == -1))
      {
        close(fdr);
        close(fdw);
        unlink(my.newfn);
        flock(fd, LOCK_UN);
        close(fd);
        return -1;
      }
    }
    else
    {
      number++;
      unlink(fullpath);
      if(strcmp(fhdr.owner, cuser.userid)==0)
        payback++;
    }
    count++;
  }
  close(fdr);
  close(fdw);
  
  if(currstat!=RMAIL && currstat !=ANNOUNCE)
  {
    if (cuser.goldmoney < payback)
      demoney((payback-cuser.goldmoney)*10000);
    degold(payback);
    deexp(payback*1024);
  }
  
  log_board3("PUR",currboard,number);
  if (f_mv(fpath, my.oldfn) == -1 || f_mv(my.newfn, fpath) == -1)
  {
    flock(fd, LOCK_UN);
    close(fd);
    return -1;
  }
  flock(fd, LOCK_UN);
  close(fd);
  return payback;
}


/* ----------------------------------------------------- */
/* id1:							 */
/* 0 ==> �̾� TagList �s��R��			 */
/* !0 ==> �̾� range [id1, id2] �R��		 */
/* ----------------------------------------------------- */

int
delete_range2(fpath, id1, id2)
  char *fpath;
  int id1, id2;
{
  fileheader fhdr;
  nol my;
  char fullpath[STRLEN], *t;
  int fdr, fdw, fd;
  register int count;

  nolfilename(&my, fpath);

  if ((fd = open(my.lockfn, O_RDWR | O_CREAT | O_APPEND, 0644)) < 0)
    return -1;
  flock(fd, LOCK_EX);

  if ((fdr = open(fpath, O_RDONLY, 0)) < 0)
  {
    flock(fd, LOCK_UN);
    close(fd);
    return -1;
  }

  if ((fdw = open(my.newfn, O_WRONLY | O_CREAT | O_EXCL, 0644)) < 0)
  {
    close(fdr);
    flock(fd, LOCK_UN);
    close(fd);
    return -1;
  }
  strcpy(fullpath, fpath);
  t = (char *) strrchr(fullpath, '/') + 1;

  count = 0;
  while (read(fdr, &fhdr, sizeof(fileheader)) == sizeof(fileheader))
  {
    count++;
    if ((fhdr.filemode & FILE_MARKED) ||	/* �аO */
      (id1 && (count < id1 || count > id2)) ||	/* range */
      (!id1 && Tagger(atoi(fhdr.filename+2), count, TAG_NIN)))	/* TagList */
    {
      if ((write(fdw, &fhdr, sizeof(fileheader)) < 0))
      {
	close(fdr);
	close(fdw);
	unlink(my.newfn);
	flock(fd, LOCK_UN);
	close(fd);
	return -1;
      }
    }
    else
    {
      strcpy(t, fhdr.filename);

      /* �Y���ݪO�N�s�u��H */

      if (currstat == READING)
      {
	cancel_post(&fhdr, fullpath);
      }

      unlink(fullpath);
    }
  }
  close(fdr);
  close(fdw);
  count = rename(fpath, my.oldfn);
  if (!count)
  {
    if (count = rename(my.newfn, fpath))
      rename(my.oldfn, fpath);	/* �U�@�X��A�A�Ϧ^�� */
  }
  flock(fd, LOCK_UN);
  close(fd);
  return count;
}


#if 0
int
update_file(dirname, size, ent, filecheck, fileupdate)
  char *dirname;
  int size, ent;
  int (*filecheck) ();
void (*fileupdate) ();
{
  char abuf[BUFSIZE];
  int fd;

  if ((fd = open(dirname, O_RDWR)) == -1)
    return -1;
  flock(fd, LOCK_EX);
  if (lseek(fd, (off_t)(size * (ent - 1)), SEEK_SET) != -1)
  {
    if (read(fd, abuf, size) == size)
      if ((*filecheck) (abuf))
      {
        lseek(fd,  -(off_t)size, SEEK_CUR);
        (*fileupdate) (abuf);
        if (write(fd, abuf, size) != size)
        {
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
  while (read(fd, abuf, size) == size)
  {
    if ((*filecheck) (abuf))
    {
      lseek(fd, -(off_t)size, SEEK_CUR);
      (*fileupdate) (abuf);
      if (write(fd, abuf, size) == size)
      {
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

#endif


/*
woju
*/
int
search_rec(char* dirname, int (*filecheck)())
{
   fileheader fhdr;
   FILE *fp;
   int ans = 0;

   if (!(fp = fopen(dirname, "r")))
      return 0;

   while (fread(&fhdr, sizeof(fhdr), 1, fp)) {
     ans++;
     if ((*filecheck) (&fhdr)) {
        fclose(fp);
        return ans;
     }
   }
   fclose(fp);
   return 0;
}

int
delete_files(char* dirname, int (*filecheck)())
{
   fileheader fhdr;
   FILE *fp, *fptmp;
   int ans = 0;
   char tmpfname[100];
   char genbuf[200];

// �ֶç�r??
// �Q�諸�ɶ��O 9/5 11:45
   if (!(fp = fopen(dirname, "r")))
      return ans;

   strcpy(tmpfname, dirname);
   strcat(tmpfname, "_tmp");

// �ֶç�̦n�ۤv�ۨ�
   if (!(fptmp = fopen(tmpfname, "w"))) {
      fclose(fp);
      return ans;
   }

   while (fread(&fhdr, sizeof(fhdr), 1, fp))
     if ((*filecheck) (&fhdr)) {
        ans++;
        setdirpath(genbuf, dirname, fhdr.filename);
        unlink(genbuf);
     }
     else
        fwrite(&fhdr, sizeof(fhdr), 1, fptmp);

   fclose(fp);
   fclose(fptmp);
   unlink(dirname);
   f_mv(tmpfname, dirname);

   return ans;
}


int
gem_files(char* dirname, int (*filecheck)())
{
   fileheader fhdr;
   FILE *fp;
   int c = 0 ,ans = 0;
   char genbuf[200];

   if (!(fp = fopen(dirname, "r")))
      return ans;

   /* wildcat.991216 : �߰ݬO�_���� tag */
   c = answer("�O�_�n�����аO ? (Y/n)");

   while (fread(&fhdr, sizeof(fhdr), 1, fp))
   {
     if ((*filecheck) (&fhdr)) 
     {
        char title[TTLEN+1] = "�� "; //��ذ� title �e�T��O�Ÿ�
        char copypath[512];
        fileheader item;
        int now;
        
        ans++;
        setdirpath(genbuf, dirname, fhdr.filename);

/* shakalaca.990705: �o�q�\���o.. :p 
        sprintf(buf,"cp -f %s %s/%s",genbuf, BBSHOME, paste_path); 
        system(buf);
*/

        strcpy(copypath, paste_path);
        stampfile(copypath, &item);
        unlink(copypath);

// wildcat : ��� f_cp 
        f_cp(genbuf, copypath, O_TRUNC); 

// wildcat : owner �ӬO�����̪� id ?!
	strcpy (item.owner, /* fhdr.owner*/ cuser.userid);
        /* shakalaca.990616: stamp �ت���, �A�N��l�ɫ��L�h,
         *                   ���e�S�o��, ���G���������D�]��F.. :(
         */
                          	
        strncpy(title + 3,fhdr.title, TTLEN-3);//�� fhdr.title ���Ჾ�T��space
        strcpy(item.title,title);              //�Aco�^��
        /* shakalaca.990616: ��ت����D, �ӫD��l���D
         *                   ����Ӽg�k�]�S��, �u�O�ګ᭱���� substitude_rec
         */
                          
        setadir (genbuf, paste_path); 
        rec_add(genbuf,&item,sizeof(item));    //maple2��append_record

	/* wildcat.991216 : �߰ݬO�_���� tag */
	if(c != 'n' && c != 'N')
	  fhdr.filemode ^= FILE_TAGED;
        now = getindex (dirname, fhdr.filename, sizeof (fileheader));
        substitute_record (dirname, &fhdr, sizeof (fhdr), now);
    }
  }  

  return ans;
}


int
delete_file(dirname, size, ent, filecheck)
  char *dirname;
  int size, ent;
  int (*filecheck) ();
{
  char abuf[1024];
  int fd;
  struct stat st;
  long numents;

  if ((fd = open(dirname, O_RDWR)) == -1)
    return -1;
  flock(fd, LOCK_EX);
  fstat(fd, &st);
  numents = ((long) st.st_size) / size;
  if (((long) st.st_size) % size)
    fprintf(stderr, "align err\n");
  if (lseek(fd,(off_t)( size * (ent - 1)), SEEK_SET) != -1)
  {
    if (read(fd, abuf, size) == size)
      if ((*filecheck) (abuf))
      {
        int i;

        for (i = ent; i < numents; i++)
        {
          if (lseek(fd, (off_t)((i) * size), SEEK_SET) == -1)
            break;
          if (read(fd, abuf, size) != size)
            break;
          if (lseek(fd, (off_t)((i - 1) * size), SEEK_SET) == -1)
            break;
          if (write(fd, abuf, size) != size)
            break;
        }
        ftruncate(fd, (off_t) size * (numents - 1));
        flock(fd, LOCK_UN);
        close(fd);
        return 0;
      }
  }
  lseek(fd, 0, SEEK_SET);
  ent = 1;
  while (read(fd, abuf, size) == size)
  {
    if ((*filecheck) (abuf))
    {
      int i;

      for (i = ent; i < numents; i++)
      {
        if (lseek(fd, (off_t)((i + 1) * size), SEEK_SET) == -1)
          break;
        if (read(fd, abuf, size) != size)
          break;
        if (lseek(fd, (off_t)(i * size), SEEK_SET) == -1)
          break;
        if (write(fd, abuf, size) != size)
          break;
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

int
rec_search(fpath, rptr, size, fptr, farg)
  char *fpath;
  char *rptr;
  int size;
  int (*fptr) ();
int farg;
{
  int fd;
  int id = 1;

  if ((fd = open(fpath, O_RDONLY, 0)) == -1)
    return 0;
  while (read(fd, rptr, size) == size)
  {
    if ((*fptr) (farg, rptr))
    {
      close(fd);
      return id;
    }
    id++;
  }
  close(fd);
  return 0;
}
#endif                          /* !defined(_BBS_UTIL_C_) */

/* ------------------------------------------ */
/* mail / post �ɡA�̾ڮɶ��إ��ɮסA�[�W�l�W */
/* ------------------------------------------ */
/* Input: fpath = directory; Output: fpath = full path; */

void
stampfile(fpath, fh)
  char *fpath;
  fileheader *fh;
{
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
    sprintf(ip, "M.%d.A", ++dtime );
  } while ((fp = open(fpath, O_CREAT | O_EXCL | O_WRONLY, 0644)) == -1);
  close(fp);
  memset(fh, 0, sizeof(fileheader));
  strcpy(fh->filename, ip);
  ptime = localtime(&dtime);
  sprintf(fh->date, "%2d/%02d", ptime->tm_mon + 1, ptime->tm_mday);
  }


/*
    ===== Added by mgtsai, Sep 10th, '96 =====
*/
void
stampdir(fpath, fh)
  char *fpath;
  fileheader *fh;
{
  register char *ip = fpath;
  time_t dtime;
  struct tm *ptime;

#if 1
  if (access(fpath, X_OK | R_OK | W_OK))
    mkdir(fpath, 0755);
#endif

  time(&dtime);
  while (*(++ip));
  *ip++ = '/';
  do
  {
    sprintf(ip, "D%X", ++dtime & 07777);
  } while (mkdir(fpath, 0755) == -1);
  memset(fh, 0, sizeof(fileheader));
  strcpy(fh->filename, ip);
  ptime = localtime(&dtime);
  sprintf(fh->date, "%2d/%02d", ptime->tm_mon + 1, ptime->tm_mday);
}


void
stamplink(fpath, fh)
  char *fpath;
  fileheader *fh;
{
  register char *ip = fpath;
  time_t dtime;
  struct tm *ptime;

#if 1
  if (access(fpath, X_OK | R_OK | W_OK))
    mkdir(fpath, 0755);
#endif

  time(&dtime);
  while (*(++ip));
  *ip++ = '/';
  do
  {
    sprintf(ip, "S%X", ++dtime );
  } while (symlink("temp", fpath) == -1);
  memset(fh, 0, sizeof(fileheader));
  strcpy(fh->filename, ip);
  ptime = localtime(&dtime);
  sprintf(fh->date, "%2d/%02d", ptime->tm_mon + 1, ptime->tm_mday);
}
/* ===== end ===== */
#if !defined(_BBS_UTIL_C_)
extern int bbs_sendmail(char *fpath, char *title, char *receiver);
#endif
/*
do_append(fpath,record,size)
  char *fpath;
  fileheader *record;
  int size;
{
  int fd;

  if ((fd = open(fpath, O_WRONLY | O_CREAT, 0644)) == -1)
  {
    perror("open");
    return -1;
  }
  flock(fd, LOCK_EX);
  lseek(fd, 0, SEEK_END);

  write(fd, record, size);

  flock(fd, LOCK_UN);
  close(fd);
  return 0;
}

int
rec_add(fpath, record, size)
  char *fpath;
  fileheader *record;
  int size;
{
  int n , m;

  if( get_num_records(fpath, sizeof(fileheader)) <= MAXKEEPMAIL * 2)
  {
    FILE *fp;
    char buf[512],address[200];
    for(n = strlen(fpath) - 1 ; fpath[n] != '/' && n>0 ; n--);
    strncpy(buf,fpath,n+1);
    buf[n+1] = 0;
    for(m = strlen(buf) - 2 ; buf[m] != '/' && m>0 ; m--);
    strcat(buf,".forward");
    if(fp = fopen(buf,"r"))
        {
          fscanf(fp,"%s",address);
          fclose(fp);
          if(buf[0] != 0 && buf[0] != ' ')
          {
            buf[n+1] = 0;
            strcat(buf,record->filename);
	       reset_tty();
               record->filemode |= FILE_TAGED;
	       do_append(fpath,record,size);
               bbs_sendmail(buf,record->title,address);
 	       restore_tty();
	       return 0;
          }
        }
  }

   do_append(fpath,record,size);
  return 0;
}
*/

int
rpg_rec(userid,RPG)
  char *userid;
  rpgrec RPG;
{
  char buf[80];
  int fd;

  sprintf(buf, BBSHOME"/home/%s/.RPG", userid);

  fd = open(buf , O_WRONLY | O_CREAT, 0600);
  flock(fd, LOCK_EX);
  lseek(fd, (off_t) (sizeof(rpgrec) * 0), SEEK_SET);
  write(fd, &RPG, sizeof(rpgrec));
  flock(fd, LOCK_UN);
  close(fd);

  return 0;
}

int
rpg_get(userid,RPG)
  char *userid;
  rpgrec *RPG;
{
  char buf[80];
  int fd;

  sprintf(buf, BBSHOME"/home/%s/.RPG", userid);
  fd = open(buf, O_RDONLY);
  read(fd, RPG, sizeof(rpgrec));
  close(fd);

  return 0;
}

