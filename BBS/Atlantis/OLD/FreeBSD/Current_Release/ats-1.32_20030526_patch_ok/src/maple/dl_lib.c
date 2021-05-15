/* ----------------------------------------------------- */
/* author : thor.bbs@bbs.cs.nthu.edu.tw                  */
/* target : dynamic link modules library for maple bbs   */
/* create : 99/02/14                                     */
/* update :   /  /                                       */
/* ----------------------------------------------------- */

#include <dlfcn.h>
#include <varargs.h>
#include <strings.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <fcntl.h>

#define BBSHOME "/home/bbsrs"
#define  RECORD_LOG     /* Dopin: 設定記錄 log 檔 預設不要 省點空間 :p */

#ifdef  RECORD_LOG
void f_cat(char *fpath, char *msg) {
  int fd;

  if((fd = open(fpath, O_WRONLY | O_CREAT | O_APPEND, 0600)) >= 0) {
    strcat(msg,"\n");
    write(fd, msg, strlen(msg));
    close(fd);
  }
}
#endif

typedef struct {
  char *path;
  void *handle;
} DL_list;

DL_list  *dl_pool;
int dl_size, dl_head;

#define DL_ALLOC_MIN    20

#ifdef  RECORD_LOG
void log_dl(char *mode, char *msg) {
  char buf[512];
  time_t now;

  time(&now);
  sprintf(buf, "%s %s %s", Cdate(&now), mode, msg);
  f_cat(BBSHOME "/log/dlog", buf);
}
#endif

void *DL_get(char *name) {                 /* format: "Xmodule_path:Xname" */
  char buf[128] = {0}, *t;
  DL_list *p, *tail;

  strcpy(buf, name);
  if(!(t = strchr(buf, ':'))) {
#ifdef  RECORD_LOG
    log_dl("DL_NAME_ERROR", buf);
#endif
    return NULL;
  }

#ifdef  RECORD_LOG
  log_dl("DL_NAME", t);
#endif
  *t++ = 0;

  if(!dl_pool) {
    /* Initialize DL entries */
    dl_size = DL_ALLOC_MIN;
    dl_head = 0;
    dl_pool = (DL_list *)malloc(dl_size * sizeof(DL_list));
#ifdef  RECORD_LOG
    log_dl("DL_NEW", buf);
#endif
  }

  p = dl_pool;
  tail = p + dl_head;

  while(p < tail) {
    if(!strcmp(buf, p->path)) {
      break;
#ifdef  RECORD_LOG
      log_dl("DL_PATH", p->path);
#endif
    }
    p++;
  }

  if(p >= tail) {     /* not found */
    if(dl_head >= dl_size) { /* not enough space */
      dl_size += DL_ALLOC_MIN;
      dl_pool = (DL_list *)realloc(dl_pool, dl_size * sizeof(DL_list));
      p = dl_pool + dl_head; /* Thor.991121: to a new place */
    }

    p->handle = dlopen((p->path = (char *)strdup(buf)), RTLD_NOW);
    dl_head ++;
#ifdef  RECORD_LOG
    log_dl("DL_RUN", buf);
#endif
  }

  if(!p->handle) {
#ifdef  RECORD_LOG
    log_dl("DL_FAIL", buf);
#endif
    return NULL;
  }

#ifdef  RECORD_LOG
  log_dl("DL_CLOSE", t);
#endif

  return dlsym(p->handle,t);
}

int DL_func(va_alist)
va_dcl
{
  va_list args;
  char *name;
  int (*f)(), ret;

  va_start(args);
  name = va_arg(args, char *);
  if(!(f = DL_get(name))) return -1; /* not get func */

  ret = (*f)(args);
  va_end(args);

  return ret;
}
