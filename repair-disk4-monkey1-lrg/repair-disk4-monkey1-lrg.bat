:: SPDX-License-Identifier: MIT
:: 
:: Copyright (c) 2022 Donovan Watteau
:: 
:: Permission is hereby granted, free of charge, to any person obtaining a copy
:: of this software and associated documentation files (the "Software"), to deal
:: in the Software without restriction, including without limitation the rights
:: to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
:: copies of the Software, and to permit persons to whom the Software is
:: furnished to do so, subject to the following conditions:
:: 
:: The above copyright notice and this permission notice shall be included in all
:: copies or substantial portions of the Software.
:: 
:: THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
:: IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
:: FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
:: AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
:: LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
:: OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
:: SOFTWARE.

@echo off

where /q curl.exe
IF %ERRORLEVEL% EQU 0 (
	where /q tar.exe
)
IF %ERRORLEVEL% NEQ 0 (
	REM This could be a more portable PowerShell script, but default Windows policies
	REM for PowerShell are what they are...
	echo ERROR: This script only works on up-to-date Windows 10 systems
	echo.
	pause
	exit
)

IF NOT EXIST .\track00.0.raw (
	echo ERROR: No track00.0.raw file was found in the current directory.
	echo Copy the full content of the mi1_dos_525_disk4/ directory next to this script.
	echo.
	pause
	exit
)

IF NOT EXIST "%temp%\kryoflux.zip" (
	echo Downloading the DTC program from KryoFlux... [private, non-commercial use only]
	curl -s -o "%temp%\kryoflux.zip" "https://www.kryoflux.com/download/kryoflux_3.00_windows.zip"
)

IF NOT EXIST "%temp%\kryoflux\" md "%temp%\kryoflux"

IF NOT EXIST "%temp%\kryoflux\kryoflux_3.00_windows\" (
	echo Extracting the KryoFlux archive...
	tar -C "%temp%\kryoflux" -xf "%temp%\kryoflux.zip"
)

"%temp%\kryoflux\kryoflux_3.00_windows\dtc\DTC.exe" -ftrack00.0.raw -i0 -k2 -fnewdisk4.img -i4 -m1 -l1
IF %ERRORLEVEL% EQU 0 (
	echo A repaired "newdisk4.img" file has been created. Extract its content with 7-Zip.
	echo.
	pause
	exit
) ELSE (
	echo ERROR: Something went wrong when converting the files with the KryoFlux tools...
	echo.
	pause
	exit
)
