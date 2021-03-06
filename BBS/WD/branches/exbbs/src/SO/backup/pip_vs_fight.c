/*--------------------------------------------------------------------------*/
/* pip_vs_fight.c 小雞對戰程式				                    */
/* 作者:chyiuan   感謝SiEpthero的技術指導				    */
/*--------------------------------------------------------------------------*/

int 
pip_set_currutmp()
{
	currutmp->pip.hp=d.hp;
	currutmp->pip.mp=d.mp;
	currutmp->pip.maxhp=d.maxhp;
	currutmp->pip.maxmp=d.maxmp;
	currutmp->pip.attack=d.attack;
	currutmp->pip.resist=d.resist;
	currutmp->pip.mresist=d.mresist;
	currutmp->pip.speed=d.speed;
	currutmp->pip.money=d.money;
}

int
pip_vf_fight(fd,first)
int fd;
int first;
{
	int ch,datac,dinjure,oldtired,oldhp;
	int oldhexp,oldmexp,oldmoney,oldhskill,oldmskill,oldbrave;
	int gameover=0,moneychange=0;	
	int oldpager;			/*叩機狀態*/
	int i;
	int notyou=0;			/*chyiuan:以免訊息被弄錯*/
	user_info *opponent;
	char data[200],buf1[256],buf2[256],mymsg[8][150];
	
	setutmpmode(CHICKENTALK);
	clear();
	
	opponent=currutmp->destuip;
	oldpager=currutmp->pager;	/*chyiuan:存在page狀態 並且變成防水*/
	currutmp->pager=3;
	currutmp->pip.pipmode=0;	/*1:輸了 2:贏了 3:不玩了 */
	currutmp->pip.leaving=1;
	pip_set_currutmp();		/*把小雞的data  down load for被呼叫者*/
	currutmp->pip.nodone=first;	/*決定誰先攻擊*/
	currutmp->pip.msgcount=0;	/*戰鬥訊息歸零*/
	currutmp->pip.chatcount=0;	/*聊天訊息歸零*/
	currutmp->pip.msg[0]='\0';
	strcpy(currutmp->pip.name,d.name);
	
	/*存下舊小雞data*/
	oldmexp=d.mexp;
	oldhexp=d.hexp;
	oldmoney=d.money;
	oldbrave=d.brave;
	oldhskill=d.hskill;
	oldmskill=d.mskill;
	
	add_io(fd, 0);
	/*對方未準備妥當  先等一下  為了防止當機 */
	while( gameover==0 && opponent->pip.leaving==0)
	{
		move(b_lines,0);
		prints("[1;46m 對方還在準備中                                                        [0m");
		ch=egetch();
	}
	for(i=0;i<8;i++)
			mymsg[i][0]='\0';
	for(i=0;i<10;i++)
			currutmp->pip.chat[i][0]='\0';
	/*開始的訊息*/
	sprintf(mymsg[0],"[1;37m%s 和 %s 的戰鬥開始了..[0m",
	opponent->pip.name,currutmp->pip.name);
	strcpy(currutmp->pip.msg,mymsg[0]);
	currutmp->pip.msgcount=0;
	/*msgcount和charcount的算法不同*/

	while (!(currutmp->pip.leaving==0 && opponent->pip.leaving==0))
	{
		clear();
		/*為了一些其他的原因  像餵食等是呼叫舊的  所以reload*/
		pip_set_currutmp();
		
		if(opponent->pip.nodone!=1)
			strcpy(mymsg[currutmp->pip.msgcount%8],currutmp->pip.msg);
		move(0,0);
		prints("[1;34m????[44;37m 自己資料 [0;1;34m?僓?????????????????????????????????????????????????????????????[m\n");
		prints("[1m   [33m姓  名:[37m%-20s                                              [31m  [m\n",
			d.name);
		sprintf(buf1,"%d/%d",d.hp,d.maxhp);
		sprintf(buf2,"%d/%d",d.mp,d.maxmp);
		prints("[1m   [33m體  力:[37m%-12s[33m法  力:[37m%-12s[33m疲  勞:[37m%-12d                  [m\n",
			buf1,buf2,d.tired);
		prints("[1m   [33m攻  擊:[37m%-12d[33m防  禦:[37m%-12d[33m速  度:[37m%-12d[33m抗  魔:[37m%-9d  [m\n",
			d.attack,d.resist,d.speed,d.mresist);
		prints("[1m   [33m戰鬥技:[37m%-12d[33m魔法技:[37m%-12d[33m魔評價:[37m%-12d[33m武評價:[37m%-9d  [m\n",
			d.hskill,d.mskill,d.mexp,d.hexp);
		prints("[1m   [33m食  物:[37m%-12d[33m補  丸:[37m%-12d[33m零  食:[37m%-12d[33m靈  芝:[37m%-9d  [m\n",
			d.food,d.bighp,d.cookie,d.medicine);
		prints("[1m   [33m人  蔘:[37m%-12d[33m雪  蓮:[37m%-12d[33m金  錢:[37m%-15d               [m\n",
			d.ginseng,d.snowgrass,d.money);
		move(7,0);
		prints("[1;34m????[44;37m 戰鬥訊息 [0;1;34m?僓?????????????????????????????????????????????????????????????[m\n");
		for(i=0;i<8;i++)
		{
			move(8+i,1);
			
			if(currutmp->pip.msgcount<8)
			{
				prints(mymsg[i]);
				/*適用pip.msgcount在8行內*/
			}
			else
			{
				prints(mymsg[(currutmp->pip.msgcount-8+i)%8]);
				/*pip.msgcount=8:表示已經有9個 所以從0->7*/
			}
		}
		move(16,0);
		prints("[1;34m????[44;37m 談話訊息 [0;1;34m?僓?????????????????????????????????????????????????????????????[m\n");
		for(i=0;i<2;i++)
		{
			move(17+i,0);
			if(currutmp->pip.chatcount<3)
			{
				prints(currutmp->pip.chat[i]);
				/*適用pip.chatcount在2行內*/
			}
			else
			{
				prints("%s",currutmp->pip.chat[(currutmp->pip.chatcount-2+i)%10]);
				/*pip.chatcount=3:表示已經有2個 所以從0->1*/
			}
		}
		move(19,0);
		prints("[1;34m????[1;37;44m 對手資料 [0;1;34m?僓?????????????????????????????????????????????????????????????[m\n");
		prints("[1m   [33m姓  名:[37m%-20s                                                [m\n",
			opponent->pip.name);
		sprintf(buf1,"%d/%d",opponent->pip.hp,opponent->pip.maxhp);
		sprintf(buf2,"%d/%d",opponent->pip.mp,opponent->pip.maxmp);
		prints("[1m   [33m體  力:[37m%-12s[33m法  力:[37m%-12s[33m金  錢:[37m%-15d               [m\n",
			buf1,buf2,opponent->pip.money);
		prints("[1;34m??????????????????????????????????????????????????????????????????????????????[m\n");
		if(opponent->pip.nodone==1)
		{
			notyou=1;
			prints("[1;37;44m  對方出招中，請稍待一會.....                                [T/^T]CHAT/回顧  [m");
		}
		else	
		{
			notyou=0;		
			prints("[1;44;37m  戰鬥命令  [46m [1]普通 [2]全力 [3]魔法 [4]防禦 [5]補充 [6]逃命 [T/^T]CHAT/回顧  [m");
		}
		ch = egetch();
		if (ch == I_OTHERDATA)
		{
			datac = recv(fd, data, sizeof(data), 0);
			if (datac <= 0)
				break;
		}
		else if(ch =='T' || ch =='t')
		{
			int len;
			char msg[120];
			char buf[80];
			len=getdata(b_lines,0, "想說:", buf, 60, 1, 0);
			if(len && buf[0]!=' ')
			{
				sprintf(msg,"[1;46;33m★%s[37;45m %s [0m",cuser.userid,buf);
				strcpy(opponent->pip.chat[currutmp->pip.chatcount%10],msg);				
				strcpy(currutmp->pip.chat[currutmp->pip.chatcount%10],msg);
				opponent->pip.chatcount++;
				currutmp->pip.chatcount++;
			}
			
		}
		else if(ch==Ctrl('T'))
		{
			clrchyiuan(7,19);
			move(7,0);
			prints("[1;31m????[41;37m 回顧談話 [0;1;31m?僓?????????????????????????????????????????????????????????????[m\n");
			for(i=0;i<10;i++)
			{
				move(8+i,0);
				if(currutmp->pip.chatcount<10)
				{
					prints(currutmp->pip.chat[i]);
					/*適用pip.msgcount在七行內*/
				}
				else
				{
					prints("%s",currutmp->pip.chat[(currutmp->pip.chatcount-10+i)%10]);
					/*pip.chatcount=10:表示已經有11個 所以從0->9*/
				}
			}
			move(18,0);
			prints("[1;31m????[41;37m 到此為止 [0;1;31m?僓?????????????????????????????????????????????????????????????[m");
			pressanykey("回顧之前的談話 只有10通");		
		}
		else if(currutmp->pip.nodone==1 && opponent->pip.leaving==1 && notyou==0)
		{
			d.nodone=1;
			switch(ch)
			{
				char buf[256];
				case '1': 
					if(rand()%9==0)
					{
						pressanykey("竟然沒打中..:~~~"); 
						sprintf(buf,"[1;33m%s [37m對 [33m%s[37m 施展普通攻擊，但是沒有打中...",
							d.name,opponent->pip.name);						
					}
					else
					{ 
						if(opponent->pip.resistmore==0)
							dinjure=(d.hskill/100+d.hexp/100+d.attack/9-opponent->pip.resist/12+rand()%20+2-opponent->pip.speed/30+d.speed/30);
						else
							dinjure=(d.hskill/100+d.hexp/100+d.attack/9-opponent->pip.resist/8+rand()%20+2-opponent->pip.speed/30+d.speed/30);            
						if(dinjure<=9)	dinjure=9;
						opponent->pip.hp-=dinjure;
						d.hexp+=rand()%2+2;
						d.hskill+=rand()%2+1;  
						sprintf(buf,"普通攻擊,對方體力減低%d",dinjure);
						pressanykey(buf);
						sprintf(buf,"[1;33m%s [37m施展了普通攻擊,[33m%s [37m的體力減低 [31m%d [37m點[0m"
							,d.name,opponent->pip.name,dinjure);						
					}
					opponent->pip.resistmore=0;
					opponent->pip.msgcount++;
					currutmp->pip.msgcount++;
					strcpy(opponent->pip.msg,buf);
					strcpy(mymsg[currutmp->pip.msgcount%8],buf);	
					currutmp->pip.nodone=2;	/*做完*/
					opponent->pip.nodone=1;
					break;
     
				case '2':
					show_fight_pic(2);
					if(rand()%11==0)
					{ 
						pressanykey("竟然沒打中..:~~~");
						sprintf(buf,"[1;33m%s [37m對 [33m%s[37m 施展全力攻擊，但是沒有打中...",
							d.name,opponent->pip.name);
					}     
					else 
					{ 
						if(opponent->pip.resistmore==0)      
							dinjure=(d.hskill/100+d.hexp/100+d.attack/5-opponent->pip.resist/12+rand()%30+6-opponent->pip.speed/50+d.speed/30);
						else
							dinjure=(d.hskill/100+d.hexp/100+d.attack/5-opponent->pip.resist/8+rand()%30+6-opponent->pip.speed/40+d.speed/30);                  
						if(dinjure<=20) dinjure=20;  
						if(d.hp>5)
						{ 
							opponent->pip.hp-=dinjure;
							d.hp-=5;
							d.hexp+=rand()%3+3;
							d.hskill+=rand()%2+2; 
							sprintf(buf,"全力攻擊,對方體力減低%d",dinjure);
							pressanykey(buf);
							sprintf(buf,"[1;33m%s [37m施展了全力攻擊,[33m%s [37m的體力減低 [31m%d [37m點[0m"
							,d.name,opponent->pip.name,dinjure);
						}
						else
						{ 
							d.nodone=1;
							pressanykey("你的HP小於5啦..不行啦...");
						}
					}
					opponent->pip.resistmore=0;
					opponent->pip.msgcount++;
					currutmp->pip.msgcount++;
					strcpy(opponent->pip.msg,buf);
					strcpy(mymsg[currutmp->pip.msgcount%8],buf);	
					currutmp->pip.nodone=2;	/*做完*/
					opponent->pip.nodone=1;
					break;
     
				case '3':
					clrchyiuan(8,19);
					oldtired=d.tired;
					oldhp=d.hp;     
					d.magicmode=0;
					dinjure=pip_magic_menu(8,19); 
					if(dinjure<0)	dinjure=5;
					if(d.nodone==0)
					{
						if(d.magicmode==1)
						{
							oldtired=oldtired-d.tired;
							oldhp=d.hp-oldhp;
							sprintf(buf,"治療後,體力提高%d 疲勞降低%d",oldhp,oldtired);
							pressanykey(buf);
							sprintf(buf,"[1;33m%s [37m使用魔法治療之後,體力提高 [36m%d [37m點，疲勞降低 [36m%d [37m點[0m",d.name,oldhp,oldtired);
						}
						else
						{
							if(rand()%15==0)
							{
								pressanykey("竟然沒打中..:~~~");  
								sprintf(buf,"[1;33m%s [37m對 [33m%s[37m 施展魔法攻擊，但是沒有打中...",
									d.name,opponent->pip.name);
							}
							else
							{  
								if(d.mexp<=100)
								{
									if(rand()%4>0)
										dinjure=dinjure*60/100;
									else
										dinjure=dinjure*80/100;
								}
								else if(d.mexp<=250 && d.mexp>100)
								{
									if(rand()%4>0)
										dinjure=dinjure*70/100;
									else
										dinjure=dinjure*85/100;           
								}
								else if(d.mexp<=500 && d.mexp>250)
								{
									if(rand()%4>0)
										dinjure=dinjure*85/100;
									else
										dinjure=dinjure*95/100;           
								}
								else if(d.mexp>500)
								{
									if(rand()%10>0)
										dinjure=dinjure*90/100;
									else
										dinjure=dinjure*99/100;           
								}
								dinjure=dinjure/2;
								opponent->pip.hp-=dinjure; 
								d.mskill+=rand()%2+2;  
								sprintf(buf,"魔法攻擊,對方體力減低%d(對戰打對折)",dinjure);
								pressanykey(buf);
								sprintf(buf,"[1;33m%s [37m施展了魔法攻擊,[33m%s [37m的體力減低 [31m%d [37m點[0m"
								,d.name,opponent->pip.name,dinjure);
							}
						}
						
						opponent->pip.msgcount++;
						currutmp->pip.msgcount++;
						strcpy(opponent->pip.msg,buf);
						strcpy(mymsg[currutmp->pip.msgcount%8],buf);
						/*恢復體力是用d.hp和d.maxhp去 所以得更新*/
						currutmp->pip.hp=d.hp;
						currutmp->pip.mp=d.mp;
						currutmp->pip.nodone=2;	/*做完*/
						opponent->pip.nodone=1;
					}
					break;  
				
				case '4':
					currutmp->pip.resistmore=1;
					pressanykey("小雞加強防禦啦....");
					sprintf(buf,"[1;33m%s [37m加強防禦，準備全力抵擋 [33m%s [37m的下一招[0m",
						d.name,opponent->pip.name);
					opponent->pip.msgcount++;
					currutmp->pip.msgcount++;
					strcpy(opponent->pip.msg,buf);
					strcpy(mymsg[currutmp->pip.msgcount%8],buf);
					currutmp->pip.nodone=2;	/*做完*/
					opponent->pip.nodone=1;
					break;
				case '5':
					pip_basic_feed();
					if(d.nodone!=1)
					{
						sprintf(buf,"[1;33m%s [37m補充了身上的能量，體力或法力有顯著的提升[0m",d.name);
						opponent->pip.msgcount++;
						currutmp->pip.msgcount++;
						strcpy(opponent->pip.msg,buf);
						strcpy(mymsg[currutmp->pip.msgcount%8],buf);						
						/*恢復體力是用d.hp和d.maxhp去 所以得更新*/
						currutmp->pip.hp=d.hp;
						currutmp->pip.mp=d.mp;
						currutmp->pip.nodone=2;	/*做完*/
						opponent->pip.nodone=1;
					}
					break;
				case '6':
					opponent->pip.msgcount++;
					currutmp->pip.msgcount++;
					if(rand()%20>=18 || (rand()%20>13 && d.speed <= opponent->pip.speed))
					{
						pressanykey("想逃跑，卻失敗了...");
						sprintf(buf,"[1;33m%s [37m想先逃跑再說...但卻失敗了...[0m",d.name);
						strcpy(opponent->pip.msg,buf);
						strcpy(mymsg[currutmp->pip.msgcount%8],buf);
					}
					else
					{
						sprintf(buf,"[1;33m%s [37m自覺打不過對方，所以決定先逃跑再說...[0m",d.name);
						strcpy(opponent->pip.msg,buf);
						strcpy(mymsg[currutmp->pip.msgcount%8],buf);
						currutmp->pip.pipmode=3;
						moneychange=(rand()%200+300);
						currutmp->pip.money-=moneychange;
						opponent->pip.money+=moneychange;
						d.brave-=(rand()%3+2);      
						if(d.hskill<0)
							d.hskill=0;
						if(d.brave<0)
							d.brave=0;
						clear(); 
						showtitle("電子養小雞", BoardName); 
						move(10,0);
						prints("            [1;31m┌──────────────────────┐[m");
						move(11,0);
						prints("            [1;31m│  [37m實力不強的小雞 [33m%-10s                 [31m│[m",d.name);
						move(12,0);
						prints("            [1;31m│  [37m在與對手 [32m%-10s [37m戰鬥後落跑啦          [31m│[m",opponent->pip.name);
						move(13,0);
						sprintf(buf1,"%d/%d",d.hexp-oldhexp,d.mexp-oldmexp);  
						prints("            [1;31m│  [37m評價增加了 [36m%-5s [37m點  技術增加了 [36m%-2d/%-2d [37m點  [31m│[m",buf1,d.hskill-oldhskill,d.mskill-oldmskill);
						move(14,0);
						sprintf(buf2,"%d [37m元",moneychange);
						prints("            [1;31m│  [37m勇敢降低了 [36m%-5d [37m點  金錢減少了 [36m%-13s  [31m│[m",oldbrave-d.brave,buf2);
						move(15,0);
						prints("            [1;31m└──────────────────────┘[m");             
						pressanykey("三十六計 走為上策...");
					}
					currutmp->pip.nodone=2;	/*做完*/
					opponent->pip.nodone=1;
					break;
       	
				
			}
		}
		if(currutmp->pip.hp<0)
		{
			currutmp->pip.pipmode=1;
			opponent->pip.pipmode=2;
		}
		if(currutmp->pip.pipmode==2 || opponent->pip.pipmode==1 || opponent->pip.pipmode==3)
		{
			clear();
			showtitle("電子養小雞", BoardName);
			move(10,0);
			prints("            [1;31m┌──────────────────────┐[m");
			move(11,0);
			prints("            [1;31m│  [37m英勇的小雞 [33m%-10s                     [31m│[m",d.name);
			move(12,0);
			prints("            [1;31m│  [37m打敗了對方小雞 [32m%-10s                 [31m│[m",opponent->pip.name);
			move(13,0);
			sprintf(buf1,"%d/%d",d.hexp-oldhexp,d.mexp-oldmexp);  
			prints("            [1;31m│  [37m評價提升了 %-5s 點  技術增加了 %-2d/%-2d 點  [31m│[m",buf1,d.hskill-oldhskill,d.mskill-oldmskill);
			move(14,0);
			sprintf(buf2,"%d 元",currutmp->pip.money-oldmoney);
			prints("            [1;31m│  [37m勇敢提升了 %-5d 點  金錢增加了 %-9s [31m│[m",d.brave-oldbrave,buf2);
			move(15,0);
			prints("            [1;31m└──────────────────────┘[m");     
			if(opponent->pip.hp<=0)     
				pressanykey("對方死掉囉..所以你贏囉..");   
			else if(opponent->pip.hp>0)
				pressanykey("對方落跑囉..所以算你贏囉.....");  
			d.money=currutmp->pip.money;
			pip_write_file();
		}
		if(gameover!=1 && (opponent->pip.pipmode==2 || currutmp->pip.pipmode==1))
		{
			moneychange=currutmp->pip.money*(1+rand()%2)/5;
			currutmp->pip.maxhp=currutmp->pip.maxhp*2/3;
			currutmp->pip.hp=currutmp->pip.maxhp/2;
			currutmp->pip.money-=moneychange;
			opponent->pip.money+=moneychange;
			d.mexp-=rand()%20+20;
			d.hexp-=rand()%20+20;
			d.hskill-=rand()%20+20;
			d.mskill-=rand()%20+20;
			pip_write_file();
			clear();
			showtitle("電子養小雞", BoardName);       
			move(10,0);
			prints("            [1;31m┌──────────────────────┐[m");
			move(11,0);
			prints("            [1;31m│  [37m可憐的小雞 [33m%-10s                     [31m│[m",d.name);
			move(12,0);
			prints("            [1;31m│  [37m在與 [32m%-10s [37m的戰鬥中，                [31m│[m",opponent->pip.name);
			move(13,0);
			prints("            [1;31m│  [37m不幸地打輸了，記者現場特別報導.........   [31m│[m");
			move(14,0);
			prints("            [1;31m└──────────────────────┘[m");        
			pressanykey("小雞打輸了....");
		}
		d.hp=currutmp->pip.hp;
		d.mp=currutmp->pip.mp;
		d.maxhp=currutmp->pip.maxhp;
		d.maxmp=currutmp->pip.maxmp;
		d.money=currutmp->pip.money;
		if(opponent->pip.pipmode!=0 || currutmp->pip.pipmode!=0)
		{
			currutmp->pip.leaving=0;
		}

	}
	pip_write_file();
	add_io(0, 0);
	close(fd);
	currutmp->destuip=0;
	currutmp->pager=oldpager;
	setutmpmode(CHICKEN);
}

int va_pip_vf_fight(va_list pvar)
{
  int sock, later;
  sock = va_arg(pvar, int);
  later = va_arg(pvar,int);
  return pip_vf_fight(sock, later);
}
