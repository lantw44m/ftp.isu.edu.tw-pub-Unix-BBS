/*-------------------------------------------------------*/
/* name.c       ( NTHU CS MapleBBS Ver 2.36 )            */
/*-------------------------------------------------------*/
/* target : name-complete routines                       */
/* create : 95/03/29                                     */
/* update : 02/08/03 (Dopin)                             */
/*-------------------------------------------------------*/

#include "bbs.h"

struct word *toplev = NULL;
static struct word *current = NULL;
static char *msg_more = "[7m-- More --[0m";

static void FreeNameList() {
  struct word *p, *temp;

  for (p = toplev; p; p = temp)
  {
    temp = p->next;
    free(p->word);
    free(p);
  }
}


void
CreateNameList()
{
  if (toplev)
    FreeNameList();
  toplev = current = NULL;
}


void
AddNameList(name)
  char *name;
{
  struct word *node;

  node = (struct word *) malloc(sizeof(struct word));
  node->next = NULL;
  node->word = (char *) malloc(strlen(name) + 1);
  strcpy(node->word, name);

  if (toplev)
    current = current->next = node;
  else
    current = toplev = node;
}


int
RemoveNameList(name)
  char *name;
{
  struct word *curr, *prev = NULL;

  for (curr = toplev; curr; curr = curr->next)
  {
    if (!strcmp(curr->word, name))
    {
      if (prev == NULL)
        toplev = curr->next;
      else
        prev->next = curr->next;

      if (curr == current)
        current = prev;
      free(curr->word);
      free(curr);
      return 1;
    }
    prev = curr;
  }
  return 0;
}


int
InNameList(name)
  char *name;
{
  struct word *p;

  for (p = toplev; p; p = p->next)
  {
    if (!strcmp(p->word, name))
      return 1;
  }
  return 0;
}


CountNameList()
{
  struct word *p;
  int ans = 0;

  for (p = toplev; p; p = p->next)
     ++ans;
  return ans;
}


void
ShowNameList(row, column, prompt)
  int row, column;
  char *prompt;
{
  struct word *p;

  move(row, column);
  clrtobot();
  outs(prompt);

  column = 80;
  for (p = toplev; p; p = p->next)
  {
    row = strlen(p->word) + 1;
    if (column + row > 76)
    {
      column = row;
      outc('\n');
    }
    else
    {
      column += row;
      outc(' ');
    }
    outs(p->word);
  }
}


void
LoadNameList(reciper, listfile, msg)
  int *reciper;
  char *listfile, *msg;
{
  FILE *fp;
  char genbuf[200];

  if (fp = fopen(listfile, "r"))
  {
    while (fgets(genbuf, STRLEN, fp))
    {
      strtok(genbuf, str_space);
      if (!InNameList(genbuf))
      {
        AddNameList(genbuf);
        (*reciper)++;
      }
    }
    fclose(fp);
    ShowNameList(3, 0, msg);
  }
}


/*
woju
*/
void
ToggleNameList(reciper, listfile, msg)
  int *reciper;
  char *listfile, *msg;
{
  FILE *fp;
  char genbuf[200];

  if (fp = fopen(listfile, "r"))
  {
    while (fgets(genbuf, STRLEN, fp))
    {
      strtok(genbuf, str_space);
      if (!InNameList(genbuf))
      {
        AddNameList(genbuf);
        (*reciper)++;
      }
      else {
        RemoveNameList(genbuf);
        (*reciper)--;
      }
    }
    fclose(fp);
    ShowNameList(3, 0, msg);
  }
}

void
SaveNameList(listfile)
  char *listfile;
{
  FILE *fp;
  struct word *p;

  if (fp = fopen(listfile, "w"))
  {
    for (p = toplev; p; p = p->next)
      fprintf(fp, "%s\n", p->word);
    fclose(fp);
  }
}


static int
NumInList(list)
  register struct word *list;
{
  register int i;

  for (i = 0; list; i++)
    list = list->next;
  return i;
}


#if 0
void
ApplyToNameList(fptr)
  int ((*fptr) ());
{
  struct word *p;

  for (p = toplev; p; p = p->next)
    (*fptr) (p->word);
}
#endif


int
chkstr(otag, tag, name)
  char *otag, *tag, *name;
{
  char ch, *oname = name;

  while (*tag)
  {
    ch = *name++;
    if (*tag != chartoupper(ch))
      return 0;
    tag++;
  }
  if (*tag && *name == '\0')
    strcpy(otag, oname);
  return 1;
}


static struct word *
GetSubList(tag, list)
  register char *tag;
  register struct word *list;
{
  struct word *wlist, *wcurr;
  char tagbuf[STRLEN];
  int n;

  wlist = wcurr = NULL;
  for (n = 0; tag[n]; n++)
  {
    tagbuf[n] = chartoupper(tag[n]);
  }
  tagbuf[n] = '\0';

  while (list)
  {
    if (chkstr(tag, tagbuf, list->word))
    {
      register struct word *node;

      node = (struct word *) malloc(sizeof(struct word));
      node->word = list->word;
      node->next = NULL;
      if (wlist)
        wcurr->next = node;
      else
        wlist = node;
      wcurr = node;
    }
    list = list->next;
  }
  return wlist;
}


static void
ClearSubList(list)
  struct word *list;
{
  struct word *tmp_list;

  while (list)
  {
    tmp_list = list->next;
    free(list);
    list = tmp_list;
  }
}


static int
MaxLen(list, count)
  struct word *list;
{
  int len = strlen(list->word);
  int t;

  while (list && count)
  {
    if ((t = strlen(list->word)) > len)
      len = t;
    list = list->next;
    count--;
  }
  return len;
}


void
namecomplete(prompt, data)
  char *prompt, *data;
{
  char *temp;
  struct word *cwlist, *morelist;
  int x, y, origx, origy;
  int ch;
  int count = 0;
  int clearbot = NA;

  if (toplev == NULL)
    AddNameList("");
  cwlist = GetSubList("", toplev);
  morelist = NULL;
  temp = data;

  outs(prompt);
  clrtoeol();
  getyx(&y, &x);
  getyx(&origy, &origx);
  standout();
  prints("%*s", 26, "");
  standend();
  move(y, x);
  refresh();

  while ((ch = igetch()) != EOF)
  {
    if (ch == '\n' || ch == '\r')
    {
      *temp = '\0';
      outc('\n');
      if (NumInList(cwlist) == 1 || (*data && !strcasecmp(cwlist->word, data)))
        strcpy(data, cwlist->word);
      ClearSubList(cwlist);
      break;
    }
    if (ch == ' ')
    {
      int col, len;

      if (NumInList(cwlist) == 1)
      {
        strcpy(data, cwlist->word);
        move(y, x);
        outs(data + count);
        count = strlen(data);
        temp = data + count;
        getyx(&y, &x);
        continue;
      }
      clearbot = YEA;
      col = 0;
      if (!morelist)
        morelist = cwlist;
      len = MaxLen(morelist, p_lines);
      move(2, 0);
      clrtobot();
      printdash("¬ÛÃö¸ê°T¤@Äýªí");
      while (len + col < 80)
      {
        int i;

        for (i = p_lines; (morelist) && (i > 0); i--)
        {
          move(3 + (p_lines - i), col);
          outs(morelist->word);
          morelist = morelist->next;
        }
        col += len + 2;
        if (!morelist)
          break;
        len = MaxLen(morelist, p_lines);
      }
      if (morelist)
      {
        move(b_lines, 0);
        outs(msg_more);
      }
      move(y, x);
      continue;
    }
    if (ch == '\177' || ch == '\010')
    {
      if (temp == data)
        continue;
      temp--;
      count--;
      *temp = '\0';
      ClearSubList(cwlist);
      cwlist = GetSubList(data, toplev);
      morelist = NULL;
      x--;
      move(y, x);
      outc(' ');
      move(y, x);
      continue;
    }
    if (count < STRLEN)
    {
      struct word *node;

      *temp++ = ch;
      count++;
      *temp = '\0';
      node = GetSubList(data, cwlist);
      if (node == NULL)
      {
        temp--;
        *temp = '\0';
        count--;
        continue;
      }
      ClearSubList(cwlist);
      cwlist = node;
      morelist = NULL;
      move(y, x);
      outc(ch);
      x++;
    }
  }
  if (ch == EOF)
    longjmp(byebye, -1);
  outc('\n');
  refresh();
  if (clearbot)
  {
    move(2, 0);
    clrtobot();
  }
  if (*data)
  {
    move(origy, origx);
    outs(data);
    outc('\n');
  }
}


/* ---------------------------------------------------- */
/* name complete for user ID                             */
/* ---------------------------------------------------- */


static int
UserMaxLen(cwlist, cwnum, morenum, count)
  char cwlist[][IDLEN + 1];
int cwnum, morenum, count;
{
  int len, max = 0;

  while (count-- > 0 && morenum < cwnum)
  {
    len = strlen(cwlist[morenum++]);
    if (len > max)
      max = len;
  }
  return max;
}


static int
UserSubArray(cwbuf, cwlist, cwnum, key, pos)
  char cwbuf[][IDLEN + 1];
char cwlist[][IDLEN + 1];
int cwnum, key, pos;
{
  int key2, num = 0;
  int n, ch;

  key = chartoupper(key);
  if (key >= 'A' && key <= 'Z')
  {
    key2 = key | 0x20;
  }
  else
  {
    key2 = key;
  }
  for (n = 0; n < cwnum; n++)
  {
    ch = cwlist[n][pos];
    if (ch == key || ch == key2)
    {
      strcpy(cwbuf[num++], cwlist[n]);
    }
  }
  return num;
}


void
usercomplete(prompt, data, flag)
  char *prompt, *data, flag;
{
  char *temp;
  char *u_namearray();
  char *cwbuf, *cwlist;
  int cwnum, x, y, origx, origy;
  int clearbot = NA, count = 0, morenum = 0;
  char ch;

  cwbuf = (char *) malloc(MAXUSERS * (IDLEN + 1));
  cwlist = u_namearray(cwbuf, &cwnum, "");
  temp = data;

  outs(prompt);
  clrtoeol();
  getyx(&y, &x);
  getyx(&origy, &origx);
  standout();
  prints("%*s", IDLEN + 1, "");
  standend();
  move(y, x);
  while ((ch = igetch()) != EOF)
  {
    if (ch == '\n' || ch == '\r')
    {
      int i;
      char *ptr;

      *temp = '\0';
      outc('\n');
      ptr = cwlist;
      for (i = 0; i < cwnum; i++)
      {
        if (ci_strncmp(data, ptr, IDLEN + 1) == 0)
          strcpy(data, ptr);
        ptr += IDLEN + 1;
      }
      break;
    }
    else if (ch == ' ')
    {
      int col, len;

      if (cwnum == 1)
      {
        strcpy(data, cwlist);
        move(y, x);
        outs(data + count);
        count = strlen(data);
        temp = data + count;
        getyx(&y, &x);
        continue;
      }
      clearbot = YEA;
      col = 0;
      len = UserMaxLen(cwlist, cwnum, morenum, p_lines);
      move(2, 0);
      clrtobot();
      printdash("¨Ï¥ÎªÌ¥N¸¹¤@Äýªí");
      while (len + col < 79)
      {
        int i;

        for (i = 0; morenum < cwnum && i < p_lines; i++)
        {
          move(3 + i, col);
          prints("%s ", cwlist + (IDLEN + 1) * morenum++);
        }
        col += len + 2;
        if (morenum >= cwnum)
          break;
        len = UserMaxLen(cwlist, cwnum, morenum, p_lines);
      }
      if (morenum < cwnum)
      {
        move(b_lines, 0);
        outs(msg_more);
      }
      else
      {
        morenum = 0;
      }
      move(y, x);
      continue;
    }
    else if (ch == '\177' || ch == '\010')
    {
      if (temp == data)
        continue;
      temp--;
      count--;
      *temp = '\0';
      cwlist = u_namearray(cwbuf, &cwnum, data);
      morenum = 0;
      x--;
      move(y, x);
      outc(' ');
      move(y, x);
      continue;
    }
    else if (count < STRLEN)
    {
      int n;

      *temp++ = ch;
      *temp = '\0';
      n = UserSubArray(cwbuf, cwlist, cwnum, ch, count);
      if (n == 0)
      {
        temp--;
        *temp = '\0';
        continue;
      }
      cwlist = cwbuf;
      count++;
      cwnum = n;
      morenum = 0;
      move(y, x);
      outc(ch);
      x++;
    }
  }
  free(cwbuf);
  if (ch == EOF)
    longjmp(byebye, -1);
  outc('\n');
  refresh();
  if (clearbot)
  {
    move(2, 0);
    clrtobot();
  }
  if (*data)
  {
    move(origy, origx);
    outs(data);
    outc('\n');
  }
}
