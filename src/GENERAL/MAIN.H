/*
旼컴컴컴컴컴컴컴컴컴컴컴컴 WATCOM C++ INCLUDEFILE 컴컴컴컴컴컴컴컴컴컴컴컴컴
쿙ame         : MAIN.H
쿌uthor       : Cyberfish/TLS & Toe/DU
쿗ast update  : 22.12.97
쿌ction       : External definitions for main.asm
�
쿙otes :
읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴*/

#include "w:\general\var.h"

#ifndef MAIN
#define MAIN

extern "C" 	void ASM_CALL  	MN_MainLoop(void); 	//Main-looproutine
extern "C" 	void ASM_CALL 	KEY_RPressed(void);	//Keypressed routine
extern "C" 	int  		KEY_OPressed;       	//Keypressed object
extern "C" 	void ASM_CALL 	BRK_RFrames(void); 	//Keypressed routine
extern "C" 	int  		BRK_OFrames;	    	//Keypressed object

#endif
