@echo off
pushd %~dp0

setlocal EnableDelayedExpansion

for /L %%i in (0, 1, %1) do (
     set "formattedValue=00000000%%i"
     set t=!formattedValue:~-8!
	 c:\Downloads\ImageMagick-7.0.7-4-portable-Q16-x64\convert.exe -quiet -metric AE debugold!t!.bmp debug!t!.bmp -compare -format "%%[distortion]" info:
	 echo          !t!
)

popd
