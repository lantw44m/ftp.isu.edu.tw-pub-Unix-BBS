#include "bbs.h"

#ifdef  EMAIL_JUSTIFY
int check_magic_key() {
  char buf[80], key[48], *mk_err[] = { "���Ҹ�ƿ��~ �Э��s����H�o�{�ҫH" };
  int i;
  FILE *fp;

  if(HAS_PERM(PERM_LOGINOK)) {
    pressanykey("�z�w�q�L�{�� ���ݰ��榹�\\��");
    return FULLUPDATE;
  }

  if(count_multi() > 1) {
    pressanykey("���\\��u���\\��@�n���ɰ���");
    return FULLUPDATE;
  }

  setuserfile(buf, "MAGIC_KEY");
  if((fp = fopen(buf, "r")) == NULL) {
    pressanykey(mk_err);
    return FULLUPDATE;
  }

  fgets(buf, 80, fp);
  for(i = 0 ; i < 80 && (uschar)buf[i] > ' ' ; i++) ;

  if(i == 80) {
    pressanykey(mk_err);
    return FULLUPDATE;
  }

  getdata(b_lines, 0, "�п�J�{�ҽs�X (�� 12345:12345) ", key, 32, DOECHO, 0);

  if(!strcmp(buf, key)) {
    item_update_passwd('l');
    xuser.userlevel |= PERM_LOGINOK;

    sprintf(xuser.justify, "%-.30s%s", xuser.email, "[�s�X]");
    strcpy(cuser.justify, xuser.justify);
    item_update_passwd('u');

    pressanykey("�{�Ҧ��\\ �Э��n��Y�i�����v��");
  }
  else pressanykey("�s�X�T�{���~ �Э��s��J");

  return FULLUPDATE;
}
/* endif of EMAIL_JUSTIFY */
#endif
