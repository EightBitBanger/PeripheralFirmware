@echo off

cls

avrdude.exe -F -p m324pa -c usbtiny -U lfuse:w:0xce:m -U hfuse:w:0xd9:m -U efuse:w:0xfc:m

echo.
echo.
echo.
echo.

choice /t:15 /d y /n
