@echo off

: clean up previous builds
: ~~~~~~~~~~~~~~~~~~~~~~~~
if exist Main.obj del Main.obj
if exist Main.exe del Main.exe

: test if both modules build OK
: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
\masm32\bin\ml /c /coff Main.asm
if errorlevel 1 goto errasm1

: link all modules, note that main module is first.
: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
\masm32\bin\Link /SUBSYSTEM:WINDOWS Main.obj LoadFile.obj rsrc.obj
if errorlevel 1 goto LinkErr
goto ShowFiles

:errasm1
echo Incomplete build, assembly error
goto TheEnd

:LinkErr
echo Linking error
goto TheEnd

:ShowFiles
dir Main.*

:TheEnd













