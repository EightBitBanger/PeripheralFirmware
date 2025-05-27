@echo off

cls

avrdude.exe -F -p m164p -c usbtiny -U flash:w:Release\peripheral.hex

echo.
echo.
echo.
echo.


choice /t:8 /d y /n

