
  ColaBBS 01040X BETA 11

  一、新增與修正功能:
    請參考 Version.txt

  二、升級:
    請將 bin 目錄下所有的檔案複製到 BBS 的 bin 目錄中.
    請將 etc 目錄下所有的檔案複製到 BBS 的 etc 目錄中.
    
    Messages.INI 新增一行訊息:
[1;35m─[40m [33m討論區簡介 [35m─────────────────────────────────[m    
    
    MENU.INI
    請將
    OtersAdmin = "O)其他設定功\能表"   "!OthersAdmin"                      "EditINI"
    改為
    OthersAdmin = "O)其他設定功\能表"   "!OthersAdmin"                      "EditINI"
    
    在 [UserAdmin] 新增:
    LSpecialPermissionUsers = "L)建立特殊權限帳號列表" "@RunPlasmid@SpecialPermissionUsers" "Account"
    
    ActivePlasmid.INI 新增:
    SpecialPermissionUsers
    
    建立兩個新討論區:
    BBSReport, 權限請設定為 "讀取 ...........O..O................." (限定系統維護管理員讀取)
    UserReg, 權限請設定為 "讀取 ...........O..O................." (限定帳號管理員, 系統維護管理員讀取)
    
  三、支援 HFNetChk 的外掛功能
    
    HFNetChk 是 Microsoft 用來檢查 Windows NT 4.0, 2000, XP, .NET Server 是否安裝新的 hotfix 的程式.
    這個外掛功能可以提供自動/手動執行 HFNetChk 並且將檢查結果輸出到某一討論區..
    
    安裝方法:
    
    下載 HFNetChk, 在
    http://www.microsoft.com/downloads/release.asp?releaseid=31154&area=featured&ordinal=2
    並將檔案解壓縮到 bin 目錄, 或是包含在 PATH 環境變數的路徑下..
    (例如 \winnt 目錄, \windows 目錄..)
    
    在 Plasmid.INI 新增:
    HFNetChk
    
    在 ActivePlasmid.INI 新增:
    HFNetChk
    
    在 [OthersAdmin] 新增:
    HFNetChk       = "H)執行 HFNetChk" "@RunPlasmid@HFNetChk@CHECK" "SYSOP"
    FHFNetChkSetup = "F)HFNetChk 設定" "@RunPlasmid@HFNetChk@SETUP" "SYSOP"    
  
    本功能只能適用在 Windows NT 4.0, 2000, XP, .NET Server 作業系統,
    並不支援 Windows 95, 98, ME.
    
    如果有使用 ColaService, 並且使用其他帳號執行 BBS, 必須幫該帳號加入 Administrators 的群組中.
    
    並請注意 Microsoft 的各項 hotfix, 以確保主機安全.
    
  四、其他
    新增下面兩個設定:
    REG.BOARDLIST.INFOMODE = true|false    是否讓討論區列表, 預設為開啟說明模式. 預設值為 false.

                                                                                     WilliamWey 2002/03/07
                                                                                   is85012@cis.nctu.edu.tw
