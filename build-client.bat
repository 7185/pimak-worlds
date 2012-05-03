@echo off

rem Specify the path to your MinGW installation
set MINGW=D:\MinGW

rem Specify the path to your Qt installation
set QT=D:\QtSDK

rem Preparing environment...
set PATH=%PATH%;%QT%\mingw\bin;%QT%\Desktop\Qt\4.8.0\mingw\bin

cd client

qmake
mingw32-make release

echo 
echo Build done
pause
