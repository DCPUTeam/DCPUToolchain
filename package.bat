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

rem Clean out any existing files
del /Q *.dll > nul 2> nul
del /Q *.exe > nul 2> nul
del /Q *.png > nul 2> nul
del /Q examples\*.dcpu16 > nul 2> nul
del /Q examples\*.dasm16 > nul 2> nul

rem Stop if the configuration name doesn't exist
if not exist ..\%ConfigurationName% goto noConfiguration

rem Copy all library and executable files
xcopy /Y ..\%ConfigurationName%\*.dll . > nul
xcopy /Y ..\%ConfigurationName%\*.exe . > nul
xcopy /Y ..\%ConfigurationName%\terminal.png . > nul

rem Copy all example files
xcopy /Y ..\emulator\*.dcpu16 .\examples > nul
xcopy /Y ..\assembler\*.dasm16 .\examples > nul

rem Change directory back again
popd

rem Complete!
echo Complete!
goto complete

:noConfiguration
echo No such configuration (cleaned dist).

:complete