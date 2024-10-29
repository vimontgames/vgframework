@echo off

rem Change to the directory where your header should be generated
cd /d "src"

rem Get the current short Git revision
for /f "delims=" %%i in ('git rev-parse --short HEAD') do set GIT_REVISION=%%i

rem Create the version header file
echo // This file is generated automatically > version.h
echo #define GIT_REVISION "%GIT_REVISION%" >> version.h

rem Optional: print a message
echo Version header generated with GIT revision: %GIT_REVISION%