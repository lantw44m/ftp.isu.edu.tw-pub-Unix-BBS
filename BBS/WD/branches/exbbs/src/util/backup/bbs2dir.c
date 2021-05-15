
#include "bbs.h"
#include "bbs_util.h"

int check_brdattr( char *bname)
{
  boardheader brd;
  FILE *fi;
  if((fi=fopen(BOARDS_REC, "rb"))==NULL)
    return 0;

  while (fread( &brd, sizeof(boardheader),1,fi)>0)
    if(strcmp(brd.brdname,bname)==0)
    {
       if( 
           (brd.brdattr & BRD_HIDE) ||
           (!(brd.brdattr & BRD_POSTMASK) && !(brd.level & PERM_BASIC))
         )
           return 0;
       else if(brd.brdattr & BRD_BRDMAIL)
         return 1;
       else
         return 2;
    }
  fclose(fi);
  return 0;
}

int isdir( char *fname )
{
  struct stat state;
  stat(fname, &state);
  return (S_ISDIR(state.st_mode));
}    

int bbs2dir( char *bname, int page, int pagesize, char *dirname)
{
   char dpath[256], bpath[256],fpath[256];
//   FILE *fi;
   fileheader item;
   int fdr,total,pagecount,count,option=0,brdattr=0;
   struct stat state;
   
   if(strncmp(bname,"man/boards/",11)==0)
   { 
      option=1;
      bname=bname+11;
      sprintf(bpath, "%s/%s%s",MAN_BRD_PATH, bname, dirname);
      sprintf(dpath, "%s/%s", bpath, ".DIR");
   }
   else if(strncmp(bname,"man/log",7)==0)
   {
      option=2;
//      bname=bname+4;
      sprintf(bpath, "%s/%s", MAN_HIST_PATH, dirname);
      sprintf(dpath, "%s/%s", bpath, ".DIR");
   }
   else
   {
      option=3;
      sprintf(bpath, "%s/%s", BOARDS_PATH, bname);
      sprintf(dpath, "%s/%s", bpath, ".DIR");
   }
   
   brdattr=check_brdattr(bname);
   if(option!=2 && brdattr==0) return 0;
   if(page==-2) 
   {
      printf("%d",brdattr);
      return 0; // 提供查詢 brdattr 是否有 BRD_BRDMAIL
   }
   
   if ((fdr = open(dpath, O_RDONLY, 0)) <= 0)
//   if( (fi=fopen( spath,"rt")) == NULL )
   {
      printf( "bbs2dir: No file: %s\n", bpath );
      return 0;
   }
   
   if(pagesize<1 || pagesize>100) pagesize=10;
   fstat(fdr, &state);
   total = state.st_size / sizeof(item);
//   printf("total=%d\n",total);
   pagecount=total/pagesize - (total % pagesize == 0);

   if(page<0)              page=pagecount;     
   if(page>pagecount)     page=pagecount;   

   lseek(fdr, sizeof(item) * pagesize * page, 1);
//   while((fread( &item, sizeof(item), 1, fi))>0)
   count=0;
   while(read(fdr, &item, sizeof(item)) == sizeof(item) && (count++ < pagesize))
   {  
      if(item.filemode & FILE_REFUSE)
        continue;
      sprintf(fpath, "%s/%s", bpath, item.filename);
      printf("%d,,%s,,%s,,%s,,%s,,%s\n",
      page,
      item.date,
      item.owner,
      item.filename,
      item.title,
      (item.filemode & FILE_MARKED) ? "M" : 
      (item.filemode & FILE_DIGEST) ? "T" : 
      (isdir(fpath)) ? "D" : " "
      );
   }

   close(fdr);
   return 1;
}


int main( int argc, char *argv[])
{
   if( argc < 2 )
   {
      printf( "usage: bbs2dir brdname page pagesize dir\n" );
      exit(1);
   }

   if(argc>4)
      bbs2dir(argv[1], atoi(argv[2]), atoi(argv[3]), argv[4]);
   else if(argc>3)
      bbs2dir(argv[1], atoi(argv[2]), atoi(argv[3]), "");
   else if(argc>2)
      bbs2dir(argv[1], atoi(argv[2]), 10, "");
   else
      bbs2dir(argv[1], -1, 10, "");
}


