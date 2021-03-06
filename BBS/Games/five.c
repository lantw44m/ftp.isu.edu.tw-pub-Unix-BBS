/*  五子棋程式   Programmed by Birdman     */
/*  140.116.102.125 連珠哇哈哈小站         */
/*  成大電機88級                           */

#include "bbs.h"
#include <sys/socket.h>
#define black 1
#define white 2
#define FDATA "five"

int player,winner=0,quitf;
int px,py,hand,tdeadf,tlivef,livethree,threefour;
int chess[250][2]={0,0};
int playboard[15][15]={0,0};
char abcd[15]={'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O'};

struct fiverec
{
    int id;
    char userid[IDLEN+1];
    char username[23];
    int fwin;
    int floss;
    int fscore;
};

int initial_board()
{
        int i,j,k,t_win,t_loss,t_score;
        FILE *fp;
        char fname[50];

        for(i=0;i<=14;i++)
         for(j=0;j<=14;j++)
           playboard[i][j]=0;

        setuserfile(fname, FDATA);
        if ((fp = fopen(fname, "r")) != NULL)
         {
           fscanf(fp,"%d  %d  %d",&t_win,&t_loss,&t_score);
           fclose(fp);
         }
/*        toprenju(t_win,t_loss,t_score);*/
        setutmpmode(FIVE);
        hand=1;
        winner=0;
        quitf=0;
        px=14;
        py=7;
        clear();
        move(0,0);
        outs("┌┬┬┬┬┬┬┬┬┬┬┬┬┬┐15           一人自己玩版本
├┼┼┼┼┼┼┼┼┼┼┼┼┼┤14
├┼┼┼┼┼┼┼┼┼┼┼┼┼┤13
├┼┼＋┼┼┼┼┼┼┼＋┼┼┤12
├┼┼┼┼┼┼┼┼┼┼┼┼┼┤11
├┼┼┼┼┼┼┼┼┼┼┼┼┼┤10
├┼┼┼┼┼┼┼┼┼┼┼┼┼┤9
├┼┼┼┼┼┼＋┼┼┼┼┼┼┤8
├┼┼┼┼┼┼┼┼┼┼┼┼┼┤7
├┼┼┼┼┼┼┼┼┼┼┼┼┼┤6
├┼┼┼┼┼┼┼┼┼┼┼┼┼┤5
├┼┼＋┼┼┼┼┼┼┼＋┼┼┤4
├┼┼┼┼┼┼┼┼┼┼┼┼┼┤3
├┼┼┼┼┼┼┼┼┼┼┼┼┼┤2
└┴┴┴┴┴┴┴┴┴┴┴┴┴┘1         q:離開       Ins:悔棋
 A B C D E F G H I J K L M N O");


        move(7,14);
        outs("●");
        move(10,40);
        outs("q:離開");
        player=white;
        playboard[7][7]=black;
        chess[1][0]=14; /*紀錄所下位址*/
        chess[1][1]=7;
         move(4,40);
         outs("第 1手 ●H 8");
        user_guide();
        do{
          if(player==1){
           move(b_lines-5,10);
           outs("白手○");
           move(20,0);
           outs("換黑下");
           per_move(0);}
          else{
           move(b_lines-5,10);
           outs("黑手●");
           move(20,0);
           outs("換白下");
           per_move(0);}
/*check if five*/
           tdeadf=tlivef=livethree=threefour=0;
            for(j=0;j<=10;j++)
             calvalue(px/2,j,px/2,j+1,px/2,j+2,px/2,j+3,px/2,j+4);
           for(i=0;i<=10;i++)/*橫向*/
             calvalue(i,py,i+1,py,i+2,py,i+3,py,i+4,py);
           for(i=-4;i<=0;i++)/*斜右下*/
             calvalue(px/2+i,py+i,px/2+i+1,py+i+1,px/2+i+2,py+i+2,
                      px/2+i+3,py+i+3,px/2+i+4,py+i+4);
           for(i=-4;i<=0;i++)/*斜左下*/
             calvalue(px/2-i,py+i,px/2-i-1,py+i+1,px/2-i-2,py+i+2,px/2-i-3,
                      py+i+3,px/2-i-4,py+i+4);

            for(j=0;j<=9;j++)
             callfour(px/2,j,px/2,j+1,px/2,j+2,px/2,j+3,px/2,j+4,px/2,j+5);
           for(i=0;i<=9;i++)/*四橫向*/
             callfour(i,py,i+1,py,i+2,py,i+3,py,i+4,py,i+5,py);
           for(i=-5;i<=0;i++){/*四斜右下*/
             callfour(px/2+i,py+i,px/2+i+1,py+i+1,px/2+i+2,py+i+2,
                      px/2+i+3,py+i+3,px/2+i+4,py+i+4,px/2+i+5,py+i+5);
                            /*四斜左下*/
             callfour(px/2-i,py+i,px/2-i-1,py+i+1,px/2-i-2,py+i+2,px/2-i-3,
                      py+i+3,px/2-i-4,py+i+4,px/2-i-5,py+i+5);
             }
           if(tlivef>=2 && winner==0)    bandhand(4);
           if(livethree>=2 && tlivef ==0) bandhand(3);
           if(threefour==black)          liveagogo(7);
           else if(threefour==white)     liveagogo(8);

          }while((!quitf) && (winner==0));

        pressanykey();
        return;
}

int user_guide()
{
        move(8,40);
        outs("上:↑下:↓     左:←右:→");
        move(9,40);
        outs("Space:下子     q:離開");
        move(10,40);
        outs("t:送訊息");
        move(11,40);
        outs("黑先有禁手    要多注意!");
        return;

}

int per_move(int net)
{
        int ch,banf=0,x;
        char genbuf[5];

        move(py,px);
        do
        {
          switch (ch)
          {
             case 't':
             case Ctrl('R'): if (currutmp->msgs[0].last_pid) {
             show_last_call_in();
             my_write(currutmp->msgs[0].last_pid, "水球丟回去：");
            /* refscreen = YEA;*/
              }
            break;

            case KEY_DOWN:
             py=py+1;
             if(py>14) py=0;
            break;

            case KEY_UP:
             py=py-1;
             if(py<0) py=14;
            break;

            case KEY_LEFT:
             px=px-1;
             if(px<0) px=28;
            break;

            case KEY_RIGHT:
             px=px+1;
             if(px>28) {px=0;px=px-1;}  /*會跳格咧*/
            break;

            case KEY_INS:
            case KEY_DEL:
            case '\177':

             if(hand==1 || net==1) break;
             px=chess[hand][0];
             py=chess[hand][1];
             move(py,px);
             outs("┼");
             x=px/2;
             playboard[x][py]=0;
             hand-=1;
             if(player==black)
                player=white;
             else
                player=black;
             move(2,40);
             prints("第 %3d 手",hand);
            break;

            case 'q':
            case 'Q':
             quitf=1;
             getdata(b_lines - 1, 0, "您確定要離開【爆笑五子棋】嗎(Y/N)？[N] ",
                genbuf, 2, LCECHO, 0);

             if (*genbuf != 'y')
              {
              quitf=0;
              move(b_lines-1,0);
              outs("                                                    ");
              }
             else
              return 0;
            break;

            default:
            break;
           }
          move(py,px);
          x=px/2;
          if(playboard[x][py]!=0)
              banf=1;
          else
              banf=0;
        }while(((ch=igetkey())!=' ') || (banf == 1 ));
        if((px%2)==1) px=px-1; /*解決netterm不合問題*/
        move(py,px);
        hand+=1;
        playboard[x][py]=player;
        if(player==black)
        {
         outs("●");
         player=white;
        }
        else
        {
         outs("○");
         player=black;
        }

        chess[hand][0]=px;
        chess[hand][1]=py;
           if(hand%2==0)
            move(((hand-1)%22)/2+4,54);
           else
            move(((hand-1)%21)/2+4,40);

           prints("第%2d手 %s%c%2d",hand,
             (hand%2==1)?"●":"○",abcd[px/2],15-py);

        return;
}

int calvalue(int x1,int y1,
             int x2,int y2,
             int x3,int y3,
             int x4,int y4,
             int x5,int y5)
{
        int n_black,n_white,empty,i,j,k;

        n_black=n_white=empty=0;

        if(x1<0||x2<0||x3<0||x4<0||x5<0||
           x1>14||x2>14||x3>14||x4>14||x5>14)    return;
        if(winner!=0) return;
        if(playboard[x2][y2]==0 || playboard[x3][y3]==0
           || playboard[x4][y4]==0)
           empty=1; /*check 10111型死四*/

        if(playboard[x1][y1]==black) n_black+=1;
        if(playboard[x1][y1]==white) n_white+=1;
        if(playboard[x2][y2]==black) n_black+=1;
        if(playboard[x2][y2]==white) n_white+=1;
        if(playboard[x3][y3]==black) n_black+=1;
        if(playboard[x3][y3]==white) n_white+=1;
        if(playboard[x4][y4]==black) n_black+=1;
        if(playboard[x4][y4]==white) n_white+=1;
        if(playboard[x5][y5]==black) n_black+=1;
        if(playboard[x5][y5]==white) n_white+=1;

        if(playboard[x1][y1]==0 && playboard[x5][y5]==0)
         {
                if(n_white==3 || n_black==3)
                 {
                   move(15,60);
                   outs("▼活三!▼");
                  }
                 if(n_black==3)
                  livethree+=1;
          }

        if((n_white==4 || n_black==4) && (empty ==1))
         {
              bell();
              tdeadf+=1;
              move(15,60);
              outs("▼小心!死四!▼");
              return;
         }

        if(n_black==5)
         { /*再掃連六*/
           tlivef=-1;
           tdeadf=0;
           livethree=0;
           for(i=0;i<=14;i++)/*四縱向*/
            for(j=0;j<=9;j++)
             callfour(i,j,i,j+1,i,j+2,i,j+3,i,j+4,i,j+5);
           for(i=0;i<=9;i++)/*四橫向*/
            for(j=0;j<=14;j++)
             callfour(i,j,i+1,j,i+2,j,i+3,j,i+4,j,i+5,j);
           for(i=0;i<=9;i++)/*四斜右下*/
            for(j=0;j<=9;j++){
             callfour(i,j,i+1,j+1,i+2,j+2,i+3,j+3,i+4,j+4,i+5,j+5);
                            /*四斜左下*/
             callfour(i,j+5,i+1,j+4,i+2,j+3,i+3,j+2,i+4,j+1,i+5,j);
             }
           if(winner==0)
           {
            bell();
            move(10,35);
            outs("[31m┌────┐[m");
            move(11,35);
            outs("[31m│  [30;42m黑勝[m [31m │[m");
            move(12,35);
            outs("[31m└────┘[m");
            igetkey();
            winner=black;
           }
          }
        if(n_white==5)
         {
           bell();
           move(10,35);
           outs("[33m┌────┐[m");
           move(11,35);
           outs("[33m│[m  [30;42m白勝[m  [33m│[m");
           move(12,35);
           outs("[33m└────┘[m");
           winner=white;
           igetkey();
          }
        return;
}

int callfour(int x1,int y1,int x2,int y2,int x3,int y3,
             int x4,int y4,int x5,int y5,int x6,int y6)
{
        int n_black,n_white,dead,i,j,k;

        n_black=n_white=dead=0;

        if(x1<0||x2<0||x3<0||x4<0||x5<0||x6<0||
           x1>14||x2>14||x3>14||x4>14||x5>14||x6>14)    return;

        if(winner!=0) return;

        if((playboard[x1][y1]!=0 && playboard[x6][y6]==0)||
           (playboard[x1][y1]==0 && playboard[x6][y6]!=0))
         dead=1; /*check 死四*/

        if(playboard[x2][y2]==black) n_black+=1;
        if(playboard[x2][y2]==white) n_white+=1;
        if(playboard[x3][y3]==black) n_black+=1;
        if(playboard[x3][y3]==white) n_white+=1;
        if(playboard[x4][y4]==black) n_black+=1;
        if(playboard[x4][y4]==white) n_white+=1;
        if(playboard[x5][y5]==black) n_black+=1;
        if(playboard[x5][y5]==white) n_white+=1;

        if(playboard[x1][y1]==0 && playboard[x6][y6]==0 &&
          (playboard[x3][y3]==0 || playboard[x4][y4]==0))
        {
          if(n_black==3 || n_white==3)
           {
             move(15,60);
             outs("▼活三!▼");
            }
           if(n_black==3)
           livethree+=1;
         }


        if(n_black==4)
         {
           if(playboard[x1][y1]== black && playboard[x6][y6]== black)
            bandhand(6);
           if(playboard[x1][y1]!=0 && playboard[x6][y6]!=0) return;
           bell();
           if(dead)
            {
              move(15,60);
              outs("▼小心死四!▼");
              tdeadf+=1;
              tlivef+=1;/*黑死四啦*/
              threefour=0;
              return;
            }


           threefour=black; tlivef+=1; /*活四也算雙四*/
         }
        if(n_white==4)
         {
           if(playboard[x1][y1]!=0 && playboard[x6][y6]!=0) return;
           bell();
           if(dead)
            {
              move(15,60);
              tdeadf+=1;
              outs("▼小心死四!▼");
              threefour=0;
              return;
            }

           threefour=white; tlivef+=1;

          }
        if(playboard[x1][y1]==black) n_black+=1;/*check 連子*/
        if(playboard[x6][y6]==black) n_black+=1;

        if(n_black==5 && (playboard[x3][y3]==0 || playboard[x4][y4]==0 ||
           playboard[x5][y5]==0 || playboard[x2][y2]==0))
           tlivef-=1;/*六缺一型*/

        if(n_black>=6)   bandhand(6);
        return;
}

int bandhand(int style)
{
        bell();
        if(style==3){
            move(10,35);
            outs("[31m┌黑敗──────┐[m");
            move(11,35);
            outs("[31m│  [37;41m黑禁手雙活三[m  [31m│[m");
            move(12,35);
            outs("[31m└────────┘[m");
           }
        else if(style==4){
            move(10,35);
            outs("[31m┌黑敗──────┐[m");
            move(11,35);
            outs("[31m│  [37;41m黑禁手雙  四[m  [31m│[m");
            move(12,35);
            outs("[31m└────────┘[m");
           }
        else {
            move(10,35);
            outs("[31m┌黑敗──────┐[m");
            move(11,35);
            outs("[31m│  [37;41m黑禁手連六子[m  [31m│[m");
            move(12,35);
            outs("[31m└────────┘[m");
           }

            igetkey();
            winner=white;
        return;
}

int liveagogo(int sk)
{
              move(15,60);
              outs("▼哇咧活四!▼");        

        return;
}


char save_page_requestor[40];

int five_pk(fd,first)
int fd;
int first;
{
        int cx, ch, cy,datac,fdone,x,y;
        char genbuf[100],data[9000],xy_po[5],genbuf1[20],x1[1],y1[1],done[1];
        user_info *opponent;
        char fname[50];
        int i,j,k,fway=1,banf=1,idone=0;
        int t_win,t_loss,t_score;
        FILE *fp;

        opponent=currutmp->destuip;

        currutmp->msgs[0].last_pid=opponent->pid;
        for(i=0;i<=14;i++)
         for(j=0;j<=14;j++)
           playboard[i][j]=0;

        setutmpmode(FIVE);/*檔案處理*/
        setuserfile(fname, FDATA);
        if ((fp = fopen(fname, "r")) == NULL)
         {
           setuserfile(fname, FDATA);
           fp=fopen(fname,"w");
           fprintf(fp,"%d  %d  %d",0,0,0);
           fclose(fp);
           t_win=t_loss=t_score=0;
         }
        else
         {
           fscanf(fp,"%d  %d  %d",&t_win,&t_loss,&t_score);
           fclose(fp);
         }


        hand=1;
        winner=0;
        quitf=0;
        px=14;
        py=7;
        clear();

        move(0,0);
        outs("┌┬┬┬┬┬┬┬┬┬┬┬┬┬┐15
├┼┼┼┼┼┼┼┼┼┼┼┼┼┤14
├┼┼┼┼┼┼┼┼┼┼┼┼┼┤13
├┼┼＋┼┼┼┼┼┼┼＋┼┼┤12
├┼┼┼┼┼┼┼┼┼┼┼┼┼┤11
├┼┼┼┼┼┼┼┼┼┼┼┼┼┤10
├┼┼┼┼┼┼┼┼┼┼┼┼┼┤9
├┼┼┼┼┼┼＋┼┼┼┼┼┼┤8
├┼┼┼┼┼┼┼┼┼┼┼┼┼┤7
├┼┼┼┼┼┼┼┼┼┼┼┼┼┤6
├┼┼┼┼┼┼┼┼┼┼┼┼┼┤5
├┼┼＋┼┼┼┼┼┼┼＋┼┼┤4
├┼┼┼┼┼┼┼┼┼┼┼┼┼┤3
├┼┼┼┼┼┼┼┼┼┼┼┼┼┤2
└┴┴┴┴┴┴┴┴┴┴┴┴┴┘1
 A B C D E F G H I J K L M N O");

        sprintf(genbuf, "%s(%s)", cuser.userid, cuser.username);
        user_guide();
        move(0,33);
        outs("[35;43m◆五子棋對戰◆[30;42m  程式:成大電機88級 Birdman  [m");
        if(first){
         move(1,33);
         prints("黑●先手 %s  ",genbuf);
         move(2,33);
         prints("VS白○手 %s  ",save_page_requestor);
        }
        else{
         move(1,33);
         prints("黑●先手 %s  ",save_page_requestor);
         move(2,33);
         prints("VS白○手 %s  ",genbuf);
        }

        add_io(fd, 0);

         move(15,40);
         if(first)
          outs("★等待對方下子★");
         else
          outs("◆現在換自己下◆");
         move(7,14);
         outs("●");
         player=white;
         playboard[7][7]=black;
         chess[1][0]=14; /*紀錄所下位址*/
         chess[1][1]=7;
         move(4,40);
         outs("第 1手 ●H 8");

        if(!first) /*超怪!*/
         fdone=1;
        else fdone=0;/*對手完成*/

        do
        {
         ch=igetkey();

         if (ch == I_OTHERDATA)
          {
           datac = recv(fd, data, sizeof(data), 0);
           if (datac <= 0){
            move(13,30);
            outs("【 [31;47m對方投降了...@_@[m 】");
            move(14,30);
            outs("【 [31;47m對方投降了...@_@[m 】");
            move(12,30);
            outs("【 [31;47m對方投降了...@_@[m 】");
            break;
           }
           i=atoi(data);
           cx=i/1000;   /*解譯data成棋盤資料*/
           cy=(i%1000)/10;
           fdone=i%10;
           hand+=1;

           if(hand%2==0)
            move(((hand-1)%22)/2+4,54);
           else
            move(((hand-1)%21)/2+4,40);

           prints("第%2d手 %s%c%2d",hand,
             (player==black)?"●":"○",abcd[cx/2],15-cy);


           move(cy,cx);
           x=cx/2;
           playboard[x][cy]=player;
           if(player==black)
           {
            outs("●");
            player=white;
            }
           else
           {
            outs("○");
            player=black;
            }
              move(15,60);
              outs("              ");
           tdeadf=tlivef=livethree=threefour=0;
           for(j=0;j<=10;j++)
             calvalue(cx/2,j,cx/2,j+1,cx/2,j+2,cx/2,j+3,cx/2,j+4);
           for(i=0;i<=10;i++)/*橫向*/
             calvalue(i,cy,i+1,cy,i+2,cy,i+3,cy,i+4,cy);
           for(i=-4;i<=0;i++)/*斜右下*/
             calvalue(cx/2+i,cy+i,cx/2+i+1,cy+i+1,cx/2+i+2,cy+i+2,
                      cx/2+i+3,cy+i+3,cx/2+i+4,cy+i+4);
           for(i=-4;i<=0;i++)/*斜左下*/
             calvalue(cx/2-i,cy+i,cx/2-i-1,cy+i+1,cx/2-i-2,cy+i+2,cx/2-i-3,
                      cy+i+3,cx/2-i-4,cy+i+4);

            for(j=0;j<=9;j++)
             callfour(cx/2,j,cx/2,j+1,cx/2,j+2,cx/2,j+3,cx/2,j+4,cx/2,j+5);
           for(i=0;i<=9;i++)/*四橫向*/
             callfour(i,cy,i+1,cy,i+2,cy,i+3,cy,i+4,cy,i+5,cy);
           for(i=-5;i<=0;i++){/*四斜右下*/
             callfour(cx/2+i,cy+i,cx/2+i+1,cy+i+1,cx/2+i+2,cy+i+2,
                      cx/2+i+3,cy+i+3,cx/2+i+4,cy+i+4,cx/2+i+5,cy+i+5);
                            /*四斜左下*/
             callfour(cx/2-i,cy+i,cx/2-i-1,cy+i+1,cx/2-i-2,cy+i+2,cx/2-i-3,
                      cy+i+3,cx/2-i-4,cy+i+4,cx/2-i-5,cy+i+5);
             }

           if(tlivef>=2 && winner==0)    bandhand(4);
           if(livethree>=2 && tlivef ==0) bandhand(3);
           if(threefour==black)          liveagogo(7);/*判斷活四要不要直接贏*/
           else if(threefour==white)     liveagogo(8);
          }
         else{
          if (ch == Ctrl('X')) {
                winner=1;
                genbuf1[0] = (char) ch;
                send(fd, genbuf1, -3, 0);
                 }
          if (ch =='t'){
              extern screenline* big_picture;
              screenline* screen0 = calloc(t_lines, sizeof(screenline));
              memcpy(screen0, big_picture, t_lines * sizeof(screenline));    
                add_io(0, 0);
                my_write(opponent->pid,"輸入訊息:");
                add_io( fd, 0);
              memcpy(big_picture, screen0, t_lines * sizeof(screenline));
              free(screen0);
              redoscr();          
                clrtoeol();
                continue;
          }
         }

         if(fdone==1)/*換我*/
         {

              move(15,40);
              outs("◆現在換自己下◆");

           if(winner!=0) break;

           move(py,px);
          switch (ch)
          {

            case KEY_DOWN:
             py=py+1;
             if(py>14) py=0;
            break;

            case KEY_UP:
             py=py-1;
             if(py<0) py=14;
            break;

            case KEY_LEFT:
             px=px-1;
             if(px<0) px=28;
            break;

            case KEY_RIGHT:
             px=px+1;
             if(px>28) {px=0;px=px-1;}  /*會跳格咧*/
            break;

            case 'q':
            case 'Q':
             quitf=1;
             getdata(b_lines - 1, 0, "您確定要離開嗎？(沒下完跑掉很沒禮貌喔!!!)(Y/N)?[N] ",
                genbuf, 2, LCECHO, 0);

             if (*genbuf != 'y')
              {
              quitf=0;
              move(b_lines-1,0);
              outs("                                                    ");
              }
             else{
              quitf=1;
                genbuf1[0] = (char) ch;
                send(fd, genbuf1, -3, 0);
                }
            break;

            case ' ':
                if(banf==1) break;

                if((px%2)==1) px=px-1; /*解決netterm不合問題*/
                move(py,px);
                hand+=1;
                playboard[x][py]=player;
                if(player==black)
                 {
                  outs("●");
                  player=white;
                 }
                else
                {
                 outs("○");
                 player=black;
                }
                chess[hand][0]=px;
                chess[hand][1]=py;
           if(hand%2==0)
            move(((hand-1)%22)/2+4,54);
           else
            move(((hand-1)%21)/2+4,40);

           prints("第%2d手 %s%c%2d",hand,
             (hand%2==1)?"●":"○",abcd[px/2],15-py);
             idone=1;
            break;

            default:
            break;
           }
        move(py,px);
          x=px/2;
          if(playboard[x][py]!=0)
              banf=1;
          else
              banf=0;

        if(idone==1){
           switch(px){
                case 0:
                 xy_po[0]='0';
                 xy_po[1]='0';
                break;
                case 2:
                 xy_po[0]='0';
                 xy_po[1]='2';
                break;
                case 4:
                 xy_po[0]='0';
                 xy_po[1]='4';
                break;
                case 6:
                 xy_po[0]='0';
                 xy_po[1]='6';
                break;
                case 8:
                 xy_po[0]='0';
                 xy_po[1]='8';
                break;
                case 10:
                 xy_po[0]='1';
                 xy_po[1]='0';
                break;
                case 12:
                 xy_po[0]='1';
                 xy_po[1]='2';
                break;
                case 14:
                 xy_po[0]='1';
                 xy_po[1]='4';
                break;
                case 16:
                 xy_po[0]='1';
                 xy_po[1]='6';
                break;
                case 18:
                 xy_po[0]='1';
                 xy_po[1]='8';
                break;
                case 20:
                 xy_po[0]='2';
                 xy_po[1]='0';
                break;
                case 22:
                 xy_po[0]='2';
                 xy_po[1]='2';
                break;
                case 24:
                 xy_po[0]='2';
                 xy_po[1]='4';
                break;
                case 26:
                 xy_po[0]='2';
                 xy_po[1]='6';
                break;
                case 28:
                 xy_po[0]='2';
                 xy_po[1]='8';
                break;
               }
           switch(py){
                case 0:
                 xy_po[2]='0';
                 xy_po[3]='0';
                break;
                case 1:
                 xy_po[2]='0';
                 xy_po[3]='1';
                break;
                case 2:
                 xy_po[2]='0';
                 xy_po[3]='2';
                break;
                case 3:
                 xy_po[2]='0';
                 xy_po[3]='3';
                break;
                case 4:
                 xy_po[2]='0';
                 xy_po[3]='4';
                break;
                case 5:
                 xy_po[2]='0';
                 xy_po[3]='5';
                break;
                case 6:
                 xy_po[2]='0';
                 xy_po[3]='6';
                break;
                case 7:
                 xy_po[2]='0';
                 xy_po[3]='7';
                break;
                case 8:
                 xy_po[2]='0';
                 xy_po[3]='8';
                break;
                case 9:
                 xy_po[2]='0';
                 xy_po[3]='9';
                break;
                case 10:
                 xy_po[2]='1';
                 xy_po[3]='0';
                break;
                case 11:
                 xy_po[2]='1';
                 xy_po[3]='1';
                break;
                case 12:
                 xy_po[2]='1';
                 xy_po[3]='2';
                break;
                case 13:
                 xy_po[2]='1';
                 xy_po[3]='3';
                break;
                case 14:
                 xy_po[2]='1';
                 xy_po[3]='4';
                break;
               }
              fdone=0;
              xy_po[4]='1';
              if(send(fd,xy_po,strlen(xy_po),0)==-1)
                break;

              move(15,40);
              outs("★等待對方下子★");
              f_display();

              move(15,60);
              outs("              ");

           tdeadf=tlivef=livethree=threefour=0;
            for(j=0;j<=10;j++)
             calvalue(px/2,j,px/2,j+1,px/2,j+2,px/2,j+3,px/2,j+4);
           for(i=0;i<=10;i++)/*橫向*/
             calvalue(i,py,i+1,py,i+2,py,i+3,py,i+4,py);
           for(i=-4;i<=0;i++)/*斜右下*/
             calvalue(px/2+i,py+i,px/2+i+1,py+i+1,px/2+i+2,py+i+2,
                      px/2+i+3,py+i+3,px/2+i+4,py+i+4);
           for(i=-4;i<=0;i++)/*斜左下*/
             calvalue(px/2-i,py+i,px/2-i-1,py+i+1,px/2-i-2,py+i+2,px/2-i-3,
                      py+i+3,px/2-i-4,py+i+4);

            for(j=0;j<=9;j++)
             callfour(px/2,j,px/2,j+1,px/2,j+2,px/2,j+3,px/2,j+4,px/2,j+5);
           for(i=0;i<=9;i++)/*四橫向*/
             callfour(i,py,i+1,py,i+2,py,i+3,py,i+4,py,i+5,py);
           for(i=-5;i<=0;i++){/*四斜右下*/
             callfour(px/2+i,py+i,px/2+i+1,py+i+1,px/2+i+2,py+i+2,
                      px/2+i+3,py+i+3,px/2+i+4,py+i+4,px/2+i+5,py+i+5);
                            /*四斜左下*/
             callfour(px/2-i,py+i,px/2-i-1,py+i+1,px/2-i-2,py+i+2,px/2-i-3,
                      py+i+3,px/2-i-4,py+i+4,px/2-i-5,py+i+5);
             }

           if(tlivef>=2 && winner==0)    bandhand(4);
           if(livethree>=2 && tlivef ==0) bandhand(3);
           if(threefour==black)          liveagogo(7);/*判斷活四要不要直接贏*/
           else if(threefour==white)     liveagogo(8);

            }
           if(winner!=0){
           genbuf1[0] = (char) ch;
           if (send(fd, genbuf1, 1, 0) != 1)
              break; }
           idone=0;
         }
        }while((!quitf) && (winner==0));


   if(winner!=0 && quitf==0)
    {
        if(first && winner==black)
         {t_win+=1;t_score+=5;}
        else if( first && winner==white)
         {t_loss+=1;t_score+=1;}
        else if( !first && winner==black)
         {t_loss+=1;t_score+=1;}
        else if( !first && winner==white)
         {t_win+=1;t_score+=7;}
    }


   if ((fp = fopen(fname, "w")) == NULL)
     return;
    else{
     fprintf(fp,"%d  %d  %d",t_win,t_loss,t_score);
     fclose(fp);
     }

   quitf=0;
   add_io(0, 0);
   close(fd);
   pressanykey();
   return;
}
#define MAX_REVIEW 6
int
f_display()
{
  char buf[MAX_REVIEW][100],line=0,i,genbuf[100],x=1;
  FILE *fp;
  setuserfile(genbuf, fn_writelog);
  for(i=0;i<MAX_REVIEW;i++) buf[i][0]=0;
  if(fp=fopen(genbuf,"r"))
        {
         while(fgets(buf[line],99,fp))
               {
                        line = (line +1)%MAX_REVIEW;
               }
         line += MAX_REVIEW;
         move(16,0);
        outs(
"[33m───────訊息──────────────────────────────[m");
         for(i=1;i<=MAX_REVIEW; i++)
          {
            move(i+16,0);
            clrtoeol();
            if(!buf[(line-i)%MAX_REVIEW][0]) break;
            outs(buf[(line-i)%MAX_REVIEW]);
          }
/*          move(i+1,0);
          outs(
"───────────────────────────────────────"
*/          fclose(fp);
          return FULLUPDATE;
        }
  return DONOTHING;
}



