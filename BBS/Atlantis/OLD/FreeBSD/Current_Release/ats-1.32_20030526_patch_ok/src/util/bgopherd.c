/*-------------------------------------------------------*/
/* util/bgopherd.c      ( NTHU CS MapleBBS Ver 2.39 )    */
/*-------------------------------------------------------*/
/* target : simple gopher daemon for BBS                 */
/* create : 95/03/29                                     */
/* update : 96/10/07                                     */
/*-------------------------------------------------------*/


#include "bbs.h"


#include <sys/wait.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <syslog.h>


#define GOPHER_PORT     70
#define GOPHER_HOME     (BBSHOME "/man")

#define TCP_QLEN        5


#define GOPHER_LOGFILE  "/var/run/bgopherd.log"
#define GOPHER_PIDFILE  "/var/run/bgopherd.pid"

static struct sockaddr_in xsin;
static int rfds;                /* read file descriptor set */

dashf(fname)
  char *fname;
{
  struct stat st;

  return (stat(fname, &st) == 0 && S_ISREG(st.st_mode));
}



/* ----------------------------------------------------- */
/* operation log and debug information                   */
/* ----------------------------------------------------- */


int flog;                       /* log file descriptor */


void
logit(key, msg)
  char *key;
  char *msg;
{
  time_t now;
  struct tm *p;
  char buf[256];

  time(&now);
  p = localtime(&now);
  sprintf(buf, "%02d/%02d/%02d %02d:%02d:%02d %-7s%s\n",
    p->tm_year, p->tm_mon + 1, p->tm_mday,
    p->tm_hour, p->tm_min, p->tm_sec, key, msg);
  write(flog, buf, strlen(buf));
}


void
log_init()
{
  flog = open(GOPHER_LOGFILE, O_WRONLY | O_CREAT | O_APPEND, 0644);
  logit("START", "gopher daemon");
}


void
log_close()
{
  close(flog);
}


/*
woju
*/
int
invalid_pname(str)
  char *str;
{
  char *p1, *p2, *p3;

  p1 = str;
  while (*p1) {
     if (!(p2 = strchr(p1, '/')))
        p2 = str + strlen(str);
     if (p1 + 1 > p2 || p1 + strspn(p1, ".") == p2)
        return 1;
     for (p3 = p1; p3 < p2; p3++)
        if (!isalnum(*p3) && !strchr("@[]-._", *p3))
          return 1;
     p1 = p2 + (*p2 ? 1 : 0);
  }
  return 0;
}



/* ----------------------------------------------------- */
/* send file out in text-mail format                     */
/* ----------------------------------------------------- */


send_file(char* fpath)
{
  FILE *fp;
  struct stat st;

  if (!stat(fpath, &st) && S_ISREG(st.st_mode) && (fp = fopen(fpath, "r")))
  {
    char buf[512];

    while (fgets(buf, 511, fp)) {
       strtok(buf, "\n\r");
       if (strcmp(buf, ".") == 0)
          puts("..");
       else
          puts(buf);
    }
    fclose(fp);
  }
}



/* ----------------------------------------------------- */
/* transform .Names to gopher index format & send it out */
/* ----------------------------------------------------- */

a_timestamp(buf, time)
  char *buf;
  time_t *time;
{
  struct tm *pt = localtime(time);
  sprintf(buf, "%02d/%02d/%02d", pt->tm_mon + 1, pt->tm_mday, pt->tm_year);
}



send_index(char* fpath, char* plus)
{
   int mode;
   FILE *fp;
   static char gtitle[80], gserver[80], gport[8];
   char *ptr, buf[1024];
   char outbuf[2048];
   char pname[MAXPATHLEN];
   fileheader fhdr;
   time_t dtime;
   char date[20];

 #ifdef  DEBUG
   logit("index", fpath);
 #endif

   sprintf(outbuf, "%s.DIR", fpath);
   if (fp = fopen(outbuf, "r")) {
      while (fread(&fhdr, sizeof(fhdr), 1, fp) == 1) {
         strcpy(gtitle, fhdr.title);
         gethostname(gserver, MAXHOSTNAMELEN);
         strcpy(gport, "70");
         sprintf(pname, "%s%s", fpath, fhdr.filename);
         mode = dashf(pname) ? 0 : 1;
         if (invalid_pname(pname))
            continue;
         sprintf(outbuf, "%c/%s", mode + '0', pname);
         ptr = buf;
         strcpy(ptr, outbuf);
         dtime = atoi(fhdr.filename + 2);
         a_timestamp(date, &dtime);
         if (*fhdr.filename == 'H') {
            char *s;
            char buffer[200];
            FILE* fp;

            if (fp = fopen(pname, "r")) {
               fgets(buffer, 200, fp);
               fclose(fp);
            }
            else
               *buffer = 0;
            s = strtok(buffer, ": \r\n");
            if (!s)
               s = "localhost";
            strcpy(gserver, s);
            strcpy(buf, "1/");
            if (s = strtok(0, " \n\r:")) {
               strcat(buf, *s == '/' ? s + 1 : s);
               if ((s = strtok(0, " :/\n\r")) && atoi(s))
                  strcpy(gport, s);
            }
            mode = 1;
         }
         printf("%s%c%-43.42s%-13s[%s]\t%s\t%s\t%s\r\n",
           plus, '0' + mode, gtitle, fhdr.owner, date, ptr, gserver, gport);
      }
      fclose(fp);
   }
}


/* ----------------------------------------------------- */
/* parse client's command and serve it                   */
/* ----------------------------------------------------- */


serve()
{
   register char *ptr;
   int ch;
   char buf[1024];

   if (fgets(buf, 1024, stdin)) {
      strtok(buf, "\r\n");
      ptr = buf;
      ch = *ptr;
      if (!ch || ch == '\r' || ch == '\n') {
         *buf = 0;
         ch = '1';
      }

      logit("cmd", ptr);

      switch (ch)
      {
      case '0':
        ptr += 2;
        if (!invalid_pname(ptr))
          send_file(ptr);
        break;

      case '1':
        if (strlen(buf) <= 2)
          *ptr = '\0';
        else
        {
          strcat(ptr, "/");
          ptr += 2;
        }
        send_index(ptr, "");
        break;

      case '\t':
        puts("+-1");
        send_index("", "+INFO: ");
        break;

      default:
        return;
      }
      puts(".");
      fflush(stdout);
   }
}


/* ----------------------------------------------------- */
/* daemon : initialize and other stuff                   */
/* ----------------------------------------------------- */


start_daemon()
{
  int n, on;
  char buf[80];
  struct sockaddr_in fsin;

  /*
   * More idiot speed-hacking --- the first time conversion makes the C
   * library open the files containing the locale definition and time zone.
   * If this hasn't happened in the parent process, it happens in the
   * children, once per connection --- and it does add up.
   */

  time_t dummy = time(NULL);
  struct tm *dummy_time = localtime(&dummy);
  struct tm *other_dummy_time = gmtime(&dummy);
  strftime(buf, 80, "%d/%b/%Y:%H:%M:%S", dummy_time);

  n = getdtablesize();

  if (fork())
    exit(0);

  while (n)
    (void) close(--n);

  n = open(GOPHER_PIDFILE, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (n >= 0)
  {
    sprintf(buf, "%5d\n", getpid());
    write(n, buf, 6);
    close(n);
  }

  (void) open("/dev/null", O_RDONLY);
  (void) dup2(0, 1);
  (void) dup2(0, 2);

  if ((n = open("/dev/tty", O_RDWR)) > 0)
  {
    ioctl(n, TIOCNOTTY, 0);
    close(n);
  }
  openlog("bgopherd", LOG_PID, LOG_AUTH);
  syslog(LOG_NOTICE, "start\n");
}

int
bind_port(port)
  int port;
{
  int sock, on;

  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0)
  {
    syslog(LOG_NOTICE, "socket\n");
    exit(1);
  }

  on = 1;
  if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof(on)) < 0)
    syslog(LOG_ERR, "(SO_REUSEADDR): %m");
  on = 0;

#if 0                           /* 0825 */
  on = 4096;
  setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char *) &on, sizeof(on));
  setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char *) &on, sizeof(on));
#endif

  xsin.sin_port = htons(port);
  if (bind(sock, (struct sockaddr *)&xsin, sizeof xsin) < 0) {
    syslog(LOG_INFO, "bgopherd bind_port can't bind to %d",port);
    exit(1);
  }

  if (listen(sock, TCP_QLEN) < 0) {
    syslog(LOG_INFO, "bgopherd bind_port can't listen to %d",port);
    exit(1);
  }

  FD_SET(sock, (fd_set *) & rfds);
  return sock;
}

void
reaper()
{
  int state, pid;
  while ((pid = waitpid(-1, &state, WNOHANG | WUNTRACED)) > 0);
}


void
abort_server()
{
  log_close();
  exit(1);
}


void
reapchild()
{
  int state, pid;

  /* signal(SIGCHLD, reapchild); */
  while ((pid = waitpid(-1, &state, WNOHANG | WUNTRACED)) > 0);
}



main(int argc, char** argv)
{
  int msock, csock, ofds, nfds, sr;
  extern int errno;
  fd_set rset;
  struct timeval tv;
  pid_t pid, slot;

  start_daemon();
  signal(SIGCHLD, reapchild);

  memset(&xsin, 0, sizeof(xsin));
  xsin.sin_family = AF_INET;
  rfds = 0;


  log_init();

  (void) signal(SIGHUP, abort_server);
  (void) signal(SIGCHLD, reaper);


  if (argc > 1)
  {
    msock = -1;
    for (nfds = 1; nfds < argc; nfds++)
    {
      csock = atoi(argv[nfds]);
      if (csock > 0)
         msock = bind_port(csock);
      else
         break;
    }
    if (msock < 0) {
      syslog(LOG_INFO, "bgopherd started with invalid arguments (no port)");
      exit(1);
    }
  }
  else {
    static int ports[] = {70 /*, 4001, 4002, 4003, 4004, 4005 */ };

    for (nfds = 0; nfds < sizeof(ports) / sizeof(int); nfds++)
    {
      msock = bind_port(ports[nfds]);
    }
  }

  setgid(BBSGID);
  setuid(BBSUID);
  chdir(GOPHER_HOME);

  /* main loop */

  ofds = rfds;
  nfds = msock + 1;

  for (;;)
  {
forever:
    rfds = ofds;
    tv.tv_sec = 60 * 30;
    tv.tv_usec = 0;
    msock = select(nfds, (fd_set *) & rfds, NULL, NULL, &tv);

    if (msock <=  0)
       continue;

    msock = 0;
    csock = 1;
    for (;;)
    {
      if (csock & rfds)
        break;
      if (++msock >= nfds)
        goto forever;
      csock <<= 1;
    }

    slot = sizeof(xsin);
    do
    {
      csock = accept(msock, (struct sockaddr *)&xsin, (int*)&slot);
    } while (csock < 0 && errno == EINTR);

    if (csock < 0)
      continue;

    pid = fork();

    if (!pid)
    {
      close(msock);
      while (--nfds >= 0)
        close(nfds);
      dup2(csock, 0);
      dup2(csock, 1);
      serve();
      exit(0);
    }
    else
      close(csock);
  }
}

