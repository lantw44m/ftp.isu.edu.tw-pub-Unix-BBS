
  ColaBBS 000303

  �@�B�s�W�P�ץ��\��:
    �аѦ� Version.txt

  �G�B�w��:
    �нƻs�Ҧ����ɮ׽ƻs�� bin �ؿ���.
    
    �b MENU.INI ���A���m�[�@�Ӱ϶�:
    
    [BoardExpire]
    Attrib = "BoardExpire �����]�w" "20" "12" "-1"
    NBoardExpireNow = "N)�Ұ� Expire" "@RunPlasmid@BoardExpire.BoardExpireNow" "EditINI"
    CBoardExpireControlPanel = "P)BoardExpire �޲z��" "@RunPlasmid@BoardExpire.BoardExpireControlPanel" "EditINI"
    Exit = "E)�^�t�γ]�w�\\���" "!SetupAdmin" "EditINI"
    
    �M����Ӧb [SetupAdmin] ����
    BoardExpireNow = "N)�Ұ� Expire" "@RunPlasmid@BoardExpire.BoardExpireNow" "EditINI"
    �令
    BoardExpire = "B)BoardExpire �����]�w" "!BoardExpire" "EditINI"
  
    �M��b [SetupAdmin] ���s�W�@�q:
    NReloadNLANTable = "N)���s���J NLAN �]�w" "@RunPlasmid@ReloadNLANTable" "EditINI"
    TEditNLANTable = "T)�s�� NLAN �]�w" "@EditFile@bin\\NLAN.INI" "EditINI"
    
    �b [UserAdmin] ���s�W�@��:
    EMailToAllUsers = "E)�H�H���Ҧ����ϥΪ�" "@RunPlasmid@MailToAllUsers" "Account"
    
    �b Plasmid.INI �s�W�@��:
    LoadNLANTable
    
    �b ActivePlasmid.INI �s�W�T��:
    MailToAllUsers
    ReloadNLANTable
    BoardExpire.BoardExpireControlPanel
     
                                                               WilliamWey 2000/03/03
                                                             is85012@cis.nctu.edu.tw
