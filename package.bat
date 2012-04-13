@echo off

rem Prompt for the mode that we want to package
echo | set /p=Enter configuration name: 
set /p ConfigurationName=

rem Change directory
pushd .
cd %~dp0
if not exist dist mkdir dist
:waitForDirectory
if not exist dist goto waitForDirectory
cd dist

rem Create the example directory if needed
if not exist examples mkdir examples
if not exist include mkdir include

rem Clean out any existing files
del /Q *.dll > nul 2> nul
del /Q *.exe > nul 2> nul
del /Q *.png > nul 2> nul
del /Q *.m > nul 2> nul
del /Q *.asm > nul 2> nul
del /Q examples\*.dcpu16 > nul 2> nul
del /Q examples\*.dasm16 > nul 2> nul
del /Q examples\*.c > nul 2> nul

rem Stop if the configuration name doesn't exist
if not exist ..\%ConfigurationName% goto noConfiguration

rem Copy all library and executable files
xcopy /Y ..\%ConfigurationName%\*.dll . > nul
xcopy /Y ..\%ConfigurationName%\*.exe . > nul
xcopy /Y ..\%ConfigurationName%\terminal.png . > nul
xcopy /Y ..\compiler\dist\*.c .\examples > nul
xcopy /Y ..\compiler\dist\*.m .\include > nul
xcopy /Y ..\compiler\bootstrap.asm .\ > nul
xcopy /Y ..\*.dll . > nul

rem Change directory back again
popd

rem Complete!
echo Complete!
goto complete

:noConfiguration
echo No such configuration (cleaned dist).

:complete