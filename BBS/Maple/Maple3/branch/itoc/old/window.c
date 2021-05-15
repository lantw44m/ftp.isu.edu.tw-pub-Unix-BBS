/*-------------------------------------------------------*/
/* window.c     ( Athenaeum BBS    Ver 0.01 )            */
/*-------------------------------------------------------*/
/* author : hialan.nation@infor.org                      */
/* target : window form                                  */
/* create : 2002/08/24                                   */
/* update : 2002/08/24                                   */
/*-------------------------------------------------------*/


#include "bbs.h"


/*
show_winline(x, y, ��������/2, �r��, �I���C��, �����C��);
show_winbox(��,�e,���D,���ܦr��,��ܼҦ�);

msgbox(��,�e,���D,���ܦr��,��ܼҦ�);
win_select(���D,���ܦr��,�ﶵ,�ﶵ��,��ܼҦ�,�w�]�r��);
win_select(���D,���ܦr��,�ﶵ,�ﶵ��,��ܼҦ�,�w�]�r��);

EX:
win_ans("�O�_�s��i�ݨ��W��H", 'n')
win_select("�[�K�峹", "�O�_�s��i�ݨ��W��? ", 0, 2, 1, 'n')

*/


static void
show_winline(x, y, win_len, words, bgcolor, barcolor)
  char *words, *bgcolor, *barcolor;
  int x, y, win_len;
{
  char buf[128];

  sprintf(buf, " �x%s %s %-*s\033[m�x ",
    (barcolor != 0) ? barcolor : bgcolor,
    (barcolor != 0) ? "��" : "  ",
    2 * (win_len - 4), words);

  move(x, y);
  clrtoeol();
  outs(buf);
  move(b_lines, 0);
}


static int
show_winbox(x, y, line, width, title, prompt, type)
  char *title, *prompt;
  int line, width, x, y;
  int type;			/* 0:clear 1:�m��  */
{
  int win_len;			/* win_len �O���X�Ӥ���r!! */
  int i, j;
  char *table[8] = {"�w", " �~", "�� ", " ��", "�� ", " �u", "�t ", "�x"};
  char buf[256];
  char *bgcolor = "\033[30;47m";
  char *title_color = "\033[1;44m";

  if (!line || line < 0)
    line = 1;

  if (width % 2)
    win_len = (width / 2) + 1;
  else
    win_len = width / 2;

  for (i = 0; i <= line + 3; i++)
  {
    move(x + i, 0);
    clrtoeol();
    prints("%80s", "");
  }
  /* �W���� */

  strcpy(buf, table[1]);

  j = win_len - 1;

  for (i = 1; i < j; i++)
    strcat(buf, table[0]);
  strcat(buf, table[2]);

  move(x, y);
  clrtoeol();
  outs(buf);

  /* ���D */
  show_winline(x + 1, y, win_len, title, title_color, 0);

  /* ���D�U��b */
  strcpy(buf, table[5]);
  j = win_len - 1;
  for (i = 1; i < j; i++)
    strcat(buf, table[0]);
  strcat(buf, table[6]);

  move(x + 2, y);
  clrtoeol();
  outs(buf);

  /* ���� */
  show_winline(x + 3, y, win_len, prompt, bgcolor, 0);

  /* �ڪ����� */
  strcpy(buf, table[3]);
  j = win_len - 1;
  for (i = 1; i < j; i++)
    strcat(buf, table[0]);
  strcat(buf, table[4]);

  move(x + 3 + line, y);
  clrtoeol();
  outs(buf);

  return win_len;
}


int				/* �^�ǩҫ����� */
msgbox(line, width, title, prompt, type)
  char *title, *prompt;
  int line, width;
  int type;			/* 0:clear 1:�m��  */
{
  int x, y, win_len;
  screenline slp[5];

  /* init window */
  if (!type)
  {
    clear();
    x = 0;
    y = 0;
    width = 78;
  }
  else
  {
    x = (b_lines - line - 5) / 2;
    y = (80 - width) / 2;
  }

  save_msgbox(slp, x, 5);
  win_len = show_winbox(x, y, line, width, title, prompt, type);
  y = vkey();
  restore_msgbox(slp, x, 5);

  return y;
}


int
win_ans(prompt, def)
  char *prompt;
  char def;		/* 'y' �� 'n' */
{
  int x, y, i;
  int win_len, ch;
  int width;
  char *p;
  char *bgcolor = "\033[30;47m";
  char *choose[2] = {"y(Y) �O", "n(N) �_"};
  screenline slp[7];

  /* init window */
  width = strlen(prompt) + 12;
  x = (b_lines - 2 - 6) / 2;
  y = (80 - width) / 2;

  save_msgbox(slp, x, 7);

  win_len = show_winbox(x, y, 3, width, "�T�{", prompt, 1);

  for (i = 0; i < 2; i++)
  {
    p = choose[i];
    show_winline(x + 4 + i, y, win_len, p + 1, bgcolor, 0);
  }

  /* default */
  i = (def == 'Y') ? 0 : 1;

  for (;;)
  {
    p = choose[i];
    show_winline(x + 4 + i, y, win_len, p + 1, bgcolor, "\033[44m");

    ch = vkey();
    if ((ch == 'y' && i) || (ch == 'n' && !i) || 
      ch == KEY_UP || ch == KEY_DOWN)
    {
      show_winline(x + 4 + i, y, win_len, p + 1, bgcolor, 0);
      i = i ? 0 : 1;
      continue;
    }
    if (ch == '\n')
    {
      ch = i ? 'n' : 'y';
      break;
    }
  }

  restore_msgbox(slp, x, 7);

  return ch;
}


#if 0
int
win_select(title, prompt, choose, many, type, def)
  int many, type;
  char *title, *prompt, **choose, def;
{
  int x, y, i;
  int win_len, ch;
  int width;
  char *p;
  char *bgcolor = "\033[30;47m";
  char *choose_yesno[2] = {"y(Y) �O", "n(N) �_"};

  if (!choose)
    choose = choose_yesno;

  for (i = 0; i < many; i++)
  {
    p = choose[i];
    if (def == *p)
    {
      def = i;
      break;
    }
  }

  /* init window */
  if (!type)
  {
    clear();
    x = 0;
    y = 0;
    width = 78;
  }
  else
  {
    width = strlen(title);
    i = strlen(prompt);
    if (i > width)
      width = i;
    for (i = 0; i < many; i++)	/* ch�Ȯɷ�@�Ȧs�ܼ�..:pp */
    {
      ch = strlen(choose[i]);
      if (ch > width)
	width = ch;
    }

    width = width + 12;
    x = (b_lines - many - 6) / 2;
    y = (80 - width) / 2;
  }

  win_len = show_winbox(x, y, many + 1, width, title, prompt, type);

  for (i = 0; i < many; i++)
  {
    p = choose[i];
    show_winline(x + 4 + i, y, win_len, p + 1, bgcolor, 0);
  }

  i = def;

  do
  {
    p = choose[i];
    show_winline(x + 4 + i, y, win_len, p + 1, bgcolor, "\033[44m");
    ch = vkey();

    switch (ch)
    {
    case KEY_UP:
      show_winline(x + 4 + i, y, win_len, p + 1, bgcolor, 0);
      i--;
      if (i < 0)
	i = many - 1;
      break;

    case KEY_DOWN:
      show_winline(x + 4 + i, y, win_len, p + 1, bgcolor, 0);
      i++;
      if (i >= many)
	i = 0;
      break;

    default:
      {
	int j;

	if (ch >= 'a' && ch <= 'z')
	  ch = (ch - 'a') + 'A';
	for (j = 0; j < many; j++)
	{
	  if (ch == *(choose[j]))
	  {
	    show_winline(x + 4 + i, y, win_len, p + 1, bgcolor, 0);
	    i = j;
	    break;
	  }
	}
	break;
      }
    }
  } while (ch != '\n');

  return *p;
}
#endif
