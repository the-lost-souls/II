/*
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ WATCOM C++ SOURCE ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
³Name         : VBE.CPP
³Author       : Cyberfish/TLS
³Last update  : 08.02.1998
³Action       : VESA routines. Using div. libraries
³
³Notes :
ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/

#include "w:\vbe\vbe.h"
#include "w:\general\var.h"
#include "w:\render\render.h"
#include "w:\general\canvas.h"

SV_devCtx      *VBE_Info;
VBE_ModeInfo   VBE_ModeList[256];
int VBE_NrModes = -1;
int VBE_Initialized = 0;
int VBE_Mode = -1;
int VBE_XRes = 320;
int VBE_YRes = 200;
int VBE_BytesPerScanline = 640;


const char SVGAErr[] = "VBE: SVGA not supported.\n";
const char VESAErr[] = "VBE: VESA 2.0 not supported.\n";
const char InitErr[] = "VBE: Functions not initialized.\n";
const char ModeErr[] = "VBE: Couldn't find mode in modelist.\n";
const char MemErr[]  = "VBE: Not enough memory for zbuffer/doublebuffer.\n";
const char ModeErr2[] = "VBE: Desired VESA mode not available.\n";

/*
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ WATCOM C++ ROUTINE ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
³Name         : VBE_Init
³ID           : VBE
³Last update  : 08.02.1998
³Action	      : Sets up different variables for the VBE functions.
³Optimized    : -
³
³In:	Nothing.
³
³Out:  Returns 0 if any error.
³
³Notes:
ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/

int VBE_Init(void)
{
	VBE_Info = SV_init(FALSE);		//getting div info

	if ( VBE_Info == 0 )			//svga?
	{
		SYS_QuitMsg = (char*)&SVGAErr;
		return 0;
	}//if

/*	if ( VBE_Info->VBEVersion < 0x0200 )	//vesa 2.0 ?
	{
		SYS_QuitMsg = (char*)&VESAErr;
		return 0;
	}//if*/

//---- Building modelist
	uint16 *P;

	P = VBE_Info->modeList;
	SV_modeInfo ModeInfo;

	VBE_ModeList[0].XRes = 320;
	VBE_ModeList[0].YRes = 200;
	VBE_ModeList[0].BitsPerPixel = 8;
	VBE_ModeList[0].Mode = 0x13;
	VBE_ModeList[0].BytesPerScanline = 320;

	VBE_NrModes = 1;
	while (*P != 0xFFFF)	{
		// Get information on each mode available
		SV_getModeInfo(*P,&ModeInfo);

		VBE_ModeList[VBE_NrModes].XRes = ModeInfo.XResolution;
		VBE_ModeList[VBE_NrModes].YRes = ModeInfo.YResolution;
		VBE_ModeList[VBE_NrModes].BitsPerPixel = ModeInfo.BitsPerPixel;
		VBE_ModeList[VBE_NrModes].Mode = ModeInfo.Mode;
		if ( (ModeInfo.Attributes & svHaveLinearBuffer) )
			VBE_ModeList[VBE_NrModes].Mode |= svLinearBuffer;

		VBE_ModeList[VBE_NrModes].BytesPerScanline = ModeInfo.BytesPerScanLine;


		P++;
		VBE_NrModes++;
		if ( VBE_NrModes > 255 ) VBE_NrModes = 255;
	}//while

	VBE_Initialized = 1;
	return 1;
}//VBE_Init




/*
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ WATCOM C++ ROUTINE ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
³Name         : VBE_GetMode
³ID           : VBE
³Last update  : 08.02.1998
³Action	      : Searches through the modelist for desired mode
³Optimized    : -
³
³In:	Horisontal/vertical resolution + bits per pixel
³
³Out:  Returns 0 if error, else modenumber.
³
³Notes:
ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/


int VBE_GetMode(int XRes, int YRes, int BitsPerPixel)
{
	if ( !VBE_Initialized )
	{
		SYS_QuitMsg = (char*)&InitErr;
		return 0;
	}//if not initialized

	for ( int C = 0; C < VBE_NrModes; C++ )
	{
		if ( 	(VBE_ModeList[C].XRes == XRes) &&
			(VBE_ModeList[C].YRes == YRes) &&
			(VBE_ModeList[C].BitsPerPixel == BitsPerPixel) ) break;
	}//for

	if ( C == VBE_NrModes )
	{
		SYS_QuitMsg = (char*)&ModeErr2;
		return 0;
	}

	return C;
}//VBE_GetMode


/*
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ WATCOM C++ ROUTINE ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
³Name         : VBE_SetMode
³ID           : VBE
³Last update  : 08.02.1998
³Action	      : Sets the desired mode if it is available
³Optimized    : -
³
³In:	Horisontal/vertical resolution + bits per pixel
³
³Out:  Returns 0 if error, else modenumber.
³
³Notes:
ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/

int VBE_SetMode(int XRes, int YRes, int BitsPerPixel)
{
	if ( !VBE_Initialized )
	{
		SYS_QuitMsg = (char*)&InitErr;
		return 0;
	}//if not initialized

	int C = 0;
	while ( C < VBE_NrModes )
	{
		if ( 	(VBE_ModeList[C].XRes == XRes) &&
			(VBE_ModeList[C].YRes == YRes) &&
			(VBE_ModeList[C].BitsPerPixel == BitsPerPixel) ) break;
		C++;
	}//for

	if ( C == VBE_NrModes )
	{
		SYS_QuitMsg = (char*)&ModeErr2;
		return 0;
	}

	VBE_Mode = C;

	SV_setMode(VBE_ModeList[C].Mode, FALSE, TRUE, 0);
	return VBE_ModeList[C].Mode;
}//VBE_SetMode


/*
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ WATCOM C++ ROUTINE ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
³Name         : VBE_SetupMode
³ID           : VBE
³Last update  : 08.02.1998
³Action	      : Allocates memory for double/z-buffer, sets up dif. variables
³Optimized    : -
³
³In:	Nothing.
³
³Out:  Returns 0 if error.
³
³Notes:
ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/

int VBE_SetupMode(int Mode)
{
	if ( Mode > VBE_NrModes )
	{
		SYS_QuitMsg = (char*)&ModeErr;
		return 0;
	}

	VBE_XRes = VBE_ModeList[Mode].XRes;
	VBE_YRes = VBE_ModeList[Mode].YRes;

	VBE_BytesPerScanline = VBE_ModeList[Mode].BytesPerScanline;
	if ( VBE_BytesPerScanline*2 != VBE_XRes ) SYS_ORoutines[4] = (int)&CNV_RStrange_Linear;

//---- Setting up doublebuffer/zbuffer/vgaoffset
	if ( SYS_DBufOffs != 0 )
		delete [] SYS_DBufOffs;

	if ( SYS_ZBufOffs != 0 )
		delete [] SYS_ZBufOffs;

	SYS_DBufSize = VBE_XRes*VBE_YRes*2;
	SYS_DBufOffs = new unsigned char[SYS_DBufSize];
	SYS_DBufLine = VBE_XRes*2;
	if ( SYS_DBufOffs == 0 )
	{
		SYS_QuitMsg = (char*)&MemErr;
		return 0;
	}

	SYS_ScreenShot = new unsigned char[SYS_DBufSize];
	if ( SYS_ScreenShot == NULL )
	{
		SYS_QuitMsg = (char*)&MemErr;
		return 0;
	}

	SYS_ZBufSize = VBE_XRes*VBE_YRes*4;
	SYS_ZBufOffs = new unsigned char[SYS_ZBufSize];
	SYS_ZBufLine = VBE_XRes*4;
	if ( SYS_ZBufOffs == 0 )
	{
		delete [] SYS_DBufOffs;
		SYS_QuitMsg = (char*)&MemErr;
		return 0;
	}

//---- Blanking dbuf/zbuf
	__asm{
		pushad
		pushfd

		mov	edi, SYS_DBufOffs
		mov	ecx, SYS_DBufSize
		shr	ecx, 2
		xor	eax, eax
		rep	stosd

		mov	edi, SYS_ZBufOffs
		mov	ecx, SYS_ZBufSize
		shr	ecx, 2
		xor	eax, eax
		rep	stosd

		popfd
		popad


	}//asm



//---- Setting div. variables...
	SYS_FPClipLeft 		= 0;
	SYS_FPClipRight 	= VBE_XRes - 1;
	SYS_FPClipBottom	= VBE_YRes - 1;
	SYS_FPClipTop		= 0;

	SYS_XPerspective	= ((float)VBE_XRes/320)*256;
	SYS_YPerspective	= ((float)VBE_YRes/200)*-256;

	SYS_XCenter 		= (float)VBE_XRes/2;
	SYS_YCenter		= (float)VBE_YRes/2;

	if ( !(VBE_ModeList[Mode].Mode & svLinearBuffer) )
		SYS_VideoMem = (void*)0x0A0000;
	else
		SYS_VideoMem = VBE_Info->videoMem;


//---- Init rendering-routines...
	T02_Init();
	T04_Init();
	T05_Init();
	T06_Init();
	T07_Init();
	T08_Init();
	T09_Init();


/*//---- Flipping banked/linear canvas-routines...
	if ( VBE_ModeList[Mode].Mode & svLinearBuffer )
	{
		SYS_ORoutines[4] = (int)&CNV_RNormal_Linear;
	}
	else
	{
		SYS_ORoutines[4] = (int)&CNV_RNormal_Banked;
	}*/

	return 1;
}//VBE_SetupMode
