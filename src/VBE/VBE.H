/*
�������������������������� WATCOM C++ INCLUDEFILE ��������������������������
�Name         : VBE.H
�Author       : Cyberfish/TLS
�Last update  : 08.02.1998
�Action       : Definitions for vesa-routines
�
�Notes :
����������������������������������������������������������������������������*/

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