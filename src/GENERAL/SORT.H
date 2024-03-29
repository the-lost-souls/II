/*
旼컴컴컴컴컴컴컴컴컴컴컴컴 WATCOM C++ INCLUDEFILE 컴컴컴컴컴컴컴컴컴컴컴컴컴
쿙ame         : SORT.H
쿌uthor       : Cyberfish/TLS & Toe/DU
쿗ast update  : 10.02.1998
쿌ction       : External definitions for sorting-routines.
�
쿙otes :
읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴*/

#ifndef SORT
#define SORT

extern "C" 	void ASM_CALL	SRT_RReset(void);		//Routine
extern "C" 	void ASM_CALL 	SRT_RSort(void);		//Routine
extern "C" 	void ASM_CALL  	SRT_RRun(void);		//Routine

extern "C" 	int 		SRT_OReset;     		//Object
extern "C" 	int 		SRT_OSort;      		//Object
extern "C" 	int 		SRT_ORun;       		//Object


#endif
