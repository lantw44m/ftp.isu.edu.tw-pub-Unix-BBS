/*-------------------------------------------------------*/
/* modes.h      ( NTHU CS MapleBBS Ver 2.36 )            */
/*-------------------------------------------------------*/
/* target : user operating mode & status                 */
/* create : 95/03/29                                     */
/* update : 02/08/11 (Dopin)                             */
/*-------------------------------------------------------*/

#ifndef _MODES_H_
#define _MODES_H_

/* ----------------------------------------------------- */
/* user 操作狀態與模式                                   */
/* ----------------------------------------------------- */

#define IDLE            0
#define MMENU           1       /* menu mode */
#define ADMIN           2
#define MAIL            3
#define TMENU           4
#define UMENU           5
#define XMENU           6
#define CLASS           7
#define LOGIN           8       /* main menu */
#define ANNOUNCE        9       /* announce */
#define POSTING         10      /* boards & class */

#define READBRD         11
#define READING         12
#define READNEW         13
#define SELECT          14
#define RMAIL           15      /* mail menu */
#define SMAIL           16
#define CHATING         17      /* talk menu */
#define XMODE           18
#define FRIEND          19
#define LAUSERS         20

#define LUSERS          21
#define MONITOR         22
#define PAGE            23
#define QUERY           24
#define TALK            25
#define EDITPLAN        26      /* user menu */
#define EDITSIG         27
#define VOTING          28
#define XINFO           29
#define MSYSOP          30

#define WWW             31
#define BRIDGE          32
#define REPLY           33
#define HIT             34
#define DBACK           35
#define NOTE            36
#define EDITING         37
#define MAILALL         38
#define ATSFUNC         39
#ifdef STAR_CHICKEN
#define CHICKEN_3       40
#endif

#define GLINK           41
#define TREE            42

#define BBSNET          0

#ifdef TAKE_IDLE
static char *IdleTypeTable[] =
{
  "偶在花呆啦",
  "情人來電",
  "覓食中",
  "拜見周公",
  "假死狀態",
  "我在思考"
};
#endif

#ifdef  _MODES_C_
static char *ModeTypeTable[] =
{
  "發呆",                       /* IDLE */
  "主選單",                     /* MMENU */
  "系統維護",                   /* ADMIN */
  "郵件選單",                   /* MAIL */
  "交談選單",                   /* TMENU */
  "使用者選單",                 /* UMENU */
  "XYZ 選單",                   /* XMENU */
  "分類看板",                   /* CLASS */
  "上站途中",                   /* LOGIN */
  "公佈欄",                     /* ANNOUNCE */
  "發表文章",                   /* POSTING */
  "看板列表",                   /* READBRD */
  "閱\讀文章",                  /* READING */
  "新文章列表",                 /* READNEW */
  "選擇看板",                   /* SELECT */
  "讀信",                       /* RMAIL */
  "寫信",                       /* SMAIL */
  "聊天室",                     /* CHATING */
  "其他",                       /* XMODE */
  "尋找好友",                   /* FRIEND */
  "上線使用者",                 /* LAUSERS */
  "使用者名單",                 /* LUSERS */
  "追蹤站友",                   /* MONITOR */
  "呼叫",                       /* PAGE */
  "查詢",                       /* QUERY */
  "交談",                       /* TALK  */
  "編名片檔",                   /* EDITPLAN */
  "編簽名檔",                   /* EDITSIG */
  "投票中",                     /* VOTING */
  "設定資料",                   /* XINFO */
  "寄給站長",                   /* MSYSOP */
  "汪汪汪",                     /* WWW */
  "打橋牌",                     /* BRIDGE */
  "回應",                       /* REPLY */
  "被水球打中",                 /* HIT */
  "水球準備中",                 /* DBACK */
  "筆記本",                     /* NOTE */
  "編輯文章",                   /* EDITING */
  "發系統通告",                 /* MAILALL */
  "本站功\能區",                /* ATSFUNC */
  "美少雞夢工場"                /* STAR CHICKEN */
};

#endif                          /* _MODES_C_ */

/* ----------------------------------------------------- */
/* menu.c 中的模式                                       */
/* ----------------------------------------------------- */

#define QUIT    0x666           /* Return value to abort recursive functions */
#define XEASY   0x333           /* Return value to un-redraw screen */

/* ----------------------------------------------------- */
/* read.c 中的模式                                       */
/* ----------------------------------------------------- */

#define RS_FORWARD      0x01    /* backward */
#define RS_TITLE        0x02    /* author/title */
#define RS_RELATED      0x04
#define RS_FIRST        0x08    /* find first article */
#define RS_CURRENT      0x10    /* match current read article */
#define RS_THREAD       0x20    /* search the first article */
#define RS_AUTHOR       0x40    /* search author's article */

#define CURSOR_FIRST    (RS_RELATED | RS_TITLE | RS_FIRST)
#define CURSOR_NEXT     (RS_RELATED | RS_TITLE | RS_FORWARD)
#define CURSOR_PREV     (RS_RELATED | RS_TITLE)
#define RELATE_FIRST    (RS_RELATED | RS_TITLE | RS_FIRST | RS_CURRENT)
#define RELATE_NEXT     (RS_RELATED | RS_TITLE | RS_FORWARD | RS_CURRENT)
#define RELATE_PREV     (RS_RELATED | RS_TITLE | RS_CURRENT)
#define THREAD_NEXT     (RS_THREAD | RS_FORWARD)
#define THREAD_PREV     (RS_THREAD)
#define AUTHOR_NEXT     (RS_AUTHOR | RS_FORWARD)
#define AUTHOR_PREV     (RS_AUTHOR)

#define DONOTHING       0       /* Read menu command return states */
#define FULLUPDATE      1       /* Entire screen was destroyed in this oper */
#define PARTUPDATE      2       /* Only the top three lines were destroyed */
#define DOQUIT          3       /* Exit read menu was executed */
#define NEWDIRECT       4       /* Directory has changed, re-read files */
#define READ_NEXT       5       /* Direct read next file */
#define READ_PREV       6       /* Direct read prev file */
#define DIRCHANGED      8       /* Index file was changed */
#define READ_REDRAW     9
#define PART_REDRAW     10

/* woju */
#define POS_NEXT        101     /* cursor_pos(locmem, locmem->crs_ln + 1, 1);*/

/* for currmode */
#define MODE_STARTED    1       /* 是否已經進入系統 */
#define MODE_POST       2       /* 是否可以在 currboard 發表文章 */
#define MODE_BOARD      4       /* 是否可以在 currboard 刪除、mark文章 */
#define MODE_DIGEST     0x10    /* 是否為 digest mode */
#define MODE_ETC        0x20    /* 是否為 etc mode */
#define MODE_SELECT     0x40    /* 是否為 select mode */
#define MODE_DIRTY      0x80    /* 是否更動過 userflag */

/* for curredit */
#define EDIT_MAIL       1       /* 目前是 mail/board ? */
#define EDIT_LIST       2       /* 是否為 mail list ? */
#define EDIT_BOTH       4       /* both reply to author/board ? */
#endif                          /* _MODES_H_ */
