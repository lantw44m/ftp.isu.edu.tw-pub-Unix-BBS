
  ColaBBS 01040X BETA 12

  一、新增與修正功能:
    請參考 Version.txt

  二、升級:
    請將 bin 目錄下所有的檔案複製到 BBS 的 bin 目錄中.
    請將 etc 目錄下所有的檔案複製到 BBS 的 etc 目錄中.

    MENU.INI 增加 SelectBoard, ViewBoard, ViewAnnounce 三個指令,
    其中 ViewBoard 以及 ViewAnnounce 後面可以加上 @討論區名稱, 就可以直接觀看該討論區.
    
    在 [TOPMENU] 新增:
    SelectBoard = "S)選擇討論區"            "@SelectBoard" "Basic"
    ReadBoard   = "R)閱\讀討論區"           "@ViewBoard"   "Basic"

    Messages.INI 中, 修改:
    第 54 行:
 離開[[1;32m←[m,[1;32me[m] 選擇[[1;32m↑[m,[1;32m↓[m] 閱讀[[1;32m→[m,[1;32mr[m] 發表文章[[1;32mCtrl-P[m] 砍信[[1;32md[m] 備忘錄[[1;32mw[m] 求助[[1;32mh[m] \n
    第 363 行:
       離開[[1;32m←[m,[1;32me[m]  選擇[[1;32m↑[m,[1;32m↓[m]  閱讀精華[[1;32m→[m] 精華區索引[[1;32mx[m] 求助[[1;32mh[m]            [m \n

    新增 PostNotePad 這個 ActivePlasmid,
    在 AcitvePlasmid.INI 中新增:
    PostNotePad
    在 MENU.INI 中的 [SYSInfo] 新增:
    PostNotepad	= "P)張貼流涎版" "@RunPlasmid@PostNotePad" "Basic"
    


                                                                  WilliamWey 2002/04/09
                                                                is85012@cis.nctu.edu.tw
