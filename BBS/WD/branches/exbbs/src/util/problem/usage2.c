/*
 Transfer From FirebirdBBS, SmallPig
*/


#include <time.h>
#include <stdio.h>
#include "bbs.h"
#include "config.h"
#include "record.c"
#include <string.h>

#undef GROUP
#undef MONTH
#define HIDE
#undef DELETED

struct binfo
{
  char  boardname[18];
  char  expname[28];
  int times;
  int sum;
  int post;
} st[MAXBOARD];

int numboards=0;

#if 1
int
brd_cmp(b, a)
struct binfo *a, *b;
{
    if(a->times!=b->times)
            return (a->times - b->times);
    return a->sum-b->sum;
}
#else
int
brd_cmp(b, a)
struct binfo *a, *b;
{
    if(a->sum!=b->sum)
            return (a->sum - b->sum);
    return a->times - b->times;
}
#endif

int report()
{
        return ;
}
char    *Ctime(date)
time_t  *date;
{
        static char buf[80];

        strcpy(buf, (char *)ctime(date));
        buf[strlen(buf)-1] = '\0';
        return buf;
}

int
record_data(board,sec)
char *board;
int sec;
{
        int i;
        for(i=0;i<numboards;i++)
        {
                if(!strcmp(st[i].boardname,board))
                {
                        st[i].times++;
                        st[i].sum+=sec;
                        return;
                }
        }
        return ;
}

int
record_data2(board)
char *board;
{
        int i;
        for(i=0;i<numboards;i++)
        {
                if(!strcmp(st[i].boardname,board))
                {
                        st[i].post++;
                        return;
                }
        }
        return ;
}


int
fillbcache(fptr)
    struct boardheader *fptr ;
{

    if( numboards >= MAXBOARD )
        return 0;

#ifdef HIDE
    if((fptr->level != 0) && !(fptr->level & PERM_POSTMASK))
        return;
#endif

    if ((strstr(fptr->title, "?["))||(strstr(fptr->title, "?U"))||
         (strstr(fptr->title, "?S")))
    return;


    strcpy(st[numboards].boardname,fptr->brdname);
    strcpy(st[numboards].expname,fptr->title);
/*    printf("%s %s\n",st[numboards].boardname,st[numboards].expname); */
    st[numboards].times=0;
    st[numboards].sum=0;
    st[numboards].post=0;
    numboards++;
    return 0 ;
}

int
fillboard()
{
  apply_record( BBSHOME"/.BOARDS", fillbcache, sizeof(struct boardheader) );
}

char *
timetostr(i)
int i;
{
        static char str[30];
        int minute,sec,hour;

        minute=(i/60);
        hour=minute/60;
        minute=minute%60;
        sec=i&60;
        sprintf(str,"%2d:%2d\'%2d\"",hour,minute,sec);
        return str;
}

main(argc, argv)
int argc;
char *argv[];
{
  char *progmode;
  FILE *fp;
  FILE *op;
  char buf[256], *p,bname[20];
  char date[80];
  int mode;
  int c[3];
  int max[3];
  unsigned int ave[3];
  int now,sec;
  int i, j,k;
  int num, n=0;
  int allpost=0;
  char    *blk[10] =
  {
      "  ","?j", "?k", "?l", "?m",
      "?n","?o", "?p", "?i", "?i",
  };
  time_t nows = time(NULL);
  struct tm *ptime;

      ptime = localtime(&nows);


  setuid(BBSUID);
  setgid(BBSGID);
  chdir(BBSHOME);

  mode = 1;

  if (argc<=2)
  {
    exit(0);
  }

  num = atoi(argv[2]);
  mode = atoi(argv[1]);

start:

#ifdef MONTH
  if(mode==1)
        strcpy(buf,"etc/mon_board.list");
  else
            strcpy(buf,"etc/mon_board.table");
#else
  if(mode==1)
        strcpy(buf,"etc/top_board.list");
  else
            strcpy(buf,"etc/top_board.table");
#endif

#ifdef MONTH
  if ((fp = fopen(BBSHOME "/adm/board.read.month", "r")) == NULL)
#else
    if ((fp = fopen(FN_USEBOARD, "r")) == NULL)
#endif

  {
    printf("cann't open use_board\n");
    return 1;
  }

  if ((op = fopen(buf, "w")) == NULL)
  {
    printf("Can't Write file %s\n",buf);
    return 1;
  }

  fillboard();
  now=time(0);
  sprintf(date,"%6.6s",Ctime(&now)+4);
  while (fgets(buf, 256, fp))
  {
//    printf("%d\n", n++);
    if(strlen(buf)<57)
        continue;
    if ( !strncmp(buf+21, "USE", 3))
    {
      p=strstr(buf,"USE");
      p+=4;
      p=strtok(p," ");
      strcpy(bname,p);
    if ( p = (char *)strstr(buf+46, "Stay: "))
    {
      sec=atoi( p + 6);
    }
    else
        sec=0;
    record_data(bname,sec);
    }

    if ( !strncmp(buf+21, "POS", 3))
    {
      p=strstr(buf,"POS");
      p+=4;
      p=strtok(p," ");
      strcpy(bname,p);
      record_data2(bname);
    }

   }
   fclose(fp);
   qsort(st, numboards, sizeof( st[0] ), brd_cmp);
   ave[0]=0;
   ave[1]=0;
   ave[2]=0;
   max[1]=0;
   max[0]=0;
   max[2]=0;
   for(i=0;i<numboards;i++)
   {
        ave[0]+=st[i].times;
        ave[1]+=st[i].sum;
        ave[2]+=st[i].times==0?0:st[i].sum/st[i].times;
        if(max[0]<st[i].times)
        {
                max[0]=st[i].times;
        }
        if(max[1]<st[i].sum)
        {
                max[1]=st[i].sum;
        }
        if(max[2]<(st[i].times==0?0:st[i].sum/st[i].times))
        {
                max[2]=(st[i].times==0?0:st[i].sum/st[i].times);
        }
        allpost=allpost+st[i].post;
   }
   c[0]=max[0]/30+1;
   c[1]=max[1]/30+1;
   c[2]=max[2]/30+1;
   numboards++;
   st[numboards-1].times=ave[0]/numboards;
   st[numboards-1].sum=ave[1]/numboards;
   strcpy(st[numboards-1].boardname,"Average");
   strcpy(st[numboards-1].expname,"?`????");


     fprintf(fp, "                       [1;31m%s[m %d?~%d??%d?? ???i\n",
     BOARDNAME, ptime->tm_year, ptime->tm_mon+1, ptime->tm_mday);

   if(mode==1)
   {
        fprintf(op,"[1;37m?W?? %-15.15s%-28.28s %5s %8s %9s %4s[m\n",
                "?Q?װϦW??","?????ԭz","?H??","?ֿn?ɶ?","?????ɶ?","POST");
   }else
   {
        fprintf(op,"      [1;37m1 [m[34m%2s[1;37m= %d (?`?H??) "
        "[1;37m1 [m[32m%2s[1;37m= %s (?ֿn?`?ɼ?) [1;37m1 [m[31m%2s[1;37m= %d ??(?????ɼ?)\n\n",
                blk[9],c[0],blk[9],timetostr(c[1]),blk[9],c[2]);
   }


 for(i=0;i<num;i++)
 {

   if(mode==1)
     fprintf(op,"[1m%4d[m %-15.15s%-28.28s %5d %-.8s %9d %4d\n",
     i+1,st[i].boardname,st[i].expname,st[i].times,timetostr(st[i].sum),
        st[i].times==0?0:st[i].sum/st[i].times, st[i].post);
   else
   {
     fprintf(op,"      [1;37m??[31m%3d [37m?W ?Q?װϦW?١G[31m%s "
                     "[35m%s[m\n",i+1,st[i].boardname,st[i].expname);
     fprintf(op,"[1;37m    ?z?w?w?w?w?w?w?w?w?w?w?w?w?w?w?w?w?w?w?w?w"
                 "?w?w?w?w?w?w?w?w?w?w?w?w?w?w?w?w\n");
     fprintf(op,"[1;37m?H???x[m[34m");
     for(j=0;j<st[i].times/c[0];j++)
     {
      fprintf(op,"%2s",blk[9]);
     }
       fprintf(op,"%2s [1;37m%d[m\n",blk[(st[i].times%c[0])*10/c[0]],
                st[i].times);
     fprintf(op,"[1;37m?ɶ??x[m[32m");
     for(j=0;j<st[i].sum/c[1];j++)
     {
       fprintf(op,"%2s",blk[9]);
     }
       fprintf(op,"%2s [1;37m%s[m\n",blk[(st[i].sum%c[1])*10/c[1]],
                timetostr(st[i].sum));
     j=st[i].times==0?0:st[i].sum/st[i].times;
     fprintf(op,"[1;37m?????x[m[31m");
     for(k=0;k<j/c[2];k++)
     {
       fprintf(op,"%2s",blk[9]);
     }
       fprintf(op,"%2s [1;37m%s[m\n",blk[(j%c[2])*10/c[2]],timetostr(j));
     fprintf(op,"[1;37m    ?|?w?w?w?w?w?w?w?w?w?w?w?w?w?w?w?w?w?w?w"
                                "?w?w?w?w?w?w?w?w?w?w?w?w?w?w?w?w?w[m\n\n");
   }
 }
     fprintf(op,"[1m%4d[m %-15.15s%-28.28s %5d %-.8s %9d %4d\n",
     i+1,st[numboards-1].boardname,st[numboards-1].expname,
       st[numboards-1].times,timetostr(st[numboards-1].sum),
        st[numboards-1].times==0?0:st[numboards-1].sum/st[numboards-1].times,
         allpost);


 fclose(op);
 mode--;
 if (mode >=0) goto start;
}

