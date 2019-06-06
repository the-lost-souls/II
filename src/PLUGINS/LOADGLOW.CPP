/*
�������������������������� WATCOM C++ SOURCE �������������������������������
�Name         : LOADGLOW.CPP
�Author       : Cyberfish/TLS
�Last update  : 13.03.1998
�Action       : Loads a tga to use as transparencymap (glow-effect)
�
�Notes :
����������������������������������������������������������������������������*/

#include "w:\gfx\gfx.h"
#include "w:\general\var.h"
#include "w:\plugins\glow.h"


const char MemErr[] = "GLW: Unable to allocate memory\n";


/*
�������������������������� WATCOM C++ ROUTINE ������������������������������
�Name         : GLW_Load
�ID           : GLW
�Last update  : 10.03.1998
�Optimized    : -
�
�In:	Flare's color
�	filename, fileoffset and filesize.
�
�Out:  Returns pointer to plugindata
�
����������������������������������������������������������������������������*/


GLW_PluginData *GLW_Load(const char *TGAFile, int FOffset, int FSize)
{
	int C1, C2;

//---- Allocating memory for plugindata...
	GLW_PluginData *GlowData = new GLW_PluginData[1];
	if ( GlowData == 0 )
	{
		SYS_QuitMsg = (char*)&MemErr;
		return 0;
	}//if


//---- Loading TGA for transparency map...
	unsigned char *TGATemp = TGA_Load(TGAFile, FOffset, FSize);
	if ( TGATemp == 0 )
	{
		delete [] GlowData;
		return 0;
	}//if

	int NrPixels = TGA_Width*TGA_Heigth;

	GlowData->GLW_MapXSize = TGA_Width;
	GlowData->GLW_MapYSize = TGA_Heigth;


//---- Converting 16-bit TGA-picture to 4-bit greyscale (saved as bytes)
	GlowData->GLW_Map = new unsigned char[TGA_Width*TGA_Heigth];
	if ( GlowData->GLW_Map == 0 )
	{
		SYS_QuitMsg = (char*)&MemErr;
		delete [] GlowData;
		delete [] TGATemp;
		return 0;
	}//if


	void *PointerA = (void*)TGATemp;
	void *PointerB = (void*)GlowData->GLW_Map;

	__asm{
		pushad
		pushfd

		mov	ecx, NrPixels
		mov	esi, PointerA
		mov	edi, PointerB

	GreyScaleLoop:
		movzx   eax, w[esi]		//load pixel

		mov	ebx, 0x1F 		//mask out blue
		and	ebx, eax
		mov	edx, 0x3E0              //mask out green
		and	edx, eax
		shr	edx, 5
		and	eax, 0x7C00           	//mask out red
		shr	eax, 10

		add	eax, ebx
		add	eax, edx

		xor	edx, edx		//get average greyscalecolor...
		mov	ebx, 3
		idiv	ebx			//divide by 3

		shr	eax, 1			//range 0-15

		mov	b[edi], al

		add	esi, 2
		inc	edi

		dec	ecx
		jnz	GreyScaleLoop

		popfd
		popad
	}//asm

	delete [] TGATemp;

	GlowData->GLW_NextOffset = 0;
	GlowData->GLW_SortValue = 0;
	GlowData->GLW_Type = 7;
	GlowData->GLW_Priority = 240;
	GlowData->GLW_Scale = 1.0;
	GlowData->GLW_R = 1.0;
	GlowData->GLW_G = 1.0;
	GlowData->GLW_B = 1.0;

	return GlowData;
}//GLW_Load



/*
�������������������������� WATCOM C++ ROUTINE ������������������������������
�Name         : GLW_Unload
�ID           : GLW
�Last update  : 13.03.1998
�Optimized    : -
�
�In:	A pointer to a GLW_PluginData structure...
�
�
�Out:  -
�
����������������������������������������������������������������������������*/

void GLW_Unload(GLW_PluginData *Glow)
{
	delete [] Glow->GLW_Map;
	delete [] Glow;
}//GLW_Unload