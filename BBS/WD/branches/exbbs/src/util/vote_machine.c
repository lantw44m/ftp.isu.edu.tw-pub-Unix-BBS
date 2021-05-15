#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <time.h>
#include <sys/stat.h>
#include <unistd.h>

int init = 0;
int mypid, pid;

static int
vote(host, path, port)
  char *host;
  char *path;
  int port;
{
  int cc, sock;

  sock = dns_open(host, port);
  if (sock < 0)
  {
    printf("can't connect\n");
    return -1;
  }

  cc = strlen(path);
  if (send(sock, path, cc, 0) != cc)
  {
    printf("can't send\n");
    close(sock);
    return -1;
  }

  shutdown(sock, 2);
  close(sock);

  if (!init && pid == mypid)
  {
    printf(path);
    init++;
  }
  return -1;
}


main(argc, argv)
  int argc;
  char **argv;
{
  char *path, *host, url[4096], **cc, buf[128], start[16], end[16];
  char fname[256], tmp[256], date[32], *fext;

  struct stat st;
  struct tm *t;
  time_t now;
  int n, i, mon, day;
  int max = 0;

  if (argc > 1) max = atoi(argv[1]);
  if (max < 1) max = 1;
  time(&now);
  t = localtime(&now);

  mon = t->tm_mon;
  day = t->tm_mday;

  host = "www.win2000.org.tw";

  now -= 86400 ;

  n = max;
  mypid = getpid();
  
  if (n > 10) max = 10;

  while(n > 10)
  {
    if (!fork()) break;
    n -= 10;
  }
  

  pid = getpid();

  for (i = 0; i < max; i++)
  {
//      sprintf(url, "GET http://www.hercafe.com.tw/feel/feel.cgi?NO=2\r\n");
	strcpy(url,
"POST http://www.win2000.org.tw/addnum.asp HTTP/1.0\r\n"
"Referer: http://www.win2000.org.tw/main.asp\r\n"
"Proxy-Connection: Keep-Alive\r\n"
"User-Agent: Mozilla/4.6 [en] (Win98; I)\r\n"
"Host: www.win2000.org.tw\r\n"
"Accept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, image/png, */*\r\n"
"Accept-Encoding: gzip\r\n"
"Accept-Language: en\r\n"
"Accept-Charset: iso-8859-1,*,utf-8\r\n"
"Cookie: SITESERVER=ID=7cf18ed85eec382a0bac6ca1ec76a3f1; ASPSESSIONIDGQGGGQZD=AODNEHKAKLEJLGPJPAAIGGIF; vote=0\r\n"
"Content-type: application/x-www-form-urlencoded\r\n"
"Content-length: 31\r\n\r\n"
"ans=3&msgid=6&vote=%A7%EB%B2%BC\r\n");

    vote(host, url, 80);
    fputc('.', stderr);
  }
  fputc('\n', stderr);

}
