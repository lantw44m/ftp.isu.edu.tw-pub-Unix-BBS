#include "bbs.h"
#include "bbs_util.h"

#define MAX_WORDS 2048

int ansi_on, ansi_back, ansi_blink, ansi_dark;

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
   case 0: case 1: case 5:
   case 30: case 31: case 32: case 33: case 34: case 35: case 36: case 37:
   case 40: case 41: case 42: case 43: case 44: case 45: case 46: case 47:
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

   if( ansi_on && (attr==0 || (attr>=30 && attr<=37)) )
   {
      strcpy( *tag, "</FONT>" ); *tag+=7;
      ansi_on=0;
   }
   
   if(ansi_back && (attr==0 || (attr>=40 && attr<=47)) )
   {
      strcpy( *tag, "</SPAN>" ); *tag+=7;
      ansi_back=0;
   }
   
   switch( attr )
   {
   case 0:  ansi_blink=0; ansi_dark=1; return;
   case 1:  ansi_dark=0; return;

   case 30: strcpy( *tag,
            (ansi_dark==0) ? "<FONT color=#808080>" : "<FONT color=#000000>" );
            *tag+=20;break;
   case 31: strcpy( *tag, 
            (ansi_dark==0) ? "<FONT color=#ff0000>" : "<FONT color=#800000>");
            *tag+=20;break;
   case 32: strcpy( *tag, 
            (ansi_dark==0) ? "<FONT color=#00ff00>" : "<FONT color=#008000>");
            *tag+=20;break;
   case 33: strcpy( *tag, 
            (ansi_dark==0) ? "<FONT color=#ffff00>" : "<FONT color=#808000>");
            *tag+=20;break;
   case 34: strcpy( *tag, 
            (ansi_dark==0) ? "<FONT color=#0000ff>" : "<FONT color=#000080>");
            *tag+=20;break;
   case 35: strcpy( *tag, 
            (ansi_dark==0) ? "<FONT color=#ff00ff>" : "<FONT color=#800080>");
            *tag+=20;break;
   case 36: strcpy( *tag, 
            (ansi_dark==0) ? "<FONT color=#00ffff>" : "<FONT color=#008080>");
            *tag+=20;break;
   case 37: strcpy( *tag, 
            (ansi_dark==0) ? "<FONT color=#ffffff>" : "<FONT color=#ffffff>");
            *tag+=20;break;
   
   case 40: strcpy( *tag, "<SPAN style='background-color: #000000'>");
            *tag+=40; break;
   case 41: strcpy( *tag, "<SPAN style='background-color: #800000'>");
             *tag+=40; break;
   case 42: strcpy( *tag, "<SPAN style='background-color: #008000'>");
            *tag+=40; break;
   case 43: strcpy( *tag, "<SPAN style='background-color: #808000'>");
            *tag+=40; break;
   case 44: strcpy( *tag, "<SPAN style='background-color: #000080'>");
            *tag+=40; break;
   case 45: strcpy( *tag, "<SPAN style='background-color: #800080'>");
            *tag+=40; break;
   case 46: strcpy( *tag, "<SPAN style='background-color: #008080'>");
            *tag+=40; break;
   case 47: strcpy( *tag, "<SPAN style='background-color: #ffffff'>");
            *tag+=40; break;
                  
   default:
      if( ansi_blink ) { strcpy( *tag, "<blink>" ); *tag+=7; }
      return;
   }

   if( ansi_blink ) { strcpy( *tag, "<blink>" ); *tag+=7; }
   
   if(attr>=30 && attr <=37)
      ansi_on=1;
   else if(attr>=40 && attr<=47)
      ansi_back=1;
             
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

   if( cap==cattr )
   {
      if( **ap=='m' || **ap==';' )
        strcpy(cattr,"0");
      else
        return 0;
   }
   else
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
   int  attr=0, num, nextansi;

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
	    if( cmd=='m' || cmd==';') tagcolor( &tp, attr );
//	    tagcolor( &tp, attr );
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

int bbs2html( char *bbsfile)
{
   char l1[MAX_WORDS];
   FILE *fi;
//   struct stat st;
//   time_t htmltime;

   if( (fi=fopen( bbsfile,"rt")) == NULL )
   {
      printf( "bbs2htmlcode: No input file: %s\n", bbsfile );
      return 1;
   }
//   lstat( bbsfile, &st );

   ansi_on = 0;
   ansi_back = 0;
   ansi_blink = 0;
   ansi_dark = 0;

   while( fgets( l1, MAX_WORDS, fi) != NULL )
   {
      cut_cr( l1 );
      ansi2tag( l1 );
      add_href( l1 );
//      if( !strcmp( l1, "--" ) )
//	 fprintf( fo, "<hr size=1>" );
//      else
	 printf( "%s\n", l1 );
   }
   /* ========== end html ============= */
   if( ansi_blink ) printf(  "</blink>" );
   if( ansi_on ) printf(  "</FONT>" );
   if( ansi_back ) printf("</SPAN>" );
   
//   fprintf( fo, "</pre>%s", WWW_END );

   fclose( fi );
   return 0;
}

int main( int argc, char *argv[])
{
   if( argc < 2 )
   {
      printf( "usage: bbs2htmlcode [input bbsfile]\n" );
      exit(1);
   }

   bbs2html( argv[1]);
}

