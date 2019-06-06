/*
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ WATCOM C++ SOURCE ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
³Name         : V00.CPP
³Author       : Cyberfish/TLS & Toe/DU
³Last update  : 10.03.1998
³Action       : Loads a vectorobject.
³
³Notes :
ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/

#include "w:\general\var.h"
#include "w:\gfx\tga.h"
#include "w:\general\texture.h"
#include <stdio.h>
#include <mem.h>
#include <math.h>

typedef unsigned char byte;

static char OpenErr[] = "Vxx: Couldn't open file\n";
static char ReadErr[] = "Vxx: Error reading file\n";
static char CorrErr[] = "Vxx: Corrupt object file\n";
static char MemErr[] = "Vxx: Couldn't allocate memory\n";
static char CheckSumErr[] = "Vxx: Checksum error in object file\n";
static char TypeErr[] = "Vxx: Couldn't recognize objecttype\n";


const char Main_ID[] = "MAIN";
const char Vert_ID[] = "VERT";
const char Face_ID[] = "POLS";
const char Matr_ID[] = "MATR";
const char Mspc_ID[] = "MSPC";

typedef struct{
	char		Main_ID[4];
	int		Main_Size;
	int		Main_Version;
	int		Main_Type;
	int		Main_FSize;
	unsigned int	Main_CheckSum;
	int		Main_Vertices;
	int		Main_Faces;
	int		Main_Materials;
} Main_Chunk;



/*
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ WATCOM C++ ROUTINE ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
³Name         : Vxx_Load
³ID           : Vxx
³Last update  : 10.03.1998
³Optimized    : -
³
³In:	Filename : File to load
³	FOffset  : Fileoffset to read from
³
³Out:  Returns offset to header
³
ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/

unsigned char *Vxx_Load(const char *FileName, int FileOffset)
{

	Main_Chunk FileHeader;

//---- Open file...
	FILE *InFile;
	InFile = fopen(FileName, "rb");
	if (InFile == NULL)
	{
		SYS_QuitMsg = OpenErr;
		return 0;
	}


//---- Read main header
	fseek(InFile, FileOffset, SEEK_SET);
	fseek(InFile, 66, SEEK_CUR);			//Dump text-info

	if (fread(&FileHeader, sizeof(Main_Chunk), 1, InFile) < 1)
	{
		SYS_QuitMsg = ReadErr;
		fclose(InFile);
		return 0;
	}


//---- Check main header...
	if (memcmp(FileHeader.Main_ID, Main_ID, 4))
	{
		SYS_QuitMsg = CorrErr;
		fclose(InFile);
		return 0;
	}


//---- allocate memory to read entire file, then read and close it
	unsigned char *TempMem = new unsigned char[FileHeader.Main_FSize];
	if (TempMem == NULL)
	{
		SYS_QuitMsg = MemErr;
		fclose(InFile);
		return 0;
	}

	fseek(InFile, 0, SEEK_SET);
	if ( fread(TempMem, FileHeader.Main_FSize, 1, InFile) < 1)
	{
		SYS_QuitMsg = ReadErr;
		fclose(InFile);
		delete [] TempMem;
		return 0;
	}

	fclose(InFile);


//---- Calculate checksum...

	unsigned int OrgCheckSum, NewCheckSum = 0;

	OrgCheckSum = FileHeader.Main_CheckSum;	//Get 'should be' checksum

	(*(Main_Chunk*)&TempMem[66]).Main_CheckSum = 0;

	//Start counting bytes...

	for ( int Counter = 0; Counter < FileHeader.Main_FSize ; Counter++)
	{
		NewCheckSum += (unsigned int)TempMem[Counter];
	}

	if ( NewCheckSum != OrgCheckSum)
	{
		SYS_QuitMsg = CheckSumErr;
		delete [] TempMem;
		return 0;
	}

	int TempMemPtr = 66 + sizeof(Main_Chunk);
	float *VertOffs;
	char *FaceOffs, *MatrOffs;
	int VertSize, FaceSize, MatrSize, SkipSize;


	while ( TempMemPtr < FileHeader.Main_FSize )
	{
		if ( !memcmp(&TempMem[TempMemPtr], Vert_ID, 4))
		{
			//Vert chunk found...
			VertOffs = (float*)&TempMem[TempMemPtr + 8];
			VertSize = *(int*)&TempMem[TempMemPtr+4];
			SkipSize = VertSize;
		}
		else if ( !memcmp(&TempMem[TempMemPtr], Face_ID, 4) )
		{
			//Face chunk found...
			FaceOffs = &TempMem[TempMemPtr + 8];
			FaceSize = *(int*)&TempMem[TempMemPtr+4];
			SkipSize = FaceSize;
		}
		else if ( !memcmp(&TempMem[TempMemPtr], Matr_ID, 4) )
		{
			//Matr chunk found...
			MatrOffs = &TempMem[TempMemPtr + 8];
			MatrSize = *(int*)&TempMem[TempMemPtr+4];
			SkipSize = MatrSize;
		}
		TempMemPtr += SkipSize + 8;
	}

	int MemReq = 0;

//---- Allocate memory for the object
	MemReq += FileHeader.Main_Vertices*V_Size;
	MemReq += FileHeader.Main_Faces*F_Size;
	MemReq += FileHeader.Main_Materials*M_Size;
	MemReq += H_Size;

	unsigned char *ObjectData = new unsigned char[MemReq];
	if ( ObjectData == NULL )
	{
		SYS_QuitMsg = MemErr;
		delete [] TempMem;
		return 0;
	}

	Header *Obj_Header = (Header*)ObjectData;
	Vertice *Obj_VertList = (Vertice*)(ObjectData+H_Size);

	Face *Obj_FaceList = (Face*)(ObjectData+H_Size +
		FileHeader.Main_Vertices*V_Size);

	Material *Obj_MatrList = (Material*)(ObjectData+H_Size +
		FileHeader.Main_Vertices*V_Size +
		FileHeader.Main_Faces*F_Size);

	for (Counter = 0; Counter < 8 ;Counter++ )
	{
		Obj_Header->H_Plugins[Counter*2] = -1;
	}


	Obj_Header->H_Vertices 	= FileHeader.Main_Vertices;
	Obj_Header->H_Faces 	= FileHeader.Main_Faces;
	Obj_Header->H_Materials = FileHeader.Main_Materials;
	Obj_Header->H_Memory 	= MemReq;

	Obj_Header->H_VertOffs 	= Obj_VertList;
	Obj_Header->H_FaceOffs 	= Obj_FaceList;
	Obj_Header->H_MatrOffs 	= Obj_MatrList;


//---- Set up vertice structures
	for (Counter = 0; Counter<FileHeader.Main_Vertices; Counter++ )
	{
		Obj_VertList[Counter].V_X = VertOffs[Counter*3];
		Obj_VertList[Counter].V_Y = VertOffs[Counter*3+1];
		Obj_VertList[Counter].V_Z = VertOffs[Counter*3+2];
		Obj_VertList[Counter].V_RFlag = 0;
	}

//---- Load materials...
	int MatOffs = 0;

	for ( Counter = 0; Counter<FileHeader.Main_Materials; Counter++ )
	{
		if ( !memcmp(&MatrOffs[MatOffs], Mspc_ID, 4 ))
		{
			Obj_MatrList[Counter].M_R = *(float*)&MatrOffs[MatOffs+8];
			Obj_MatrList[Counter].M_G = *(float*)&MatrOffs[MatOffs+12];
			Obj_MatrList[Counter].M_B = *(float*)&MatrOffs[MatOffs+16];

			Obj_MatrList[Counter].M_Luminosity =
			 *(float*)&MatrOffs[MatOffs+20];

			Obj_MatrList[Counter].M_Diffuse =
			 *(float*)&MatrOffs[MatOffs+24];

			Obj_MatrList[Counter].M_Transparency =
			 *(float*)&MatrOffs[MatOffs+28];


		      	if ( MatrOffs[MatOffs + 40] != 0 ) //Loading texture
			{
				Obj_MatrList[Counter].M_MapOffset =
				 TXT_LoadTexture(	&MatrOffs[MatOffs+40],
				 			0, 0,		//Foffs, fsize
							Obj_MatrList[Counter].M_MapWidth,
							Obj_MatrList[Counter].M_MapHeigth);

				if ( Obj_MatrList[Counter].M_MapOffset == 0 )
  				 return 0;	//SYS_QuitMsg already set

			}//if

		}//if
		MatOffs += *(int*)&MatrOffs[MatOffs+4] + 8;
	}//for


//---- Set up face structures
	float A0, A1, A2, B0, B1, B2, NVX, NVY, NVZ, Length, DotProduct;


	for (Counter = 0; Counter<FileHeader.Main_Faces; Counter++ )
	{

		Obj_FaceList[Counter].F_V0Offs = &Obj_VertList[*(int*)&FaceOffs[Counter*40]];
		Obj_FaceList[Counter].F_V1Offs = &Obj_VertList[*(int*)&FaceOffs[Counter*40+4]];
		Obj_FaceList[Counter].F_V2Offs = &Obj_VertList[*(int*)&FaceOffs[Counter*40+8]];

		//Temporary used to help calculate normal vectors
		Obj_FaceList[Counter].F_NextOffset = *(int*)&FaceOffs[Counter*40];
		Obj_FaceList[Counter].F_SortValue = *(int*)&FaceOffs[Counter*40+4];
		Obj_FaceList[Counter].F_Type = *(int*)&FaceOffs[Counter*40+8];

		int Mat = FaceOffs[Counter*40 + 36];

		Obj_FaceList[Counter].F_Material = &Obj_MatrList[Mat];


		Obj_FaceList[Counter].F_U0 = *(float*)&FaceOffs[Counter*40+12] * (Obj_MatrList[FaceOffs[Counter*40 + 36]].M_MapWidth-1);
		Obj_FaceList[Counter].F_V0 = *(float*)&FaceOffs[Counter*40+16] * (Obj_MatrList[FaceOffs[Counter*40 + 36]].M_MapHeigth-1);
		Obj_FaceList[Counter].F_U1 = *(float*)&FaceOffs[Counter*40+20] * (Obj_MatrList[FaceOffs[Counter*40 + 36]].M_MapWidth-1);
		Obj_FaceList[Counter].F_V1 = *(float*)&FaceOffs[Counter*40+24] * (Obj_MatrList[FaceOffs[Counter*40 + 36]].M_MapHeigth-1);
		Obj_FaceList[Counter].F_U2 = *(float*)&FaceOffs[Counter*40+28] * (Obj_MatrList[FaceOffs[Counter*40 + 36]].M_MapWidth-1);
		Obj_FaceList[Counter].F_V2 = *(float*)&FaceOffs[Counter*40+32] * (Obj_MatrList[FaceOffs[Counter*40 + 36]].M_MapHeigth-1);


		//Caclulate face vectors
		A0 = Obj_FaceList[Counter].F_V1Offs->V_X -
			Obj_FaceList[Counter].F_V0Offs->V_X;

		A1 = Obj_FaceList[Counter].F_V1Offs->V_Y -
			Obj_FaceList[Counter].F_V0Offs->V_Y;

		A2 = Obj_FaceList[Counter].F_V1Offs->V_Z -
			Obj_FaceList[Counter].F_V0Offs->V_Z;


		B0 = Obj_FaceList[Counter].F_V2Offs->V_X -
			Obj_FaceList[Counter].F_V0Offs->V_X;

		B1 = Obj_FaceList[Counter].F_V2Offs->V_Y -
			Obj_FaceList[Counter].F_V0Offs->V_Y;

		B2 = Obj_FaceList[Counter].F_V2Offs->V_Z -
			Obj_FaceList[Counter].F_V0Offs->V_Z;


		NVX = A1*B2-A2*B1;
		NVY = A2*B0-A0*B2;
		NVZ = A0*B1-A1*B0;
		DotProduct = NVX*NVX + NVY*NVY + NVZ*NVZ;
		if ( DotProduct == 0 )
			Length = 1;
		else	Length = sqrt(NVX*NVX + NVY*NVY + NVZ*NVZ);

		Obj_FaceList[Counter].F_NVX = NVX / Length;
		Obj_FaceList[Counter].F_NVY = NVY / Length;
		Obj_FaceList[Counter].F_NVZ = NVZ / Length;

		Obj_FaceList[Counter].F_Priority = 0;

	}

//---- Calculate "smoothed" normalvectors... (for vertices)
	if ( (FileHeader.Main_Type == 13) || (FileHeader.Main_Type == 14) )
	for(int VertLoop = 0; VertLoop < FileHeader.Main_Vertices; VertLoop++)
	{
		NVX = 0.0;
		NVY = 0.0;
		NVZ = 0.0;

		for ( int FaceLoop = 0; FaceLoop < FileHeader.Main_Faces; FaceLoop++ )
		{
			if ( VertLoop == Obj_FaceList[FaceLoop].F_NextOffset )
			{
				NVX += Obj_FaceList[FaceLoop].F_NVX;
				NVY += Obj_FaceList[FaceLoop].F_NVY;
				NVZ += Obj_FaceList[FaceLoop].F_NVZ;
			}
			else if ( VertLoop == Obj_FaceList[FaceLoop].F_SortValue )
			{
				NVX += Obj_FaceList[FaceLoop].F_NVX;
				NVY += Obj_FaceList[FaceLoop].F_NVY;
				NVZ += Obj_FaceList[FaceLoop].F_NVZ;
			}
			else if ( VertLoop == Obj_FaceList[FaceLoop].F_Type )
			{
				NVX += Obj_FaceList[FaceLoop].F_NVX;
				NVY += Obj_FaceList[FaceLoop].F_NVY;
				NVZ += Obj_FaceList[FaceLoop].F_NVZ;
			}

		}//Faceloop

		Length = sqrt(NVX*NVX + NVY*NVY + NVZ*NVZ);

		Obj_VertList[VertLoop].V_NVX = NVX / Length;
		Obj_VertList[VertLoop].V_NVY = NVY / Length;
		Obj_VertList[VertLoop].V_NVZ = NVZ / Length;
//		Obj_VertList[VertLoop].V_RFlag = 0;
	}//VertLoop

	Obj_Header->H_Type = FileHeader.Main_Type;

	int RenderType;
	switch ( FileHeader.Main_Type )
	{
		case 3: RenderType = 1;
			break;
		case 5: RenderType = 2;
			break;
		case 10: RenderType = 3;
			 break;
		case 12: RenderType = 4;
			 break;
		case 13: RenderType = 5;
			 break;
		case 14: RenderType = 6;
			 break;

		default: SYS_QuitMsg = TypeErr;
			 return 0;
	}
	for (Counter = 0; Counter<FileHeader.Main_Faces; Counter++ )
		Obj_FaceList[Counter].F_Type = RenderType;

	delete [] TempMem;
	return ObjectData;
}//Vxx_Load



/*
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ WATCOM C++ ROUTINE ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
³Name         : Vxx_Unload
³ID           : Vxx
³Last update  : 10.03.1998
³Optimized    : -
³
³In:	Vectordata : Pointer to vectorheader
³
³Out:  -
³
³Notes:
ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/

void Vxx_Unload(Header *VectorData)
{
	Material *MatrOffs = VectorData->H_MatrOffs;


//---- Deleting textures...
	for ( int C = 0; C < VectorData->H_Materials; C++)
	{
		if (MatrOffs->M_MapOffset != 0) TXT_UnloadTexture(MatrOffs->M_MapOffset);
		MatrOffs++;
	}//for


//---- Deleting vectorobject's data...
	delete [] VectorData;

}//Vxx_Unload

