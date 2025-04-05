@echo off
pushd %~dp0
setlocal EnableDelayedExpansion

for /L %%i in (0, 1, %1) do (
     set "formattedValue=00000000%%i"
     set t=!formattedValue:~-8!
     set t2=!formattedValue:~-6!
	 if exist C:\work\BDD6502\testdata\%2-!t2!.bmp (
		 c:\Downloads\ImageMagick-7.0.7-4-portable-Q16-x64\convert.exe -quiet -metric AE debug!t!.bmp C:\work\BDD6502\testdata\%2-!t2!.bmp -compare -format "%%[distortion]" info:
		 echo          C:\work\BDD6502\testdata\%2-!t!
	 )
)
popd
