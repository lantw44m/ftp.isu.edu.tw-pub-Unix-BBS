/*---------------------------------------------------------------------------*/
/* �p���ϧΰ�                                                                */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include "bbs.h"
#define getdata(a, b, c , d, e, f, g) getdata(a,b,c,d,e,f,NULL,g)

enum
{STRIP_ALL,ONLY_COLOR,NO_RELOAD};

int
show_basic_pic(int i)
{
 char buf[256];
 clrchyiuan(6,17);
#ifdef MAPLE
 sprintf(buf,"src/maple/pipgame/basic/pic%d",i);
#else
 sprintf(buf,"etc/games/pip/basic/pic%d",i);
#endif  // END MAPLE
 show_file(buf,6,12,ONLY_COLOR);
}

int
show_feed_pic(int i)  /*�Y�F��*/
{
 char buf[256];
 clrchyiuan(6,18);
#ifdef MAPLE
 sprintf(buf,"src/maple/pipgame/feed/pic%d",i);
#else
 sprintf(buf,"etc/games/pip/feed/pic%d",i);
#endif  // END MAPLE
 show_file(buf,6,12,ONLY_COLOR);
}

int
show_buy_pic(int i)  /*�ʶR�F��*/
{
 char buf[256];
 clrchyiuan(6,18);
#ifdef MAPLE
 sprintf(buf,"src/maple/pipgame/buy/pic%d",i);
#else
 sprintf(buf,"etc/games/pip/buy/pic%d",i);
#endif  // END MAPLE
 show_file(buf,6,12,ONLY_COLOR);
}



int
show_usual_pic(int i)  /* ���`���A */
{
 char buf[256];
 clrchyiuan(6,18);
#ifdef MAPLE
 sprintf(buf,"src/maple/pipgame/usual/pic%d",i);
#else
 sprintf(buf,"etc/games/pip/usual/pic%d",i);
#endif  // END MAPLE
 show_file(buf,6,12,ONLY_COLOR);

}

int
show_special_pic(int i)  /* �S���� */
{
 char buf[256];
 clrchyiuan(6,18);
#ifdef MAPLE
 sprintf(buf,"src/maple/pipgame/special/pic%d",i);
#else
 sprintf(buf,"etc/games/pip/special/pic%d",i);
#endif  // END MAPLE
 show_file(buf,6,12,ONLY_COLOR);

}

int
show_practice_pic(int i)  /*�צ�Ϊ��� */
{
 char buf[256];
 clrchyiuan(6,18);
#ifdef MAPLE
 sprintf(buf,"src/maple/pipgame/practice/pic%d",i);
#else
 sprintf(buf,"etc/games/pip/practice/pic%d",i);
#endif  // END MAPLE
 show_file(buf,6,12,ONLY_COLOR);
}

int
show_job_pic(int i)    /* ���u��show�� */
{
 char buf[256];
 clrchyiuan(6,18);
#ifdef MAPLE
 sprintf(buf,"src/maple/pipgame/job/pic%d",i);
#else
 sprintf(buf,"etc/games/pip/job/pic%d",i);
#endif  // END MAPLE
 show_file(buf,6,12,ONLY_COLOR);

}


int
show_play_pic(int i)  /*�𶢪���*/
{
 char buf[256];
 clrchyiuan(6,18);
#ifdef MAPLE
 sprintf(buf,"src/maple/pipgame/play/pic%d",i);
#else
 sprintf(buf,"etc/games/pip/play/pic%d",i);
#endif  // END MAPLE
 if(i==0)
    show_file(buf,2,16,ONLY_COLOR);
 else
    show_file(buf,6,12,ONLY_COLOR);
}

int
show_guess_pic(int i)  /* �q���� */
{
 char buf[256];
 clrchyiuan(6,18);
#ifdef MAPLE
 sprintf(buf,"src/maple/pipgame/guess/pic%d",i);
#else
 sprintf(buf,"etc/games/pip/guess/pic%d",i);
#endif  // END MAPLE
 show_file(buf,6,12,ONLY_COLOR);
}

int
show_weapon_pic(int i)  /* �Z���� */
{
 char buf[256];
 clrchyiuan(1,10);
#ifdef MAPLE
 sprintf(buf,"src/maple/pipgame/weapon/pic%d",i);
#else
sprintf(buf,"etc/games/pip/weapon/pic%d",i);
#endif  // END MAPLE
 show_file(buf,1,10,ONLY_COLOR);
}

int
show_palace_pic(int i)  /* �Ѩ����ڥ� */
{
 char buf[256];
 clrchyiuan(0,13);
#ifdef MAPLE
 sprintf(buf,"src/maple/pipgame/palace/pic%d",i);
#else
sprintf(buf,"etc/games/pip/palace/pic%d",i);
#endif  // END MAPLE
 show_file(buf,0,11,ONLY_COLOR);

}

int
show_badman_pic(int i)  /* �a�H */
{
 char buf[256];
 clrchyiuan(6,18);
#ifdef MAPLE
 sprintf(buf,"src/maple/pipgame/badman/pic%d",i);
#else
 sprintf(buf,"etc/games/pip/badman/pic%d",i);
#endif  // END MAPLE
 show_file(buf,6,14,ONLY_COLOR);
}

int
show_fight_pic(int i)  /* ���[ */
{
 char buf[256];
 clrchyiuan(6,18);
#ifdef MAPLE
 sprintf(buf,"src/maple/pipgame/fight/pic%d",i);
#else
 sprintf(buf,"etc/games/pip/fight/pic%d",i);
#endif  // END MAPLE
 show_file(buf,6,14,ONLY_COLOR);
}

int
show_die_pic(int i)  /*���`*/
{
 char buf[256];
 clrchyiuan(0,23);
#ifdef MAPLE
 sprintf(buf,"src/maple/pipgame/die/pic%d",i);
#else
 sprintf(buf,"etc/games/pip/die/pic%d",i);
#endif  // END MAPLE
 show_file(buf,0,23,ONLY_COLOR);
}

int
show_system_pic(int i)  /*�t��*/
{
 char buf[256];
 clrchyiuan(1,23);
#ifdef MAPLE
 sprintf(buf,"src/maple/pipgame/system/pic%d",i);
#else
 sprintf(buf,"etc/games/pip/system/pic%d",i);
#endif  // END MAPLE
 show_file(buf,4,16,ONLY_COLOR);
}

int
show_ending_pic(int i)  /*����*/
{
 char buf[256];
 clrchyiuan(1,23);
#ifdef MAPLE
 sprintf(buf,"src/maple/pipgame/ending/pic%d",i);
#else
 sprintf(buf,"etc/games/pip/ending/pic%d",i);
#endif  // END MAPLE
 show_file(buf,4,16,ONLY_COLOR);
}

int 
show_resultshow_pic(int i)	/*��ì�u*/
{
 char buf[256];
 clrchyiuan(0,24);
#ifdef MAPLE
 sprintf(buf,"src/maple/pipgame/resultshow/pic%d",i);
#else
 sprintf(buf,"etc/games/pip/resultshow/pic%d",i);
#endif  // END MAPLE
 show_file(buf,0,24,ONLY_COLOR);
}
