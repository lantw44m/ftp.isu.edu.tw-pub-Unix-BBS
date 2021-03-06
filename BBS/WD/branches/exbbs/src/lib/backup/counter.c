#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define SEM_ENTER      -1      /* enter semaphore */
#define SEM_LEAVE      1       /* leave semaphore */
#define SEM_RESET      0       /* reset semaphore */
#define C_SEMKEY	2222

/* 計數器 wildcat */

int counter_semid;

void
counter(filename,modes,n)
  char *filename;
  char *modes;
  int n;
{
  FILE *fp;
  unsigned long visited=0;
//  char buf[1024];

  sem_init(C_SEMKEY,&counter_semid);
  sem_lock(SEM_ENTER,counter_semid);
  if(fp = fopen(filename,"r"))
  {
    fscanf(fp,"%lu",&visited);
    fclose(fp);
  }

  prints("                      "
         "[1;37;44m 您是%s第[36m %-10ld [33m位 %s 的使用者 [0m",
  n ? "今天" : "有史以來",++visited,modes);
  unlink(filename);

  if(fp = fopen(filename,"w"))
  {
    fprintf(fp,"%ld",visited);
    fclose(fp);
  }
  sem_lock(SEM_LEAVE,counter_semid);
}
