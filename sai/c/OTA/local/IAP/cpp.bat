@echo off
rem del ..\..\Release\Inc\DRV_Led.h /S/Q
del ..\..\Release\Lib\%2 /S/Q

xcopy ..\Inc\*.h ..\..\Release\Inc /S/Y
xcopy %1 ..\..\Release\Lib /Y
del %1
@echo on