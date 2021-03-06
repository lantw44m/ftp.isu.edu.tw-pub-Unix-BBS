/*
 *發信人: wsyfish.bbs@fpg.m4.ntu.edu.tw (等待中的魚魚站長), 看板: SobVersion
 *標  題: [util]站上星座統計 horoscope.c
 *發信站: 小魚的紫色花園 (Fri Sep  5 12:52:13 1997)
 *轉信站: sob!news.civil.ncku!fpg
 */

/*-------------------------------------------------------*/
/* util/sex.c           ( NTHU CS MapleBBS Ver 2.36 )    */
/*-------------------------------------------------------*/
/* target : 站上性別統計                                 */
/* create : 99/01/06                                     */
/* update : 99/01/06                                     */
/*-------------------------------------------------------*/

/* 本程式由 wildcat (wildcat.bbs@wd.twbbs.org)
   參考 horoscope.c 修改完成 */

#include "bbs.h"

#define DOTPASSWDS BBSHOME"/.PASSWDS"

struct userec cuser;

main()
{
  int i, j;
  FILE *fp;
  int max, item, maxsex,totalman=0;
  time_t now;
  int act[3];
  char *name[4] = {"男生",
                   "女生",
                   "不明",
                   ""
                           };
  char    *blk[10] =
  {
      "  ","▏", "▎", "▍", "▌",
      "▋","▊", "▉", "█", "█",
  };


  now = time(0);
  chdir(BBSHOME);
  fp=fopen(DOTPASSWDS, "r");

  memset(act, 0, sizeof(act));
  while((fread(&cuser, sizeof(cuser), 1, fp)) >0 )
  {
    switch (cuser.sex)
    {
      case 0:
        act[0]++;
        break;
      case 1:
        act[1]++;
        break;
      case 2:
        act[0]++;
        break;
      case 3:
        act[1]++;
        break;
      case 4:
        act[0]++;
        break;
      case 5:
        act[1]++;
        break;
      case 6:
        act[2]++;
        break;
      case 7:
        act[2]++;
        break;
      default:
        act[2]++;
        break;
    }

  }
  fclose(fp);


  for (i = max  = maxsex = 0; i < 3; i++)
  {
    if (act[i] > max)
    {
      max = act[i];
      maxsex = i;
    }
  }

  item = max / 30 + 1;

  if ((fp = fopen("etc/sexcount", "w")) == NULL)
  {
    printf("cann't open etc/sexcount\n");
    return 1;
  }

  fprintf(fp,"           [1m[42;33m %s [m 性別統計 %s\n", BBSNAME,ctime(&now));

  for(i = 0;i < 3;i++)
  {
    fprintf(fp,"\n[1m[47;34m %s [m[32m", name[i]);
    for(j = 0; j < act[i]/item; j++)
    {
      fprintf(fp,"%2s",blk[9]);
    }
    fprintf(fp,"%2s [1;33m%d[m\n\n",blk[(act[i] % item) * 10 / item],
            act[i]);
    totalman += act[i];
  }
  fprintf(fp,"\n\n\n                          [1m[42;33m 站上總人口 [m %d 人", totalman);
  fclose(fp);
}
