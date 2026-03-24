@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
if %ERRORLEVEL% neq 0 exit /b %ERRORLEVEL%
cl.exe /Zi /EHsc /nologo /I"d:\alabuga\include" /Fe:"d:\alabuga\main.exe" d:\alabuga\src\*.cpp
exit /b %ERRORLEVEL%
