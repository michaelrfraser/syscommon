@echo off
call "%VS140COMNTOOLS%\vsvars32.bat"
devenv scripts\windows\vc14\vc14.sln /Rebuild "Ansi Debug|x86"
devenv scripts\windows\vc14\vc14.sln /Rebuild "Ansi Release|x86"
devenv scripts\windows\vc14\vc14.sln /Rebuild "Unicode Debug|x86"
devenv scripts\windows\vc14\vc14.sln /Rebuild "Unicode Release|x86"

devenv scripts\windows\vc14\vc14.sln /Rebuild "Ansi Debug|x64"
devenv scripts\windows\vc14\vc14.sln /Rebuild "Ansi Release|x64"
devenv scripts\windows\vc14\vc14.sln /Rebuild "Unicode Debug|x64"
devenv scripts\windows\vc14\vc14.sln /Rebuild "Unicode Release|x64"