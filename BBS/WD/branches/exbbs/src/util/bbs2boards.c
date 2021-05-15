#include "bbs.h"
#include "bbs_util.h"

int
board_cmp(a, b)
  boardheader *a, *b;
{
  int bcmp;
  char modea[8], modeb[8];
  strncpy(modea, a->title, 6);
  strncpy(modeb, b->title, 6);
  modea[6] = modeb[6] = '\0';
  bcmp = strcmp(modea, modeb);
  if (!bcmp)
    bcmp = strcasecmp(a->brdname, b->brdname);
  return bcmp;
}

boardheader allbrd[MAXBOARD];

int bbs2brd( int option, char *title)
{
   int i,j,count,similar;
   FILE *fi;
   char temp[8],brdtitle[128];
   
   if(option<0) return;

   if( (fi=fopen( BOARDS_REC,"rb")) == NULL )
   {
      printf( "bbs2brd: No file: %s\n", BOARDS_REC );
      return 1;
   }

   i=0;
   memset(allbrd, 0, MAXBOARD * sizeof(boardheader));
   while (fread( &allbrd[i], sizeof(boardheader),1,fi)>0)
     if (allbrd[i].brdname[0])
       i++;
       
   count=i;
   qsort(allbrd, count, sizeof(boardheader), board_cmp);
   
   for(i=0;i<count;i++)
   {
      if(option==0)
        if (!(allbrd[i].brdattr & BRD_CLASS))
           continue;

      if(option>0)
      {
        if((allbrd[i].brdattr & BRD_HIDE)
           || (allbrd[i].brdattr & BRD_CLASS))
           continue;
        
        if(!(allbrd[i].brdattr & BRD_GROUPBOARD)
           && !(allbrd[i].level & PERM_BASIC)
           && !(allbrd[i].brdattr & BRD_POSTMASK))
           continue;
      }

       if(option==2)
       {
         similar=0;
         strncpy(brdtitle, allbrd[i].title, 4);
//         printf("%s ",brdtitle);
         for(j=0 ; j<=strlen(title)-4 ; j+=2)
         {
            strncpy(temp, title+j, 4);
            similar = similar + (!strncmp(brdtitle, temp, 4));
//            printf("%s ",temp);
         }
//         printf("\n");
         if(similar==0)
           continue;
       }
      
      printf("%s,,%s,,%s,,%s\n",
      allbrd[i].brdname,
      strlen(allbrd[i].BM)>0 ? allbrd[i].BM : "µL",
      allbrd[i].title,
      (allbrd[i].brdattr & BRD_GROUPBOARD || allbrd[i].brdattr & BRD_CLASS) ? "G" : 
      (allbrd[i].brdattr & BRD_BRDMAIL) ? "e" : " "
//      allbrd[i].level
      );
   }

   return 0;
}


int main( int argc, char *argv[])
{
   if( argc < 2 )
   {
      printf( "usage: bbs2brd [option] [title]\n" );
      exit(1);
   }

   if(argc>2)
      bbs2brd(atoi(argv[1]), argv[2]);
   else if(argc>1)
      bbs2brd(atoi(argv[1]), " ");
   else
      bbs2brd(0," ");
}

