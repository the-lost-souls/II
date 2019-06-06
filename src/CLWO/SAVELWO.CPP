#include <mem.h>
#include <stdio.h>
#include "div.h"
#include <string.h>
#include <math.h>
#include <ctype.h>

int  AnalyzeLWO(void *LWOFile);
void SavePNTS(Vertice *Vertices);
void ReadMaterials(void);
void SavePOLS(Face *Faces);
void SaveSURF(Material *Materials);
void MapCoords( float X,
		float Y,
		float Z,
		float &U,
		float &V,
		int   FaceMaterial);


int 	NrVertices 	= 0;
int 	NrFaces		= 0;
int 	NrMaterials	= 0;
int 	PNTS		= 0;
int	POLS		= 0;
int	SRFS		= 0;

TextHeader *TextHeader_;
Chunk      *MainChunk;
FileHeader *FileHeader_;
Chunk	   *VertChunk;
Vertice    *Vertices;
Chunk      *FaceChunk;
Face	   *Faces;
Chunk	   *MatrChunk;
Material   *Materials;



typedef struct
{
	int	ID;
	void	*Offset;
	int	Size;
} LWOChunk;

typedef struct
{
	char 	Name[256];
	float   R, G, B;
	float	Diffuse, Luminosity, Transparency;
	char	TextureFile[256];
	float	XCenter, YCenter, ZCenter, XSize, YSize, ZSize;
	int	Axis;	//0 = xaxis, 1 = yaxis, 2 = zaxis
	int	MapType;	//0 = flat, 1 = sylindrical, 2 = spherical
} Surface;

const int MaxSurfaces = 50;
Surface Surfaces[MaxSurfaces];

const int MaxChunks = 200;
LWOChunk Chunks[MaxChunks];

char IDString[] = "CLWO v2.00\nCyberfish/TLS\nTxxx Vxxxxxx Fxxxxxx Mxxxx\n";


/*
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ WATCOM C++ ROUTINE ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
³Name         : SaveLWO
³Last update  : 06.01.1998
³Action	      : Converts an .lwo-file
³
³In:	pointer to loaded .lwo-file
³
³Out:  Returns offset to converted file
ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
void *SaveLWO(void *LWOFile, int Type)
{
	int C;

//---- Analyzing file
	if ( AnalyzeLWO(LWOFile) == 0 )
	{
		return 0;
	}//if


//---- Reading materials...
	ReadMaterials();


//---- Allocating memory and setting up pointers...
	int Memreq = 	  sizeof(TextHeader)
			+ sizeof(FileHeader) + sizeof(Chunk)
			+ sizeof(Vertice)*NrVertices + sizeof(Chunk)
			+ sizeof(Face)*NrFaces + sizeof(Chunk)
			+ sizeof(Chunk);
	for ( C = 0; C < NrMaterials; C++ )
	{
		Memreq += strlen(Surfaces[C].TextureFile) + 1 + sizeof(Material);
	}//for

	unsigned char *OutLWO = new unsigned char[Memreq];
	if ( OutLWO == NULL )
	{
		printf("ERR: Error allocating memory ");
		return 0;
	}
	printf("%d bytes memory allocated\n", Memreq);

	TextHeader_ 	= (TextHeader*)OutLWO;
	MainChunk	= (Chunk*)((char*)TextHeader_ + sizeof(TextHeader));
	FileHeader_ 	= (FileHeader*)((char*)MainChunk + sizeof(Chunk));
	VertChunk   	= (Chunk*)((char*)FileHeader_ + sizeof(FileHeader));
	Vertices    	= (Vertice*)((char*)VertChunk + sizeof(Chunk));
	FaceChunk	= (Chunk*)((char*)Vertices + sizeof(Vertice)*NrVertices);
	Faces		= (Face*)((char*)FaceChunk + sizeof(Chunk));
	MatrChunk   	= (Chunk*)((char*)Faces + sizeof(Face)*NrFaces);
	Materials   	= (Material*)((char*)MatrChunk + sizeof(Chunk));


//---- Saving vertices...
	VertChunk->ID   = VertID;
	VertChunk->Size = NrVertices*sizeof(Vertice);
	SavePNTS(Vertices);


//---- Saving faces...
	SavePOLS(Faces);


//---- Saving materials...
	SaveSURF(Materials);


//---- Setting up header...
	printf("Setting up header...\n");

	MainChunk->ID 	= MainID;
	MainChunk->Size = sizeof(FileHeader);

	FileHeader_->Version	= 200;
	FileHeader_->ObjectType	= Type;
	FileHeader_->FileSize 	= Memreq;
	FileHeader_->CheckSum	= 0;
	FileHeader_->Vertices 	= NrVertices;
	FileHeader_->Faces 	= NrFaces;
	FileHeader_->Materials 	= NrMaterials;

	VertChunk->ID	= VertID;
	VertChunk->Size	= sizeof(Vertice)*NrVertices;
	FaceChunk->ID	= FaceID;
	FaceChunk->Size	= sizeof(Face)*NrFaces;
	MatrChunk->ID	= MatrID;	//size is set by SaveMaterials

	Int2Str(Type, (char*)&IDString[26], 3);
	Int2Str(NrVertices, (char*)&IDString[31], 6);
	Int2Str(NrFaces, (char*)&IDString[39], 6);
	Int2Str(NrMaterials, (char*)&IDString[47], 4);

	strcpy((char*)TextHeader_, IDString);

	unsigned int CheckSum = 0;
	unsigned char *P = (unsigned char*)OutLWO;
	for ( C = 0; C < FileHeader_->FileSize; C++ )
	{
		CheckSum += (unsigned int)P[C];
	}//for
	FileHeader_->CheckSum = CheckSum;

	return OutLWO;
}//SaveLWO



/*
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ WATCOM C++ ROUTINE ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
³Name         : AnalyzeLWO
³Last update  : 05.01.1998
³Action	      : Gets different info from the .lwo (faces, vertices etc.)
³
³In:	pointer to loaded .lwo-file
³
³Out:  Returns an evt. error (0 = error)
ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
int AnalyzeLWO(void *LWOFile)
{
	printf("Analyzing LWO...\n");

	int C = 0;
	unsigned char *LWOPointer;

//---- Check FORM -chunk
	if ( *(int*)LWOFile != 'MROF' )
	{
		printf("ERR: Couldn't find FORM chunk ");
		return 0;

	}//if
	printf("FORM chunk found\n");


//---- Check LWOB -chunk
	if ( *(int*)((int)LWOFile + 8) != 'BOWL')
	{
		printf("ERR: Couldn't find LWOB chunk ");
		return 0;
	}//if
	printf("LWOB chunk found\n");


//---- Counting chunks...
	int LWOSize = CInt((void*)((int)LWOFile + 4));
	int BytesRead = 4;
	LWOPointer = (unsigned char*)((int)LWOFile + 12);

	C = 0;
	do
	{
		Chunks[C].ID 		= *(int*)LWOPointer;
		Chunks[C].Offset 	= &LWOPointer[0] + 8;
		Chunks[C].Size 		= CInt(&LWOPointer[4]);

		BytesRead += CInt(&LWOPointer[4]) + 8;
		LWOPointer += CInt(&LWOPointer[4]) + 8;

		if ( C > MaxChunks )
		{
			printf("ERR: Too many chunks in file ");
			return 0;
		}

		C++;
	} while ( BytesRead < LWOSize);

	printf("%d additional chunks found\n", C);

	Chunks[C].ID = 0;


//---- Searching for PNTS-chunk...
	C = 0;
	while ( (Chunks[C].ID != 'STNP') && (Chunks[C].ID != 0) ) C++;

	if ( Chunks[C].ID == 0 )
	{
		printf("ERR: Couldn't find PNTS chunk ");
		return 0;
	}
	PNTS = C;

//---- Searching for POLS-chunk...
	C = 0;
	while ( (Chunks[C].ID != 'SLOP') && (Chunks[C].ID != 0) ) C++;

	if ( Chunks[C].ID == 0 )
	{
		printf("ERR: Couldn't find POLS chunk ");
		return 0;
	}
	POLS = C;

//---- Searching for SRFS-chunk...
	C = 0;
	while ( (Chunks[C].ID != 'SFRS') && (Chunks[C].ID != 0) ) C++;

	if ( Chunks[C].ID == 0 )
	{
		printf("ERR: Couldn't find SRFS chunk ");
		return 0;
	}
	SRFS = C;


//---- Setting Vertices, Faces, Materials...

	NrVertices = Chunks[PNTS].Size / 12;
	printf("%d vertices found\n", NrVertices);

	BytesRead = 0;
	NrFaces = 0;
	LWOPointer = (unsigned char*)Chunks[POLS].Offset;
	do
	{
		int Temp = CSInt(LWOPointer);
		if ( Temp >= 3 ) NrFaces += Temp - 2;

		BytesRead += CSInt(LWOPointer)*2 + 4;
		LWOPointer += CSInt(LWOPointer)*2 + 4;

	} while ( BytesRead < Chunks[POLS].Size );
	printf("%d faces (triangles) found\n", NrFaces);

	BytesRead = 0;
	NrMaterials = 0;
	LWOPointer = (unsigned char*)Chunks[SRFS].Offset;
	do
	{
		strcpy((char*)&Surfaces[NrMaterials].Name, (char*)LWOPointer);

		LWOPointer += strlen(Surfaces[NrMaterials].Name) + 1;
		BytesRead += strlen(Surfaces[NrMaterials].Name) + 1;

		if ( (strlen(Surfaces[NrMaterials].Name) & 1) == 0 )
		{
			LWOPointer++;
			BytesRead++;
		}
		NrMaterials++;
	} while ( BytesRead < Chunks[SRFS].Size );
	printf("%d materials found\n", NrMaterials);

	return 1;
}//AnalyzeLWO





/*
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ WATCOM C++ ROUTINE ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
³Name         : SavePNTS
³Last update  : 05.01.1998
³Action	      : Converts coordinates from an .lwo file
³
³In:	pointer to where to store the coordinates
³
³Out:  -
ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
void SavePNTS(Vertice *Vertices)
{
	printf("Converting %d vertices...\n", NrVertices);

	unsigned char *P = (unsigned char*)Chunks[PNTS].Offset;
	int Temp;

	for ( int C = 0; C < NrVertices; C++ )
	{
		Temp = CInt(&P[C*12 + 0]);
		Vertices[C].X = *(float*)&Temp;
		Temp = CInt(&P[C*12 + 4]);
		Vertices[C].Y = *(float*)&Temp;
		Temp = CInt(&P[C*12 + 8]);
		Vertices[C].Z = *(float*)&Temp;
	}//for

}//SavePNTS


/*
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ WATCOM C++ ROUTINE ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
³Name         : ReadMaterials
³Last update  : 05.01.1998
³Action	      : Reads all necessary data from material-subchunks...
³
³In:	-
³
³Out:  -
ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
void ReadMaterials(void)
{
	printf("Reading %d materials...\n", NrMaterials);

	int C1 = 0;
	int C2 = 0;
	int C3 = 0;
	while ( Chunks[C1].ID != 0 )
	{
		if ( Chunks[C1].ID == 'FRUS' )
		{
			C3 = 0;
		   	while ( strcmp((char*)Chunks[C1].Offset, Surfaces[C3].Name ) != 0) C3++;

			Surfaces[C3].Luminosity = 0;
			Surfaces[C3].Diffuse = 0;

			int BytesRead = strlen(Surfaces[C3].Name) + 1;
			if ( (BytesRead & 1) == 1) BytesRead++;

			unsigned char *P = (unsigned char*)Chunks[C1].Offset + BytesRead;
			do
			{
				switch ( *(int*)&P[0] )
				{
				//COLR: ground color of surface
					case 'RLOC':    Surfaces[C3].R = (float)P[6]/255;
							Surfaces[C3].G = (float)P[7]/255;
							Surfaces[C3].B = (float)P[8]/255;
							break;

				//VLUM: floating point luminosity
					case 'MULV':	C2 = CInt(&P[6]);
							Surfaces[C3].Luminosity = *(float*)&C2;
							break;
				//LUMI: fixed point luminosity
					case 'IMUL':    C2 = CSInt(&P[6]);
							Surfaces[C3].Luminosity = (float)C2/256;
							break;

				//VDIF: floating point diffuse
					case 'FIDV':	C2 = CInt(&P[6]);
							Surfaces[C3].Diffuse    = *(float*)&C2;
							break;
				//DIFF: fixed point diffuse
					case 'FFID':    C2 = CSInt(&P[6]);
							Surfaces[C3].Diffuse    = (float)C2/256;
							break;

				//VTRN: floating point transparency
					case 'NRTV':	C2 = CInt(&P[6]);
							Surfaces[C3].Transparency = *(float*)&C2;
							break;
				//TRAN: fixed point transparency
					case 'NART':    C2 = CSInt(&P[6]);
							Surfaces[C3].Transparency = (float)C2/256;
							break;

				//TIMG: textstring with filename texture
					case 'GMIT':    strcpy((char*)&Surfaces[C3].TextureFile, (char*)&P[6]);
							for ( C2 = 0; C2 < strlen(Surfaces[C3].TextureFile); C2++ )
								Surfaces[C3].TextureFile[C2] = toupper(Surfaces[C3].TextureFile[C2]);

							if ( NoPath == 1 )
							{
								int Slash = 0;
								for ( C2 = 0; C2 < strlen(Surfaces[C3].TextureFile); C2++ )
									if ( Surfaces[C3].TextureFile[C2] == '\\' ) Slash = C2;

								strcpy(Surfaces[C3].TextureFile, &Surfaces[C3].TextureFile[Slash + 1]);
							}//if

							break;

				//TSIZ: texture size
					case 'ZIST':    C2 = CInt(&P[6]);
							Surfaces[C3].XSize = *(float*)&C2;
							C2 = CInt(&P[10]);
							Surfaces[C3].YSize = *(float*)&C2;
							C2 = CInt(&P[14]);
							Surfaces[C3].ZSize = *(float*)&C2;
							break;

				//TCTR: texture center
					case 'RTCT':	C2 = CInt(&P[6]);
							Surfaces[C3].XCenter = *(float*)&C2;
							C2 = CInt(&P[10]);
							Surfaces[C3].YCenter = *(float*)&C2;
							C2 = CInt(&P[14]);
							Surfaces[C3].ZCenter = *(float*)&C2;
							break;

				//TFLG: texture flags
					case 'GLFT':    C2 = CSInt(&P[6]);
							Surfaces[C3].Axis = C2 & 7;
							break;

				//CTEX: texture mapping type
					case 'XETC':	Surfaces[C3].MapType = 0;
					       		if ( strcmp("Planar Image Map", (char*)&P[6]) == 0 )
					       		{
					       			Surfaces[C3].MapType = Flat;
								break;
							}//if
							if ( strcmp("Sylindrical Image Map", (char*)&P[6]) == 0 )
							{
								Surfaces[C3].MapType = Sylindrical;
								break;
							}//if

							if ( strcmp("Spherical Image Map", (char*)&P[6]) == 0 )
							{
								Surfaces[C3].MapType = Spherical;
								break;
							}//if

							printf("WARNING: Mapping type not supported: ");
							printf((char*)&P[6]);
							printf("\n");
							break;


					default:	break;
				} //switch

				BytesRead += CSInt(&P[4]) + 6;
				P += CSInt(&P[4]) + 6;
			} while ( BytesRead < Chunks[C1].Size );




		}//if
		C1++;
	}//while

}//ReadMaterials


/*
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ WATCOM C++ ROUTINE ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
³Name         : SavePOLS
³Last update  : 05.01.1998
³Action	      : Converts faces from an .lwo-file
³
³In:	pointer to where to store the faces
³
³Out:  -
ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
void SavePOLS(Face *Faces)
{
	printf("Converting %d faces...\n", NrFaces);

	int BytesRead = 0;
	int Face = 0;
	int FaceMaterial;
	int Vert0, Vert1, Vert2;
	float X0, Y0, Z0, X1, Y1, Z1, X2, Y2, Z2;
	float U0, V0, U1, V1, U2, V2;
	int C;
	unsigned char *P = (unsigned char*)Chunks[POLS].Offset;


	do
	{
		int PolyVerts = CSInt(&P[0]);
		FaceMaterial = CSInt(&P[PolyVerts*2 + 2]) - 1;

		Vert0 = CSInt(&P[2]);

		X0 = Vertices[Vert0].X - Surfaces[FaceMaterial].XCenter;
		Y0 = Vertices[Vert0].Y - Surfaces[FaceMaterial].YCenter;
		Z0 = Vertices[Vert0].Z - Surfaces[FaceMaterial].ZCenter;

		MapCoords(X0, Y0, Z0, U0, V0, FaceMaterial);

		C = PolyVerts;
		while ( C >= 3 )
		{
			Vert1 = CSInt(&P[(C - 2)*2 + 2]);
			Vert2 = CSInt(&P[(C - 1)*2 + 2]);

			Faces[Face].Vert0 = Vert0;
			Faces[Face].Vert1 = Vert1;
			Faces[Face].Vert2 = Vert2;
			Faces[Face].Material = FaceMaterial;


		//---- Calculating mappingcoords...
			X1 = Vertices[Vert1].X - Surfaces[FaceMaterial].XCenter;
			Y1 = Vertices[Vert1].Y - Surfaces[FaceMaterial].YCenter;
			Z1 = Vertices[Vert1].Z - Surfaces[FaceMaterial].ZCenter;
			MapCoords(X1, Y1, Z1, U1, V1, FaceMaterial);

			X2 = Vertices[Vert2].X - Surfaces[FaceMaterial].XCenter;
			Y2 = Vertices[Vert2].Y - Surfaces[FaceMaterial].YCenter;
			Z2 = Vertices[Vert2].Z - Surfaces[FaceMaterial].ZCenter;
			MapCoords(X2, Y2, Z2, U2, V2, FaceMaterial);

			int SmallestU = (int)U0;
			if ( (int)U1 < SmallestU ) SmallestU = (int)U1;
			if ( (int)U2 < SmallestU ) SmallestU = (int)U2;

			int SmallestV = (int)V0;
			if ( (int)V1 < SmallestV ) SmallestV = (int)V1;
			if ( (int)V2 < SmallestV ) SmallestV = (int)V2;

			float U0_ = U0 - SmallestU;
			float U1_ = U1 - SmallestU;
			float U2_ = U2 - SmallestU;
			float V0_ = V0 - SmallestV;
			float V1_ = V1 - SmallestV;
			float V2_ = V2 - SmallestV;

			Faces[Face].U0 = U0_;
			Faces[Face].V0 = V0_;
			Faces[Face].U1 = U1_;
			Faces[Face].V1 = V1_;
			Faces[Face].U2 = U2_;
			Faces[Face].V2 = V2_;

			C--;
			Face++;
		}//while

		BytesRead += PolyVerts*2 + 4;
		P += PolyVerts*2 + 4;
	} while ( BytesRead < Chunks[POLS].Size );

}//SavePOLS


/*
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ WATCOM C++ ROUTINE ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
³Name         : MapCoords
³Last update  : 05.01.1998
³Action	      : Calculates mapping coordinates
³
³In:	The point's X, Y, Z
³
³Out:   The calculated U and V
ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/

void MapCoords( float X,
		float Y,
		float Z,
		float &U,
		float &V,
		int   FaceMaterial)

{

	if ( Surfaces[FaceMaterial].MapType == Flat )
	{

		switch ( Surfaces[FaceMaterial].Axis )
		{
			case XAxis: 	U = (Z/Surfaces[FaceMaterial].ZSize) + 0.5;
					V = (-Y/Surfaces[FaceMaterial].YSize) + 0.5;
					break;
			case YAxis:	U = (X/Surfaces[FaceMaterial].XSize) + 0.5;
					V = (-Z/Surfaces[FaceMaterial].ZSize) + 0.5;
					break;
			case ZAxis:	U = (X/Surfaces[FaceMaterial].XSize) + 0.5;
					V = (-Y/Surfaces[FaceMaterial].YSize) + 0.5;
					break;
		}
		U = fabs(U);
		V = fabs(V);
		U = (int)(U*100 + 0.5);	//Rounding to two decimals
		V = (int)(V*100 + 0.5); //Rounding to two decimals
		U = U/100;
		V = V/100;

	}//if "Flat"
}//MapCoords

/*
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ WATCOM C++ ROUTINE ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
³Name         : SaveSURF
³Last update  : 05.01.1998
³Action	      : Converts faces from an .lwo-file
³
³In:	pointer to where to store the faces
³
³Out:  -
ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
void SaveSURF(Material *Materials)
{
	printf("Converting %d materials...\n", NrMaterials);

	MatrChunk->Size = 0;

	int C1 = 0;
	for ( C1 = 0; C1 < NrMaterials; C1++ )
	{
		Materials->ID = SpecID;
		Materials->R = Surfaces[C1].R;
		Materials->G = Surfaces[C1].G;
		Materials->B = Surfaces[C1].B;

		Materials->Luminosity = Surfaces[C1].Luminosity;
		Materials->Diffuse = Surfaces[C1].Diffuse;
		Materials->Transparency = Surfaces[C1].Transparency;

		Materials->TextureFOffset = 0;
		Materials->TextureFSize = 0;
		strcpy(Materials->TextureFile, Surfaces[C1].TextureFile);

		Materials->Size = sizeof(Material) - 8 + strlen(Materials->TextureFile) + 1;
		MatrChunk->Size += Materials->Size + 8;
		*(int*)&Materials += Materials->Size + 8;
	}//for
}//SaveSURF

