/*
 * Mine: '99 All-rewritten version
 *
 * Author: piaip
 *
 * $Log: mine.c,v $
 * Revision 1.1  1999/05/19 16:09:22  bbs
 * /home/bbs/src/maple: source original version
 *
 * Revision 1.3  1999/01/30 02:09:29  piaip
 * Better interface, keys help,... [beta]
 *
 * Revision 1.2  1999/01/29 13:23:28  piaip
 * For bbs version [alpha]
 *
 * Revision 1.1  1999/01/29 12:25:53  piaip
 * Initial revision
 *
 */

// 關於螢幕輸出
// 如果用 outs 會爛掉.
// 詢問過 MH 後了解到有幾種做法:
// 1) 把該處先填空白再重填 (好像也無效)
// 2) 整 '行' 重繪
// 3) 用 printf (redirected to socket) (要 flush) (可是我弄不出來)
// 仔細研究後發現與 bigpicture[] 有關, 而整行重繪事實上要先 clrtoeol 才是.
// 
// 又, 如果方向鍵 (左右) 會亂跳，把 cursor 擺在兩個 char 的中間。
// (也是 MH 的協助)
// 
// 由於使用 outs, 所以注意最好把你的 struct.h 中 screenline 的大小變大...
// 事實上, 我無法解決當輸出太多, 把 ANSILINELEN 用光(screenline)的情形

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "bbs.h"
#define _CHINESE_

enum {  MAP_MAXY = 20,
        MAP_MAXX = 30,

        // These are flags for bitwise operators
        TILE_BLANK = 0,
        TILE_MINE  = 1,
        TILE_TAGGED= 0x10,
        TILE_EXPAND= 0x20,

        TILE_NOUSE
        };
static int MAP_Y = MAP_MAXY, MAP_X = MAP_MAXX;

#ifndef __BORLANDC__
 #define random(x) (rand() % x)
#endif

// This function was testing how output works.
void pouts(const char *s) {
     // output(s, strlen(s) );
     // oflush();
     outs(s);
     // while(*s) ochar(*s++);
};

static int TotalMines = 0, TaggedMines = 0,currx = 0, curry = 0;
static char MineMap[ MAP_MAXY+2 ][ MAP_MAXX+2 ];

void initMap() {
    int x, y, i;

    for (y=0; y<MAP_Y+2; y++)
        for (x=0; x<MAP_X+2; x++)
        {
            MineMap[y][x] = TILE_BLANK;
            if(y==0 || x==0 || y==MAP_Y+1 || x==MAP_X+1)
                MineMap[y][x] |= TILE_EXPAND;
        };

    for (i=0; i<TotalMines; ) {
        x = random(MAP_X) + 1;
        y = random(MAP_Y) + 1;
        if(MineMap[y][x] == TILE_BLANK) {
            MineMap[y][x] = TILE_MINE;
            i++;
        };
    };
};

int countNeighbor(int y, int x, int bitmask) {
    /*
    return (MineMap[y-1][x+1] & TILE_MINE+
            MineMap[y-1][x  ] & TILE_MINE+
            MineMap[y-1][x-1] & TILE_MINE+
            MineMap[y  ][x+1] & TILE_MINE+
            MineMap[y  ][x  ] & TILE_MINE+
            MineMap[y  ][x-1] & TILE_MINE+
            MineMap[y+1][x+1] & TILE_MINE+
            MineMap[y+1][x  ] & TILE_MINE+
            MineMap[y+1][x-1] & TILE_MINE
            );
     */

    int sum = 0;
    if( MineMap[y-1][x+1] & bitmask ) ++sum;
    if( MineMap[y-1][x  ] & bitmask ) ++sum;
    if( MineMap[y-1][x-1] & bitmask ) ++sum;
    if( MineMap[y  ][x+1] & bitmask ) ++sum;
    if( MineMap[y  ][x  ] & bitmask ) ++sum;
    if( MineMap[y  ][x-1] & bitmask ) ++sum;
    if( MineMap[y+1][x+1] & bitmask ) ++sum;
    if( MineMap[y+1][x  ] & bitmask ) ++sum;
    if( MineMap[y+1][x-1] & bitmask ) ++sum;
    return sum;
};

const char *symTag=
#ifdef _CHINESE_
    //"[1;41;37mＭ[m";
    //"[1;41;37mΨ[m";
    //"Ｍ";
    "Ψ";
#else
    " M";
#endif

const char *symWrong=
#ifdef _CHINESE_
    "[1;41;37m〤[m";
#else
    " X";
#endif

const char *symBlank=
#ifdef _CHINESE_
    "■";
#else
    " ?";
#endif

const char *strMines[] = {
#ifdef _CHINESE_
    //"□",
    "　",
    "１",
    "２",
    "３",
    "４",
    "５",
    "６",
    "７",
    "８",
    ""
#else
    " _",
    " 1",
    " 2",
    " 3",
    " 4",
    " 5",
    " 6",
    " 7",
    " 8",
    ""
#endif
};

enum {  MAP_START_X = 16 };		// Must be > Prompts

static time_t init_time = 0;

void drawInfo() {
      move(b_lines-1, 0);
      clrtoeol();
      prints("所花時間: %.0lf 秒, 剩下 %d 個地雷未標記.\n",
		  difftime(time(0), init_time) ,TotalMines - TaggedMines);
};

void drawPrompt() {
      stand_title("Piaip's 踩地雷 $Revision: 1.1 $");
      move(3, 0);
      outs("按鍵說明:\n"
           "\n"
	   "移動     方向鍵\n"
	   "翻開     空白鍵\n"
	   "標記地雷   ｍ\n"
	   "掃雷       ｔ\n\n"
	   "離開    Esc / q\n"
	   );
};

void drawMapLine(int y, int flShow) {
    int x = 0;
     
    drawInfo();
    move(y+1, MAP_START_X);
    clrtoeol();
    for (x = 1; x <= MAP_X; x++) {

	  if( x== currx && y == curry) pouts("[44;31m");

	  if(MineMap[y][x] & TILE_TAGGED) {
	      if(flShow && (MineMap[y][x] & TILE_MINE) == 0 )
		  pouts(symWrong);
	      else pouts(symTag);
	  }
	  else if (MineMap[y][x] & TILE_EXPAND)
		  pouts( strMines[countNeighbor(y, x, TILE_MINE)] );
	  else if (flShow && (MineMap[y][x] & TILE_MINE) )
		  pouts(symTag);
	  else pouts(symBlank);

          if( x== currx && y == curry) pouts("[m");
    };
    
    move(curry+1, currx*2 +MAP_START_X -1);
};

void drawMap(int flShow) {
    int y;

    clear();
    drawPrompt();
    for (y=1; y<MAP_Y+1; y++) {
        drawMapLine(y, flShow);
    };
};

static int flLoseMine = 0;

static void loseMine() {
    drawMap(1);
    move(b_lines-1, 0);
    clrtoeol();
    outs("[1;37;44m你輸了[m");
    pressanykey(0);
    flLoseMine = 1;
};

void ExpandMap(int y, int x, int flTrace) {
    if(!flTrace)  {
          if(MineMap[y][x] & TILE_TAGGED || MineMap[y][x] & TILE_EXPAND) return;
          if((MineMap[y][x] & TILE_MINE) && (!(MineMap[y][x] & TILE_TAGGED)))
	  { loseMine(); return; };
          MineMap[y][x] |= TILE_EXPAND;
          drawMapLine(y, 0);
    };
    if(flTrace || countNeighbor(y, x, TILE_MINE)==0) {
        if( flTrace || (MineMap [y-1][x  ] & TILE_EXPAND) == 0)
            ExpandMap(y-1, x  ,0);
        if( flTrace || (MineMap [y  ][x-1] & TILE_EXPAND) == 0)
            ExpandMap(y  , x-1,0);
        if( flTrace || (MineMap [y+1][x  ] & TILE_EXPAND) == 0)
            ExpandMap(y+1, x  ,0);
        if( flTrace || (MineMap [y  ][x+1] & TILE_EXPAND) == 0)
            ExpandMap(y  , x+1,0);
        if( flTrace || (MineMap [y-1][x-1] & TILE_EXPAND) == 0)
            ExpandMap(y-1, x-1,0);
        if( flTrace || (MineMap [y+1][x-1] & TILE_EXPAND) == 0)
            ExpandMap(y+1, x-1,0);
        if( flTrace || (MineMap [y-1][x+1] & TILE_EXPAND) == 0)
            ExpandMap(y-1, x+1,0);
        if( flTrace || (MineMap [y+1][x+1] & TILE_EXPAND) == 0)
            ExpandMap(y+1, x+1,0);
    };
};

void TraceMap(int y, int x) {
      if(!(MineMap[y][x] & TILE_EXPAND)) return;
      if(countNeighbor(y, x, TILE_MINE) == countNeighbor(y, x, TILE_TAGGED) ) {
        ExpandMap(y, x, 1);
      };
};

void playMine() {
    int ch ;
    currx = MAP_X/2+1, curry = MAP_Y/2+1;
    flLoseMine = 0;

    drawMap(0);
    while( !flLoseMine && ((ch = igetkey()) != 'q') ) {

        switch(ch) {
	case KEY_ESC:
	      return;
	      break;
	      
	case KEY_UP:	
	      if(curry > 1) {
		    drawMapLine(curry--, 0);
		    drawMapLine(curry, 0);
	      };
	      break;

        case KEY_DOWN:  
	      if(curry < MAP_Y) {
		    drawMapLine(curry++, 0);
	      };
	      break;

        case KEY_LEFT: 
	     if(currx > 1) currx--; 
	     break;
	     
	case KEY_RIGHT:  
	     if(currx < MAP_X) currx++; 
	     break;
	     
	case Ctrl('P'):
	     drawMap(1);
	     pressanykey(0);
	     drawMap(0);
	     break;
	     
        case '\r':
        case 't':
        case '\n':
             TraceMap(curry, currx); 
	     break;
	     
        case ' ':  
             ExpandMap(curry, currx, 0); 
	     break;

        case 'm':  
	     if( (MineMap[curry][currx] & TILE_EXPAND) ) {
		   if(MineMap[curry][currx] & TILE_TAGGED)  {
                       TaggedMines--;
                       MineMap[curry][currx] ^= TILE_EXPAND;
                   } else
                     break;
             } else {
                   TaggedMines++;
                   MineMap[curry][currx] ^= TILE_EXPAND;
             };

             MineMap[curry][currx] ^= TILE_TAGGED;
             if(TaggedMines == TotalMines) return;
             break;
	     
        default:
             break;
        };
        drawMapLine(curry, 0);
    }
};

int Mine() {
    int x, y;
    char ans[5];

    stand_title("Piaip 踩地雷");
    outmsg("本程式由 piaip 發展中, 有任何建議請 mail to piaip.bbs@sob.twbbs.org.");
    while( (!getdata(2, 0, "你要玩 [1]入門 [2]進階 [3]高級: ", 
		ans, 4, DOECHO, "1")) && (atoi(ans)<1 || atoi(ans)>3) );
    switch( atoi(ans) ) {
	  case 1:
		MAP_X = 10;
		MAP_Y = 10;
		break;
	  case 2:
		MAP_X = 20;
		MAP_Y = 15;
		break;
	  case 3:
		MAP_X = 30;
		MAP_Y = 20;
		break;
    };

    TotalMines = (MAP_X/10)*(MAP_Y);
    TaggedMines = 0;
    initMap();
    init_time = time(0);
    playMine();
    if(!flLoseMine) {
	  for (y=1; y<MAP_Y+1; y++)
		for(x=1; x<MAP_X+1; x++)
		      if((MineMap[y][x] & TILE_MINE) && 
			 !(MineMap[y][x] & TILE_TAGGED))
		      { loseMine(); y=MAP_Y+1; break; }

	  if(!flLoseMine) {
		move(b_lines -1, 0);
		clrtoeol();
		outs("[1;37;44m你贏了!  ");
		prints("所花時間: %.0lf 秒[m\n", 
			    difftime( time(0), init_time ));
		pressanykey(0);
	  };
    };
    return RC_FULL;
};

