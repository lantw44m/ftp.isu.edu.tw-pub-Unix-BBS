
  ColaBBS 000402

  �@�B�s�W�P�ץ��\��:
    �аѦ� Version.txt

  �G�B�w��:
    �нƻs�Ҧ����ɮ׽ƻs�� bin �ؿ���.
    
    �b ActivePlasmid.INI �s�W���:
    Games.WinLinez.WinLinez
    BoardManager.BoardManager
    BoardManager.NewBoard
    BoardManager.EditBoard
    BoardManager.DelBoard
    
    �b MENU.INI ���A���m�[�J:
    Games = "G)�u�W�C�ֳ�" "!Games" "Page"

    [Games]
    Attrib = "�u�W�C�ֳ�" "32" "13" "2"
    WinLinez = "1)�Ʋy�y" "@RunPlasmid@Games.WinLinez.WinLinez" "Page"
    Exit =     "E)�^�W�@�h���"     "!.."       "Basic"
    
    �Эק� MENU.INI ���� BoardAdmin �Ϭq:
    [BoardAdmin]
    Attrib = "���Ⱥ޲z�\\���" "20" "12" "-1"
    NewBoard = "M)�Q�װϺ޲z��" "@RunPlasmid@BoardManager.BoardManager" "OBoards"
    NewBoard = "N)�}�ҷs�Q�װ�" "@RunPlasmid@BoardManager.NewBoard" "OBoards"
    ChangeBoard = "C)�ק�Q�װϳ]�w" "@RunPlasmid@BoardManager.EditBoard" "OBoards"
    BoardDelete = "B)�R���Q�װ�" "@RunPlasmid@BoardManager.DelBoard" "OBoards"
    Exit = "E)�^�D���" "!.." "OBoards"
    
  �T�BBoardManager
    �޲z�Q�װϪ��u��, �䤤���Ѫ� NewBoard, EditBoard, DelBoard,
    �u���F���Ѹ򤧫e���Ѫ��\��ۦP���ϥΤ�k,
    �p�G���ݭn�ϥ�, �i�H���[�� ActivePlasmid.INI ��,
    �N���|�Y���B�~���O����F!
  
  �|�B����
    �w�g�Q�ڲ��r�F�@�~�F..
    
                                                             WilliamWey 2000/04/02
                                                           is85012@cis.nctu.edu.tw
