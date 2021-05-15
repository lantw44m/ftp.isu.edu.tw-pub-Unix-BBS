/*
*    �M�z�H�c���H��A�j�M�C�@���ɮסA�M�᭫�� .DIR�A�屼�U���ɡC
*
*    $Id: remail.c,v 1.1 2000/01/15 01:45:40 edwardc Exp $
*/

#include        <stdio.h>
#include        <sys/types.h>
#include        <sys/stat.h>
#include        <dirent.h>
#include        <limits.h>
#include        "bbs.h"
#define BBSMAIL "/home/bbs/mail"
#define TRUE  1
#define FALSE 0

report()
{
}

main(argc,argv)
int argc;
char *argv[];
{
   myftw("/home/bbs/mail") ;
}

int myftw(pathname)
char *pathname ;
{
   struct stat statbuf ;
   DIR *dp ;
   char buf[80] ;
   struct dirent *dirp ;
   int all=0, done=0 ;
   if(pathname == NULL)
      strcpy(pathname, BBSMAIL) ;
   if((dp = opendir(BBSMAIL)) == NULL)
   {
      printf("\n�L�k�}�ҥؿ� %s�I", pathname) ;
      return 0;
   }
   while(dirp = readdir(dp))
   {
      char workpath[256] ;
      char userpath[256] ;
      DIR *workdp, *userdp;
      struct dirent *workdirp, *userdirp;
      char dot_dir[256], dot_dir_bak[256] ;
      
      if(!strcmp(dirp->d_name, ".") ||
         !strcmp(dirp->d_name, "..") ||
         dirp->d_type != DT_DIR)
      {
         continue ;
      }
      printf("\n�i�J�ؿ� %s/%s ....\n", pathname, dirp->d_name) ;
      sprintf(workpath, "%s/%s", pathname, dirp->d_name) ;
      if( (workdp = opendir(workpath)) == NULL )
      {
         printf("\n�L�k�}�ҥؿ� %s�I", workpath) ;
         return 0 ;
      }
      while(workdirp = readdir(workdp))
      {
         int mailcounts ;
         int allactions ;
         
         if(!strcmp(workdirp->d_name, ".") ||
            !strcmp(workdirp->d_name, "..") ||
            dirp->d_type != DT_DIR)
         {
            continue ;
         }
         printf("\n   ��s %12s ���H��", workdirp->d_name) ;
         sprintf(userpath, "%s/%s", workpath, workdirp->d_name) ;
         if( (userdp = opendir(userpath)) == NULL )
         {
            printf("\n      �L�k�}�� %s ���H��ؿ��I", userpath) ;
            continue ;
         }
         sprintf(dot_dir, "%s/.DIR", userpath) ;
         sprintf(dot_dir_bak, "%s.bak", dot_dir) ;
         rename(dot_dir, dot_dir_bak) ;
         printf("\n      ��z %s �H��A���� .DIR �ɡI\n", workdirp->d_name) ;
         while( (userdirp = readdir(userdp)) != NULL)
         {
            if( !strcmp(userdirp->d_name, ".") ||
                !strcmp(userdirp->d_name, "..") ||
                userdirp->d_name[0] != 'M')
            {
               continue ;
            }
            mailcounts += do_remail(userpath, userdirp->d_name, dot_dir) ;
            allactions++ ;
         }
         closedir(userdp) ;
         chown(dot_dir, 9999, 99) ;
         printf("\n      �@���� %d �g�H��A %d�g�H�󥢱ѡC\n", mailcounts, allactions - mailcounts) ;
         mailcounts = 0 ;
         allactions = 0 ;
      }
      closedir(workdp) ;
   }
   closedir(dp) ;
   printf("\n�Ҧ�����ഫ����\n") ;
}

int do_remail(path, file, dot_dir)
   char *path ;
   char *file ;
   char *dot_dir ;
{
   FILE *fp ;
   char filename[256] ;
   struct fileheader fh ;
   int step = 0 ;
   char buf[256] ;
   char *ptr, *ptr2 ;
   
   sprintf(filename, "%s/%s", path, file) ;
   if( (fp = fopen(filename, "r")) == NULL)
   {
      printf("\n         �L�k�}���ɮ� %s�I", file) ;
      return 0;
   }
   strncpy(fh.filename, file, sizeof(fh.filename)) ; /* ��J�ɦW */
   fh.level = 0 ;
   memset(fh.accessed, 0, sizeof(fh.accessed)) ;
   while ( fgets(buf, 256, fp) != NULL)
   {
      if(strstr(buf,"�o�H�H: ") ||
         strstr(buf,"�@  ��: ") ||
         strstr(buf,"�H�H�H: "))
      {
         ptr = &buf[8] ;
         ptr2 = strchr(ptr, ' ') ;
         if(ptr2 != NULL) *ptr2 = '\0' ;
         
         ptr2 = strchr(ptr, '@') ;
         if(ptr2 != NULL) *ptr2 = '\0' ;
         
         ptr2 = strchr(ptr, '.') ;
         if(ptr2 != NULL) *(ptr2+1) = '\0' ;
         
         strncpy(fh.owner, ptr, sizeof(fh.owner)) ;
         printf("#") ;
         step = 1 ;
      }
      if(strstr(buf, "��  �D: ")||strstr(buf,"�D  ��: "))
      {
         ptr = &buf[8] ;
         ptr[strlen(ptr) - 1] = 0 ;
         strncpy(fh.title, ptr, sizeof(fh.title)) ;
         step = 2 ;
      }
      if(step == 2)
         break ;
   }
   fclose(fp) ;
   if(step == 2)
   {
      fh.filename[ STRLEN - 1] = 'S' ;
      fh.filename[ STRLEN - 2] = 'S' ;
      append_record(dot_dir, &fh, sizeof(fh)) ;
      
      return 1 ;
   }
   else
   {
      unlink(filename) ;
      return 0 ;
   }
}
