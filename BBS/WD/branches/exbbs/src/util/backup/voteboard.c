/*-------------------------------------------------------*/
/* util/voteboard.c     ( NTHU CS MapleBBS Ver 2.36 )    */
/*-------------------------------------------------------*/
/* target : 站內投票看板                                 */
/* create : 97/09/05                                     */
/* update : 97/09/05                                     */
/*-------------------------------------------------------*/

/* by wsyfish */

#include "bbs.h"

#define DOTBOARDS BBSHOME"/.BOARDS"

boardheader allbrd[MAXBOARD];
static char STR_bv_topic[] = "topic";
char *str_board_file = BBSHOME"/boards/%s/%s";

struct vote_board
{
  char brdname[IDLEN + 1];
  char topic[50];
};
typedef struct vote_board vb;

void
setbfile(buf, boardname, fname)
  char *buf, *boardname, *fname;
{
  sprintf(buf, str_board_file, boardname, fname);
}

main()
{
  FILE *fp, *ftopic;
  int i;
/*  int inf;*/
  int count;
  vb voteboard[MAXBOARD];  

/*  inf = open(DOTBOARDS, O_RDONLY);
  if (inf == -1)
  {
    printf("error open file\n");
    exit(1);
  }*/

  if ((fp = fopen(DOTBOARDS, "r")) == NULL)
  {
    printf("cann't read");
    return 1;
  }

  i = 0;
  count = 0;
  memset(allbrd, 0, MAXBOARD * sizeof(boardheader));
  memset(voteboard, 0, MAXBOARD * sizeof(vb));

  while (fread(&allbrd[i], sizeof(boardheader), 1, fp))
  {
    if (allbrd[i].brdname[0] && allbrd[i].bvote == 1)
    {
      strcpy(voteboard[i].brdname, allbrd[i].brdname);
      i++;
      count++;
    }
  }
  fclose(fp);

  if ((fp = fopen("etc/votetopic", "w")) != NULL)
  {
    char buf[STRLEN];

    for(i = 0; i < count; i++){
       setbfile(buf, voteboard[i].brdname, STR_bv_topic);
       if((ftopic = fopen(buf, "r")) != NULL){
/*         fscanf(ftopic, "%s", &voteboard[i].topic);*/
         fgets(voteboard[i].topic, 50, ftopic);
         fclose(ftopic);
       }      
       fwrite(&voteboard[i], sizeof(vb), 1, fp);
    }
    fclose(fp);
  }

  if ((fp = fopen(BBSHOME"/etc/voteboard", "w")) == NULL)
  {
    printf("cann't open etc/voteboard\n");
    return 1;
  }
  fprintf(fp, " [41;37;1m┴┬┴┬┴┬[46m?歈??????????????????裐歈?????????????????????????????[41;1m┬┴┬┴┬┴[0m\n");
  fprintf(fp, " [41;37;1m┬┴┬┴┬┴[46m??[33;1m☆ [32m 選舉公告欄  [33;1m☆[m[1;46;37m?矙?[32m請至下列投票所投票          [1;46;37m??[0m[41;1m┴┬┴┬┴┬[0m\n");
  fprintf(fp, " [41;37;1m┴┬┴┬┴┬[46m?礜w────────?鐒?                            [1;46;37m??[0m[41;1m┬┴┬┴┬┴[0m\n");
  fprintf(fp, " [41;37;1m┬┴┬┴┬┴[46m??  ▁▂████▃  ?矙? [33m%-12.12s  %-12.12s [1;46;37m??[0m[41;1m┴┬┴┬┴┬[0m\n", voteboard[0].brdname, voteboard[1].brdname);
  fprintf(fp, " [41;37;1m┴┬┴┬┴┬[46m??      ●●  ベт  ?矙? [33m%-12.12s  %-12.12s [1;46;37m??[0m[41;1m┬┴┬┴┬┴[0m\n", voteboard[2].brdname, voteboard[3].brdname);
  fprintf(fp, " [41;37;1m┬┴┬┴┬┴[46m??       __  / ◤   ?矙? [33m%-12.12s  %-12.12s [1;46;37m??[0m[41;1m┴┬┴┬┴┬[0m\n", voteboard[4].brdname, voteboard[5].brdname);
  fprintf(fp, " [41;37;1m┴┬┴┬┴┬[46m?僓??????????????????禊? [33m%-12.12s  %-12.12s [1;46;37m??[0m[41;1m┬┴┬┴┬┴[0m\n", voteboard[6].brdname, voteboard[7].brdname);
  fprintf(fp, " [41;37;1m┬┴┬┴┬┴[46m??[35;1m請投給一號候選人  [1;46;37m?矙?                            [1;46;37m??[0m[41;1m┴┬┴┬┴┬[0m\n");
  fprintf(fp, " [41;37;1m┴┬┴┬┴┬[46m??  投下您神聖的一票?矙?            花園市政府公告  [1;46;37m??[0m[1;41m┬┴┬┴┬┴[0m\n");
  fprintf(fp, " [41;37;1m┬┴┬┴┬┴[46m?裺??????????????????潁裺?????????????????????????????[41;1m┴┬┴┬┴┬[0m");

  fclose(fp);
}
