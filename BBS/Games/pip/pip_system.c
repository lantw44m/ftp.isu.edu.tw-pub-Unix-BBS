/*---------------------------------------------------------------------------*/
/* 系統選單:個人資料  小雞放生  特別服務                                     */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include <time.h>
#include "bbs.h"
#include "pip.h"
extern struct chicken d;
extern time_t start_time;
extern time_t lasttime;
#define getdata(a, b, c , d, e, f, g) getdata(a,b,c,d,e,f,NULL,g)

char weaponhead[7][10]={
"沒有裝備",
"塑膠帽子", 
"牛皮小帽",
"㊣安全帽",
"鋼鐵頭盔",
"魔法髮箍",
"黃金聖盔"};


char weaponrhand[10][10]={
"沒有裝備",
"大木棒",  
"金屬扳手",
"青銅劍",  
"晴雷劍", 
"蟬翼刀", 
"忘情劍", 
"獅頭寶刀",
"屠龍刀",  
"黃金聖杖"
};  

char weaponlhand[8][10]={
"沒有裝備",
"大木棒", 
"金屬扳手",
"木盾",
"不鏽鋼盾",
"白金之盾",
"魔法盾",
"黃金聖盾"
};


char weaponbody[7][10]={
"沒有裝備",
"塑膠冑甲",
"特級皮甲",
"鋼鐵盔甲",
"魔法披風",
"白金盔甲",
"黃金聖衣"};

char weaponfoot[8][12]={
"沒有裝備",
"塑膠拖鞋",
"東洋木屐",
"特級雨鞋",
"NIKE運動鞋",
"鱷魚皮靴",
"飛天魔靴",
"黃金聖靴"
};

int 
pip_system_freepip()
{
      char buf[256];
      move(b_lines-1, 0);
      clrtoeol();
#ifdef MAPLE
      getdata(b_lines-1,1, "真的要放生嗎？(y/N)", buf, 2, 1, 0);
#else
      getdata(b_lines-1,1, "真的要放生嗎？(y/N)", buf, 2, DOECHO, YEA);
#endif  // END MAPLE
      if (buf[0]!='y'&&buf[0]!='Y') return 0;
      sprintf(buf,"%s 被狠心的 %s 丟掉了~",d.name,cuser.userid);
      pressanykey(buf);
      d.death=2;
      pipdie("[1;31m被狠心丟棄:~~[0m",2);
      return 0;
}


int
pip_system_service()
{
     int pipkey;
     int oldchoice;
     char buf[200];
     char oldname[21];
     time_t now;     

     move(b_lines, 0);
     clrtoeol();
     move(b_lines,0);
     prints("[1;44m  服務項目  [46m[1]命名大師 [2]變性手術 [3]結局設局                                [0m");
     pipkey=egetch();
     
     switch(pipkey)
     {
     case '1':
       move(b_lines-1,0);
       clrtobot();
#ifdef MAPLE
       getdata(b_lines-1, 1, "幫小雞重新取個好名字：", buf, 11, DOECHO,NULL);
#else
       getdata(b_lines-1, 1, "幫小雞重新取個好名字：", buf, 11, DOECHO,YEA);
#endif  // END MAPLE
       if(!buf[0])
       {
         pressanykey("等一下想好再來好了  :)");
         break;
       }
       else
       {
        strcpy(oldname,d.name);
        strcpy(d.name,buf);
        /*改名記錄*/
        now=time(0);
        sprintf(buf, "[1;37m%s %-11s把小雞 [%s] 改名成 [%s] [0m\n", Cdate(&now), cuser.userid,oldname,d.name);
        pip_log_record(buf);
        pressanykey("嗯嗯  換一個新的名字喔...");
       }
       break;
       
     case '2':  /*變性*/
       move(b_lines-1,0);
       clrtobot();
       /*1:公 2:母 */
       if(d.sex==1)
       { 
         oldchoice=2; /*公-->母*/
         move(b_lines-1, 0);
         prints("[1;33m將小雞由[32m♂[33m變性成[35m♀[33m的嗎？ [37m[y/N][0m");
       }
       else
       { 
         oldchoice=1; /*母-->公*/
         move(b_lines-1, 0); 
         prints("[1;33m將小雞由[35m♀[33m變性成[35m♂[33m的嗎？ [37m[y/N][0m");
       }
       move(b_lines,0);
       prints("[1;44m  服務項目  [46m[1]命名大師 [2]變性手術 [3]結局設局                                [0m");
       pipkey=egetch();
       if(pipkey=='Y' || pipkey=='y')
       {
         /*改名記錄*/
         now=time(0);
         if(d.sex==1)
           sprintf(buf,"[1;37m%s %-11s把小雞 [%s] 由♂變性成♀了[0m\n",Cdate(&now), cuser.userid,d.name);
         else
           sprintf(buf,"[1;37m%s %-11s把小雞 [%s] 由♀變性成♂了[0m\n",Cdate(&now), cuser.userid,d.name);           
         pip_log_record(buf);
         pressanykey("變性手術完畢...");       
         d.sex=oldchoice;
       }  
       break;
       
     case '3':
       move(b_lines-1,0);
       clrtobot();
       /*1:不要且未婚 4:要且未婚 */
       oldchoice=d.wantend;
       if(d.wantend==1 || d.wantend==2 || d.wantend==3)
       { 
         oldchoice+=3; /*沒有-->有*/
         move(b_lines-1, 0); 
         prints("[1;33m將小雞遊戲改成[32m[有20歲結局][33m? [37m[y/N][0m");
	 sprintf(buf,"小雞遊戲設定成[有20歲結局]..");         
       }
       else
       { 
         oldchoice-=3; /*有-->沒有*/
         move(b_lines-1, 0); 
         prints("[1;33m將小雞遊戲改成[32m[沒有20歲結局][33m? [37m[y/N][0m");
         sprintf(buf,"小雞遊戲設定成[沒有20歲結局]..");
       }
       move(b_lines,0);
       prints("[1;44m  服務項目  [46m[1]命名大師 [2]變性手術 [3]結局設局                                [0m");
       pipkey=egetch();
       if(pipkey=='Y' || pipkey=='y')
       {
         d.wantend=oldchoice;
         pressanykey(buf);
       }  
       break;     
     } 
     return 0;
}

int
pip_data_list()  /*看小雞個人詳細資料*/
{
  char buf[256];
  char inbuf1[20];
  char inbuf2[20];
  int tm;
  int pipkey;
  int page=1;
  
  tm=(time(0)-start_time+d.bbtime)/60/30;

  clear();  
  move(1,0);
  prints("       [1;33m?歈??????????????????歈??????裐?????????[m\n");
  prints("       [0;37m?矙???  ?矙? ??   ?矙裺裐歈??潁? ??   ??[m\n");
  prints("       [1;37m?矙???  ?矙矙歈?  ??  ?矙?    ?矙歈?  ??[m\n");
  prints("       [1;34m?裺????????裺潁裺???  ?裺?    ?裺潁裺???[32m......................[m");
  do
  { clrchyiuan(5,23);
    switch(page)
    {
     case 1:
       move(5,0);
       sprintf(buf,
       "[1;31m ╭┤[41;37m 基本資料 [0;1;31m├─────────────────────────────╮[m\n");  
       prints(buf);
  
       sprintf(buf,
       "[1;31m │[33m﹟姓    名 :[37m %-10s [33m﹟生    日 :[37m %02d/%02d/%02d   [33m﹟年    紀 :[37m %-2d         [31m│[m\n",
       d.name,d.year,d.month,d.day,tm);
       prints(buf);  
  
       sprintf(inbuf1,"%d/%d",d.hp,d.maxhp);  
       sprintf(inbuf2,"%d/%d",d.mp,d.maxmp);  
       sprintf(buf,
       "[1;31m │[33m﹟體    重 :[37m %-5d(米克)[33m﹟體    力 :[37m %-11s[33m﹟法    力 :[37m %-11s[31m│[m\n",
       d.weight,inbuf1,inbuf2);
       prints(buf);  
  
       sprintf(buf,
       "[1;31m │[33m﹟疲    勞 :[37m %-3d        [33m﹟病    氣 :[37m %-3d        [33m﹟髒    髒 :[37m %-3d        [31m│[m\n",
       d.tired,d.sick,d.shit);
       prints(buf);  
   
       sprintf(buf,  
       "[1;31m │[33m﹟腕    力 :[37m %-7d    [33m﹟親子關係 :[37m %-7d    [33m﹟金    錢 :[37m %-11d[31m│[m\n",
       d.wrist,d.relation,d.money);
       prints(buf);  
  
       sprintf(buf,  
       "[1;31m ├┤[41;37m 能力資料 [0;1;31m├─────────────────────────────┤[m\n");
       prints(buf);  
   
       sprintf(buf,   
       "[1;31m │[33m﹟氣    質 :[37m %-10d [33m﹟智    力 :[37m %-10d [33m﹟愛    心 :[37m %-10d [31m│[m\n",
       d.character,d.wisdom,d.love);
       prints(buf);  
   
       sprintf(buf, 
       "[1;31m │[33m﹟藝    術 :[37m %-10d [33m﹟道    德 :[37m %-10d [33m﹟家    事 :[37m %-10d [31m│[m\n",
       d.art,d.etchics,d.homework);
       prints(buf);  
 
       sprintf(buf, 
       "[1;31m │[33m﹟禮    儀 :[37m %-10d [33m﹟應    對 :[37m %-10d [33m﹟烹    飪 :[37m %-10d [31m│[m\n",
       d.manners,d.speech,d.cookskill);
       prints(buf);    
 
       sprintf(buf,  
       "[1;31m ├┤[41;37m 狀態資料 [0;1;31m├─────────────────────────────┤[m\n");
       prints(buf);  
 
       sprintf(buf, 
       "[1;31m │[33m﹟快    樂 :[37m %-10d [33m﹟滿    意 :[37m %-10d [33m﹟人    際 :[37m %-10d [31m│[m\n",
       d.happy,d.satisfy,d.toman);
       prints(buf);
  
       sprintf(buf, 
       "[1;31m │[33m﹟魅    力 :[37m %-10d [33m﹟勇    敢 :[37m %-10d [33m﹟信    仰 :[37m %-10d [31m│[m\n",
       d.charm,d.brave,d.belief);
       prints(buf);  

       sprintf(buf, 
       "[1;31m │[33m﹟罪    孽 :[37m %-10d [33m﹟感    受 :[37m %-10d [33m            [37m            [31m│[m\n",
       d.offense,d.affect);
       prints(buf);  

       sprintf(buf, 
       "[1;31m ├┤[41;37m 評價資料 [0;1;31m├─────────────────────────────┤[m\n");
       prints(buf);  

       sprintf(buf, 
       "[1;31m │[33m﹟社交評價 :[37m %-10d [33m﹟戰鬥評價 :[37m %-10d [33m﹟魔法評價 :[37m %-10d [31m│[m\n",
       d.social,d.hexp,d.mexp);
       prints(buf);  

       sprintf(buf, 
       "[1;31m │[33m﹟家事評價 :[37m %-10d [33m            [37m            [33m            [37m            [31m│[m\n",
       d.family);
       prints(buf);  
  
       sprintf(buf, 
       "[1;31m ╰────────────────────────────────────╯[m\n");
       prints(buf);  
       
       move(b_lines-1,0);       
       sprintf(buf, 
       "                                                              [1;36m第一頁[37m/[36m共二頁[m\n");
       prints(buf);  
       break;

     case 2:
       move(5,0);
       sprintf(buf, 
       "[1;31m ╭┤[41;37m 物品資料 [0;1;31m├─────────────────────────────╮[m\n");
       prints(buf);  
  
       sprintf(buf, 
       "[1;31m │[33m﹟食    物 :[37m %-10d [33m﹟零    食 :[37m %-10d [33m﹟大 補 丸 :[37m %-10d [31m│[m\n",
       d.food,d.cookie,d.bighp);
       prints(buf);  
  
       sprintf(buf, 
       "[1;31m │[33m﹟藥    草 :[37m %-10d [33m﹟書    本 :[37m %-10d [33m﹟玩    具 :[37m %-10d [31m│[m\n",
       d.medicine,d.book,d.playtool);
       prints(buf);  
  
       sprintf(buf, 
       "[1;31m ├┤[41;37m 遊戲資料 [0;1;31m├─────────────────────────────┤[m\n");
       prints(buf);  
  
       sprintf(buf, 
       "[1;31m │[33m﹟猜 拳 贏 :[37m %-10d [33m﹟猜 拳 輸 :[37m %-10d                         [31m│[m\n",
       d.winn,d.losee);
       prints(buf);  
  
       sprintf(buf, 
       "[1;31m ├┤[41;37m 武力資料 [0;1;31m├─────────────────────────────┤[m\n");
       prints(buf);  
  
       sprintf(buf, 
       "[1;31m │[33m﹟攻 擊 力 :[37m %-10d [33m﹟防 禦 力 :[37m %-10d [33m﹟速 度 值 :[37m %-10d [31m│[m\n",
       d.attack,d.resist,d.speed);
       prints(buf);  
       sprintf(buf, 
       "[1;31m │[33m﹟抗魔能力 :[37m %-10d [33m﹟戰鬥技術 :[37m %-10d [33m﹟魔法技術 :[37m %-10d [31m│[m\n",
       d.mresist,d.hskill,d.mskill);
       prints(buf);  
  
       sprintf(buf, 
       "[1;31m │[33m﹟頭部裝備 :[37m %-10s [33m﹟右手裝備 :[37m %-10s [33m﹟左手裝備 :[37m %-10s [31m│[m\n",
       weaponhead[d.weaponhead],weaponrhand[d.weaponrhand],weaponlhand[d.weaponlhand]);
       prints(buf);  
  
       sprintf(buf, 
       "[1;31m │[33m﹟身體裝備 :[37m %-10s [33m﹟腳部裝備 :[37m %-10s [33m            [37m            [31m│[m\n",
       weaponbody[d.weaponbody],weaponfoot[d.weaponfoot]);
       prints(buf);  
  
       sprintf(buf, 
       "[1;31m ╰────────────────────────────────────╯[m\n");
       prints(buf); 

       move(b_lines-1,0);
       sprintf(buf, 
       "                                                              [1;36m第二頁[37m/[36m共二頁[m\n");
       prints(buf);          
       break;
    }
    move(b_lines,0);
    sprintf(buf,"[1;44;37m  資料選單  [46m  [↑/PAGE UP]往上一頁 [↓/PAGE DOWN]往下一頁 [Q]離開:            [m");
    prints(buf);    
    pipkey=egetch();
    switch(pipkey)
    {
      case KEY_UP:
      case KEY_PGUP:
      case KEY_DOWN:
      case KEY_PGDN:
        if(page==1)
           page=2;
        else if(page==2)
           page=1;
        break;
#ifdef MAPLE
      case Ctrl('R'):
        if (currutmp->msgs[0].last_pid)
        {
          show_last_call_in();
          my_write(currutmp->msgs[0].last_pid, "水球丟回去：");
        }
        break;
#endif  // END MAPLE
    }
  }while((pipkey!='Q')&&(pipkey!='q')&&(pipkey!=KEY_LEFT));
  return 0;
}
