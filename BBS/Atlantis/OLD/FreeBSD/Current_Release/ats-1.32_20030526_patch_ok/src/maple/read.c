/*-------------------------------------------------------*/
/* read.c       ( NTHU CS MapleBBS Ver 2.36 )            */
/*-------------------------------------------------------*/
/* target : board/mail interactive reading routines      */
/* create : 95/03/29                                     */
/* update : 02/09/01 (Dopin)                             */
/*-------------------------------------------------------*/

#include "bbs.h"

struct keeploc {
  char *key;
  int top_ln;
  int crs_ln;
  struct keeploc *next;
};
typedef struct keeploc keeploc;

char currdirect[MAXPATHLEN];
static fileheader *headers = NULL;
static int last_line;
static int hit_thread;

#ifdef USE_MULTI_TITLE
extern char title_color;
#endif
extern int search_num();

/* ----------------------------------------------------- */
/* cursor & reading record position control              */
/* ----------------------------------------------------- */

keeploc *getkeep(char *s, int def_topline, int def_cursline) {
  static struct keeploc *keeplist = NULL;
  struct keeploc *p;
  void *malloc();

  if (def_cursline >= 0)
     for (p = keeplist; p; p = p->next)
     {
       if (!strcmp(s, p->key))
       {
         if (p->crs_ln < 1)
           p->crs_ln = 1;
         return p;
       }
     }
  else
     def_cursline = -def_cursline;
  p = (keeploc *) malloc(sizeof(keeploc));
  p->key = (char *) malloc(strlen(s) + 1);
  strcpy(p->key, s);
  p->top_ln = def_topline;
  p->crs_ln = def_cursline;
  p->next = keeplist;
  return (keeplist = p);
}


void
fixkeep(s, first)
  char *s;
  int first;
{
  keeploc *k;

  k = getkeep(s, 1, 1);
  if (k->crs_ln >= first)
  {
    k->crs_ln = (first == 1 ? 1 : first - 1);
    k->top_ln = (first < 11 ? 1 : first - 10);
  }
}


/* calc cursor pos and show cursor correctly */

static int
cursor_pos(locmem, val, from_top)
  struct keeploc *locmem;
  int val;
  int from_top;
{
  int top;

  if (val > last_line)
  {
    bell();
    val = last_line;
  }
  if (val <= 0)
  {
    bell();
    val = 1;
  }

  top = locmem->top_ln;
  if (val >= top && val < top + p_lines)
  {
    cursor_clear(3 + locmem->crs_ln - top, 0);
    locmem->crs_ln = val;
    cursor_show(3 + val - top, 0);
    return DONOTHING;
  }
  locmem->top_ln = val - from_top;
  if (locmem->top_ln <= 0)
    locmem->top_ln = 1;
  locmem->crs_ln = val;
  return PARTUPDATE;
}


static int
move_cursor_line(locmem, mode)
  keeploc *locmem;
  int mode;
{
  int top, crs;
  int reload = 0;

  top = locmem->top_ln;
  crs = locmem->crs_ln;
  if (mode == READ_PREV)
  {
    if (crs <= top)
    {
      top -= p_lines - 1;
      if (top < 1)
        top = 1;
      reload = 1;
    }
    if (--crs < 1)
    {
      crs = 1;
      reload = -1;
    }
  }
  else if (mode == READ_NEXT)
  {
    if (crs >= top + p_lines - 1)
    {
      top += p_lines - 1;
      reload = 1;
    }
    if (++crs > last_line)
    {
      crs = last_line;
      reload = -1;
    }
  }
  locmem->top_ln = top;
  locmem->crs_ln = crs;
  return reload;
}

strcmp_lower(char* s1, char *s2)
{
   return !strstr_lower(s1, s2);
}

my_strcmp(char* s1, char* s2)
{
   return strncmp(s1, s2, 40);
}

my_strcasecmp(char* s1, char* s2)
{
   return strncasecmp(s1, s2, 40);
}

static int
thread(locmem, stype)
  keeploc *locmem;
  int stype;
{
  static char a_ans[32], t_ans[32];
  char ans[32], s_pmt[64];
  register char *tag, *query;
  register int now, pos, match, near;
  fileheader fh;
  int circulate_flag = 1;  /* circulate at end or begin */
  int (*cmpf)() = strcmp_lower;


  match = hit_thread = 0;
  now = pos = locmem->crs_ln;

/*
woju
*/
  if (stype == 'a' || stype == 'A') {
     if (!*currowner)
        return DONOTHING;
     query = currowner;
     circulate_flag = 0;
     stype = (stype == 'a') ? AUTHOR_NEXT : AUTHOR_PREV;
     cmpf = my_strcasecmp;
  }
  else if (stype == '/' || stype == '?') {
     if (!*t_ans)
        return DONOTHING;
     query = t_ans;
     circulate_flag = 0;
     stype = RS_TITLE | ((stype == '/') ? RS_FORWARD  : 0);
  }
  else if (stype & RS_RELATED) {
    tag = headers[pos - locmem->top_ln].title;
    if (stype & RS_CURRENT)
    {
      cmpf = my_strcmp;
      if (stype & RS_FIRST)
      {
        if (!strncmp(currtitle, tag, 40))
          return DONOTHING;
        near = 0;
      }
      query = currtitle;
    }
    else
    {
      query = subject(tag);
      if (stype & RS_FIRST)
      {
        if (query == tag)
          return DONOTHING;
        near = 0;
      }
    }
  }
  else if (!(stype & RS_THREAD))
  {
    query = (stype & RS_TITLE) ? t_ans : a_ans;
    if (!*query && query == a_ans)
       if (*currowner)
          strcpy(a_ans, currowner);
       else if (*currauthor)
          strcpy(a_ans, currauthor);
    sprintf(s_pmt, "%s·j´M%s [%s] ",(stype & RS_FORWARD) ? "©¹«á":"©¹«e",
           (stype & RS_TITLE) ? "¼ÐÃD" : "§@ªÌ", query);
/*  sprintf(s_pmt, "·j´M%s [%s] ", (stype & RS_TITLE) ? "¼ÐÃD" : "§@ªÌ", query); */
    getdata(b_lines - 1, 0, s_pmt, ans, 30, DOECHO, 0);
    if (*ans)
    {
      strcpy(query, ans);
    }
    else
    {
      if (*query == '\0')
        return DONOTHING;
    }
  }

  tag = fh.owner;

  do
  {
/*
woju
*/
    if (!circulate_flag || stype & RS_RELATED)
    {
      if (stype & RS_FORWARD)
      {
        if (++now > last_line)
          return DONOTHING;
      }
      else
      {
        if (--now <= 0)
        {
          if ((stype & RS_FIRST) && (near))
          {
            hit_thread = 1;
            return cursor_pos(locmem, near, 10);
          }
          return DONOTHING;
        }
      }
    }
    else
    {
      if (stype & RS_FORWARD)
      {
        if (++now > last_line)
          now = 1;
      }
      else
      {
        if (--now <= 0)
          now = last_line;
      }
    }

    get_record(currdirect, &fh, sizeof(fileheader), now);

    if (fh.owner[0] == '-')
      continue;

    if (stype & RS_THREAD)
    {
      if (strncasecmp(fh.title, str_reply, 3))
      {
        hit_thread = 1;
        return cursor_pos(locmem, now, 10);
      }
      continue;
    }

    if (stype & RS_TITLE)
      tag = subject(fh.title);

    if (!cmpf(tag, query))
    {
      if (stype & RS_FIRST)
      {
        if (tag != fh.title)
        {
          near = now;
          continue;
        }
      }
      hit_thread = 1;
      match = cursor_pos(locmem, now, 10);
      if ((!(stype & RS_CURRENT)) && (stype & RS_RELATED) &&
        strncmp(currtitle, query, 40))
      {
        strncpy(currtitle, query, 40);
        match = PARTUPDATE;
      }
      break;
    }
  }
  while (now != pos);

  return match;
}


/* ------------------ */
/* ÀÉ®×¶Ç¿é¤u¨ã°Æµ{¦¡ */
/* ------------------ */


void
z_download(fpath)
  char *fpath;
{
  char cmd[100] = "bin/sz -a ";
  char pname[50];
  char fname[13];
  int i;

  getdata(b_lines - 1, 0, "¨Ï¥Î Z-Modem ¤U¶ÇÀÉ¦W:", fname, 9, LCECHO, 0);
  for (i = 0; i < 8; i++)
     if (!(isalnum(fname[i]) || fname[i] == '-' || fname[i] == '_'))
        if (i)
           break;
        else
           return;

  fname[i] = 0;
  strcat(fname, ".bbs");
  setuserfile(pname, fname);
  unlink(pname);
  Link(fpath, pname);
  strcat(cmd, pname);
  if (mbbsd) {
    pid_t pid;
    int status;

    if (pid = fork())
      waitpid(pid, &status, 0);
    else
      execl("bin/msz", "-a", pname, NULL);
  }
  else
     system(cmd);
  unlink(pname);
}


#ifdef INTERNET_EMAIL
void mail_forward(fileheader *fhdr, char *direct, int mode) {
  char buf[STRLEN], *p;

#ifdef LOCK_ARTICAL
  if(fhdr->report == 's') return;
#endif

  strncpy(buf, direct, sizeof(buf));
  if (p = strrchr(buf, '/'))
    *p = '\0';
  switch (doforward(buf, fhdr, mode))
  {
  case 0:
    outmsg(msg_fwd_ok);
    break;
  case -1:
    outmsg(msg_fwd_err1);
    break;
  case -2:
    outmsg(msg_fwd_err2);
  }
  refresh();
  sleep(1);
}
#endif


static int
select_read(locmem,sr_mode)
  keeploc *locmem;
  int sr_mode;
{
  register char *tag,*query;
  fileheader fh;
  char fpath[80], genbuf[MAXPATHLEN];
  char static t_ans[TTLEN+1]="";
  char static a_ans[IDLEN+1]="";
  int fd, fr, size = sizeof(fileheader);
  struct stat st;

  if( currmode & MODE_SELECT)
    return -1;
  if(sr_mode == RS_TITLE)
    query = subject(headers[locmem->crs_ln - locmem->top_ln].title);
  else if (sr_mode == RS_FIRST) {
    sprintf(fpath, "boards/%s/SR.thread", currboard);
    if (stat(fpath, &st) == 0 && st.st_mtime > time(0) - 3600 * 24) {
       currmode ^= MODE_SELECT;
       strcpy(currdirect,fpath);
       return st.st_size;
    }
    query = "Re:";
  }
  else if (sr_mode == RS_CURRENT)
     query = ".";
  else if (sr_mode == RS_THREAD)
     query = "m";
  else {
    query = (sr_mode == RS_RELATED) ? t_ans : a_ans;
    sprintf(fpath, "·j´M%s [%s] ",
        (sr_mode == RS_RELATED) ? "¼ÐÃD" : "§@ªÌ", query);
    if(getdata(b_lines, 0,fpath,fpath, 30, DOECHO,0))
    {
      char buf[64];
      str_lower(buf,fpath);
      strcpy(query,buf);
    }
    if(!(*query))
     return DONOTHING;
  }
  outmsg("·j´M¤¤,½Ðµy«á...");refresh();
  if ((fd = open(currdirect, O_RDONLY, 0)) != -1)
  {
    sprintf(genbuf,"SR.%s",cuser.userid);
    if(currstat==RMAIL)
      sethomefile(fpath,cuser.userid,genbuf);
    else if (sr_mode == RS_FIRST) {
      fileheader* fhdr;
      int num, i, j, count;
      FILE* fp;

      fstat(fd, &st);
      if (fhdr = malloc(st.st_size)) {
         num = read(fd, fhdr, st.st_size) / sizeof(fileheader);
         close(fd);
      }
      else {
         close(fd);
         return;
      }
      sprintf(fpath, "boards/%s/SR.thread", currboard);


      if (fp = fopen(fpath, "w")) {
         for (i = 0; i < num; i++)
            if (strncmp(fhdr[i].title, "Re:", 3)) {
               for (count = j = 0; j < num; j++)
                  if (!strncmp(fhdr[i].title, subject(fhdr[j].title), 40))
                     ++count;
               sprintf(fhdr[i].date, "%5d", count);
               fwrite(fhdr + i, sizeof(fileheader), 1, fp);
            }
         fclose(fp);
      }
      free(fhdr);
      currmode ^= MODE_SELECT;
      strcpy(currdirect,fpath);
      return num;
    }
    else
      setbfile(fpath,currboard,genbuf);
    if(((fr= open(fpath,O_WRONLY | O_CREAT | O_TRUNC,0600)) != -1))
    {
        switch(sr_mode)
        {
          case RS_TITLE:
            while(read(fd,&fh,size) == size){
              tag = subject(fh.title);
              if(!strncmp(tag, query, 40))
                write(fr,&fh,size);
            }
            break;
          case RS_RELATED:
            while(read(fd,&fh,size) == size){
              tag = fh.title;
              if(strstr_lower(tag,query))
                write(fr,&fh,size);
            }
            break;
          case RS_AUTHOR:
            while(read(fd,&fh,size) == size){
              tag = fh.owner;
              if(strstr_lower(tag,query))
                write(fr,&fh,size);
            }
            break;
          case RS_CURRENT:
            while(read(fd,&fh,size) == size){
              tag = fh.owner;
              if(!strchr(tag, '.'))
                write(fr,&fh,size);
            }
            break;
          case RS_THREAD:
            while(read(fd,&fh,size) == size){
              if(fh.filemode & (FILE_MARKED |  FILE_DIGEST))
                write(fr,&fh,size);
            }
            break;
        }
      fstat(fr,&st);
      close(fr);
    }
    close(fd);
    if(st.st_size){
      currmode ^= MODE_SELECT;
      strcpy(currdirect,fpath);
    }
  }
  return st.st_size;
}

static int i_read_key(rcmdlist, locmem, ch)
  struct one_key *rcmdlist;
  struct keeploc *locmem;
  int ch;
{
  int i, mode = DONOTHING;
  static thread_title;

  switch (ch)
  {
  case 'q':
  case 'e':
  case KEY_LEFT:
    if (thread_title) {

       --thread_title;
       if (thread_title) {
          sprintf(currdirect, "boards/%s/SR.thread", currboard);
          return NEWDIRECT;
       }
    }
    return (currmode & MODE_SELECT) ? board_select() :
        (currmode & MODE_ETC) ? board_etc() :
        (currmode & MODE_DIGEST) ? board_digest() : DOQUIT;

  case Ctrl('L'):
    refresh();
    break;

  /* woju */ /* Dopin: for DSO mode */
  case Ctrl('R'):
     DL_func("bin/message.so:get_msg", 0);
     return FULLUPDATE;

  /* Dopin */
#ifdef CTRL_G_REVIEW
  case Ctrl('G'):
     DL_func("bin/message.so:get_msg", 1);
     return FULLUPDATE;
#endif

#ifdef CHOICE_RMSG_USER
  case Ctrl('V'):
     choice_water_id();
     break;
#endif

  case Ctrl('U'):
     t_users();
     return FULLUPDATE;
  case Ctrl('B'):
     m_read();
     return FULLUPDATE;
  case KEY_ESC:
#ifdef TAKE_IDLE
     if (KEY_ESC_arg == 'i') {
        t_idle();
        return FULLUPDATE;
     }
     else
#endif
     if(KEY_ESC_arg == 'n') {
        edit_note();
        return FULLUPDATE;
     }
     break;
/*
  case 'a':
    if (thread(locmem, RS_FORWARD))
      return PARTUPDATE;
    return FULLUPDATE;;

  case 'A':
    if (thread(locmem, 0))
      return PARTUPDATE;
    return FULLUPDATE;;

  case '/':
    if (thread(locmem, RS_TITLE | RS_FORWARD))
      return PARTUPDATE;
    return FULLUPDATE;;

  case '?':
    if (thread(locmem, RS_TITLE))
      return PARTUPDATE;
    return FULLUPDATE;;
*/
  case 'a':
  case 'A':
    if(select_read(locmem,RS_AUTHOR))
      return NEWDIRECT;
    else
      return READ_REDRAW;

  case '/':
  case '?':
    if(select_read(locmem,RS_RELATED))
     return NEWDIRECT;
    else
      return READ_REDRAW;

  case 'S':
    if(select_read(locmem,RS_TITLE))
      return NEWDIRECT;
    else
      return READ_REDRAW;

  case 'L':
    if(select_read(locmem,RS_CURRENT)) /* local articles */
      return NEWDIRECT;
    else
      return READ_REDRAW;

  case 'G':
    if(select_read(locmem,RS_THREAD)) /* marked articles */
      return NEWDIRECT;
    else
      return READ_REDRAW;

  case 'u':
    if(!thread_title && select_read(locmem,RS_FIRST)) {
      thread_title = 1;
      return NEWDIRECT;
    }
    else {
      bell();
      return DONOTHING;
    }

    /* quick search title first */
  case '=':
    return thread(locmem, RELATE_FIRST);

  case '\\':
    return thread(locmem, CURSOR_FIRST);

    /* quick search title forword */
  case ']':
    return thread(locmem, RELATE_NEXT);

  case '+':
    return thread(locmem, CURSOR_NEXT);

    /* quick search title backword */
  case '[':
    return thread(locmem, RELATE_PREV);

  case '-':
    return thread(locmem, CURSOR_PREV);

  case '<':
  case ',':
    return thread(locmem, THREAD_PREV);

  case '.':
  case '>':
    return thread(locmem, THREAD_NEXT);

  case 'p':
  case 'k':
  case KEY_UP:
    return cursor_pos(locmem, locmem->crs_ln - 1, p_lines - 2);

  case 'n':
  case 'j':
  case KEY_DOWN:
    return cursor_pos(locmem, locmem->crs_ln + 1, 1);

  case ' ':
  case KEY_PGDN:
  case 'N':
  case Ctrl('F'):
    if (last_line >= locmem->top_ln + p_lines)
    {
      if (last_line > locmem->top_ln + p_lines)
        locmem->top_ln += p_lines;
      else
        locmem->top_ln += p_lines - 1;
      locmem->crs_ln = locmem->top_ln;
      return PARTUPDATE;
    }
    cursor_clear(3 + locmem->crs_ln - locmem->top_ln, 0);
    locmem->crs_ln = last_line;
    cursor_show(3 + locmem->crs_ln - locmem->top_ln, 0);
    break;

  case KEY_PGUP:
  case 'P':
    if (locmem->top_ln > 1)
    {
      locmem->top_ln -= p_lines;
      if (locmem->top_ln <= 0)
        locmem->top_ln = 1;
      locmem->crs_ln = locmem->top_ln;
      return PARTUPDATE;
    }
    break;

  case KEY_END:
  case '$':
    if (last_line >= locmem->top_ln + p_lines)
    {
      locmem->top_ln = last_line - p_lines + 1;
      if (locmem->top_ln <= 0)
        locmem->top_ln = 1;
      locmem->crs_ln = last_line;
      return PARTUPDATE;
    }
    cursor_clear(3 + locmem->crs_ln - locmem->top_ln, 0);
    locmem->crs_ln = last_line;
    cursor_show(3 + locmem->crs_ln - locmem->top_ln, 0);
    break;

  case 'F':
  case 'U':
/*
woju
    if (HAS_PERM(PERM_FORWARD))
    {
*/
      mail_forward(&headers[locmem->crs_ln - locmem->top_ln],
        currdirect, ch);
      return FULLUPDATE;
/*
    }
*/
    break;
  case Ctrl('Q'):
     return my_query(headers[locmem->crs_ln - locmem->top_ln].owner);
  case 'Z':
    if (HAS_PERM(PERM_FORWARD))
    {
      char fname[80];

      setdirpath(fname, currdirect, &headers[locmem->crs_ln - locmem->top_ln]);
      z_download(fname);
      return FULLUPDATE;
    }
    break;

  case 'l':
      DL_func("bin/record_list.so:find_cb1");
      return FULLUPDATE;

  case 'o':
      DL_func("bin/record_list.so:find_cb2");
      return FULLUPDATE;

  case '\n':
  case '\r':
  case KEY_RIGHT:
    if (currmode & MODE_SELECT) {
       struct keeploc *locmem;
       int curpos, num;
       char buf[200];
       fileheader fh;

       get_record(currdirect, &fh, sizeof(fileheader),
          getkeep(currdirect, 1, 1)->crs_ln);
       sprintf(buf, "boards/%s/.DIR", currboard);
       curpos = getindex(buf, fh.filename);
       num =  curpos - p_lines + 1;
       locmem = getkeep(buf, 1, 1);
       locmem->top_ln = num < 1 ? 1 : num;
       locmem->crs_ln = curpos;
    }

    if (thread_title == 1) {
       ++thread_title;
       currmode &= ~MODE_SELECT;
       setbdir(currdirect, currboard);
       select_read(locmem,RS_TITLE);
       return NEWDIRECT;
    }
    ch = 'r';

  default:
    for (i = 0; rcmdlist[i].fptr; i++)
    {
      if (rcmdlist[i].key == ch)
      {
        mode = (*(rcmdlist[i].fptr)) (locmem->crs_ln,
          &headers[locmem->crs_ln - locmem->top_ln], currdirect);
        break;
      }
      if (rcmdlist[i].key == 'h')
      {
        if (currmode & MODE_ETC || currmode & MODE_DIGEST)
          return DONOTHING;
      }
    }
  }
  return mode;
}

i_read(cmdmode, direct, dotitle, doentry, rcmdlist, bhdr)
  char *direct;
  void (*dotitle) ();
  void *(*doentry) ();
  struct one_key *rcmdlist;
  boardheader *bhdr;
{
  keeploc *locmem;
  char lbuf[11];
  int recbase, mode, ch;
  int num, entries;
  int i;
  int jump = 0;
  char genbuf[4];
  char currdirect0[MAXPATHLEN];
  int last_line0 = last_line;
  int hit_thread0 = hit_thread;
  fileheader* headers0 = headers;
  static enter;

  if(enter > 3)
     return;
  ++enter;

  strcpy(currdirect0 ,currdirect);

#define FHSZ    sizeof(fileheader)

  headers = (fileheader *) calloc(p_lines, FHSZ);
  strcpy(currdirect, direct);
  mode = NEWDIRECT;

  if(bhdr) {
     char buf[64];
     FILE *fs;

     if(HAS_PERM(PERM_SYSOP)) goto SHOW_FHDR;
#ifndef NO_USE_MULTI_STATION
     if(is_station_sysop() && !strcmp(bhdr->station, cuser.station) ||
        is_BM(currBM+6))
        goto SHOW_FHDR;
#endif
     if(!(bhdr->level & PERM_ENTERSC)) goto SHOW_FHDR;
     if(!(cuser.userlevel & PERM_LOGINOK)) goto return_i_read;

     setbfile(buf, currboard, "permlist");
     if(!belong(buf, cuser.userid)) {
        if(bhdr->level & PERM_ENTERSCRQ) {
           FILE *fp;

           setbfile(buf, currboard, "ask_addperm");
           if((fp = fopen(buf, "r")) == NULL) {
              pressanykey("¥»ªOµL¶iªO«Å¥Ü ½Ð¼g«Hµ¹ªO¥D¥Ó½Ð¤JªO");
              goto return_i_read;
           }

           clear();
           more(buf, "YEA");
           eolrange(b_lines-3, b_lines);
           getdata(b_lines-1, 2, "¦¹¬°¥»ªO¤§¶iªO«Å¥Ü ½T©w¿í¦u y/N ? ",
                   buf, 2, LCECHO, 0);
           if(buf[0] == 'y') {
              setbfile(buf, currboard, "permlist");
              fs = fopen(buf, "a+");
              if(!fs) {
                 pressanykey("¨Ó»«¦W³æÀÉ¶}ÀÉ¥¢±Ñ ½Ð³sµ¸¯¸°È¤H­û");
                 goto return_i_read;
              }
              fprintf(fs, "%s\n", cuser.userid);
              fclose(fs);
           }
           else goto return_i_read;
        }
        else {
           pressanykey("¦¹ªO»Ý¦VªO¥D¥Ó½Ð¶iªO ½ÐÁpµ¸ªO¥D¥[¤JªO¤Í");
           goto return_i_read;
        }
     }
  }
SHOW_FHDR:
  do
  {
    /* -------------------------------------------------------- */
    /* ¨Ì¾Ú mode Åã¥Ü fileheader                                 */
    /* -------------------------------------------------------- */

    if (cmdmode)
       setutmpmode(cmdmode);

    switch (mode)
    {
    case NEWDIRECT:             /* ²Ä¤@¦¸¸ü¤J¦¹¥Ø¿ý */
    case DIRCHANGED:

      last_line = get_num_records(currdirect, FHSZ);

      if (mode == NEWDIRECT)
      {
        if (last_line == 0)
        {
          if (curredit & EDIT_MAIL)
          {
            outs("¨S¦³¨Ó«H");
            refresh();
            sleep(1);
            goto return_i_read;
          }
          else if (currmode & MODE_ETC)
          {
            board_etc(); /* Kaede */
            outmsg("©|¥¼¦¬¿ý¨ä¥¦¤å³¹");
            refresh();
            sleep(1);
          }
          else if (currmode & MODE_DIGEST)
          {
            board_digest(); /* Kaede */
            outmsg("©|¥¼¦¬¿ý¤åºK");
            refresh();
            sleep(1);
          }
          else if (currmode & MODE_SELECT)
          {
            board_select(); /* Leeym */
            outmsg("¨S¦³¦¹¨t¦Cªº¤å³¹");
            refresh();
            sleep(1);
          }
          else
          {
            getdata(b_lines - 1, 0, "¬ÝªO·s¦¨¥ß (P)µoªí¤å³¹ (Q)Â÷¶}¡H[Q] ",
              genbuf, 4, LCECHO, 0);
            if (genbuf[0] == 'p')
              do_post();
            goto return_i_read;
          }
        }
        num = last_line - p_lines + 1;
        locmem = getkeep(currdirect, num < 1 ? 1 : num, last_line);
      }
      recbase = -1;

    case FULLUPDATE:
      (*dotitle) ();

    case PARTUPDATE:
      if (last_line < locmem->top_ln + p_lines)
      {
        num = get_num_records(currdirect, FHSZ);
        if (last_line != num)
        {
          last_line = num;
          recbase = -1;
        }
      }

      if (last_line == 0)
         goto return_i_read;
      else if (recbase != locmem->top_ln)
      {
        recbase = locmem->top_ln;
        if (recbase > last_line)
        {
          recbase = last_line - p_lines >> 1;
          if (recbase < 1)
            recbase = 1;
          locmem->top_ln = recbase;
        }
        entries = get_records(currdirect, headers, FHSZ, recbase, p_lines);
      }
      if (locmem->crs_ln > last_line)
        locmem->crs_ln = last_line;

      move(3, 0);
      clrtobot();

    case PART_REDRAW:

      move(3, 0);
      for (i = 0; i < entries; i++)
        (*doentry) (locmem->top_ln + i, &headers[i]);

    case READ_REDRAW:
#ifdef USE_MULTI_TITLE
    {
      char buf[200];
      sprintf(buf, "[37;%2dm%s", title_color,
                   curredit & EDIT_MAIL ? msg_mailer : MSG_POSTER);
      outmsg(buf);

    }
#else
      outmsg(curredit & EDIT_MAIL ? msg_mailer : MSG_POSTER);
#endif
      break;

    /* woju */
    case READ_PREV:
    case READ_NEXT:
    case RELATE_PREV:
    case RELATE_NEXT:
    case RELATE_FIRST:
    case POS_NEXT:
    case 'A':
    case 'a':
    case '/':
    case '?':
      jump = 1;
      break;
    }

    /* -------------------------------------------------------- */
    /* Åª¨úÁä½L¡A¥[¥H³B²z¡A³]©w mode                             */
    /* -------------------------------------------------------- */

    if (!jump) {
       cursor_show(3 + locmem->crs_ln - locmem->top_ln, 0);
       ch = igetkey();
       mode = DONOTHING;
    }
    else
       ch = ' ';

    if (mode == POS_NEXT) {
       mode = cursor_pos(locmem, locmem->crs_ln + 1, 1);
       if (mode == DONOTHING)
          mode = PART_REDRAW;
       jump = 0;
    }
    else if (ch >= '0' && ch <= '9')
    {
      if ((i = search_num(ch, last_line)) != -1)
        mode = cursor_pos(locmem, i + 1, 10);
    }
    else
    {
      if (!jump)
         mode = i_read_key(rcmdlist, locmem, ch);

      while (mode == READ_NEXT || mode == READ_PREV ||
        mode == RELATE_FIRST || mode == RELATE_NEXT || mode == RELATE_PREV ||
        mode == THREAD_NEXT || mode == THREAD_PREV || mode == 'A' || mode == 'a' || mode == '/' || mode == '?')
      {
        int reload;

        if (mode == READ_NEXT || mode == READ_PREV)
        {
          reload = move_cursor_line(locmem, mode);
        }
        else
        {
          reload = thread(locmem, mode);
          if (!hit_thread)
          {
            mode = FULLUPDATE;
            break;
          }
        }

        if (reload == -1)
        {
          mode = FULLUPDATE;
          break;
        }
        else if (reload)
        {
          recbase = locmem->top_ln;
          entries = get_records(currdirect, headers, FHSZ, recbase, p_lines);
          if (entries <= 0)
          {
            last_line = -1;
            break;
          }
        }
        num = locmem->crs_ln - locmem->top_ln;
        if (headers[num].owner[0] != '-')
          mode = i_read_key(rcmdlist, locmem, ch);
      }
    }
  } while (mode != DOQUIT);

#undef  FHSZ
return_i_read:
   free(headers);
   last_line = last_line0;
   hit_thread = hit_thread0;
   headers = headers0;
   strcpy(currdirect ,currdirect0);
   --enter;
   return;
}
