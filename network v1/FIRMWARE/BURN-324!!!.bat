@echo off

cls

avrdude.exe -V -p m324pa -c usbtiny -U flash:w:firmware\Release\firmware.hex

echo.
echo.
echo.
echo.


choice /t:1 /d y /n

