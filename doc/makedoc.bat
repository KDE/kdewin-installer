@echo off
:: 
::

if exist "%ProgramFiles%\doxygen\bin\doxygen.exe" (
	%ProgramFiles%\doxygen\bin\doxygen installer.doxy
) else (
	echo doxygen not found
)

