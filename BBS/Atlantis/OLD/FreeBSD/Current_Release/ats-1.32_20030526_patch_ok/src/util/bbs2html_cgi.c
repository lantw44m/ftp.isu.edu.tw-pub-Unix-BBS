#define BBSHOME "/home/bbsrs"
#define BBSMAN BBSHOME "/man"
/*

             bbs2html v1.2        by wcf@CCCA.NCTU.edu.tw

From: wcf@CCCA.NCTU.edu.tw (Wei Chang feng)
Subject: BBS ºëµØ°Ï -> WWW html
Date: 22 Nov 1996 09:11:44 GMT

    §Ú¼g¤F¤@­Ó¥i¥H§â BBS ºëµØ°Ïªº¤º®e (~bbs/0Announce) Âà¦¨ html ªºµ{¦¡,
  ·|§â BBS ºëµØ°Ïªº¤å³¹§@¥H¤Uªº±Æª©:

    1. ¥[ background, title, icons
    2. §â ANSI color code Âà¦¨ html <font color=???>
    3. §â ANSI °{Ã{±±¨î½XÂà¦¨ <blink>..
    4. §â¤å¤¤©Ò¦³§äªº¨ìªº URL Âà¦¨ hyper link..
    5. ¥u§ó·s BBS ºëµØ°Ï¸Ì§ó·s¹LªºÀÉ®×.

    Âà´«¥X¨Óªº html ·|¦³®É¶¡ªº serial number, ¤ñ¹ï¦¹ serial number »P
  ­ìºëµØ°ÏªºÀÉ®×´N¥iª¾¹D¬O§_§ó·s¹L.

    SOURCE ©ñ¦b ftp://hsnu.csie.nctu.edu.tw/BBS2HTML.C
    §Ú¦b Linux ¤Î FreeBSD ¤W´ú¹L ok.
    §Ú¤]¦b Phoenix ¤Î Maple ¤W¸Õ¹L. SOB «h¨S¸Õ¹L..
    (sob-version patched by woju.bbs@sob.org)

    ¨â­Ó¹êÅç«~¦b http://hsnu.csie.nctu.edu.tw/hsnubbs/
                 http://www.ccca.nctu.edu.tw/~wcf/

    SOURCE code ¸Ì¦³¤@¨ÇÅÜ¼Æ¥i¥H¤è«K¤j®a­×§ï:

    index:
        INDEX_TITLE      title «e­±ªº mark
        INDEX_BACKGROUND ­I´º
        INDEX_HEAD_ICON  head ¤Wªº icon
        INDEX_COLOR      page ªºÃC¦â
        INDEX_END        WWW ©³³¡¥[ªº link..

    article:
        ART_TITLE      title «e­±ªº mark
        ART_BACKGROUND ­I´º
        ART_HEAD_ICON  head ¤Wªº icon
        ART_COLOR      page ªºÃC¦â
        ART_END        WWW ©³³¡¥[ªº link..

--
Wei Chang-feng (ÃQªø­·) , wcf@CCCA.NCTU.edu.tw
Consultant of Campus Computer Communication Association
Rm 101, Computer Center, National Chiao Tung University, Hsinchu, Taiwan
TEL : (035)712121 Ext 52833

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

#define MAX_WORDS 1000

int ansi_on, ansi_blink;

char *INDEX_TITLE = "[sob-man main]";
char *INDEX_BACKGROUND;
char *INDEX_HEAD_ICON;
char INDEX_COLOR[]="bgcolor=#ddddaa text=#000088 vlink=#8040ff link=#8800ff";
char INDEX_END[]="</BODY></HTML>";

char *ART_TITLE = "[sob-man]";
char *ART_BACKGROUND;
char *ART_HEAD_ICON;
char ART_COLOR[]="bgcolor=#000000 text=#ddffdd vlink=#ffff00 link=#ffff11";
char ART_END[]="</BODY></HTML>";

void cut_cr( char *l )
{
   char *i;

   for( i=l; *i!=0; i++ );
   if( *(i-1) != 0 ) *(i-1)=0;
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
      if( !strcmp(url,"http://") || !strcmp(url,"HTTP://") ) found=1;

      strncpy( url, ap, 6 ); url[6]=0;
      if( !strcmp(url,"ftp://") || !strcmp(url,"FTP://") ) found=1;

      strncpy( url, ap, 7 ); url[7]=0;
      if( !strcmp(url,"file://") || !strcmp(url,"FILE://") ) found=1;

      strncpy( url, ap, 9 ); url[9]=0;
      if( !strcmp(url,"gopher://") || !strcmp(url,"GOPHER://") ) found=1;

      strncpy( url, ap, 9 ); url[9]=0;
      if( !strcmp(url,"telnet://") || !strcmp(url,"TELNET://") ) found=1;

      strncpy( url, ap, 7 ); url[7]=0;
      if( !strcmp(url,"mailto:") || !strcmp(url,"MAILTO:") ) found=1;

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


void tag8859_1( char **tag, char c )
{
   switch( c )
   {
   case '\"': strcpy( *tag, "&quot" );   *tag+=5;break;
   case '<':  strcpy( *tag, "&lt" );     *tag+=3;break;
   case '>':  strcpy( *tag, "&gt" );     *tag+=3;break;
   case '&':  strcpy( *tag, "&amp" );    *tag+=4;break;

   default:
      **tag=c; (*tag)++;
   }
}


void tagcolor( char **tag, int attr )
{
   switch( attr ) /* === filter no-used ansi control === */
   {
   case 0: case 5:
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

   case 30: strcpy( *tag, "<FONT color=gray>" );   *tag+=17;break;
/*
woju
*/
   case 31: strcpy( *tag, "<FONT color=red>" );    *tag+=16;break;
   case 32: strcpy( *tag, "<FONT color=green>" );  *tag+=18;break;
   case 33: strcpy( *tag, "<FONT color=yellow>" ); *tag+=19;break;
   case 34: strcpy( *tag, "<FONT color=blue>" );   *tag+=17;break;
   case 35: strcpy( *tag, "<FONT color=fuchsia>" );*tag+=20;break;
   case 36: strcpy( *tag, "<FONT color=aqua>" );   *tag+=17;break;
/*
   case 31: strcpy( *tag, "<FONT color=#FF0000>" ); *tag+=20;break;
   case 32: strcpy( *tag, "<FONT color=#00FF00>" ); *tag+=20;break;
   case 33: strcpy( *tag, "<FONT color=#FFFF00>" ); *tag+=20;break;
   case 34: strcpy( *tag, "<FONT color=#0000FF>" ); *tag+=20;break;
   case 35: strcpy( *tag, "<FONT color=#FF00FF>" ); *tag+=20;break;
   case 36: strcpy( *tag, "<FONT color=#00FFFF>" ); *tag+=20;break;
*/
   case 37: strcpy( *tag, "<FONT color=white>" );  *tag+=18;break;

   default:
      if( ansi_blink ) { strcpy( *tag, "<blink>" ); *tag+=7; }
      return;
   }

   if( ansi_blink ) { strcpy( *tag, "<blink>" ); *tag+=7; }
   ansi_on=1;
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
      else /* Not ANSI cntrol */
      {
         tag8859_1( &tp, *ap ); ap++;
      }
   }
   *tp = 0;
   strcpy( l, tag );
}


int ann2html( char *bbsfile, char *tit )
{
   char l1[MAX_WORDS], title[MAX_WORDS];
   FILE *fi, *fo;
   struct stat st;
   time_t htmltime;

   sprintf(l1, BBSMAN "/%s", bbsfile);
   if(isdir(l1) || (fi=fopen(l1, "r")) == NULL )
   {
      printf( "ann2html: No input file: %s\n", l1);
      return 1;
   }

   fo = stdout;
   ansi_on = 0;
   ansi_blink = 0;
   strcpy( title, tit ); ansi2tag( title );
/*
woju
*/
   ART_TITLE = title;

   /* ========== html headers ============= */
   fprintf( fo,
"Content-type: text/html\n\n"
"<!-- BBS2HTML[%lu] Areicle by wcf@CCCA.NCTU.edu.tw  -->\n"
"<HTML>"
"<HEAD>"
"<TITLE>%s</TITLE>"
"</HEAD>"
"\n"
"<BODY background=\"%s\" %s"
">\n"
"<center>"
"<H1>%s</H1>\n<table><tr><td><pre><hr>"
           , st.st_mtime, title, ART_BACKGROUND,
             ART_COLOR, title );


   /* ========== text body ============= */
   while( fgets( l1, MAX_WORDS, fi) != NULL )
   {
      cut_cr( l1 );
      ansi2tag( l1 );
      add_href( l1 );
      if( !strcmp( l1, "--" ) )
         fprintf( fo, "<hr>" );
      else
         fprintf( fo, "%s\n", l1 );
   }

   /* ========== end html ============= */
   if( ansi_blink ) fprintf( fo, "</blink>" );
   if( ansi_on ) fprintf( fo, "</FONT>" );
   fprintf( fo, "</pre></td></tr></table></center>%s", ART_END );

   fclose( fi );
   return 0;
}

int isdir( char *fname )
{
   struct stat st;

   lstat( fname, &st );
   return( S_ISDIR( st.st_mode ) );
}

/*
woju, sob-version man
   ----------------------------------------------------- */
/* DIR of board struct : 128 bytes                       */
/* ----------------------------------------------------- */

#define FNLEN    33             /* Length of filename  */
#define IDLEN    12             /* Length of bid/uid */
#define TTLEN    72             /* Length of title */

struct fileheader
{
  char filename[FNLEN];         /* M.9876543210.A */
  char savemode;                /* file save mode */
  char owner[IDLEN + 2];        /* uid[.] */
  char date[6];                 /* [02/02] or space(5) */
  char title[TTLEN + 1];
  unsigned char filemode;              /* must be last field @ boards.c */
};
typedef struct fileheader fileheader;

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

char*
tencode(char* s)
{
   static char ans[201];
   int i;

   for (i = 0; i < 100 && s[i]; i++)
      sprintf(ans + i * 2, "%02x", (unsigned char)s[i]);
   return ans;
}


dashf(fname)
  char *fname;
{
  struct stat st;

  return (stat(fname, &st) == 0 && S_ISREG(st.st_mode));
}


idxconvert(char* fpath, FILE *fi)
{
   char l1[MAX_WORDS], l2[MAX_WORDS], oldname[MAX_WORDS], *p, *p2;
   char title[MAX_WORDS];
   fileheader fhdr;
   int len;
   char date[20];
   time_t dtime;
   int n;
   FILE *fo = stdout;


   /* ========== text body ============= */
   fprintf(fo, "<center><table CELLSPACING=6 valign=\"top\" cellpading=\"10\">\n");
   n = 0;
   while (fread(&fhdr, sizeof(fhdr), 1, fi) == 1) {
      ++n;
      sprintf(l2, BBSMAN "/%s%s", fpath, fhdr.filename);
      fhdr.title[50] = 0;
      dtime = atoi(fhdr.filename + 2);
      a_timestamp(date, &dtime);

      sprintf(title, "%s</td><td>%s</td><td>%s</td>",
         fhdr.title, fhdr.owner, date);

      if (*fhdr.filename == 'H') {              /* gopher link */
         char gserver[100] = "localhost", gport[100] = "70",
              gpath[100] = "", *s;

         s = strtok(fhdr.title + 3, ": ");
         strcpy(gserver, s);
         if (s = strtok(0, " \n\r:")) {
            strcpy(gpath, *s == '/' ? s + 1 : s);
            if ((s = strtok(0, " :/\n\r")) && atoi(s))
               strcpy(gport, s);
         }
         fprintf( fo,
"<tr><td>%d</td><td><a href=\"gopher://%s:%s/11/%s\">%s</a></tr>\n"
                    ,n ,gserver, gport, gpath, title);
      }
      else {
         fprintf( fo,
"<tr><td>%d</td><td><a href=\"/cgi-bin/bbs2html_cgi?%s%s%s+%s\">%s</a></tr>\n"
           ,n ,fpath, fhdr.filename,
           dashf(l2) ? "" : "/", tencode(fhdr.title), title);
      }
   }
   fprintf(fo, "</table></center>\n");
}


int idx2html( char *dirpath, char *tit )
{
   char l1[MAX_WORDS], title[MAX_WORDS];
   FILE *fi, *fo;
   struct stat st;
   time_t htmltime;

   sprintf(l1, BBSMAN "/%s.DIR", dirpath);

   if( (fi=fopen( l1,"r")) == NULL )
   {
      printf( "idx2html: no input file: %s\n", l1);
      return 1;
   }

   ansi_on = 0;
   ansi_blink = 0;
   strcpy( title, tit ); ansi2tag( title );
/*
woju
*/
   INDEX_TITLE = title;
   fo = stdout;

   /* ========== html headers ============= */
   fprintf( fo,
"Content-type: text/html\n\n"
"<!-- BBS2HTML[%lu] Index by wcf@CCCA.NCTU.edu.tw  -->\n"
"<HTML>\n"
"<HEAD>\n"
"<TITLE>%s</TITLE>\n"
"</HEAD>\n"
"\n"
"<BODY background=\"%s\" %s"
">\n\n"
"<center>\n"
"<H1>%s</H1>\n</center>\n<hr>"
           , st.st_mtime, title, INDEX_BACKGROUND,
             INDEX_COLOR, title );

   /* ========== text body ============= */
   idxconvert(dirpath, fi);

   /* ========== end html ============= */
   if( ansi_blink ) fprintf( fo, "</blink>" );
   if( ansi_on ) fprintf( fo, "</FONT>" );
   fprintf( fo, "%s", INDEX_END );
   fclose(fi);
   return 0;
}


int bbs2html( char *spath, char *tit )
{
   char l1[MAX_WORDS], title[MAX_WORDS], *p, *p2;
   fileheader fhdr;
   FILE *fi;
   int len;

   /* ========== text body ============= */
   strcpy(title, tit);
   title[50] = 0;
   if (!*spath || spath[strlen(spath) - 1] == '/')
      idx2html( spath, title );
   else
      ann2html(spath, title);
   return 0;
}

char*
tdecode(char* s)
{
   static char ans[101];
   int i, n;

   for (i = 0; i < 100 && s[i * 2]; i++) {
      sscanf(s + i * 2, "%2x", &n);
      ans[i] = n;
   }
   return ans;
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


int main( int argc, char *argv[])
{
   char htmldir[1024] = "";


/*
      printf( "usage: bbs2html_cgi [path [title]]\n" );
*/
   if (argc > 1 && strcmp(argv[1], "/")) {
      strcpy(htmldir, argv[1]);
      if (invalid_pname(htmldir))
         return 1;
   }
   bbs2html(htmldir, argc > 2 ?
      tdecode(argv[2]) : "FreeBSD ­Ñ¼Ö³¡ BBS ºëµØ°Ï");
}
