@echo off
tasm32 /m3 t01.asm
tasm32 /m3 t02_org.asm
tasm32 /m3 t04_org.asm

del render.lib
tlib render.lib +t01.obj +t02_org.obj +t04_org.obj
