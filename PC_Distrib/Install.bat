@echo off

rem This defines the installation directory, please change it:
set VCDIR=C:\Program Files\Microsoft Visual Studio 8\VC

echo Files will be copied to the following directory:
echo %VCDIR%
echo Please verify that it's correct, or edit this batch file.
pause

echo *****************
echo * Library files *
echo *****************
copy OSLib.lib "%VCDIR%\lib"
IF ERRORLEVEL 1 GOTO ERROR

copy *.lib "%VCDIR%\lib"

echo *****************
echo * Include files *
echo *****************
mkdir "%VCDIR%\include\oslib"
copy *.h "%VCDIR%\include\oslib"

mkdir "%VCDIR%\include\oslib\GL"
copy GL\*.h "%VCDIR%\include\oslib\GL"


IF ERRORLEVEL 1 GOTO ERROR
echo Installation completed successfully.
pause
exit

:ERROR
color c
echo Installation failed. Please verify the installation path!
pause
