@echo off
call "%VS80COMNTOOLS%\vsvars32.bat"
devenv scripts\windows\vc8\vc8.sln /Rebuild "Ansi Debug|Win32"
devenv scripts\windows\vc8\vc8.sln /Rebuild "Ansi Release|Win32"
devenv scripts\windows\vc8\vc8.sln /Rebuild "Unicode Debug|Win32"
devenv scripts\windows\vc8\vc8.sln /Rebuild "Unicode Release|Win32"

devenv scripts\windows\vc8\vc8.sln /Rebuild "Ansi Debug|x64"
devenv scripts\windows\vc8\vc8.sln /Rebuild "Ansi Release|x64"
devenv scripts\windows\vc8\vc8.sln /Rebuild "Unicode Debug|x64"
devenv scripts\windows\vc8\vc8.sln /Rebuild "Unicode Release|x64"