copy release\oslib.lib ..\PC_Distrib
copy "..\oslib\*.h" "..\PC_Distrib\"
cd ..\PC_Distrib
Install.bat
pause
