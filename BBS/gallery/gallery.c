/*-------------------------------------------------------*/
/* SO/gallery.c   ( NTHU CS MapleBBS Ver 2.36 )          */
/*-------------------------------------------------------*/
/* target : create gallery account			 */
/* create : 04/03/08                                     */
/* update : 04/03/08                                     */
/* author : shakalaca.bbs@php.twbbs.org			 */
/*-------------------------------------------------------*/

#include "bbs.h"
#include <md5.h>          

#define ALBUM_PATH	"/home/data/www/albums"
#define FN_GALLERY_LOG	"run/gallery.log"
          
typedef struct
{
  char username[32];
  char fullname[32];
  char password[64];
  char email[64];
  char uid[32];
  char lastactiondate[64];
} GalleryUser;

/* ------------------------------------------------------------------ */

void
f_add(fpath, msg)
  char *fpath;
  char *msg;
{
  int fd;

  if ((fd = open(fpath, O_WRONLY | O_CREAT | O_APPEND, 0664)) >= 0)
  {
    write(fd, msg, strlen(msg));
    close(fd);
  }
}


void
f_join(from, to)
  char *from, *to;
{
  int in, out, len;
  char buf[512];

  if ((in = open(from, O_RDONLY)) >= 0)
  {
    if ((out = open(to, O_WRONLY | O_CREAT | O_APPEND, 0664)) >= 0)
    {
      while (len = read(in, buf, sizeof(buf)))
        write(out, buf, len);

      close(out);
    }
    close(in);
  }
} 


void
gallery_log(mode)
  char *mode;
{
  time_t now = time(0);
  char buf[128];

  sprintf(buf, "%s %s (%s)\n", Cdate(&now), mode, cuser.userid);

  f_add(FN_GALLERY_LOG, buf);
}


/* ------------------------------------------------------------------ */

int
create_album(u, uid)
  GalleryUser u;
  char *uid;
{
  char fpath[128], file[128], buf[256], buf2[128];

  sprintf(fpath, "%s/%s", ALBUM_PATH, cuser.userid);

  if (!dashd(fpath))
  {
    umask(0002);
    if (mkdir(fpath, 0775) == -1)
    {
      pressanykey("���~�G�L�k�إ߬�ï�A�гq�������I");
      sprintf(fpath, "%s/.users/%s", ALBUM_PATH, uid);
      unlink(fpath);
      gallery_log("ERROR CREATING DIRECTORY");
      return 0;
    }
    
    sprintf(file, "%s/photos.dat", fpath);
    f_add(file, "N;");
      
    sprintf(file, "%s/album.dat", fpath);

    sprintf(buf2, "%s ����ï", cuser.userid);
    sprintf(buf, "O:5:\"album\":5:{s:6:\"fields\";a:57:{s:5:\"title\";s:%d:\"%s\";",
      strlen(buf2), buf2);
    f_add(file, buf);    
    
    f_join("etc/gallery/album.dat.head", file);

    sprintf(buf, "s:11:\"clicks_date\";i:%ld;", time(NULL));
    f_add(file, buf);

    f_join("etc/gallery/album.dat.body", file);
    
    sprintf(buf, "s:4:\"name\";s:%d:\"%s\";s:5:\"owner\";%s;s:13:\"last_mod_time\";i:%ld;", 
      strlen(cuser.userid), cuser.userid, u.uid, time(NULL));
    f_add(file, buf);
    
    f_join("etc/gallery/album.dat.foot", file);
  }  
/*
  else
    pressanykey("�ؿ��w�s�b�A���A�t�~�إ�");
*/

  return 1;
}


int 
create_hashfile(u, hash)
  GalleryUser u;
  char *hash;
{
  char fpath[64], input[1024];

  sprintf(input, "O:12:\"gallery_user\":13:{s:8:\"username\";%s;s:8:\"fullname\";\
%s;s:8:\"password\";%s;s:5:\"email\";%s;s:7:\"isAdmin\";b:0;s:15:\"canCreateAlbums\";s:1:\"0\";\
s:3:\"uid\";%s;s:15:\"defaultLanguage\";s:5:\"en_US\";s:7:\"version\";i:5;s:15:\"recoverPassHash\";N;\
s:10:\"lastAction\";s:8:\"register\";s:14:\"lastActionDate\";%s;s:9:\"origEmail\";%s;}",
    u.username, u.fullname, u.password, u.email, u.uid, u.lastactiondate, u.email);

  sprintf(fpath, "%s/.users/%s", ALBUM_PATH, hash); 
  f_add(fpath, input);  

  if (!dashf(fpath))
    return 0;
  else
    return 1;
}


void
gen_salt(password, retbuf)
  char *password, *retbuf;
{
  int i, ch;
  char salt[5], tmpbuf[64], buf[33];
  
  srand(time(NULL) * 10000000);
  for (i = 0; i < 4; i++)
  {
    ch = rand()%(109-48+1) +48;
    if(ch > 57) ch += 7;
    if(ch > 90) ch += 6;
    salt[i] = ch;
  }
  salt[4] = '\0';

  sprintf(tmpbuf, "%s%s", salt, password);
  MD5Data(tmpbuf, strlen(tmpbuf), buf);
  sprintf(retbuf, "%s%s", salt, buf);
}


int
create_account()
{
  GalleryUser gu; 
  char tmpbuf[64], passbuf[32];
 
  if (cuser.uflag & USER_GALLERY)
  {
    pressaykey("�z�w�g�ӽйL��ï�F�I");
    return 0;
  }
   
  sprintf(gu.username, "s:%d:\"%s\"", strlen(cuser.userid), cuser.userid);
  sprintf(gu.fullname, "s:%d:\"%s\"", strlen(cuser.username), cuser.username);
  sprintf(gu.email, "s:%d:\"%s\"", strlen(cuser.email), cuser.email);
  sprintf(gu.lastactiondate, "i:%ld", time(NULL));

  for (;;)
  {
    if ((getdata(b_lines, 0, "�г]�w�K�X�G", passbuf, 9, NOECHO, 0) < 3) ||
      !strcmp(passbuf, cuser.userid))
    {
      pressanykey("�K�X��²��A���D�J�I�A�ܤ֭n 4 �Ӧr�A�Э��s��J");
      continue;
    }

    getdata(b_lines, 0, "���ˬd�K�X�G", passbuf + 10, 9, NOECHO, 0);
    if (!strcmp(passbuf, passbuf + 10))
      break;
      
    pressanykey("�K�X��J���~�A�Э��s��J�K�X�C");
  }

  passbuf[strlen(passbuf)] = '\0';
  gen_salt(passbuf, tmpbuf);
  sprintf(gu.password, "s:%d:\"%s\"", strlen(tmpbuf), tmpbuf);

  sprintf(tmpbuf, "%ld_%ld", time(NULL), rand());  
  sprintf(gu.uid, "s:%d:\"%s\"", strlen(tmpbuf), tmpbuf);
  
  if (!create_hashfile(gu, tmpbuf))
  {
    gallery_log("ERROR CREATING USER PROFILE");
    pressanykey("���~�G�L�k�إ߸���ɡA�гq�������I");
    return 0;
  }

  if (!create_album(gu, tmpbuf))
  {
    return 0;
  }
  
  cuser.uflag |= USER_GALLERY;

  gallery_log("CREATE ALBUM");
  pressanykey("�w�إ߸�� :)");  
  return 0;
}
