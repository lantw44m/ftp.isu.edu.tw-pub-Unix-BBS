
  ColaBBS 991110

  �@�B�s�W�P�ץ��\��:
    �аѦ� Version.txt

  �G�B�w��:
    �нƻs�Ҧ����ɮ׽ƻs�� bin �ؿ���.
    
    �M��b Plasmid.INI ���[�J���:
    LoginCounterBar
    ExpireAccount
    
    �Цb ActivePlasmid.INI ���[�J�|��:
	ReloadINIIPAccess
	ReloadINILoginCounterBar
	LoginCounterBar
	SendInternetMail
	
    �b MENU.INI ���A���m�[�@��
    ISendInternetMail = "I)�H Internet �H��" "@RunPlasmid@SendInternetMail"    "Post"
  
    �٦�
	ReloadINILoginCounterBar = "R)���s���J LoginCounterBar �]�w"	"@RunPlasmid@ReloadINILoginCounterBar"	"EditINI"
	LoginCounterBarToFile = "L)�s�@�Y�� LoginCounterBar �ɮ�" "@RunPlasmid@LoginCounterBar"	"EditINI"
    
	�}�ӷs�Q�װ�, ���W�� BBSRecord
      21��BBSRecord          [�t��]     CISNT BBS ���Q��P����     SYSOP	

    �]�w LoginCounterBar.INI
	Enabled=1					�O�_�Ұʲέp�H���\��
	Block=5						�έp�H���C��϶��N����
	FileEnabled=1				�O�_�ҰʱN�H���O����Q�װϪ��\��
	FileBlock=5					�g��Q�װϮɨC��϶��N����
	FileBoard=BBSRecord			�g����ӰQ�װ�
	FileAuthor=SYSOP			�έ��ӱb���o��
	FileTitle=�C��W���H���έp	���D��?
 
  �T�BColaSMTPd �� ColaSMTPd_MBOX_BOARD
    �аѦ� SMTP_INI ���� board.ini �@�]�w
    �b SMTP_VirtualHost.INI ���[�J�U�C�]�w
    board@cisnt.cis.nctu.edu.tw = SMTP_INI\board.ini
    �M��]�w�� board.ini ��,
    �H�� SYSOP.board@cisnt.cis.nctu.edu.tw ���H��N�|���� post ��Q�װϤ�.
    �i�H�t�X�q�\ maillist �ϥ�.
  
 
                                                               WilliamWey 1999/11/10
                                                             is85012@cis.nctu.edu.tw
