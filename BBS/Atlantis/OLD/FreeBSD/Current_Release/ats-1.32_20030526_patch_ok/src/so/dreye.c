/*-------------------------------------------------------*/
/* dreye.c    (YZU WindTopBBS Ver 3.02 )                 */
/*-------------------------------------------------------*/
/* author : statue.bbs@bbs.yzu.edu.tw                    */
/* target : Dreye 譯典通線上字典                         */
/* create : 01/07/09                                     */
/* update : Dopin (for SOB Atlantis Version) 02/08/17    */
/*-------------------------------------------------------*/

/*
普通
http://www.dreye.com/tw/dict/dict.phtml?w=hello&d=010300
變化形
http://www.dreye.com/tw/dict/dict.phtml?w=hello&d=010301
同義字/反義字
http://www.dreye.com/tw/dict/dict.phtml?w=hello&d=010304
*/

#include "bbs.h"

#ifdef DREYE         /* Dopin: 有定義才引入 否則浪費記憶體 */

#undef QUERY
#include "dns.h"

char radix32[32];

/* Thor.990331: n src 的三度空間 */
void url_encode(unsigned char *dst, unsigned char *src) {
   for( ; *src ; src++) {
      if(*src == ' ')
         *dst++ = '+';
      else if(!not_alnum(*src))
         *dst++ = *src;
      else {
         register cc = *src;
         *dst++ = '%';
         *dst++ = radix32[cc >> 4];
         *dst++ = radix32[cc & 0xf];
      }
   }
   *dst = '\0';
}

#define HTTP_PORT       80
#define SERVER_dreye    "www.dreye.com"
#define CGI_dreye       "/tw/dict/dict.phtml"
#define REF             "http://www.dreye.com.tw/"

static int http_conn(char *server, char *s) {
  int sockfd, start_show;
  int cc, tlen;
  char *xhead, *xtail, tag[10], fname[50];
  static char pool[2048];
  FILE *fp;

  if((sockfd = dns_open(server, HTTP_PORT)) < 0) {
    outmsg("無法與伺服器取得連結，查詢失敗");
    return 0;
  }
  else out2line(1, b_lines-1, "正在連接伺服器，請稍後... [按任意鍵離開] ");

  write(sockfd, s, strlen(s));
  shutdown(sockfd, 1);

  /* parser return message from web server */
  xhead = pool;
  xtail = pool;
  tlen = 0;
  start_show = 0;

  sprintf(fname, "tmp/%s.dreye", cuser.userid);

  fp = fopen(fname, "w");

  for (;;) {
    if(xhead >= xtail) {
      xhead = pool;
      cc = read(sockfd, xhead, sizeof(pool));
      if(cc <= 0) break;
      xtail = xhead + cc;
    }
    cc = *xhead++;

    if((tlen == 7) && (!strncmp(tag, "/table", 6))) start_show = 1;

    /* 不印 buttom 的 Dr.eye */
    if((tlen == 3) && (!strncmp(tag, "hr", 2))) start_show = 0;
    if((tlen == 3) && (!strncmp(tag, "td", 2))) fputc(' ', fp);
    if((tlen == 6) && (!strncmp(tag, "table", 5))) fputc('\n', fp);
    if((tlen == 4) && (!strncmp(tag, "div", 3))) fputc('\n', fp);

    if(cc == '<') {
      tlen = 1;
      continue;
    }

    if(tlen) {
      /* support<br>and<P>and</P> */

      if(cc == '>') {
        if((tlen == 3) && (!strncmp(tag, "tr", 2)))  fputc('\n', fp);
        else if ((tlen == 2) && (!strncmp(tag, "P", 1)))  fputc('\n', fp);
        else if ((tlen == 3) && (!strncmp(tag, "br", 2)))  fputc('\n', fp);

        tlen = 0;
        continue;
      }

      if(tlen <= 6) tag[tlen - 1] = cc;

      tlen++;
      continue;
    }

    if(start_show) if(cc != '\r' && cc != '\n') fputc(cc, fp);
  }
  close(sockfd);
  fputc('\n', fp);
  fclose(fp);

  more(fname, YEA);

  getdata(b_lines, 0, "  是否將查詢結果存入信箱 y/N ? ", tag, 2, LCECHO, 0);
  if(*tag == 'y') mail2user(cuser.userid, "[SYSTEM]", " 譯典通查詢結果 ",
                            fname, 'H');
  else unlink(fname);

  return FULLUPDATE;
}

static int dreye(char *word, char *ans) {
  char atrn[256], sendform[512];
  char ue_word[90];
  int d;

  url_encode(ue_word, word);

  if(ans[0] == '3') d = 10304;
  else if(ans[0] == '2') d = 10301;
  else d = 10300;

  sprintf(atrn, "w=%s&d=%d", ue_word, d);
  sprintf(sendform, "GET %s?%s HTTP/1.0\n\n", CGI_dreye, atrn);
  http_conn(SERVER_dreye, sendform);

  return FULLUPDATE;
}

int main_dreye() {
  char ans[2];
  char word[30];

  ans[0] = '1';
  while(1) {
    clear();
    move(1, 23);
    outs("\033[1;37;44m◎ Dreye譯典通線上字典 v0.1 ◎\033[m");
    move(3, 0);
    outs("  此字典來源為 Dreye 譯典通線上字典。\n");
    outs("  WWW: http://www.dreye.com/\n");
    outs("  author: statue.bbs@bbs.yzu.edu.tw\n");
    outs("  modify: Dopin.bbs@ats.twbbs.org");

    getdata(8, 0, "  word: ", word, 30, LCECHO, 0);
    if(!word[0]) break;

    getdata(9, 0, "  (1)意義 (2)變化形 (3)同義字/反義字 (Q)離開 [1] ",
            ans, 3, LCECHO, 0);
    if(ans[0] != 'q') dreye(word, ans);
    else break;
  }

  return FULLUPDATE;
}
#endif
