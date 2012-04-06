@echo off
echo ==== Copying RAM images for debugging...
xcopy /Y dist\*.dasm16 ..\%1
echo ==== Setting up distribution...
if not exist ..\%1\dist mkdir ..\%1\dist
if not exist ..\%1\dist\assembler mkdir ..\%1\dist\assembler
if not exist ..\%1\dist\assembler\examples mkdir ..\%1\dist\assembler\examples
echo ==== Copying RAM images and assembly for distribution...
xcopy /Y ..\emulator\dist\*.dcpu16 ..\%1\dist\assembler\examples
xcopy /Y dist\*.dasm16 ..\%1\dist\assembler\examples
echo ==== Copying libraries for distribution...
xcopy /Y ..\%1\argtable2.dll ..\%1\dist\assembler
xcopy /Y ..\%1\SDL.dll ..\%1\dist\assembler
xcopy /Y ..\%1\libtcod-VS.dll ..\%1\dist\assembler
xcopy /Y ..\%1\zlib1.dll ..\%1\dist\assembler
xcopy /Y ..\%1\terminal.png ..\%1\dist\assembler
echo ==== Copying executables for distribution...
xcopy /Y ..\%1\assembler.exe ..\%1\dist\assembler
xcopy /Y ..\%1\emulator.exe ..\%1\dist\assembler