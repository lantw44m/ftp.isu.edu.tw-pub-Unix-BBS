/*--------------------------------------------------------------------------*/
/*  小雞參數設定                      					    */
/*--------------------------------------------------------------------------*/
struct chicken {
 /*---基本的資料---*/ //14
 char name[20];		/*姓    名*/
 char birth[21];	/*生    日*/
 int year;		/*生日  年*/
 int month;		/*生日  月*/
 int day; 		/*生日  日*/
 int sex;		/*性    別 1:♂   2:♀  */
 int death;             /*1:  死亡 2:拋棄 3:結局*/
 int nodone;		/*1:  未做*/
 int relation;		/*兩人關係*/
 int liveagain;		/*復活次數*/
 int dataB;
 int dataC;
 int dataD;
 int dataE;
  
 /*---身體的參數---*/ //12
 int hp;		/*體    力*/
 int maxhp;             /*最大體力*/
 int weight;            /*體    重*/
 int tired;		/*疲 勞 度*/
 int sick;		/*病    氣*/
 int shit;		/*清 潔 度*/ 
 int wrist;		/*腕    力*/
 int bodyA;
 int bodyB;
 int bodyC;
 int bodyD;
 int bodyE;
 
 /*---評價的參數---*/ //9
 int social;		/*社交評價*/
 int family;		/*家事評價*/
 int hexp;		/*戰鬥評價*/
 int mexp;		/*魔法評價*/
 int tmpA;
 int tmpB;
 int tmpC;
 int tmpD;
 int tmpE;
 
 /*---戰鬥用參數---*/ //13
 int mp;		/*法    力*/
 int maxmp;             /*最大法力*/
 int attack;		/*攻 擊 力*/
 int resist;		/*防 禦 力*/
 int speed;		/*速    度*/
 int hskill;		/*戰鬥技術*/
 int mskill;		/*魔法技術*/
 int mresist;		/*抗魔能力*/
 int magicmode;		/*魔法型態*/
 int fightB;
 int fightC;
 int fightD;
 int fightE;
 

 /*---武器的參數---*/ //10
 int weaponhead;	/*頭部武器*/
 int weaponrhand;	/*右手武器*/
 int weaponlhand;	/*左手武器*/
 int weaponbody;	/*身體武器*/
 int weaponfoot;	/*腳的武器*/ 
 int weaponA;
 int weaponB;
 int weaponC;
 int weaponD;
 int weaponE;
 
 /*---各能力參數---*/ //17
 int toman;		/*待人接物*/ 
 int character;		/*氣 質 度*/ 
 int love;		/*愛    心*/ 
 int wisdom;		/*智    慧*/
 int art;		/*藝術能力*/
 int etchics;		/*道    德*/
 int brave;		/*勇    敢*/
 int homework;		/*掃地洗衣*/
 int charm;		/*魅	力*/
 int manners;		/*禮    儀*/
 int speech;		/*談	吐*/
 int cookskill;		/*烹    飪*/
 int learnA;
 int learnB;
 int learnC;
 int learnD;
 int learnE;
 
 
 /*---各狀態數值---*/ //11
 int happy;		/*快 樂 度*/
 int satisfy;		/*滿 意 度*/
 int fallinlove;	/*戀愛指數*/
 int belief;		/*信    仰*/
 int offense;		/*罪    孽*/
 int affect;		/*感    受*/
 int stateA;
 int stateB;
 int stateC;
 int stateD;
 int stateE;
 
 /*---吃的東西啦---*/ //9
 int food;		/*食    物*/
 int medicine;          /*靈    芝*/
 int bighp;             /*大 補 丸*/
 int cookie;		/*零    食*/
 int ginseng;		/*千年人蔘*/
 int snowgrass;		/*天山雪蓮*/
 int eatC;
 int eatD;
 int eatE;
 
 /*---擁有的東西---*/ //8
 int book;		/*書    本*/
 int playtool; 		/*玩    具*/
 int money;		/*金    錢*/
 int thingA;		
 int thingB;		
 int thingC;
 int thingD;
 int thingE; 
 
 /*---猜拳的參數---*/ //2
 int winn;		
 int losee;
 
 /*---參見王臣--*/ //16
 int royalA;		/*from守衛*/
 int royalB;		/*from近衛*/
 int royalC;		/*from將軍*/
 int royalD;		/*from大臣*/
 int royalE;		/*from祭司*/ 		
 int royalF;		/*from寵妃*/
 int royalG;		/*from王妃*/
 int royalH;  		/*from國王*/
 int royalI;		/*from小丑*/
 int royalJ;		/*from王子*/
 int seeroyalJ;		/*是否已經看過王子了*/
 int seeA;
 int seeB;
 int seeC;
 int seeD;
 int seeE;

 /*---結局----*///2
 int wantend;		/*20歲結局*/
 /*1:不要且未婚 2:不要且已婚  3:不要且當第三者 4:要且未婚  5:要且已婚 6:要且當第三者*/
 int lover;		/*愛人*/
 /*0:沒有 1:魔王 2:龍族 3:A 4:B 5:C 6:D 7:E */ 
 
 /*-------工作次數--------*/
 int workA; /*家事*/
 int workB; /*保姆*/
 int workC; /*旅店*/
 int workD; /*農場*/
 int workE; /*餐廳*/
 int workF; /*教堂*/
 int workG; /*地攤*/
 int workH; /*伐木*/
 int workI; /*美髮*/
 int workJ; /*獵人*/
 int workK; /*工地*/
 int workL; /*守墓*/
 int workM; /*家教*/
 int workN; /*酒家*/
 int workO; /*酒店*/
 int workP; /*夜*/
 int workQ;
 int workR;
 int workS;
 int workT;
 int workU;
 int workV;
 int workW;
 int workX;
 int workY;
 int workZ;
 
 /*-------上課次數--------*/
 int classA;
 int classB;
 int classC;
 int classD;
 int classE;
 int classF;
 int classG;
 int classH; 
 int classI;
 int classJ;
 int classK;
 int classL;
 int classM;
 int classN;
 int classO;
 
 /*---小雞的時間---*/ //1
 time_t bbtime;
};
typedef struct chicken chicken;

/*--------------------------------------------------------------------------*/
/*  選單的設定                                                              */
/*--------------------------------------------------------------------------*/
struct pipcommands
{
	int (*fptr) ();
	int key1 ;
	int key2 ;
};
typedef struct pipcommands pipcommands;

/*--------------------------------------------------------------------------*/
/*  怪物參數設定                                                            */
/*--------------------------------------------------------------------------*/
struct playrule
{
  char *name;           /*名字*/  
  int hp;		/*體力*/
  int maxhp; 		/*MAXHP*/
  int mp;		/*法力*/
  int maxmp;		/*MAXMP*/
  int attack;		/*攻擊*/
  int resist;		/*防護*/
  int speed;		/*速度*/
  int money;		/*財寶*/
  char special[5];	/*特別*/  /*1:怕火 2:怕冰 */
  int map;		/*圖檔*/
  int death;		/*死活*/
};
typedef struct playrule playrule;

/* d.special的定義            */
/* 00000 第一系的魔法傷害較小 */
/* 10000 第一系的魔法傷害較大 */
/* 11000 一二系的魔法傷害較大 */
/* 以下類推                    */
/* 雷系 冰系 火系 土系 風系 */
/*struct playrule goodmanlist[] = {
}*/ 

#define PIP_BADMAN       (3)            /* 怪物整體個數  */   

/*--------------------------------------------------------------------------*/
/*  魔法參數設定                                                            */
/*--------------------------------------------------------------------------*/
struct magicset
{
  char *name;           /*魔法的名字*/ 
  int needmp;		/*需要的法力*/
  int hpmode;		/*0:減少 1:增加 2:最大值*/
  int hp;		/*殺傷生命力*/
  int tiredmode;        /*0:減少 1:增加 2:最小值*/
  int tired;		/*疲勞度改變*/
  int map;		/*圖檔*/
};
typedef struct magicset magicset;


/*--------------------------------------------------------------------------*/
/*  參見王臣參數設定                                                        */
/*--------------------------------------------------------------------------*/
struct royalset
{
  char *num;		/*代碼*/
  char *name;           /*王臣的名字*/ 
  int needmode;		/*需要的mode*/ /*0:不需要 1:禮儀 2:談吐*/
  int needvalue;	/*需要的value*/
  int addtoman;		/*最大的增加量*/
  int maxtoman;		/*庫存量*/
  char *words1;
  char *words2;
};
typedef struct royalset royalset;

/*--------------------------------------------------------------------------*/
/*  物品參數設定                                                            */
/*--------------------------------------------------------------------------*/
struct goodsofpip
{
   int num;		/*編號*/
   char *name;		/*名字*/
   char *msgbuy;	/*功用*/
   char *msguse;	/*說明*/
   int money;		/*金錢*/
   int change;		/*改變量*/
   int pic1;
   int pic2;
};
typedef struct goodsofpip goodsofpip;

struct goodsofpip pipfoodlist[] = {
0,"物品名",	"說明buy",	"說明feed",			0,	0,	0,0,
1,"好吃的食物",	"體力恢復50",	"每吃一次食物會恢復體力50喔!",	50,	50,	1,1,
2,"美味的零食",	"體力恢復100",	"除了恢復體力，小雞也會更快樂",	120,	100,	2,3,
0,NULL,NULL,NULL,0,0,0,0
};

struct goodsofpip pipmedicinelist[] = {
0,"物品名",	"說明buy",	"說明feed",			0,	0,	0,0,
1,"好用大補丸",	"體力恢復600",	"恢復大量流失體力的良方",	500,	600,	4,4,
2,"珍貴的靈芝",	"法力恢復50",	"每吃一次靈芝會恢復法力50喔!",	100,	50,	7,7,
3,"千年人參王",	"法力恢復500",	"恢復大量流失法力的良方",	800,	500,	7,7,
4,"天山雪蓮",	"法力體力最大",	"這個  好貴......",		10000,	0,	7,7,
0,NULL,NULL,NULL,0,0,0,0
};

struct goodsofpip pipotherlist[] = {
0,"物品名",	"說明buy",	"說明feed",			0,	0,	0,0,
1,"樂高玩具組",	"快樂滿意度",	"玩具讓小雞更快樂啦...",	50,	0,	5,5,
2,"百科全書",	"知識的來源",	"書本讓小雞更聰明更有氣質啦...",100,	0,	6,6,
0,NULL,NULL,NULL,0,0,0,0
};

/*--------------------------------------------------------------------------*/
/*  武器參數設定                                                            */
/*--------------------------------------------------------------------------*/
struct weapon
{
  char *name;           /*名字*/  
  int needmaxhp;	/*需要hp*/
  int needmaxmp;	/*需要mp*/
  int needspeed;	/*需要的speed*/
  int attack;		/*攻擊*/
  int resist;		/*防護*/
  int speed;		/*速度*/
  int cost;		/*買價*/
  int sell;		/*賣價*/
  int special;		/*特別*/
  int map;		/*圖檔*/

};
typedef struct weapon weapon;

/*名字,需hp,需mp,需speed,攻擊,防護,速度,買價,賣價,特別,圖檔*/
struct weapon headlist[] = {
"不買裝備",  0,  0,  0,  0,  0,  0,     0,     0,0,0,
"塑膠帽子",  0,  0,  0,  0,  5,  0,   500,   300,0,0,	
"牛皮小帽",  0,  0,  0,  0, 10,  0,  3500,  1000,0,0,
"㊣安全帽", 60,  0,  0,  0, 20,  0,  5000,  3500,0,0,
"鋼鐵頭盔",150, 50,  0,  0, 30,  0, 10000,  6000,0,0,
"魔法髮箍",100,150,  0,  0, 25,  0, 50000, 10000,0,0, 
"黃金聖盔",300,300,300,  0,100,  0,300000,100000,0,0,
NULL,        0,  0,  0,  0,  0,  0,   0,   0,0,0
};

/*名字,需hp,需mp,需speed,攻擊,防護,速度,買價,賣價,特別,圖檔*/
struct weapon rhandlist[] = {
"不買裝備",  0,  0,  0,  0,  0,  0,     0,     0,0,0,
"大木棒",    0,  0,  0,  5,  0,  0,  1000,   700,0,0,	
"金屬扳手",  0,  0,  0, 10,  0,  0,  2500,  1000,0,0,
"青銅劍",   50,  0,  0, 20,  0,  0,  6000,  4000,0,0,
"晴雷劍",   80,  0,  0, 30,  0,  0, 10000,  8000,0,0,
"蟬翼刀",  100, 20,  0, 40,  0,  0, 15000, 10000,0,0, 
"忘情劍",  100, 40,  0, 35, 20,  0, 15000, 10000,0,0,
"獅頭寶刀",150,  0,  0, 60,  0,  0, 35000, 20000,0,0,
"屠龍刀",  200,  0,  0,100,  0,  0, 50000, 25000,0,0,
"黃金聖杖",300,300,300,100, 20,  0,150000,100000,0,0,
NULL,        0,  0,  0,  0,  0,  0,    0,   0,0,0
};

/*名字,需hp,需mp,需speed,攻擊,防護,速度,買價,賣價,特別,圖檔*/
struct weapon lhandlist[] = {
"不買裝備",  0,  0,  0,  0,  0,  0,     0,     0,0,0,
"大木棒",    0,  0,  0,  5,  0,  0,  1000,   700,0,0,	
"金屬扳手",  0,  0,  0, 10,  0,  0,  1500,  1000,0,0,
"木盾",	     0,  0,  0,  0, 10,  0,  2000,  1500,0,0,
"不鏽鋼盾", 60,  0,  0,  0, 25,  0,  5000,  3000,0,0,
"白金之盾", 80,  0,  0, 10, 40,  0, 15000, 10000,0,0,
"魔法盾",   80,100,  0, 20, 60,  0, 80000, 50000,0,0,
"黃金聖盾",300,300,300, 30,100,  0,150000,100000,0,0,
NULL,        0,  0,  0,  0,  0,  0,    0,   0,0,0
};

/*名字,需hp,需mp,需speed,攻擊,防護,速度,買價,賣價,特別,圖檔*/
struct weapon bodylist[] = {
"不買裝備",  0,  0,  0,  0,  0,  0,     0,     0,0,0,
"塑膠冑甲", 40,  0,  0,  0,  5,  0,  1000,   700,0,0,	
"特級皮甲", 50,  0,  0,  0, 10,  0,  2500,  1000,0,0,
"鋼鐵盔甲", 80,  0,  0,  0, 25,  0,  5000,  3500,0,0,
"魔法披風", 80, 40,  0,  0, 20, 20, 15500, 10000,0,0,
"白金盔甲",100, 30,  0,  0, 40, 20, 30000, 20000,0,0, 
"黃金聖衣",300,300,300, 30,100,  0,150000,100000,0,0,
NULL,        0,  0,  0,  0,  0,  0,     0,   0,0,0
};

/*名字,需hp,需mp,需speed,攻擊,防護,速度,買價,賣價,特別,圖檔*/
struct weapon footlist[] = {
"不買裝備",  0,  0,  0,  0,  0,  0,     0,     0,0,0,
"塑膠拖鞋",  0,  0,  0,  0,  0, 10,   800,   500,0,0,
"東洋木屐",  0,  0,  0, 15,  0, 10,  1000,   700,0,0, 	
"特級雨鞋",  0,  0,  0,  0, 10, 10,  1500,  1000,0,0,
"NIKE運動鞋",70, 0,  0,  0, 10, 40,  8000,  5000,0,0,
"鱷魚皮靴", 80, 20,  0, 10, 25, 20, 12000,  8000,0,0,
"飛天魔靴",100,100,  0, 30, 50, 60, 25000, 10000,0,0,
"黃金聖靴",300,300,300, 50,100,100,150000,100000,0,0,
NULL,        0,  0,  0,  0,  0,  0,    0,   0,0,0
};

struct royalset royallist[] = {
"T",	"拜訪對象",	  0,	0,	 0,	  0,NULL,NULL,
"A",	"風塵騎兵連",	  1,	10,	15,    	100,"你真好，來陪我聊天..","守衛星空的安全是很辛苦的..",
"B",	"風塵００７",	  1,   100,	25,	200,"真是禮貌的小雞..我喜歡...","特務就是秘密保護站長安全的人..",
"C",	"鎮國大將軍",	  1,   200,	30,	250,"告訴你唷！當年那個戰役很精彩喔..","你真是高貴優雅的小雞...",
"D",	"參謀總務長",	  1,   300,	35,	300,"我幫站長管理這個國家唷..","你的聲音很好聽耶..我很喜歡喔...:)",
"E",	"小夢站長",	  1,   400,	35,	300,"你很有教養唷！很高興認識你...","優雅的你，請讓我幫你祈福....",
"F",	"海妖站長",	  1,   500,	40,	350,"你好可愛喔..我喜歡你唷....","對啦..以後要多多來和我玩喔...",
"G",	"咩咩站長",	  1,   550,	40,	350,"跟你講話很快樂喔..不像站長一樣無聊..","來，坐我膝蓋\上，聽我講故事..",
"H",	"大魚站長",	  1,   600,	50,     400,"一站之長責任重大呀..:)..","謝謝你聽我講話..以後要多來喔...",
"I",	"星空灌水群",	  2,    60,	 0,	  0,"不錯唷..蠻機靈的喔..很可愛....","來  我們一起來灌水吧....",
"J",	"青年帥武官",	  0,	 0,	 0,	  0,"你好，我是武官，剛從銀河邊境回來休息..","希望下次還能見到你...:)",
NULL,		NULL,NULL,    NULL,    NULL,NULL,NULL
};

struct playrule badmanlist[] = {
"榕樹怪",	 60,0,	20,0,	20,	20,	 20,	150, "11101",	0,0,
"網路魔",	 60,0,	20,0,	30,	30,	 30,	200, "01111",	0,0,
"蘑菇小怪",	 80,0,	40,0,	50,	35,	 60,	250, "11110",	0,0,
"毒蠍",		 85,0,	30,0,	80,	90,	 80,	500, "10111",	0,0,
"惡狗",		 90,0,  50,0,   75,	70,	 60,	550, "11010",	0,0,
"紅眼鬼貓",	130,0,	50,0,	75,	90,	 70,	500, "11011",	0,0,
"紫色魔鯊",	140,0,	60,0,	80,	80,	 80,	550, "10101",	0,0,
"怪物蟑螂",	150,0,	70,0,	85,	70,	 67,	500, "11110",	0,0,
"蜘蛛精",	180,0,	50,0,   90,	90,	 80,	850, "00111",	0,0,
"食人巫師",	175,0, 100,0,  100,	80,	 60,    800, "11010",	0,0,
"大槌怪",	240,0,  80,0,  110,    100,	 70,    800, "00111",	0,0,
"白色惡魔",	250,0,  60,0,  120,    110,	 80,    900, "01011",	0,0,
"死神魔",	280,0,  80,0,  150,    120,	 90,   1200, "00011",   0,0,
"大恐龍",	300,0,	50,0,  160,    120,	 90,   1500, "11001",	0,0,
"超級噴火龍",	500,0, 100,0,  250,    250,	150,   1500, "11000",	0,0,
"骷髏\頭怪",	600,0, 400,0,  350,    400,	250,   2000, "00110",	0,0,
"阿強一號",	700,0, 500,0,  600,    900,     500,   2000, "10011",	0,0,
"面具怪人",	700,0, 500,0,  800,    850,	300,   2000, "11100",	0,0,
"U2外星人",	800,0, 600,0,  800,    800,	600,   2000, "11010",	0,0,
"中國疆屍",	800,0, 600,0, 1000,   1000,     500,   2000, "10100",	0,0,
"彩色酋長",     900,0, 800,0, 1200,   1200,     600,   3000, "11100",   0,0,
"魔音吉他手",  1000,0, 850,0, 1400,   1000,     650,   3000, "11001",   0,0,
"萬年老龜",    1200,0,1000,0, 1300,   1500,     500,   3000, "01011",   0,0,
"八神",	       1200,0, 900,0, 1500,   1300,     800,   3000, "10101",   0,0,
"鐵面人",      1500,0,1200,0, 1800,   1800,    1200,   4000, "00011",   0,0,
"大嘴",        1600,0,1000,0, 1700,   1800,    1100,   4000, "00110",   0,0,
"骷髏\兵",      1700,0,1500,0, 1800,   1800,    1250,  4000, "10110",   0,0,
"熔化妖",      1750,0,1300,0, 1800,   2000,    1000,   4000, "01011",	0,0,
"使徒",	       2500,0,2500,0, 2500,   2500,    2500,   5000, "10001",   0,0,
"埃及木乃伊",  3500,0,3000,0, 3500,   3500,    2000,   5000, "10110",	0,0,
"古小兔",      5000,0,4500,0, 5000,   6000,    4000,   5000, "11100",   0,0,
"十字機器人",  6000,0,5000,0, 5500,   6500,    5000,   5000, "01001",	0,0,
"飛天小巫師",  6500,0,5500,0, 6500,   6000,    6000,   6000, "01101",	0,0,
"炸蛋超人",    7000,0,6000,0, 8000,   7000,    6000,   6500, "00001",	0,0,
"Akira",      13500,0,6000,0,15000,  12000,   13000,   8000, "00100",	0,0, 
"Sarah",      15000,0,6000,0,14000,  13000,   12000,   8000, "10000",	0,0, 
"Jacky",      13000,0,6000,0,15000,  12000,   13000,   8000, "01000",	0,0, 
"Wolf",       16000,0,6000,0,13000,  15000,   10000,   8000, "00010",	0,0, 
"Jeffery",    15000,0,6000,0,12000,  14000,   13000,   8000, "00001",	0,0, 
"Kage",       12000,0,6000,0,11000,  11000,   15000,   8000, "10000",	0,0, 
"Lau",        14000,0,6000,0,14000,  13000,   14000,   8000, "01000",	0,0, 
"Lion",       16000,0,6000,0,12000,  15000,   11000,   8000, "00100",	0,0, 
"Shun",       11000,0,6000,0,13000,  12000,   11000,   8000, "00010",	0,0, 
"Aoi",        14000,0,6000,0,13000,  14000,   13000,   8000, "00001",	0,0, 
"Taka",       15000,0,6000,0,11000,  15000,   11000,   8000, "10000",	0,0, 
"Pai",        11000,0,6000,0,11000,  11000,   16000,   8000, "01000",	0,0, 
NULL,		  0,0,	 0,0,	 0,	 0,	  0,	  0,	NULL,	0,0
};


struct magicset treatmagiclist[] = {
"治療法術",	0,	0,	0,	0,	0,	0,
"氣療術",	5,	1,	20,	0,	10,	0,
"凝神歸元",     20,     1,	100,    0,	40,	0,
"元靈歸心",     50,     1,	400,    0,	60,	0,
"五氣朝元",     100,    2,	0,	2,	0,	0,
NULL,		NULL,	NULL,	NULL,	NULL,	NULL,	NULL
};                            
     
struct magicset thundermagiclist[] = {
"雷系法術",	0,	0,	0,	0,	0,	0,
"雷咒",		10,	0,	20,	1,	5,	0,
"五雷咒",	20,	0,	50,	1,	5,	0,
"天雷咒",	40,	0,	100,	1,	5,	0,
"雷之網",	100,	0,	250,	1,	10,	0,
"瘋狂之雷",	200,	0,	500,	1,	10,	0,
"雷神之舞",	600,	0,	1500,	1,	10,	0,
NULL,		NULL,	NULL,	NULL,	NULL,	NULL,	NULL
};

struct magicset icemagiclist[] = {
"冰系法術",	0,	0,	0,	0,	0,	0,
"冰咒",		10,	0,	20,	1,	5,	0,
"寒冰咒",	20,	0,	50,	1,	5,	0,
"玄冰咒",	40,	0,	100,	1,	5,	0,
"風雷冰天",	100,	0,	250,	1,	10,	0,
"絕對零度之冰",	200,	0,	500,	1,	10,	0,
"冰神之舞",	500,	0,	1400,	1,	10,	0,
NULL,		NULL,	NULL,	NULL,	NULL,	NULL,	NULL
};

struct magicset firemagiclist[] = {
"炎系法術",	0,	0,	0,	0,	0,	0,
"火術",		5,	0,	10,	1,	5,	0,
"炎咒",		10,	0,	20,	1,	5,	0,
"炎殺咒",	20,	0,	50,	1,	5,	0,
"煉獄真火",	40,	0,	100,	1,	5,	0,
"火龍掌",	100,	0,	250,	1,	10,	0,
"火龍招喚",	200,	0,	500,	1,	10,	0,
"火神之舞",	600,	0,	1600,	1,	10,	0,
NULL,		NULL,	NULL,	NULL,	NULL,	NULL,	NULL
};

struct magicset earthmagiclist[] = {
"土系法術",	0,	0,	0,	0,	0,	0,
"土咒",		10,	0,	20,	1,	5,	0,
"飛岩術",	20,	0,	50,	1,	5,	0,
"地裂天崩",	40,	0,	100,	1,	5,	0,
"泰山壓頂",	100,	0,	250,	1,	10,	0,
"土龍召喚",	200,	0,	500,	1,	10,	0,
"山神之舞",	450,	0,	1300,	1,	10,	0,
NULL,		NULL,	NULL,	NULL,	NULL,	NULL,	NULL
};

struct magicset windmagiclist[] = {
"風系法術",	0,	0,	0,	0,	0,	0,
"風咒",		10,	0,	20,	1,	5,	0,
"旋風咒",	20,	0,	50,	1,	5,	0,
"狂風術",	40,	0,	100,	1,	5,	0,
"龍捲風",	100,	0,	250,	1,	10,	0,
"風捲殘雲",	200,	0,	500,	1,	10,	0,
"風神之舞",	400,	0,	1200,	1,	10,	0,
NULL,		NULL,	NULL,	NULL,	NULL,	NULL,	NULL
};

/*名字        體力MAX法力MAX  攻擊   防護     速度    財寶   特別   圖檔*/
struct playrule resultmanlist[] = {
"茱麗葉塔",	 60,0,	20,0,	20,	20,	 20,	150, "11101",	0,0,
"菲歐利娜",	 60,0,	20,0,	30,	30,	 30,	200, "01111",	0,0,
"阿妮斯",	 80,0,	40,0,	50,	35,	 60,	250, "11110",	0,0,
"帕多雷西亞",	 85,0,	30,0,	80,	90,	 80,	500, "10111",	0,0,
"卡美拉美", 	 90,0,  50,0,   75,	70,	 60,	550, "11010",	0,0,
"姍娜麗娃",	 90,0,  40,0,   10,     30,      50,    880, "10100",   0,0,
NULL,		  0,0,	 0,0,	 0,	 0,	  0,	  0,	NULL,	0,0
};

struct newendingset
{
  char *girl;		/*女生結局的職業*/ 
  char *boy;		/*男生結局的職業*/
  int grade;		/*評分*/
};
typedef struct newendingset newendingset;

/*萬能*/
struct newendingset endmodeall_purpose[] = {
"女性職業",		"男生職業",		0,
"成為這個國家新女王",	"成為這個國家新國王",	500,
"成為國家的宰相",	"成為國家的宰相",	400,
"成為教會中的大主教",	"成為教會中的大主教",	350,
"成為國家的大臣",	"成為國家的大臣",	320,
"成為一位博士",		"成為一位博士",		300,
"成為教會中的修女",	"成為教會中的神父",	150,
"成為法庭上的大法官",   "成為法庭上的法官",	200,
"成為知名的學者",	"成為知名的學者",	120,
"成為一名女官",		"成為一名男官",		100,
"在育幼院工作",		"在育幼院工作",		100,
"在旅館工作",		"在旅館工作",		100,
"在農場工作",		"在農場工作",		100,
"在餐\廳工作",		"在餐\廳工作",		100,
"在教堂工作",		"在教堂工作",		100,
"在地攤工作",		"在地攤工作",		100,
"在伐木場工作",		"在伐木場工作",		100,
"在美容院工作",		"在美容院工作",		100,
"在狩獵區工作",		"在狩獵區工作",		100,
"在工地工作",		"在工地工作",		100,
"在墓園工作",		"在墓園工作",		100,
"擔任家庭教師工作",	"擔任家庭教師工作",	100,
"在酒家工作",		"在酒家工作",		100,
"在酒店工作",		"在酒店工作",		100,
"在大夜總會工作",	"在大夜總會工作",	100,
"在家中幫忙",		"在家中幫忙",		50,
"在育幼院兼差",		"在育幼院兼差",		50,
"在旅館兼差",		"在旅館兼差",		50,
"在農場兼差",		"在農場兼差",		50,
"在餐\廳兼差",		"在餐\廳兼差",		50,
"在教堂兼差",		"在教堂兼差",		50,
"在地攤兼差",		"在地攤兼差",		50,
"在伐木場兼差",		"在伐木場兼差",		50,
"在美容院兼差",		"在美容院兼差",		50,
"在狩獵區兼差",		"在狩獵區兼差",		50,
"在工地兼差",		"在工地兼差",		50,
"在墓園兼差",		"在墓園兼差",		50,
"擔任家庭教師兼差",	"擔任家庭教師兼差",	50,
"在酒家兼差",		"在酒家兼差",		50,
"在酒店兼差",		"在酒店兼差",		50,
"在大夜總會兼差",	"在大夜總會兼差",	50,
NULL,		NULL,	0
};

/*戰鬥*/
struct newendingset endmodecombat[] = {
"女性職業",		"男生職業",			0,
"被封為勇者 戰士型",  	"被封為勇者 戰士型",		420,
"被拔擢成為一國的將軍",	"被拔擢成為一國的將軍",		300,
"當上了國家近衛隊隊長",	"當上了國家近衛隊隊長",		200,
"當了武術老師",		"當了武術老師",			150,
"變成騎士報效國家",	"變成騎士報效國家",		160,
"投身軍旅生活，成為士兵","投身軍旅生活，成為士兵",	80,
"變成獎金獵人",		"變成獎金獵人",			0,
"以傭兵工作維生",	"以傭兵工作維生",		0,
NULL,           NULL,   0
};

/*魔法*/
struct newendingset endmodemagic[] = {
"女性職業",	     	"男生職業",		0,
"被封為勇者 魔法型",	"被封為勇者 魔法型",	420,
"被聘為王宮魔法師",	"被聘為王官魔法師",	280,
"當了魔法老師",		"當了魔法老師",		160,
"變成一位魔導士",	"變成一位魔導士",	180,
"當了魔法師",		"當了魔法師",		120,
"以占卜師幫人算命為生",	"以占卜師幫人算命為生",	40,
"成為一個魔術師",	"成為一個魔術師",	20,
"成為街頭藝人",		"成為街頭藝人",		10,
NULL,           NULL	,0
};

/*社交*/
struct newendingset endmodesocial[] = {
"女性職業",     	"男生職業",		0,
"成為國王的寵妃",	"成為女王的駙馬爺",	170,
"被挑選成為王妃",	"被選中當女王的夫婿",	260,
"被伯爵看中，成為夫人",	"成為了女伯爵的夫婿",	130,
"成為富豪的妻子",	"成為女富豪的夫婿",	100,
"成為商人的妻子",	"成為女商人的夫婿",	80,
"成為農人的妻子",	"成為女農人的夫婿",	80,
"成為地主的情婦",	"成為女地主的情夫",	-40,
NULL,           NULL,	0
};
/*藝術*/
struct newendingset endmodeart[] = {
"女性職業",		"男生職業",	0,
"成為了小丑",		"成為了小丑",	100,
"成為了作家",		"成為了作家",	100,
"成為了畫家",		"成為了畫家",	100,
"成為了舞蹈家",		"成為了舞蹈家",	100,
NULL,           NULL,	0
};

/*暗黑*/
struct newendingset endmodeblack[] = {
"女性職業",     	"男生職業",		0,
"變成了魔王",		"變成了魔王",		-1000,
"混成了太妹",		"混成了流氓",		-350,
"做了[ＳＭ女王]的工作",	"做了[ＳＭ國王]的工作",	-150,
"當了黑街的大姐",	"當了黑街的老大",	-500,
"變成高級娼婦",		"變成高級情夫",		-350,
"變成詐欺師詐欺別人",	"變成金光黨騙別人錢",	-350,
"以流鶯的工作生活",	"以牛郎的工作生活",	-350,
NULL,		NULL,	0
};

/*家事*/
struct newendingset endmodefamily[] = {
"女性職業",     	"男生職業",		0,
"正在新娘修行",		"正在新郎修行",		50,
NULL,		NULL,	0
};
