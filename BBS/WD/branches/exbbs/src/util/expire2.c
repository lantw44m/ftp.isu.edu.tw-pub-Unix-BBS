/*-------------------------------------------------------*/
/* util/expire2                            Robert Liu    */
/*-------------------------------------------------------*/
/* target : 舊文章刪除/看板整理				 */
/* create : 2001/07/01				 	 */
/* update : 2001/07/07				 	 */
/*-------------------------------------------------------*/

#include "bbs.h"
#include "bbs_util.h"

int datachanged=0;

Rename(char* src, char* dst)
{
   char cmd[256];
   if (rename(src, dst) == 0) return 0;
   sprintf(cmd, "/bin/mv %s %s", src, dst);
   return system(cmd);
}

void
expire(boardheader *brd, char *option)
{
  struct stat state;
  fileheader head;

  char lockfile[128],       tmpfile[128],
       bakfile[128],       fpath[128],
       direct[128],       fn[128];
       
  int fd, fi, fo;
  int done=0, ftime, duetime, keep, total, count, maxp, minp, duedays;

  sprintf(fpath, "%s/%s", BOARDS_PATH, brd->brdname);
  sprintf(direct, "%s/.DIR", fpath);
  sprintf(lockfile, "%s.lock", direct);

  if ((fd = open(lockfile, O_RDWR | O_CREAT | O_APPEND, 0644)) == -1) return;
  flock(fd, LOCK_EX);

  if ((fi = open(direct, O_RDONLY, 0)) > 0)
  {
    fstat(fi, &state);
    total = state.st_size / sizeof(head);
    count = 0;
    sprintf(tmpfile, "%s.new", direct);
    unlink(tmpfile);

     if(belong(EXPIRE_CONF,brd->brdname,&duedays,&maxp,&minp))
     {
       brd->postotal=maxp;
       datachanged=1;
     }
     else
     {
       duedays=360;
       maxp = brd->postotal;
       if (maxp<=0)  maxp=0;
       if(brd->brdattr & BRD_OUTGOING && brd->brdattr & BRD_INGOING)
          maxp=maxp*2;
       minp = maxp /5;
     }

    if(strcmp(option,"DEL")==0)
    {
     if ((fo = open(tmpfile, O_WRONLY | O_CREAT | O_EXCL, 0644)) > 0)
     {
      while (read(fi, &head, sizeof head) == sizeof head)
      {
        keep = 1;
        done = (strcmp(option,"DEL")==0);
        duetime = time(NULL) - duedays * 24 * 60 * 60;
        ftime = atoi(head.filename + 2);
        sprintf(fn, "%s/%s", fpath, head.filename);
        
        if(ftime < duetime) keep=0;
        if(total > maxp) keep=0;
        if((head.filemode & FILE_DIGEST || head.filemode & FILE_MARKED))
           keep=1;

        if(total <= minp) keep=1;
        if(head.owner[0] == '-') keep=0;

        if(done)
        {
          if (keep)
          {
            if (write(fo, &head, sizeof head) == -1)
            {
              done = 0;
              break;
            }
          }
          else
          {
            unlink(fn);
            strcat(fn,".X\0");
            unlink(fn);
            total--;
            count++;
          }
        }
      }
      close(fo);
     }
    }
    close(fi);
  }

  if (done)
  {
    sprintf(bakfile, "%s.old", direct);
    if (Rename(direct, bakfile) != -1)
      Rename(tmpfile, direct);
  }

  printf("%4d %4d %4d %4d %5d ", duedays, maxp, minp, count, total);
  
//  if(strcmp(option,"CHK")==0)
    printf("%-.30s",brd->title);
  
  flock(fd, LOCK_UN);
  close(fd);
}

int
main(argc, argv)
  int argc;
  char *argv[];
{
  boardheader brd;
  char fn[64], fn2[64], brdname[32], option[16]="nothing"; 
  FILE *fd, *fd2;
  int i=0;
  
  strcpy(fn,BOARDS_REC);
  strcpy(fn2,BOARDS_TMP);

  if(argc<2)
  {
    printf("\n");
    printf("usage: expire2 boardname [option]\n\n");
    printf("boardname : ALL   (check all boards)\n");
    printf("option    : DEL   (check and delete posts on board)\n\n");
    return 0;
  }
  
  if(argc>1) strcpy(brdname, argv[1]);
  if(argc>2) strcpy(option, argv[2]);
   
  //if ((fd = open(fn, O_RDONLY)) < 0)
  if ((fd = fopen(fn,"r"))==NULL)
  {
    printf("%s read error\n",fn);
    return 0;
  }

  if (strcmp(option,"DEL")==0 && (fd2 = fopen(fn2, "w"))==NULL)
  {
    printf("%s write error\n",fn2);
    return 0;
  }
  
  printf("    brdname       days maxp minp kill total\n");
  
//  while (read(fd, &brd, sizeof(brd)) == sizeof(brd))
  while(fread(&brd, sizeof(boardheader), 1, fd)>0)
  {
    if(strcmp(brd.brdname,brdname)==0 || (strcmp(brdname,"ALL")==0))
    {
      printf("%3d %-13s ", ++i, brd.brdname);
    
      if(brd.brdattr & BRD_CLASS || brd.brdattr & BRD_GROUPBOARD)
        printf("---- ---- ---- ----");
      else
         expire(&brd, option);
      printf("\n");
    }

    if(strcmp(option,"DEL")==0)
       fwrite(&brd, sizeof(boardheader), 1,fd2);

  }
  fclose(fd);
  if(strcmp(option,"DEL")==0)
  {
     fclose(fd2);
  }
  if(datachanged)
  {
       f_mv(BOARDS_REC, BOARDS_OLD);
       f_mv(BOARDS_TMP, BOARDS_REC);
  }   
  return 0;
}
