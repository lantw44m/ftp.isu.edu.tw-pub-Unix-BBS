/*-------------------------------------------------------*/
/* util/bhttpd.c        ( NCKU CivilEngineer LeeymBBS)   */
/*-------------------------------------------------------*/
/* target : BBS's HTTP interface                         */
/* create : 97/04/07                                     */
/* update : 97/04/08                                     */
/*-------------------------------------------------------*/
/* author : leeym@mail.civil.ncku.edu.tw                 */
/*-------------------------------------------------------*/
/* syntax : bhttpd                                       */
/*-------------------------------------------------------*/

#include "bbs.h"
#include "dopin.h"
#include <sys/ipc.h>
#include <sys/shm.h>

#define PORT 8080

/*nekobe HTML Tag define
        0:enable
        1:disable       */
#define ENABLE_TAG      1

#define ACCESSLOG       BBSHOME "/etc/bhttpd-access.log"
#define ERRORLOG        BBSHOME "/etc/bhttpd-error.log"

/* Dopin */
#define COUNTERDIR      BBSHOME "/www/counter/"

#define BBSIMG          "newage.stat.tku.edu.tw/~tku/pic/tkupro.gif"
#define BBSIMG_S        "newage.stat.tku.edu.tw/~tku/pic/tkupro_s.gif"
#define BBSBACK         "newage.stat.tku.edu.tw/pic/tkupro.gif"
#define BBSANIMATION    "http://newage.stat.tku.edu.tw/~tku/pro/pro_flash.html"

#define Nekobe_NAME     "Nekobe Wu."
#define Nekobe_EMAIL    "nekobe@hpl.me.tku.edu.tw"
#define Gene_NAME       "Gene Hong"
#define Gene_EMAIL      "gene@iim.nctu.edu.tw"
#define Leeym_NAME      "Yen-Ming Lee"
#define Leeym_EMAIL     "leeym@mail.civil.ncku.edu.tw"

#define PROFILE_NUM    10
#define PROFILE_SIZE 1024

int ansi_on, ansi_blink, ansi_hlight;

#define field_count 5
#define MAX_WORDS       1024

#define WHITE   "#ffffff"
#define GRAY    "#c0c0c0"
#define BLACK   "#000000"

#define RED     "#a00000"
#define LRED    "#ff0000"

#define GREEN   "#007000"
#define LGREEN  "#00ff00"

#define BLUE    "#0000a0"
#define LBLUE   "#0000ff"

#define YELLOW  "#a0a000"
#define LYELLOW "#ffff00"

#define PURPLE  "#a000a0"
#define LPURPLE "#ff00ff"

#define CRAY    "#00a0a0"
#define LCRAY   "#00ffff"

/*-------------------------------------------------------*/
/* .UTMP cache                                           */
/*-------------------------------------------------------*/

struct UTMPFILE *utmpshm;
userec uec;
user_info *ushm_head, *ushm_tail;

struct BCACHE *brdshm;
boardheader allbrd[MAXBOARD];
boardheader *bcache;
int includetext = 0, header = 1, readline = 0;
char protocol[20] = "";

struct user_info *uentp;
char field_str[ field_count ][ 128 ];
int  field_lst_no [ field_count ];
int  user_num = 0;

int  field_lst_size [ field_count ] = {
   12, 20, 18, 15,  8
};

char *field_name[] = {
    "¥N¸¹",
    "¼ÊºÙ",
    "¬G¶m",
    "°ÊºA",
    "µo§b",
    NULL
};

/* Gene's include */

char *fn_board=".BOARDS";

int dashl(fname)
  char *fname;
{
  struct stat st;

  return (lstat(fname, &st) == 0 && S_ISLNK(st.st_mode));
}

int numboards = -1;

int
ci_strcmp(s1, s2)
  register char *s1, *s2;
{
  register int c1, c2, diff;

  do
  {
    c1 = *s1++;
    c2 = *s2++;
    if (c1 >= 'A' && c1 <= 'Z')
      c1 |= 32;
    if (c2 >= 'A' && c2 <= 'Z')
      c2 |= 32;
    if (diff = c1 - c2)
      return (diff);
  } while (c1);
  return 0;
}

static void
attach_err(shmkey, name)
  int shmkey;
  char *name;
{
  fprintf(stderr, "[%s error] key = %x\n", name, shmkey);
  exit(1);
}

static void *attach_shm(shmkey, shmsize)
  int shmkey, shmsize;
{
  void *shmptr;
  int shmid;

  shmid = shmget(shmkey, shmsize, 0);
  if (shmid < 0)
  {
    shmid = shmget(shmkey, shmsize, IPC_CREAT | 0600);
    if (shmid < 0)
      attach_err(shmkey, "shmget");
    shmptr = (void *) shmat(shmid, NULL, 0);
    if (shmptr == (void *) -1)
      attach_err(shmkey, "shmat");
    memset(shmptr, 0, shmsize);
  }
  else
  {
    shmptr = (void *) shmat(shmid, NULL, 0);
    if (shmptr == (void *) -1)
      attach_err(shmkey, "shmat");
  }
  return shmptr;
}

void
resolve_boards()
{
  if (brdshm == NULL)
  {
    brdshm = attach_shm(BRDSHM_KEY, sizeof(*brdshm));
    if (brdshm->touchtime == 0)
      brdshm->touchtime = 1;
    bcache = brdshm->bcache;
  }

  while (brdshm->uptime < brdshm->touchtime)
     reload_bcache();
  numboards = brdshm->number;
}

boardheader *
getbcache(bname)
  char *bname;
{
  register int i;
  register boardheader *bhdr;

  resolve_boards();
  for (i = 0, bhdr = bcache; i < numboards; i++, bhdr++)
    /* if (Ben_Perm(bhdr)) */
    if (!ci_strcmp(bname, bhdr->brdname))
      return bhdr;
  return NULL;
}

void
resolve_utmp()
{
  if (utmpshm == NULL)
  {
    utmpshm = attach_shm(UTMPSHM_KEY, sizeof(*utmpshm));
    if (utmpshm->uptime == 0)
      utmpshm->uptime = utmpshm->number = 1;
  }
}

reload_bcache()
{
   if (brdshm->busystate)
   {
     sleep(1);
   }
   else
   {
     int fd;

     brdshm->busystate = 1;

     if ((fd = open(fn_board, O_RDONLY)) > 0)
     {
       brdshm->number = read(fd, bcache, MAXBOARD * sizeof(boardheader))
         / sizeof(boardheader);
       close(fd);
     }

     /* µ¥©Ò¦³ boards ¸ê®Æ§ó·s«á¦A³]©w uptime */

     brdshm->uptime = brdshm->touchtime;
     /*log_usies("CACHE", "reload bcache");*/
     brdshm->busystate = 0;
   }
}

/* Gene's include end */

int
counter(fname)
  char *fname;
{
  char buf[80];
  FILE *fn;
  int num=0;

  sprintf(buf, "%s%s", COUNTERDIR, fname);
  if (fn=fopen(buf, "r"))
  {
    fscanf(fn, "%d", &num);
    fclose(fn);
  }
  num++;
  if (fn=fopen(buf, "w"))
  {
    fprintf(fn, "%d", num);
    fclose(fn);
  }
  return num;
}

print_head()
{
    int i, size;
    printf("<center><TD bgcolor=%s>No.\n", LYELLOW);
    for (i = 0; i < field_count; i++) {
        size  = field_lst_size[ i ];
        printf("<TD bgcolor=%s>%-*.*s ", LYELLOW, size, size, field_name[i]);
    }
    printf("<TR>\n");
}


print_record()
{
    int i, size;

    for (i = 0 ; i < field_count; i++) {
        size  = field_lst_size[ i ];
        if(i==0)

        printf("<TD bgcolor=%s align=center>%-d<td bgcolor=%s>"
        "<font size=4><A HREF=http:/~%s/>%-*.*s</A></font>\n",
        LCRAY, user_num+1, LGREEN,
        field_str[i], size, size, field_str[i]);

/*nekobe*/
       else if(i==1)
        printf("<TD><A HREF=mailto:%s.tku@tkupro.stat.tku.edu.tw>"
               "%s</A>",field_str[i-1],field_str[i]);

       else
/*
        printf("<TD bgcolor=%s>%-*.*s ", BLACK, size, size,
        field_str[i][0] ? field_str[i] : "<pre>");
*/
        printf("<TD>%-*.*s ", size, size,
        field_str[i][0] ? field_str[i] : "<pre>");
    }
    printf("<TR>\n");
}


char *
idle_str(tty)
  char *tty;
{
  static char hh_mm_ss[8];
  struct stat buf;

  if (stat(tty, &buf) || (strstr(tty, "tty") == NULL))
  {
    strcpy(hh_mm_ss, "Unknown");
  }
  else
  {
    time_t diff;
    diff = (time(0) - buf.st_atime);
    if (diff)
     sprintf(hh_mm_ss, "%2d:%02d\'%02d\"", diff / 3600, diff / 60 , diff % 60);
    else
     strcpy(hh_mm_ss, "<pre>");
  }
  return hh_mm_ss;
}


dump_record(serial_no, p)
int serial_no;
struct user_info *p;
{
    int i = 0;

    sprintf( field_str[i++], "%s", p->userid );
    sprintf( field_str[i++], "%s", p->username );
    sprintf( field_str[i++], "%s", p->from );
    sprintf( field_str[i++], "%s", ModeTypeTable[p->mode] );
    sprintf( field_str[i++], "%s", p->mode>WWW ? "NA": idle_str(p->tty) );
}

int
chkmail(userid)
char *userid;
{
    FILE *mail;
    char buf[80];
    struct stat st;
    fileheader fh;

    sprintf(buf, BBSHOME"/home/%s/.DIR", userid);
    stat(buf, &st);

    if( access(buf,F_OK) || !st.st_size) /* .DIR ¤£¦s¦b©Î¬° 0 size */
        return 0;
    else
     if( (mail=fopen(buf,"r")) == NULL){
         perror("fopen:.DIR");
         exit(1);
     }
     else
     while( fread(&fh,sizeof(fileheader),1,mail) > 0);
     if(fh.filemode & FILE_READ)
         return 0;
     else
         return 1;
}


int
board_cmp(a, b)
  boardheader *a, *b;
{
  int bcmp;
  char modea[8], modeb[8];
  strncpy(modea, a->title, 6);
  strncpy(modeb, b->title, 6);
  modea[6] = modeb[6] = '\0';
  bcmp = strcmp(modea, modeb);
  if (!bcmp)
    bcmp = strcasecmp(a->brdname, b->brdname);
  return bcmp;
}


int
get_num_records(fpath, size)
  char *fpath;
{
  struct stat st;
  if (stat(fpath, &st) == -1)
    return 0;
  return (st.st_size / size);
}

fileheader *
getfhdr(char *fdir, int no)
{
  int fd;
  int size = sizeof(fileheader), i = 0;
  char buf[size];
  fileheader *fhdr = (fileheader *) malloc(sizeof(fileheader));
  if (fd = open(fdir, O_RDONLY))
  {
    lseek(fd, size * (no - 1), SEEK_SET);
    if (read(fd, buf, size) == size)
    {
      close(fd);
      fhdr = (fileheader *) buf;
      return (fhdr);
    }
    else
    {
      close(fd);
      return NULL;
    }
  }
  return NULL;
}


int
getindex(fpath, fname, size)
  char *fpath;
  char *fname;
  int size;
{
  int fd, now=0;
  fileheader fhdr;

  if ((fd = open(fpath, O_RDONLY, 0)) != -1)
  {
    while((read(fd, &fhdr, size) == size)){
      now++;
      if(!strcmp(fhdr.filename, fname)){
        close(fd);
        return now;
      }
    }
    close(fd);
  }
  return 0;
}

void
accesslog(method, mesg)
  char *method, *mesg;
{
  FILE *fp;
  char genbuf[200];

  if (fp = fopen(ACCESSLOG, "a"))
  {
    time_t now = time(0);

    fprintf(fp, "client - - [%s +0800] \"%s %s %s\" 200 - \n",
      Cdate(&now), method, mesg, protocol);
    fclose(fp);
  }
}

void
errorlog(method, mesg)
  char *method, *mesg;
{
  FILE *fp;
  char genbuf[200];

  if (fp = fopen(ERRORLOG, "a"))
  {
    time_t now = time(0);

    fprintf(fp, "[%s +0800] access to %s fail for client, reason: %s does not exist - %s\n",
      Cdate(&now), method, mesg);
    fclose(fp);
  }
}


/* ----------------------------------------------------- */
/* HTML output sub finction                              */
/* ----------------------------------------------------- */

html_head(char *str, int refresh, int blackground, int firstpage)
{
  time_t now = time(0);
  if (!blackground)
    printf("<HTML><style>A{text-decoration:none}</style>"
    "<TITLE>%s BBS ¯¸ -- %s</TITLE>"
    "<BODY background=\"http://%s\">\n",
        BOARDNAME, str, BBSBACK);
  else
    printf("<HTML><style>A{text-decoration:none}</style>"
    "<TITLE>%s BBS ¯¸ -- %s</TITLE>"
    "<BODY bgcolor=%s text=%s link=%s vlink=%s alink=%s>\n",
        BOARDNAME, str, BLACK, GRAY, WHITE, GRAY, LRED);
  if (refresh)

    printf("<head><META HTTP-EQUIV=\"Refresh\" CONTENT=60></HEAD>");
  /*nekobe*/
  if (firstpage)
  {
  printf("<CENTER><IMG SRC=http://%s BORDER=0></CENTER>", BBSIMG);
  printf("<TD><CENTER><font size=6><i><b>%s</b></i></font>\n",BOARDNAME);
  /*
  printf("<HR><CENTER>
  <A HREF=http://newage.stat.tku.edu.tw/~tku/pro/pro_flash.html>­Ó¤H¯¸¶i¯¸°Êµe(»Ý­nShockwave Flash Plug-in)
  </A></CENTER><BR>");
  printf("<CENTER>½u¤W¦Û°Ê¦w¸Ë <a href=http://www.macromedia.com/shockwave/download/activex/flash/default.htm
  target=_blank>Shockwave Flash ActiveX Control Installer
                                            </a>(IE4.0¾A¥Î)<CENTER><BR>");
  printf("<CENTER><A HREF=ftp://ftp.tku.edu.tw/WWW/Plug-in/Shockwave/Shockwave_Flash_Installer.exe>¤U¸üShockware Flash
                                                               Plug-in(For Netscape)</A><BR></CENTER><HR>");*/
  }
  else
  printf("<CENTER><TABLE BORDER=0><TR><TD><IMG SRC=http://%s BORDER=0></TD>
  <TD><font size=6><i><b><a href=http://%s:%d/>%s</a></b></i></font></TD></TR></TABLE>\n",
          BBSIMG_S, MYHOSTNAME, PORT, BOARDNAME);

  /*
  printf("<TD><CENTER><font size=6><i><b>%s</b></i></font>\n",BOARDNAME);
  */
  printf("<br>");
  /*
  printf("[ BBS |\n");
  printf(" <a href=http://ftp.civil.ncku.edu.tw/>FTP</a> |\n");
  printf(" <a href=http://mailing.civil.ncku.edu.tw/>Mailing</a> |\n");
  printf(" <a href=http://mrtg.civil.ncku.edu.tw/>MRTG</a> |\n");
  printf(" <a href=http://news.civil.ncku.edu.tw/>News</a> |\n");
  printf(" <a href=http://ns.civil.ncku.edu.tw/>NS</a> |\n");
  printf(" <a href=http://proxy.civil.ncku.edu.tw/>Proxy</a> |\n");
  printf(" <a href=http://www.civil.ncku.edu.tw/wel.html>WWW</a> ]<p>\n");
  printf("<img src=http://www.civil.ncku.edu.tw/pic/cvlhr2.gif>");
  */
  printf("<img src=http://newage.stat.tku.edu.tw/pic/cvlhr2.gif>");
  printf("<p>³Ì«á§ó·s®É¶¡ %s<p>\n", ctime(&now));
  printf("[ <a href=/man/>ºëµØ¤½§GÄæ</a> |\n");
  printf(" <a href=/boards/>§G§i°Q½×°Ï</a> |\n");
  printf(" <a href=/user/>½u¤W¨Ï¥ÎªÌ</a> ]<p>\n");
}

html_error(char *url, char *thing)
{
  char buf[512];
  sprintf(buf, "¿ù»~ªº URL: %s", url);
  html_head(buf, 0, 0, 0);
  printf("<h1>§ä¤£¨ì %s<br>½ÐÀË¬dÀÉ®×¸ô®|¦WºÙ</h1>\n", url);
  errorlog(url, thing);
}

void tag8859_1( char **tag, char c )
{
/*   switch( c ) */
   {
/*
   case '\"': strcpy( *tag, "&quot " );   *tag+=6;break;
   case '<':  strcpy( *tag, "&lt " );     *tag+=4;break;
   case '>':  strcpy( *tag, "&gt " );     *tag+=4;break;
   case '&':  strcpy( *tag, "&amp " );    *tag+=5;break;
   default:
*/
      **tag=c; (*tag)++;
   }
}

int getansi( char **ap, int *attr, char *cmd )
{
   char cattr[100], *cap;

   cap = cattr; *cap=0;
   if( **ap == 0 ) return EOF;

   while( **ap>='0' && **ap<='9' )
   {
      *cap=**ap; cap++; (*ap)++;
   }
   if( cap==cattr ) return 0;

   *cap=0;
   sscanf( cattr, "%d", attr );
   if( **ap == 0 ) return 1;

   *cmd=**ap; (*ap)++;
   if( **ap == 0 ) return 2;
   return 3;
}

void add_href( char *l )
{
   char tag[MAX_WORDS], url[MAX_WORDS];
   char *ap, *tp, *u;
   int found;

   ap=l; tp=tag;

   while( *ap!=0 )
   {
      found=0;

      strncpy( url, ap, 7 ); url[7]=0;
      if (!strcasecmp(url,"http://") && (ENABLE_TAG==1))
        found=1;

      strncpy( url, ap, 6 ); url[6]=0;
      if (!strcmp(url,"ftp://") && (ENABLE_TAG==1))
        found=1;

      strncpy( url, ap, 7 ); url[7]=0;
      if (!strcmp(url,"file://") && (ENABLE_TAG==1))
        found=1;

      strncpy( url, ap, 9 ); url[9]=0;
      if (!strcmp(url,"gopher://") && (ENABLE_TAG==1))
        found=1;

      strncpy( url, ap, 9 ); url[9]=0;
      if (!strcmp(url,"telnet://") && (ENABLE_TAG==1))
        found=1;

      strncpy( url, ap, 7 ); url[7]=0;
/*nekobe*/
      if (!strcmp(url,"mailto:") && (ENABLE_TAG==1))
        found=1;

      if( found )
      {
         for( u=url; *ap!=0; *u++=*ap++ )
         {
            if( *ap==' ' ) break;
            if( *ap=='\"' ) break;
            if( *ap=='&' ) break;
            if( (unsigned char)*ap>=127 ) break;
         }
         *u=0;

         sprintf( tp, "<A HREF=\"%s\">%s</A>", url, url );
         tp+=(strlen(tp));
      }
      else /* Not URL */
      {
         *tp++=*ap++;
      }
   }

   *tp = 0;
   strcpy( l, tag );
}

void tagcolor( char **tag, int attr )
{
   switch( attr ) /* === filter no-used ansi control === */
   {
   case 0: case 5: case 1:
   case 30: case 31: case 32: case 33: case 34: case 35: case 36: case 37:
      break;

   default:
      return;
   }


   if( attr==5 )
   {
      if( ansi_blink==0 )
      { ansi_blink=1; strcpy( *tag, "<blink>" ); *tag+=7; }
      return;
   }

   if( ansi_blink ) { strcpy( *tag, "</blink>" ); *tag+=8; }

   if( ansi_on )
   {
      strcpy( *tag, "</FONT>" ); *tag+=7;
      ansi_on=0;
   }

   switch( attr )
   {
   case 0:  ansi_blink=0; return;
   case 1:  ansi_hlight=1; break;

   case 30:
        sprintf(*tag, "<FONT color=%s>", ansi_hlight ? GRAY : BLACK);
        *tag+=20;
        break;
   case 31:
        sprintf(*tag, "<FONT color=%s>", ansi_hlight ? LRED : RED);
        *tag+=20;
        break;
   case 32:
        sprintf(*tag, "<FONT color=%s>", ansi_hlight ? LGREEN : GREEN);
        *tag+=20;
        break;
   case 33:
        sprintf(*tag, "<FONT color=%s>", ansi_hlight ? LYELLOW : YELLOW);
        *tag+=20;
        break;
   case 34:
        sprintf(*tag, "<FONT color=%s>", ansi_hlight ? LBLUE : BLUE);
        *tag+=20;
        break;
   case 35:
        sprintf(*tag, "<FONT color=%s>", ansi_hlight ? LPURPLE : PURPLE);
        *tag+=20;
        break;
   case 36:
        sprintf(*tag, "<FONT color=%s>", ansi_hlight ? LCRAY : CRAY);
        *tag+=20;
        break;
   case 37:
        sprintf(*tag, "<FONT color=%s>", ansi_hlight ? WHITE : GRAY);
        *tag+=20;
        break;

   default:
      if( ansi_blink ) { strcpy( *tag, "<blink>" ); *tag+=7; }
      return;
   }

   if (ansi_blink) {
        strcpy( *tag, "<blink>" );
        *tag+=7;
   }

   ansi_on=1;
}

void ansi2tag( char *l )
{
   char tag[MAX_WORDS], esc[3];
   char *ap, *tp, cmd;
   int  attr, num, nextansi;

   esc[2]=0; nextansi=0;
   ap=l; tp=tag;
   while( *ap!=0 )
   {
      esc[0]=ap[0];esc[1]=ap[1];
      if( !strcmp(esc,"[") || nextansi )
      {
         if( nextansi ) nextansi=0; else ap+=2;
         num=getansi( &ap, &attr, &cmd );
         if( num==EOF )
         {
            /* end-of-string */
            break;
         }
         else if( num==0 )
         {
            /* ANSI+noint? eat the word */
            ap++;
         }
         else if( num==1 )
         {
            /* ANSI+int+EOL go next line */
            break;
         }
         else if( num==2 )
         {
            /* ANSI+attr+cmd+EOL, set color and go next line */
            if( cmd=='m') tagcolor( &tp, attr );
            break;
         }
         else if( num==3 )
         {
            /* ANSI+attr+cmd+str, set color.. */
            tagcolor( &tp, attr );
            if( cmd==';') nextansi=1;
         }
      }
      else
      {
         tag8859_1( &tp, *ap ); ap++;
      }
   }
   *tp = 0;
   strcpy( l, tag );
}


/* ----------------------------------------------------- */
/* BOARD                                                 */
/* ----------------------------------------------------- */

html_boardlist()
{
  FILE *fp;
  int i, count, inf;
/*nekobe*/
  int i_real;
  html_head("¬Ýª©¦Cªí", 0, 0, 0);
  /*
       printf("¤w¦³ "
         "<img src=http://newage.stat.tku.edu.tw/cgi-bin/"
         "Count.cgi??dd=C|df=board.dat></a> ¤H¦¸¨Ó¬Ýª©<p>");
  */
       printf("¤w¦³ %d ¤H¦¸¨Ó¬Ý¹L", counter("board"));
  printf("<table><tr><td bgcolor=yellow>½s¸¹</td><td bgcolor=yellow>¬Ýª©</td>"
    "<td bgcolor=yellow>¤¤¤å»¡©ú</td><td bgcolor=yellow>ª©¥D</td></tr>");

  i = count = 0;
/*nekobe*/
  i_real=0;
#if 1
  inf = open(BBSHOME "/" FN_BOARD, O_RDONLY);
  if (inf == -1)
  {
    printf("error open file\n");
    exit(1);
  }

  /* read in all boards */

  i = 0;
  memset(allbrd, 0, MAXBOARD * sizeof(boardheader));
  while (read(inf, &allbrd[i], sizeof(boardheader)) == sizeof(boardheader))
  {
    if (allbrd[i].brdname[0])
    {
      i++;
    }
  }
  close(inf);

  /* sort them by name */
  count = i;
  qsort(allbrd, count, sizeof(boardheader), board_cmp);

    /* write out the target file */

    for (i = 0; i < count; i++)
    {
      if (!allbrd[i].level && !strstr(allbrd[i].title, "£[") &&
      !strstr(allbrd[i].title, "£U") && !strstr(allbrd[i].title, "£S"))
      {
      printf("<tr><td>%d</td><td><a href=/boards/%s/>%s</a></td><td>%s</td>"
        "<td>%s</td></tr>\n", i_real+1, allbrd[i].brdname, allbrd[i].brdname,
        allbrd[i].title, allbrd[i].BM);
        i_real++;
      }
    }
#endif
  printf("</table>");
}

html_board(char *board)
{
  char buf[80], brdname[20];
  boardheader *bhdr;

  strcpy(buf, board);
  strcpy(brdname, strtok(buf, "/\n\0"));
  bhdr = getbcache(brdname);
  if (bhdr == NULL)
  {
    sprintf(buf, "/boards/%s", board);
    html_error(buf, "board");
  }
  else if (!bhdr->level && !strstr(bhdr->title, "£[") &&
        !strstr(bhdr->title, "£U") && !strstr(bhdr->title, "£S"))
  {
    if (strchr(board, '/') && strchr(strchr(board, '/') + 1, '/'))
    {
      sprintf(buf, "¬Ýª©: %s", bhdr->brdname);
      html_head(buf, 0, 0, 0);
    /*
      printf("¬Ýª© %s ¦³ "
         "<img src=http://newage.stat.tku.edu.tw/cgi-bin/"
         "Count.cgi??dd=C|df=board-%s.dat></a> ¤H¦¸<p>",
         bhdr->brdname, bhdr->brdname);
    */
      sprintf(buf, "board-%s", bhdr->brdname);
      printf("¬Ýª© %s ¦³ %d ¤H¦¸¨Ó¬Ý¹L", bhdr->brdname, counter(buf));

      html_articlelist(board);
    }
    else
    {
      sprintf(buf, "%s/boards/%s/.DIR", BBSHOME, board);
      /*
      printf("<head><META HTTP-EQUIV=Refresh "
            "CONTENT=0;URL=http://%s/boards/%s/%d/></HEAD>", MYHOSTNAME,
            bhdr->brdname, get_num_records(buf, sizeof(fileheader)));*/
      printf("<head><META HTTP-EQUIV=Refresh "
            "CONTENT=5;URL=http:/boards/%s/%d/></HEAD>",
            bhdr->brdname, get_num_records(buf, sizeof(fileheader)));
      html_head("¶iª©µe­±", 0, 0, 1);
      brd_note(board);
    }
  }
  else
  {
    html_head("¬Ýª©­­¨î¾\\Åª", 0, 0, 0);
    printf("¬Ýª© %s ­­¨î¾\\Åª<p>\n", brdname);
  }
}

/* ----------------------------------------------------- */
/* ARTICLE                                               */
/* ----------------------------------------------------- */

html_articlebar(char *board, int no)
{
  char buf[80], flastname[80], fnextname[80];
  int total;
  sprintf(buf, "%s/boards/%s/.DIR", BBSHOME, board);
  total = get_num_records(buf, sizeof(fileheader));

  if (no <= 1)
  {
    strcpy(fnextname, getfhdr(buf, no+1)->filename);
    printf("<table CELLSPACING=0 CELLPADDING=4 BORDER=0 width=80%%><tr>"
      "<td width=20%% align=center bgcolor=%s>"
      "<a href=mailto:%s.board@%s>µoªí¤å³¹</a></td>"
      "<td width=20%% align=center bgcolor=%s>¤W¤@½g¤å³¹</td>"
      "<td width=20%% align=center bgcolor=%s><a href=/boards/%s/%s>¤U¤@½g¤å³¹</a></td>"
      "<td width=20%% align=center bgcolor=%s><a href=/boards/%s/%d/>¤å³¹¦Cªí</a></td>"
      "<td width=20%% align=center bgcolor=%s><a href=/man/boards/%s/>¥»ª©ºëµØ°Ï</a></td></tr></table><p>\n",
      YELLOW, board, MYHOSTNAME, YELLOW, YELLOW, board, fnextname,
      YELLOW, board, no < 100 ? 100 : no + 100, YELLOW, board);
  }
  else if (no == total)
  {
    strcpy(flastname, getfhdr(buf, no-1)->filename);
    printf("<table CELLSPACING=0 CELLPADDING=4 BORDER=0 width=80%%><tr>"
      "<td width=20%% align=center bgcolor=%s>"
      "<a href=mailto:%s.board@%s>µoªí¤å³¹</a></td>"
      "<td width=20%% align=center bgcolor=%s><a href=/boards/%s/%s>¤W¤@½g¤å³¹</a></td>"
      "<td width=20%% align=center bgcolor=%s>¤U¤@½g¤å³¹</td>"
      "<td width=20%% align=center bgcolor=%s><a href=/boards/%s/%d/>¤å³¹¦Cªí</a></td>"
      "<td width=20%% align=center bgcolor=%s><a href=/man/boards/%s/>¥»ª©ºëµØ°Ï</a></td></tr></table><p>\n",
      YELLOW, board, MYHOSTNAME, YELLOW, board, flastname, YELLOW,
      YELLOW, board, no, YELLOW, board);
  }
  else
  {
    strcpy(flastname, getfhdr(buf, no-1)->filename);
    strcpy(fnextname, getfhdr(buf, no+1)->filename);
    printf("<table CELLSPACING=0 CELLPADDING=4 BORDER=0 width=80%%><tr>"
      "<td width=20%% align=center bgcolor=%s>"
      "<a href=mailto:%s.board@%s>µoªí¤å³¹</a></td>"
      "<td width=20%% align=center bgcolor=%s><a href=/boards/%s/%s>¤W¤@½g¤å³¹</a></td>"
      "<td width=20%% align=center bgcolor=%s><a href=/boards/%s/%s>¤U¤@½g¤å³¹</a></td>"
      "<td width=20%% align=center bgcolor=%s><a href=/boards/%s/%d/>¤å³¹¦Cªí</a></td>"
      "<td width=20%% align=center bgcolor=%s><a href=/man/boards/%s/>¥»ª©ºëµØ°Ï</a></td></tr></table><p>\n",
      YELLOW, board, MYHOSTNAME, YELLOW, board, flastname, YELLOW, board, fnextname,
      YELLOW, board, no < total - 100 ? (total - 100) : total, YELLOW, board);
  }
}

html_articlelistbar(char *board, int last)
{
  char buf[80];
  int total;
  sprintf(buf, "%s/boards/%s/.DIR", BBSHOME, board);
  total = get_num_records(buf, sizeof(fileheader));

  if (total < 100)
    printf("<table CELLSPACING=0 CELLPADDING=4 BORDER=0 width=80%%><tr>"
      "<td width=20%% align=center bgcolor=%s>"
      "<a href=mailto:%s.board@%s>µoªí¤å³¹</a></td>"
      "<td width=20%% align=center bgcolor=%s>¤W¤@¦Ê½g¤å³¹</td>"
      "<td width=20%% align=center bgcolor=%s>¤U¤@¦Ê½g¤å³¹</td>"
      "<td width=20%% align=center bgcolor=%s><a href=/boards/>¬Ýª©¦Cªí</a></td>"
      "<td width=20%% align=center bgcolor=%s><a href=/man/boards/%s/>¥»ª©ºëµØ°Ï</a></td></tr></table><p>\n",
      YELLOW, board, MYHOSTNAME, YELLOW, YELLOW, YELLOW, YELLOW, board);
  else if (last < 100)
    printf("<table CELLSPACING=0 CELLPADDING=4 BORDER=0 width=80%%><tr>"
      "<td width=20%% align=center bgcolor=%s>"
      "<a href=mailto:%s.board@%s>µoªí¤å³¹</a></td>"
      "<td width=20%% align=center bgcolor=%s>¤W¤@¦Ê½g¤å³¹</td>"
      "<td width=20%% align=center bgcolor=%s><a href=/boards/%s/%d/>¤U¤@¦Ê½g¤å³¹</a></td>"
      "<td width=20%% align=center bgcolor=%s><a href=/boards/>¬Ýª©¦Cªí</a></td>"
      "<td width=20%% align=center bgcolor=%s><a href=/man/boards/%s/>¥»ª©ºëµØ°Ï</a></td></tr></table><p>\n",
      YELLOW, board, MYHOSTNAME, YELLOW,
      YELLOW, board, last + 100 > total ? total : last + 100, YELLOW, YELLOW, board);
  else if (last == total)
    printf("<table CELLSPACING=0 CELLPADDING=4 BORDER=0 width=80%%><tr>"
      "<td width=20%% align=center bgcolor=%s>"
      "<a href=mailto:%s.board@%s>µoªí¤å³¹</a></td>"
      "<td width=20%% align=center bgcolor=%s><a href=/boards/%s/%d/>¤W¤@¦Ê½g¤å³¹</a></td>"
      "<td width=20%% align=center bgcolor=%s>¤U¤@¦Ê½g¤å³¹</td>"
      "<td width=20%% align=center bgcolor=%s><a href=/boards/>¬Ýª©¦Cªí</a></td>"
      "<td width=20%% align=center bgcolor=%s><a href=/man/boards/%s/>¥»ª©ºëµØ°Ï</a></td></tr></table><p>\n",
      YELLOW, board, MYHOSTNAME, YELLOW, board, last > 100 ? last - 100 : 0, YELLOW, YELLOW, YELLOW, board);
  else
    printf("<table CELLSPACING=0 CELLPADDING=4 BORDER=0 width=80%%><tr>"
      "<td width=20%% align=center bgcolor=%s>"
      "<a href=mailto:%s.board@%s>µoªí¤å³¹</a></td>"
      "<td width=20%% align=center bgcolor=%s><a href=/boards/%s/%d/>¤W¤@¦Ê½g¤å³¹</a></td>"
      "<td width=20%% align=center bgcolor=%s><a href=/boards/%s/%d/>¤U¤@¦Ê½g¤å³¹</a></td>"
      "<td width=20%% align=center bgcolor=%s><a href=/boards/>¬Ýª©¦Cªí</a></td>"
      "<td width=20%% align=center bgcolor=%s><a href=/man/boards/%s/>¥»ª©ºëµØ°Ï</a></td></tr></table><p>\n",
      YELLOW, board, MYHOSTNAME, YELLOW, board, last > 100 ? last - 100 : 0,
      YELLOW, board, last + 100 > total ? total : last + 100, YELLOW, YELLOW, board);
}

html_articlelist(char *str)
{
  FILE *fp;
  int i, first, last;
  char board[20], buf[80];
  fileheader fhdr;

  i = first = last = 0;
  strcpy(board, strtok(str, "/\n"));
  last = atoi(strtok(NULL, "/\n"));
  first = last > 100 ? last - 100 : 0;
  sprintf(buf, "%s/boards/%s/.DIR", BBSHOME, board);
  if (fp = fopen(buf, "r"))
  {
    html_articlelistbar(board, last);
    printf("<table><tr><td bgcolor=yellow>½s¸¹</td><td bgcolor=yellow>¤é´Á</td>"
        "<td bgcolor=yellow>§@ªÌ</td><td bgcolor=yellow>¼ÐÃD</td></tr>\n");
    while(fread(&fhdr, sizeof(fhdr), 1, fp) && ++i)
    {
      if (i < first)
        continue;
      else if (i > last)
        break;
      printf("<tr><td>%d</td><td>%s</td><td>%s</td><td>"
        "<a href=/boards/%s/%s>%s</td></tr>\n",
        i, fhdr.date, fhdr.owner, board, fhdr.filename, fhdr.title);
    }
    printf("</table>");
    html_articlelistbar(board, last);
  fclose(fp);
  }
}


char *
html_word(char *word)
{
  char *ans = (char *) malloc(sizeof(char) * 512), buf[512];
  int i, j, k, ansi;
  ansi2tag(word);
  add_href(word);
  i = j = k =0;
  buf[0] = '\0';
  if (*word == ':' || *word == '>')
  {
    sprintf(buf, "<font color=%s>%s</font>", CRAY, word);
    includetext = 1;
  }
  else if (!strncmp(word, "¡°", 2))
  {
    sprintf(buf, "<font color=%s>%s</font>", LGREEN, word);
    includetext = 1;
  }
  else if (!strncmp(word, "==>", 3) || word[0] == ':' || word[0] == '>')
  {
    sprintf(buf, "<font color=%s>%s</font>", CRAY, word);
    includetext = 1;
  }
  else if (includetext)
  {
    sprintf(buf, "<font color=%s>%s</font>", GRAY, word);
    includetext = 0;
  }
  else if (!strcmp(word, "--\n"))
    sprintf(buf, "</blink></u></font><font color=%s>%s", GRAY, word);
  else
    sprintf(buf, "%s", word);
  ans = buf;
  return (char *) (ans);
}

void
showuser(uec)
  userec *uec;
{
  int i=0 , user_num;
  FILE *plans;
  char modestr[40], buf[600], *ptr;
  char *cnum[10]={"¤@","¤G","¤T","¥|","¤­","¤»","¤C","¤K","¤E","¤Q"};
  user_info *unow, *utail;

  printf("</center><table>");
  printf("<tr><td><font size=4 color=%s>%s</font><font size=4 color=%s>"
        " <a href=mailto:%s.bbs@%s>%s</a>(%s) "
        "¦@¤W¯¸ %d ¦¸¡Aµoªí¹L %d ½g¤å³¹</td></tr>\n",
  (uec->userlevel & PERM_LOGINOK) ? LRED : CRAY,
  (uec->userlevel & PERM_LOGINOK) ? "¡À" : "¡H", BLACK,
  uec->userid, MYHOSTNAME,
  uec->userid, uec->username, uec->numlogins, uec->numposts);
  ptr=uec->lasthost;
  if( strchr(ptr,'@') ){
      ptr=index(uec->lasthost,'@')+1;
  }
  printf("<tr><td>³Ìªñ(%s)±q[%s]¤W¯¸</td></tr>\n",
        Cdate(&(uec->lastlogin)), ptr);

  strcpy(modestr,"¤£¦b¯¸¤W");

  ushm_head = utmpshm->uinfo;
  ushm_tail = ushm_head + USHM_SIZE;

  unow = ushm_head;
  utail = ushm_tail;

  do
  {
    if (!strcmp(uec->userid, unow->userid))
    {
      if (!PERM_HIDE(unow))
         {
            strcpy(modestr, ModeTypeTable[unow->mode]);

          if(!unow->mode){
            if (unow->chatid[0] == 1)
              strcpy(modestr,"¦^À³©I¥s");
            if (unow->chatid[0] == 2)
              sprintf(modestr,"¤¤%sÁû¤ô²y");
            else if (unow->chatid[0] == 3)
              strcpy(modestr,"¤ô²y·Ç³Æ¤¤");
            else
              sprintf(modestr, "µo§b: %s", unow->destuid == 6 ?
                unow->chatid : IdleTypeTable[(0 <= unow->destuid &&
                unow->destuid < 6) ? unow->destuid : 0]);
              }

          if(unow->mode == LOGIN && uec->numlogins == 0)
            strcpy(modestr,"·s¥ë¦ñµù¥U¤¤");
         }
      break;
    }
  } while (++unow <= ushm_tail);

  sprintf(buf, BBSHOME"/home/%s/forward", uec->userid);

  printf("<tr><td><font color=%s>[¥Ø«e°ÊºA¡G%s]</font><font color=%s> %s</td></tr>\n",
    LRED, modestr, BLACK, dashf(buf) ? "«H¥ó¦Û°ÊÂà±H" :
      (chkmail(uec->userid) ? "¦³·s¶i«H¥óÁÙ¨S¬Ý":"©Ò¦³«H¥ó³£¬Ý¹L¤F"));

  sprintf(buf, BBSHOME"/home/%s/plans", uec->userid);
  if( !(i=access(buf, F_OK)) ){
      printf("<tr><td>%s ªº¦W¤ù:</td></tr></table><pre>\n", uec->userid);
      if( (plans=fopen(buf,"r")) == NULL ){
          perror("fopen:plans");
          exit(1);
      }
      while (/*i++ < MAXQUERYLINES && */ fgets(buf, 512, plans))
      {
        char word[1000];
        sprintf(word, "%s", html_word(buf));
        printf(word);
      }
      printf("</font></pre><table>");
      fclose(plans);
  }
  else
      printf("<tr><td>%s ¥Ø«e¨S¦³¦W¤ù</td></tr>\n", uec->userid);

  sprintf(buf, BBSHOME"/home/%s/note.ans", uec->userid);
  if( !(i=access(buf, F_OK)) ){
      printf("<tr><td><a href=/~%s/note>¬Ý %s ªº¯d¨¥ª©</a></td></tr>\n"
      , uec->userid, uec->userid);
     printf("</font>");
  }
  else
      printf("<tr><td>%s ¥Ø«e¨S¦³¯d¨¥ª©</td></tr>\n", uec->userid);

  sprintf(buf, BBSHOME"/home/%s/.profile", uec->userid);
  if (!(i=access(buf, F_OK)) )
  {
      printf("<tr><td><a href=/~%s/profile>¬Ý %s ªº°ò¥»¸ê®Æ</a></td></tr>\n"
       , uec->userid, uec->userid);
  }
  else
    printf("<tr><td>%s ¥Ø«e¨S¦³°ò¥»¸ê®Æ</td></tr>\n", uec->userid);

  printf("</table>");
}


html_note(char *userid)
{
  char buf[600];
  FILE *plans;

    sprintf(buf, "%sªº¯d¨¥¥»", userid);
    html_head(buf, 0, 1, 0);

  sprintf(buf, BBSHOME"/home/%s/note.ans", userid);

  printf("<a href=#write>¼g¯d¨¥µ¹%s</a></center><pre>\n", userid, userid);

  if( (plans=fopen(buf,"r")) == NULL ){
          printf("</pre>­ì¥»¨S¦³¨¥¥»<p>\n");/*
          perror("fopen:plans");*/
      }
  else
  {
      while (fgets(buf, 512, plans))
      {
        char word[1000];
        sprintf(word, "%s", html_word(buf));
        printf(word);
      }
     printf("</font></pre>");
     fclose(plans);
  }

  printf("<hr>");
  printf("<center><font size=6><b><a name=write>¼g¯d¨¥µ¹%s</b></font></a></center>", userid);
  printf("<form action=http://%s/~%s/cgi-bin/note.cgi method=post>\n",
          MYHOSTNAME, BBSUSER);
  printf("<input type=hidden name=name value=%s><p>\n", userid);
  printf("¥N  ¸¹: <input type=text name=myname size=12 maxlength=12><p>\n");
  printf("²Ä¤@¦æ: <input type=text name=line1 size=40 maxlength=80><p>\n");
  printf("²Ä¤G¦æ: <input type=text name=line2 size=40 maxlength=80><p>\n");
  printf("²Ä¤T¦æ: <input type=text name=line3 size=40 maxlength=80><p>\n");
  printf("<input type=submit name=option value=°e¥X>     ");
  printf("<input type=reset name=name value=²M°£>\n");
  printf("</form>");
}

int html_profile(char *userid) {
  FILE *pf;
  char buffer[300], fnb[200];
  char proname[PROFILE_NUM][PROFILE_SIZE];
  char prodata[PROFILE_NUM][PROFILE_SIZE];
  int i, j, size, fix, still;
  char *p;

  sprintf(buffer, "%sªº°ò¥»¸ê®Æ", userid);
  html_head(buffer, 1, 0, 0);

  printf("<center><font size=6>¨Ï¥ÎªÌ %s ªº°ò¥»¸ê®Æ</font><br>", userid);
  printf("[ <a href=/~%s/> %s ªºª¬ºA</a> ", userid, userid);
  printf(" | <a href=/~%s/note> %s ªº¯d¨¥¥» </a> ", userid, userid);
  printf(" ] <br><br>");


  sprintf(fnb, BBSHOME"/home/%s/.profile", userid);

  if (pf=fopen(fnb, "r"))
  {
    while (fgets(buffer, 256, pf))
    {
      p=(char *)buffer;
      i++;
      j=0;
      while(buffer[j] != '|') j++;
      strncpy(proname[i], buffer, j);
      j=j+2;
      if (strlen(buffer)>j+1)
      {
        p+=j;
        if (strstr(p, "\n")) p=strtok(p, "\n");
        strcpy(prodata[i], p);
      }
    }
    fclose(pf);
  }
  size=i;

  printf("<table border=1>\n");
/*nekobe*/
  printf("<tr><td rowspan=5>%s</td><td>%s</td><td>%s</td>", prodata[6],  proname[1], prodata[1]);
  for (i=2; i<=5; i++)
  {
        printf("<tr><td>%s</td><td>%s¡@</td></tr>\n", proname[i], prodata[i]);
  }
  for (i=7; i<=size; i++)
  {
  printf("<tr><td>%s</td><td colspan=2>%s¡@</td></tr>\n", proname[i], prodata[i]);
  }

/*  for (i=1; i<=size; i++)
  {
    printf("<tr><td>%s<td>%s</tr>\n", proname[i], prodata[i]);
  }
*/
  printf("</table>\n");
}


html_mainnote()
{
  char buf[600];
  FILE *plans;

  sprintf(buf, BBSHOME"/note.ans");

  printf("<p><a href=/write>¼g¯d¨¥</a></center><pre>\n");

      if( (plans=fopen(buf,"r")) == NULL ){
          printf("</font></pre>");
          perror("fopen:plans");
          exit(1);
      }
      while (fgets(buf, 512, plans))
      {
        char word[1000];
        sprintf(word, "%s", html_word(buf));
        printf(word);
      }
     printf("</font></pre>");
     fclose(plans);
}

brd_note(char *board)
{
  char buf[600];
  FILE *plans;

  printf("</center><p><pre>\n");

  sprintf(buf, BBSHOME"/boards/%s/notes", board);

      if( (plans=fopen(buf,"r")) == NULL ){
          return;
      }
      while (fgets(buf, 512, plans))
      {
        char word[1000];
        sprintf(word, "%s", html_word(buf));
        printf(word);
      }
     printf("</font></pre>");
     fclose(plans);
}


html_boardarticle(char *fpath)
{
  FILE *fp;
  char buf[512], fdir[512], fname[20], brdname[20], title[TTLEN];
  fileheader *fhdr;
  int no, i;

  sprintf(buf, "%s%s", BBSHOME, fpath);
  if (fp = fopen(buf, "r"))
  {
    sprintf(fdir, "%s%s", BBSHOME, fpath);
    strcpy(fname, strrchr(fpath, '/') + 1);
    strcpy(buf, fpath + 8);
    strcpy(brdname, strtok(buf, "/"));
    strcpy(strrchr(fdir, '/') + 1, ".DIR");
    no = getindex(fdir, fname, sizeof(fileheader));
    strcpy(title, getfhdr(fdir, no)->title);
    sprintf(buf, "¤å³¹: %s", title);
    html_head(buf, 0, 1, 0);
    html_articlebar(brdname, no);
    printf("</center><pre><font size=4>");
    while(fgets(buf, 512, fp))
    {
      char word[512];
      sprintf(word, "%s", html_word(buf));
      printf(word);
    }
    printf("</font></pre><center>");
    html_articlebar(brdname, no);
  }
  else
    html_error(fpath, "article");
}

html_manarticle(char *fpath)
{
  FILE *fp;
  char buf[512], fdir[512], fname[20], title[TTLEN];
  fileheader *fhdr;
  int no, i;

  sprintf(buf, "%s%s", BBSHOME, fpath);

  if (strstr(buf, "/S."))
  {
    if (buf[strlen(buf+1)]=='/')
    {
      buf[strlen(buf)-1]='\0';
      i = readlink(buf, fdir, 250);
      fdir[i]='/';
      fdir[i+1]='\0';
      html_man(fdir+10);
      return 0;
    }
  }

  sprintf(buf, "%s%s", BBSHOME, fpath);
  if (fp = fopen(buf, "r"))
  {
    sprintf(fdir, "%s%s", BBSHOME, fpath);
    strcpy(fname, strrchr(fpath, '/') + 1);
    strcpy(buf, fpath + 8);
    strcpy(strrchr(fdir, '/') + 1, ".DIR");
    no = getindex(fdir, fname, sizeof(fileheader));
    strcpy(title, getfhdr(fdir, no)->title);
    sprintf(buf, "¤å³¹: %s", title);
    html_head(buf, 0, 1, 0);
    printf("</center><pre><font size=4>");
    while(fgets(buf, 512, fp))
    {
      char word[512];
      sprintf(word, "%s", html_word(buf));
      printf(word);
    }
    printf("</font></pre><center>");
  }
  else
    html_error(fpath, "article");
}


html_userlist()
{
  int i;

  html_head("½u¤W¨Ï¥ÎªÌ¦Cªí", 1, 0, 0);

  printf("¤w¦³ %d ¤H¦¸¨Ó¬Ý¹L", counter("user"));


  printf("<TABLE border=1>");
  print_head();

  for (i = user_num = 0; i < USHM_SIZE; i++)
  {
    uentp = &(utmpshm->uinfo[i]);
    if (uentp->userid[0]
        && !PERM_HIDE(uentp)
        && !uentp->invisible)
    {
      dump_record(i, uentp);
      print_record();
      user_num++;
    }
  }
  printf("<TD COLSPAN=6 bgcolor=%s>"
        "<CENTER>¡i%s¡j¥Ø«e¯¸¤W¦³ %d ¤H"
        "</CENTER></FONT><TR></TABLE>\n", LYELLOW, BOARDNAME, user_num );
}

html_tail()
{
  printf("<p><center>[ <A href=\"telnet://%s\">",MYHOSTNAME);
  printf("µn¤J BBS ¯¸ |</a>\n");
  /*
  printf(" <A href=http://mrtg.civil.ncku.edu.tw/bbs/bbs.civil.ncku.edu.tw.html>");
  printf("¥»¯¸½u¤W¨Ï¥ÎªÌ²Î­p¦Cªí |</a>\n");
  */
  printf(" <A href=mailto:SYSOP.bbs@%s>",MYHOSTNAME);
  printf("¯d¤U±zÄ_¶Qªº·N¨£µ¹ SYSOP</a> ]<HR>");
  printf("<TABLE BORDER=0><TR><TD>");
  printf("<address>PRO-HTTPD author:"
          "%s &lt;<a href=mailto:%s>%s</a>&gt;</address>",
           Gene_NAME, Gene_EMAIL, Gene_EMAIL);
  printf("<address>Images author:"
           "%s &lt;<a href=mailto:%s>%s</a>&gt;</address>",
                      Nekobe_NAME, Nekobe_EMAIL, Nekobe_EMAIL);
  printf("<address>Origion author:"
        "%s &lt;<a href=mailto:%s>%s</a>&gt;</address></TD>",
        Leeym_NAME, Leeym_EMAIL, Leeym_EMAIL);
  /*
  printf("<TD><A align=center HREF=\"http://freebsd.csie.nctu.edu.tw\">"
      "<img src=http://newage.stat.tku.edu.tw/pic/powerlogo.gif "
      "alt=\"Powered By FreeBSD\" border=0></a></TD></TR></TABLE></CENTER></BODY></HTML>\n");
  */
  printf("<TD><A ALIGN=CENTER HREF=\"http://newage.stat.tku.edu.tw:8888\">"
      "<IMG SRC=http://newage.stat.tku.edu.tw/~tku/pic/proally.jpg "
      "ALT=\"PRO BBS ALLY!\" BORDER=0></A></TD></TR></TABLE></CENTER></BODY></HTML>\n");
}

html_query(char *userid)
{
  char buf[80];
  FILE *pwd;

  if((pwd=fopen(BBSHOME"/.PASSWDS","r")) == NULL){
     printf("µLªk¶}±Ò±K½XÀÉ<p>\n");
     return;
  }

  resolve_utmp();

  while(fread(&uec, sizeof(userec), 1, pwd) > 0 )
  {
      if(ci_strcmp(uec.userid, userid))
      {
        continue;
      }
      else
      {
        sprintf(buf, "¬d¸ß¨Ï¥ÎªÌ %s", userid);
        html_head(buf, 1, 0, 0);
        /*
        printf("¨Ï¥ÎªÌ %s ¦³ "
         "<img src=http://newage.stat.tku.edu.tw/cgi-bin/"
         "Count.cgi??dd=C|df=user-%s.dat></a> ¤H¦¸<p>",
         userid, userid);
        */
        sprintf(buf, "user-%s", userid);
        printf("¨Ï¥ÎªÌ %s ¦³ %d ¤H¦¸¨Ó¬d¹L", userid, counter(buf));

        showuser(&uec);
      }
      return;
  }
  sprintf(buf, "/~%s/", userid);
  html_error(buf, "user");
}

html_man(char *man)
{
  fileheader fhdr;
  char fdir[1024], fname[1024];
  FILE *fp;
  int i = 0;

  sprintf(fdir, "ºëµØ°Ï: %s", man);
  html_head(fdir, 0, 0, 0);
  /*
        printf("¤w¦³ "
         "<img src=http://newage.stat.tku.edu.tw/cgi-bin/"
         "Count.cgi??dd=C|df=man.dat></a> ¤H¦¸¬ÝºëµØ°Ï<p>");
  */
  printf("¤w¦³ %d ¤H¦¸¨Ó¬Ý¹L", counter("man"));

  sprintf(fdir,"%s/%s/.DIR", BBSHOME, man);
  if (fp = fopen(fdir, "r")) {
    printf("<center><table>\n");
    printf("<td align=left bgcolor=%s>Ãþ«¬</td>", LYELLOW);
    printf("<td align=left bgcolor=%s>½s¸¹</td>", LYELLOW);
    printf("<td align=left bgcolor=%s>¼ÐÃD</td>", LYELLOW);
    printf("<td align=left bgcolor=%s>½s¿ï</td>", LYELLOW);
    printf("<td align=left bgcolor=%s>¤é´Á</td></tr>", LYELLOW);
    while(fread(&fhdr, sizeof(fhdr), 1, fp) && ++i) {
      sprintf(fname,"%s/%s/%s", BBSHOME, man, fhdr.filename);
      printf("<tr>");
      printf("<td><img src=http://newage.stat.tku.edu.tw/icons/%s></td>",
        dashd(fname) ? "folder.gif" : dashf(fname) ? "text.gif" :
        dashl(fname) ? "link.gif" : "unknown.gif");
      printf("<td>%d</td>", i);
      printf("<td><a href=/%s%s%s>", man, fhdr.filename, dashd(fname) ? "/" : "");
      printf("%s</a></td>", fhdr.title);
      printf("<td>%s</td>", fhdr.owner);
      printf("<td>%s</td>", fhdr.date);
      printf("</tr>\n");
    }
    printf("</center></table>\n");
  }
}

pro_file()
{
  FILE *fp;
  char buf[512];

  sprintf(buf, "%s/www/profile.html", BBSHOME);
  fp=fopen(buf, "r");

  while (fgets(buf, 512, fp))
  {
    printf(buf);
  }
  fclose(fp);
}

mainpage()
{
  FILE *fp;
  char buf[512];

  sprintf(buf, "%s/www/mainpage.html", BBSHOME);
  fp=fopen(buf, "r");

  while (fgets(buf, 512, fp))
  {
    printf(buf);
  }
  fclose(fp);
}

/* ----------------------------------------------------- */
/* Main program                                          */
/* ----------------------------------------------------- */

main(argc, argv, envp)
  int argc;
  char *argv[];
  char *envp[];
{
  char method[10], buf[512];
  int done=0;

  resolve_utmp();

  if (fscanf(stdin, "%s %s %s", method, buf, protocol))
  {
    accesslog(method, buf);
    if (!strcmp(buf, "/user/"))
      html_userlist();
    else if (!strncmp(buf, "/man/", 5))
    {
     if (strstr(buf, "/M.") || strstr(buf, "/S.") || strstr(buf, "/G."))
        html_manarticle(buf);
     else
        html_man(buf + 1);
    }
    else if (!strncmp(buf, "/boards/", 7))
    {
      if (!strcmp(buf, "/boards/"))
        html_boardlist();
      else if (strstr(buf, "/M."))
        html_boardarticle(buf);
      else
        html_board(buf+8);
    }
    else if (!strncmp(buf, "/~", 2))
    {
      if (strstr(buf, "note"))
      {
        html_note(strtok(buf +2, "/\n"));
        done=1;
      }
      if (strstr(buf, "profile"))
      {
        html_profile(strtok(buf +2, "/\n"));
        done=1;
      }
      if ((buf[2])&& !(done))
      {
        html_query(strtok(buf + 2, "/\n"));
        done=1;
      }
      if (!(done))
        printf("<head><META HTTP-EQUIV=\"Refresh\" CONTENT=0;URL=/user/></HEAD>");
    }
    else if (!strcmp(buf, "/"))
    {
      html_head("¥D¿ï³æ", 0, 0, 1);
      printf("¤w¦³ %d ¤H¦¸¨Ó¬Ý¹L", counter("main"));
      printf("<p>[ <a href=%s>¶i¯¸°Êµe</a> | ",
             BBSANIMATION);
      printf("<a href=http://%s/~%s/index.html>¥»¯¸¤¶²Ð</a> | ",
             MYHOSTNAME, BBSUSER);
      printf("<a href=/note>¬Ý¯d¨¥ªO</a> | <a href=/profile>¬Ý±Æ¦æº]</a> ]");

      /* for(i=0; argv[i]!= (char *) 0; i++)
        printf("%s\n", argv[i]);*/
      mainpage();
    }
    else if (!strcmp(buf, "/note"))
    {
      html_head("¯d¨¥ª©", 0, 1, 0);
      printf("¤w¦³ %d ¤H¦¸¨Ó¬Ý¹L", counter("note"));
      html_mainnote();
    }
    else
    if (!strcmp(buf, "/profile"))
    {
      html_head("¥»¯¸¯¸¤Í", 0, 0, 1);
      printf("¤w¦³ %d ¤H¦¸¨Ó¬Ý¹L", counter("profile"));
      pro_file();
    }
    else
      html_error(buf, "unknown");
  }
  html_tail();
}
