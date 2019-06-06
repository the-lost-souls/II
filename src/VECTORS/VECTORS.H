/*
旼컴컴컴컴컴컴컴컴컴컴컴컴 WATCOM C++ INCLUDEFILE 컴컴컴컴컴컴컴컴컴컴컴컴컴
쿙ame         : VECTORS.H
쿌uthor       : Cyberfish/TLS & Toe/DU
쿗ast update  : 08.02.1998
쿌ction       : Definitions for all vectorroutines.
�
쿙otes :
읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴*/
#include "w:\general\var.h"

#ifndef VEC
#define VEC

unsigned char *Vxx_Load(const char *FileName, int FileOffset);
void Vxx_Unload(Header *VectorData);
void *SCN_Load(const char *FileName, int FileOffset);
void SCN_Unload(void *OldScene);

extern "C" 	void ASM_CALL 	V01();
extern "C" 	void ASM_CALL 	V03();
extern "C" 	void ASM_CALL 	V04();
extern "C" 	void ASM_CALL 	V05();
extern "C" 	void ASM_CALL 	V06();
extern "C" 	void ASM_CALL 	V07();

#endif

