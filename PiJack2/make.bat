rem @echo off

if "%1" == "" goto MISSINGTARGET
if "%1" == "dump" goto DUMP
if "%1" == "clean" goto CLEAN
set RPI=%1

set OOB= asm.o exceptionstub.o synchronize.o mmu.o pigfx.o uart.o irq.o utils.o gpio.o mbox.o prop.o board.o actled.o framebuffer.o console.o gfx.o dma.o nmalloc.o ee_printf.o stupid_timer.o block.o emmc.o c_utils.o mbr.o fat.o config.o ini.o binary_assets.o C64UserPort24Bit.o DisplayBombJack.o binaryData.o smp.o

set CFLAGS= -Wall -Wextra -O2 -g -nostdlib -nostartfiles -fno-stack-limit -ffreestanding -fsigned-char
set CPPFLAGS= %CFLAGS% -fno-exceptions -fno-rtti
if "%RPI%"=="1" (
  set CFLAGS= %CFLAGS% -march=armv6j -mtune=arm1176jzf-s -DRPI=1
)
if "%RPI%"=="2" (
  set CFLAGS= %CFLAGS% -march=armv7-a -mtune=cortex-a7 -DRPI=2
)
if "%RPI%"=="3" (
  set CFLAGS= %CFLAGS% -march=armv8-a -mtune=cortex-a53 -DRPI=3
)
if "%RPI%"=="4" (
  set CFLAGS= %CFLAGS% -march=armv8-a -mtune=cortex-a53 -DRPI=4
)

echo Compiling for Pi generation %RPI%

::Get Repository Version
md build 2>NUL
set GIT_DESCRIBE=1234567890
set BUILD_VERSION=1234567890

:: Where's libgcc?
arm-none-eabi-gcc -print-libgcc-file-name -> build\tmp.txt
set /p LIBGCC=<build\tmp.txt
del build\tmp.txt

:: Build pigfx_config.h
setlocal DisableDelayedExpansion
set "search=$VERSION$"
set "replace=%BUILD_VERSION%"
del src\pigfx_config.h 2>NUL
for /F "delims=" %%a in (pigfx_config.h.in) DO (
  set line=%%a
  setlocal EnableDelayedExpansion
  >> src\pigfx_config.h echo(!line:%search%=%replace%!
  endlocal
)

:: Compiling c files
del build\*.* /Q 2>NUL
del *.elf /Q 2>NUL
:: Compiling s files
for %%s in (src/*.s) do arm-none-eabi-gcc src\%%s %CFLAGS% -c -o build\%%~ns.o
for %%c in (src/*.c) do arm-none-eabi-gcc src\%%c %CFLAGS% -c -o build\%%~nc.o
for %%c in (src/*.cpp) do arm-none-eabi-gcc src\%%c %CPPFLAGS% -c -o build\%%~nc.o

:: Converting binary (bitmap or audio) data to obj file
:: https://balau82.wordpress.com/2012/02/19/linking-a-binary-blob-with-gcc/
:: arm-none-eabi-objdump -t build\binaryData.o
:: _binary_binaryData_bin_start _binary_binaryData_bin_end _binary_binaryData_bin_size
copy /Y "C:\Work\C64\AnimationBitmap\Data\binaryData.bin" binaryData.bin
arm-none-eabi-objcopy -I binary -O elf32-littlearm -B armv6 binaryData.bin build\binaryData.o
:: _binary_smp_raw_start _binary_smp_raw_end _binary_smp_raw_size
copy /Y "C:\Work\C64\AnimationBitmap\Data\smp.raw" smp.raw
arm-none-eabi-objcopy -I binary -O elf32-littlearm -B armv6 smp.raw build\smp.o

:: linking files
for %%A in (%OOB%) do (call :addbuild %%A)
arm-none-eabi-ld %OBJS% "%LIBGCC%" -T memmap -o pigfx.elf
:: generate .img
arm-none-eabi-objcopy pigfx.elf -O binary pigfx.img

:: Copy img file
if "%RPI%"=="1" copy pigfx.img bin\kernel.img
if "%RPI%"=="2" copy pigfx.img bin\kernel7.img
if "%RPI%"=="3" copy pigfx.img bin\kernel8-32.img
if "%RPI%"=="4" copy pigfx.img bin\recovery7l.img

goto end

:: Add build\ before every o file
:addbuild
set OBJS= %OBJS% build\%1
goto END

:: Create dump file
:DUMP
arm-none-eabi-objdump --disassemble-zeroes -D pigfx.elf > pigfx.dump
goto end

:: Cleanup files
:CLEAN
del src\pigfx_config.h /Q 2>NUL
del build\*.* /Q 2>NUL
del *.hex /Q 2>NUL
del *.elf /Q 2>NUL
del *.img /Q 2>NUL
del *.dump /Q 2>NUL
del tags /Q 2>NUL
goto end

:MISSINGTARGET
echo Define Target, e.g. make 1 for Pi 1

:END
