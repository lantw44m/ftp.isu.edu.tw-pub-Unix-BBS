
  ColaBBS 01040X BETA 8

  �@�B�s�W�P�ץ��\��:
    �аѦ� Version.txt

  �G�B�ɯ�:
    �бN bin �ؿ��U�Ҧ����ɮ׽ƻs�� BBS �� bin �ؿ���.
    �бN etc �ؿ��U�Ҧ����ɮ׽ƻs�� BBS �� etc �ؿ���.

    �b ActivePlasmid.INI ���s�W���:
    FromHostChecker
    SendMailToSYSOPs
    
    �b MENU.INI ���s�W:
    �b [Mail] �U�s�W:
    OMailToSYSOPs = "O)�H�H��������" "@RunPlasmid@SendMailToSYSOPs" "Basic"
    
    �b [Info] �U�s�W:
    HostChecker = "H)�]�w�ӷ�����" "@RunPlasmid@FromHostChecker@Setup" "EditINI"
    FLoginFailed = "F)��ܵn�J���~����" "@RunPlasmid@ShowLoginFailed" "Basic"
    
    �b [Admin] �U�s�W:
    PLoginFailed = "P)��� SYSOP �n�J���~����" "@RunPlasmid@ShowLoginFailed@VIEW@SYSOP" "EditINI"
    
    �b [UserAdmin] �U�s�W:
    FLoginFailed = "F)��ܨϥΪ̵n�J���~����" "@RunPlasmid@ShowLoginFailed@VIEW" "Account"
    
    �b [EditINI] �U�s�W:
    BadIDList = "B)�s�� BadIDList.INI �ɮ�" "@EditFile@bin\\BadIDList.INI" "EditINI"
    "4LoadBadIDList" = "4)���s���J BadIDList.INI �ɮ�" "@RunPlasmid@BBSTools@ReloadBadIDList" "EditINI"
    
    �}�� [�n�J����޲z��] �s�W�@�Ӷ���:
    �N��:              �n�J�ӷ��ˬd
    �Ұʪ��A:          �}��
    �R�O:              RunPlasmid
    �Ѽ�:              FromHostChecker
    �v���Ҧ�:          1 ���䤤���@�h�Ұ�
    �v���]�w:          ..............O.................
    ²��:              ����ۤv�b�����s�u�ӷ�
    ����:              �i�H�]�w�ۤv���b���u��q���Ǧa��s�u
    �ϥΪ�����:        0 �i�H����
    �M��N�L�h������u���Ұ�.
    
  �T�B��L
    ColaBBS.INI ���s�W�U�C�]�w:
    disableSYSOP = true/false        �O�_���� SYSOP �b��, �w�]�� false
    BMTools.defTitleBegin = number   ���媩�W�b����쪺���_�l��m, �Ц������̫e���� group �����r��
                  �Ҧp: "0[����]     ���ʬݪO�s�i��", "��" �o�Ӧr������ "0" �H��O�b�� 12 �Ӧ�m�W.
                  �w�]�� 12
    
                                                                                  WilliamWey 2001/08/28
                                                                                is85012@cis.nctu.edu.tw
