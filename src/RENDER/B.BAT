@echo off
tasm %1 /m3
call buildlib
dlink %1 w:\vectors\v01.obj ,,,W:\DEBUG W:\GENERAL\SYS W:\GFX\GFX W:\VESA\VBE w:\render\render.lib
pause
%1
