#include "bbs.h"
#define ba rpguser.race == 6 ? 10*rpguser.level : 5


int
gagb()
{
  int tmoney,money;
  char genbuf[200],buf[80];
  char ans[5]="";
  /* 倍率        0  1   2   3   4   5   6   7   8   9   10 */
  float bet[11]={0,100, 50, 10, 3 ,1.5,1.2,0.9, 0.8 ,0.5,0.1};
  int a,b,c,count;  
  FILE *fp;
  time_t now;
  
  setutmpmode(XAXB);
  clear();
  showtitle("猜數字！", BoardName);
  move(1,0);
  prints("[1m現在你身上共有 [33m%d [37m元。[m",cuser.silvermoney);
  move(6,0);
  prints("[36m%s[m",msg_seperator);
  move(17,0);
  prints("[36m%s[m",msg_seperator);
  fp = fopen("game/money", "r");
  fscanf(fp, "%d", &tmoney);
  fclose(fp);

  move(20,0);
  prints("現在遊樂場內共有 %d  元",tmoney);

  getdata(3, 0, "請問你要下注多少(1 ~ 250000)？ ", genbuf, 7, 1, 0);
  money=atoi(genbuf);
  if(money<=0||money>cuser.silvermoney||money>250000) 
    return 0;
  demoney(money); 

  move(1,15); 
  prints("%d",cuser.silvermoney);
  tmoney+=money; 

  move(20,17); 
  prints("%d",tmoney);

  do{
    itoa(rand()%10000,ans);
    for (a=0;a<3;a++)
    for (b=a+1;b<4;b++)
    if(ans[a]==ans[b]) ans[0]=0;
  }while (!ans[0]);

  for(count=1;count<11;count++)
  {
    do{
      getdata(5,0,"請猜 → ",genbuf,5,1,0);
      if(!strcmp(genbuf,"貓老大"))
      {
        prints("%s",ans);
        igetch();
      }
      c=atoi(genbuf);
      itoa(c,genbuf);
      for (a=0;a<3;a++)
      for (b=a+1;b<4;b++)
      if(genbuf[a]==genbuf[b]) 
        genbuf[0]=0;
      if(!genbuf[0])
      {
        pressanykey("輸入數字有問題!!");
      }
    }while (!genbuf[0]);
    move(count+6,0);
    prints("  [1;31m第 [37m%2d [31m次： [37m%s  ->  [33m%dA [36m%dB [m",count,genbuf,an(genbuf,ans),bn(genbuf,ans));
    if(an(genbuf,ans)==4) break;
  }
  now=time(0);

  if(count > 10)
  {
    sprintf(buf,"你輸了呦！正確答案是 %s，下次再加油吧!!",ans);
    game_log(XAXB,"[1;32m可憐沒猜到，輸了 %d 元！[m", money);
  }
  else
  {
  int oldmoney=money;
    money *= bet[count];
    inmoney(money);
    inexp(ba*(10-count));
    tmoney-=money;
    if(money-oldmoney > 0)
      sprintf(buf,"恭喜！總共猜了 %d 次，淨賺獎金 %d 元",count,money-oldmoney);
    if(money-oldmoney == 0)
      sprintf(buf,"唉∼∼總共猜了 %d 次，沒輸沒贏！",count);
    if(money-oldmoney < 0)
      sprintf(buf,"啊∼∼總共猜了 %d 次，賠錢 %d 元！",count,oldmoney-money);
    move(1,15); 
    prints("%d",cuser.silvermoney);
    move(20,17); 
    prints("%d",tmoney);
    if(money-oldmoney > 0)
      game_log(XAXB,"[1;31m猜了 %d 次，淨賺 %d 元！[m", count,money-oldmoney);
    if(money-oldmoney < 0)
      game_log(XAXB,"[1;32m猜了 %d 次，虧了 %d 元！[m", count,oldmoney-money);
  }

  fp = fopen("game/money", "w");
  fprintf(fp, "%d", tmoney);
  fclose(fp);

  pressanykey(buf);
  return 0;
}

itoa(i,a)
int i;
char *a;
{
 int j,k,l=1000;
// prints("itoa: i=%d ",i);

 for(j=3;j>0;j--)
 {
  k=i-(i%l);
  i-=k;
  k=k/l+48;
  a[3-j]=k;
  l/=10;
 }
 a[3]=i+48;
 a[4]=0;

// prints(" a=%s\n",a);
// igetch();
}

int
an(a,b)
char *a,*b;
{
 int i,k=0;
 for(i=0;i<4;i++)
  if(*(a+i)==*(b+i)) k++;
 return k;
}

int
bn(a,b)
char *a,*b;
{
 int i,j,k=0;
 for(i=0;i<4;i++)
  for(j=0;j<4;j++)
   if(*(a+i)==*(b+j)) k++;
 return (k-an(a,b));
}
