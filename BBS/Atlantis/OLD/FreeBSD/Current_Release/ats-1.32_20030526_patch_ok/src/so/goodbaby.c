#include "bbs.h"

#ifdef GOOD_BABY
union rej {
  struct {
    char a;
    char b;
    char c;
    char d;
  } A;

  struct {
    usint i;
  } B;
} rtemp = {0};


int reject_me() {
  char ch, buf[4];
  int hr;

  if(belong("etc/reject_me", cuser.userid)) {
    usint now = (usint)time(NULL);

    item_update_passwd('l');
    pressanykey("ĵ�i !!! �@���]�w�ɮ� �����N�L�k�A�W�� !! �мf�V�Ҽ{");
    getdata(23, 0, "(A)�]�w�h�[�����i�W��  (B)�]�w�C���ɤ���W�� [Q] : ",
            buf, 2, LCECHO, 0);
    ch = buf[0];

    if(ch == 'a') {
      getdata(b_lines, 0, "�]�w�X�p�ɤ��i�A�W�� (1-336) [Q] ? ", buf, 4,
              DOECHO, 0);
      if(buf[0]) {
        hr = atoi(buf);
        if(hr <= 0 && hr > 336) goto SET_ERROR;
        xuser.reject_me_time = now + hr * 3600;
      }
      else goto END_SET;
    }

    if(ch == 'b') {
      getdata(b_lines, 0, "�]�w�C�餣��W�u�}�l�ɶ� (0-23) [Enter ����] ? ",
              buf, 3, DOECHO, 0);
      if(buf[0]) {
        rtemp.A.a = (char)atoi(buf);
        if(rtemp.A.a < 0 || rtemp.A.a > 23) goto SET_ERROR;
      }
      else goto END_SET;

      getdata(b_lines, 0, "�]�w�C�餣��W�u�����ɶ� (0-23) [Enter ����] ? ",
              buf, 3, DOECHO, 0);
      if(buf[0]) {
         rtemp.A.b = (char)atoi(buf);
         if(rtemp.A.b < 0 || rtemp.A.b > 23) goto SET_ERROR;
      }
      else goto END_SET;

      /* Dopin: �P�@�Ӯɶ��`�� ���~ */
      if(rtemp.A.a == rtemp.A.b) goto SET_ERROR;
      else xuser.reject_me_time = rtemp.B.i;

      pressanykey("�]�w�ɶ��� %d �ɦ� %d ��", rtemp.A.a, rtemp.A.b);
    }

    cuser.reject_me_time = xuser.reject_me_time;
    item_update_passwd('u');

    getdata(b_lines, 0, "�{�b�N���� BBS �s�u y/N ? ", buf, 2, LCECHO, 0);
    if(buf[0] == 'y') {
      reset_tty();
      exit(1);
    }
    goto END_SET;

SET_ERROR:
    pressanykey("���� �ɭ��]�w���~ ...");
  }
  else pressanykey("��p �z�����v�i�H�ϥΥ��\\��... �p���ݭn�ЦV�����ӽ�");

END_SET:
  return FULLUPDATE;
}

void check_reject_me() {
  usint now = (usint)time(NULL);

  if(cuser.reject_me_time && belong("etc/reject_me", cuser.userid)) {
    if(cuser.reject_me_time < 65536) {
      char h, fg = 0;

      get_tm_time(NULL, NULL, NULL, &h, NULL);
      rtemp.B.i = cuser.reject_me_time;
      if(rtemp.A.a > rtemp.A.b) { if(h >= rtemp.A.a || h < rtemp.A.b) fg = 1; }
      else                      { if(h >= rtemp.A.a && h < rtemp.A.b) fg = 1; }

      if(fg) {
        pressanykey("�z�� %d �ɦ� %d �ɶ����o�W�u", rtemp.A.a, rtemp.A.b);
        goto RESET_TTY;
      }
      else return;
    }
    else {
      if(now < cuser.reject_me_time) {
        pressanykey("��p �z�٦� %d �����~�i�W�u",
                    (cuser.reject_me_time - now) / 60);
RESET_TTY:
        reset_tty();
        exit(1);
      }
    }
  }
  cuser.reject_me_time = 0;
}
#endif
