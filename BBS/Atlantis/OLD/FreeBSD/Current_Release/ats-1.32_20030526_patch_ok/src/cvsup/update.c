#include <stdio.h>
#include "cvsup.h"

#undef DEBUG

cvsindex CVS = {0};
lists   LIST = {0};

int patch_update(int year, char month, char *mode) {
   char buf[256], cbuf[64];
   int i;
   FILE *fp;

   sprintf(buf, "%s/cvs_index", CVS_WORK);
   if((fp = fopen(buf, "rb")) == NULL) return 1;
   i = fread(&CVS, sizeof(CVS), 1, fp);
   fclose(fp);

   if(i == 1) {
      int j, curcvscode;

      if(CVS.lock) return -1;
      if((fp = fopen(CVS_FILE, "r")) == NULL) return -1;
      fgets(cbuf, 255, fp);
      fclose(fp);
      cbuf[16] = cbuf[22] = 0;

#ifdef DEBUG
      printf("%s %s\n", cbuf, CVS.global_cvs);
#endif

      if(strncmp(cbuf, CVS.global_cvs, 15)) return -2;
      curcvscode = atoi(&cbuf[17]);

      for(j = 0 ; j < 32 ; j++) {
#ifdef DEBUG
         printf("%d %d\n", CVS.code[j].cvsid, curcvscode);
#endif
         if(CVS.code[j].cvsid > curcvscode) {
            char buf2[256];
            int k;

            if(!strcmp(mode, "visit")) {
               sprintf(buf, "[MESSAGE] We have new Patch above CVSID: %d",
               CVS.code[j].cvsid);
               write_log(buf);
               return 999;
            }

            chdir(CVS_WORK);
#ifdef DEBUG
            puts(CVS.code[j].checksum.filename);
#endif
            sprintf(buf, "wget -c %s/%d-%d/%s.tgz", OFFICAL_URL, year, month,
                    CVS.code[j].checksum.filename);

            sprintf(buf2, "%s.sum", buf);

            for(k = 0 ; k < ERROR_COUNT ; k++)
               if(system(buf)) continue;
               else break;
            if(k == ERROR_COUNT) return -3;

            for(k = 0 ; k < ERROR_COUNT ; k++)
               if(system(buf2)) continue;
               else break;
            if(k == ERROR_COUNT) return -3;

            sprintf(buf2, "%s.tgz", CVS.code[j].checksum.filename);
            if(check_sum(buf2)) return -4;
            sprintf(buf2, "tar xfz %s.tgz > /dev/null 2>&1",
                    CVS.code[j].checksum.filename);
            system(buf2);
            sprintf(buf2, "%s/work", CVS_WORK);
            chdir(buf2);

            sprintf(buf, "%s/files.info", buf2);
            if((fp = fopen(buf, "rb")) == NULL) return -5;
            else {
               tarball finfo;
               k = fread(&finfo, sizeof(finfo), 1, fp);
               fclose(fp);
               if(k != 1) return -6;
               if(finfo.files > 20) return -7;

               for(k = 0 ; k < finfo.files ; k++) {
                  if(finfo.fname[k][0] == '+') {
                    sprintf(buf, "APPEND New file %s", finfo.ffpath[k]);
                    write_log(buf);

                    sprintf(buf, "cp -f %s %s", &finfo.fname[k][1],
                            finfo.ffpath[k]);

                  }
                  else if(finfo.fname[k][0] == '^') {
                    sprintf(buf, "APPEND New Directory %s", finfo.ffpath[k]);
                    write_log(buf);

                    sprintf(buf, "mkdir %s > null 2>&1", finfo.ffpath[k]);
                  }
                  else if(finfo.fname[k][0] == '!') {
                    int cnt;

                    if(!strchr(finfo.ffpath[k], ':')) return -9;

                    if(!strcmp(mode, "force-update")) {
                      strcpy(buf, finfo.ffpath[k]);

                      for(cnt = 0 ; cnt < 255 ; cnt++) {
                        if((uschar)buf[cnt] < ' ') break;
                        if(buf[cnt] == ':') buf[cnt] = ' ';
                      }
                    }
                    else {
                      sprintf(buf, "GET Command to RUN %s", finfo.ffpath[k]);
                      write_log(buf);

                      continue;
                    }
                  }
                  else {
                    if(!strcmp(mode, "force-update"))
                       sprintf(buf, "patch -f -p0 %s < ./%s", finfo.ffpath[k],
                               finfo.fname[k]);
                    else
                       sprintf(buf, "patch -p0 %s < ./%s", finfo.ffpath[k],
                               finfo.fname[k]);
                  }

                  if((fp = fopen(CVS_TEMP, "w+")) == NULL) return -8;
                  fprintf(fp, "%s / %s", finfo.ffpath[k], finfo.fname[k]);
                  fclose(fp);

                  if(system(buf)) return -9;
                  unlink(CVS_TEMP);

                  if((fp = fopen(CVS_CUR, "w+")) == NULL) return -10;
                  fprintf(fp, "%s", CVS.code[j].cvs);
                  fclose(fp);

                  if((fp = fopen(CVS_FILE, "w+")) == NULL) return -11;
                  fprintf(fp, "%s %.5d", cbuf, CVS.code[j].cvsid);
                  fclose(fp);
               }

               sprintf(buf, "[SUCCESS] Patch to %s OK! ...",
                       CVS.code[j].checksum.filename);
               write_log(buf);
            }
         }
      }
   }
   else return -999;

   fp = fopen(CVS_CUR, "w+");
   fprintf(fp, "%s", CVS.global_cvs);
   fclose(fp);

   return 0;
}
