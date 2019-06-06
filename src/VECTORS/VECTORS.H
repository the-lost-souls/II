/*
�������������������������� WATCOM C++ INCLUDEFILE ��������������������������
�Name         : VECTORS.H
�Author       : Cyberfish/TLS & Toe/DU
�Last update  : 08.02.1998
�Action       : Definitions for all vectorroutines.
�
�Notes :
����������������������������������������������������������������������������*/
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
