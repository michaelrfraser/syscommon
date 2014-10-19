@echo off
call "%VS100COMNTOOLS%\vsvars32.bat"
devenv scripts\windows\vc10\vc10.sln /Rebuild "Ansi Debug|Win32"
devenv scripts\windows\vc10\vc10.sln /Rebuild "Ansi Release|Win32"
devenv scripts\windows\vc10\vc10.sln /Rebuild "Unicode Debug|Win32"
devenv scripts\windows\vc10\vc10.sln /Rebuild "Unicode Release|Win32"

devenv scripts\windows\vc10\vc10.sln /Rebuild "Ansi Debug|x64"
devenv scripts\windows\vc10\vc10.sln /Rebuild "Ansi Release|x64"
devenv scripts\windows\vc10\vc10.sln /Rebuild "Unicode Debug|x64"
devenv scripts\windows\vc10\vc10.sln /Rebuild "Unicode Release|x64"