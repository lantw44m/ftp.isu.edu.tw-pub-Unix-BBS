
把 Games 目錄複製到 bin 下面,
在 ActivePlasmid.INI 中加一行:
Games.WinLinez.WinLinez

然後在 MENU.INI 中的適當位置加入下面幾行.

Games = "G)線上遊樂場" "!Games" "Page"

[Games]
Attrib = "線上遊樂場" "32" "13" "2"
WinLinez = "1)排球球" "@RunPlasmid@Games.WinLinez.WinLinez" "Page"
Exit =     "E)回上一層選單"     "!.."       "Basic"
