/*************************************************************************
 *
 *        bbs  �p�{����  by Ptt
 *
 *************************************************************************/

#include "bbs.h"

/* �ϥο������ */ 
void inmoney(int money)
{
	ACCT x;
	acct_load(&x,cuser.userid);  
        cuser.money = x.money + money;
        acct_save(&cuser);
}

void demoney(int money)
{
      ACCT x; 
      acct_load(&x,cuser.userid);
      if(x.money <=
         (unsigned long int) money)  cuser.money=0;
      else 
         cuser.money = x.money - money; 
      acct_save(&cuser); 
}

check_money(unsigned long int money)
{
  if(cuser.money<money)
    {
        char buf[50];
        sprintf(buf,"��p�I�z���W�u�� %d ���A������I",cuser.money);
        vmsg(buf);
        return 1;
    }
return 0;
}
int
log_file(char *filename,char *buf)
{
    FILE *fp;
    if( (fp = fopen(filename, "a" )) != NULL ) {
       fputs( buf, fp );
       if(!strchr(buf,'\n')) fputc('\n',fp);
         fclose( fp );
       return 0;
    }
    else return -1;
}
                                                                                