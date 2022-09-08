@echo off
call "c:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\VC\Auxiliary\Build\vcvars32.bat"
devenv scripts\windows\vc15\vc15.sln /Rebuild "Ansi Debug|x86"
devenv scripts\windows\vc15\vc15.sln /Rebuild "Ansi Release|x86"
devenv scripts\windows\vc15\vc15.sln /Rebuild "Unicode Debug|x86"
devenv scripts\windows\vc15\vc15.sln /Rebuild "Unicode Release|x86"

devenv scripts\windows\vc15\vc15.sln /Rebuild "Ansi Debug|x64"
devenv scripts\windows\vc15\vc15.sln /Rebuild "Ansi Release|x64"
devenv scripts\windows\vc15\vc15.sln /Rebuild "Unicode Debug|x64"
devenv scripts\windows\vc15\vc15.sln /Rebuild "Unicode Release|x64"
