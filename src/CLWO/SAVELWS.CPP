#include <stdio.h>
#include <string.h>
#include "div.h"
#include <fstream.h>
#include "savelwo.h"

const int MaxObjects	= 150;
const int MaxKeyFrames  = 200;
const int MaxLights	= 100;

typedef struct
{
	int	ID;
	int	Size;
	int	Version;
	int	FileSize;
	int	CheckSum;
	int	NrObjects;
	int	NrLights;
	int	FirstFrame;
	int	LastFrame;
	float	FramesPerSecond;

}_Header;

_Header Header;

typedef struct
{
	int	Frame;
	float	Continuity, Tension, Bias;
	int     Linear;
	float	XPos, YPos, ZPos;
	float	XAng, YAng, ZAng;
}KeyFrame;

typedef struct
{
	int	Frame;
	float	Continuity, Tension, Bias;
	int     Linear;
	float	Value;
}KeyFrame2;

typedef struct
{
	int	NrKeyFrames;
	int	EndBehavior;
	KeyFrame2 KeyFrames[MaxKeyFrames];
}Envelope;


typedef struct
{
	int	ID;
	int	Size;
	float	XPivot, YPivot, ZPivot;         //pivotpoint
	int	EndBehavior;			//
	int	ParentObject;			//-1 = no parent object
	int	PathAlignment;			//
	int	MorphTarget;
	float	Metamorph;
	float	Dissolve;
	int	NrKeyFrames;			//number of keyframes
	KeyFrame KeyFrames[MaxKeyFrames];	//keyframedata
	int	FileOffset;
	char 	Path[256];			//filename
}ObjectData;

typedef struct
{
	int	ID;
	int	Size;
	int	Type;
	float	R, G, B;
	float	Range, Intensity;
	float	ConeAngle, EdgeAngle;
	int	EndBehavior;
	int	ParentObject;
	int	PathAlignment;
	int	NrKeyFrames;
	KeyFrame KeyFrames[MaxKeyFrames];

	Envelope IntensityEnv;
	Envelope RangeEnv;
	Envelope ConeAngleEnv;
	Envelope EdgeAngleEnv;
}LightData;

typedef struct
{
	int	ID;
	int	Size;
	int	EndBehavior;
	int	ParentObject;
	int	PathAlignment;
	int	TargetObject;
	int	NrKeyFrames;
	KeyFrame KeyFrames[MaxKeyFrames];
}CameraData;


ifstream InFile;
char InLine[256];
int Active = 0;		//0 = object, 1 = light, 2 = camera
int Valid = 0;

void 	F_LWSC(void);
void 	F_FirstFrame(void);
void 	F_LastFrame(void);
void 	F_FramesPerSecond(void);
int  	F_LoadObject(void);
int  	F_ObjectMotion(void);
void 	F_EndBehavior(void);
void 	F_ParentObject(void);
int 	F_AddLight(void);
void 	F_PivotPoint(void);
int  	F_LightMotion(void);
void 	F_LightColor(void);
int  	F_LgtIntensity(void);
void 	F_LightType(void);
int  	F_LightRange(void);
int     F_ConeAngle(void);
int 	F_EdgeAngle(void);
void 	F_ShowCamera(void);
void	F_TargetObject(void);
int  	F_CameraMotion(void);
void 	F_LookAheadFrames(void);
int 	F_AddNullObject(void);
void	F_MorphTarget(void);
void	F_Metamorph(void);
void	F_ObjDissolve(void);



int ReadEnvelope(Envelope &Env);

char TxtHeader[256];


ObjectData 	*Objects;
LightData  	*Lights;
CameraData	*Camera;
int NrObjects 	= -1;
int NrLights  	= -1;


void *SaveLWS(const char *FileName)
{
	int C1, C2;

//---- Open file...
	FILE *TempFile;
	TempFile = fopen(FileName, "rb");
	if (TempFile == NULL)
	{
		printf("ERR: Error opening file ");
		return 0;
	}

//---- Getting filesize and allocating mem...
	fpos_t _FSize;
	fseek(TempFile, 0, SEEK_END);
	fgetpos(TempFile, &_FSize);
	int FSize = (int)_FSize;

	char *Temp = new char[FSize];

	if ( Temp == NULL )
	{
		printf("ERR: Error allocating memory ");
		fclose(TempFile);
		return 0;
	}


//---- Read entire file into memory...
	fseek(TempFile, 0, SEEK_SET);
	if (fread(Temp, FSize, 1, TempFile) < 1)
	{
		printf("ERR: Error reading file ");
		delete [] Temp;
		fclose(TempFile);
		return 0;
	}

	fclose(TempFile);

	for ( C1 = 0; C1 < FSize; C1++ )
		if ( Temp[C1] == ',') Temp[C1] = '.';



//---- Saving file...
	TempFile = fopen("temp.lws", "wb");
	if (TempFile == NULL)
	{
		printf("ERR: Error opening file (temp.lws)\n");
		delete [] Temp;
		return 0;
	}

	if (fwrite(Temp, FSize, 1, TempFile) < 1)
	{
		printf("ERR: Error writing file (temp.lws)\n");
		delete [] Temp;
		fclose(TempFile);
		return 0;
	}

	delete [] Temp;
	fclose(TempFile);


//---- Setting dif. initial values...
	Objects = new ObjectData[MaxObjects];
	Lights  = new LightData[MaxLights];
	Camera  = new CameraData;
	for ( C1 = 0; C1 < MaxObjects; C1++ )
	{
		Objects[C1].ID = 'JBOV';
		Objects[C1].FileOffset = 0;
		Objects[C1].ParentObject = -1;
		Objects[C1].PathAlignment = 0;
		Objects[C1].MorphTarget = -1;
		Objects[C1].Metamorph = 0.0;
		Objects[C1].Dissolve = 0.0;
		Objects[C1].XPivot = 0;
		Objects[C1].YPivot = 0;
		Objects[C1].ZPivot = 0;
	}//for

	for ( C1 = 0; C1 < MaxLights; C1++ )
	{
		Lights[C1].ID = 'THGL';
		Lights[C1].ParentObject = -1;
		Lights[C1].PathAlignment = 0;
		Lights[C1].Range = 1e20;
		Lights[C1].ConeAngle = 0;
		Lights[C1].EdgeAngle = 0;
		Lights[C1].Intensity = 0;

		Lights[C1].IntensityEnv.NrKeyFrames = 0;
		Lights[C1].RangeEnv.NrKeyFrames = 0;
		Lights[C1].ConeAngleEnv.NrKeyFrames = 0;
		Lights[C1].EdgeAngleEnv.NrKeyFrames = 0;
	}//for

	Camera->ID = 'RMAC';
	Camera->PathAlignment = 0;
	Camera->ParentObject = -1;
	Camera->TargetObject = -1;



//---- Opening file...
	InFile.open("temp.lws", ios::nocreate | ios::in);
	if ( InFile.fail() )
	{
		printf("ERR: Couldn't open tempfile ");
		delete [] Objects;
		delete [] Lights;
		delete [] Camera;
		return 0;
	}//if


//---- Reading file...
	while ( !InFile.eof() )
	{
		InFile >> InLine;
		if ( strcmp(InLine, "LWSC") 		== 0 ) F_LWSC();
		if ( strcmp(InLine, "FirstFrame") 	== 0 ) F_FirstFrame();
		if ( strcmp(InLine, "LastFrame") 	== 0 ) F_LastFrame();
		if ( strcmp(InLine, "FramesPerSecond") 	== 0 ) F_FramesPerSecond();
		if ( strcmp(InLine, "LoadObject" ) 	== 0 ) if (F_LoadObject() == 0)
		{
			delete [] Objects;
			delete [] Lights;
			delete [] Camera;
			return 0;
		}//if
		if ( strcmp(InLine, "ObjectMotion")	== 0 ) if (F_ObjectMotion() == 0)
		{
			delete [] Objects;
			delete [] Lights;
			delete [] Camera;
			return 0;
		}//if
		if ( strcmp(InLine, "EndBehavior") 	== 0 ) F_EndBehavior();
		if ( strcmp(InLine, "ParentObject") 	== 0 ) F_ParentObject();
		if ( strcmp(InLine, "AddLight") 	== 0 ) if (F_AddLight() == 0)
		{
			delete [] Objects;
			delete [] Lights;
			delete [] Camera;
			return 0;
		}//if

		if ( strcmp(InLine, "PivotPoint") 	== 0 ) F_PivotPoint();
		if ( strcmp(InLine, "LightMotion") 	== 0 ) if (F_LightMotion() == 0)
		{
			delete [] Objects;
			delete [] Lights;
			delete [] Camera;
			return 0;
		}//if
		if ( strcmp(InLine, "LightColor")	== 0 ) F_LightColor();
		if ( strcmp(InLine, "LgtIntensity") 	== 0 ) if (F_LgtIntensity() == 0)
		{
			delete [] Objects;
			delete [] Lights;
			delete [] Camera;
			return 0;
		}//if
		if ( strcmp(InLine, "LightRange" ) 	== 0 ) if (F_LightRange() == 0)
		{
			delete [] Objects;
			delete [] Lights;
			delete [] Camera;
			return 0;
		}//if

		if ( strcmp(InLine, "LightType") 	== 0 ) F_LightType();
		if ( strcmp(InLine, "ConeAngle" ) 	== 0 ) if (F_ConeAngle() == 0)
		{
			delete [] Objects;
			delete [] Lights;
			delete [] Camera;
			return 0;
		}//if

		if ( strcmp(InLine, "EdgeAngle" ) 	== 0 ) if (F_EdgeAngle() == 0)
		{
			delete [] Objects;
			delete [] Lights;
			delete [] Camera;
			return 0;
		}//if

		if ( strcmp(InLine, "ShowCamera") 	== 0 ) F_ShowCamera();
		if ( strcmp(InLine, "TargetObject" ) 	== 0 ) F_TargetObject();
		if ( strcmp(InLine, "CameraMotion") 	== 0 ) if (F_CameraMotion() == 0)
		{
			delete [] Objects;
			delete [] Lights;
			delete [] Camera;
			return 0;
		}//if
		if ( strcmp(InLine, "LookAheadFrames")  == 0 ) F_LookAheadFrames();
		if ( strcmp(InLine, "AddNullObject" ) 	== 0 ) if ( F_AddNullObject() == 0)
		{
			delete [] Objects;
			delete [] Lights;
			delete [] Camera;
			return 0;
		}//if
		if ( strcmp(InLine, "MorphTarget") 	== 0 ) F_MorphTarget();
		if ( strcmp(InLine, "Metamorph") 	== 0 ) F_Metamorph();
		if ( strcmp(InLine, "ObjDissolve" )	== 0 ) F_ObjDissolve();


	}

	InFile.close();

	if ( Valid == 0 )
	{
		printf("ERR: Not a valid .lws-file ");
		delete [] Objects;
		delete [] Lights;
		delete [] Camera;
		return 0;
	}//if


//---- Morphing metamorph-objects...
	__asm int 3
	char *Original;
	char *Target;
	for ( C1 = 0; C1 <= NrObjects; C1++ )
	{
		if ( Objects[C1].MorphTarget != -1 )
		{

			Original = (char*)ReadFile(Objects[C1].Path);
			if ( Original == 0 )
			{
				printf("ERR: Not enough memory for morphing.\n");
				delete [] Objects;
				delete [] Lights;
				delete [] Camera;

				return 0;
			}//if

			Target = (char*)ReadFile(Objects[Objects[C1].MorphTarget].Path);
			if ( Original == 0 )
			{
				printf("ERR: Not enough memory for morphing.\n");
				delete [] Objects;
				delete [] Lights;
				delete [] Camera;
				delete [] Original;

				return 0;
			}//if


			float *OriginalVerts;
			float *TargetVerts;

			int MemPointer = 66;
			while ( *(int*)&Original[MemPointer] != 'TREV' )
				MemPointer += *(int*)&Original[MemPointer + 4] + 8;
			OriginalVerts = (float*)&Original[MemPointer + 8];

			int MorphFloats = *(int*)&Original[MemPointer + 4] / 4; //Number of floats

			MemPointer = 66;
			while ( *(int*)&Target[MemPointer] != 'TREV' )
				MemPointer += *(int*)&Target[MemPointer + 4] + 8;
			TargetVerts = (float*)&Target[MemPointer + 8];

			for ( C2 = 0; C2 < MorphFloats; C2++ )
				OriginalVerts[C2] = (TargetVerts[C2] - OriginalVerts[C2])*Objects[C1].Metamorph + OriginalVerts[C2];


		//---- Creating new checksum...
			FileHeader *MorphHeader = (FileHeader*)&Original[74];
			MorphHeader->CheckSum = 0;

			unsigned int MorphCheckSum = 0;
			for ( MemPointer = 0; MemPointer < MorphHeader->FileSize; MemPointer++ )
				MorphCheckSum += (unsigned int)*(unsigned char*)&Original[MemPointer];
			MorphHeader->CheckSum = MorphCheckSum;



		//---- Saving file...
			FILE *MorphFile;
			MorphFile = fopen(Objects[C1].Path, "wb");
			if (MorphFile == NULL)
			{
				printf("ERR: Error opening file for morphing\n");

				delete [] Original;
				delete [] Target;
				delete [] Objects;
				delete [] Lights;
				delete [] Camera;

				return 0;
			}

			if (fwrite(Original, MorphHeader->FileSize, 1, MorphFile) < 1)
			{
				printf("ERR: Error writing morphed object\n");

				delete [] Original;
				delete [] Target;
				delete [] Objects;
				delete [] Lights;
				delete [] Camera;

				fclose(MorphFile);
				return 0;
			}

			delete [] Original;
			delete [] Target;
			fclose(MorphFile);

		}
	}//for

	for ( C1 = 0; C1 <= NrObjects; C1++ )
       		if ( Objects[C1].Dissolve == 1.0 )
		{
       			strcpy(Objects[C1].Path, "NullObject");
			Objects[C1].FileOffset = -1;
		}//if





//---- Allocating final memoryblock...
	int MemReq = 0;
	MemReq += 256;			//Text-Header
	MemReq += sizeof(Header);	//SCEN-header

					//Adding objects...
	for ( C1 = 0; C1 <= NrObjects; C1++ )
	{
		Objects[C1].Size = 0;

		Objects[C1].Size += 8*4;				//div. stuff
		Objects[C1].Size += Objects[C1].NrKeyFrames * sizeof(KeyFrame);
		Objects[C1].Size += strlen(Objects[C1].Path) + 1; 	//obj-path
		MemReq += Objects[C1].Size + 8;
	}//for

					//Adding lights...
	for ( C1 = 0; C1 <= NrLights; C1++ )
	{
		Lights[C1].Size = 0;
		Lights[C1].Size += 12*4;				//div. stuff
		Lights[C1].Size += Lights[C1].NrKeyFrames * sizeof(KeyFrame);
		MemReq += Lights[C1].Size + 8;

	//---- Adding lights' plugins...
		if ( Lights[C1].IntensityEnv.NrKeyFrames != 0 )
			MemReq += + 8 	//Chunkheader
				  + 8	//NrKeyFrames and EndBehavior
				  + Lights[C1].IntensityEnv.NrKeyFrames*sizeof(KeyFrame2);

		if ( Lights[C1].RangeEnv.NrKeyFrames != 0 )
			MemReq += + 8 	//Chunkheader
				  + 8	//NrKeyFrames and EndBehavior
				  + Lights[C1].RangeEnv.NrKeyFrames*sizeof(KeyFrame2);

		if ( Lights[C1].ConeAngleEnv.NrKeyFrames != 0 )
			MemReq += + 8 	//Chunkheader
				  + 8	//NrKeyFrames and EndBehavior
				  + Lights[C1].ConeAngleEnv.NrKeyFrames*sizeof(KeyFrame2);

		if ( Lights[C1].EdgeAngleEnv.NrKeyFrames != 0 )
			MemReq += + 8 	//Chunkheader
				  + 8	//NrKeyFrames and EndBehavior
				  + Lights[C1].EdgeAngleEnv.NrKeyFrames*sizeof(KeyFrame2);


	}//for

	Camera->Size = Camera->NrKeyFrames*sizeof(KeyFrame) + 5*4;
	MemReq += Camera->Size + 8;

	int P = 0;
	unsigned char *Final = new unsigned char[MemReq];
	for ( C1 = 0; C1 < MemReq; C1++ ) Final[C1] = 0;

//---- Setting up final scene-data...
	strcpy(TxtHeader,"CLWS v1.00\nCyberfish/TLS\n\nObjects    =xxxxd\nLights     =xxxxd\nFirstFrame =xxxxd\nLastFrame  =xxxxxxd\n");
	Int2Str(NrObjects + 1, &TxtHeader[38], 4);
	Int2Str(NrLights + 1, &TxtHeader[56], 4);
	Int2Str(Header.FirstFrame, &TxtHeader[74], 4);
	Int2Str(Header.LastFrame, &TxtHeader[92], 6);

	strcpy((char*)&Final[0], TxtHeader);
	Final[strlen(TxtHeader)] = 0x1A;
	P = 256 + sizeof(Header);

	for ( C1 = 0; C1 <= NrObjects; C1++ )	//Copying objects...
	{
		*(int*)&Final[P + 0] 	= Objects[C1].ID;
		*(int*)&Final[P + 4] 	= Objects[C1].Size;
		*(float*)&Final[P + 8]	= Objects[C1].XPivot;
		*(float*)&Final[P + 12]	= Objects[C1].YPivot;
		*(float*)&Final[P + 16]	= Objects[C1].ZPivot;
		*(int*)&Final[P + 20] 	= Objects[C1].EndBehavior;
		*(int*)&Final[P + 24] 	= Objects[C1].ParentObject;
		*(int*)&Final[P + 28] 	= Objects[C1].PathAlignment;
		*(int*)&Final[P + 32] 	= Objects[C1].NrKeyFrames;
		P += 36;
		for ( C2 = 0; C2 < Objects[C1].NrKeyFrames; C2++ )
		{
			*(KeyFrame*)&Final[P] = Objects[C1].KeyFrames[C2];
			P += sizeof(KeyFrame);
		}//for
		*(int*)&Final[P] = Objects[C1].FileOffset;
		strcpy((char*)&Final[P + 4], Objects[C1].Path);
		P += strlen(Objects[C1].Path) + 5;


	}//for

	for ( C1 = 0; C1 <= NrLights; C1++ )	//Copying lights...
	{
		*(int*)&Final[P + 0] 	= Lights[C1].ID;
		*(int*)&Final[P + 4] 	= Lights[C1].Size;
		*(int*)&Final[P + 8]	= Lights[C1].Type;
		*(float*)&Final[P + 12]	= Lights[C1].R;
		*(float*)&Final[P + 16]	= Lights[C1].G;
		*(float*)&Final[P + 20]	= Lights[C1].B;
		*(float*)&Final[P + 24] = Lights[C1].Range;
		*(float*)&Final[P + 28] = Lights[C1].Intensity;
		*(float*)&Final[P + 32] = Lights[C1].ConeAngle;
		*(float*)&Final[P + 36] = Lights[C1].EdgeAngle;
		*(int*)&Final[P + 40] 	= Lights[C1].EndBehavior;
		*(int*)&Final[P + 44] 	= Lights[C1].ParentObject;
		*(int*)&Final[P + 48] 	= Lights[C1].PathAlignment;
		*(int*)&Final[P + 52] 	= Lights[C1].NrKeyFrames;
		P += 14*4;

		for ( C2 = 0; C2 < Lights[C1].NrKeyFrames; C2++ )
		{
			*(KeyFrame*)&Final[P] = Lights[C1].KeyFrames[C2];
			P += sizeof(KeyFrame);
		}//for

		if ( Lights[C1].IntensityEnv.NrKeyFrames != 0 )
		{
			*(int*)&Final[P +  0] = 'TNIL';
			*(int*)&Final[P +  4] = 8 + Lights[C1].IntensityEnv.NrKeyFrames*sizeof(KeyFrame2);
			*(int*)&Final[P +  8] = Lights[C1].IntensityEnv.EndBehavior;
			*(int*)&Final[P + 12] = Lights[C1].IntensityEnv.NrKeyFrames;

			P += 4*4;

			for ( C2 = 0; C2 < Lights[C1].IntensityEnv.NrKeyFrames; C2++  )
			{
				*(KeyFrame2*)&Final[P] = Lights[C1].IntensityEnv.KeyFrames[C2];
				P += sizeof(KeyFrame2);
			}//for
		}//if

		if ( Lights[C1].RangeEnv.NrKeyFrames != 0 )
		{
			*(int*)&Final[P +  0] = 'GNRL';
			*(int*)&Final[P +  4] = 8 + Lights[C1].RangeEnv.NrKeyFrames*sizeof(KeyFrame2);
			*(int*)&Final[P +  8] = Lights[C1].RangeEnv.EndBehavior;
			*(int*)&Final[P + 12] = Lights[C1].RangeEnv.NrKeyFrames;

			P += 4*4;

			for ( C2 = 0; C2 < Lights[C1].RangeEnv.NrKeyFrames; C2++  )
			{
				*(KeyFrame2*)&Final[P] = Lights[C1].RangeEnv.KeyFrames[C2];
				P += sizeof(KeyFrame2);
			}//for
		}//if

		if ( Lights[C1].ConeAngleEnv.NrKeyFrames != 0 )
		{
			*(int*)&Final[P +  0] = 'NOCL';
			*(int*)&Final[P +  4] = 8 + Lights[C1].ConeAngleEnv.NrKeyFrames*sizeof(KeyFrame2);
			*(int*)&Final[P +  8] = Lights[C1].ConeAngleEnv.EndBehavior;
			*(int*)&Final[P + 12] = Lights[C1].ConeAngleEnv.NrKeyFrames;

			P += 4*4;

			for ( C2 = 0; C2 < Lights[C1].ConeAngleEnv.NrKeyFrames; C2++  )
			{
				*(KeyFrame2*)&Final[P] = Lights[C1].ConeAngleEnv.KeyFrames[C2];
				P += sizeof(KeyFrame2);
			}//for
		}//if

		if ( Lights[C1].EdgeAngleEnv.NrKeyFrames != 0 )
		{
			*(int*)&Final[P +  0] = 'GDEL';
			*(int*)&Final[P +  4] = 8 + Lights[C1].EdgeAngleEnv.NrKeyFrames*sizeof(KeyFrame2);
			*(int*)&Final[P +  8] = Lights[C1].EdgeAngleEnv.EndBehavior;
			*(int*)&Final[P + 12] = Lights[C1].EdgeAngleEnv.NrKeyFrames;

			P += 4*4;

			for ( C2 = 0; C2 < Lights[C1].EdgeAngleEnv.NrKeyFrames; C2++  )
			{
				*(KeyFrame2*)&Final[P] = Lights[C1].EdgeAngleEnv.KeyFrames[C2];
				P += sizeof(KeyFrame2);
			}//for
		}//if
	}//for


						//Copying camera...
	*(int*)&Final[P + 0] 	= Camera->ID;
	*(int*)&Final[P + 4] 	= Camera->Size;
	*(int*)&Final[P + 8] 	= Camera->EndBehavior;
	*(int*)&Final[P + 12] 	= Camera->ParentObject;
	*(int*)&Final[P + 16] 	= Camera->PathAlignment;
	*(int*)&Final[P + 20] 	= Camera->TargetObject;
	*(int*)&Final[P + 24] 	= Camera->NrKeyFrames;
	P += 28;
	for ( C1 = 0; C1 < Camera->NrKeyFrames; C1++ )
	{
		*(KeyFrame*)&Final[P] = Camera->KeyFrames[C1];
		P += sizeof(KeyFrame);
	}//for

	Header.ID		= 'NECS';
	Header.Size		= sizeof(Header) - 8;
	Header.FileSize 	= MemReq;
	Header.CheckSum		= 0;
	Header.NrObjects	= NrObjects + 1;
	Header.NrLights		= NrLights + 1;
	Header.Version		= 100;

	*(_Header*)&Final[256] 	= Header;	//Copying header

						//Calculating checksum...
	unsigned int CheckSum = 0;
	for ( P = 0; P < Header.FileSize; P++ )
	{
		CheckSum += (unsigned int)Final[P];
	}//for
	Header.CheckSum = CheckSum;

	*(_Header*)&Final[256] 	= Header;	//Copying header




	delete [] Objects;
	delete [] Lights;
	delete [] Camera;
	return Final;
}//SaveLWS




//----------------- Function "LWSC"

void F_LWSC(void)
{
	Valid = 1;
}//F_LWSC






//----------------- Function "FirstFrame"
void F_FirstFrame(void)
{
	InFile >> Header.FirstFrame;
}//F_FirstFrame;


//----------------- Function "LastFrame"
void F_LastFrame(void)
{
	InFile >> Header.LastFrame;
}//F_LawsFrame;


//----------------- Function "FramesPerSecond"
void F_FramesPerSecond(void)
{
	InFile >> Header.FramesPerSecond;
}//F_FrameStep;





//----------------- Function "LoadObject"
int F_LoadObject(void)
{

	char ObjInFile[256] = "\0";
	char ObjOutFile[256] = "\0";

	Active = 0;
	NrObjects++;
	if ( NrObjects == MaxObjects )
	{
		printf("ERR: Too many objects in .lws-file");
		return 0;
	}
	InFile >> ObjInFile;


//---- Converting object...
	ClrScr(7, 0);
	printf("LWO-converter v2.00 Cyberfish/TLS\n");
	printf("컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�\n");
	printf("Converting ");
	printf(ObjInFile);
	printf("...\n\n");


	if ( SearchStr(ObjInFile, '.') == 0) *(int*)&ObjInFile[strlen(ObjInFile)] = 'OWL.';
	if ( ObjOutFile[0] == '\0' )
	{
		strcpy(ObjOutFile, ObjInFile);
		*(int*)&ObjOutFile[strlen(ObjInFile) - 4] = 'NIB.';
	}//if

	strcpy(Objects[NrObjects].Path, ObjOutFile);

						//Getting objecttype...
	int Pos = 0;
	for ( int C1 = 0; C1 < strlen(ObjInFile); C1++ )
		if ( ObjInFile[C1] == '_' ) Pos = C1;
	if ( Pos == 0 )
	{
		printf("ERR: Filename doesn't define objecttype\n");
		return 0;
	}//if

	int Type;
	if ( Str2Int((char*)&ObjInFile[Pos + 1], Type, '.') != 0 )
	{
		printf("ERR: Unable converting number for objecttype\n");
		return 0;
	}//if


	void *LWO;
	LWO = ReadFile(ObjInFile);
	if ( LWO == 0 )
	{
		printf("(");
		printf(ObjInFile);
		printf(")\n");
		return 0;
	}//if

	unsigned char *Bin;
	Bin = (unsigned char*)SaveLWO(LWO, Type);

	delete [] LWO;

	if ( Bin == 0 )
	{
		printf("(");
		printf(ObjInFile);
		printf(")\n");
		return 0;
	}//if

//---- Saving file...
	FILE *BINFile;
	BINFile = fopen(ObjOutFile, "wb");
	if (BINFile == NULL)
	{
		printf("ERR: Error opening file (");
		printf(ObjOutFile);
		printf(")\n");
		delete [] Bin;
		return 0;
	}

	if (fwrite(Bin, ((FileHeader*)&Bin[74])->FileSize, 1, BINFile) < 1)
	{
		printf("ERR: Error writing file (");
		printf(ObjOutFile);
		printf(")\n");
		delete [] Bin;
		fclose(BINFile);
		return 0;
	}

	delete [] Bin;
	fclose(BINFile);

	return 1;
}//F_LoadObject




//----------------- Function "ObjectMotion"
int F_ObjectMotion(void)
{
	InFile >> InLine;			//Dump (identifier)
	InFile >> InLine;      			//Dump number of id-channels

						//Reading NrKeyFrames
	int NrKeyFrames;
	InFile >> NrKeyFrames;
	if (NrKeyFrames > MaxKeyFrames)
	{
		printf("ERR: Too many keyframes ");
		return 0;
	}//if

	Objects[NrObjects].NrKeyFrames = NrKeyFrames;

						//Reading keyframedata...
	for ( int C = 0; C < NrKeyFrames; C++ )
	{
		InFile >> Objects[NrObjects].KeyFrames[C].XPos;
		InFile >> Objects[NrObjects].KeyFrames[C].YPos;
		InFile >> Objects[NrObjects].KeyFrames[C].ZPos;

		InFile >> Objects[NrObjects].KeyFrames[C].YAng;
		InFile >> Objects[NrObjects].KeyFrames[C].XAng;
		InFile >> Objects[NrObjects].KeyFrames[C].ZAng;

		Objects[NrObjects].KeyFrames[C].XAng = (Objects[NrObjects].KeyFrames[C].XAng/360)*2*3.1459;
		Objects[NrObjects].KeyFrames[C].YAng = (Objects[NrObjects].KeyFrames[C].YAng/360)*2*3.1459;
		Objects[NrObjects].KeyFrames[C].ZAng = (Objects[NrObjects].KeyFrames[C].ZAng/360)*2*3.1459;

		InFile >> InLine;        //Dump scaling-values
		InFile >> InLine;
		InFile >> InLine;

		InFile >> Objects[NrObjects].KeyFrames[C].Frame;
		InFile >> Objects[NrObjects].KeyFrames[C].Linear;

		InFile >> Objects[NrObjects].KeyFrames[C].Tension;
		InFile >> Objects[NrObjects].KeyFrames[C].Continuity;
		InFile >> Objects[NrObjects].KeyFrames[C].Bias;

	}//for
	return 1;
}//F_ObjectMotion




//----------------- Function "EndBehavior"
void F_EndBehavior(void)
{
	int EndBehavior;
	InFile >> EndBehavior;
	switch ( Active )
	{
		case 0: Objects[NrObjects].EndBehavior = EndBehavior;
			break;
		case 1: Lights[NrLights].EndBehavior = EndBehavior;
			break;
		case 2: Camera->EndBehavior = EndBehavior;
			break;

	}

}//F_EndBavior



//----------------- Function "ParentObject"
void F_ParentObject(void)
{
	int ParentObject;
	InFile >> ParentObject;

	switch ( Active )
	{
		case 0: Objects[NrObjects].ParentObject = ParentObject - 1;
			break;
		case 1: Lights[NrLights].ParentObject = ParentObject - 1;
			break;
		case 2: Camera->ParentObject = ParentObject - 1;
			break;


	}
}//F_ParentObject




//----------------- Function "MorphTarget"
void F_MorphTarget(void)
{
	InFile >> Objects[NrObjects].MorphTarget;
	Objects[NrObjects].MorphTarget--;
}//F_MorphTarget




//----------------- Function "Metamorph"
void F_Metamorph(void)
{
	InFile >> Objects[NrObjects].Metamorph;
}//F_MetaMorph


void	F_ObjDissolve(void)
{
	InFile >> Objects[NrObjects].Dissolve;
}//F_ObjDissolve





//------------------ Function "AddLight"
int F_AddLight(void)
{
	Active = 1;
	NrLights++;
	if ( NrLights == MaxLights )
	{
		printf("ERR: Too many lights in .lws-file");
		return 0;
	}
	return 1;
}//F_AddLight



//------------------ Function "PivotPoint"
void F_PivotPoint(void)
{
	InFile >> Objects[NrObjects].XPivot;
	InFile >> Objects[NrObjects].YPivot;
	InFile >> Objects[NrObjects].ZPivot;
}//F_PivotPoint



//------------------ Function "LightMotion"
int F_LightMotion(void)
{
	InFile >> InLine;		//Dump <identifer>
	InFile >> InLine;		//Dump "number of id-channels"

	int NrKeyFrames;
	InFile >> NrKeyFrames;
	if ( NrKeyFrames > MaxKeyFrames )
	{
		printf("ERR: Too many keyframes ");
		return 0;
	}//if

	Lights[NrLights].NrKeyFrames = NrKeyFrames;

						//Reading keyframedata...
	for ( int C = 0; C < NrKeyFrames; C++ )
	{
		InFile >> Lights[NrLights].KeyFrames[C].XPos;
		InFile >> Lights[NrLights].KeyFrames[C].YPos;
		InFile >> Lights[NrLights].KeyFrames[C].ZPos;

		InFile >> Lights[NrLights].KeyFrames[C].YAng;
		InFile >> Lights[NrLights].KeyFrames[C].XAng;
		InFile >> Lights[NrLights].KeyFrames[C].ZAng;

		Lights[NrLights].KeyFrames[C].XAng = (Lights[NrLights].KeyFrames[C].XAng/360)*2*3.1459;
		Lights[NrLights].KeyFrames[C].YAng = (Lights[NrLights].KeyFrames[C].YAng/360)*2*3.1459;
		Lights[NrLights].KeyFrames[C].ZAng = (Lights[NrLights].KeyFrames[C].ZAng/360)*2*3.1459;

		InFile >> InLine >> InLine >> InLine;	//Dump scaling-values

		InFile >> Lights[NrLights].KeyFrames[C].Frame;
		InFile >> Lights[NrLights].KeyFrames[C].Linear;

		InFile >> Lights[NrLights].KeyFrames[C].Tension;
		InFile >> Lights[NrLights].KeyFrames[C].Continuity;
		InFile >> Lights[NrLights].KeyFrames[C].Bias;

	}//for

	return 1;
}//F_LightMotion




//------------------ Function "LightColor"
void F_LightColor(void)
{
	int R, G, B;
	InFile >> R >> G >> B;
	Lights[NrLights].R = (float)R/255;
	Lights[NrLights].G = (float)G/255;
	Lights[NrLights].B = (float)B/255;
}//F_LightColor




//------------------ Function "LgtIntensity"
int F_LgtIntensity(void)
{
	char Temp[20];
	InFile >> Temp;
	if ( strcmp(Temp, "(envelope)") == 0 )
	{
		if ( ReadEnvelope(Lights[NrLights].IntensityEnv) == 0 ) return 0;
	}
	else Str2Float(Temp, Lights[NrLights].Intensity, '\0');
	return 1;
}//F_LgtIntensity



//------------------ Function "LightRange"
int F_LightRange(void)
{
	char Temp[20];
	InFile >> Temp;
	if ( strcmp(Temp, "(envelope)") == 0 )
	{
		if ( ReadEnvelope(Lights[NrLights].RangeEnv) == 0 ) return 0;
	}
	else Str2Float(Temp, Lights[NrLights].Range, '\0');
	return 1;
}//F_LightRange



//------------------ Function "LightType"
void F_LightType(void)
{
	InFile >> Lights[NrLights].Type;
}//F_LightType



//------------------ Function "ConeAngle"
int F_ConeAngle(void)
{
	char Temp[20];
	InFile >> Temp;
	if ( strcmp(Temp, "(envelope)") == 0 )
	{
		if ( ReadEnvelope(Lights[NrLights].ConeAngleEnv) == 0 ) return 0;
	}
	else Str2Float(Temp, Lights[NrLights].ConeAngle, '\0');


//---- Converting angles to radians...
	Lights[NrLights].ConeAngle = (Lights[NrLights].ConeAngle/360)*2*3.1459;
	for ( int C = 0; C < Lights[NrLights].ConeAngleEnv.NrKeyFrames; C++ )
		Lights[NrLights].ConeAngleEnv.KeyFrames[C].Value =
			(Lights[NrLights].ConeAngleEnv.KeyFrames[C].Value/360)*2*3.1459;

	return 1;
}//F_LightType



//------------------ Function "EdgeAngle"
int F_EdgeAngle(void)
{
	char Temp[20];
	InFile >> Temp;
	if ( strcmp(Temp, "(envelope)") == 0 )
	{
		if ( ReadEnvelope(Lights[NrLights].EdgeAngleEnv) == 0 ) return 0;
	}
	else Str2Float(Temp, Lights[NrLights].EdgeAngle, '\0');


//---- Converting angles to radians...
	Lights[NrLights].EdgeAngle = (Lights[NrLights].EdgeAngle/360)*2*3.1459;
	for ( int C = 0; C < Lights[NrLights].EdgeAngleEnv.NrKeyFrames; C++ )
		Lights[NrLights].EdgeAngleEnv.KeyFrames[C].Value =
			(Lights[NrLights].EdgeAngleEnv.KeyFrames[C].Value/360)*2*3.1459;

	return 1;
}//F_EdgeAngle






//------------------ Function "ShowCamera"
void F_ShowCamera(void)
{
	Active = 2;
}//F_ShowCamera


void F_TargetObject(void)
{
	InFile >> Camera->TargetObject;
	Camera->TargetObject--;
}//F_TargetObject



//------------------ Function "CameraMotion"
int F_CameraMotion(void)
{
	InFile >> InLine;		//Dump <identifer>
	InFile >> InLine;		//Dump "number of id-channels"

	int NrKeyFrames;
	InFile >> NrKeyFrames;
	if ( NrKeyFrames > MaxKeyFrames )
	{
		printf("ERR: Too many keyframes ");
		return 0;
	}//if

	Camera->NrKeyFrames = NrKeyFrames;

						//Reading keyframedata...
	for ( int C = 0; C < NrKeyFrames; C++ )
	{
		InFile >> Camera->KeyFrames[C].XPos;
		InFile >> Camera->KeyFrames[C].YPos;
		InFile >> Camera->KeyFrames[C].ZPos;

		InFile >> Camera->KeyFrames[C].YAng;
		InFile >> Camera->KeyFrames[C].XAng;
		InFile >> Camera->KeyFrames[C].ZAng;

		Camera->KeyFrames[C].XAng = (Camera->KeyFrames[C].XAng/360)*2*3.1459;
		Camera->KeyFrames[C].YAng = (Camera->KeyFrames[C].YAng/360)*2*3.1459;
		Camera->KeyFrames[C].ZAng = (Camera->KeyFrames[C].ZAng/360)*2*3.1459;

		InFile >> InLine >> InLine >> InLine;	//Dump scaling-values

		InFile >> Camera->KeyFrames[C].Frame;
		InFile >> Camera->KeyFrames[C].Linear;

		InFile >> Camera->KeyFrames[C].Tension;
		InFile >> Camera->KeyFrames[C].Continuity;
		InFile >> Camera->KeyFrames[C].Bias;

	}//for

	return 1;
}//F_CameraMotion




//------------------ Function "LookAheadFrames"
void F_LookAheadFrames(void)
{
	int PathAlignment;
	InFile >> PathAlignment;
	switch ( Active )
	{
		case 0: Objects[NrObjects].PathAlignment = PathAlignment;
			break;
		case 1: Lights[NrLights].PathAlignment = PathAlignment;
			break;
		case 2: Camera->PathAlignment = PathAlignment;
			break;


	}//switch

}//F_ShowCamera




//------------------ Function "AddNullObject"
int F_AddNullObject(void)
{
	Active = 0;
	NrObjects++;
	if ( NrObjects == MaxObjects )
	{
		printf("ERR: Too many objects in .lws-file");
		return 0;
	}
	strcpy(Objects[NrObjects].Path, "NullObject");
	Objects[NrObjects].FileOffset = -1;
	return 1;
}//F_AddNullObject




int ReadEnvelope(Envelope &Env)
{
	InFile >> Env.NrKeyFrames;	//Dump "number of info channels"
	InFile >> Env.NrKeyFrames;	//Read number of keyframes
	if ( Env.NrKeyFrames > MaxKeyFrames )
	{
		printf("ERR: Too many keyframes in envelope\n");
		return 0;
	}

	for ( int C = 0; C < Env.NrKeyFrames; C++)//Read keyframedata...
	{
		InFile >> Env.KeyFrames[C].Value;

		InFile >> Env.KeyFrames[C].Frame;
		InFile >> Env.KeyFrames[C].Linear;

		InFile >> Env.KeyFrames[C].Tension;
		InFile >> Env.KeyFrames[C].Continuity;
		InFile >> Env.KeyFrames[C].Bias;
	}//for

	char Temp[20];
	InFile >> Temp;			//Dump "EndBehavior"
	InFile >> Env.EndBehavior;	//Read value
	return 1;
}//ReadEnvelope
