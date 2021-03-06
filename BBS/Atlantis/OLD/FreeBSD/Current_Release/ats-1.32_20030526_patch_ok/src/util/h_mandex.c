/*-------------------------------------------------------*/
/* util/h_mandex.c [原 mandex.c] ( MapleBBS SobVersion ) */
/*-------------------------------------------------------*/
/* target : 精華區索引, 整理,檔案規模統計程式            */
/* create : 95/03/29                                     */
/* source : 索引部份 by opus, tmg, woju                  */
/*          統計 檔案規模部份 by Prince                  */
/*          隱藏 私有目錄部份 by Dopin                   */
/* update : 02/11/27 (Dopin)
/*-------------------------------------------------------*/

#include        "bbs.h"
#include        "dopin.h"
#define         MAX_COUNT       25

char fn_index[] = ".index";
char fn_new[] = ".index.new";
char index_title[] = "◎ 精華區目錄索引";
char delay_list[] =  BBSHOME "/etc/mandelay";        /* 編輯日期紀錄檔 */
char space_list[] =  BBSHOME "/etc/mansort.space";   /* 容量規模紀錄檔 */
char file_list[] =   BBSHOME "/etc/mansort.file";    /* 檔案規模紀錄檔 */
char refuse_list[] = BBSHOME "/etc/refuse.checkman"; /* 不列入統計之精華區 */
char nh_level = -1;

FILE *fndx;
int ndir, nfile, index_pos, timenow, tmp_delay, tmp_space, allb;
char topdir[128], pgem[128], pndx[128];

struct bdata {
  char bname[IDLEN + 1]; /* 板名 */
  int delay;             /* 多久沒整理(天) */
  int file_num;          /* 檔案總數 */
  int dir_num;           /* 目錄總數 */
  int space;             /* 總容量 */

} bdata[MAXBOARD];

struct brd {
  char bname[IDLEN + 1];
  usint level;

} brd[MAXBOARD];

/* Prince: 過濾板名 */
/* 傳回值: -1 該板不存在 */
/*          0 該板非一般看板 */
/*          1 一般看板 */
int board_allow(char* bname) {
  int i;

  for(i = 0; i < allb ; i++) {
    if(!strcmp(bname, brd[i].bname) && brd[i].level == 0) return 1;
    else if(!strcmp(bname, brd[i].bname)) return 0;
  }
  return -1;

}

void str_time(char *buf) {
  time_t dtime;
  struct tm *pt;

  time(&dtime);
  pt = localtime(&dtime);
  sprintf(buf, "%02d/%02d/%02d", pt->tm_mon + 1, pt->tm_mday, pt->tm_year%100);
}

int ucolor(int j) {
  if(j == 1)       return 31;
  else if(j == 2)  return 33;
  else if(j == 3)  return 32;
  else if(j <= 5)  return 36;
  else if(j <= 10) return 34;
  else if(j <= 20) return 35;
  else             return 37;
}

int delay_cmp(b,a)
struct bdata  *a, *b;
{
    return a->delay - b->delay;
}

int file_cmp(b,a)
struct bdata  *a, *b;
{
    return (a->file_num + a->dir_num)  - (b->file_num + b->dir_num) ;
}

int space_cmp(b,a)
struct bdata  *a, *b;
{
  return a->space - b->space;
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

void mandex(int level, char *num_header, char *fpath) {
  FILE *fgem;
  char *fname, *ptr, buf[256];
  struct stat st;
  int count;
  fileheader fhdr;

  fgem = fopen(fpath, "r+");
  if(fgem == NULL) return;

  fname = strrchr(fpath, '.');

  if(!level) {
    strcpy(pgem, fpath);

    strcpy(fname, fn_new);
    fndx = fopen(fpath, "w");
    if (fndx == NULL) {
      fclose(fgem);
      return;
    }

    fprintf(fndx, "序號\t\t\t精華區主題\n"
      "──────────────────────────────\n");
    strcpy(pndx, fpath);
    ndir = nfile = 0;
    index_pos = -1;
  }

  count = 0;
  while (fread(&fhdr, sizeof(fhdr), 1, fgem) == 1) {
     strcpy(fname, fhdr.filename);
     if (!level && !strncmp(fhdr.title, index_title, strlen(index_title))
         && index_pos < 0) {
        index_pos = count;
        unlink(fpath);
     }

     if(strstr(fhdr.title, "[Hide]") || strstr(fhdr.title, "[隱藏]")) {
        nh_level = level;
        continue;
     }

     if(nh_level > 0)
        if(level > nh_level) continue;
        else nh_level = 0;

     /* Prince: 如果是目錄 */
     if (dashd(fpath)) {
       sprintf(buf,"[1;%dm%*d. %s[m\n",31+level,3 * level + 3,++count, fhdr.title);
       fputs(buf, fndx);

       ++ndir;

       if (*fhdr.title != '#' && level < 10) {
          strcat(fpath, "/.DIR");
          mandex(level + 1, buf + 1, fpath);
       }
     }
     else {
     /* Prince: 如果是檔案 */
       sprintf(buf,"[0;37m%*d. %s[m\n",3 * level + 3,++count, fhdr.title);
       fputs(buf, fndx);

       ++nfile;

       if(!strstr(fpath,"S.")/* SLink不計*/ && dashf(fpath)) {
         st.st_size = 0;
         stat(fpath, &st);
         tmp_space = tmp_space + st.st_size;

         if ( tmp_delay > (timenow - st.st_mtime) / 86400)
           tmp_delay = (timenow - st.st_mtime) / 86400;
       }
     }
  }

  if(!level) {
    char lpath[MAXPATHLEN];

    fclose(fndx);
    strcpy(fname, fn_index);
    rename(pndx, fpath);
    strcpy(pndx, fpath);

    sprintf(buf, "%s.new", pgem);
    if(index_pos >= 0 || (fndx = fopen(buf, "w"))) {
      fname[-1] = 0;
      stamplink(fpath, &fhdr);
      unlink(fpath);
      strcpy(fhdr.owner, "[SYSTEM]");
      sprintf(lpath, "%s/%s", topdir, pndx);
      st.st_size = 0;
      stat(lpath, &st);
      sprintf(fhdr.title, "%s (%.1fk)", index_title, st.st_size / 1024.);
      symlink(lpath, fpath);

      if(index_pos < 0) {
         fwrite(&fhdr, sizeof(fhdr), 1, fndx);
         rewind(fgem);
         while (fread(&fhdr, sizeof(fhdr), 1, fgem) == 1)
            fwrite(&fhdr, sizeof(fhdr), 1, fndx);
         fclose(fndx);
         fclose(fgem);
         rename(buf, pgem);
      }
      else {
         fseek(fgem, index_pos * sizeof(fhdr), 0);
         fwrite(&fhdr, sizeof(fhdr), 1, fgem);
         fclose(fgem);
      }
      return;
    }
  }
  fclose(fgem);
}

/* 記錄到目錄裡,供其他統計程式使用 */
void b_record(char *fname, int space, int delay, int file, int dir) {
  char buf[200];
  FILE *fp;

  sprintf(buf, BBSHOME "/boards/%s/manspace", fname);
  if (fp = fopen(buf, "w")) {
     fprintf(fp, "%d\n", space);
     fclose(fp);
  }

  sprintf(buf, BBSHOME "/boards/%s/mandelay", fname);
  if (fp = fopen(buf, "w")) {
     fprintf(fp, "%d\n", delay);
     fclose(fp);
  }

  sprintf(buf, BBSHOME "/boards/%s/manfiles", fname);
  if (fp = fopen(buf, "w")) {
     fprintf(fp, "%d\n", file);
     fclose(fp);
  }

  sprintf(buf, BBSHOME "/boards/%s/mandirs", fname);
  if (fp = fopen(buf, "w")) {
     fprintf(fp, "%d\n", dir);
     fclose(fp);
  }

}

int main(void) {
  DIR *dirp;
  FILE *fp;
  struct dirent *de;
  int ch, count, j, color, inf;
  char *fname, fpath[MAXPATHLEN], buf[50];
  time_t dtime;
  boardheader tmpbrd;

  time(&dtime);
  timenow = dtime;

  inf = open(BBSHOME "/.BOARDS", O_RDONLY);
  allb = 0;
  while (read(inf, &tmpbrd, sizeof(boardheader)) == sizeof(boardheader)) {
    if(tmpbrd.brdname[0]) {
       strcpy(brd[allb].bname, tmpbrd.brdname);
       brd[allb].level = tmpbrd.level;

       allb++;
    }
  }
  close(inf);

  chdir(strcpy(topdir, BBSHOME "/man/boards"));

  if(!(dirp = opendir("."))) {
    printf("## unable to enter [man/boards]\n");
    exit(-1);
  }

  count = 0;
  while(de = readdir(dirp)) {
     fname = de->d_name;
     ch = fname[0];

     if(belong(refuse_list, fname)) continue;

     if(ch != '.') {
        sprintf(fpath, "%s/.DIR", fname);

        if(board_allow(fname) == 1) {
          nfile = 0;
          ndir = 0;
          tmp_space = 0;
          tmp_delay = 16384;
          strcpy(bdata[count].bname, fname);

          mandex(0, "", fpath);

          bdata[count].space = tmp_space;
          bdata[count].delay = tmp_delay;
          bdata[count].file_num = nfile;
          bdata[count].dir_num = ndir;

          b_record(fname, tmp_space, tmp_delay, nfile, ndir);

          count++;
        }
        else if (board_allow(fname) == 0)
          mandex(0, "", fpath);
    }
  }
  closedir(dirp);
  str_time(buf);

  qsort(bdata, count , sizeof(bdata[0]), delay_cmp);

  if(fp = fopen(delay_list, "w")){
    fprintf(fp, "          [1;33;44m未編精華區天數列表[%s][m\n\n", buf);
    fprintf(fp, "          [1;33m%-14s     [1;33m%-20s[m\n", "板  名[m",
                "未編精華區天數");
    fprintf(fp, "      ─────────────────────────\n");

    for(j = 0; j < count; j++) {
      if(bdata[j].delay == 16384)
        fprintf(fp, "          [1;31m%-14s     %-7s[m\n", bdata[j].bname,
        "未編");
      else {
        if(bdata[j].delay >= 30) color = 33;
        else if(bdata[j].delay >= 15) color = 32;
        else color = 37;

        fprintf(fp, "          [1;%dm%-14s     %-4d %-3s[m\n", color,
        bdata[j].bname, bdata[j].delay, "天");
      }
    }
    fclose(fp);
  }

  qsort(bdata, count , sizeof(bdata[0]), file_cmp);

  if(fp = fopen(file_list, "w")) {
     fprintf(fp, "              [1;33;44m精華區檔案規模排名[%s][m\n\n", buf);
     fprintf(fp,"        %-4s   %-12s %-6s %-6s     %-6s\n", "名次", "板  名",
                "目錄數", "檔案數", "總和");
     fprintf(fp,"      ────────────────────────\n");

     for(j = 0; j <= MAX_COUNT; j++)
        fprintf(fp, "          [1;%dm%-4d %-14s %-6d %-6d   %-6d[m\n",
          ucolor(j + 1), j + 1, bdata[j].bname, bdata[j].dir_num,
          bdata[j].file_num, bdata[j].dir_num + bdata[j].file_num);

     fclose(fp);
  }

  qsort(bdata, count , sizeof(bdata[0]), space_cmp);

  if(fp = fopen(space_list, "w")) {

    fprintf(fp, "             [1;33;44m精華區容量排名[%s][m\n\n", buf);
    fprintf(fp, "           %-5s %-14s     %-14s\n", "名次", "板  名",
                "精華區容量");
    fprintf(fp, "      ────────────────────────\n");

    for(j = 0; j <= MAX_COUNT; j++)
      fprintf(fp,"           [1;%dm%-5d %-14s       %-14d[m\n",
         ucolor(j + 1), j + 1, bdata[j].bname, bdata[j].space);

    fclose(fp);
  }

  exit(0);
}
