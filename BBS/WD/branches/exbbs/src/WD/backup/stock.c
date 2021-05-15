// ªÑ²¼Åo by dsyan 87.6.21

#include "bbs.h"

#define TOTAL_STOCK (1000)
#define RED (31)
#define GREEN (32)
#define WHITE (37)

struct stock {
	char name[TOTAL_STOCK][7];
	char ud[21];
	unsigned long price[TOTAL_STOCK];
	unsigned int max_page;
	unsigned int max;
	int bgcolor[TOTAL_STOCK];
	int fgcolor[TOTAL_STOCK];
};

unsigned int
p_stock ()
{
	char result[15] = "¥[Åv«ü¼Æº¦¶^x";
	int color;
	char tmp[200], f_name[200], f_in[200], f_out[200];
	char user_stock_name[TOTAL_STOCK][7],buf[256];
	unsigned long int money;
	usint own[TOTAL_STOCK];
	usint page = 0, total = 0, total_global, total_user, i;
	unsigned long st_total, st_price;
	struct stock *stock;
	FILE *stock_global_file, *stock_global_name;
	FILE *stock_user_own,  *stock_user_name, *stock_user_log;
		
	more ("/home/bbs/etc/game/stock_welcome", YEA);
	showtitle ("­·¹ÐªÑ¥«", BoardName);
	
	setutmpmode (STOCK);
	
	stock = (struct stock *) malloc(sizeof(struct stock));
	if(stock == NULL) {
		pressanykey("°O¾ÐÅé°t¸m¿ù»~¡I¦A¸Õ¤@¦¸¬Ý¬Ý¡I");
		return 1;
	}

	for (i = 0; i < TOTAL_STOCK; i++) { // ²MªÅ°O¾ÐÅé¤º®e¡C
		strcpy ((char *) stock->name[i], "");
		stock->price[i] = 0;
	}
	
	// Åª¨ú¾ãÅé¸ê®ÆÀÉ...
	
	if (stock_global_file = fopen ("/home/bbs/game/stock_now", "r")) {
		long tmpp, i;
		char tmpn[7];
		
		total_global = 0;
		
		for (i = 0; i < TOTAL_STOCK ; i++)
			stock -> price[i] = 0;
			
		fgets (tmp, 200, stock_global_file); // §â²Ä¤@¦æ¥h±¼¡C
		
		tmp[20] = 0;
		
		if (strcmp (tmp, stock -> ud)) {
			strcpy (stock -> ud, tmp);
			
			// Åª¨úÂÂ¦³ªÑ²¼¦WºÙ...
			
			if (stock_global_name = fopen ("/home/bbs/game/stock_name", "r")) {
				while (fgets (stock -> name[total_global], 7, stock_global_name)) {
					fgets (tmp, 7, stock_global_name);
					total_global++;
				}
				fclose (stock_global_name);
			}
			
			// Åª¨ú§¹²¦¡C
			
//			game_log (STOCK, "§ó·s¸ê®Æ %s", stock -> ud);
			
			// ·s¼WªÑ²¼¦WºÙ...
			
			i = 0;
			
			stock_global_name = fopen ("/home/bbs/game/stock_name", "a+");
			while (fgets (tmp, 200, stock_global_file)) {
				if (tmp[0] == '|' && !(strncmp(result, tmp + 3, 6))) {
					if (!strncmp(tmp + 54, "x", 1)) {
						strncpy (tmp + 54, result + 10, 2);
						color = 32;
					}
					else {
						strncpy (tmp + 54, result + 8, 2);
						color = 31;
					}
					strncpy (result, tmp + 47, 14);
					result[14] = '\0';
					continue;					
				}
				if (tmp[0] != '|' || tmp[1] < '0' || tmp[1] > '9')
					continue;
				tmp[12] = 0;
				tmp[53] = 0;
				tmpp = atof (tmp + 47) * 100;
				strcpy (tmpn, tmp + 6);
				
				i++;
				
				// ·s¼W¸ê®Æ¨ìÀÉ®×§ÀºÝ...
				
				if (i > total_global) {
					strcpy (stock -> name[i], tmpn);
					fprintf (stock_global_name, "%s\n", tmpn);
					total_global++;
                }
                
                stock -> price[i] = tmpp;
                
                // ¨M©w¿Ã¹õ¤Wªº¿é¥X...
                
				for (total_user = 0; total_user < total_global; total_user++) {
					if (!strcmp (tmpn, stock -> name[total_user])) {
						stock -> price[total_user] = tmpp;
						if (!strncmp (tmp + 20, "-", 1))
							stock -> bgcolor [total_user] = GREEN + 10;
						else if (!strncmp (tmp + 20, "+", 1))
							stock -> bgcolor [total_user] = RED + 10;
						else 
							stock -> bgcolor [total_user] = 40;
						if (!strncmp (tmp + 54, "x", 1))
							if (stock -> bgcolor [total_user] != 40)
								stock -> fgcolor [total_user] = WHITE;
							else
								stock -> fgcolor [total_user] = GREEN;
						else 
							if (stock -> bgcolor [total_user] != 40)
								stock -> fgcolor [total_user] = WHITE;
							else
								stock -> fgcolor [total_user] = RED;
						break;
					}
				}
				
				// ³B²z§¹²¦¡C
			}
			fclose (stock_global_name);
			fclose (stock_global_file);
			
			// ¨M©wÁ`­¶¼Æ...
			
			stock -> max = total_global;
			stock -> max_page = total_global / 40;
			
			if (stock -> max_page % 40)
				stock -> max_page++;
		}
	}

	// Åª¨ú¨Ï¥ÎªÌªÑ²¼¦WºÙ...

	i = 0;
	
	sprintf (f_name, "/home/bbs/home/%s/stock_name", cuser.userid);
	
	if (stock_user_name = fopen (f_name, "r")) { 
		while (fscanf(stock_user_name, "%s", &user_stock_name[i]) != EOF) {
			i++;
		}
		fclose (stock_user_name);
		
		// ·s¼W¸ê®Æ¨ì¨Ï¥ÎªÌ¸ê®Æ®w¤¤¡A¥H¨Ï¨Ï¥ÎªÌ¸ê®Æ¯à»P¾ãÅé¸ê®Æ¤@­P...
		
		if (i < total_global) {
			// ¾ãÅé¸ê®Æ®w¡C
			sprintf (f_in, "/home/bbs/etc/game/stock_name");
			
			// ¨Ï¥ÎªÌ¸ê®Æ®w¡C
			sprintf (f_out, "/home/bbs/home/%s/stock_name", cuser.userid);
			
			sprintf (tmp, "/bin/cp -f %s %s", f_in, f_out);
			
			// §â¾ãÅé¸ê®Æ®w»\¹L¥h¡C
			
			system (tmp);
			
			// ¦A­«·s¸ü¤JªÑ¥«¦WºÙ¡C
			sprintf (f_name, "/home/bbs/home/%s/stock_name", cuser.userid);
			stock_user_name = fopen (f_name, "r");
			i = 0;
			while (fscanf(stock_user_name, "%s", &user_stock_name[i]) != EOF) {
				i++;
			}
			fclose(stock_user_name);
		}
	}
	else {
		sprintf (f_name, "/home/bbs/etc/game/stock_name");
		
		i = 0;
				
		if(stock_global_name = fopen(f_name, "r")) {
			sprintf (f_name, "/home/bbs/home/%s/stock_name", cuser.userid);
			stock_user_name = fopen(f_name, "w");
			while(fscanf(stock_global_name, "%s", &user_stock_name[i]) != EOF) {
				fprintf(stock_user_name, "%s\n", user_stock_name[i]);
				i++;
			}
			fclose(stock_global_name);
			fclose(stock_user_name);
		}
	}
	
	// Åª¨ú§¹²¦¡C

	// Åª¨ú¨Ï¥ÎªÌªÑ²¼¾Ö¦³¸ê®Æ...
	
	sprintf (f_name, "/home/bbs/home/%s/stock", cuser.userid);
	
	if (stock_user_own = fopen (f_name, "r"))
		for (i = 0; i < TOTAL_STOCK; i++)
			fscanf (stock_user_own, "%d\n", &own[i]);
	else {
		stock_user_own = fopen (f_name, "w");
		for (i = 0; i < TOTAL_STOCK; i++) {
			fprintf (stock_user_own, "%d\n", 0);
			own[i] = 0;
		}
	}

	fclose (stock_user_own);
	
	// Åª¨ú§¹²¦¡C

	while (-1) {
		unsigned int i, r;
		time_t now = time(NULL);
		struct tm *ptime = localtime(&now);

		// µe­±¥Dµ{¦¡

		move (1, 0);
		clrtobot ();
		prints ("[37;1;46m ½s¸¹ [42m ªÑ ²¼ ¦W ºÙ [43m »ù ®æ"
		" [45m «ù¦³±i¼Æ [m    [37;1;46m ½s¸¹ [42m ªÑ ²¼ ¦W ºÙ"
		" [43m »ù ®æ [45m «ù¦³±i¼Æ [m");
		for (i = 0; i < 20; i++) {
			move (i + 2, 0);
			r = page * 40 + i;
			if (r + 1 <= stock -> max)
				prints (" %3d)    [%d;1;%dm%8s[m%6d.%02d %5d         ", r + 1, stock -> bgcolor[r], stock -> fgcolor[r], stock -> name[r], stock -> price[r] / 100, stock -> price[r] % 100, own[r]);
			r = page * 40 + i + 20;
			if (r + 1 <= stock -> max)
				prints ("%3d)    [%d;1;%dm%8s[m%6d.%02d %5d\n", r + 1, stock -> bgcolor[r], stock -> fgcolor[r], stock -> name[r], stock -> price[r] / 100, stock -> price[r] % 100, own[r]);
		}
		move (22, 0);
		prints ("[44mN:¤U­¶ P:¤W­¶ B:¶R S:½æ V:¬Ý E:­× Q:¸õ¥X $:%-8d  %2d/%2d [m[47;%dm ¦¬½L%10s[m", 
		cuser.money, page + 1, stock -> max_page, color, result);
		total = igetkey ();
		switch (total) {
			case '\r':
				if ((total = search_num( total, stock -> max_page-1)) >= 0 && total != -1)
					page = total ;
				break;
			case 'b':
			case 'B':
				now = time (0);
				ptime = localtime (&now);
				if (ptime -> tm_hour > 9 && ptime -> tm_hour < 13) {
					move (23, 0);
					pressanykey("¥æ©ö®É¶¡¬°¨C¤Ñ¤U¤È¤@ÂI¨ì¹j¤Ñ¦­¤W¤EÂI..");
					break;
				}
				move (23, 0);
				prints ("­n¶R¨º®a¡H(1-%d) ", stock -> max);
				getdata (23, 20, " ", tmp, 4, 1, 0);
				total = atoi (tmp);
				if (total < 1 || total > stock -> max) {
					pressanykey("¿é¤J¿ù»~");
					break;
				}
				st_price = stock -> price[total - 1] / 100;
				if (st_price == 0.0) {
					pressanykey("¦ÑÁó¤£¦b®a¡A¤µ¤Ñ¤£½æ°Õ");
					break;
				}
				if (st_price >= 1000) {
					pressanykey("ºî¦X«ü¼ÆµLªk¶R½æ¡I");
					break;
				}
				st_price = stock -> price[total - 1];
				st_total = cuser.money / ((st_price)*1.02);
				total_user = (unsigned int) st_total;
				move (23, 0);
				prints ("­n¶R´X±i¡H(0 -%2d)   ", total_user);
				getdata (23, 20, " ", tmp, 5, 1, 0);
				total_global = atoi (tmp);
				if (total_global > total_user || total_global < 1) {
					pressanykey("¿é¤J¿ù»~");
					break;
				}
				now = time (0);
				st_total = total_global * stock -> price[total - 1] * 1.02;
				sprintf (tmp, "%s %s ", Cdate (&now), cuser.userid);
				sprintf(buf, "%s ¶R¤F½s¸¹ %d %s %d ±i.. ªá %d", tmp, 
				total, stock -> name[total - 1], total_global, 
				(unsigned int) st_total);
				f_cat("log/stock.log",buf);
				sprintf (f_name, "home/%s/stock_log", cuser.userid);
				stock_user_log = fopen (f_name, "a+");
				fprintf(stock_user_log, "%s ¥H %f ªº»ù®æ¶R¤F½s¸¹ %d %s %d ±i.. ªá %d\n", 
				Cdate (&now), (float) stock -> price[total - 1] / 100, 
				total, stock -> name[total - 1], total_global, 
				(unsigned int) st_total);
				fclose(stock_user_log);
				money = (unsigned long int) st_total;
				demoney(money);
				own[total - 1] += total_global;
				break;
			
			case 's':
			case 'S':
				now = time (0);
				ptime = localtime (&now);
				if (ptime -> tm_hour > 9 && ptime -> tm_hour < 14) {
					move (23, 0);
					pressanykey("¥æ©ö®É¶¡¬°¨C¤Ñ¤U¤È¨âÂI¨ì¹j¤Ñ¦­¤W¤EÂI..");
					break;
				}
				move (23, 0);
				prints ("­n½æ¨º®a¡H(1-%d) ", stock -> max);
				getdata (23, 20, " ", tmp, 4, 1, 0);
				total = atoi (tmp);
				if (total < 1 || total > stock -> max || !own[total - 1]) {
					pressanykey("¿é¤J¿ù»~");
					break;
				}
				st_price = stock -> price[total - 1];
				move (23, 0);
				prints ("­n½æ´X±i¡H(0 -%2d)   ", own[total - 1]);
				getdata (23, 20, " ", tmp, 5, 1, 0);
				total_global = atoi (tmp);
				if (total_global > own[total - 1] || total_global <= 0) {
					pressanykey("¿é¤J¿ù»~");
					break;
				}
				now = time (0);
				st_total = total_global * st_price * 0.95;
				sprintf (tmp, "%s %s ", Cdate (&now), cuser.userid);
				sprintf(buf, "%s ½æ¤F½s¸¹ %d %s %d ±i.. ±o %d", tmp, 
				total, stock -> name[total - 1], total_global, 
				(unsigned int) st_total);
				f_cat("log/stock.log",buf);
				sprintf (f_name, "/home/bbs/home/%s/stock_log", cuser.userid);
				stock_user_log = fopen (f_name, "a+");
				fprintf (stock_user_log, "%s ¥H %f ªº»ù®æ½æ¤F½s¸¹ %d %s %d ±i.. ±o %d\n", 
				Cdate (&now), (float) stock -> price[total - 1] / 100, total, 
				stock -> name[total - 1], total_global, (unsigned int) st_total);
				fclose (stock_user_log);
				money = (unsigned long int) st_total;
				inmoney(money);
				own[total - 1] -= total_global;
				sprintf (f_name, "/home/bbs/home/%s/stock", cuser.userid);
				stock_user_own = fopen (f_name, "w");
				for (total = 0; total < TOTAL_STOCK; total++)
					fprintf (stock_user_own, "%d\n", own[total]);
				fclose (stock_user_own);
				break;
			
			case 'v':
			case 'V':
				sprintf (f_name, "home/%s/stock_log", cuser.userid);
				more (f_name, YEA);
				showtitle ("§Ñ´¸ªÑ¥«¤j¦ë", BoardName);
				break;
			
			case 'e':
			case 'E':
				sprintf (f_name, "home/%s/stock_log", cuser.userid);
				total_user = vedit (f_name, NA);
				showtitle ("§Ñ´¸ªÑ¥«¤j¦ë", BoardName);
				break;
			
			case KEY_UP:
			case KEY_PGUP:
			case 'p':
			case 'P':
				if (page)
					page--;
				break;

			case ' ':
			case KEY_DOWN:
			case KEY_PGDN:
			case 'n':
			case 'N':
				if (page + 1 < stock -> max_page)
					page++;
				break;
		
			case KEY_LEFT:
			case 'q':
			case 'Q':
				sprintf (f_name, "/home/bbs/home/%s/stock", cuser.userid);
				stock_user_own = fopen (f_name, "w");
				for (total = 0; total < TOTAL_STOCK; total++)
					fprintf (stock_user_own, "%d\n", own[total]);
				fclose (stock_user_own);
				return 1;
		
			default:
				break;
			
		}
	}
}
