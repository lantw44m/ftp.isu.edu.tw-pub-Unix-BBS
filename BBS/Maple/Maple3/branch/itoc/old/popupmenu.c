/*-------------------------------------------------------*/
/* popupmenu.c	( YZU_CSE WindTop BBS )			 */
/*-------------------------------------------------------*/
/* target : popup menu					 */
/* create : 03/02/12					 */
/* modify :   /  /  					 */
/* author : verit.bbs@bbs.yzu.edu.tw			 */
/*-------------------------------------------------------*/


#include "bbs.h"

static screenline sl[t_lines];


/* ----------------------------------------------------- */
/* is_big5() �P�_�r�ꪺ�� pos �Ӧr���O���@�ر��p	 */
/* return 1  �N����r���e�b��				 */
/*       -1  �N����r����b��				 */
/*        0  �N���O����r				 */
/* ----------------------------------------------------- */


static int
is_big5(src, pos, mode)
  char *src;
  int pos;
  int mode;
{
  int wstate = 0;
  int word = 0;
  char *str;

  for (str = src; word < pos; str++)
  {
    if (mode)
    {
      if (*str == KEY_ESC)
      {
	str = strchr(str, 'm');
	if (!str)
	  return -1;
	continue;
      }
    }

    wstate = (*str < 0) ? ((wstate == 1) ? 0 : 1) : 0;
    word++;
  }

  if (wstate)
    return -1;

  if (*str < 0 && wstate == 0)
    return 1;
  else
    return 0;
}


/* verit: �p�⦩����X����ڪ��� */
static int
count_len(data)
  char *data;
{
  int len;
  char *ptr, *tmp;

  ptr = data;
  len = strlen(data);

  while (ptr)
  {
    ptr = strchr(ptr, KEY_ESC);
    if (ptr)
    {
      for (tmp = ptr; *tmp != 'm'; tmp++);
      len -= (tmp - ptr + 1);
      ptr = tmp + 1;
    }
  }
  return len;
}


/* verit: ���o�C�� */
static int
get_color(s, len, fc, bc, bbc)
  char *s;
  int len;
  int *fc;
  int *bc;
  int *bbc;
{
  char buf[32], *p, *e;
  int color;
  int state = 0, reset = 0, exit = 0;

  memset(buf, 0, sizeof(buf));
  strncpy(buf, s + 2, len - 1);

  for (p = e = &buf[0]; exit == 0; ++p)
  {
    if (*p == ';' || *p == 'm')
    {
      if (*p == 'm')
	exit = 1;
      *p = 0;

      color = atoi(e);

      if (color == 0)
      {
	*bbc = 0;
	reset = 1;
      }
      else if (color > 0 && color < 10)
      {
	*bbc = color;
	state |= 0x1;
      }
      else if (color > 29 && color < 38)
      {
	*fc = color;
	state |= 0x2;
      }
      else if (color > 39 && color < 48)
      {
	*bc = color;
	state |= 0x4;
      }
      e = p + 1;
    }
  }

  if (reset == 1)
  {
    if (!(state & 0x4))
      *bc = 40;
    if (!(state & 0x2))
      *fc = 37;
    if (!(state & 0x1))
      *bbc = 0;
  }

  if (state == 0)
  {
    *bc = 40;
    *fc = 37;
    *bbc = 0;
  }
}


static int
vs_line(msg, x, y)
  char *msg;
  int x, y;
{
  char buf[512], color[16], *str, *tmp, *cstr;
  int len = count_len(msg);
  int word, slen, fc = 37, bc = 40, bbc = 0;

  memset(buf, 0, sizeof(buf));

  sl[x].data[sl[x].len] = '\0';
  str = tmp = sl[x].data;

  for (word = 0; word < y && *str; ++str)
  {
    if (*str == KEY_ESC)
    {
      cstr = strchr(str, 'm');
      get_color(str, cstr - str, &fc, &bc, &bbc);
      str = cstr;
      continue;
    }
    word++;
  }

  strncpy(buf, sl[x].data, str - tmp);

  while (word++ < y)
    strcat(buf, " ");

  slen = strlen(buf) - 1;

  /* verit: ���p�o������r�����e�b�X�N�M�� */
  if (is_big5(buf, slen, 0) > 0)
    buf[slen] = ' ';

  strcat(buf, msg);

  if (*str)
  {
    for (word = 0; word < len && *str; ++str)
    {
      if (*str == KEY_ESC)
      {
	cstr = strchr(str, 'm');
	get_color(str, cstr - str, &fc, &bc, &bbc);
	str = cstr;
	continue;
      }
      word++;
    }

    if (*str)
    {
      /* verit: �ѨM�᭱�C��ɦ� */
      sprintf(color, "\033[%d;%d;%dm", bbc, fc, bc);
      strcat(buf, color);

      /* verit: ���p�̫�@�r�������媺��b���N�M�� */
      slen = strlen(buf);
      strcat(buf, str);
      if (is_big5(tmp, str - tmp, 0) < 0)
	buf[slen] = ' ';
    }
  }

  move(x, 0);
  outs(buf);
}


/* mode �N��[�I�����C�� */
static int
draw_ans_item(desc, mode, x, y, hotkey)
  char *desc;
  int mode;
  int x;
  int y;
  char hotkey;
{
  char buf[128];

  sprintf(buf, " \033[0;37m�j\033[4%d;37m%s%c\033[1;36m%c"
    "\033[0;37;4%dm%c%-25s%s\033[0;47;30m�p\033[40;30;1m�p\033[m ", 
    mode, (mode > 0) ? "�t" : "  ", (hotkey == *desc) ? '[' : '(', *desc, 
    mode, (hotkey == *desc) ? ']' : ')', desc + 1, (mode > 0) ? "�u" : "  ");
  vs_line(buf, x, y);
  move(b_lines, 0);
}


static int
draw_menu_des(desc, title, x, y, cur)
  char *desc[];
  char *title;
  int x, y;
  int cur;
{
  int num;
  char buf[128];
  char hotkey;
  hotkey = desc[0][0];

  sprintf(buf, " \033[0;40;37m�ššššššššššššššššš�\033[m   ");
  vs_line(buf, x - 2, y);
  sprintf(buf, " \033[0;37;44m�j%-31s \033[0;47;34m�p\033[m   ", title);
  vs_line(buf, x - 1, y);
  for (num = 1; desc[num]; num++)
    draw_ans_item(desc[num], (num == cur) ? 1 : 0, x++, y, hotkey);
  sprintf(buf, " \033[0;47;30m�h\033[30;1m�h�h�h�h�h�h�h�h�h�h�h�h�h�h�h�h�h"
    "\033[40;30;1m�p\033[m ");
  vs_line(buf, x, y);
  return num - 2;
}


/*------------------------------------------------------ */
/* x, y  �O�ۥX�������W���� (x, y) ��m			 */
/* title �O���������D					 */
/* desc  �O�ﶵ���ԭz�G					 */
/*       �Ĥ@�Ӧr�ꥲ������� char			 */
/*         �Ĥ@�Ӧr���N��@�}�l��а�����m		 */
/*         �ĤG�Ӧr���N����U KEY_LEFT ���w�]�^�ǭ�	 */
/*       �������r��O�C�ӿﶵ���ԭz (���r��������)	 */
/*       �̫�@�Ӧr�ꥲ���� NULL			 */
/*------------------------------------------------------ */

int
popupmenu_ans(x, y, title, desc)
  int x, y;
  char *title;
  char *desc[];
{
  int cur, old_cur, num, tmp;
  int ch;
  char t[64];
  char hotkey;
  hotkey = desc[0][0];

  vs_save(sl);
  sprintf(t, "�i%s�j", title);
  num = draw_menu_des(desc, t, x, y, 0);
  cur = old_cur = 0;
  for (tmp = 0; tmp < num; tmp++)
  {
    if (desc[tmp + 1][0] == hotkey)
      cur = old_cur = tmp;
  }
  draw_ans_item(desc[cur + 1], 1, x + cur, y, hotkey);

  while (1)
  {
    old_cur = cur;
    switch (ch = vkey())
    {
    case KEY_LEFT:
      vs_restore(sl);
      return (desc[0][1] | 0x20);
    case KEY_UP:
      cur = (cur == 0) ? num : cur - 1;
      break;
    case KEY_DOWN:
      cur = (cur == num) ? 0 : cur + 1;
      break;
    case KEY_HOME:
      cur = 0;
      break;
    case KEY_END:
      cur = num;
      break;
    case KEY_RIGHT:
    case '\n':
      vs_restore(sl);
      return (desc[cur + 1][0] | 0x20);
    default:
      for (tmp = 0; tmp <= num; tmp++)
      {
	if ((ch | 0x20) == (desc[tmp + 1][0] | 0x20))
	{
	  cur = tmp;
	  break;
	}
      }
      break;
    }
    if (old_cur != cur)
    {
      draw_ans_item(desc[old_cur + 1], 0, x + old_cur, y, hotkey);
      draw_ans_item(desc[cur + 1], 1, x + cur, y, hotkey);
    }
  }
  return 0;
}
