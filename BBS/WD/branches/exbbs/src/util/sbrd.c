/*-------------------------------------------------------*/
/* util/showboard.c	( NTHU CS MapleBBS Ver 2.36 )	 */
/*-------------------------------------------------------*/
/* target : 看板資料修改(sorted)			 */
/* create : 95/03/29				 	 */
/* update : 95/12/15				 	 */
/*-------------------------------------------------------*/
/* Usage:	showboard .BOARDS			 */
/*-------------------------------------------------------*/

#include "bbs.h"
#define BOARDS          BBSHOME "/.BOARDS"
#define BOARDSNEW       BBSHOME "/.BOARDS.new"
#define EXPIRE_CONF     BBSHOME "/etc/expire.conf"

struct life
{
  char bname[16];               /* board ID */
  int days;                     /* expired days */
  int maxp;                     /* max post */
  int minp;                     /* min post */
};
typedef struct life life;

int
getlife(filelist, key, db)
  char *filelist;
  char *key;
  life *db;
{
  FILE *fp;
  char buf[128], *bname, *ptr;
  int rc=0, number;


  if (fp = fopen(filelist, "r"))
  {
    while (fgets(buf, sizeof(buf), fp))
    {
      if (buf[0] == '#')
         continue;

      if ((bname = (char *) strtok(buf, " \t\n\r")) && !strcasecmp(bname, key))
      {
         strcpy(db->bname, bname);
         ptr = (char *) strtok(NULL, " \t\r\n");
         if (ptr && (number = atoi(ptr)) > 0)
         {
            db->days = number;
            db->maxp = 1000;
            db->minp = 100;
            ptr = (char *) strtok(NULL, " \t\r\n");
            if (ptr && (number = atoi(ptr)) > 0)
            {
              db->maxp = number;
              ptr = (char *) strtok(NULL, " \t\r\n");
              if (ptr && (number = atoi(ptr)) > 0)
                 db->minp = number;
            }
         }
         rc=1;
         break;
      }
    }
    fclose(fp);
  }
  return rc;
}


int
main(argc, argv)
  int argc;
  char *argv[];
{
  int fd, fd2, i=0, count=MAXBOARD;
  boardheader brd;
  life db;
  char fn[64], fn2[64];
  
  strcpy(fn,BOARDS);
  strcpy(fn2,BOARDSNEW);

  if(argc>1)
    strcpy(fn,argv[1]);
  
  if(argc>2)
     strcpy(fn2,argv[2]);

  if(argc>3)
    count=atoi(argv[3]);

  if ((fd = open(fn, O_RDONLY)) < 0)
  {
    printf("%s error\n",fn);
    return 0;
  }
  
  fd2=open(fn2, O_WRONLY | O_CREAT, 0600);

  printf(" brdname postotal pad attr days maxp minp\n");

  while (read(fd, &brd, sizeof(brd)) == sizeof(brd))
  {
    printf("%-13s ",brd.brdname);
    printf("%3d ",brd.postotal);
    printf("%3d ",sizeof(brd.pad));
    printf("%4d ",brd.brdattr);
    
    db.days=360;
    db.maxp=0;
    db.minp=0;
    
    if(brd.brdattr & BRD_CLASS || brd.brdattr & BRD_GROUPBOARD)
      printf("---- ---- ----");
    else
    {
       getlife(EXPIRE_CONF,brd.brdname,&db);
       printf("%4d %4d %4d", db.days, db.maxp, db.minp);
    }
    printf("\n");

    brd.postotal = db.maxp/100;    
    brd.postotal = brd.postotal * 100;
    write(fd2, &brd, sizeof(brd));
    i++;
    if(i>count)
       break;
  }
  
  close(fd);
  close(fd2);

  return 0;
}
