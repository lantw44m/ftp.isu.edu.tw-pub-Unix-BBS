
  ColaBBS 000122

  �@�B�s�W�P�ץ��\��:
    �аѦ� Version.txt

  �G�B�w��:
    �нƻs�Ҧ����ɮ׽ƻs�� bin �ؿ���.
    
    �Цb ActivePlasmid.INI ���[�J�T��:
    ReloadINISMTPAccess
	ReloadINISMTPRelay
	ReloadINISMTPVirtualHost
	�åB�� SysInfo
	�令 SysInfo.SysInfo
	
    �b MENU.INI ���A���m�[�@�Ӱ϶�:
	
	[ColaSMTPdAdmin]
	Attrib = "�ϥΪ̺޲z�\\���" "20" "12" "-1"
	"1EditAccess" = "1)�s�� Access.INI �ɮ�"     "@EditFile@bin\\SMTP_Access.INI"    "EditINI"
	"2EditRelay" = "2)�s�� Relay.INI �ɮ�"     "@EditFile@bin\\SMTP_Relay.INI"    "EditINI"
	"3EditVirtualHost" = "3)�s�� VirtualHost.INI �ɮ�"     "@EditFile@bin\\SMTP_VirtualHost.INI"    "EditINI"
	"1LoadAccess" = "1)���s���J Access.INI �ɮ�"     "@RunPlasmid@ReloadINISMTPAccess"    "EditINI"
	"2LoadRelay" = "2)���s���J Relay.INI �ɮ�"     "@RunPlasmid@ReloadINISMTPRelay"    "EditINI"
	"3LoadVirtualHost" = "3)���s���J VirtualHost.INI �ɮ�"     "@RunPlasmid@ReloadINISMTPVirtualHost"    "EditINI"
	Exit =        "E)�^�D���"           "!.."          "SYSOP"
	
	�åB�b�A����m�[�W�@��, �Ҧp�b [Admin] �϶���:
	MColaSMTPdAdmin = "M)ColaSMTPd �]�w�\\���"    "!ColaSMTPdAdmin"  "EditINI"
	
	���~, �ٻݭn��ʤ@�Ӧ�m:
	ISysInfo = "I)�t�θ�T"   "@RunPlasmid@SysInfo"    "Basic"
	�Ч令
	ISysInfo = "I)�t�θ�T"   "@RunPlasmid@SysInfo.SysInfo"    "Basic"
	
  �T�BSysInfo
    �w�ˮɽЪ`�N�n�N PDH.dll �ƻs��P ColaBBS.EXE �P�ؿ��U.
    ����������T�u�b Windows NT �U�����@��, �Ҧp�t�έt����.
    ���~, �b Windows 9x �U, �]���t�έ���, �ҥH�̱`�}���ɶ��� 49 �Ѧh�@�I.
 
                                                               WilliamWey 2000/01/22
                                                             is85012@cis.nctu.edu.tw
