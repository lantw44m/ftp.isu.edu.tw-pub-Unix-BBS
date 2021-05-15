/*-------------------------------------------------------*/
/* util/expire.c        ( NTHU CS MapleBBS Ver 2.36 )    */
/*-------------------------------------------------------*/
/* target : 自動砍信工具程式                             */
/* create : 95/03/29                                     */
/* update : 95/12/15                                     */
/*-------------------------------------------------------*/
/* syntax : expire [day] [max] [min]                     */
/*-------------------------------------------------------*/

#include"bbs.h"
#include <strings.h>
typedef int (*FPTR)(const void*, const void*);

#define DEF_DAYS        1000
#define DEF_MAXP        5000
#define DEF_MINP        1

#define EXPIRE_CONF     BBSHOME "/etc/expire.conf"
#define NOKEEP_CONF     BBSHOME "/etc/nokeep.conf"

char *bpath = BBSHOME "/boards";

struct life
{
  char bname[16];               /* board ID */
  int days;                     /* expired days */
  int maxp;                     /* max post */
  int minp;                     /* min post */
};
typedef struct life life;


/*
woju
Cross-fs rename()
*/

Rename(char* src, char* dst)
{
   char cmd[200];

   if (rename(src, dst) == 0)
      return 0;

   sprintf(cmd, "/bin/mv %s %s", src, dst);
   return system(cmd);

}

int
belong(filelist, key)
  char *filelist;
  char *key;
{
  FILE *fp;
  int rc = 0;

  if (fp = fopen(filelist, "r"))
  {
    char buf[80], *ptr;

    while (fgets(buf, 80, fp))
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

void
expire(brd)
  life *brd;
{
  fileheader head;
  struct stat state;
  char lockfile[128], tmpfile[128], bakfile[128];
  char fpath[128], index[128], *fname;
  int duetime, ftime, total, fd, fdr, fdw, done, keep, trash=0, count=0;

  printf("BOARD:%s ", brd->bname);
  trash=belong(NOKEEP_CONF, brd->bname);

#ifdef  VERBOSE
  if (brd->days < 1)
  {
    printf(":Err: expire time must more than 1 day.\n");
    return;
  }
  else if (brd->maxp < 100)
  {
    printf(":Err: maxmum posts number must more than 100.\n");
    return;
  }
#endif

  sprintf(index, "%s/%s/.DIR", bpath, brd->bname);
  sprintf(lockfile, "%s.lock", index);
  if ((fd = open(lockfile, O_RDWR | O_CREAT | O_APPEND, 0644)) == -1)
    return;
  flock(fd, LOCK_EX);

  strcpy(fpath, index);
  fname = (char *) strrchr(fpath, '.');

  duetime = time(NULL) - brd->days * 24 * 60 * 60;
  done = 0;
  if ((fdr = open(index, O_RDONLY, 0)) > 0)
  {
    fstat(fdr, &state);
    total = state.st_size / sizeof(head);
    sprintf(tmpfile, "%s.new", index);
    unlink(tmpfile);
    count=0;
    if ((fdw = open(tmpfile, O_WRONLY | O_CREAT | O_EXCL, 0644)) > 0)
    {
      while (read(fdr, &head, sizeof head) == sizeof head)
      {
        done = 1;
        keep = 1;
        
        ftime = atoi(head.filename + 2);
        
        if (head.owner[0] == '-')
            keep = 0;
          
        if (head.filemode & FILE_DIGEST 
            && ftime < duetime)
            keep = 0;
       
        if (ftime < duetime || total > brd->maxp)
            keep = 0;

        if (head.filemode & FILE_MARKED
           || total <= brd->minp)
           keep = 1;
        
        if (trash == 1 && (total > brd->maxp || ftime < duetime))
           keep = 0;

        if (keep)
        {
          if (write(fdw, &head, sizeof head) == -1)
          {
            done = 0;
            break;
          }
        }
        else
        {
          strcpy(fname, head.filename);
          unlink(fpath);
//          printf("\t%s\n", fname);
          total--;
          count++;
        }
      }
      close(fdw);
    }
    close(fdr);
  }

  if (done)
  {
    sprintf(bakfile, "%s.old", index);
    if (Rename(index, bakfile) != -1)
      Rename(tmpfile, index);
      
    printf("KILL: %d TRASH: %d\n",count,trash);
  }
  flock(fd, LOCK_UN);
  close(fd);
}


main(argc, argv)
  char *argv[];
{
  FILE *fin;
  int number, count;
  life db, table[MAXBOARD], *key;
  struct dirent *de;
  DIR *dirp;
  char *ptr, *bname, buf[256];

  db.days = ((argc > 1) && (number = atoi(argv[1])) > 0) ? number : DEF_DAYS;
  db.maxp = ((argc > 2) && (number = atoi(argv[2])) > 0) ? number : DEF_MAXP;
  db.minp = ((argc > 3) && (number = atoi(argv[3])) > 0) ? number : DEF_MINP;

  /* --------------- */
  /* load expire.ctl */
  /* --------------- */

  count = 0;
  if (fin = fopen(EXPIRE_CONF, "r"))
  {
    while (fgets(buf, 256, fin))
    {
      if (buf[0] == '#')
        continue;

      bname = (char *) strtok(buf, " \t\r\n");
      if (bname && *bname)
      {
        ptr = (char *) strtok(NULL, " \t\r\n");
        if (ptr && (number = atoi(ptr)) > 0)
        {
          key = &(table[count++]);
          strcpy(key->bname, bname);
          key->days = number;
          key->maxp = db.maxp;
          key->minp = db.minp;

          ptr = (char *) strtok(NULL, " \t\r\n");
          if (ptr && (number = atoi(ptr)) > 0)
          {
            key->maxp = number;

            ptr = (char *) strtok(NULL, " \t\r\n");
            if (ptr && (number = atoi(ptr)) > 0)
            {
              key->minp = number;
            }
          }
        }
      }
    }
    fclose(fin);
  }

  if (count > 1)
  {
    qsort(table, count, sizeof(life), (FPTR)strcasecmp);
  }

  /* ---------------- */
  /* visit all boards */
  /* ---------------- */

  if (!(dirp = opendir(bpath)))
  {
    printf(":Err: unable to open %s\n", bpath);
    return -1;
  }

  while (de = readdir(dirp))
  {
    ptr = de->d_name;
    if (ptr[0] > ' ' && ptr[0] != '.')
    {
      if (count)
        key = (life *) bsearch(ptr, table, count, sizeof(life), (FPTR)strcasecmp);
      else
        key = NULL;
      if (!key)
        key = &db;
      strcpy(key->bname, ptr);
      expire(key);
    }
  }
  closedir(dirp);
}
