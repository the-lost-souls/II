/*
旼컴컴컴컴컴컴컴컴컴컴컴컴 WATCOM C++ INCLUDEFILE 컴컴컴컴컴컴컴컴컴컴컴컴컴
쿙ame         : VBE.H
쿌uthor       : Cyberfish/TLS
쿗ast update  : 08.02.1998
쿌ction       : Definitions for vesa-routines
�
쿙otes :
읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴*/

#include "w:\vbe\uvbelib.h"
#include "w:\vbe\pmode.h"
#include "w:\vbe\svga.h"

#ifndef VBE
#define VBE

typedef struct {
	int 	XRes;
	int	YRes;
	int	BitsPerPixel;
	int	Mode;
	int	BytesPerScanline;
	int	Linear;
} VBE_ModeInfo;

extern 		SV_devCtx      *VBE_Info;
extern 		int		VBE_NrModes;
extern 		VBE_ModeInfo	VBE_ModeList[];
extern "C"	int 		VBE_XRes;
extern "C"	int 		VBE_YRes;
extern 		int 		VBE_Mode;
extern "C"      int 		VBE_BytesPerScanline;


int VBE_Init(void);
int VBE_GetMode(int XRes, int YRes, int BitsPerPixel);
int VBE_SetMode(int XRes, int YRes, int BitsPerPixel);
int VBE_SetupMode(int Mode);


#endif
