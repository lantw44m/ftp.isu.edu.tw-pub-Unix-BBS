/*-------------------------------------------------------*/
/* stuff.c      ( NTHU CS MapleBBS Ver 2.36 )            */
/*-------------------------------------------------------*/
/* target : utility routines                             */
/* create : 95/03/29                                     */
/* update : 95/12/15                                     */
/*-------------------------------------------------------*/

#include "bbs.h"
#include <varargs.h>

void
itoa(i,a)
  int i;
  char *a;
{
  int j,l;

  l=strlen(a)-1;
  a[l]='\0';

  for(j=l-1 ; j>=0 ; j--)
  {
    a[j]=i%10 + 48;
    i=i/10;
  }
}


void
setuserfile(buf, fname)
  char *buf, *fname;
{
  sprintf(buf, "home/%s/%s", cuser.userid, fname);
}


void
setbdir(buf, boardname)
  char *buf, *boardname;
{
  sprintf(buf, "boards/%s/%s", boardname,
    currmode & MODE_DIGEST ? fn_digest : 
    currmode & MODE_DELETED ? fn_deleted : ".DIR");
}


int
invalid_fname(str)
  char *str;
{
  char ch;

  if (strspn(str, ".") == strlen(str))
     return 1;

  while (ch = *str++)
  {
    if (not_alnum(ch) && !strchr("@[]-._", ch))
      return 1;
  }
  return 0;
}


int
invalid_pname(str)
  char *str;
{
  char *p1, *p2, *p3;

  p1 = str;
  while (*p1) 
  {
    if (!(p2 = strchr(p1, '/')))
      p2 = str + strlen(str);
    
    if (p1 + 1 > p2 || p1 + strspn(p1, ".") == p2)
      return 1;
    
    for (p3 = p1; p3 < p2; p3++)
      if (not_alnum(*p3) && !strchr("@[]-._", *p3))
        return 1;
    
    p1 = p2 + (*p2 ? 1 : 0);
  }
  return 0;
}



int
valid_ident(ident)
  char *ident;
{
  static char *invalid[] = {"unknown@", "root@", "gopher@", "bbs@",
  "@bbs", "guest@", "@ppp", "@slip", NULL};
  char buf[128];
  int i;

  str_lower(buf, ident);
  for (i = 0; invalid[i]; i++)
    if (strstr(buf, invalid[i]))
      return 0;
  return 1;
}


/*
woju
*/
int
userid_is_BM(userid, list)
  char *userid, *list;                  /* ªO¥D¡GBM list */
{
  register int ch, len;

  ch = list[0];
  if ((ch > ' ') && (ch < 128))
  {
    len = strlen(userid);
    do
    {
      if (!ci_strncmp(list, userid, len))
      {
        ch = list[len];
        if ((ch == 0) || (ch == '/') || (ch == ']'))
          return 1;
      }
      while (ch = *list++)
      {
        if (ch == '/')
          break;
      }
    } while (ch);
  }
  return 0;
}


int
is_BM(list)
  char *list;
{
 return userid_is_BM(cuser.userid, list);
}


/* ----------------------------------------------------- */
/* ÀÉ®×ÀË¬d¨ç¼Æ¡GÀÉ®×¡B¥Ø¿ý¡BÄÝ©ó                        */
/* ----------------------------------------------------- */


off_t
dashs(fname)
  char *fname;
{
  struct stat st;

  if (!stat(fname, &st))
        return (st.st_size);
  else
        return -1;
}


long
dasht(fname)
  char *fname;
{
  struct stat st;

  if (!stat(fname, &st))
        return (st.st_mtime);
  else
        return -1;
}


int
dashl(fname)
  char *fname;
{
  struct stat st;

  return (lstat(fname, &st) == 0 && S_ISLNK(st.st_mode));
}


dashf(fname)
  char *fname;
{
  struct stat st;

  return (stat(fname, &st) == 0 && S_ISREG(st.st_mode));
}


int
dashd(fname)
  char *fname;
{
  struct stat st;

  return (stat(fname, &st) == 0 && S_ISDIR(st.st_mode));
}


int
belong(filelist, key)
  char *filelist;
  char *key;
{
  FILE *fp;
  int rc = 0;

  if (fp = fopen(filelist, "r"))
  {
    char buf[80], *ptr;

    while (fgets(buf, 80, fp))
    {
      if ((ptr = strtok(buf, " \t\n\r")) && !strcasecmp(ptr, key))
      {
        rc = 1;
        break;
      }
    }
    fclose(fp);
  }
  return rc;
}


int
belong_spam(filelist, key)
  char *filelist;
  char *key;
{
  FILE *fp;
  int rc = 0;

  if (fp = fopen(filelist, "r"))
  {
    char buf[STRLEN], *ptr;

    while (fgets(buf, STRLEN, fp))
    {
      if(buf[0] == '#') continue;
      if ((ptr = strtok(buf, " \t\n\r")) && strstr(key, ptr))
      {
        rc = 1;
        break;
      }
    }
    fclose(fp);
  }
  return rc;
}


char *
Cdate(clock)
  time_t *clock;
{
  static char foo[22];
  struct tm *mytm = localtime(clock);

  strftime(foo, 22, "%D %T %a", mytm);
  return (foo);
}


char *
Cdatelite(clock)
  time_t *clock;
{
  static char foo[18];
  struct tm *mytm = localtime(clock);

  strftime(foo, 18, "%D %T", mytm);
  return (foo);
}

void
pressanykey(va_alist)
  va_dcl
{
  va_list ap;
  char msg[250], *fmt;
  int ch;
//  extern int dumb_term;
//  extern screenline* big_picture;
//  screenline* line;      

  msg[0]=0;
  va_start(ap);
  fmt = va_arg(ap, char *);
  if(fmt) vsprintf(msg, fmt, ap);
  va_end(ap);
  if (msg[0])
  {
    move(b_lines, 0); clrtoeol();
    prints(COLOR1"[1m ¡¹ [37m%-75s[m", msg);
  }
  else
    outmsg(COLOR1"[1m                        ¡¹ ½Ð«ö [37m(Space/Return)"
    COLOR1" Ä~Äò ¡¹                         [m");
                              
  do
  {
      ch = dogetch();
//    ch = igetkey();
//    if (ch == KEY_ESC && KEY_ESC_arg == 'c')
//      capture_screen();
  } while ((ch != ' ') && (ch != '\r') && (ch != '\n') );
  
  move(b_lines, 0);
  clrtoeol();
  refresh();
}


void
bell()
{
  char sound[3], *ptr;

  ptr = sound;
  memset(ptr, Ctrl('G'), sizeof(sound));
  if(HAS_HABIT(HABIT_BELL))
    write(1, ptr, sizeof(sound));
}


int
search_num(ch, max)
{
  int clen = 1;
  int x, y;
  extern unsigned char scr_cols;
  char genbuf[10];

  outmsg("[7m ¸õ¦Ü²Ä´X¶µ¡G[0m");
  outc(ch);
  genbuf[0] = ch;
  getyx(&y, &x);
  x--;
  while ((ch = igetch()) != '\r')
  {
    if (ch == 'q' || ch == 'e')
      return -1;
    if (ch == '\n')
      break;
    if (ch == '\177' || ch == Ctrl('H'))
    {
      if (clen == 0)
      {
        bell();
        continue;
      }
      clen--;
      move(y, x + clen);
      outc(' ');
      move(y, x + clen);
      continue;
    }
    if (!isdigit(ch))
    {
      bell();
      continue;
    }
    if (x + clen >= scr_cols || clen >= 6)
    {
      bell();
      continue;
    }
    genbuf[clen++] = ch;
    outc(ch);
  }
  genbuf[clen] = '\0';
  move(b_lines, 0);
  clrtoeol();
  if (genbuf[0] == '\0')
    return -1;
  clen = atoi(genbuf);
  if (clen == 0)
    return 0;
  if (clen > max)
    return max;
  return clen - 1;
}


void
stand_title(title)
  char *title;
{
  clear();
  prints(COLOR1"[1m¡i %s ¡j[m\n", title);
}


/* opus : cursor position */


void
cursor_load(fname)
  char *fname;
{
  FILE *fp;
  char buf[64], csr[16]="¤ô²y";
  
  sethomefile(buf,cuser.userid,fname);
  if(dashf(buf))
  {
    if (fp = fopen(buf,"r"))
       fscanf(fp, "%s", &csr);
    fclose(fp);
  }
  if(strlen(csr)<2 || strlen(csr)>10)
     strcpy(csr,"¤ô²y");
  strip_ansi(csr, csr, STRIP_ALL);
  strcpy(currutmp->cursor, csr);
}


void
cursor_show(row, column)
  int row, column;
{
//  FILE *fp;
  int i;
  char *CUR="¡´"; //buf[256];
/*
  sethomefile(buf,cuser.userid,"cursor");
  if(dashf(buf))
  {
    if (fp = fopen(buf,"r"))
      fscanf(fp, "%s", buf);
    if (strlen(buf) >= 2)
      CUR = buf;
    fclose(fp);
  }
*/
  i = (rand()%(strlen(CUR)/2))*2;
  move(row, column);
  prints("%c%c", CUR[i], CUR[i+1]);
//  move(row, column + 1);
  move(b_lines,0);
}


void
cursor_clear(row, column)
  int row, column;
{
  move(row, column);
  outs(STR_UNCUR);
}


int
cursor_key(row, column)
  int row, column;
{
  int ch;

  cursor_show(row, column);
  ch = egetch();
  move(row, column);
  outs(STR_UNCUR);
  return ch;
}

#ifdef CURSOR_BAR

void
cursor_bar_clear(row, column,t1,t2)
  int row,column;
  char *t1;
  char *t2;
{
  char buf[256];
  sprintf(buf,"%*s  [1;37m[[36m%c[37m]%-27s[m ",
    column,"",t1[1],t1+2);
  if(currstat <= CLASS)
    strcat(buf, t2);
  outmsgline(buf,row);
}

void
cursor_bar_show(row, column,t1,t2)
  int row,column;
  char *t1;
  char *t2;
{
  int i=0;
  char *CUR="¡´", buf[256];
  
  sprintf(buf,"%*s[1;37m¡´[44m[[33m%c[37m]%-27s[m ",
    column,"", t1[1], t1+2);
  if(currstat <= CLASS)
    strcat(buf,t2);
  outmsgline(buf,row);
//  prints("%c%c","¬Pªe©]¸Ü"[i],"¬Pªe©]¸Ü"[i+1]);
  move(b_lines,0);
}

/*
int
cursor_bar_key(row, column)
  int row, column;
{
  int ch;
    
  cursor_bar_show(row, column);
  ch = egetch();
  move(row, column);
  outs(STR_UNCUR);
  return ch;
}
*/

#endif


void
printdash(mesg)
  char *mesg;
{
  int head = 0, tail;

  if (mesg)
    head = (strlen(mesg) + 1) >> 1;

  tail = head;

  while (head++ < 38)
    outch('-');

  if (tail)
  {
    outch(' ');
    outs(mesg);
    outch(' ');
  }

  while (tail++ < 38)
    outch('-');
  outch('\n');
}


// wildcat : ¨âºØ³f¹ô
int
check_money(unsigned long int money,int mode)
{
  unsigned long int usermoney;

  usermoney = mode ? cuser.goldmoney : cuser.silvermoney;
  if(usermoney<money)
  {
    move(1,0);
    clrtobot();
    move(10,10);
    pressanykey("©êºp¡I±z¨­¤W¥u¦³ %d ¤¸¡A¤£°÷­ò¡I",usermoney);
    return 1;
  }
  return 0;
}


// wildcat : ¸gÅç­È
int
check_exp(unsigned long int exp)
{
  if(cuser.exp<exp)
  {
    move(1,0);
    clrtobot();
    move(10,10);
    pressanykey("©êºp¡I±zªº¸gÅç­È¥u¦³ %d ÂI¡A¤£°÷­ò¡I",cuser.exp);
    return 1;
  }
  return 0;
}

// wildcat : ¤dÁH¼úª÷ :p
void
get_bonus()
{
  int money;
  time_t now = time(0);
  char buf[255];
  
  money= random()%2001;
  xuser.silvermoney +=money;
  sprintf(buf,"[1;31m®¥³ß [33m%s [31mÀò±o¤dÁH¼úª÷ [36m%d ¤¸»È¹ô , %s[m",
    cuser.userid,money,Cdate(&now));
  f_cat(BBSHOME"/log/y2k_bonus",buf);

  pressanykey("®¥³ß§AÀò±o %d ¤¸»È¹ôªº¤dÁH¼úª÷",money);
}  

/* wildcat 981218 */
#define INTEREST_TIME	86400*7	// wildcat:7¤Ñµo©ñ¤@¦¸§Q®§
#define BANK_RATE	1.000	// wildcat:»È¦æ§Q²v 1.00

void
update_data()
{
  int man = count_multi();
  int add = (time(0) - update_time)/60/man;

  getuser(cuser.userid);

  if((time(0) - xuser.dtime) >= INTEREST_TIME && xuser.silvermoney)
  {
//    xuser.silvermoney *= BANK_RATE;
    xuser.dtime = time(0);
  }  
  
  if(add)
  {
    rpgtmp.age += add;
    xuser.exp += rpgtmp.race != 3 ? add*2 : add*2*rpgtmp.level;
//    xuser.silvermoney += rpgtmp.race != 3 ? add*2 : add*10;
    xuser.totaltime += ((time(0) - update_time) / man) ;
    if(rpgtmp.hp && rpgtmp.hp < rpgtmp.con*30)
      rpgtmp.hp += 3*add;
    if(rpgtmp.hp > rpgtmp.con*30)
      rpgtmp.hp = rpgtmp.con*30;  
    if(rpgtmp.mp < rpgtmp.wis*10)
      rpgtmp.mp += add;
    update_time = time(0);
  }
  cuser = xuser;
  rpguser = rpgtmp;
  substitute_record(fn_passwd, &cuser, sizeof(userec), usernum);
  rpg_rec(xuser.userid,rpguser);
}


int
show_file(char *filename, int y, int lines, int mode)
{
  FILE *fp;
  char buf[256];
  clrchyiuan(y, y + lines);
  move(y, 0);
  if ((fp = fopen(filename, "r")))
  {
    while(fgets(buf,256,fp) && lines--)
      outs(Ptt_prints(buf,mode));
    fclose(fp);
  }
  else 
    return 0;

  return 1;
}

int
show_welcomes()
{
  int i, j, id, num, num1;
  char buf[128], fn[128], path[128];
  fileheader item;
  char welcomes[24][128];

  id=0;
  setapath(path, "Announce");
  setadir(buf, path);
  num = rec_num(buf, sizeof item);
  for (i = 0; i <= num; i++)
     if (rec_get(buf, &item, sizeof item, i) != -1)
       if (item.title[3]=='[' && item.title[8]==']')
       {
         fileheader subitem;
         setapath(path,"Announce");
         sprintf(fn,"%s/%s",path,item.filename);
         setadir(path, fn);
         num1 = rec_num(path, sizeof item);
         for (j = 0; j <= num1; j++)
           if (rec_get(path, &subitem, sizeof item, j) != -1)
           {
              sprintf(welcomes[id++],"%s/%s",
                 item.filename , subitem.filename);
              if (id > 24) break;
           }
         if (id > 24) break;
       }

  setapath(path, "Announce");
  sprintf(fn, "%s/%s", path, welcomes[rand()%id]);
  show_file(fn, 0, 21, ONLY_COLOR);

  return 1;
}

/*---------------------------------------------------------------------*/
/* int chyiuan_ansi(buf,str,max)ªº¥Îªk:¦Ûµe¹³¥Î                        */
/* buf:chyiuan_ansi¹L«áªºstring                                        */
/* str:chyiuan_ansi¤§«eªºstring                                        */
/* count:¶Ç¦^move®ÉÀ³¸Óshiftªº¼Æ­È                                     */
/* ³Æµù:¦pªG¬O±m¦â¼Ò¦¡, ¶W¹L­­¨î­È®É, ·|¬å±¼¦r¦ê¶W¹L³¡¥÷, ¦ý«O¯dcolor  */
/*---------------------------------------------------------------------*/

int
chyiuan_ansi(buf,str,max)
  char *buf,*str;
  int max;
{
  int count = 0;
  int count0 = 0;
  int count1 = 0;
  char buf0[256];
  count0 = strip_ansi(buf0,str,0);
  if((cuser.uflag & COLOR_FLAG) && count0 <= max)
  {
    count1=strip_ansi(NULL,str,1);
    count=count1-count0;
    strcpy(buf, str);
  }
  else if((cuser.uflag & COLOR_FLAG) && count0 > max)
  {
    count0 = cut_ansistr(buf0,str,max);
    count1 = strip_ansi(NULL,buf0,1);
    count=count1-count0;
    strcpy(buf, buf0);
  }
  else
  {
    count=0;
    strcpy(buf, buf0);
  }
  return count;
}


int
answer(char *s)
{
  char ch;
  outmsg(s);
  ch = igetch ();
  if (ch == 'Y')
    ch = 'y';
  if (ch == 'N')
    ch = 'n';
  return ch;
}

#if defined(SunOS) || defined(SOLARIS)

#include <syslog.h>

void
setenv(name, value, overwrite)
  const char *name;
  const char *value;
  int overwrite;
{
  if (overwrite || (getenv(name) == 0))
  {
    char *p;

    if ((p = malloc(strlen(name) + strlen(value) + 2)) == 0)
    {
      syslog(LOG_ERR, "out of memory\n");
      exit(1);
    }
    sprintf(p, "%s=%s", name, value);
    putenv(p);
  }
}

atexit(procp)
void (*procp)();
{
   on_exit(procp, 0);
}

#endif

capture_screen()
{
   char fname[200];
   FILE* fp;
   extern screenline *big_picture;
   extern uschar scr_lns;
   int i;

   setuserfile(fname, "buf.0");
   if (fp = fopen(fname, "w")) {
      for (i = 0; i < scr_lns; i++)
         fprintf(fp, "%.*s\n", big_picture[i].len, big_picture[i].data);
      fclose(fp);
   }
}


edit_note()
{
   char fname[200];
   int mode0 = currutmp->mode;
   int stat0 = currstat;
   char c0 = *quote_file;

   *quote_file = 0;
   setutmpmode(NOTE);
   setuserfile(fname, "note");
   vedit(fname, 0);
   currutmp->mode = mode0;
   currstat = stat0;
   *quote_file = c0;
}


char*
my_ctime(const time_t *t)
{
  struct tm *tp;
  static char ans[100];

  tp = localtime(t);
  sprintf(ans, "%d/%02d/%02d %02d:%02d:%02d", tp->tm_year % 100,
     tp->tm_mon + 1,tp->tm_mday, tp->tm_hour, tp->tm_min, tp->tm_sec);
  return ans;
}
#if 0
/* ----------------------------------------------------- */
/* ¼È¦sÀÉ TBF (Temporary Buffer File) routines           */
/* ----------------------------------------------------- */

char *
tbf_ask()
{
  static char fn_tbf[10] = "buf.0";
  getdata(b_lines, 0, "½Ð¿ï¾Ü¼È¦sÀÉ(0-9)¡G", fn_tbf + 4, 2, DOECHO,"0");
  return fn_tbf;
}
#endif


int
inexp(unsigned long int exp)
{
  update_data();
  if(belong(BBSHOME"/game/rpg/baduser",cuser.userid)) return -1;  
  cuser.exp = xuser.exp + exp;
  substitute_record(fn_passwd, &cuser, sizeof(userec), usernum);
  return cuser.exp;
}

int
deexp(unsigned long int exp)
{
  update_data();
  if(xuser.exp <= exp) cuser.exp=0;
  else cuser.exp = xuser.exp - exp;
  substitute_record(fn_passwd, &cuser, sizeof(userec), usernum);
  return cuser.exp;
}


/*
 * check_personal_note() ªº¥\¥Î¸ò chkmail() ¤@¼Ë..
 * ©Ò¥H¥i¥H¥[¦b my_query() ¤¤:³o¼Ë§O¤Hquery®É´N¥i¥H¬Ý¨ì¦³¨S¦³·s¯d¨¥
 * ÁÙ¦³¥i¥H¥[¦b show_title() ¤¤:³o¼Ë¦³·s¯d¨¥®É´N·|¹³¦³·s«H¥ó¤@¼Ë¦btitle´£¥Ü³á!
 */

int
check_personal_note(int newflag, char* userid) {
 char fpath[256];
 FILE *fp;
 int  total = 0;
 notedata myitem;
 char *fn_note_dat      = "pnote.dat";

 if (userid == NULL)
   setuserfile(fpath, fn_note_dat);
 else
   sethomefile(fpath, userid, fn_note_dat);

 if ((fp = fopen(fpath, "r")) != NULL) {
   while (fread(&myitem, sizeof(myitem), 1, fp) == 1) {
     if (newflag)
       if (myitem.buf[0][0] == 0) total++;
     else
       total++;
   }
   fclose(fp);
   return total;
 }
 return 0;
}

game_log(va_alist)
va_dcl
{
 va_list ap;
 int file;
 char *fmt,msg[200],ff[40];
 time_t now;
 FILE *fs;

 va_start(ap);
  file = va_arg(ap, int);
  fmt = va_arg(ap, char *);
  vsprintf(msg, fmt, ap);
 va_end(ap);

 switch(file) /* ³o¤@¬q¥i¥H¦Û¤v§ï! */
 {
  case XAXB: strcpy(ff,"log/ab.log"); break;
  case CHICKEN: strcpy(ff,"log/pip.log"); break;
  case BLACKJACK: strcpy(ff,"log/bj.log"); break;
  case STOCK: strcpy(ff,"log/stock.log"); break;
  case DICE: strcpy(ff,"log/dice.log"); break;
  case GP: strcpy(ff,"log/gp.log"); break;
  case MARIE: strcpy(ff,"log/marie.log"); break;
  case RACE: strcpy(ff,"log/race.log"); break;
  case BINGO: strcpy(ff,"log/bingo.log"); break; 
  case NINE: strcpy(ff,"log/nine.log"); break;
  case NumFight: strcpy(ff,"log/fightnum.log"); break;
  case CHESSMJ: strcpy(ff,"log/chessmj.log"); break;
  case SEVENCARD: strcpy(ff,"log/seven.log"); break;
  case FIVE: strcpy(ff,"log/five.log"); break;
 }
 fs=fopen(ff,"a+");
 now=time(0);
 fprintf(fs,"[1;33m%s [32m%s [36m%s[m\n", Cdate(&now),cuser.userid,msg);
 fclose(fs);
}

int
show_help(mode)
  int mode;
{
  if(inmore)
    more(BBSHOME"/etc/help/MORE.help",YEA);
  else if(mode == LUSERS)
  {
   // more(BBSHOME"/etc/help/LUSERS.help",YEA);
    if(HAS_PERM(PERM_SYSOP))
      more(BBSHOME"/etc/help/talk_sysop.hlp",YEA);
    else
      more(BBSHOME"/etc/help/talk_user.hlp",YEA);
  }
  else if(mode == READBRD || mode == READNEW)
    more(BBSHOME"/etc/help/BOARD.help",YEA);
  else if(mode == READING)
    more(BBSHOME"/etc/help/READ.help",YEA);
  else if(mode == ANNOUNCE)
    more(BBSHOME"/etc/help/ANNOUNCE.help",YEA);
  else if(mode == RMAIL)
    more(BBSHOME"/etc/help/MAIL.help",YEA);
  else if(mode == EDITING)
    more(BBSHOME"/etc/help/EDIT.help",YEA);
  else
    HELP();
  return 0;
}
void
sysop_bbcall(va_alist)
  va_dcl
{
  va_list ap;
  FILE *fp;
  char NumMode[10][5],PagerNum[10][10];
  int i=0,j;
  char msg[250], *fmt,buf[200];

  msg[0]=0;
  va_start(ap);
  fmt = va_arg(ap, char *);
  if(fmt) vsprintf(msg, fmt, ap);
  va_end(ap);
    
  if(fp=fopen(BBSHOME"/etc/sysop_bbcall","r"))
  {
    while(fgets(buf,256,fp))
    {
      if(buf[0] == '#') continue; 
      buf[strlen(buf) - 1] = '\0';   
      strncpy(NumMode[i],buf+1,3);
      NumMode[i][3] = '\0';  
      strncpy(PagerNum[i],buf+4,6);
      PagerNum[i][6] = '\0';  
      i++;
    }
    fclose(fp);
  }
  for(j = 0 ; i > j; j++)
  {
    DL_func("SO/bbcall.so:bbcall",atoi(NumMode[j]),1,PagerNum[j],0,msg);
  }
}

void
user_bbcall(va_alist)
  va_dcl
{
  va_list ap;
  char NumMode[10],PagerNum[10];
  char msg[250], *fmt;

  msg[0]=0;
  va_start(ap);
  fmt = va_arg(ap, char *);
  if(fmt) vsprintf(msg, fmt, ap);
  va_end(ap);
    
  sprintf(NumMode,"0%d",cuser.pagermode);
  strncpy(PagerNum,cuser.pagernum,6);
  PagerNum[6] = '\0';  

  DL_func("SO/bbcall.so:bbcall",atoi(NumMode),1,PagerNum,0,msg);
}

int
mail2user(userec muser, char *title, char *fname)
{
  fileheader mhdr;
  char buf[128], buf1[80];

  sprintf(buf1, "home/%s/mailbox", muser.userid);
  stampfile(buf1, &mhdr);
  strcpy(mhdr.owner, cuser.userid);
  strcpy(mhdr.title, title);
  mhdr.savemode = mhdr.filemode = 0;
  sprintf(buf, "home/%s/mailbox/.DIR", muser.userid);
  rec_add(buf, &mhdr, sizeof(mhdr));
  f_cp(fname, buf1, O_TRUNC);
  return 0;
}

void
debug(mode)
  char *mode;
{
  time_t now = time(0);
  char buf[512];

  sprintf(buf, "%s %s %s\n", Cdate(&now), mode, cuser.userid);      
  f_cat("debug",buf);
}

void
Security (x, y, sysopid, userid)
     int x, y;
     char *sysopid, *userid;
{
  FILE *fp = fopen ("etc/security", "w");
  fileheader fhdr;
  time_t now = time (0);
  char genbuf[200], reason[30];
  int i, flag = 0;
  for (i = 4; i < NUMPERMS; i++)
    {
      if (((x >> i) & 1) != ((y >> i) & 1))
	{
	  if (!flag)
	    {
	      now = time (NULL) - 6 * 60;
	      sprintf (genbuf, "§@ªÌ: [¨t²Î¤é»x] ¬ÝªO: Security\n");
	      strcat (genbuf, "¼ÐÃD: [¤½¦w°O¿ý] ¯¸ªø­×§ïÅv­­³ø§i\n");
	      fputs (genbuf, fp);
	      sprintf (genbuf, "®É¶¡: %s\n", ctime (&now));
	      fputs (genbuf, fp);
	    }
	  sprintf (genbuf, "   ¯¸ªø[1;32m%s%s%s%s[mªºÅv­­\n", sysopid, (((x >> i) & 1) ? "[1;33mÃö³¬" : "[1;33m¶}±Ò"), userid, permstrings[i]);
	  fputs (genbuf, fp);
	  flag++;
	}
    }
  if (flag)
    {
      clrtobot ();
      clear ();
      while (!getdata (5, 0
		       ,"½Ð¿é¤J²z¥Ñ¥H¥Ü­t³d¡G", reason, 60, DOECHO
		       ,"¬Ýª©ª©¥D:"));
      sprintf (genbuf, "\n   [1;37m¯¸ªø%s­×§ïÅv­­²z¥Ñ¬O¡G%s[m", cuser.userid, reason);
      fputs (genbuf, fp);
      fclose (fp);
      sprintf (genbuf, "boards/%s", "Security");
      stampfile (genbuf, &fhdr);
      rename ("etc/security", genbuf);

      sprintf (fhdr.title, "[¤½¦w°O¿ý] ¯¸ªø­×§ï%sÅv­­³ø§i", userid);
      strcpy (fhdr.owner, "[¨t²Î¤é»x]");
      sprintf (genbuf, "boards/%s/.DIR", "Security");
      rec_add (genbuf, &fhdr, sizeof (fhdr));
    }
}

