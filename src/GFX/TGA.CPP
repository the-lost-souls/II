/*
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ WATCOM C++ SOURCE ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
³Name         : TGA.CPP
³Author       : Cyberfish/TLS
³Last update  : 01.04.1998
³Action       : Loads/decodes a TGA-picture (type 2/10) into memory...
³
³Notes : Merry christmas! :)
ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/

#include <stdio.h>
#include <mem.h>
#include "w:\general\var.h"

struct TGA_Header
{
	unsigned char IDFieldLength;
	unsigned char ColorMapType;
	unsigned char ImageType;

	unsigned char CMapOrigLo;
	unsigned char CMapOrigHi;
	unsigned char CMapLengthLo;
	unsigned char CMapLengthHi;
	unsigned char CMapSize;
	unsigned char XOrigLo;
	unsigned char XOrigHi;
	unsigned char YOrigLo;
	unsigned char YOrigHi;
	unsigned char WidthLo;
	unsigned char WidthHi;
	unsigned char HeigthLo;
	unsigned char HeigthHi;
	unsigned char Bits;

	unsigned char IDescriptor;
};

static char 	OpenErr[] = "TGA: Error opening file ...                    \n";
static char	ReadErr[] = "TGA: Error reading file ...                    \n";
static char 	TypeErr[] = "TGA: Type not supported ...                    \n";
static char 	MemErr[]  = "TGA: Error allocating memory ...               \n";

int 	TGA_Width;
int 	TGA_Heigth;
int 	TGA_XPos;
int	TGA_YPos;
int 	TGA_Bits;


/*
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ WATCOM C++ ROUTINE ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
³Name         : TGA_Load
³ID           : TGA
³Last update  : 01.04.1998
³Optimized    : -
³
³In:	Filename : File to load
³	FOffset  : Fileoffset to read from
³	FSize    : Size of file to load. 0 = read to end of file
³
³Out:  Returns offset to picture. 0 if error.
³
³Notes: Can only read 15/16-bit pictures.
ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/

unsigned char *TGA_Load(const char *FName, int FOffset, int FSize)
{
	TGA_Header *FHeader;

	int Qk = strlen(FName) - 15;
	if ( Qk < 0 ) Qk = 0;
	strcpy(&OpenErr[48 - 21], &FName[Qk]);
	strcpy(&ReadErr[48 - 21], &FName[Qk]);
	strcpy(&TypeErr[48 - 21], &FName[Qk]);
	strcpy(&MemErr[48 - 17], &FName[Qk]);

//---- Open file...
	FILE *InFile;
	InFile = fopen(FName, "rb");
	if (InFile == NULL)
	{
		SYS_QuitMsg = OpenErr;
		return 0;
	}


//---- Evt. getting filesize...
	if ( FSize == 0 )
	{
		fpos_t _FSize;
		fseek(InFile, 0, SEEK_END);
		fgetpos(InFile, &_FSize);
		FSize = (int)_FSize - FOffset;

	}//if


//---- Allocating memory, reading and closing file...
	fseek(InFile, FOffset, SEEK_SET);
	unsigned char *TempMem = new unsigned char[FSize];
	if (TempMem == NULL)
	{
		SYS_QuitMsg = MemErr;
		fclose(InFile);
		return 0;
	}

	if (fread(TempMem, FSize, 1, InFile) < 1)
	{
		delete [] TempMem;
		SYS_QuitMsg = ReadErr;
		fclose(InFile);
		return 0;
	}

	fclose(InFile);

//---- Analyzing header and allocating more memory
	FHeader = (TGA_Header*)TempMem;

	TGA_Width  = (FHeader->WidthHi << 8) + FHeader->WidthLo;
	TGA_Heigth = (FHeader->HeigthHi << 8) + FHeader->HeigthLo;
	TGA_Bits   = (FHeader->Bits);

	int BytesPrPixel = (TGA_Bits+1) >> 3;

	int MemReq = TGA_Width*TGA_Heigth*BytesPrPixel;
	unsigned char *Picture = new unsigned char[MemReq];
	if (Picture == NULL)
	{
		SYS_QuitMsg = MemErr;
		delete [] TempMem;
		return 0;
	}


//---- Setting TGAData to picturedata...

	int CMSize = (FHeader->CMapLengthHi << 8) + (FHeader->CMapLengthLo);
	CMSize = CMSize * ((FHeader->CMapSize+1) >> 3) * FHeader->ColorMapType;

	unsigned char *TGAData = TempMem
				+sizeof(TGA_Header)
				+FHeader->IDFieldLength
				+CMSize;



	switch ( (int)FHeader->ImageType )
	{
		case 2: {
				memcpy(Picture, TGAData, MemReq);
				break;
			}//case 2
		case 10:{
				__asm{
					pushad
					pushfd

					mov	esi, TGAData
					mov	edi, Picture

					mov	edx, TGA_Width
					imul	edx, TGA_Heigth
//					imul	edx, BytesPrPixel

					xor	ebx, ebx

				RLELoop:
					cmp	ebx, edx
					jge	DoneRLE

					movzx   ecx, b[esi]
					and	ecx,  0x7F      	// =01111111b
					inc	ecx
					add	ebx, ecx		//increase pixelcounter

					test	BYTE PTR [esi], 0x80
					jz	RawPacket

					mov     ax, [esi + 1]		//Load colorvalue
					rep	stosw			//Store pixels

					add	esi, 3			//Dump packet
					jmp	RLELoop

				RawPacket:
					inc	esi
					rep	movsw			//copy rawpacket
					jmp	RLELoop

				DoneRLE:

					popfd
					popad
				}//asm
				break;
			}//case 10


		default:{
				SYS_QuitMsg = TypeErr;
				delete [] TempMem;
				delete [] Picture;
			 	return 0;
			}
	}


//---- Evt. flipping picture upside down...
	if ( (FHeader->IDescriptor & 0x20) == 0 ) 	//mask out bit 5
	{
		unsigned char *TempLine = new unsigned char[TGA_Width*BytesPrPixel];
		if (TempLine == NULL)
		{
			SYS_QuitMsg = MemErr;
			delete [] TempMem;
			delete [] Picture;
			return 0;
		}

		for ( int C = 0; C < (TGA_Heigth >> 1); C++ )
		{
			memcpy(	TempLine,			//copy upper line
				(void*)&Picture[C*TGA_Width*BytesPrPixel],
				TGA_Width*BytesPrPixel);

			memcpy( (void*)&Picture[C*TGA_Width*BytesPrPixel],
				(void*)&Picture[((TGA_Heigth-1)-C)*TGA_Width*BytesPrPixel],
				TGA_Width*BytesPrPixel);

			memcpy(	(void*)&Picture[((TGA_Heigth-1)-C)*TGA_Width*BytesPrPixel],
				TempLine,
				TGA_Width*BytesPrPixel);
		}


		delete [] TempLine;
	}

	delete [] TempMem;
	return Picture;
}//TGA_Load


