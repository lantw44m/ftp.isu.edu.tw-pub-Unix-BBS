// 股票囉 by dsyan 87.6.21

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
	char result[15] = "加權指數漲跌x";
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
	showtitle ("風塵股市", BoardName);
	
	setutmpmode (STOCK);
	
	stock = (struct stock *) malloc(sizeof(struct stock));
	if(stock == NULL) {
		pressanykey("記憶體配置錯誤！再試一次看看！");
		return 1;
	}

	for (i = 0; i < TOTAL_STOCK; i++) { // 清空記憶體內容。
		strcpy ((char *) stock->name[i], "");
		stock->price[i] = 0;
	}
	
	// 讀取整體資料檔...
	
	if (stock_global_file = fopen ("/home/bbs/game/stock_now", "r")) {
		long tmpp, i;
		char tmpn[7];
		
		total_global = 0;
		
		for (i = 0; i < TOTAL_STOCK ; i++)
			stock -> price[i] = 0;
			
		fgets (tmp, 200, stock_global_file); // 把第一行去掉。
		
		tmp[20] = 0;
		
		if (strcmp (tmp, stock -> ud)) {
			strcpy (stock -> ud, tmp);
			
			// 讀取舊有股票名稱...
			
			if (stock_global_name = fopen ("/home/bbs/game/stock_name", "r")) {
				while (fgets (stock -> name[total_global], 7, stock_global_name)) {
					fgets (tmp, 7, stock_global_name);
					total_global++;
				}
				fclose (stock_global_name);
			}
			
			// 讀取完畢。
			
//			game_log (STOCK, "更新資料 %s", stock -> ud);
			
			// 新增股票名稱...
			
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
				
				// 新增資料到檔案尾端...
				
				if (i > total_global) {
					strcpy (stock -> name[i], tmpn);
					fprintf (stock_global_name, "%s\n", tmpn);
					total_global++;
                }
                
                stock -> price[i] = tmpp;
                
                // 決定螢幕上的輸出...
                
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
				
				// 處理完畢。
			}
			fclose (stock_global_name);
			fclose (stock_global_file);
			
			// 決定總頁數...
			
			stock -> max = total_global;
			stock -> max_page = total_global / 40;
			
			if (stock -> max_page % 40)
				stock -> max_page++;
		}
	}

	// 讀取使用者股票名稱...

	i = 0;
	
	sprintf (f_name, "/home/bbs/home/%s/stock_name", cuser.userid);
	
	if (stock_user_name = fopen (f_name, "r")) { 
		while (fscanf(stock_user_name, "%s", &user_stock_name[i]) != EOF) {
			i++;
		}
		fclose (stock_user_name);
		
		// 新增資料到使用者資料庫中，以使使用者資料能與整體資料一致...
		
		if (i < total_global) {
			// 整體資料庫。
			sprintf (f_in, "/home/bbs/etc/game/stock_name");
			
			// 使用者資料庫。
			sprintf (f_out, "/home/bbs/home/%s/stock_name", cuser.userid);
			
			sprintf (tmp, "/bin/cp -f %s %s", f_in, f_out);
			
			// 把整體資料庫蓋過去。
			
			system (tmp);
			
			// 再重新載入股市名稱。
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
	
	// 讀取完畢。

	// 讀取使用者股票擁有資料...
	
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
	
	// 讀取完畢。

	while (-1) {
		unsigned int i, r;
		time_t now = time(NULL);
		struct tm *ptime = localtime(&now);

		// 畫面主程式

		move (1, 0);
		clrtobot ();
		prints ("[37;1;46m 編號 [42m 股 票 名 稱 [43m 價 格"
		" [45m 持有張數 [m    [37;1;46m 編號 [42m 股 票 名 稱"
		" [43m 價 格 [45m 持有張數 [m");
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
		prints ("[44mN:下頁 P:上頁 B:買 S:賣 V:看 E:修 Q:跳出 $:%-8d  %2d/%2d [m[47;%dm 收盤%10s[m", 
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
					pressanykey("交易時間為每天下午一點到隔天早上九點..");
					break;
				}
				move (23, 0);
				prints ("要買那家？(1-%d) ", stock -> max);
				getdata (23, 20, " ", tmp, 4, 1, 0);
				total = atoi (tmp);
				if (total < 1 || total > stock -> max) {
					pressanykey("輸入錯誤");
					break;
				}
				st_price = stock -> price[total - 1] / 100;
				if (st_price == 0.0) {
					pressanykey("老闆不在家，今天不賣啦");
					break;
				}
				if (st_price >= 1000) {
					pressanykey("綜合指數無法買賣！");
					break;
				}
				st_price = stock -> price[total - 1];
				st_total = cuser.money / ((st_price)*1.02);
				total_user = (unsigned int) st_total;
				move (23, 0);
				prints ("要買幾張？(0 -%2d)   ", total_user);
				getdata (23, 20, " ", tmp, 5, 1, 0);
				total_global = atoi (tmp);
				if (total_global > total_user || total_global < 1) {
					pressanykey("輸入錯誤");
					break;
				}
				now = time (0);
				st_total = total_global * stock -> price[total - 1] * 1.02;
				sprintf (tmp, "%s %s ", Cdate (&now), cuser.userid);
				sprintf(buf, "%s 買了編號 %d %s %d 張.. 花 %d", tmp, 
				total, stock -> name[total - 1], total_global, 
				(unsigned int) st_total);
				f_cat("log/stock.log",buf);
				sprintf (f_name, "home/%s/stock_log", cuser.userid);
				stock_user_log = fopen (f_name, "a+");
				fprintf(stock_user_log, "%s 以 %f 的價格買了編號 %d %s %d 張.. 花 %d\n", 
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
					pressanykey("交易時間為每天下午兩點到隔天早上九點..");
					break;
				}
				move (23, 0);
				prints ("要賣那家？(1-%d) ", stock -> max);
				getdata (23, 20, " ", tmp, 4, 1, 0);
				total = atoi (tmp);
				if (total < 1 || total > stock -> max || !own[total - 1]) {
					pressanykey("輸入錯誤");
					break;
				}
				st_price = stock -> price[total - 1];
				move (23, 0);
				prints ("要賣幾張？(0 -%2d)   ", own[total - 1]);
				getdata (23, 20, " ", tmp, 5, 1, 0);
				total_global = atoi (tmp);
				if (total_global > own[total - 1] || total_global <= 0) {
					pressanykey("輸入錯誤");
					break;
				}
				now = time (0);
				st_total = total_global * st_price * 0.95;
				sprintf (tmp, "%s %s ", Cdate (&now), cuser.userid);
				sprintf(buf, "%s 賣了編號 %d %s %d 張.. 得 %d", tmp, 
				total, stock -> name[total - 1], total_global, 
				(unsigned int) st_total);
				f_cat("log/stock.log",buf);
				sprintf (f_name, "/home/bbs/home/%s/stock_log", cuser.userid);
				stock_user_log = fopen (f_name, "a+");
				fprintf (stock_user_log, "%s 以 %f 的價格賣了編號 %d %s %d 張.. 得 %d\n", 
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
				showtitle ("忘晴股市大亨", BoardName);
				break;
			
			case 'e':
			case 'E':
				sprintf (f_name, "home/%s/stock_log", cuser.userid);
				total_user = vedit (f_name, NA);
				showtitle ("忘晴股市大亨", BoardName);
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
