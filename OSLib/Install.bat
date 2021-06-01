@echo off

rem This defines the installation directory, please change it:
set PSPSDKDIR=C:\devkitPro\devkitPSP\psp\sdk

echo Files will be copied to the following directory:
echo %PSPSDKDIR%
echo Please verify that it's correct, or edit this batch file.
pause

echo *****************
echo * Archive files *
echo *****************
copy libosl.a %PSPSDKDIR%\lib
rem copy ..\OSTools\libost.a %PSPSDKDIR%\lib
IF ERRORLEVEL 1 GOTO ERROR

copy libz.a %PSPSDKDIR%\lib
copy libpng.a %PSPSDKDIR%\lib
copy lib\libmikmod.a %PSPSDKDIR%\lib

echo *****************
echo * Include files *
echo *****************
mkdir %PSPSDKDIR%\include\oslib
copy audio.h %PSPSDKDIR%\include\oslib
copy bgm.h %PSPSDKDIR%\include\oslib
copy drawing.h %PSPSDKDIR%\include\oslib
copy keys.h %PSPSDKDIR%\include\oslib
copy map.h %PSPSDKDIR%\include\oslib
copy messagebox.h %PSPSDKDIR%\include\oslib
copy oslib.h %PSPSDKDIR%\include\oslib
copy text.h %PSPSDKDIR%\include\oslib
copy vfpu.h %PSPSDKDIR%\include\oslib
copy VirtualFile.h %PSPSDKDIR%\include\oslib
copy vram_mgr.h %PSPSDKDIR%\include\oslib
rem copy usb.h %PSPSDKDIR%\include\oslib

IF ERRORLEVEL 1 GOTO ERROR
echo Installation completed successfully.
pause
exit

:ERROR
color c
echo Installation failed. Please verify the installation path!
pause
