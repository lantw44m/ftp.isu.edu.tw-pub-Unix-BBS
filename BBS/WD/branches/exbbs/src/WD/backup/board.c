/*-------------------------------------------------------*/
/* board.c      ( NTHU CS MapleBBS Ver 2.36 )            */
/*-------------------------------------------------------*/
/* target : 看板、群組功能                               */
/* create : 95/03/29                                     */
/* update : 95/12/15                                     */
/*-------------------------------------------------------*/


#include "bbs.h"


#define BRC_MAXSIZE     24576
#define BRC_MAXNUM      80
#define BRC_ITEMSIZE    (BRC_STRLEN + 1 + BRC_MAXNUM * sizeof( int ))

#define         MAX_FAVORITE    64      /* 限定最多可以有多少個我的最愛 */


int brc_size, brc_changed = 0;
int brc_list[BRC_MAXNUM], brc_num;

char brc_buf[BRC_MAXSIZE];
char brc_name[BRC_STRLEN];

static time_t brc_expire_time;


extern int numboards;
extern boardheader *bcache;

typedef struct
{
  int pos, total;
  char *name, *title, *BM; // desc[3][80];
  uschar unread, zap, bvote;
  usint brdattr;
}      boardstat;

boardstat *nbrd;


int *zapbuf;
int brdnum, yank_flag = 0;

char *boardprefix;

static char *str_local_board = "☆○◇◎△";  /* 代表 local board class */
static char *str_good_board = "☆★";  /* 代表 good board class */

/* ----------------------------------------------------- */
/* home/userid/.boardrc maintain                         */
/* ----------------------------------------------------- */

static char *fn_boardrc = ".boardrc";

static char *
brc_getrecord(ptr, name, pnum, list)
  char *ptr, *name;
  int *pnum, *list;
{
  int num;
  char *tmp;

  strncpy(name, ptr, BRC_STRLEN);
  ptr += BRC_STRLEN;
  num = (*ptr++) & 0xff;
  tmp = ptr + num * sizeof(int);
  if (num > BRC_MAXNUM)
    num = BRC_MAXNUM;
  *pnum = num;
  memcpy(list, ptr, num * sizeof(int));
  return tmp;
}


static char *
brc_putrecord(ptr, name, num, list)
  char *ptr, *name;
  int num, *list;
{
  if (num > 0 && list[0] > brc_expire_time)
  {
    if (num > BRC_MAXNUM)
      num = BRC_MAXNUM;

    while (num > 1 && list[num - 1] < brc_expire_time)
      num--;

    strncpy(ptr, name, BRC_STRLEN);
    ptr += BRC_STRLEN;
    *ptr++ = num;
    memcpy(ptr, list, num * sizeof(int));
    ptr += num * sizeof(int);
  }
  return ptr;
}


void
brc_update()
{
  if (brc_changed && cuser.userlevel)
  {
    char dirfile[STRLEN], *ptr;
    char tmp_buf[BRC_MAXSIZE - BRC_ITEMSIZE], *tmp;
    char tmp_name[BRC_STRLEN];
    int tmp_list[BRC_MAXNUM], tmp_num;
    int fd, tmp_size;

    ptr = brc_buf;
    if (brc_num > 0)
      ptr = brc_putrecord(ptr, brc_name, brc_num, brc_list);

    setuserfile(dirfile, fn_boardrc);
    if ((fd = open(dirfile, O_RDONLY)) != -1)
    {
      tmp_size = read(fd, tmp_buf, sizeof(tmp_buf));
      close(fd);
    }
    else
    {
      tmp_size = 0;
    }

    tmp = tmp_buf;
    while (tmp < &tmp_buf[tmp_size] && (*tmp >= ' ' && *tmp <= 'z'))
    {
      tmp = brc_getrecord(tmp, tmp_name, &tmp_num, tmp_list);
      if (strncmp(tmp_name, currboard, BRC_STRLEN))
        ptr = brc_putrecord(ptr, tmp_name, tmp_num, tmp_list);
    }
    brc_size = (int) (ptr - brc_buf);

    if ((fd = open(dirfile, O_WRONLY | O_CREAT, 0644)) != -1)
    {
      ftruncate(fd, 0);
      write(fd, brc_buf, brc_size);
      close(fd);
    }
    brc_changed = 0;
  }
}

void
read_brc_buf()
{
  char dirfile[STRLEN];
  int fd;

  if (brc_buf[0] == '\0')
  {
    setuserfile(dirfile, fn_boardrc);
    if ((fd = open(dirfile, O_RDONLY)) != -1)
    {
      brc_size = read(fd, brc_buf, sizeof(brc_buf));
      close(fd);
    }
    else
    {
      brc_size = 0;
    }
  }
}


int
brc_initial(boardname)
  char *boardname;
{
  char *ptr;

  if (strcmp(currboard, boardname) == 0)
  {
    return brc_num;
  }
  brc_update();
  strcpy(currboard, boardname);
  currbrdattr = bcache[getbnum(currboard)-1].brdattr;
  read_brc_buf();

  ptr = brc_buf;
  while (ptr < &brc_buf[brc_size] && (*ptr >= ' ' && *ptr <= 'z'))
  {
    ptr = brc_getrecord(ptr, brc_name, &brc_num, brc_list);
    if (strncmp(brc_name, currboard, BRC_STRLEN) == 0)
      return brc_num;
  }
  strncpy(brc_name, boardname, BRC_STRLEN);
  brc_num = brc_list[0] = 1;
  return 0;
}


void
brc_addlist(fname)
  char *fname;
{
  int ftime, n, i;

  if (!cuser.userlevel)
    return;

  ftime = atoi(&fname[2]);
  if (ftime <= brc_expire_time
     /* || fname[0] != 'M' || fname[1] != '.' */ )
  {
    return;
  }
  if (brc_num <= 0)
  {
    brc_list[brc_num++] = ftime;
    brc_changed = 1;
    return;
  }
  if ((brc_num == 1) && (ftime < brc_list[0]))
    return;
  for (n = 0; n < brc_num; n++)
  {
    if (ftime == brc_list[n])
    {
      return;
    }
    else if (ftime > brc_list[n])
    {
      if (brc_num < BRC_MAXNUM)
        brc_num++;
      for (i = brc_num - 1; --i >= n; brc_list[i + 1] = brc_list[i]);
      brc_list[n] = ftime;
      brc_changed = 1;
      return;
    }
  }
  if (brc_num < BRC_MAXNUM)
  {
    brc_list[brc_num++] = ftime;
    brc_changed = 1;
  }
}


int
brc_unread(fname)
  char *fname;
{
  int ftime, n;

  ftime = atoi(&fname[2]);
  if (ftime <= brc_expire_time
     /* || fname[0] != 'M' || fname[1] != '.' */ )
    return 0;
  if (brc_num <= 0)
    return 1;
  for (n = 0; n < brc_num; n++)
  {
    if (ftime > brc_list[n])
      return 1;
    else if (ftime == brc_list[n])
      return 0;
  }
  return 0;
}


/* ----------------------------------------------------- */
/* .bbsrc processing                                     */
/* ----------------------------------------------------- */

char *str_bbsrc = ".bbsrc";

static void
load_zapbuf()
{
  register int n, size;
  char fname[60];

  /* MAXBOARDS ==> 至多看得見 4 個新板 */

  n = numboards + 4;
  size = n * sizeof(int);
  zapbuf = (int *) malloc(size);
  while (n)
    zapbuf[--n] = login_start_time;
  setuserfile(fname, str_bbsrc);
  if ((n = open(fname, O_RDONLY, 0600)) != -1)
  {
    read(n, zapbuf, size);
    close(n);
  }
  if (!nbrd)
    nbrd = (boardstat *) malloc(MAXBOARD * sizeof(boardstat));
  brc_expire_time = login_start_time - 365 * 86400;
}


static void
save_zapbuf()
{
  register int fd, size;
  char fname[60];

  setuserfile(fname, str_bbsrc);
  if ((fd = open(fname, O_WRONLY | O_CREAT, 0600)) != -1)
  {
    size = numboards * sizeof(int);
    write(fd, zapbuf, size);
    close(fd);
  }
}

/*
woju
Ref: bbs.c: brdperm(char* brdname, char* userid)
*/

int
Ben_Perm(bptr)
  boardheader *bptr;
{
  register int level,brdattr;
  register char *ptr;
  char buf[64];

  level = bptr->level;
  brdattr = bptr->brdattr;

  if (HAS_PERM(PERM_SYSOP))
    return 1;

  ptr = bptr->BM;
  if (is_BM(ptr))
    return 1;

  /* 祕密看板 */

  setbfile(buf, bptr->brdname, FN_LIST);
  if (brdattr & BRD_HIDE) /* 私人/隱藏 */
  {
    if (!belong_list(buf, cuser.userid))
    {
      if(brdattr & BRD_POSTMASK)  /* 隱藏 */
        return 0;
      else
        return 2;
    }
    else return 1;
  }
  
  
                                         /* 限制閱讀權限 */
  if (level && !(brdattr & BRD_POSTMASK) && !HAS_PERM(level))
    return 0;

    return 1;

}


static void
load_boards(char *bname , usint mode)
{
  boardheader *bptr;
  boardstat *ptr;
  char brdclass[5];
  int n;
  register char state;

  resolve_boards();
  if (!zapbuf)
    load_zapbuf();

  brdnum = 0;
  for (n = 0; n < numboards; n++)
  {
    bptr = &bcache[n];
    if (bptr->brdname[0] == '\0')
      continue;
    if (bname)
    {
      if(strcmp(bname,bptr->brdname)) continue;
      ptr = &nbrd[brdnum++];
      ptr->name = bptr->brdname;
      ptr->title = bptr->title;
      ptr->BM = bptr->BM;
      //strcpy(ptr->desc[0], bptr->desc[0]);
      //strcpy(ptr->desc[1], bptr->desc[1]);
      //strcpy(ptr->desc[2], bptr->desc[2]);
      ptr->pos = n;
      ptr->total = -1;
      ptr->bvote = bptr->bvote;
      ptr->zap = (zapbuf[n] == 0);
      return;
    }

    if(mode)
    {
      if(!(bptr->brdattr & mode))
        continue;
    }
    else if (boardprefix)
    {
      if (boardprefix == str_local_board || boardprefix == str_good_board)
      {
        strncpy(brdclass, bptr->title + 5, 2);
        brdclass[2] = '\0';
      }
      else
      {
        strncpy(brdclass, bptr->title, 4);
        brdclass[4] = '\0';
      }
      if (strstr(boardprefix, brdclass) == NULL)
        continue;
    }
    else if (currmode & MODE_FAVORITE) 
    {
      char fpath[80];
      if (currmode & MODE_FAVORITE)
        setuserfile(fpath, fn_myfavorite);
      if (!belong(fpath, bptr->brdname)) 
        continue;
    }
    else if(bptr->brdattr & BRD_GROUPBOARD || bptr->brdattr & BRD_CLASS)
      continue;
    if ((state = Ben_Perm(bptr)) && (yank_flag == 1 || (yank_flag == 2 &&
         ((bptr->brdattr & BRD_GROUPBOARD || bptr->brdattr & BRD_CLASS || have_author(bptr->brdname))))
         || yank_flag != 2 && zapbuf[n]))
    {
      ptr = &nbrd[brdnum++];
      ptr->total = -1;
/*      ptr->lastposttime = &(brdshm->lastposttime[n]); */
      ptr->name = bptr->brdname;
      ptr->title = bptr->title;
      ptr->BM = bptr->BM;
      //strcpy(ptr->desc[0], bptr->desc[0]);
      //strcpy(ptr->desc[1], bptr->desc[1]);
      //strcpy(ptr->desc[2], bptr->desc[2]);
      ptr->pos = n;
      ptr->bvote = bptr->bvote;
      ptr->zap = (zapbuf[n] == 0);
      ptr->brdattr =  bptr->brdattr;

      if((bptr->brdattr & BRD_HIDE) && state == 1)
              ptr->brdattr |= BRD_POSTMASK;
/*      check_newpost(ptr);*/
    }
  }

  /* 如果 user 將所有 boards 都 zap 掉了 */

  if (!brdnum && !boardprefix)
     if (yank_flag == 0)
        yank_flag = 1;
     else if (yank_flag == 2)
        yank_flag = 0;
}


static int
search_board(num)
{
  char genbuf[IDLEN + 2];
/* Ptt get out
  if (num >= 0) {
     getdata(b_lines - 1, 0, "請輸入看板名稱：", genbuf, IDLEN + 1, DOECHO,0);
     move(b_lines - 1, 0);
     clrtoeol();
     move(b_lines, 0);
     if (genbuf[0])
     {
       int n = num + 1;
       str_lower(genbuf, genbuf);
       while (n != num)
       {
         if (++n >= brdnum)        n = 0;
         if (strstr_lower(nbrd[n].name, genbuf))
           return n;
         if (++n >= brdnum)
           n = 0;
       }
     }
  }
  else {
*/
     move(b_lines-1,0);
     clrtoeol();
     CreateNameList();
     for (num = 0; num < brdnum; num++)
        AddNameList(nbrd[num].name);
     namecomplete("請輸入看板名稱：", genbuf);
     for (num = 0; num < brdnum; num++)
        if (!strcmp(nbrd[num].name, genbuf))
           return num;
/*
  }
*/
  return -1;
}


int
check_newpost(ptr)
  boardstat *ptr;
{
  fileheader fh;
  struct stat st;
  char fname[FNLEN];
  char genbuf[200];
  int fd, total;

  ptr->total = ptr->unread = 0;
  setbdir(genbuf, ptr->name);
  if ((fd = open(genbuf, O_RDWR)) < 0)
    return 0;
  fstat(fd, &st);
  total = st.st_size / sizeof(fh);
  if (total <= 0)
  {
    close(fd);
    return 0;
  }
  ptr->total = total;
  if (!brc_initial(ptr->name))
  {
    ptr->unread = 1;
  }
  else
  {
    lseek(fd, (off_t)((total - 1) * sizeof(fh)), SEEK_SET);
    if (read(fd, fname, FNLEN) > 0 && brc_unread(fname))
    {
      ptr->unread = 1;
    }
  }
  close(fd);
  return 1;
}


static int
unread_position(dirfile, ptr)
  char *dirfile;
  boardstat *ptr;
{
  fileheader fh;
  char fname[FNLEN];
  register int num, fd, step, total;

  total = ptr->total;
  num = total + 1;
  if (ptr->unread && (fd = open(dirfile, O_RDWR)) > 0)
  {
    if (!brc_initial(ptr->name))
    {
      num = 1;
    }
    else
    {
      num = total - 1;
      step = 4;
      while (num > 0)
      {
        lseek(fd, (off_t)(num * sizeof(fh)), SEEK_SET);
        if (read(fd, fname, FNLEN) <= 0 || !brc_unread(fname))
          break;
        num -= step;
        if (step < 32)
          step += step >> 1;
      }
      if (num < 0)
        num = 0;
      while (num < total)
      {
        lseek(fd, (off_t)(num * sizeof(fh)), SEEK_SET);
        if (read(fd, fname, FNLEN) <= 0 || brc_unread(fname))
          break;
        num++;
      }
    }
    close(fd);
  }
  if (num < 0)
    num = 0;
  return num;
}


static void
brdlist_foot()
{
  prints(COLOR2"  選擇看板  "COLOR1"[1m  [33m(v/V)[37m標記已讀/未讀  "
    "[33m(y)[33m列出[37m%s  [33m(z)[37m取消/訂閱\     [44m %13s [m",
    yank_flag ? "訂閱\" : "全部", cuser.userid);
}
/*
woju
*/
have_author(char* brdname)
{
   char dirname[100];
   extern cmpfowner();

   sprintf(dirname, "正在搜尋作者[33m%s[m 看板:[1;33m%s[0m.....",
           currauthor,brdname);
   move(b_lines, 0);
   clrtoeol();
   outs(dirname);
   refresh();
   setbdir(dirname, brdname);
   str_lower(currowner, currauthor);
   return search_rec(dirname, cmpfowner);
}

static void
show_brdone(head, newflag, bar, row)
  int head, newflag, bar, row;
{
  char buf[128], outmsg[256];
  boardstat *ptr;
  static int color, ci=0;
  int mycolor ;
  static char *bcolor[6]={"[1;32m","[1;33m",
                          "[1;37m","[1;36m",
                          "[1;35m","[1;37m"};
  static char *unread[2]={"◇","◆"};
  
  ptr = &nbrd[head];
  if (ptr->total == -1)
     check_newpost(ptr);
     
  if(bar==1)
    sprintf(outmsg, "●[1;33;44m");
  else
    sprintf(outmsg, "  [1;37;40m");
     
  if (yank_flag == 2)
     sprintf(buf,"%4d %c%c",
             head,ptr->brdattr & BRD_HIDE ? ')' : ' ',
             (ptr->brdattr & BRD_GROUPBOARD || ptr->brdattr & BRD_CLASS) ? ' ' :
             'A');

  else if (!newflag)
     sprintf(buf,"%4d %c%s ", head,
                 !(ptr->brdattr & BRD_HIDE) ? ' ':
                 (ptr->brdattr & BRD_POSTMASK) ? ')' : '-',
                 ptr->zap ? "--" :
                 (ptr->brdattr & BRD_GROUPBOARD) ? "Σ" :
                 (ptr->brdattr & BRD_CLASS) ? "□" :
                 unread[ptr->unread]);

  else if (ptr->zap)
  {
     sprintf(buf, " ------");
  }
  else
  {
     if (ptr->total == -1)
        check_newpost(ptr);
     if (newflag)
     {
       if(ptr->brdattr & BRD_GROUPBOARD)
         sprintf(buf,"     [33mΣ ");
       else if(ptr->brdattr & BRD_CLASS)
         sprintf(buf,"     [33mΣ ");
       else
         sprintf(buf,"%4d %s ", (ptr->total),
                      ptr->zap ? "--" :
                      unread[ptr->unread]);
      }
  }
  strcat(outmsg, buf);
  
  sprintf(buf,"[37m%-13s[m",ptr->name);
  strcat(outmsg, buf);
  
  mycolor = (unsigned int)(ptr->title[0]+ptr->title[1]+ptr->title[2]);
//  if(mycolor!=color) { ci=(ci+1)%6; color=mycolor; }
//  ci=mycolor % 6;  
  ci=2;

  sprintf(buf,"%s%5.5s[m%-2.2s %-32.32s%s %-12.12s",
          bcolor[ci],
          ptr->title,ptr->title+5,ptr->title+7, 
          (ptr->bvote == 1 ? "[1;33m有[m" : 
          ptr->bvote == 2 ? "[1;37m開[m" : "  "),
          ptr->BM);
  strcat(outmsg, buf);
  outmsgline(outmsg, row);
}

static void
show_brdlist(head, clsflag, newflag)
{
  int myrow;
  
  if (clsflag)
  {
    showtitle("看板列表", BoardName);
    prints("[←]主選單 [→]閱\讀 [↑↓]選擇 [y]載入 [S]排序 [/]搜尋 [TAB]文摘•看板 [^Z]求助\n"
      COLOR1"[1m%-22s 類別 轉 %-32s投 板    主     [m",
      newflag ? "  總數 讀 看  板  " : "  編號  看  板", "中 文 敘 述");
    move(b_lines, 0);
    brdlist_foot();
  }

  if (brdnum > 0)
  {
    myrow = 2;
    while (++myrow < b_lines)
    {
      move(myrow, 0);
      if (head < brdnum)
      {
        show_brdone(head++,newflag,0, myrow);
      }
      clrtoeol();
    }
  }
}

/* 根據 title 或 name 做 sort */

static
int
cmpboard(brd, tmp)
  boardstat *brd, *tmp;
{
  register int type = 0;

  if (!type)
   {
    type = strncmp(brd->title, tmp->title, 4);
    type *= 256;
    type += strcasecmp(brd->name, tmp->name);
   }
  if (!(cuser.uflag & BRDSORT_FLAG))
    type = strcasecmp(brd->name, tmp->name);
  return type;
}

static void
set_menu_BM(char *BM)
{
  if (HAS_PERM(PERM_ALLBOARD) || is_BM(BM))
  {
    currmode |= MODE_MENU;
  }
}

static void
choose_board(int newflag, usint mode)
{
  static int num = 0;
  int attmode = 1;
  boardstat *ptr;
  int head, ch, tmp,tmp1;
  char genbuf[200],*prefixtmp;
  extern time_t board_visit_time;
  extern int m_newbrd(),m_mod_board(char *bname), m_mod_expire(char *bname);

  setutmpmode(newflag ? READNEW : READBRD);
  brdnum = 0;
  if (!cuser.userlevel)         /* guest yank all boards */
    yank_flag = 1;

  do
  {
    if (brdnum <= 0)
    {
      load_boards(NULL,mode);
      if (brdnum <= 0)
        break;
      qsort(nbrd, brdnum, sizeof(boardstat), cmpboard);
      head = -1;
    }

    if (num < 0)
      num = 0;
    else if (num >= brdnum)
      num = brdnum - 1;

    if (head < 0)
    {
      if (newflag)    
      {
        tmp = num;
        while (num < brdnum)
        {
          ptr = &nbrd[num];
          if (ptr->total == -1)
            check_newpost(ptr);
          if (ptr->unread)
            break;
          num++;
        }
        if (num >= brdnum)
          num = tmp;
      }
      head = (num / p_lines) * p_lines;
      show_brdlist(head, 1, newflag);
    } 
    else if (num < head || num >= head + p_lines)
    {
      head = (num / p_lines) * p_lines;
      show_brdlist(head, 0, newflag);
    }
/*
    clrchyiuan(b_lines-5,b_lines-1);
    move(b_lines-5,0);
    prints("[1;36m%s[m\n", msg_seperator);
    prints(" [1;33m%s [m板的看板說明 :\n%-80.80s\n%-80.80s\n%-80.80s",
    nbrd[num].name,nbrd[num].desc[0],nbrd[num].desc[1],nbrd[num].desc[2]);
    {
      int c;
      
      c = page_lines - (num%page_lines);
      if(num+c < brdnum && nbrd[num+c].name)
      {
        move(b_lines-5,69);
        prints("[還有下頁唷！]");
      }
    }
*/

    if(HAS_HABIT(HABIT_LIGHTBAR))
    {
      show_brdone(num, newflag, 1, 3+num-head);
      move(b_lines,0);
      ch = egetch();
      show_brdone(num, newflag, 0, 3+num-head);
      move(b_lines,0);
    }
    else
      ch = cursor_key(3+num-head,0);
    
    switch (ch)
    {
    case 'e':
    case KEY_LEFT:
    case EOF:
      ch = 'q';
    case 'q':
      break;

    case 'c':
      if (yank_flag == 2) {
         newflag = yank_flag = 0;
         brdnum = -1;
      }
      show_brdlist(head, 1, newflag ^= 1);
      break;

// wildcat : show board attr in list
     case 'A':
           if(!HAS_PERM(PERM_SYSOP))
              break;
           if(attmode >= 2) attmode = 1;
           else attmode = 2;
           show_brdlist(head, attmode, newflag);
           break;

    case 'a': {
      if (yank_flag != 2 ) {
         sprintf(genbuf, "%s", currauthor);
         if (getdata(1, 0,"作者:", genbuf, IDLEN + 2, DOECHO,currauthor))
            strncpy(currauthor, genbuf, IDLEN + 2);
         if (*currauthor)
            yank_flag = 2;
         else
            yank_flag= 0;
      }
      else
         yank_flag = 0;
      brdnum = -1;
      show_brdlist(head, 1, newflag);
      break;
    }
    
    case 'f':
    case 'F':
    {
      char fpath[256],msg[80];
      ptr = &nbrd[num];
      brc_initial(ptr->name);
      setuserfile(fpath,fn_myfavorite);
      if (ch == 'f' )
      {
        if(file_list_count(fpath) > MAX_FAVORITE)
          pressanykey("個人喜愛看版已達最大量..^_^");
        else
        {
          idlist_add(fpath, NULL,ptr->name);
          sprintf(msg, "[%s]版已經加入..^O^", ptr->name);
          pressanykey(msg);
        }
        brdnum = -1;
      }
      else if (ch =='F')
      {
        idlist_delete(fpath,ptr->name);
        sprintf(msg,"[%s]版已經移除了..:(", ptr->name);
        pressanykey(msg);
        brdnum = -1;
      }
      break;
    }
    case KEY_PGUP:
    case 'P':
    case 'b':
    case Ctrl('B'):
      if (num)
      {
        num -= p_lines;
        break;
      }

    case KEY_END:
    case '$':
      num = brdnum - 1;
      break;

    case ' ':
    case KEY_PGDN:
    case 'N':
    case Ctrl('F'):
      if (num == brdnum - 1)
        num = 0;
      else
        num += p_lines;
      break;

    case KEY_ESC: if (KEY_ESC_arg == 'n') {
       edit_note();
       show_brdlist(head, 1, newflag);
       }
       break;

    case KEY_UP:
    case 'p':
    case 'k':
      if (num-- <= 0)
        num = brdnum - 1;
      break;

    case KEY_DOWN:
    case 'n':
    case 'j':
      if (++num < brdnum)
        break;

    case '0':
    case KEY_HOME:
      num = 0;
      break;

    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      if ((tmp = search_num(ch, brdnum)) >= 0)
        num = tmp;
      brdlist_foot();
      break;

    case Ctrl('A'):
       Announce();
       show_brdlist(head, 1, newflag);
       break;

    case '/':
      if ((tmp = search_board(num)) >= 0)
        num = tmp;
      show_brdlist(head, 1, newflag);
      break;

    case 'S':
      cuser.uflag ^= BRDSORT_FLAG;
      substitute_record(fn_passwd, &cuser, sizeof(userec), usernum); /* 記錄 */
      move(3,0);
      clrtobot();
      qsort(nbrd, brdnum, sizeof(boardstat), cmpboard);
      head = 999;
      move(b_lines,0);
      brdlist_foot();
      break;

    case 'y':
      if (yank_flag == 2)
         yank_flag = 0;
      else
         yank_flag ^= 1;
      brdnum = -1;
      break;

    case 'z':                   /* opus: no perm check ? */
      if (HAS_PERM(PERM_BASIC))
      {
        ptr = &nbrd[num];
        ptr->zap = !ptr->zap;
        if(ptr->brdattr & BRD_NOZAP) ptr->zap = 0;
        if(!ptr->zap) check_newpost(ptr);
        zapbuf[ptr->pos] = (ptr->zap ? 0 : login_start_time);
        head = 999;
      }
      break;

    case 'v':
    case 'V':
      ptr = &nbrd[num];
      brc_initial(ptr->name);
      if (ch == 'v')
      {
        ptr->unread = 0;
        zapbuf[ptr->pos] = time((time_t *) &brc_list[0]);
      }
      else
        zapbuf[ptr->pos] = brc_list[0] = ptr->unread = 1;
      brc_num = brc_changed = 1;
      brc_update();
      show_brdlist(head, 0, newflag);
      break;

    case 's':
       if ((tmp = search_board(-1)) < 0) 
          show_brdlist(head, 1, newflag);
       num = tmp;
       break;

    case 'Q':
       if (HAS_PERM(PERM_BOARD) || (currmode & MODE_MENU)) 
       {
           ptr = &nbrd[num];
//           move(1,1);
//           clrtobot();
           m_mod_board(ptr->name);
           brdnum = -1;
        }
        break;

    case 'B':
        if (HAS_PERM(PERM_BOARD) || (currmode & MODE_MENU)) {
           m_newbrd();
           brdnum = -1;
        }
        break;
    case 'E':
        if(HAS_PERM(PERM_BOARD))
        {
           ptr = &nbrd[num];
           m_mod_expire(ptr->name);
           brdnum = -1;
        }
        break;
                                                                                                                                       break;
    case KEY_RIGHT:
    case '\n':
    case '\r':
    case 'r':
      {
        char buf[STRLEN];

        ptr = &nbrd[num];

        if (!(ptr->brdattr & BRD_GROUPBOARD || ptr->brdattr & BRD_CLASS))     /* 非sub class */
        {

/*
   wildcat 000121 : 只要判斷私人板就好 , 隱藏板看的到的就看的到 , 看不到的
		    還是看不到 :Q , 本來有段奇怪的 FN_APPLICATION 就拿掉吧
		    可是怎麼覺得是在重複 Ben_Perm 做的判斷?
*/
//          if((ptr->brdattr & BRD_HIDE || !(ptr->brdattr & BRD_POSTMASK))
          if((ptr->brdattr & BRD_HIDE) &&
             (!HAS_PERM(PERM_SYSOP) && !is_BM(ptr->BM) ) )
          {
            setbfile(buf, ptr->name, FN_LIST);               
            if(!belong_list(buf,cuser.userid))
            {
//              pressanykey(P_BOARD);
              pressanykey("對不起，此板只允許\ 看板好友 進入哦!");
              break;
            }
          }

          brc_initial(ptr->name);

          if (yank_flag == 2)
          {
            setbdir(buf, currboard);
            tmp = have_author(currboard) - 1;
            head = tmp - t_lines / 2;
            getkeep(buf, head > 1 ? head : 1, -(tmp + 1));
          }
          else if (newflag)
          {
            setbdir(buf, currboard);
            tmp = unread_position(buf, ptr);
            head = tmp - t_lines / 2;
            getkeep(buf, head > 1 ? head : 1, tmp + 1);
          }
          board_visit_time = zapbuf[ptr->pos];
          if (!ptr->zap)
            time((time_t *) &zapbuf[ptr->pos]);
          Read();
          check_newpost(ptr);
          head = -1;
          setutmpmode(newflag ? READNEW : READBRD);
        }
        else                                   /* sub class */
        {
            prefixtmp = boardprefix;
            tmp1=num; num=0;
            boardprefix = ptr->title+7;
            set_menu_BM(ptr->BM);
            log_board2(ptr->name,0);
            if(!strcmp(ptr->name,"Personal_All"))
              choose_board(cuser.habit & HABIT_BOARDLIST,BRD_PERSONAL);
            else if(!strcmp(ptr->name,"Hide_All"))
              choose_board(cuser.habit & HABIT_BOARDLIST,BRD_HIDE);
            else
              choose_board(cuser.habit & HABIT_BOARDLIST,0);

            currmode &= ~MODE_MENU;
            num=tmp1;
            boardprefix = prefixtmp;
            brdnum = -1;

         }
      }
    }
  } while (ch != 'q');
  save_zapbuf();
}


int
board()
{
  choose_board(cuser.habit & HABIT_BOARDLIST,0);
  return 0;
}


int
local_board()
{
  boardprefix = str_local_board;
  choose_board(cuser.habit & HABIT_BOARDLIST,0);
  return 0;
}

int
good_board()
{
  boardprefix = str_good_board;
  choose_board(cuser.habit & HABIT_BOARDLIST,0);
  return 0;
}

int
Boards()
{
  boardprefix = NULL;
  choose_board(cuser.habit & HABIT_BOARDLIST,0);
  return 0;
}


int
New()
{
  int mode0 = currutmp->mode;
  int stat0 = currstat;

  boardprefix = NULL;
  choose_board(cuser.habit & HABIT_BOARDLIST,0);
  currutmp->mode = mode0;
  currstat = stat0;
  return 0;
}

int
Favor()
{
  int mode0 = currutmp->mode;
  int stat0 = currstat;
  char fpath[80];
  FILE *fp;
  boardprefix = NULL;
  currmode |= MODE_FAVORITE;

  setuserfile(fpath, fn_myfavorite);
  if ((fp = fopen(fpath, "r")) == NULL) return;
  else fclose(fp);

  choose_board(cuser.habit & HABIT_BOARDLIST,0);

  currmode &= ~MODE_FAVORITE;
  currutmp->mode = mode0;
  currstat = stat0;
  return 0;
}



int
favor_edit() {
  char fpath[80];
  int count, column, dirty;
  FILE *fp;
  char genbuf[200];
  char bn[60];

  if (brdnum <= 0)
  {
    load_boards(NULL,0);
    qsort(nbrd, brdnum, sizeof(boardstat), cmpboard);
  }

  setuserfile(fpath, fn_myfavorite);
  move(0, 0);

  dirty = 0;
  while (1)
  {
    stand_title("我的最愛");

    count = 0;
    CreateNameList();

    if (fp = fopen(fpath, "r"))
    {
      move(3, 0);
      column = 0;
      while (fgets(genbuf, STRLEN, fp))
      {
        if (genbuf[0] <= ' ')
          continue;
        strtok(genbuf, str_space);
        if (count < MAX_FAVORITE) {
          AddNameList(genbuf);
          prints("%-13s", genbuf);
          count++;
          if (++column > 5) {
            column = 0;
            outc('\n');
          }
        }
      }
      fclose(fp);
    }
    getdata(1, 0, (count ? "(A)增加 (D)刪除 (Q)結束？[Q] " :
        "(A)增加 (Q)結束？[Q] "), bn, 4, LCECHO, 0);
    if (*bn == 'a') {
      move(1, 0);
      
      brdcomplete(MSG_MY_FAVORITE, bn);
      //wordcomplete(MSG_MY_FAVORITE, bn, 1);
      
      if (count < MAX_FAVORITE) {
        if (bn[0] && !InNameList(bn)) {
          idlist_add(fpath, NULL, bn);
          dirty = 1;
        }
      }
    }
    else if ((*bn == 'd') && count)
    {
      move(1, 0);
      bn[0] = 0;
      namecomplete(MSG_MY_FAVORITE, bn);
      if (bn[0] && InNameList(bn))
      {
        idlist_delete(fpath, bn);
        dirty = 1;
      }
    }
    else
      break;
  }

  return 0;
}

extern struct BCACHE *brdshm;

char *
b_namearray(buf, pnum, tag)
  char buf[][IDLEN + 1], *tag;
  int *pnum;
{
  register struct BCACHE *reg_bshm = brdshm;
  register char *ptr, tmp;
  register int n, total;
  char tagbuf[STRLEN];
  int ch, ch2, num;

  resolve_boards();
  total = reg_bshm->number;
  if (*tag == '\0')
  {
    for (n = num = 0; n < total; n++)
    {
      if (!Ben_Perm(&reg_bshm->bcache[n])) continue;
      ptr = reg_bshm->bcache[n].brdname;
      tmp = *ptr;
      strcpy(buf[num++], ptr);
    }
    *pnum = num;
    *pnum = num;
    return buf[0];
  }
  for (n = 0; tag[n]; n++)
  {
    tagbuf[n] = chartoupper(tag[n]);
  }
  tagbuf[n] = '\0';
  ch = tagbuf[0];
  ch2 = ch - 'A' + 'a';
  for (n = num = 0; n < total; n++)
  {
    if (!Ben_Perm(&reg_bshm->bcache[n])) continue;
    ptr = reg_bshm->bcache[n].brdname;
    tmp = *ptr;
    if (tmp == ch || tmp == ch2)
    {
      if (chkstr(tag, tagbuf, ptr))
        strcpy(buf[num++], ptr);
    }
  }
  *pnum = num;
  return buf[0];
}

void force_board(char *bname)
{
  boardstat *ptr;
  char buf[80];

  setbpath(buf ,bname);
  if(!dashd(buf)) return;
  brdnum = 0;
  load_boards(bname,0);
  ptr = &nbrd[0];
  check_newpost(ptr);
  while(ptr->unread && cuser.userlevel) /* guest skip force read */
  {
    char buf[80];
    sprintf(buf," %s 版有新文章! 請閱\讀完新文章後再離開.. ^^",bname);
    pressanykey(buf);
    brc_initial(ptr->name);
    Read();
    check_newpost(ptr);
  }
}

void voteboard()
{
  boardstat *ptr;
  char buf[80];

  setbpath(buf ,VOTEBOARD);
  if(!dashd(buf)) return;
  brdnum = 0;
  load_boards(VOTEBOARD,0);
  ptr = &nbrd[0];
  brc_initial(ptr->name);
  Read();
}
