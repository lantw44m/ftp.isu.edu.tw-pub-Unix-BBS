
  ColaBBS 000309

  �@�B�s�W�P�ץ��\��:
    �аѦ� Version.txt

  �G�B�w��:
    �нƻs�Ҧ����ɮ׽ƻs�� bin �ؿ���.
    
    �b MENU.INI ���A���m�[�@�Ӱ϶�:
    
    [UserReg]
    Attrib = "���U�ɯŬݳo��" "32" "13" "2"
    FillForm = "F)���U�ӤH�ԲӸ��" "@FillForm" "Basic"
    MRegMail = "M)�H�X�����T�{�H��" "@RunPlasmid@UserReg.SendRegMail" "Basic"
    NRegNumberMail = "N)�H�X�����T�{���X" "@RunPlasmid@UserReg.SendRegNumberMail" "Basic"
    IInputRegNumber = "I)��J�����T�{���X" "@RunPlasmid@UserReg.InputRegNumber" "Basic"
    Exit =     "E)�^�W�@�h���"     "!.."       "Basic"
    
    �M����Ӧb [Info] ����
    FillForm =   "F)���U�ӤH�ԲӸ��"   "@FillForm"   "Basic"
    �令
    RUserReg = "R)���U�ɯŬݳo��" "!UserReg" "Basic"
      
    �b Plasmid.INI �s�W�@��:
    UserReg.UserReg
    
    �b ActivePlasmid.INI �s�W�T��:
    UserReg.SendRegMail
    UserReg.SendRegNumberMail
    UserReg.InputRegNumber
    
  �T�BColaSMTPd, ColaSMTPd_MBOX_REGMAIL
    �t�X�������T�{�H�ɨϥ�, �����n�s�W���� SYSOP.reg@cisnt.cis.nctu.edu.tw �o�ت��H��,
    �Цb SMTP_VirtualHost.INI �W�[�@�� reg@cisnt.cis.nctu.edu.tw = SMTP_INI\bbsreg.ini
    �M��b SMTP_INI �ؿ����s�W�@���ɮ� bbsreg.ini
    ���e��:
    MBOX = ColaSMTPd_MBOX_REGMAIL
    
                                                               WilliamWey 2000/03/09
                                                             is85012@cis.nctu.edu.tw
