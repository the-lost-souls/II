/*
�������������������������� WATCOM C++ INCLUDEFILE ��������������������������
�Name         : MAIN.H
�Author       : Cyberfish/TLS & Toe/DU
�Last update  : 22.12.97
�Action       : External definitions for main.asm
�
�Notes :
����������������������������������������������������������������������������*/

#include "w:\general\var.h"

#ifndef MAIN
#define MAIN

extern "C" 	void ASM_CALL  	MN_MainLoop(void); 	//Main-looproutine
extern "C" 	void ASM_CALL 	KEY_RPressed(void);	//Keypressed routine
extern "C" 	int  		KEY_OPressed;       	//Keypressed object
extern "C" 	void ASM_CALL 	BRK_RFrames(void); 	//Keypressed routine
extern "C" 	int  		BRK_OFrames;	    	//Keypressed object

#endif