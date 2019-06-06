#include <string.h>
#include "w:\gfx\tga.h"
#include "w:\general\var.h"

const int MaxTextures = 256;

int TXT_Initialized = 0;

static int TXT_Textures[MaxTextures];
void TXT_Initialize(void);

const char TXT_Err1[] = "TXT: Too many textures\n";
const char TXT_Err2[] = "TXT: Picture type not supported\n";


typedef struct
{
	void	*TXT_Texture;
	int	TXT_Width;
	int	TXT_Heigth;
	int	TXT_Users;
	int	TXT_FileOffset;
	char	TXT_FileName[];
}TXT_Data;




void TXT_Initialize(void)
{
	for ( int C = 0; C < MaxTextures; C++ )
		TXT_Textures[C] = NULL;

	TXT_Initialized = 1;
}//TXT_initialize





void *TXT_LoadTexture(	const char *FileName,
			int FileOffset,
			int FileSize,
			int &Width,
			int &Heigth)
{
	if ( TXT_Initialized == 0 ) TXT_Initialize();

	int C;
	int _Width, _Heigth;

	TXT_Data *TempTXT;
//---- Check if texture already is loaded...
	for ( C = 0; C < MaxTextures; C++)
	{
		if ( TXT_Textures[C] != NULL )
		{
			TempTXT = (TXT_Data*)TXT_Textures[C];
			if ( (TempTXT->TXT_FileOffset == FileOffset) && (strcmp(TempTXT->TXT_FileName, FileName) == 0) )
			{
				TempTXT->TXT_Users++;
				Width = TempTXT->TXT_Width;
				Heigth = TempTXT->TXT_Heigth;
				return TempTXT->TXT_Texture;
			}//if
		}//if
	}//for


//---- New texture so searching for "free" texture...
	for ( int Avail = 0; Avail < MaxTextures; Avail++ )
		if ( TXT_Textures[Avail] == 0 ) break;

	if ( Avail == MaxTextures )
	{
		SYS_QuitMsg = (char*)&TXT_Err1;
		return 0;
	}//if


//---- Loading texture...
	unsigned char *TOffset;


	if ( strcmp( (char*)&FileName[strlen(FileName) - 4], ".TGA" ) == 0 )
	{
		if ( FileName[1] == ':' ) TOffset = TGA_Load((char*)&FileName[3], FileOffset, FileSize);
		else TOffset = TGA_Load(FileName, FileOffset, FileSize);
		_Width = TGA_Width;
		_Heigth = TGA_Heigth;
	}
	else
	{
		SYS_QuitMsg = (char*)&TXT_Err2;
		return 0;
	}

	if ( TOffset == 0 ) return 0;


//---- Setting up texture...
	TempTXT = (TXT_Data*)new unsigned char [sizeof(TXT_Data) + strlen(FileName) + 1];
	TXT_Textures[Avail] = (int)TempTXT;

	TempTXT->TXT_Texture = TOffset;
	TempTXT->TXT_Users = 1;
	TempTXT->TXT_FileOffset = FileOffset;
	TempTXT->TXT_Width = _Width;
	TempTXT->TXT_Heigth = _Heigth;
	strcpy(TempTXT->TXT_FileName, FileName);


//---- Blending texture...
	int FPDump;
	float BlendingValue = 0.3;
	float ColorRange = 31.0;
	__asm{
		pushad
		pushfd

		mov	esi, TOffset
		mov	ecx, _Width
		imul	ecx, _Heigth
		fldcw	FPU_Round

	BlendLoop:
		xor	edx, edx

		mov	ax, [esi]		//Blue
		and	eax, 0x1F
		mov	FPDump, eax
		fld	ColorRange
		fisub	FPDump
		fmul	BlendingValue
		fiadd	FPDump
		fistp	FPDump
		mov	eax, FPDump
		cmp	eax, 31
		jbe	BlueOK
		mov	eax, 31
	BlueOK:
		or	edx, eax


		mov	ax, [esi]		//Green
		shr	eax, 5
		and	eax, 0x1F
		mov	FPDump, eax
		fld	ColorRange
		fisub	FPDump
		fmul	BlendingValue
		fiadd	FPDump
		fistp	FPDump
		mov	eax, FPDump
		cmp	eax, 31
		jbe	GreenOK
		mov	eax, 31
	GreenOK:
		shl	eax, 5
		or	edx, eax

		mov	ax, [esi]		//Red
		shr	eax, 10
		and	eax, 0x1F
		mov	FPDump, eax
		fld	ColorRange
		fisub	FPDump
		fmul	BlendingValue
		fiadd	FPDump
		fistp	FPDump
		mov	eax, FPDump
		cmp	eax, 31
		jbe	RedOK
		mov	eax, 31
	RedOK:
		shl	eax, 10
		or	edx, eax

		mov	[esi], dx

		add	esi, 2

		dec	ecx
		jnz	BlendLoop


		popfd
		popad
	}//asm

	Width = TempTXT->TXT_Width;
	Heigth = TempTXT->TXT_Heigth;
	return TOffset;
}//TXT_LoadTexture



void TXT_UnloadTexture(void *TextureOffset)
{
	for ( int C = 0; C < MaxTextures; C++ )
	{
		TXT_Data *TempTXT = (TXT_Data*)TXT_Textures[C];
		if ( TempTXT->TXT_Texture == TextureOffset )
		{
			if ( TempTXT->TXT_Users == 1 )
			{
				delete [] TempTXT->TXT_Texture;
				delete [] TempTXT;
				TXT_Textures[C] = NULL;
			}//if

			TempTXT->TXT_Users--;
		}//if
	}//for
}//UnloadTexture
