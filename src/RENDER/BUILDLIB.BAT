@echo off
tasm32 /m3 t01.asm
tasm32 /m3 t02.asm
tasm32 /m3 t04_org.asm
tasm32 /m3 t05_org.asm

del render.lib
tlib render.lib +t01.obj +t02.obj +t04_org.obj +t05_org.obj
