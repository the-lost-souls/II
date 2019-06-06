/*
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ WATCOM C++ SOURCE ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
³Name         : LOADSCN.CPP
³Author       : Cyberfish/TLS
³Last update  : 26.03.1998
³Action       : Loads a vectorscene (.SCN)
³
³Notes :
ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/

#include "w:\general\var.h"
#include "w:\vectors\vectors.h"
#include "w:\vectors\scene.h"
#include <stdio.h>
#include <mem.h>
#include <math.h>

typedef unsigned char byte;

static char OpenErr[] = "SCN: Couldn't open file\n";
static char ReadErr[] = "SCN: Error reading file\n";
static char CorrErr[] = "SCN: Corrupt scene file\n";
static char MemErr[] = "SCN: Couldn't allocate memory\n";
static char CheckSumErr[] = "SCN: Checksum error in object file\n";
static char TypeErr[] = "SCN: Couldn't recognize objecttype\n";


const char Main_ID[] 	= "SCEN";
const char VObj_ID[] 	= "VOBJ";
const char Light_ID[] 	= "LGHT";
const char Camera_ID[] 	= "CAMR";

typedef struct{
	int 		ID;
	int		Size;
	int		Version;
	int		FileSize;
	unsigned int	CheckSum;
	int		Objects;
	int		Lights;
	int		FirstFrame;
	int		LastFrame;
	float		FramesPerSecond;
} Main_Chunk;

int CurObject;
int CurLight;
int CurEnvelope;
int P;
int UsedMem;
unsigned char *MemBlock;
unsigned char *TempMem;
SceneData *Scene;

int SetupObject(void);
int SetupLight(void);
void SetupCamera(void);
void SetupIntensityEnv(void);


void Calculate_Tangents(KeyFrame *KeyFrames, int NrKeyFrames);
float Tangent(  float P0,
		float P1,
		float P2,
		float Tension,
		float Continuity,
		float Bias);

/*
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ WATCOM C++ ROUTINE ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
³Name         : SCN_Load
³Last update  : 04.02.1998
³Action       : Loads a scene-file (.SCN)
³
³Input variables : 	Filename
³			Fileoffset
³
³Notes :
ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/

void *SCN_Load(const char *FileName, int FileOffset)
{
	int C1, C2;

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
	fseek(InFile, FileOffset + 256, SEEK_SET);	//Dump text-header

	if (fread(&FileHeader, sizeof(Main_Chunk), 1, InFile) < 1)
	{
		SYS_QuitMsg = ReadErr;
		fclose(InFile);
		return 0;
	}


//---- Check main header...
	if (FileHeader.ID != 'NECS')
	{
		SYS_QuitMsg = CorrErr;
		fclose(InFile);
		return 0;
	}


//---- allocate memory to read entire file, then read and close it
	TempMem = new unsigned char[FileHeader.FileSize];
	if (TempMem == NULL)
	{
		SYS_QuitMsg = MemErr;
		fclose(InFile);
		return 0;
	}

	fseek(InFile, 0, SEEK_SET);
	if ( fread(TempMem, FileHeader.FileSize, 1, InFile) < 1)
	{
		SYS_QuitMsg = ReadErr;
		fclose(InFile);
		delete [] TempMem;
		return 0;
	}

	fclose(InFile);


//---- Calculate checksum...

	unsigned int OrgCheckSum, NewCheckSum = 0;

	OrgCheckSum = FileHeader.CheckSum;	//Get 'should be' checksum

	(*(Main_Chunk*)&TempMem[256]).CheckSum = 0;

	//Start counting bytes...
	int FileSize = FileHeader.FileSize;

	__asm
	{
		pushad
		pushfd

		mov	ecx, FileSize
		mov	esi, TempMem
		xor	eax, eax
	CheckSumLoop:
		movzx	ebx, BYTE PTR [esi]
		add	eax, ebx
		inc	esi
		dec	ecx
		jnz	CheckSumLoop

		mov	NewCheckSum, eax

		popfd
		popad
	}//asm

	if ( NewCheckSum != OrgCheckSum)
	{
		SYS_QuitMsg = CheckSumErr;
		delete [] TempMem;
		return 0;
	}//if

//---- Count memory required for all scene-data...

	P = 256;
	int BytesRead = 256;

	int Memreq = 0;
	int NrEnvelopes = 0;

	Memreq += sizeof(SceneData);                          //scene-struct
	Memreq += sizeof(SceneObjectData)*FileHeader.Objects; //each sceneobject
	Memreq += 4*( FileHeader.Objects + 1);		      //objectlist
	Memreq += sizeof(SceneLightData)*FileHeader.Lights;   //each light
	Memreq += 4*( FileHeader.Lights + 1); 		      //lightlist
	Memreq += sizeof(SceneCameraData);		      //Cameradata
	Memreq += 17*4;					      //nullobject with plugins

	while ( BytesRead < FileHeader.FileSize )
	{
		switch ( *(int*)&TempMem[P] )
		{
			case 'JBOV' : 	Memreq += sizeof(KeyFrame)*(*(int*)&TempMem[P + 32]);
					break;

			case 'THGL' :	Memreq += sizeof(Light);
					Memreq += sizeof(KeyFrame)*(*(int*)&TempMem[P + 52]);
					break;

			case 'RMAC' :	Memreq += sizeof(KeyFrame)*(*(int*)&TempMem[P + 24]);
					break;

			case 'TNIL' :	Memreq += sizeof(KeyFrame2)*(*(int*)&TempMem[P + 12]);
					Memreq += sizeof(Envelope);
					NrEnvelopes++;
					break;

/*			case 'GNRL' :	Memreq += sizeof(KeyFrame2)*(*(int*)&TempMem[P + 12]);
					Memreq += sizeof(Envelope);
					break;

			case 'NOCL' :	Memreq += sizeof(KeyFrame2)*(*(int*)&TempMem[P + 12]);
					Memreq += sizeof(Envelope);
					break;

			case 'GDEL' :	Memreq += sizeof(KeyFrame2)*(*(int*)&TempMem[P + 12]);
					Memreq += sizeof(Envelope);
					break;*/

			default:	break;
		}//switch

		BytesRead += *(int*)&TempMem[P + 4] + 8; //chunksize + chunkheader
		P += *(int*)&TempMem[P + 4] + 8; 	 //chunksize + chunkheader
	}//while


//---- Setting up pointers...
	UsedMem = 0;
	MemBlock = new unsigned char[Memreq];
	if ( MemBlock == 0 )
	{
		SYS_QuitMsg = MemErr;
		delete [] TempMem;
		return 0;
	}

	Scene = (SceneData*)&MemBlock[UsedMem];
	UsedMem += sizeof(SceneData);

	Scene->ObjectData = (SceneObjectData*)&MemBlock[UsedMem];
	UsedMem += sizeof(SceneObjectData)*FileHeader.Objects;

	Scene->ObjectList = (int*)&MemBlock[UsedMem];
	UsedMem += sizeof(int)*(FileHeader.Objects + 1);

	Scene->LightData = (SceneLightData*)&MemBlock[UsedMem];
	UsedMem += sizeof(SceneLightData)*FileHeader.Lights;

	Scene->LightList = (int*)&MemBlock[UsedMem];
	UsedMem += sizeof(int)*(FileHeader.Lights + 1);

	Scene->Envelopes = (Envelope*)&MemBlock[UsedMem];
	UsedMem += sizeof(Envelope)*NrEnvelopes;

	Scene->Camera = (SceneCameraData*)&MemBlock[UsedMem];
	UsedMem += sizeof(SceneCameraData);




//---- Setting up scene (objects/lights/camera)
	Scene->NrObjects = FileHeader.Objects;
	Scene->NrLights  = FileHeader.Lights;
	Scene->FirstFrame= FileHeader.FirstFrame;
	Scene->LastFrame = FileHeader.LastFrame;
	Scene->FramesPerSecond = FileHeader.FramesPerSecond;
	Scene->NrEnvelopes = NrEnvelopes;

	CurObject = 0;
	CurLight = 0;
	CurEnvelope = 0;

	P = 256;
	BytesRead = 256;

	while ( BytesRead < FileHeader.FileSize )
	{

		switch ( *(int*)&TempMem[P] )
		{
			case 'JBOV' :	if ( SetupObject() == 0 ) return 0;
					break;

			case 'THGL' :   if ( SetupLight() == 0) return 0;
					break;

			case 'RMAC' :   SetupCamera();
					break;

			case 'TNIL' :   SetupIntensityEnv();
					break;

/*			case 'GNRL' :	break;

			case 'NOCL' :	break;

			case 'GDEL' :	break;*/

			default:	break;
		}//switch

		BytesRead += *(int*)&TempMem[P + 4] + 8; //chunksize + chunkheader
		P += *(int*)&TempMem[P + 4] + 8; 	 //chunksize + chunkheader
	}//while

	delete [] TempMem;


//---- Setting up nullobject with scene-plugin...
	int *SceneObject = (int*)&MemBlock[UsedMem];
	UsedMem += 17*4;
	for ( C1 = 1; C1 < 17; C1++ ) SceneObject[C1] = -1; //Erase plugins...

	SceneObject[0] = 1;			//type = Nullobject
	SceneObject[1] = (int)&ScenePlugin;     //Scene-routine
	SceneObject[2] = (int)Scene;		//Scene-data

	return SceneObject;
}//Vxx_Load


/*
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ WATCOM C++ ROUTINE ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
³Name         : SetupObject
³Last update  : 29.01.1998
³Action       :
³
³Input variables :
³
³Notes :
ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/

int SetupObject(void)
{

//---- Copying dif. values...
	Scene->ObjectData[CurObject].XPivot = *(float*)&TempMem[P +  8];
	Scene->ObjectData[CurObject].YPivot = *(float*)&TempMem[P + 12];
	Scene->ObjectData[CurObject].ZPivot = *(float*)&TempMem[P + 16];

	Scene->ObjectData[CurObject].EndBehavior   = *(int*)&TempMem[P + 20];
	Scene->ObjectData[CurObject].PathAlignment = *(int*)&TempMem[P + 28];

	Scene->ObjectData[CurObject].ParentObject = 0;
	if ( *(int*)&TempMem[P + 24] != -1)	//Setting parentmatrix...
		Scene->ObjectData[CurObject].ParentObject = &Scene->ObjectData[ (*(int*)&TempMem[P + 24]) ];

	Scene->ObjectData[CurObject].NrKeyFrames = *(int*)&TempMem[P + 32];

	int TempP = P + 36;

//---- Setting keyframe-pointer...
	Scene->ObjectData[CurObject].KeyFrames   = (KeyFrame*)&MemBlock[UsedMem];
	UsedMem += sizeof(KeyFrame)*Scene->ObjectData[CurObject].NrKeyFrames;

	for ( int C1 = 0; C1 < Scene->ObjectData[CurObject].NrKeyFrames; C1++ )
	{
		Scene->ObjectData[CurObject].KeyFrames[C1].Frame = *(int*)&TempMem[TempP];

		Scene->ObjectData[CurObject].KeyFrames[C1].Tension    = *(float*)&TempMem[TempP + 4];
		Scene->ObjectData[CurObject].KeyFrames[C1].Continuity = *(float*)&TempMem[TempP + 8];
		Scene->ObjectData[CurObject].KeyFrames[C1].Bias       = *(float*)&TempMem[TempP + 12];

		Scene->ObjectData[CurObject].KeyFrames[C1].Linear = *(int*)&TempMem[TempP + 16];

		Scene->ObjectData[CurObject].KeyFrames[C1].XPos = *(float*)&TempMem[TempP + 20];
		Scene->ObjectData[CurObject].KeyFrames[C1].YPos = *(float*)&TempMem[TempP + 24];
		Scene->ObjectData[CurObject].KeyFrames[C1].ZPos = *(float*)&TempMem[TempP + 28];

		Scene->ObjectData[CurObject].KeyFrames[C1].XAng = *(float*)&TempMem[TempP + 32];
		Scene->ObjectData[CurObject].KeyFrames[C1].YAng = *(float*)&TempMem[TempP + 36];
		Scene->ObjectData[CurObject].KeyFrames[C1].ZAng = *(float*)&TempMem[TempP + 40];

		TempP += 44;
	}//for

//---- Calculating tangents for all keys...
	Calculate_Tangents(Scene->ObjectData[CurObject].KeyFrames, Scene->ObjectData[CurObject].NrKeyFrames);


//---- Loading object from file...
	if ( *(int*)&TempMem[TempP] != -1 )
	{
		if ( TempMem[TempP + 5] == ':' )
		{
			TempMem[TempP + 4] = ' ';
			TempMem[TempP + 5] = ' ';
			TempMem[TempP + 6] = ' ';
		}//if

		Scene->ObjectList[CurObject] = (int)Vxx_Load((char*)&TempMem[TempP + 4], *(int*)&TempMem[TempP]);
		if ( Scene->ObjectList[CurObject] == 0) return 0;
		Scene->ObjectData[CurObject].Offset = (void*)Scene->ObjectList[CurObject];
	}//if
	else		//Setting up nullobject...
	{
		Header *NullObject = new Header[1];
		Scene->ObjectList[CurObject] = (int)NullObject;
		Scene->ObjectData[CurObject].Offset = (void*)NullObject;

		NullObject->H_Type = 1;		//NullObject
		NullObject->H_Vertices = 0;
		NullObject->H_Faces = 0;
		NullObject->H_Materials = 0;
		NullObject->H_VertOffs = NULL;
		NullObject->H_FaceOffs = NULL;
		NullObject->H_MatrOffs = NULL;
		for ( C1 = 0; C1 < 16; C1++ ) NullObject->H_Plugins[C1] = -1;
	}//else

	CurObject++;
	return 1;
}//SetupObject





/*
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ WATCOM C++ ROUTINE ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
³Name         : SetupLight
³Last update  : 04.02.1998
³Action       :
³
³Input variables :
³
³
³Notes :
ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/

int SetupLight(void)
{
	int C1;
//---- Setting pointers...
	Scene->LightList[CurLight] = (int)&MemBlock[UsedMem];
	Scene->LightData[CurLight].Offset = &MemBlock[UsedMem];

//---- Blanking all unused data...
	Scene->LightData[CurLight].XPivot = 0;
	Scene->LightData[CurLight].YPivot = 0;
	Scene->LightData[CurLight].ZPivot = 0;
	ZeroMatrix(Scene->LightData[CurLight].ExternalMatrix);

//---- Setting up light-structure...
	Light *LightP = (Light*)Scene->LightList[CurLight];
	UsedMem += sizeof(Light);

	for (C1 = 0; C1 < 16; C1++) LightP->L_Plugins[C1] = -1;

	LightP->L_Type   	= *(int*)&TempMem[P + 8] + 1;
	LightP->L_Plugins[0]	= (int)&SceneLightPlugin;
	LightP->L_R       	= *(float*)&TempMem[P + 12];
	LightP->L_G       	= *(float*)&TempMem[P + 16];
	LightP->L_B       	= *(float*)&TempMem[P + 20];
	LightP->L_MaxRange 	= *(float*)&TempMem[P + 24];
	LightP->L_Intensity	= *(float*)&TempMem[P + 28];
	LightP->L_ConeAngle	= *(float*)&TempMem[P + 32];
	LightP->L_EdgeAngle	= *(float*)&TempMem[P + 36];

	LightP->L_EdgeAngle	= cos(LightP->L_ConeAngle - LightP->L_EdgeAngle);
	LightP->L_ConeAngle	= cos(LightP->L_ConeAngle);

//---- Setting up plugindata...
	Scene->LightData[CurLight].EndBehavior   = *(int*)&TempMem[P + 40];
	Scene->LightData[CurLight].PathAlignment = *(int*)&TempMem[P + 48];

	Scene->LightData[CurLight].ParentObject = 0;
	if ( *(int*)&TempMem[P + 44] != -1)	//Setting parentmatrix...
		Scene->LightData[CurLight].ParentObject = &Scene->ObjectData[ (*(int*)&TempMem[P + 44]) ];

	Scene->LightData[CurLight].NrKeyFrames = *(int*)&TempMem[P + 52];

	int TempP = P + 56;

//---- Setting keyframe-pointer...
	Scene->LightData[CurLight].KeyFrames   = (KeyFrame*)&MemBlock[UsedMem];
	UsedMem += sizeof(KeyFrame)*Scene->LightData[CurLight].NrKeyFrames;

	for ( C1 = 0; C1 < Scene->LightData[CurLight].NrKeyFrames; C1++ )
	{
		Scene->LightData[CurLight].KeyFrames[C1].Frame = *(int*)&TempMem[TempP];

		Scene->LightData[CurLight].KeyFrames[C1].Tension    = *(float*)&TempMem[TempP + 4];
		Scene->LightData[CurLight].KeyFrames[C1].Continuity = *(float*)&TempMem[TempP + 8];
		Scene->LightData[CurLight].KeyFrames[C1].Bias 	     = *(float*)&TempMem[TempP + 12];

		Scene->LightData[CurLight].KeyFrames[C1].Linear = *(int*)&TempMem[TempP + 16];

		Scene->LightData[CurLight].KeyFrames[C1].XPos = *(float*)&TempMem[TempP + 20];
		Scene->LightData[CurLight].KeyFrames[C1].YPos = *(float*)&TempMem[TempP + 24];
		Scene->LightData[CurLight].KeyFrames[C1].ZPos = *(float*)&TempMem[TempP + 28];

		Scene->LightData[CurLight].KeyFrames[C1].XAng = *(float*)&TempMem[TempP + 32];
		Scene->LightData[CurLight].KeyFrames[C1].YAng = *(float*)&TempMem[TempP + 36];
		Scene->LightData[CurLight].KeyFrames[C1].ZAng = *(float*)&TempMem[TempP + 40];

		TempP += 44;
	}//for

//---- Calculating tangents for all keys...
	Calculate_Tangents(Scene->LightData[CurLight].KeyFrames, Scene->LightData[CurLight].NrKeyFrames);

	CurLight++;
	return 1;
}//SetupLight




/*
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ WATCOM C++ ROUTINE ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
³Name         : SetupCamera
³Last update  : 29.01.1998
³Action       :
³
³Input variables :
³
³
³Notes :
ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
void SetupCamera(void)
{
//---- Zeroing unused data...
	Scene->Camera->XPivot = 0;
	Scene->Camera->YPivot = 0;
	Scene->Camera->ZPivot = 0;
	ZeroMatrix(Scene->Camera->ExternalMatrix);


	Scene->Camera->EndBehavior   = *(int*)&TempMem[P +  8];
	Scene->Camera->PathAlignment = *(int*)&TempMem[P + 16];

	Scene->Camera->ParentObject = 0;
	if ( *(int*)&TempMem[P + 12] != -1)	//Setting parentobject
		Scene->Camera->ParentObject = &Scene->ObjectData[ (*(int*)&TempMem[P + 12]) ];

	Scene->Camera->TargetObject = 0;
	if ( *(int*)&TempMem[P + 20] != -1)	//Setting targetobject
		Scene->Camera->TargetObject = &Scene->ObjectData[ (*(int*)&TempMem[P + 20]) ];

	Scene->Camera->NrKeyFrames = *(int*)&TempMem[P + 24];

	int TempP = P + 28;

//---- Setting keyframe-pointer...
	Scene->Camera->KeyFrames   = (KeyFrame*)&MemBlock[UsedMem];
	UsedMem += sizeof(KeyFrame)*Scene->Camera->NrKeyFrames;

	for ( int C1 = 0; C1 < Scene->Camera->NrKeyFrames; C1++ )
	{
		Scene->Camera->KeyFrames[C1].Frame = *(int*)&TempMem[TempP];

		Scene->Camera->KeyFrames[C1].Tension    = *(float*)&TempMem[TempP + 4];
		Scene->Camera->KeyFrames[C1].Continuity = *(float*)&TempMem[TempP + 8];
		Scene->Camera->KeyFrames[C1].Bias       = *(float*)&TempMem[TempP + 12];

		Scene->Camera->KeyFrames[C1].Linear = *(int*)&TempMem[TempP + 16];

		Scene->Camera->KeyFrames[C1].XPos = *(float*)&TempMem[TempP + 20];
		Scene->Camera->KeyFrames[C1].YPos = *(float*)&TempMem[TempP + 24];
		Scene->Camera->KeyFrames[C1].ZPos = *(float*)&TempMem[TempP + 28];

		Scene->Camera->KeyFrames[C1].XAng = *(float*)&TempMem[TempP + 32];
		Scene->Camera->KeyFrames[C1].YAng = *(float*)&TempMem[TempP + 36];
		Scene->Camera->KeyFrames[C1].ZAng = *(float*)&TempMem[TempP + 40];

		TempP += 44;
	}//for

//---- Calculating tangents for all keys...
	Calculate_Tangents(Scene->Camera->KeyFrames, Scene->Camera->NrKeyFrames);

}//SetupCamera


/*
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ WATCOM C++ ROUTINE ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
³Name         : SetupIntensityEnv
³Last update  : 19.02.1998
³Action       : Sets up an envelope for lights' intensity
³
³Input variables :
³
³
³Notes :
ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
void SetupIntensityEnv(void)
{
	int C1, C2;

	Scene->Envelopes[CurEnvelope].EndBehavior = *(int*)&TempMem[P +  8];
	Scene->Envelopes[CurEnvelope].NrKeyFrames = *(int*)&TempMem[P + 12];

	Scene->Envelopes[CurEnvelope].KeyFrames = (KeyFrame2*)&MemBlock[UsedMem];
	UsedMem += sizeof(KeyFrame2)*Scene->Envelopes[CurEnvelope].NrKeyFrames;

	Light *TempLight = (Light*)Scene->LightList[CurLight - 1];
	Scene->Envelopes[CurEnvelope].Value = (float*)&TempLight->L_Intensity;

	int TempP = P + 4*4;
	for ( C1 = 0; C1 < Scene->Envelopes[CurEnvelope].NrKeyFrames; C1++ )
	{
		Scene->Envelopes[CurEnvelope].KeyFrames[C1].Frame = *(int*)&TempMem[TempP];

		Scene->Envelopes[CurEnvelope].KeyFrames[C1].Tension = *(float*)&TempMem[TempP + 4];
		Scene->Envelopes[CurEnvelope].KeyFrames[C1].Continuity = *(float*)&TempMem[TempP + 8];
		Scene->Envelopes[CurEnvelope].KeyFrames[C1].Bias = *(float*)&TempMem[TempP + 12];

		Scene->Envelopes[CurEnvelope].KeyFrames[C1].Linear = *(int*)&TempMem[TempP + 16];

		Scene->Envelopes[CurEnvelope].KeyFrames[C1].Value = *(float*)&TempMem[TempP + 20];

		TempP += 6*4;

	}//for


//---- Calculating tangents...
	if ( Scene->Envelopes[CurEnvelope].NrKeyFrames > 2 )
	{

	//---- Calculating all non-endpoint-tangents...
		for ( C1 = 1; C1 < (Scene->Envelopes[CurEnvelope].NrKeyFrames-1); C1++  )
		{


		//---- Incoming and outgoing tangents for value
			Scene->Envelopes[CurEnvelope].KeyFrames[C1].Tan_a =
				Tangent(Scene->Envelopes[CurEnvelope].KeyFrames[C1-1].Value,
					Scene->Envelopes[CurEnvelope].KeyFrames[C1+0].Value,
					Scene->Envelopes[CurEnvelope].KeyFrames[C1+1].Value,
					Scene->Envelopes[CurEnvelope].KeyFrames[C1].Tension,
					Scene->Envelopes[CurEnvelope].KeyFrames[C1].Continuity,
					Scene->Envelopes[CurEnvelope].KeyFrames[C1].Bias);

			Scene->Envelopes[CurEnvelope].KeyFrames[C1].Tan_b =
				Tangent(Scene->Envelopes[CurEnvelope].KeyFrames[C1-1].Value,
					Scene->Envelopes[CurEnvelope].KeyFrames[C1+0].Value,
					Scene->Envelopes[CurEnvelope].KeyFrames[C1+1].Value,
					Scene->Envelopes[CurEnvelope].KeyFrames[C1].Tension,
					Scene->Envelopes[CurEnvelope].KeyFrames[C1].Continuity*-1,
					Scene->Envelopes[CurEnvelope].KeyFrames[C1].Bias);
		}//for

	//---- Calculating start-tangent for position X/Y/Z
		Scene->Envelopes[CurEnvelope].KeyFrames[0].Tan_a = 0;
		Scene->Envelopes[CurEnvelope].KeyFrames[0].Tan_b =
			( 1.5*(Scene->Envelopes[CurEnvelope].KeyFrames[1].Value - Scene->Envelopes[CurEnvelope].KeyFrames[0].Value) - 0.5*Scene->Envelopes[CurEnvelope].KeyFrames[1].Tan_a)*(1-Scene->Envelopes[CurEnvelope].KeyFrames[0].Tension);

	//---- Calculating end-tangent for position X/Y/Z
		int EndKey = Scene->Envelopes[CurEnvelope].NrKeyFrames - 1;
		Scene->Envelopes[CurEnvelope].KeyFrames[EndKey].Tan_a = ( 1.5*(Scene->Envelopes[CurEnvelope].KeyFrames[EndKey].Value - Scene->Envelopes[CurEnvelope].KeyFrames[EndKey - 1].Value) - 0.5*Scene->Envelopes[CurEnvelope].KeyFrames[EndKey - 1].Tan_b)*(1-Scene->Envelopes[CurEnvelope].KeyFrames[EndKey - 1].Tension);
		Scene->Envelopes[CurEnvelope].KeyFrames[EndKey].Tan_b = 0;

	}//if NrKeyFrames >2



	if ( Scene->Envelopes[CurEnvelope].NrKeyFrames == 2 )
	{

	//---- Calculating outgoing tangents for KeyFrame[0]
		Scene->Envelopes[CurEnvelope].KeyFrames[0].Tan_a = 0;
		Scene->Envelopes[CurEnvelope].KeyFrames[0].Tan_b = (Scene->Envelopes[CurEnvelope].KeyFrames[1].Value - Scene->Envelopes[CurEnvelope].KeyFrames[0].Value)*(1 - Scene->Envelopes[CurEnvelope].KeyFrames[0].Tension);

	//---- Calculating incoming tangents for KeyFrame[1]
		Scene->Envelopes[CurEnvelope].KeyFrames[1].Tan_a = (Scene->Envelopes[CurEnvelope].KeyFrames[1].Value - Scene->Envelopes[CurEnvelope].KeyFrames[0].Value)*(1 - Scene->Envelopes[CurEnvelope].KeyFrames[1].Tension);
		Scene->Envelopes[CurEnvelope].KeyFrames[1].Tan_b = 0;

	}//if NrKeyFrames == 2

	CurEnvelope++;


}//void



/*
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ WATCOM C++ ROUTINE ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
³Name         : Calculate_Tangents...
³Last update  : 29.01.1998
³Action       : Calculates all tangents for keyframe-data
³
³Input variables : 	Pointer to keyframes
³
³Notes :
ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/


void Calculate_Tangents(KeyFrame *KeyFrames, int NrKeyFrames)
{
	int C1, C2;

	if ( NrKeyFrames > 2 )
	{

	//---- Calculating all non-endpoint-tangents...
		for ( C1 = 1; C1 < (NrKeyFrames-1); C1++  )
		{


		//---- Incoming and outgoing tangents for position X
			KeyFrames[C1].XPosTan_a = Tangent( KeyFrames[C1-1].XPos,
							   KeyFrames[C1+0].XPos,
							   KeyFrames[C1+1].XPos,
							   KeyFrames[C1].Tension,
							   KeyFrames[C1].Continuity,
							   KeyFrames[C1].Bias);

			KeyFrames[C1].XPosTan_b = Tangent( KeyFrames[C1-1].XPos,
							   KeyFrames[C1+0].XPos,
							   KeyFrames[C1+1].XPos,
							   KeyFrames[C1].Tension,
							   KeyFrames[C1].Continuity*-1,
							   KeyFrames[C1].Bias);

		//---- Incoming and outgoing tangents for position Y
			KeyFrames[C1].YPosTan_a = Tangent( KeyFrames[C1-1].YPos,
							   KeyFrames[C1+0].YPos,
							   KeyFrames[C1+1].YPos,
							   KeyFrames[C1].Tension,
							   KeyFrames[C1].Continuity,
							   KeyFrames[C1].Bias);

			KeyFrames[C1].YPosTan_b = Tangent( KeyFrames[C1-1].YPos,
							   KeyFrames[C1+0].YPos,
							   KeyFrames[C1+1].YPos,
							   KeyFrames[C1].Tension,
							   KeyFrames[C1].Continuity*-1,
							   KeyFrames[C1].Bias);

		//---- Incoming and outgoing tangents for position X
			KeyFrames[C1].ZPosTan_a = Tangent( KeyFrames[C1-1].ZPos,
							   KeyFrames[C1+0].ZPos,
							   KeyFrames[C1+1].ZPos,
							   KeyFrames[C1].Tension,
							   KeyFrames[C1].Continuity,
							   KeyFrames[C1].Bias);

			KeyFrames[C1].ZPosTan_b = Tangent( KeyFrames[C1-1].ZPos,
							   KeyFrames[C1+0].ZPos,
							   KeyFrames[C1+1].ZPos,
							   KeyFrames[C1].Tension,
							   KeyFrames[C1].Continuity*-1,
							   KeyFrames[C1].Bias);


		//---- Incoming and outgoing tangents for angle X
			KeyFrames[C1].XAngTan_a = Tangent( KeyFrames[C1-1].XAng,
							   KeyFrames[C1+0].XAng,
							   KeyFrames[C1+1].XAng,
							   KeyFrames[C1].Tension,
							   KeyFrames[C1].Continuity,
							   KeyFrames[C1].Bias);

			KeyFrames[C1].XAngTan_b = Tangent( KeyFrames[C1-1].XAng,
							   KeyFrames[C1+0].XAng,
							   KeyFrames[C1+1].XAng,
							   KeyFrames[C1].Tension,
							   KeyFrames[C1].Continuity*-1,
							   KeyFrames[C1].Bias);

		//---- Incoming and outgoing tangents for angle Y
			KeyFrames[C1].YAngTan_a = Tangent( KeyFrames[C1-1].YAng,
							   KeyFrames[C1+0].YAng,
							   KeyFrames[C1+1].YAng,
							   KeyFrames[C1].Tension,
							   KeyFrames[C1].Continuity,
							   KeyFrames[C1].Bias);

			KeyFrames[C1].YAngTan_b = Tangent( KeyFrames[C1-1].YAng,
							   KeyFrames[C1+0].YAng,
							   KeyFrames[C1+1].YAng,
							   KeyFrames[C1].Tension,
							   KeyFrames[C1].Continuity*-1,
							   KeyFrames[C1].Bias);

		//---- Incoming and outgoing tangents for angle Z
			KeyFrames[C1].ZAngTan_a = Tangent( KeyFrames[C1-1].ZAng,
							   KeyFrames[C1+0].ZAng,
							   KeyFrames[C1+1].ZAng,
							   KeyFrames[C1].Tension,
							   KeyFrames[C1].Continuity,
							   KeyFrames[C1].Bias);

			KeyFrames[C1].ZAngTan_b = Tangent( KeyFrames[C1-1].ZAng,
							   KeyFrames[C1+0].ZAng,
							   KeyFrames[C1+1].ZAng,
							   KeyFrames[C1].Tension,
							   KeyFrames[C1].Continuity*-1,
							   KeyFrames[C1].Bias);




		}//for

	//---- Calculating start-tangent for position X/Y/Z
		KeyFrames[0].XPosTan_a = 0;
		KeyFrames[0].XPosTan_b = ( 1.5*(KeyFrames[1].XPos - KeyFrames[0].XPos) - 0.5*KeyFrames[1].XPosTan_a)*(1-KeyFrames[0].Tension);

		KeyFrames[0].YPosTan_a = 0;
		KeyFrames[0].YPosTan_b = ( 1.5*(KeyFrames[1].YPos - KeyFrames[0].YPos) - 0.5*KeyFrames[1].YPosTan_a)*(1-KeyFrames[0].Tension);

		KeyFrames[0].ZPosTan_a = 0;
		KeyFrames[0].ZPosTan_b = ( 1.5*(KeyFrames[1].ZPos - KeyFrames[0].ZPos) - 0.5*KeyFrames[1].ZPosTan_a)*(1-KeyFrames[0].Tension);


	//---- Calculating start-tangent for angle X/Y/Z
		KeyFrames[0].XAngTan_a = 0;
		KeyFrames[0].XAngTan_b = ( 1.5*(KeyFrames[1].XAng - KeyFrames[0].XAng) - 0.5*KeyFrames[1].XAngTan_a)*(1-KeyFrames[0].Tension);

		KeyFrames[0].YAngTan_a = 0;
		KeyFrames[0].YAngTan_b = ( 1.5*(KeyFrames[1].YAng - KeyFrames[0].YAng) - 0.5*KeyFrames[1].YAngTan_a)*(1-KeyFrames[0].Tension);

		KeyFrames[0].ZAngTan_a = 0;
		KeyFrames[0].ZAngTan_b = ( 1.5*(KeyFrames[1].ZAng - KeyFrames[0].ZAng) - 0.5*KeyFrames[1].ZAngTan_a)*(1-KeyFrames[0].Tension);

	//---- Calculating end-tangent for position X/Y/Z
		int EndKey = NrKeyFrames - 1;
		KeyFrames[EndKey].XPosTan_a = ( 1.5*(KeyFrames[EndKey].XPos - KeyFrames[EndKey - 1].XPos) - 0.5*KeyFrames[EndKey - 1].XPosTan_b)*(1-KeyFrames[EndKey - 1].Tension);
		KeyFrames[EndKey].XPosTan_b = 0;

		KeyFrames[EndKey].YPosTan_a = ( 1.5*(KeyFrames[EndKey].YPos - KeyFrames[EndKey - 1].YPos) - 0.5*KeyFrames[EndKey - 1].YPosTan_b)*(1-KeyFrames[EndKey - 1].Tension);
		KeyFrames[EndKey].YPosTan_b = 0;

		KeyFrames[EndKey].ZPosTan_a = ( 1.5*(KeyFrames[EndKey].ZPos - KeyFrames[EndKey - 1].ZPos) - 0.5*KeyFrames[EndKey - 1].ZPosTan_b)*(1-KeyFrames[EndKey - 1].Tension);
		KeyFrames[EndKey].ZPosTan_b = 0;


	//---- Calculating end-tangent for angle X/Y/Z
		KeyFrames[EndKey].XAngTan_a = ( 1.5*(KeyFrames[EndKey].XAng - KeyFrames[EndKey - 1].XAng) - 0.5*KeyFrames[EndKey - 1].XAngTan_b)*(1-KeyFrames[EndKey - 1].Tension);
		KeyFrames[EndKey].XAngTan_b = 0;

		KeyFrames[EndKey].YAngTan_a = ( 1.5*(KeyFrames[EndKey].YAng - KeyFrames[EndKey - 1].YAng) - 0.5*KeyFrames[EndKey - 1].YAngTan_b)*(1-KeyFrames[EndKey - 1].Tension);
		KeyFrames[EndKey].YAngTan_b = 0;

		KeyFrames[EndKey].ZAngTan_a = ( 1.5*(KeyFrames[EndKey].ZAng - KeyFrames[EndKey - 1].ZAng) - 0.5*KeyFrames[EndKey - 1].ZAngTan_b)*(1-KeyFrames[EndKey - 1].Tension);
		KeyFrames[EndKey].ZAngTan_b = 0;

	}//if NrKeyFrames >2



	if ( NrKeyFrames == 2 )
	{

	//---- Calculating outgoing tangents for KeyFrame[0]
		KeyFrames[0].XPosTan_a = 0;
		KeyFrames[0].XPosTan_b = (KeyFrames[1].XPos - KeyFrames[0].XPos)*(1 - KeyFrames[0].Tension);

		KeyFrames[0].YPosTan_a = 0;
		KeyFrames[0].YPosTan_b = (KeyFrames[1].YPos - KeyFrames[0].YPos)*(1 - KeyFrames[0].Tension);

		KeyFrames[0].ZPosTan_a = 0;
		KeyFrames[0].ZPosTan_b = (KeyFrames[1].ZPos - KeyFrames[0].ZPos)*(1 - KeyFrames[0].Tension);

		KeyFrames[0].XAngTan_a = 0;
		KeyFrames[0].XAngTan_b = (KeyFrames[1].XAng - KeyFrames[0].XAng)*(1 - KeyFrames[0].Tension);

		KeyFrames[0].YAngTan_a = 0;
		KeyFrames[0].YAngTan_b = (KeyFrames[1].YAng - KeyFrames[0].YAng)*(1 - KeyFrames[0].Tension);

		KeyFrames[0].ZAngTan_a = 0;
		KeyFrames[0].ZAngTan_b = (KeyFrames[1].ZAng - KeyFrames[0].ZAng)*(1 - KeyFrames[0].Tension);


	//---- Calculating incoming tangents for KeyFrame[1]
		KeyFrames[1].XPosTan_a = (KeyFrames[1].XPos - KeyFrames[0].XPos)*(1 - KeyFrames[1].Tension);
		KeyFrames[1].XPosTan_b = 0;

		KeyFrames[1].YPosTan_a = (KeyFrames[1].YPos - KeyFrames[0].YPos)*(1 - KeyFrames[1].Tension);
		KeyFrames[1].YPosTan_b = 0;

		KeyFrames[1].ZPosTan_a = (KeyFrames[1].ZPos - KeyFrames[0].ZPos)*(1 - KeyFrames[1].Tension);
		KeyFrames[1].ZPosTan_b = 0;

		KeyFrames[1].XAngTan_a = (KeyFrames[1].XAng - KeyFrames[0].XAng)*(1 - KeyFrames[1].Tension);
		KeyFrames[1].XAngTan_b = 0;

		KeyFrames[1].YAngTan_a = (KeyFrames[1].YAng - KeyFrames[0].YAng)*(1 - KeyFrames[1].Tension);
		KeyFrames[1].YAngTan_b = 0;

		KeyFrames[1].ZAngTan_a = (KeyFrames[1].ZAng - KeyFrames[0].ZAng)*(1 - KeyFrames[1].Tension);
		KeyFrames[1].ZAngTan_b = 0;

	}//if NrKeyFrames == 2
}//Calculate_Tangents...


/*
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ WATCOM C++ ROUTINE ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
³Name         : Tangent
³Last update  : 29.01.1998
³Action       : Calculates a spline-tangent
³
³Input variables : Three points and Tension, Continuity, Bias
³
³Notes :
ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/

float Tangent(  float P0,
		float P1,
		float P2,
		float Tension,
		float Continuity,
		float Bias)
{
	float g1, g2, g3;

	g1 = (P1 - P0)*(1+Bias);
	g2 = (P2 - P1)*(1-Bias);
	g3 = g2 - g1;

	return (g1 + 0.5*g3*(1-Continuity))*(1-Tension);
}//Tangent



/*
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ WATCOM C++ ROUTINE ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
³Name         : SCN_Unload
³Last update  : 26.03.1998
³Action       : Unloads a scene.
³
³Input variables : Pointer to scene-data, _NOT_ scene-object!!!!!!
³
³
³Notes :
ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/

void SCN_Unload(void *OldScene)
{
	SceneData *RemoveScene = (SceneData*)OldScene;
	for ( int C = 0; C < RemoveScene->NrObjects; C++)
	{
		Header *Object = (Header*)RemoveScene->ObjectData[C].Offset;
		Vxx_Unload(Object);
	}//for

	delete [] RemoveScene;
}//SCN_Unload
