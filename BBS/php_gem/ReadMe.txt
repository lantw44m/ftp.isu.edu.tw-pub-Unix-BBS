
	�o�O php.twbbs.org �� plan ����ذϪ����Y��.
	�H�U�O .DIR �����c, �Ʊ��j�a�����U :)

typedef struct
{
  char filename[FNLEN];         /* M.9876543210.A */
  char savemode;                /* file save mode */
  char owner[IDLEN + 2];        /* uid[.] */
  char date[6];                 /* [02/02] or space(5) */
  char title[TTLEN + 1];
  uschar filemode;              /* must be last field @ boards.c */
}	fileheader;

