rem Ton répertoire devkitpsp en fait
set path=c:\devkitPro\devkitPSP\bin;c:\devkitPro\msys\bin;%path%

rem Lance la compilation
make

rem Copie vers ta PSP
copy EBOOT.PBP G:\PSP\GAME\OSLib_test
