/*-------------------------------------------------------*/
/* maple/agree.h              (SOB 0.22 ATS Verion 1.31) */
/*-------------------------------------------------------*/
/* target : �s�p�t�ε{�������Y�� �`�� �@�Ψ禡�w�q       */
/* create : 02/09/09 by Dopin                            */
/* update : 02/09/10                                     */
/*-------------------------------------------------------*/
/* syntax :                                              */
/*-------------------------------------------------------*/

#define DIR BBSHOME "/agree"        /* �u�@�ؿ� */
#define TARGET DIR "/agree.ags"     /* �O���ɩҦb������| */
#define LOCKFILE DIR "/lock"        /* �t����w�� */
#define TEMPFILE DIR "/temp"        /* �t�μȦs�� */
#define BACKUP   DIR "/backup.ags"  /* �t�γƥ��� */

#define MINLIMIT "10"  /* �t�ιw�]�̤p�s�p���e�H�� (���r�� "1" ~ "65535") */
#define MAXAGREES 128  /* �̰��ФŶW�L 254 ����� �p���X�R �Ч������ܼ� */

#define POSTBOARD "SYSOP"                   /* �w�]���i���ݪO�W */
#define POSTTO BBSHOME "/boards/" POSTBOARD /* �w�]���i���ݪO������| */

char *err_msg[] = { "�ثe�L����t�γs�p", "�L�����s�p���", "��", "��ƿ��~" };
char *action[] = { "�ק�", "�R��", "�}��s�p", "����s�p", "���i" };
char *limit[] = {
"�q�L�{��", "�H�c�L�W��", "�O�d�b��", "�O�D�v��", "���ȤH��", "����", "�L����"
};

struct ags {
   char   flag;
   char   title[50];
   char   fpath[70];
   char   author[IDLEN+1];
   char   start[6];
   char   end[6];
   char   postboard[(IDLEN+1)*2];
   ushort count;
   ushort minlimit;
   char anonymous;
   usint  perm;
};
typedef struct ags ags;

ags work[MAXAGREES];

void set_agree_file(char *buf, uschar number) {
   sprintf(buf, DIR "/%d.agree", number);
}

int check_agree(ags *check) {
   int i;
   char month, day, data_m, data_d;

   if(!check->flag) return 0;

   get_tm_time(NULL, &month, &day, NULL, NULL);
   if(!month || !day) return 0;

   i = check_mmdd(check->start, month, day);
   if(i == -2) return 0;
   if(i == -1) return 0;

   if(!strcmp(check->end, "--/--")) return 1;
   i = check_mmdd(check->end, month, day);
   if(i == -2) return 0;
   if(i == 1) return 0;

   return 1;
}

int perm_acs(int mode, usint *perm, char key_v) {
   if(!mode) {
      if(!perm) return 1;
      if(HAS_PERM(*perm)) return 1;
      else return 0;
   }

   if(mode == 1) {
      if(*perm < PERM_LOGINOK) return 7;
      if(*perm & PERM_LOGINOK) return 1;
      if(*perm & PERM_MAILLIMIT) return 2;
      if(*perm & PERM_XEMPT) return 3;
      if(*perm & PERM_BM) return 4;
      if(*perm & PERM_ACCOUNTS || *perm & PERM_BOARD) return 5;
      if(*perm & PERM_SYSOP) return 6;
   }

   if(mode == 2) {
      if(key_v == '1') *perm = PERM_LOGINOK;
      if(key_v == '2') *perm = PERM_MAILLIMIT;
      if(key_v == '3') *perm = PERM_XEMPT;
      if(key_v == '4') *perm = PERM_BM;
      if(key_v == '5') *perm = PERM_ACCOUNTS || PERM_BOARD;
      if(key_v == '6') *perm = PERM_SYSOP;
      if(key_v == '7') *perm = 0;
   }
}
