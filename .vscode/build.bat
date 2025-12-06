@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
cl.exe /Zi /EHsc /nologo /Fe"%1\%2.exe" "%3" ws2_32.lib

