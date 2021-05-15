/*     壽星程式               96 10/11            */
/*     the program is designed by Ptt             */
/*     you can emailto://b3504102@csie.ntu.edu.tw */
/*     Ptt BBS telnet://ptt.m8.ntu.edu.tw         */

#include <stdio.h>
#include "bbs.h"

#define DOTPASSWDS "/home/" BBSUSER "/.PASSWDS"
#define OUTFILE    "/home/" BBSUSER "/etc/birth.today"

struct userec cuser;

main(argc, argv)
int argc;
char **argv;
{
    FILE *fp1;
    char today[50][14],today_name[50][24],month[500][14],
         month_day[500],week[200][14],week_day[200];
    int i,day=0,mon=0,wee=0,a[50],b[50];
    time_t now;
    struct tm *ptime;

    now = time(NULL) ;     /* back to ancent */
    ptime = localtime(&now);

    fp1=fopen(DOTPASSWDS, "r");

    while( (fread( &cuser, sizeof(cuser), 1, fp1))>0 ) {
       if(!cuser.month || !cuser.day) continue;  /* Dopin: 解決除零錯誤 */
           if(cuser.month == ptime->tm_mon +1 )
             {
             if(cuser.day == ptime->tm_mday)
               {
                a[day]=cuser.numlogins;
                b[day]=cuser.numposts;
                strcpy(today[day  ],cuser.userid);
                strcpy(today_name[day++],cuser.username);
               }
             else if(cuser.day <= ptime->tm_mday+2 && cuser.day >=
                 ptime->tm_mday-2)
               {
                 week_day[wee] = cuser.day;
                 strcpy(week[wee++],cuser.userid);
               }
             else
               {
                 month_day[mon] = cuser.day;
                 strcpy(month[mon++],cuser.userid);
               }
             }
      }
    fclose(fp1);

    fp1=fopen(OUTFILE,"w");

    fprintf(fp1,"\n                      "
"[1m★[35m★[34m★[33m★[32m★[31m★[45;33m 壽星一覽 "
"[40m★[32m★[33m★[34m★[35m★[1m★[m \n\n");
    if(day>0)
    {
     fprintf(fp1,"[33m【[1;45m本日壽星[40;33m】[m \n");
     for (i=0;i<day;i++)
        {
    fprintf(fp1,"   [33m[%2d/%-2d] %-14s[0m %-24s login:%-5d post:%-5d\n",
           ptime->tm_mon+1,ptime->tm_mday,today[i],today_name[i],a[i],b[i]);
        }
    }
    if(week>0)
    {
     fprintf(fp1,"[33m【[1;45m前後兩天內壽星[40;33m】[m \n");
     for (i=0;i<wee;i++)
        {
          fprintf(fp1,"   [%2d/%-2d] [36m%-14s[m"
                  ,ptime->tm_mon+1,week_day[i],week[i]);
          if(!((i+1)%3)) fprintf(fp1,"\n");
        }
    }
/*
    fprintf(fp1,"\n\n[33m【[1;45m本月壽星[40;33m】[m \n");
    for (i=0;i<mon;i++)
        {
          fprintf(fp1,"   [%2d/%-2d] %-14s"
                  ,ptime->tm_mon+1,month_day[i],month[i]);
          if(!((i+1)%3)) fprintf(fp1,"\n");
        }
*/
    fclose(fp1);
    return 0;
}
