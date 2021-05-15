/*-------------------------------------------------------*/
/* bbcall.c       ( NTHU CS MapleBBS Ver 3.00 )          */
/*-------------------------------------------------------*/
/* author : yshuang@dragon2.net                          */
/* target : BBS 網路傳呼 0943/0946 聯華 Alphacall        */
/*                      & 0948 大眾電信                  */
/* create : 98/12/01                                     */
/* update : 98/12/10                                     */
/*-------------------------------------------------------*/
/* wsyfish 做一些必要之修改                              */
/*-------------------------------------------------------*/

#include <stdlib.h>
#include <netdb.h>
#include <sys/uio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define WEBPORT	80
#define PARA	"Connection: Keep-Alive\nUser-Agent: Mozilla/4.5b1 [en] (X11; I; FreeBSD 2.2.7-STABLE i386)\nContent-type: application/x-www-form-urlencoded\nAccept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, image/png, */*\nAccept-Encoding: gzip\nAccept-Language: en\nAccept-Charset: iso-8859-1,*,utf-8\n"

struct BBCALL_DATA
  {
    int label;			/* 1:被標記 */
    char name[13];		//12

    char class[5];		//05

    char birth[6];		//05

    char phone[19];		//18

    char bbcall[21];		//21

    char callpass[11];		//10

    char email[51];		//50

    char address[65];		//64

    char explain[65];		//64=229

  };
typedef struct BBCALL_DATA BBCALL_DATA;

int
bbcall_func(int nummode, int callmode, char *PagerNo, char *PagerPassword, char *PagerMsg)
{
  char trn[512], result[1024], sendform[512];
  char PagerYear[4], PagerYearC[3], PagerMonth[3], PagerDay[3], PagerHour[3],
    PagerMin[3];
  char PageSender[10];
  char genbuf[200];
  char LOG_FILE[100];
  char SERVER_TOOL[100];
  char *PAGER_SERVER[10] =
  {
    "0940缺",			//0940
     "www.chips.com.tw",	//0941
     "0942缺",			//0942
     "www.pager.com.tw",	//0943
     "0944缺",			//0944
     "www.tw0945.com",		//0945
     "www.pager.com.tw",	//0946
     "web1.hoyard.com.tw",	//0947
     "www.fitel.net.tw",	//0948
     "www.southtel.com.tw"	//0949
  };
  /* PAGER_CGI[2n+0]:數字 PAGER_CGI[2n+1]:文字 */
  char *PAGER_CGI[20] =
  {
    "缺", "缺",
    "/cgi-bin/paging1.pl", "/cgi-bin/paging1.pl",
    "缺", "缺",
    "/web/webtopager/tpnasp/dowebcall.asp", "/web/webtopager/tpnasp/dowebcall.asp",
    "缺", "缺",
    "/Scripts/fiss/PageForm.exe", "/Scripts/fiss/PageForm.exe",
    "/web/webtopager/tpnasp/dowebcall.asp", "/web/webtopager/tpnasp/dowebcall.asp",
    "/scripts/fp_page1.dll", "/scripts/fp_page1.dll",
    "/cgi-bin/Webpage.dll", "/cgi-bin/Webpage.dll",
    "../pager/Webpg.asp", "../pager/Webpg.asp"};
  /* PAGER_REFER[2n+0]:數字 PAGER_REFER[2n+1]:文字 */
  char *PAGER_REFER[20] =
  {
    "缺", "缺",
    "http://www.chips.com.tw:9100/WEB2P/page_1.htm", "http://www.chips.com.tw:9100/WEB2P/page_1.htm",
    "缺", "缺",
    "http://www.pager.com.tw/web/webtopager/webcall.asp", "http://www.pager.com.tw/web/webtopager/webcall.asp",
    "缺", "缺",
    "http://www.tw0945.com/call_numPg.HTM", "http://www.tw0945.com/call_AlphaPg.HTM",
    "http://www.pager.com.tw/web/webtopager/webcall.asp", "http://www.pager.com.tw/web/webtopager/webcall.asp",
    "http://web1.hoyard.com.tw/freeway/freewayi.html#top", "http://web1.hoyard.com.tw/freeway/freewayi.html#top",
    "http://www.fitel.net.tw/html/svc03.htm#top", "http://www.fitel.net.tw/html/svc03.htm#top",
    "http://www.southtel.com.tw/numpg.htm", "http://www.southtel.com.tw/Alphapg.htm"};
  /* PAGER_CHECK[2n+0]:立即 PAGER_CHECK[2n+1]:預約 */
  char *PAGER_CHECK[20] =
  {
    "缺", "缺",
    "傳送中", "預約中",
    "缺", "缺",
    "正確", "正確",
    "缺", "缺",
    "SUCCESS.HTM", "SUCCESS.HTM",
    "正確", "正確",
    "成 功\", "成 功\",
    "傳呼成功\", "傳呼成功\",
    "SUCCESS.HTM", "SUCCESS.HTM"};

  struct sockaddr_in serv_addr;
  struct hostent *hp;
  int sockfd;
  int timemode = 0;
  int whichone;

  whichone = (nummode - 940) * 2 + callmode;
  timemode = 0;
  sprintf (PagerYear, "%4d", 1999);
  sprintf (PagerYearC, "%2d", 88);
  sprintf (PagerMonth, "%02d", 1);
  sprintf (PagerDay, "%02d", 1);
  sprintf (PagerHour, "%02d", 0);
  sprintf (PagerMin, "%02d", 0);

  sprintf (LOG_FILE, "/bbs/log/0%d.log", nummode);
  sprintf (result, "\n\n----傳送紀錄開始----\n號碼: 0%3d%s \n訊息: %s\n模式: [%s] %s\n----傳送紀錄結束----\n",
	   nummode, PagerNo, PagerMsg, (timemode) ? "預約傳呼" : "立即傳呼", (timemode) ? genbuf : " ");
  f_cat (LOG_FILE, result);

  genbuf[0] = '\0';
  switch (nummode)
    {
    case 941:
      sprintf (genbuf, "=on&year=%s&month=%s&day=%s&hour=%s&min=%s", PagerYear, PagerMonth, PagerDay, PagerHour, PagerMin);
      sprintf (trn, "PAGER_NO=%s&MSG_TYPE=NUMERIC&TRAN_MSG=%s&%s%s",
	       PagerNo, PagerMsg, timemode ? "DELAY" : "NOW", timemode ? genbuf : "=on");
      break;
    case 943:
    case 946:
      sprintf (trn, "CoId=0%d&ID=%s&Name=&FriendPassword=%s&&Year=%s&Month=%s&Day=%s&Hour=%s&Minute=%s&Msg=%s",
	       nummode, PagerNo, PagerPassword, PagerYear, PagerMonth, PagerDay, PagerHour, PagerMin, PagerMsg);
      break;
    case 945:
      sprintf (genbuf, "txYear=%s&txMonth=%s&txDay=%s&txHour=%s&txMinute=%s", (timemode == 2) ? PagerYear : "", (timemode == 2) ? PagerMonth : "", (timemode == 2) ? PagerDay : "", (timemode == 2) ? PagerHour : "", (timemode == 2) ? PagerMin : "");
      if (callmode)
	sprintf (trn, "hiUsage=AlphaPage&hiLanguage=Taiwan&hiDataDir=R:\fiss\RmtFiles&txPagerNo=%s&txPassword=%s&txSender=WD&txContent=%s&&%s",
		 PagerNo, PagerPassword, PagerMsg, genbuf);
      else
	sprintf (trn, "hiUsage=NumericPage&hiLanguage=Taiwan&hiDataDir=R:\fiss\RmtFiles&txPagerNo=%s&txPassword=%s&txContent=%s&&%s",
		 PagerNo, PagerPassword, PagerMsg, genbuf);
      break;
    case 947:
      sprintf (trn, "AccountNo_0=%s&Password_0=%s&Sender=WD&Message=%s",
	       PagerNo, PagerPassword, PagerMsg);
      break;
    case 948:
      sprintf (trn, "MfcISAPICommand=SinglePage&svc_no=%s&reminder=%s&year=%s&month=%s&day=%s&hour=%s&min=%s&message=%s\n",
	       PagerNo, (timemode) ? "1" : "0", PagerYearC, PagerMonth, PagerDay, PagerHour, PagerMin, PagerMsg);
      break;
    case 949:
      sprintf (genbuf, "txYear=%s&txMonth=%s&txDay=%s&txHour=%s&txMinute=%s", (timemode == 2) ? PagerYear : "", (timemode == 2) ? PagerMonth : "", (timemode == 2) ? PagerDay : "", (timemode == 2) ? PagerHour : "", (timemode == 2) ? PagerMin : "");
//      if(callmode)
      sprintf (PageSender, "&txSender=WD-BBS");
      sprintf (trn, "func=%s&txPagerNo=%s&username=%s%s&txPassword=%s&txContent=%s&tran_type=%s&%s",
	       callmode ? "Alphapg" : "Numpg", PagerNo, "", callmode ? PageSender : "", PagerPassword, PagerMsg, timemode ? "off" : "on", genbuf);

/*      else
   sprintf(trn,"func=Alphapg&hiDataDir=S:\fiss\RmtFiles&hiInterval=0&hiLanguage=Taiwan&hiUsage=NumericPage&txPagerNo=%s&username=%s&txPassword=%s&txContent=%s&tran_type=%s&%s",
   PagerNo,"",PagerPassword,PagerMsg,timemode?"off":"on",genbuf); */
      break;
    }
  sprintf (sendform, "POST %s HTTP/1.0\nReferer: %s\n%sContent-length:%d\n\n%s",
       PAGER_CGI[whichone], PAGER_REFER[whichone], PARA, strlen (trn), trn);

  sockfd = socket (AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
    return;

  memset ((char *) &serv_addr, 0, sizeof (serv_addr));
  serv_addr.sin_family = AF_INET;

  strcpy (SERVER_TOOL, PAGER_SERVER[nummode - 940]);
  if ((hp = gethostbyname (SERVER_TOOL)) == NULL)
    return;

  memcpy (&serv_addr.sin_addr, hp->h_addr, hp->h_length);
  switch (nummode)
    {
    case 943:
    case 945:
    case 946:
    case 947:
    case 948:
    case 949:
      serv_addr.sin_port = htons (WEBPORT);
      break;
    case 941:
      serv_addr.sin_port = htons (9100);
      break;
    }
  if (connect (sockfd, (struct sockaddr *) &serv_addr, sizeof serv_addr))
    {
      sprintf (result, "無法連線，傳呼失敗\n");
      f_cat (LOG_FILE, result);
      return;
    }
  write (sockfd, sendform, strlen (sendform));
  shutdown (sockfd, 1);
  while (read (sockfd, result, sizeof (result)) > 0)
    {
      if (strstr (result, PAGER_CHECK[(nummode - 940) * 2 + timemode]) != NULL)
	{
	  close (sockfd);
	  f_cat (LOG_FILE, result);
	  return;
	}
      f_cat (LOG_FILE, result);
      memset (result, 0, sizeof (result));
    }
  close (sockfd);
  return;
}

main(argc, argv)
  int argc;
  char **argv;
{
  int i;
  int nummode, callmode;
  char *PagerNo, *PagerPassword, *PagerMsg; 

  if (argc < 5)
  {
    printf("Usage: %s <0???> <數字機0 or 文字機1> <Num> <Passwd> <Msg>\n", argv[0]);
    exit(1);
  }
  nummode = atoi(argv[1]);
  callmode = atoi(argv[2]);
  PagerNo = argv[3];
  PagerPassword = argv[4];
  PagerMsg = argv[5];
  bbcall_func(nummode, callmode, PagerNo, PagerPassword, PagerMsg); 
}
