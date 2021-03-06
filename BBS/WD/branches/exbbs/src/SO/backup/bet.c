/********************************/
/*   瘋狂賭盤 (原名:下注遊戲)   */
/********************************/

//原構想及程式由 dsyan 提供及撰寫..
//後經 weiren 改寫部分架構得此程式..
//有任何問題或建議歡迎與這兩位連絡.. :)

// dsyan 是天長地久(Forever.twbbs.org) 的站長
// =>dsyan.bbs@Forever.twbbs.org
// weiren 是荒謬化境 (weird.twbbs.org.tw) 的站長
// =>http://www.weiren.net/
// =>weiren@weiren.net   
 
#include "bbs.h"
#define maxboard 40
#define newgame 5 /* 最後開的幾盤要變色 */
#define LOCK_FILE BBSHOME"/game/bet.lock"

time_t
get_the_time()
{
  char buf[8],ch;
  char *t_max = "19392959";
  time_t sch_time;
  struct tm ptime;
  int i;

  outs("請輸入要停注的時間(請用24小時制):  月  日  時  分");

  for(i=0;i<8;i++){
     move(3,(33+i/2*4+i%2));
     ch = igetch();
     if(ch == 'q' || ch == 'Q') return 0;
     else if ( ch == '\r') { i = -1; continue;}
     else if ( ch == '\177' || ch == Ctrl('H'))
     {
      if(i)i -= 2;
      else i = -1;
      continue;
     }                     /*允許倒退鍵及ENTER重新輸入*/
     if(ch >= '0' && ch <= t_max[i]){
       outc(ch);
       buf[i] = ch - '0';
     }
     else{ bell(); --i;}
  }
  ptime.tm_year = 99;ptime.tm_sec=0;
  ptime.tm_mon  = buf[0]*10+buf[1]-1;
  ptime.tm_mday = buf[2]*10+buf[3];
  ptime.tm_hour = buf[4]*10+buf[5];
  ptime.tm_min  = buf[6]*10+buf[7];
  sch_time      = mktime(&ptime);
  return sch_time;
}

int p_bet()
{
 char i,j,k,num,cont,cc[51], dd[13], genbuf[60],genbuf2[30], cho[9][51];
 int t[maxboard],money,tmp,phh,a,b,newbet[newgame];
 int turn=0,ch_color;
 char ch;
 time_t set_time, close_time;
 FILE *fs,*fss;

 more(BBSHOME"/game/bet.welcome",YEA);
 sprintf(genbuf2,BBSHOME"/game/bet.new");
 if(fss=fopen(genbuf2,"r")){
   for(j=0;j<newgame;j++){
      fscanf(fss,"%d\n",&newbet[j]);
   }
   fclose(fss);
 }

 while(-1)
 {
   setutmpmode(BET);
   showtitle("瘋狂賭盤", BoardName);

   if(dashf(LOCK_FILE)){
     pressanykey("發錢時間停止下注");
     return;
   }

do{
   move(1,0);
   prints("[44;1m編號  單  價           項    目    敘    述            開 局 者   尚有幾小時  [m\n");
   fs=fopen(BBSHOME"/game/bet.list","r");
   if(turn==0)tmp=0;
   else{
     tmp=20;
     for(k=0;k<20;k++){
       fscanf(fs,"%d %ld %s\n",&t[i],&close_time,dd);
       fgets(genbuf,60,fs);
     }
   }
   for(i=tmp;i<20+tmp;i++)
   {
     fscanf(fs,"%d %ld %s\n",&t[i],&close_time,dd);
     fgets(genbuf,60,fs);
     j=strlen(genbuf)-1;
     genbuf[j]=0;
     a=(close_time-time(0))/3600/24;
     b=(close_time-time(0))/3600%24;

     ch_color=0;
     for(j=0;j<newgame;j++){
       if(newbet[j]==i+1)ch_color=1;
     }

     sprintf(genbuf2,BBSHOME"/game/bet.ans%d",i+1);
     if(t[i]){
       if(fss=fopen(genbuf2,"r")){
         fclose(fss);
         prints("[35;1m %2d    %5d  %-40s %-12s [等待發錢][m\n"
                ,i+1,t[i],genbuf,dd);
       }
       else if(close_time-time(0)>0)
         prints("[%d;1m %2d    %5d  %-40s %-12s %2d天%2d小時[m\n"
                ,ch_color?37:32,i+1,t[i],genbuf,dd,a,b);
       else
         prints("[32;1m %2d    %5d  %-40s %-12s     [封盤][m\n"
                ,i+1,t[i],genbuf,dd);
     }
     else
       prints("[31;1m %2d    -----      尚未有人開局[m\n",i+1);
   }
   fclose(fs);
   move(22,0);clrtoeol();
   prints("[1;44;37m按鍵說明: [n]下一頁 [p]上一頁 [b]下注/開桌/開盤 [q]離開    金: %10d    [m",cuser.silvermoney);
   redoscr();
   do{ch=igetch();}while(ch!='q'&&ch!='n'&&ch!='p'&&ch!='b');
   if(ch=='q')return; if(ch=='n')turn=1; if(ch=='p')turn=0;
   j=0;
   if(ch=='b'){
     getdata(22, 0,"請輸入 1-40 或直接按 Enter 離開：", cc, 3, 1, 0);
     j=atoi(cc);
   }
}while(!j||j>maxboard||j<1);

   if(dashf(LOCK_FILE)){
     pressanykey("發錢時間停止下注");
     return;
   }

   if(t[j-1]) /* 已有人開局 */
   {
     fs=fopen(BBSHOME"/game/bet.list","r");
     for(i=0;i<j;i++)
     {
       fscanf(fs,"%d %ld %s\n",&t[i],&close_time,dd);
       fgets(genbuf,60,fs);
     }
     fclose(fs);
     if(close_time<time(0))  /* 時間到 */
     {
       sprintf(genbuf,BBSHOME"/game/bet.ans%d",j);
       if(fs=fopen(genbuf,"r"))
       {
        fclose(fs);
        pressanykey("結果已公佈..獎金將於明天 6:30am 發放..:)");
       }
       else if(strcmp(dd,cuser.userid))
        pressanykey("下注期限已到..等待 %s 開局..",dd);
       else
       {
           int ch[9],all=0;

           clear();
           showtitle("瘋狂賭盤", BoardName);
           prints("\n\n[44;1m  彩票數    倍率      敘  述           [m\n\n");
           sprintf(genbuf,BBSHOME"/game/bet.cho%d",j);
           fs=fopen(genbuf,"r");
           fscanf(fs,"%d\n%d\n",&tmp,&all);
           for(i=0;i<tmp;i++)
           {
             long xx;
             fgets(cho[i],250,fs);
             fscanf(fs,"%d\n",&ch[i]);
             sprintf(genbuf,"home/%s/bet.cho%d.%d",cuser.userid,j,i+1);
             xx=all*100/ch[i];
             prints("%d)%6d   %3d.%-2d   %s",i+1,ch[i],xx/100,xx%100,cho[i]);
           }
           fclose(fs);
           sprintf(genbuf,"請輸入最後的結果(1-%d)或直接按 Enter離開：",tmp);

           getdata(20,0,genbuf,cc,3,DOECHO,0);
           num=atoi(cc);

   if(dashf(LOCK_FILE)){
     pressanykey("發錢時間停止下注");
     return;
   }
           if(num<=tmp && num>=1)
           {
             getdata(21,0,"確定嗎?? (y/N)",cc,3,DOECHO,0);
   if(dashf(LOCK_FILE)){
     pressanykey("發錢時間停止下注");
     return;
   }
             if (*cc=='y')
             {
               sprintf(genbuf,BBSHOME"/game/bet.ans%d",j);
               fss=fopen(genbuf,"w");
               fprintf(fss,"%d",num);
               fclose(fss);
             }
           }
       }
     }
     else
     {
       sprintf(genbuf,BBSHOME"/game/bet.scr%d",j);
       more(genbuf,YEA);
       while(1)
       {
         int ch[9],all=0,hhall=0;

         clear();
         showtitle("瘋狂賭盤", BoardName);
         prints("\n\n[44;1m  彩票數       倍率      敘  述           [m\n\n");
         sprintf(genbuf,BBSHOME"/game/bet.cho%d",j);
         fs=fopen(genbuf,"r");
         fscanf(fs,"%d\n%d\n",&tmp,&all);
         for(i=0;i<tmp;i++)
         {
           int hh=0;
           long xx;
           fgets(cho[i],250,fs);
           fscanf(fs,"%d\n",&ch[i]);
           sprintf(genbuf,BBSHOME"/game/bet.cho%d.%d",j,i+1);
           if(fss=fopen(genbuf,"r"))
           {
             char id[13];
             phh=0;
             while( fscanf(fss,"%s %d\n",id,&hh) != EOF ){
               if(!strcmp(id,cuser.userid))
               {
                 phh+=hh;
                 hhall+=hh;
               }
             }
             fclose(fss);
           }
           else hh=0;
           xx=all*100/ch[i];
           prints("%2d)%4d/%4d   %3d.%-2d   %s",i+1,phh,ch[i],xx/100,xx%100,cho[i]);
         }
         fclose(fs);
         prints("\n全>%4d/%4d\n",hhall,all);
         prints("\n本局總賭金: %d 單張賭票價格: %d",all*t[j-1],t[j-1]);

         sprintf(genbuf,"請輸入你的選擇(1-%d)或按 q 跳出：",tmp);
         getdata(20,0,genbuf,cc,3,DOECHO,0);
   if(dashf(LOCK_FILE)){
     pressanykey("發錢時間停止下注");
     return;
   }
         if(cc[0]=='q') break;
         num=cc[0]-48;
         if(num>tmp||num<1)
           pressanykey("輸入範圍有誤!!請重新輸入..");
         else
         {
           sprintf(genbuf,"一張彩券 %d 元，你目前有 %d，要買幾張？(0-%d)",
             t[j-1],cuser.silvermoney,cuser.silvermoney/t[j-1]);
           getdata(21,0,genbuf,cc,6,DOECHO,0);
   if(dashf(LOCK_FILE)){
     pressanykey("發錢時間停止下注");
     return;
   }
           if(atoi(cc)>cuser.silvermoney/t[j-1] || !atoi(cc))
             pressanykey("輸入範圍有誤!!取消..");
           else
           {
             int hh=0;
             if(close_time<time(0)){pressanykey("超過時間了!");return;}
             demoney(atoi(cc)*t[j-1]);
             sprintf(genbuf,BBSHOME"/game/bet.cho%d",j);
             fs=fopen(genbuf,"r");
             fscanf(fs,"%d\n%d\n",&tmp,&all);
             for(i=0;i<tmp;i++){
               fgets(cho[i],250,fs);
               fscanf(fs,"%d\n",&ch[i]); 
             }
             fclose(fs); 
             ch[num-1]+=atoi(cc);
             all+=atoi(cc);
             fs=fopen(genbuf,"w");
             fprintf(fs,"%d\n%d\n",tmp,all);
             for(i=0;i<tmp;i++)
             {
               char g=strlen(cho[i])-1;
               cho[i][g]=0;
               fprintf(fs,"%s\n%d\n",cho[i],ch[i]);
             }
             fclose(fs);
             sprintf(genbuf,BBSHOME"/game/bet.cho%d.%d",j,num);
             fs=fopen(genbuf,"a+");
             fprintf(fs,"%s %d\n",cuser.userid,atoi(cc));
             fclose(fs);

             sprintf(genbuf,"home/%s/bet.cho%d.%d",cuser.userid,j,num);
             if(fs=fopen(genbuf,"r"))
             {
               fscanf(fs,"%d\n",&hh);
               hh+=atoi(cc);
               fclose(fs);
             }
             else hh=atoi(cc);

             fs=fopen(genbuf,"w");
             fprintf(fs,"%d\n",hh);
             fclose(fs);
           }
         }
       }
     }
   }
   else /* 自己開局 */
   {
     cont=1;
     more(BBSHOME"/game/bet.anno",YEA);
     getdata(22,0,"確定要開局？(y/N)",cc,3,1,0);
   if(dashf(LOCK_FILE)){
     pressanykey("發錢時間停止下注");
     return;
   }
     if(cc[0]!='y' && cc[0]!='Y') cont=0;

     if(cont)
     {
       sprintf(cc,"home/%s/bet.scr",cuser.userid);
       pressanykey("請按任一鍵開始編輯此次 [開局宗旨]");
       if(vedit(cc,NA,0)==-2) cont=0;
     }

     if(cont)
     {
       move(3,0);set_time=get_the_time();
       if(!set_time) cont=0;
       if(set_time < time(0) + 36000){pressanykey("時間過短(最少 10 hr)");cont=0;}
       if(set_time > time(0) + (86400*14)){pressanykey("時間過長(最長 14 天)");cont=0;}
       if(set_time>time(0)+86400&&j<=13){pressanykey("1-13 桌時間最多 24 hr");cont=0;}
     }

     if(cont)
       if(!getdata(4,0,"請簡短敘述此局：",cc,41,1,0)) cont=0;

     if(cont)
     {
       getdata(5,0,"共有幾個選項？(2-9)",genbuf,3,DOECHO,0);
       tmp=atoi(genbuf);
       if(tmp<1 || tmp>9)
       {
         pressanykey("選項數目錯誤!!");
         cont=0;
       }
     }

     if(cont)
     {
       getdata(6,0,"彩券一張多少錢？(100-10000)", genbuf, 6, DOECHO, 0);
       money=atoi(genbuf);
       cont=0;
       if(money<100 || money>10000)
         pressanykey("輸入金額錯誤!!");
       else if(cuser.silvermoney<money*(10*tmp+5))
         pressanykey("你的錢不足 %d 元喔!!",money*(10*tmp+5));
       else
        cont=1;
     }

     if(cont)
     {
       for(i=0;i<tmp;i++)
       {
         move(7+i,0); prints("選項%d：",i+1);
         do{getdata(7+i,7,"",cho[i],50,DOECHO,0);}while(!cho[i][0]);
       }
       sprintf(genbuf,BBSHOME"/game/bet.cho%d",j);
   if(dashf(LOCK_FILE)){
     pressanykey("發錢時間停止下注");
     return;
   }
       if(dashf(genbuf)){
         fclose(fs);pressanykey("有人比你快一步了");return;
       }

       fs=fopen(genbuf,"w");
       fprintf(fs,"%d\n%d\n",tmp,tmp*10);
       for(i=0;i<tmp;i++)
       {
         fprintf(fs,"%s\n10\n",cho[i]);
         sprintf(genbuf,BBSHOME"/game/bet.cho%d.%d",j,i+1);
         fss=fopen(genbuf,"a+");
         fprintf(fss,"%s %d\n",cuser.userid,10);
         fclose(fss);

         sprintf(genbuf,"home/%s/bet.cho%d.%d",cuser.userid,j,i+1);
         fss=fopen(genbuf,"w");
         fprintf(fss,"10\n");
         fclose(fss);
       }
       fclose(fs);

       sprintf(genbuf2,BBSHOME"/game/bet.new");
       if(fss=fopen(genbuf2,"r")){
         for(i=0;i<newgame;i++){
            fscanf(fss,"%d\n",&newbet[i]);
         }
         fclose(fss);
       }
       if(fss=fopen(genbuf2,"w")){
         fprintf(fss,"%d\n",j);
         for(i=0;i<newgame-1;i++){
            fprintf(fss,"%d\n",newbet[i]);
         }
         fclose(fss);
       }

       demoney(money*(10*tmp+5));
       time(&close_time);
       close_time = set_time;
       while(fs=fopen(BBSHOME"/game/bet.list.w","r"))
       {
         fclose(fs);
       }
       fs=fopen(BBSHOME"/game/bet.list","r");
       fss=fopen(BBSHOME"/game/bet.list.w","w");
       for(i=0;i<maxboard;i++)
       {
         if(i+1==j)
         {
           fgets(genbuf,60,fs);
           fgets(genbuf,60,fs);
           fprintf(fss,"%d %ld %s\n",money,close_time,cuser.userid);
           fprintf(fss,"%s\n",cc);
         }
         else
         {
           fgets(genbuf,60,fs);
           fprintf(fss,genbuf);
           fgets(genbuf,60,fs);
           fprintf(fss,genbuf);
         }
       }
       fclose(fs);
       fclose(fss);
       system("mv -f game/bet.list.w game/bet.list");
       sprintf(genbuf,"mv -f home/%s/bet.scr game/bet.scr%d",cuser.userid,j);
       system(genbuf);
       pressanykey("開桌完成!!");
     }
     else
     {
      sprintf(cc,"rm -f game/bet.scr%d",j);
      system(cc);
      pressanykey("開桌取消!!");
     }
   }
 }
}

