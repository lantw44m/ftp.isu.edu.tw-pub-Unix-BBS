
  ColaBBS 990914

  �@�B�s�W�P�ץ��\��:
    �аѦ� Version.txt

  �G�B�w��:
    �нƻs�Ҧ����ɮ׽ƻs�� bin �ؿ���.
    �æb ColaBBS.INI ���s�W���:
    SMTPAddress=localhost
	SMTPPort=25
 
  �T�BIssueManager
    Plasmid.INI:
    �s�W�@��
    IssueManager.Load
  
	ActivePlasmid.INI:
	�s�W���
    IssueManager.Reload
    IssueManager.Setup
	
	MENU.INI �s�W:
    [SetupAdmin]
    Issue = "I)�i���e���޲z�������]�w" "!Issue" "EditINI"
    
    [Issue]
    Attrib = "�i���e���޲z�������]�w" "20" "12" "-1"
    RIM = "R)���s���J�i���e��" "@RunPlasmid@IssueManager.Reload" "EditINI"
    SIM = "S)�]�w�i���e���޲z��" "@RunPlasmid@IssueManager.Setup" "EditINI"
    Exit = "E)�^�t�γ]�w�\\���" "!SetupAdmin" "EditINI"
  
    �]�w��k�Ь� cisnt.cis.nctu.edu.tw �� ColaBBSFAQ �Q�װϪ�
    "[�ð�] �h���i���e���n���]�w�O?"
  
  �|�BPackageMail
	ActivePlasmid.INI:
	�s�W�@��
	PackageMail
	
	MENU.INI �s�W:
	PackageMail = "P)���]�H��" "@RunPlasmid@PackageMail" "Post"
	
  ���BColaSMTPd
    Plasmid.INI:
    �s�W�@��
    ColaSMTPd.ColaSMTPd
    
    �åB�� ColaBBS.INI ���� UserColaMail �]�w�� false
	UseColaMail=false
	�M��ݬ� InternetMail �O���O true
    InternetMail=true
    
    ColaSMTPd ���h�ӳ]�w��, �Ьݸӳ]�w�ɤ�������.
    ColaSMTPd.INI
    SMTP_Access.INI
    SMTP_Relay.INI
    SMTP_VirtualHost.INI    
    
    ��]�w�����D, �Ш� cisnt.cis.nctu.edu.tw �� ColaBBS �Q�װ�.
    	
                                                          WilliamWey 1999/09/14
                                                        is85012@cis.nctu.edu.tw
