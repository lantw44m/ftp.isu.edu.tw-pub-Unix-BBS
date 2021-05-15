
/* ¨Ï¥Î¿úªº¨ç¼Æ */

#include "bbs.h"

/* ªá¿ú¿ï³æ */
unsigned setuperm(pbits,nb,money)  /* ¶RÅv­­¥Î */
  unsigned pbits;
  char nb;
  int money;
{
  register int i;
    i = nb - 'a';
    if(!((pbits>>i)&1))
    {
     pbits ^= (1 << i);
     degold(money);
    }
    return(pbits);
}

/*¶R½æ°O¿ı*/
void
tradelog(userid,i)
char *userid;
int i;
{
  time_t now = time(0);
  char genbuf[200];
  char *item[5]={"«H½c¤W­­´£ª@","²×·¥Áô¨­¤jªk","¬G¶m­×§ïÄ_¨å","«H¥óµL¤W­­","³zµøÃè"};
  now = time(NULL) - 6 * 60;
  sprintf(genbuf,"¦b %s ¨Ï¥ÎªÌ [1;32m%s[m ÁÊ¶R¤F[1;36m%s[mªºÅv­­",Cdate(&now),cuser.userid,item[i]);
  f_cat("log/trade.log",genbuf);
}

void
p_cloak()
{
  char buf[4];
  move(b_lines-2,0);
  if(check_money(BUY_C,GOLD))      return;
  prints("ª÷ÃB¡G%d    ",BUY_C); // cost
  getdata(b_lines-1, 0,
    (currutmp->invisible==1) ? "½T©w­n²{¨­?[y/N]" : "½T©w­nÁô¨­?[y/N]",buf,3,LCECHO,"N");
  if(buf[0]!='y')    return;
  degold(BUY_C);

  if(currutmp->invisible & 1)
    currutmp->invisible &= ~1;
  else
    currutmp->invisible |= 1;
            
  pressanykey((currutmp->invisible==1) ? MSG_CLOAKED : MSG_UNCLOAK);
  return;
}

void
p_scloak()
{
  char buf[4];
  move(b_lines-2,0);
  if(check_money(BUY_C,GOLD))      return;
  prints("ª÷ÃB¡G%d    ",BUY_S); // cost
  getdata(b_lines-1, 0,"½T©w­nÁÊ¶R³zµøÃè? (y/N) ",buf,3,LCECHO,"N");
  if(buf[0]!='y')    return;
  degold(BUY_S);
  currutmp->invisible |= SEE;
  pressanykey(MSG_SEECLOAK);
  return;
}


void
p_fcloak()
{
  register int i;
  char ans[4];
  if(check_money(BUY_SC,GOLD) || HAS_PERM(PERM_CLOAK))
  {
    if(HAS_PERM(PERM_CLOAK))
      pressanykey("§A¤w¸g¥i¥HÁô§Î¤FÁÙ¨Ó¶R¡A¶û¿ú¤Ó¦h°Ú¡H");
    return;
  }
  move(b_lines-3,0);
  prints("ª÷ÃB¡G%d    ",BUY_SC); // cost
  getdata(b_lines-2, 0, "½T©w­nÁÊ¶R²×·¥Áô¨­¤jªk¡H[y/N]",ans,3,LCECHO,0);
  if(ans[0]!='y')
    return;
  rec_get(fn_passwd, &xuser, sizeof(xuser), usernum);
  i=setuperm(cuser.userlevel,'g',BUY_SC);
  update_data();
  cuser.userlevel=i;
  substitute_record(fn_passwd, &cuser, sizeof(userec), usernum);
  tradelog(cuser.userid,1);
  pressanykey("®¥³ß±z¤w¸g¾Ç·|¤F²×·¥Áô¨­¤jªk!!");
  return;
}

void
p_from()
{
  char ans[4], genbuf[32];
  move(b_lines-3,0);
  if(check_money(BUY_F,GOLD)) return;
  prints("ª÷ÃB¡G%d    ",BUY_F); // cost
  getdata(b_lines-2, 0, "½T©w­n§ï¬G¶m?[y/N]",ans,3,LCECHO,"N");
  if(ans[0]!='y') return;
  if (getdata(b_lines-1, 0, "½Ğ¿é¤J·s¬G¶m¡G", genbuf, 17, DOECHO,0))
        {
           degold(BUY_F);
           strip_ansi(genbuf, genbuf, 0);
           strncpy(currutmp->from, genbuf, 24);
           currutmp->from_alias=0;
        }
  return;
}

void
p_ffrom()
{
  register int i;
  char ans[4];
  if(check_money(BUY_SF,GOLD) || HAS_PERM(PERM_FROM) || HAS_PERM(PERM_SYSOP))
  {
    if(HAS_PERM(PERM_FROM) || HAS_PERM(PERM_SYSOP))
      pressanykey("§A¤w¸g¥i¥H­×§ï¬G¶m¤FÁÙ¨Ó¶R¡A¶û¿ú¤Ó¦h°Ú¡H");
    return;
  }
  move(b_lines-3,0);
  prints("ª÷ÃB¡G%d    ",BUY_SF); // cost
  getdata(b_lines-2, 0, "½T©w­nÁÊ¶R­×§ï¬G¶mÄ_¨å¡H[y/N]",ans,3,LCECHO,0);
  if(ans[0]!='y') return;
    rec_get(fn_passwd, &xuser, sizeof(xuser), usernum);
  i=setuperm(cuser.userlevel,'t',BUY_SF);
  update_data();
  cuser.userlevel=i;
  substitute_record(fn_passwd, &cuser, sizeof(userec), usernum);
  tradelog(cuser.userid,2);
  pressanykey("®¥³ß§A±o¨ì¤F¬G¶m­×§ïÄ_¨å");
  return;
}

void
p_exmail()
{
  char ans[4],buf[100];
  int  n;
  if(cuser.exmailbox >= MAXEXKEEPMAIL )
  {
    pressanykey("®e¶q³Ì¦h¼W¥[ %d «Ê¡A¤£¯à¦A¶R¤F¡C", MAXEXKEEPMAIL);
    return;
  }
  move(b_lines-3,0);
  prints("ª÷ÃB¡G%d    ",BUY_M); // cost  
  sprintf(buf,"±z´¿¼WÁÊ %d «Ê®e¶q¡AÁÙ­n¦A¶R¦h¤Ö?",cuser.exmailbox);
  getdata(b_lines-2, 0, buf,ans,3,LCECHO,"10");
  n = atoi(ans);
  if(!ans[0] || !n )
    return;
  if(n+cuser.exmailbox > MAXEXKEEPMAIL )
     n = MAXEXKEEPMAIL - cuser.exmailbox;
  if(check_money(n*BUY_M,GOLD))
    return;
  degold(BUY_M*n);
  inmailbox(n);
  return;
}

/*
void
p_ulmail()
{
  register int i;
  char ans[4];
  if(check_money(100000,GOLD) || HAS_PERM(PERM_MAILLIMIT))
  {
    if(HAS_PERM(PERM_MAILLIMIT))
      pressanykey("§Aªº«H½c¤w¸g¨S¦³­­¨î¤FÁÙ¨Ó¶R¡A¶û¿ú¤Ó¦h°Ú¡H");
    return;
  }
  getdata(b_lines-2, 0, "½T©w­nªá $100000 ÁÊ¶RµL¤W­­«H½c?[y/N]",ans,3,LCECHO,0);
  if(ans[0]!='y')
    return;
  rec_get(fn_passwd, &xuser, sizeof(xuser), usernum);
  i=setuperm(cuser.userlevel,'f',100000);
  update_data();
  cuser.userlevel=i;
  substitute_record(fn_passwd, &cuser, sizeof(userec), usernum);
  tradelog(cuser.userid,3);
  pressanykey("®¥³ß±z¤w¸g±o¨ì¤FµL¤W­­ªº«H½c!!");
  return;
}
*/

void
p_give()
{
   int mymoney, money, mode=0, rc, rate=50;
   char id[IDLEN+1],buf[256],reason[60];
   FILE *fp=fopen("tmp/givemoney","w");
   fileheader mymail;
   time_t now;
   
   time(&now);
   move(15,0);
   update_data();
   usercomplete("  ½Ğ¿é¤J¹ï¤èªºID¡G", id);
   if (!id[0] || !ci_strcmp(id,"guest"))
      return;
      
   getdata(17, 0, "  0. »È¹ô  1.ª÷¹ô¡G",buf, 2, LCECHO, "0");
   if(buf[0]=='1') mode=1;
   if(buf[0]=='0') mode=0;

   getdata(19, 0, "  ­nÂà¦h¤Ö¹L¥h¡H", buf, 9, LCECHO,0);
   
   mymoney = atoi(buf);
   if(mymoney<100)
   {
     pressanykey("©êºp¡A¨C¦¸Âà±b¥æ©ö¤§¼Æ­È¤U­­¬°100");
     return;
   }
   
   if(check_money(mymoney, mode)) return;

   money = mymoney - mymoney/rate;

   if(mode==1)
   {
     degold(mymoney);
     rc=inugold(id, money);
   }
   else
   {
     demoney(mymoney);
     rc=inumoney(id, money);
   }
   
   if(rc>0)
   {
     sprintf(buf,"§@ªÌ: %s \n"
                 "¼ĞÃD:[Âà±b³qª¾] °e§A %d ªT %s ¡I\n"
                 "®É¶¡: %s\n",
                 cuser.userid,
                 money,
                 (mode ? "ª÷¹ô" : "»È¹ô"),
                 ctime(&now));
     fputs(buf,fp);
     while(!getdata(21,0,"  ½Ğ¿é¤J¤@­Ó²z¥Ñ¡G",reason,60,DOECHO ,"¿ú¤Ó¦h"));
     
     sprintf(buf,"[1;32m%s[37m Âà±b [33m%d [37m ªT %s ¨ì§Aªº±b¤á¸Ì¡C\n"
                 "¥Lªº²z¥Ñ¬O¡G[33m %s [m",
                 cuser.userid,
                 money,
                 (mode ? "ª÷¹ô" : "»È¹ô"),
                 reason);
     fputs(buf,fp);

     sprintf(buf,"home/%s/mailbox", id);
     stampfile(buf, &mymail);
     strcpy(mymail.owner, cuser.userid);
     rename ("tmp/givemoney",buf);
     
     sprintf(mymail.title,"[Âà±b³qª¾] %d ªT %s ¶i±b¤F¡I",
     money,
     (mode ? "ª÷¹ô" : "»È¹ô")) ;
     sprintf(buf,"home/%s/mailbox/.DIR",id);
     rec_add(buf, &mymail, sizeof(mymail));
     sprintf(buf,"[1;33m%s %s [37m§â %s [33m%d ªT [37mÂà±bµ¹[33m %s[37m",
     Cdate(&now),
     cuser.userid,
     (mode ? "ª÷¹ô" : "»È¹ô"),
     money,
     id);
     f_cat("log/bank.log",buf);
   }
   else
   {
     if(mode==1)
       ingold(mymoney);
     else
       inmoney(mymoney);
       
     pressanykey("Âà±b¥¢±Ñ!");
   }
   fclose(fp);
   return;
}

void
water_exchange()
{
  char buf[128], ans[10];
  int Money, rate=1;
//  time_t now = time(0);

  move(13,0);
  clrtobot();
  prints("  §A¦¬¶°¨ìªº¤ô²y¦³ %d ­Ó\n\n",cuser.receivemsg);
  prints("  ¤µ¤é¶×§I¤ñ²v¡G  ¤ô²y¡G»È¹ô = 1¡G%d\n",rate);

  getdata(17,0,"  ­n®³¦h¤Ö¤ô²y¨Ó´«¡H ",ans,10,LCECHO,0);
  if(!ans[0]) return;
  
  Money = atol(ans);
  if(Money<1 || Money > cuser.receivemsg) return;
  
  sprintf(buf,"  ¬O§_­n±N¤ô²y %d ­Ó ´«¦¨»È¹ô %d ªT ? [y/N] ",
          Money, Money);
  
  getdata(19,0,buf,ans,2,LCECHO,"y");
  
  if(ans[0] == 'y')
  {
    rec_get(fn_passwd, &xuser, sizeof(xuser), usernum);
    cuser.receivemsg = cuser.receivemsg - Money;
    substitute_record(fn_passwd, &cuser,sizeof(userec), usernum);
    inmoney(Money);
    pressanykey("OK! ±z¥Ø«eªº»È¹ô¬° %d ªT", cuser.silvermoney);
  }
  else
    pressanykey("¨ú®ø.....");
  
}


void
exchange()
{
  char buf[100],ans[10];
  int i, mymoney, money, rate=50;
  time_t now = time(0);

  move(13,0);
  clrtobot();
  prints("  §A¨­¤W¦³ª÷¹ô %d ¤¸,»È¹ô %d ¤¸\n\n",cuser.goldmoney,cuser.silvermoney);
  prints("  ¤µ¤é¶×§I¤ñ²v¡G  ª÷¹ô¡G»È¹ô = 1¡G10000\n");
  
  if(!getdata(17,0,"  0.»È¹ô´«ª÷¹ô  1.ª÷¹ô´«»È¹ô ",ans,2,LCECHO,0)) return;
  if(ans[0] < '0' || ans[0] > '1') return;
  i = atoi(ans);
  while(1)
  {
    if(i == 0)
      getdata(19,0,"  ­n®³¦h¤Ö»È¹ô¨Ó´«¡H ",ans,10,LCECHO,0);
    else
      getdata(19,0,"  ­n®³¦h¤Öª÷¹ô¨Ó´«¡H ",ans,10,LCECHO,0);
    if(!ans[0]) return;
    mymoney = atol(ans);
    if(i == 0)
    {
       if(mymoney<10000 || mymoney > cuser.silvermoney)
          continue;
    }
    else
    {
       if(mymoney<=0 || mymoney > cuser.goldmoney)
          continue;
    }
    break;
  }
  
  if(i == 0)
     money = mymoney/10000;
  else
     money = mymoney*10000 - mymoney*10000/rate;
  
  if(i == 0)
    sprintf(buf,"  ¬O§_­n±N»È¹ô %d ªT ´«¬°ª÷¹ô %d ªT ? [y/N]",mymoney,money);
  else
    sprintf(buf,"  ¬O§_­n±Nª÷¹ô %d ªT ´«¬°»È¹ô %d ªT ? [y/N]",mymoney,money);
    
  getdata(21,0,buf,ans,2,LCECHO,0);
  if(ans[0] == 'y')
  {
    if(i == 0)
    {
      demoney(mymoney); ingold(money);
      sprintf(buf,"[1;36m%s %s [37m§â»È¹ô [33m%d ¤¸ [37mÂà´«¬°ª÷¹ô %d ¤¸",
        Cdate(&now),cuser.userid,mymoney, money);
    }
    else
    {
      degold(mymoney); inmoney(money);
      sprintf(buf,"[1;32m%s %s [37m§âª÷¹ô [33m%d ¤¸ [37mÂà´«¬°»È¹ô %d ¤¸",
        Cdate(&now),cuser.userid,mymoney, money);
    }
    f_cat("log/bank.log",buf);
    pressanykey("OK! ±z¥Ø«eªºª÷¹ô %d ªT, »È¹ô %d ªT",
                cuser.goldmoney,cuser.silvermoney);
  }
  else
    pressanykey("¨ú®ø.....");
}

/* ª÷®w */
void
bank()
{
  char buf[10];
  if(lockutmpmode(BANK)) return;
  setutmpmode(BANK);
  stand_title("¹q¤l»È¦æ");

  if (count_multi() > 1)
  {
    pressanykey("±z¤£¯à¬£»º¤À¨­¶i¤J»È¦æËç !");
    unlockutmpmode();
    return;
  }

  counter(BBSHOME"/log/counter/bank","¨Ï¥Î»È¦æ",0);
  move(2, 0);
  update_data();
  prints("%12s ±z¦n§r¡IÅwªï¥úÁ{¥»»È¦æ¡C
[1;36mùúùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùû
ùø[32m±z²{¦b¦³»È¹ô:[33m %-12d [32mªT¡Aª÷¹ô:[33m %-12d[32m ªT[36m      ùø
ùø[32m±zªº»È¹ô¤W­­:[33m %-12d [32mªT¡C[36m                           ùø
ùàùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùâ
ùø ¥Ø«e»È¦æ´£¨Ñ¤U¦C¤T¶µªA°È¡G                               ùø",
    cuser.userid, cuser.silvermoney, cuser.goldmoney,
    (cuser.totaltime*1) + (cuser.numlogins*10) + (cuser.numposts*1000) + 10000 );
    move(7, 0);
      outs("\
ùø[33m1.[36m Âà±bªA°È[37m¡G1000ªT»È¹ô¥H¤W (¤âÄò¶O¡G 2%)                [36m ùø
ùø[33m2.[33m ¶×§IªA°È[37m¡G»È¹ô <--> ª÷¹ô (¤âÄò¶O¡G 2%)                [36m ùø
ùø[33m3.[32m ¥æ´«ªA°È[37m¡G¤ô²y ---> »È¹ô (¤âÄò¶O¡G 0%)                [36m ùø
ùø[33mQ.[37m Â÷¶}»È¦æ[36m                                               ùø
ùüùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùı[m");

  getdata(13,0,"  ½Ğ¿é¤J±z»İ­nªºªA°È¡G", buf, 2, DOECHO, 0);
  if (buf[0] == '1')
    p_give();
  else if (buf[0] == '2')
    exchange();
  else if (buf[0] == '3')
    water_exchange();

  update_data();
  pressanykey("ÁÂÁÂ¥úÁ{¡A¤U¦¸¦A¨Ó¡I");
  unlockutmpmode();
}

void
goldwish()
{
  int i,m;
//  time_t now = time(0);

  move(2,0);
  clrtobot();
  degold(1);

  i=rand() % 3000 - 2000;
  move(13,26);

  if (i>0)
     if (i%100 ==0)
     {
       m=i/100;
       cuser.goldmoney+=m;
       prints("¤Ñ¤U±¼¤U¨Óª÷¹ô %d ªT!",m);
     }
     else
     {
       cuser.exp+=i;
       prints("¸gÅç­È¤W¤É %d ÂI!",i);
     }

  else
     if (i%10==0)
     {
       m = (cuser.goldmoney > (-i/10) ? (-i/10) : cuser.goldmoney);
       cuser.goldmoney-=m;
       prints("%d ªTª÷¹ôÅÜ¦¨¤F¥ÛÀY...",m);
     }
     else
     {
       m = (cuser.exp > (-i/10) ? (-i/10) : cuser.exp);
       cuser.exp-=m;
       prints("¸gÅç­È¤U­° %d ÂI...",m);
     }
  substitute_record(fn_passwd, &cuser, sizeof(userec), usernum);

  pressanykey("Ä@±æ¤p´cÅ]¡G®¥³ß§A....³Ü³Ü³Ü...");
}

void
silverwish()
{
  int i;
//  time_t now = time(0);
  move(2,0);
  clrtobot();
  demoney(1000);
  i=rand() % 200 - 100;
  i= (i<0) ? 1 : i;
  move(13,26);

  if ((i+1)%25==0)
  {
    cuser.bequery = xuser.bequery + (i%2);
    prints("¤H®ğ«ü¼Æ¤W¤É %d ÂI!",(i%2));
  }
  else
  {
    cuser.exp = xuser.exp + i;
    prints("¸gÅç­È¤W¤É %d ÂI!",i);
  }
  substitute_record(fn_passwd, &cuser, sizeof(userec), usernum);

  pressanykey("Ä@±æ¤p´cÅ]¡G®¥³ß§A....³Ü³Ü³Ü...");
}

void
darkwish()
{
  char buf[10];
  if(lockutmpmode(WISH)) return;
  setutmpmode(WISH);
  stand_title("¶Â·t³\\Ä@¦À");

  if (count_multi() > 1)
  {
    pressanykey("±z¤£¯à¬£»º¤À¨­¶i¤J³\\Ä@¦À®@ !");

    unlockutmpmode();
    return;
  }

  counter(BBSHOME"/log/counter/darkwish","¶Â·t³\\Ä@¦À",0);

  do{
  move(3, 0);
  prints("%12s ¼K...±z¦n..Åwªï¨Ó¨ì ¶Â·t³\\Ä@¦À...

  §Ú¬O[1;31;40mÄ@±æ¤p´cÅ][1;37;40m¡A¥u­n±zªÖªá¤p¿ú¡A¥»Å]´N¹F¦¨§AªºÄ@±æ..

  ¼K¼K¡A¥ıÅı§Ú¬İ¬İ§A¦³¦h¤Ö¿ú... [1;33;40mª÷¤l[1;37;40m:%-12d [1;36;40m»È¨â[1;37;40m:%-12d",

  cuser.userid, cuser.goldmoney, cuser.silvermoney);
  move(9, 0);

  outs("
[1;33;40m  1. ¤@ªTª÷¹ô³\\¤@Ä@ -- ¦n¹B±o¦nÄ@¡AÃa¹B±oÃaÄ@¡A ¥ş¾a±zªº¤â®ğ..¼K¼K..

[1;36;40m  2. ¤dªT»È¹ô±o¤@Ä@ -- ¦³¥»Å]¸n§A¡A¦³¦nµLÃa°Õ..

[1;37;40m  Q. Â÷¶}

");
  getdata(16,0,"  ½Ğ¿ï¾Ü±z­nªº§a¡A¼K¼K..¡G", buf, 3, DOECHO, 0);
  if (buf[0] == '1' && cuser.goldmoney >= 1)
    goldwish();
  else if (buf[0] == '2' && cuser.silvermoney >= 1000)
    silverwish();
  else
      break;
//  update_data();
  }while(1);

  pressanykey("¼K¼K¼K....¤U¦¸¦A¨Ó§r...");
  unlockutmpmode();
}

void
time_machine()
{
  char genbuf[128], pwdfile[128], keyword[128], ans[10];
  int id=0;

  if(lockutmpmode(TIME_MACHINE)) return;

  setutmpmode(TIME_MACHINE);
  stand_title("¶Â·t®É¥ú¾÷");
  if (count_multi() > 1)
  {
    pressanykey("±z¤£¯à¬£»º¤À¨­¶i¤J!");
    
    unlockutmpmode();
    return;
  }
  if (!HAS_PERM(PERM_LOGINOK))
  {
    pressanykey("±zªºÅv­­¤£¨¬!");
    unlockutmpmode();
    return;
  }

  counter(BBSHOME"/log/counter/time_machine","®É¥ú¾÷",0);

  update_data();
  pressanykey("®É¥ú¾÷±Ò°Ê¤¤...");
      
  ans[0]='1';
  while(ans[0]!='3')
  {
    stand_title("¶Â·t®É¥ú¾÷");
    move(1,0);
    clrtobot();
    move(1,0);
    prints("


                      ùúùùùùùùùùùùùùùùùùùùùùùùùùùû
                      ùø®É¥ú¾÷¾Ş§@¿ï³æ          ùø
                      ùàùùùùùùùùùùùùùùùùùùùùùùùùùâ
                      ùø1. ®É¥ú°O¿ıÃ¯  $ 1 ª÷   ùø
                      ùø2. ®É¥ú¦^·¹¾¹  $ 10»È   ùø
                      ùø3. Â÷¶}                 ùø
                      ùüùùùùùùùùùùùùùùùùùùùùùùùùùı

");
    getdata(12,0,"  ½Ğ¿é¤J±z»İ­nªºªA°È¡G", ans, 2, DOECHO, 0);
    if(ans[0]=='1' && !check_money(1,1))
    {
       sprintf(pwdfile, "cat boards/System_Log/M*");
       getdata(14,0,"  ½Ğ¿é¤J­n¬d¸ßªºÃöÁä¤å¦r¤§¤@¡G",genbuf, 30, DOECHO, 0);
       genbuf[30]='\0';
       sprintf(keyword, " | grep \"%s\"", genbuf);
       strcat(pwdfile, keyword);

       getdata(15,0,"  ½Ğ¿é¤J­n¬d¸ßªºÃöÁä¤å¦r¤§¤G¡G",genbuf, 30, DOECHO, 0);
       genbuf[30]='\0';
       sprintf(keyword, " | grep \"%s\"", genbuf);
       strcat(pwdfile, keyword);

       sprintf(keyword, "tmp/%s.log", cuser.userid);
       sprintf(genbuf, "%s > %s", pwdfile, keyword);
       
       system(genbuf);
       more(keyword, YEA);
       degold(1);
    }
    else if(ans[0]=='2' && !check_money(10,0))
    {
      if(HAS_PERM(PERM_SYSOP))
      {
        getdata(14,0,"  ½Ğ¿é¤J­n¬d¸ßªºID¡G",genbuf, IDLEN+1, DOECHO, 0);
        if(genbuf[0])
          strcpy(keyword, genbuf);
      }
      else
      {
        strcpy(keyword, cuser.userid);
      }
        
         getdata(15,0,"  ½Ğ¿é¤J­n¦^·¹ªº¤é´Á¡G",genbuf, 5, DOECHO, 0);
         genbuf[5]='\0';
         sprintf(pwdfile,"backup/passwds_%s", genbuf);
         if(dashs(pwdfile)==-1)
         {
           pressanykey("¨S¦³¦¹°O¿ıÀÉ®×!");
         }
         else
         {
           id = searchuser(keyword);
           rec_get(pwdfile, &xuser, sizeof(userec), id);
           move(1,0);
           user_display(&xuser, 1);
           pressanykey(NULL);
           demoney(10);
         }
         
    }
  }   
  
  pressanykey("®É¥ú¾÷Ãö³¬¤¤...");    
  unlockutmpmode();
  return;
}
