for %%I in (.) do set CurrProject=%%~nxI

copy /y "%CD%\%CurrProject%.json" "%userprofile%\Desktop\Workspace\%CurrProject%\SKSE\Plugins\%CurrProject%.json"
copy /y "%CD%\%CurrProject%.json" "D:\Program Files (x86)\Steam\steamapps\common\Skyrim Special Edition\MO2\mods\%CurrProject%\SKSE\Plugins\%CurrProject%.json"