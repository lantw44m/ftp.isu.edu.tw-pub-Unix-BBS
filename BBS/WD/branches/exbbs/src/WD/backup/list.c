/* list.c       ( NTHU CS MapleBBS Ver 2.36 )            */
/*-------------------------------------------------------*/
/* target : lists' routines                              */
/* create : 00/01/12                                     */
/* update : 00/01/12                                     */
/*-------------------------------------------------------*/

#include "bbs.h"

extern int cmpuname();
extern cmpfilename();
extern char currdirect[64];
extern struct one_key list_comm[];
extern int last_line;
extern char msg_cc[];
void listtitle();
void listdoent();
void ListEdit();

// wildcat : shakalaca §A¨º­Ó list_belong §Ú¤£·|¥Î /_\
int 
belong_list(fname, userid)
  char *fname, *userid; 
{
  int fd, can = 0;
  PAL pal;
    
  if ((fd = open(fname, O_RDONLY)) >= 0)
  {
    while (read(fd, &pal, sizeof(pal)) == sizeof(pal))
    {
      if (!strcmp(pal.userid, userid))
      {
        if(pal.ftype & M_BAD)
          can = 0;
        else
          can = 1;
        break;
      }
    }
    close(fd);
  }
  return can;
}

int
brd_bad_guy(fname, userid)
  char *fname, *userid;
{
  int fd;
  PAL pal;

  if ((fd = open(fname, O_RDONLY)) >= 0)
  {
    while (read(fd, &pal, sizeof(pal)) == sizeof(pal))
    {
      if (!strcmp(pal.userid, userid))
        if(pal.ftype & M_BAD)
           return 1;
    }
    close(fd);
  }
  return 0;
}



static void
list_desc(fhdr, echo)
  fileheader *fhdr;
  int echo;
{
  char buf[80];
  
  if (currstat == LISTMAIN)
    sprintf(buf, "¹ï¦¹¦W³æªº´y­z¡G");
  else
    sprintf(buf, "¹ï¥Lªº´y­z¡G");
  
  if (echo == DOECHO)
    memset(fhdr, 0, sizeof(fileheader));
  getdata(1, 0, buf, fhdr->title, 60, DOECHO,
    (echo != DOECHO) ? fhdr->title : 0);
  if (strstr(currdirect, FN_PAL) || strstr(currdirect, "boards"))
  {
    getdata(1, 0, "Ãa¤H(¸TÁn)¶Ü (Y/N) ? [N]", buf, 3, DOECHO, 0);
    if (*buf != 'y')
      fhdr->filemode |= M_PAL;
    else
      fhdr->filemode |= M_BAD;
  }
}


int
list_add()
{
  char listfile[80], fpath[80], ans[4];
  fileheader hdr;
  time_t now = time(NULL);
  struct tm *ptime = localtime(&now);

  if (currstat != LISTEDIT)
  {
    strcpy(listfile, "list.0");
    getdata(1, 0, "·s¼W 1)¤@¯ë¦W³æ 2)ªB¤Í¦W³æ 3)¤å³¹³qª¾ Q)uit¡H[1] ", 
      ans, 3, LCECHO, 0);

    if (ans[0] == '\0')
      ans[0] = '1';
    
    switch(ans[0])
    {
    case '2':
      strcpy(listfile, FN_PAL);
      break;
  
    case '3':
      strcpy(listfile, FN_POSTLIST);
      break;
    
    case 'q':
      return RC_FULL;
      
    default:
      getdata(1, 0, "½Ð¿ï¾Ü¦W³æ (0-9)¡H[0] ", ans, 3, DOECHO, 0);

      if (ans[0] == '\0')
        ans[0] = '0';

      if (ans[0] >= '0' && ans[0] <= '9')
        listfile[5] = ans[0];
    }

    setuserfile(fpath, FN_LIST);
    if (belong_list(fpath, listfile))
    {
      pressanykey("¤w¦³¦¹¦W³æ¤F !");
      return RC_FULL;
    }

    list_desc(&hdr, DOECHO);
    strcpy(hdr.filename, listfile);
    sprintf(hdr.date, "%02d/%02d",  ptime->tm_mon + 1, ptime->tm_mday);
    setuserfile(fpath, FN_LIST);
    if (rec_add(fpath, &hdr, sizeof(fileheader)) == -1)
      pressanykey("¨t²Îµo¥Í¿ù»~! ½Ð³qª¾¯¸ªø!");

    if (currstat != LISTMAIN)
      ListMain();
  }
  else
  {
    move(1, 0);
    usercomplete(msg_uid, listfile);

    if (belong_list(currdirect, listfile))
    {
      pressanykey("¤w¦³³o¦ì¯¸¤Í¤F !");
      return RC_FULL;
    }

    if (listfile[0] && searchuser(listfile))
    {
      hdr.filemode = 0;
      list_desc(&hdr, DOECHO);
      if (strcmp(listfile, cuser.userid) && 
        !strstr(currdirect, "boards") &&
         strstr(currdirect, FN_PAL))
      {
        PAL aloha;
        int pos;
        memset(&aloha, 0, sizeof(aloha));

        sethomefile(fpath, listfile, FN_ALOHA);
              
        pos = rec_search(fpath, &aloha, sizeof(aloha), cmpuname, (int) cuser.userid);
        if (pos)
          rec_del(fpath, sizeof(PAL), pos, NULL, NULL);

        getdata(1, 0, "¥[¤J¤W¤U¯¸³qª¾¶Ü (Y/N) ? [N]", ans, 3, DOECHO, 0);
        if (*ans == 'y')
        {
          hdr.filemode |= M_ALOHA;
          sprintf(aloha.date, "%02d/%02d",  ptime->tm_mon + 1, ptime->tm_mday);
          strcpy(aloha.userid, cuser.userid);
          rec_add(fpath, &aloha, sizeof(aloha));
        }
      }
      strcpy(hdr.filename, listfile);
      sprintf(hdr.date, "%02d/%02d",  ptime->tm_mon + 1, ptime->tm_mday);
      if (rec_add(currdirect, &hdr, sizeof(fileheader)) == -1)
        pressanykey("¨t²Îµo¥Í¿ù»~! ½Ð³qª¾¯¸ªø!");
    }
  }
   
  if (strstr(currdirect, FN_PAL))
    friend_load();
  return RC_FULL;
}


static int
list_del(ent, fhdr, direct)
  int ent;
  fileheader * fhdr;
  char *direct;
{
  char genbuf[100];

  getdata(1, 0, msg_del_ny, genbuf, 3, LCECHO, 0);
  if (genbuf[0] == 'y')
  {
    if (currstat == LISTMAIN)
    {
      setuserfile(genbuf, fhdr->filename);
      unlink(genbuf);
    }
    if (strstr(currdirect, FN_PAL))
    {
      PAL aloha;
      int pos;
	
      sethomefile(genbuf, fhdr->filename, FN_ALOHA);
      pos = rec_search(genbuf, &aloha, sizeof(aloha), cmpuname, (int) cuser.userid);
      if (pos)
        rec_del(genbuf, sizeof(PAL), pos, NULL, NULL);
    }
    strcpy(currfile, fhdr->filename);
    delete_file (direct, sizeof (fileheader), ent, cmpfilename);
  }

  if (strstr(currdirect, FN_PAL))
    friend_load();
  return RC_FULL;
}


/* shakalaca.000115: À³¸Ó¥Î i_read ¦AÅª¤@¹Mªº.. ºC§ï */
static int
list_view(ent, fhdr, direct)
  int ent;
  fileheader * fhdr;
  char *direct;
{
  char buf[80];
    
  if (!fhdr)
    return RC_FULL;

  if (currstat == LISTMAIN)
  {
    setuserfile(buf, fhdr->filename);
    ListEdit(buf);
  }
  else
  {
     my_query(fhdr->filename);
  }

  return RC_FULL;
}


static int
list_merge(ent, fhdr, direct)
  int ent;
  fileheader *fhdr;
  char *direct;
{
  /* ¤Þ¤J¦W³æ: §PÂ_­n¤Þ¤Jªº¦W³æ¤¤¦pªG¦³ id ¦s¦b target «h¸õ¹L */
  int fd;
  PAL list;
  char buf[STRLEN], source[STRLEN];

  if (currstat == LISTEDIT)
  {
// wildcat : ¥»¨Ó getdata §Ñ°Oµ¹ªø«×¤F°Õ :Q
    if (!getdata(b_lines, 0, "¤Þ¤J 1)¤@¯ë¦W³æ 2)ªB¤Í¦W³æ 3)¤å³¹³qª¾ ¡H", buf, 2, DOECHO, 0))
      return RC_FOOT;

    switch(buf[0])
    {
    case '1':
      strcpy(source, "list.0");
      getdata(1, 0, "½Ð¿ï¾Ü¦W³æ (0-9)¡H[0] ", buf, 3, DOECHO, 0);

      if (buf[0] == '\0')
        buf[0] = '0';

      if (buf[0] >= '0' && buf[0] <= '9')
        source[5] = buf[0];

      break;
    
    case '2':
      strcpy(source, "pal");
      break;
  
    case '3':
      strcpy(source, "postlist");
      break;
    
    default:
      return RC_FULL;
    }

    setuserfile(buf, source);
    if ((fd = open(buf, O_RDONLY)) >= 0)
    {
      while (read(fd, &list, sizeof(list)) == sizeof(list))
      {
        if (!belong_list(currdirect, list.userid))
        /* ±N list.filename ¥[¤J currdirect */
          rec_add(currdirect, &list, sizeof(PAL));
      }
      close(fd);
    }
    return RC_NEWDIR;  
  }
  return RC_NONE;
}


static int
list_multi(ent, fhdr, direct)
  int ent;
  fileheader *fhdr;
  char *direct;
{
  int reciper;
  
  if (currstat == LISTEDIT)
  {
    CreateNameList();
    LoadNameList(&reciper, currdirect, msg_cc);
    multi_send(NULL, 0);
    return RC_FULL;  
  }
  return RC_NONE;
}


static int
list_edit(ent, fhdr, direct)
  int ent;
  fileheader *fhdr;
  char *direct;
{
  fileheader tmpfhdr = *fhdr;
  char fpath[80], ans[3];
  
  tmpfhdr.filemode = 0;
  list_desc(&tmpfhdr, GCARRY);

/* shakalaca.000117: §ï´y­z¦Ó¥H¹À.. :p ¥Î enter ¤]¥i¥H§ï¤º®e, so..
  setuserfile(buf, fhdr->filename);
  if (currstat == LISTMAIN)
    ListEdit(buf);
*/
  if (strcmp(fhdr->filename, cuser.userid) && 
    !strstr(currdirect, "boards")&& 
    strstr(currdirect, FN_PAL))
  {
    PAL aloha;

    sethomefile(fpath, fhdr->filename, FN_ALOHA);
    {
      int pos;
              
      pos = rec_search(fpath, &aloha, sizeof(aloha), cmpuname, (int) cuser.userid);
      if (pos)
        rec_del(fpath, sizeof(PAL), pos, NULL, NULL);
    }                    
    getdata(1, 0, "¥[¤J¤W¤U¯¸³qª¾¶Ü (Y/N) ? [N]", ans, 3, DOECHO, 0);
    if (*ans == 'y')
    {
      tmpfhdr.filemode |= M_ALOHA;
      strcpy(aloha.userid, cuser.userid);
      rec_add(fpath, &aloha, sizeof(aloha));
    }
  }

  *fhdr = tmpfhdr;
  substitute_record (direct, fhdr, sizeof (*fhdr), ent);
    
  return RC_FULL;
}


static int
list_move(ent, fhdr, direct)
  int ent;
  fileheader *fhdr;
  char *direct;
{
  fileheader *tmp;
  char newnum[5], buf[30];
  int num, max, min;
  int fail;

  sprintf (buf, "½Ð¿é¤J²Ä %d ¿ï¶µªº·s¦¸§Ç¡G", ent);
  if (!getdata (b_lines, 0, buf, newnum, 5, DOECHO, 0))
    return RC_FOOT;

  if ((num = atoi(newnum) - 1) < 0)
    num = 0;
  else if (num >= last_line)
    num = last_line - 1;

  min = num < (ent - 1) ? num : (ent - 1);
  max = num > (ent - 1) ? num : (ent - 1);
  tmp = (fileheader *) calloc (max + 1, sizeof(fileheader));

  fail = 0;
  if (get_records (direct, tmp, sizeof(fileheader), 1, min) != min)
    fail = 1;
    
  if (num > (ent - 1))
  {
    if (get_records (direct, &tmp[min], sizeof(fileheader), ent + 1, max - min) != max - min)
      fail = 1;
    if (get_records (direct, &tmp[max], sizeof(fileheader), ent, 1) != 1)
      fail = 1;
  }
  else
  {
    if (get_records (direct, &tmp[min], sizeof(fileheader), ent, 1) != 1)
      fail = 1;
    if (get_records (direct, &tmp[min + 1], sizeof(fileheader), num + 1, max - min) != max - min)
      fail = 1;
  }
  if (!fail)
    substitute_record (direct, tmp, sizeof(fileheader) * (max + 1), 1);
  ent = num + 1;
  free (tmp);
  return RC_NEWDIR;
}


static int
list_help(ent, fhdr, direct)
  int ent;
  fileheader * fhdr;
  char *direct;
{
  move(3, 0);
  clrtobot();
  move(3, 0);
  prints("\n¬I¤u¤¤.. ^^;");
  pressanykey(NULL);
            
  return RC_FULL;
}
              

struct one_key list_comm[] = {
  'r', list_view, 		// ¶i¤J/ÀËµø
  'c', list_edit,		// ­×§ï
  'a', list_add,		// ·s¼W
  'd', list_del,		// §R°£   
/* ·|µo¥Í°ÝÃDªº¦a¤è.. */
  'i', list_merge, 		// ¤Þ¤J¨ä¥L¦W³æ
  's', list_multi,		// ¸s²Õ±H«H
  'm', list_move,		// §ïÅÜ¦ì¸m
  'h', list_help,  		// ¨Ï¥Î»¡©ú
  '\0', NULL
};


void
listdoent(num, ent, row, bar)
  int num;
  fileheader *ent;
{
  char buf[128],outmsg[256];
  
  if(bar==1)
    sprintf(outmsg,"¡´[1;33;44m");
  else
    sprintf(outmsg,"  [1;37;40m");
  
  if (currstat != LISTMAIN)
  {
    sprintf(buf,"%3d [1;33m%c\033[31m%c\033[37m%c  %-6s %-13s %-40s[m", num, 
      ent->filemode & M_PAL ? 'f' : ' ',
      ent->filemode & M_BAD ? 'b' : ' ',
      ent->filemode & M_ALOHA ? 'a' : ' ',
      is_alnum(ent->date[0]) ? ent->date : "      ", ent->filename, ent->title);
  }
  else
    sprintf(buf,"%3d %-6s %-13s %-40s[m", num, ent->date, ent->filename, ent->title);

  strcat(outmsg,buf);
  outmsgline(outmsg,row);    
  move(b_lines,0);
}


void
listtitle()
{
  showtitle("¸s²Õ¦W³æ", BoardName);
  if (currstat != LISTMAIN)
/*  if (strstr(currdirect, FN_PAL) || strstr(currdirect, "boards")) */
  outs("\
  [a]·s¼W [c]­×§ï [d]§R°£ [m]²¾°Ê [i]¤Þ¤J¦W³æ [s]¸s²Õ±H«H [¡÷]Æ[¬Ý [h]elp\n\
" COLOR1 "\033[1m ½s¸¹ ¼Ò¦¡ ¤é  ´Á ¦W  ºÙ        ´y        ­z                                   \033[0m");
else
  outs("\
  [a]·s¼W [c]­×§ï [d]§R°£ [m]²¾°Ê [¡÷]Æ[¬Ý [h]elp\n\
" COLOR1 "\033[1m ½s¸¹ ¤é  ´Á ¦W  ºÙ        ´y        ­z                                        \033[0m");
}


int
ListMain()
{
  char buf[STRLEN];

  setuserfile(buf, FN_LIST);
  i_read(LISTMAIN, buf, listtitle, listdoent, list_comm, NULL);

  return RC_FULL;
}


void
ListEdit(fname)
  char *fname;
{
  char currdirect0[STRLEN];
  
  strcpy(currdirect0, currdirect);
  strcpy(currdirect, fname);
  i_read(LISTEDIT, fname, listtitle, listdoent, list_comm, NULL);
  strcpy(currdirect, currdirect0);
}
