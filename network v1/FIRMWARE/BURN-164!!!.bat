@echo off

cls

\CodeBlocks\avr-gcc\bin\avrdude.exe -V -p m164a -c usbtiny -U flash:w:firmware\Release\firmware.hex

echo.
echo.
echo.
echo.


choice /t:1 /d y /n

pause