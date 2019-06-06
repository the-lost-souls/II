#include "w:\general\var.h"
#include "w:\vectors\vectors.h"
#include "w:\general\main.h"
#include "w:\general\sort.h"
#include "w:\render\render.h"
#include "w:\plugins\plugins.h"
#include "w:\vectors\lights.h"
#include "w:\vectors\scene.h"
#include "w:\midas112\include\midasdll.h"
#include "w:\plugins\glow.h"
#include "w:\vbe\vbe.h"
#include "w:\general\canvas.h"
#include "w:\general\2d.h"
#include "w:\gfx\tga.h"
#include <stdio.h>
#include <iostream.h>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>

#include "h:\scenes\2d\2d.h"
#include "h:\scenes\dance\dance.h"
#include "h:\scenes\city\city.h"
#include "h:\scenes\birdies\birdies.h"
#include "h:\scenes\race04\race04.h"
#include "h:\scenes\race03\race03.h"
#include "h:\scenes\race02\race02.h"
#include "h:\scenes\race01\race01.h"
#include "h:\tunnel\tunnel.h"

extern MIDASmodulePlayHandle ModPlayHandle;
extern MIDASmodule Module;

void CityTiming(void);

//---- Static data for part 1 (the tunnel)

typedef struct
{
	int	Sectors;
	int	Rings;
	float	TimeDelay;

	SceneObjectData *PathObject;
	SceneData *Scene;
	float	VertList[];
}TailData;

char CityFile[256] = "scenes\\city\\c13.scn";
int *City_Objects 	= NULL;
int *City_Lights 	= NULL;
SceneData *City 	= NULL;
Header *City_CreateTail(float Radius, int Sectors, int Rings);
void City_TailPlugin(void);

GLW_PluginData *CityGlow01;
GLW_PluginData CityGlows[9];

FOG_Data City_FOG;
BLN_Data City_BLN;


//---- Tails...
Header *City_Tail  	    = NULL;
Header *City_Tails[10]	    = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
TailData *City_TailData   = NULL;
TailData *City_TailsData[10]  = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};


struct
{
	int Type;
	int Plugins[16];
} Null_Timing_City;

TIM_Data City_TimingData = 	{
				0, 0,
				2, 0,
				6100,
				6250,

			};

//-----------------------------------------------------

void CityTiming(void)
{
	TIM_Data *Timer = (TIM_Data*)SYS_POffs;
	if ( Timer->StartFlag == 0 )
	{
		Timer->StartFlag = 1;

		Timer->CurrentKey = 0;
//		Timer->StartTime = SYS_TimerTick;
//		City->StartTime = SYS_TimerTick;
	}//if

	if ( Timer->CurrentKey == Timer->NrKeys ) Timer->CurrentKey = -1;

	if ( Timer->CurrentKey != -1 )
	{
		int Time = SYS_TimerTick - Timer->StartTime;
		if ( Time > Timer->TimingKeys[Timer->CurrentKey] )
		{
			switch ( Timer->CurrentKey )
			{
				case 0: City_BLN.BLN_FadingTime = 100;
					City_BLN.BLN_StartFlag = 0;
					City_BLN.BLN_Opacity1 = 1;
					break;

				case 1:	MIDASstopModule(ModPlayHandle);

					UnloadTunnel();
					UnloadRace01();
					UnloadRace02();
					UnloadRace03();
					UnloadRace04();

				//---- Unloading most of the city too...
					if ( City != 0 ) SCN_Unload((void*)City);
					if ( City_Lights != 0) delete [] City_Lights;
					if ( CityGlow01 != 0) GLW_Unload(CityGlow01);
					City = NULL;
					City_Lights = NULL;
					CityGlow01 = NULL;

					__asm{
						pushad

						mov	edi, SYS_DBufOffs
						xor	eax, eax
						mov	ecx, SYS_DBufSize
						shr	ecx, 2
						rep	stosd

						mov	eax, 16
						call	SYS_ORoutines[eax]

						popad
					}//asm

					int C;
					for ( C = 0; C < 6; C++ )
					{
						if ( City_Tails[C] != NULL ) delete [] City_Tails[C];
						City_Tails[C] = NULL;
					}

					for ( C = 0; C < 6; C++ )
					{
						if ( City_TailsData[C] != NULL ) delete [] City_TailsData[C];
						City_TailsData[C] = NULL;
					}

					if ( InitDraw() == 0 )
					{
						SYS_QuitFlag = -1;
						break;
					}

					if ( InitDance() == 0 )
					{
						SYS_QuitFlag = -1;
						break;
					}

/*					__asm
					{
						mov	ah, 01
						int	21h
					}//asm*/

					ModPlayHandle = MIDASplayModuleSection(Module, 44, 81, 1, TRUE);

					int Time = SYS_TimerTick;
					Draw_TimingData.StartTime = Time;
					Draw->StartTime = Time;
					MapCoordData.StartTime = Time;
					SYS_PRoutines[3] = (int)&T09_Call;

					SYS_OListOffs = Draw_Objects;
					SYS_LListOffs = Draw_Lights;

					break;
			}
			Timer->CurrentKey++;
		}

	}//if

}//CityTiming



int InitCity(void)
{
	int C1, C2;

//---- Loading scene...
	int *CityObject = (int*)SCN_Load(CityFile, 0);
	if ( CityObject == 0 ) return 0;
	City = (SceneData*)CityObject[2];


//---- Setting up timerplugin...
	Null_Timing_City.Type = 1;
	Null_Timing_City.Plugins[0] = (int)&CityTiming;
	Null_Timing_City.Plugins[1] = (int)&City_TimingData;
	Null_Timing_City.Plugins[2] = -1;

//---- Setting up fog...
	City_FOG.FOG_Type = 17;
	City_FOG.FOG_Plugins[0] = -1;
	City_FOG.FOG_R = 0.35;
	City_FOG.FOG_G = 0.35;
	City_FOG.FOG_B = 0.40;
	City_FOG.FOG_Distance = 20;

//---- Blending...
	City_BLN.BLN_Type = 16;
	City_BLN.BLN_Plugins[0] = -1;

	City_BLN.BLN_R = 0.0;
	City_BLN.BLN_G = 0.0;
	City_BLN.BLN_B = 0.0;

	City_BLN.BLN_Opacity0 = 0.0;
	City_BLN.BLN_Opacity1 = 0.0;

	City_BLN.BLN_StartFlag = 0;
	City_BLN.BLN_FadingTime = 100;

//---- Creating objectlist...
	City_Objects = new int[11 + City->NrObjects + 10];
	if ( City_Objects == 0 )
	{
		SYS_QuitMsg = "ERR: Not enough memory\n";
		return 0;
	}//if

	City_Objects[0] = (int)&SRT_OReset;
	City_Objects[1] = (int)CityObject;
	City_Objects[2] = (int)&LIT_Init;

	for ( C1 = 0; C1 < City->NrObjects; C1++ )
		City_Objects[C1 + 3] = (int)City->ObjectList[C1];

	//Setting up tails...
	for ( C1 = 0; C1 < 6; C1++ )
	{

	//---- TailObjects
		City_Tails[C1] = City_CreateTail(1.5, 4, 6);
		if ( City_Tails[C1] == 0 ) return 0;

		City_TailsData[C1] = (TailData*)new unsigned char[sizeof(TailData) + City_Tails[C1]->H_Vertices*12];
		if ( City_TailsData[C1] == NULL )
		{
			SYS_QuitMsg = "ERR: Not enough memory\n";
			return 0;
		}//if

		City_TailsData[C1]->Sectors = 4;
		City_TailsData[C1]->Rings = 6;
		City_TailsData[C1]->TimeDelay = 4.0;
		City_TailsData[C1]->PathObject = &City->ObjectData[80 + C1];
		City_TailsData[C1]->Scene = City;

		for ( C2 = 0; C2 < City_Tails[C1]->H_Vertices; C2++ )
		{
			City_TailsData[C1]->VertList[C2*3 + 0] = City_Tails[C1]->H_VertOffs[C2].V_X;
			City_TailsData[C1]->VertList[C2*3 + 1] = City_Tails[C1]->H_VertOffs[C2].V_Y;
			City_TailsData[C1]->VertList[C2*3 + 2] = City_Tails[C1]->H_VertOffs[C2].V_Z;
		}//for

		City_Tails[C1]->H_Plugins[0] = (int)&City_TailPlugin;
		City_Tails[C1]->H_Plugins[1] = (int)City_TailsData[C1];


		City_Objects[C1 + City->NrObjects + 3] = (int)City_Tails[C1];
	}//for

	City_Objects[C1 + City->NrObjects + 3] = (int)&SRT_OSort;
	City_Objects[C1 + City->NrObjects + 4] = (int)&SRT_ORun;
	City_Objects[C1 + City->NrObjects + 5] = (int)&City_FOG;
	City_Objects[C1 + City->NrObjects + 6] = (int)&City_BLN;
	City_Objects[C1 + City->NrObjects + 7] = (int)&CNV_ONormal;
	City_Objects[C1 + City->NrObjects + 8] = (int)&KEY_OPressed;
	City_Objects[C1 + City->NrObjects + 9] = (int)&Null_Timing_City;
	City_Objects[C1 + City->NrObjects +10] = -1;


//---- Creating lightlist...
	City_Lights = new int[City->NrLights + 1];
	if ( City_Lights == 0 )
	{
		SYS_QuitMsg = "ERR: Not enough memory\n";
		return 0;
	}//if

	for ( C1 = 0; C1 < City->NrLights; C1++ )
		City_Lights[C1] = (int)City->LightList[C1];

	City_Lights[C1] = -1;


//---- Setting up flares...
	Header *HeaderP = NULL;

	HeaderP = (Header*)City->ObjectData[64].Offset;
	CityGlow01 = GLW_Load("maps\\flare4.tga", 0, 0);
	if ( CityGlow01 == 0 ) return 0;
	HeaderP->H_Plugins[0] = (int)&GLW_Plugin;
	HeaderP->H_Plugins[1] = (int)CityGlow01;
	HeaderP->H_Plugins[2] = -1;
	CityGlow01->GLW_Scale = 0.08;
	CityGlow01->GLW_R = 1.0;
	CityGlow01->GLW_G = 1.0;
	CityGlow01->GLW_B = 1.0;

	for ( C1 = 0; C1 < 8; C1++ )
	{
		CityGlows[C1] = *CityGlow01;
		HeaderP = (Header*)City->ObjectData[65 + C1].Offset;
		HeaderP->H_Plugins[0] = (int)&GLW_Plugin;
		HeaderP->H_Plugins[1] = (int)&CityGlows[C1];
		HeaderP->H_Plugins[2] = -1;
	}

	return 1;
}//InitCity


void UnloadCity(void)
{
	if ( City != 0 ) SCN_Unload((void*)City);
	if ( City_Objects != 0) delete [] City_Objects;
	if ( City_Lights != 0) delete [] City_Lights;
	if ( CityGlow01 != 0) GLW_Unload(CityGlow01);
	City = NULL;
	City_Objects = NULL;
	City_Lights = NULL;
	CityGlow01 = NULL;

	int C;
	for ( C = 0; C < 6; C++ )
	{
		if ( City_Tails[C] != NULL ) delete [] City_Tails[C];
		City_Tails[C] = NULL;
	}

	for ( C = 0; C < 6; C++ )
	{
		if ( City_TailsData[C] != NULL ) delete [] City_TailsData[C];
		City_TailsData[C] = NULL;
	}

}//UnloadCity


Header *City_CreateTail(float Radius, int Sectors, int Rings)
{
	int C1, C2, C3;

//---- Allocating memory...
	int MemReq = 	sizeof(Header)
		      + sizeof(Vertice)*Sectors*Rings
		      + sizeof(Face)*(Rings - 1)*Sectors*2
		      + sizeof(Material)*(Rings - 1);

	Header *TailObject = (Header*)new unsigned char[MemReq];
	if ( TailObject == NULL )
	{
		SYS_QuitMsg = "ERR: Not enough memory\n";
		return 0;
	}//if


//---- Setting up header...
	TailObject->H_Type = 12;
	for ( C1 = 0; C1 < 16; C1++ ) TailObject->H_Plugins[C1] = -1;
	for ( C1 = 0; C1 < 12; C1++ ) TailObject->H_Matrix[C1] = 0;
	TailObject->H_Matrix[ 0] = 1;
	TailObject->H_Matrix[ 5] = 1;
	TailObject->H_Matrix[10] = 1;

	TailObject->H_Vertices  = Sectors*Rings;
	TailObject->H_Faces	= (Rings - 1)*Sectors*2;
	TailObject->H_Materials = (Rings - 1);

	char *TempP = (char*)TailObject;
	TempP += sizeof(Header);
	TailObject->H_VertOffs = (Vertice*)TempP;
	TempP += sizeof(Vertice)*TailObject->H_Vertices;
	TailObject->H_FaceOffs = (Face*)TempP;
	TempP += sizeof(Face)*TailObject->H_Faces;
	TailObject->H_MatrOffs = (Material*)TempP;

	TailObject->H_Memory = MemReq;


//---- Building Vertices...
	C3 = 0;
	for ( C1 = 0; C1 < Rings; C1++ )
	{
		float RingRadius = Radius - C1*(Radius/Rings);
		for ( C2 = 0; C2 < Sectors; C2++ )
		{
			float Angle = ( (2*Pi)/Sectors )*C2;
			TailObject->H_VertOffs[C3].V_X = (-RingRadius)*sin(Angle);
			TailObject->H_VertOffs[C3].V_Y = (RingRadius)*cos(Angle);
			TailObject->H_VertOffs[C3].V_Z = 0;
			TailObject->H_VertOffs[C3].V_RFlag = 0;
			C3++;
		}//for
	}//for


//---- Building faces...
	C3 = 0;
	Vertice *V0 = NULL;
	Vertice *V1 = NULL;
	Vertice *V2 = NULL;
	Vertice *V3 = NULL;

	for ( C1 = 0; C1 < (Rings - 1); C1++ )
	{
		V1 = &TailObject->H_VertOffs[C1*Sectors + (Sectors - 1)];
		V2 = &TailObject->H_VertOffs[(C1 + 1)*Sectors + (Sectors - 1)];
		for ( C2 = 0; C2 < Sectors; C2++ )
		{
			V0 = V1;
			V3 = V2;
			V1 = &TailObject->H_VertOffs[C1*Sectors + C2];
			V2 = &TailObject->H_VertOffs[(C1 + 1)*Sectors + C2];

			float A0, A1, A2, B0, B1, B2, DotProduct, Length, NVX, NVY, NVZ;

		//---- Building first face...
			TailObject->H_FaceOffs[C3].F_V0Offs = V0;
			TailObject->H_FaceOffs[C3].F_V1Offs = V1;
			TailObject->H_FaceOffs[C3].F_V2Offs = V2;
			TailObject->H_FaceOffs[C3].F_Type = 4;
			TailObject->H_FaceOffs[C3].F_Priority = 240;
			TailObject->H_FaceOffs[C3].F_Material = &TailObject->H_MatrOffs[C1];

			A0 = V1->V_X - V0->V_X;
			A1 = V1->V_Y - V0->V_Y;
			A2 = V1->V_Z - V0->V_Z;

			B0 = V2->V_X - V0->V_X;
			B1 = V2->V_Y - V0->V_Y;
			B2 = V2->V_Z - V0->V_Z;

			NVX = A1*B2-A2*B1;
			NVY = A2*B0-A0*B2;
			NVZ = A0*B1-A1*B0;
			DotProduct = NVX*NVX + NVY*NVY + NVZ*NVZ;
			if ( DotProduct == 0 ) Length = 1;
				else Length = sqrt(NVX*NVX + NVY*NVY + NVZ*NVZ);

			TailObject->H_FaceOffs[C3].F_NVX = NVX / Length;
			TailObject->H_FaceOffs[C3].F_NVY = NVY / Length;
			TailObject->H_FaceOffs[C3].F_NVZ = NVZ / Length;
			C3++;


		//---- Building second face...
			TailObject->H_FaceOffs[C3].F_V0Offs = V0;
			TailObject->H_FaceOffs[C3].F_V1Offs = V2;
			TailObject->H_FaceOffs[C3].F_V2Offs = V3;
			TailObject->H_FaceOffs[C3].F_Type = 4;
			TailObject->H_FaceOffs[C3].F_Priority = 240;
			TailObject->H_FaceOffs[C3].F_Material = &TailObject->H_MatrOffs[C1];

			A0 = V2->V_X - V0->V_X;
			A1 = V2->V_Y - V0->V_Y;
			A2 = V2->V_Z - V0->V_Z;

			B0 = V3->V_X - V0->V_X;
			B1 = V3->V_Y - V0->V_Y;
			B2 = V3->V_Z - V0->V_Z;

			NVX = A1*B2-A2*B1;
			NVY = A2*B0-A0*B2;
			NVZ = A0*B1-A1*B0;
			DotProduct = NVX*NVX + NVY*NVY + NVZ*NVZ;
			if ( DotProduct == 0 ) Length = 1;
				else Length = sqrt(NVX*NVX + NVY*NVY + NVZ*NVZ);

			TailObject->H_FaceOffs[C3].F_NVX = NVX / Length;
			TailObject->H_FaceOffs[C3].F_NVY = NVY / Length;
			TailObject->H_FaceOffs[C3].F_NVZ = NVZ / Length;
			C3++;


		}//for
	}//for


//---- Building materials...
	for ( C1 = 0; C1 < TailObject->H_Materials; C1++ )
	{
		TailObject->H_MatrOffs[C1].M_R = 1.0;
		TailObject->H_MatrOffs[C1].M_G = 1.0;
		TailObject->H_MatrOffs[C1].M_B = 1.0;

		TailObject->H_MatrOffs[C1].M_Luminosity = 1.0;
		TailObject->H_MatrOffs[C1].M_Diffuse = 1.0;
		TailObject->H_MatrOffs[C1].M_Transparency = C1*((float)1/(Rings-1))/2 + 0.5;
		TailObject->H_MatrOffs[C1].M_MapWidth = 0;
		TailObject->H_MatrOffs[C1].M_MapHeigth = 0;
		TailObject->H_MatrOffs[C1].M_MapOffset = NULL;

	}//for

	return TailObject;
}//Race03_CreateTail






void City_TailPlugin(void)
{
	Header *Object = (Header*)SYS_PCaller;
	TailData *TailP = (TailData*)SYS_POffs;
	int C1, C2, C3;

	float TempMatrixA[12];
	float TempMatrixB[12];

	float InFrame = TailP->Scene->CurrentFrame;
	GetCameraMatrix(InFrame, TailP->Scene->Camera, TempMatrixB);


//---- Rotating vertices...
	C3 = 0;
	Vertice *TempVert = Object->H_VertOffs;
	for ( C1 = 0; C1 < TailP->Rings; C1++ )
	{
		GetObjectMatrix(InFrame, TailP->PathObject, TempMatrixA);
		MulMatrix(TempMatrixB, TempMatrixA, TempMatrixA);
		for ( C2 = 0; C2 < TailP->Sectors; C2++ )
		{
			float X = TailP->VertList[C3*3 + 0];
			float Y = TailP->VertList[C3*3 + 1];
			float Z = TailP->VertList[C3*3 + 2];

			TempVert[C3].V_X =   X*TempMatrixA[ 0]
					   + Y*TempMatrixA[ 1]
					   + Z*TempMatrixA[ 2]
					   +   TempMatrixA[ 3];

			TempVert[C3].V_Y =   X*TempMatrixA[ 4]
					   + Y*TempMatrixA[ 5]
					   + Z*TempMatrixA[ 6]
					   +   TempMatrixA[ 7];

			TempVert[C3].V_Z =   X*TempMatrixA[ 8]
					   + Y*TempMatrixA[ 9]
					   + Z*TempMatrixA[10]
					   +   TempMatrixA[11];


			C3++;
		}//for

		InFrame -= TailP->TimeDelay;
		if ( InFrame < 0 ) InFrame = 0;
	}//for


//---- Calculating normalvectors...
	Face *TempFace = Object->H_FaceOffs;
	for ( C1 = 0; C1 < Object->H_Faces; C1++ )
	{
		float A0, A1, A2, B0, B1, B2, DotProduct, Length, NVX, NVY, NVZ;

		float X0 = TempFace->F_V0Offs->V_X;
		float Y0 = TempFace->F_V0Offs->V_Y;
		float Z0 = TempFace->F_V0Offs->V_Z;

		float X1 = TempFace->F_V1Offs->V_X;
		float Y1 = TempFace->F_V1Offs->V_Y;
		float Z1 = TempFace->F_V1Offs->V_Z;

		float X2 = TempFace->F_V2Offs->V_X;
		float Y2 = TempFace->F_V2Offs->V_Y;
		float Z2 = TempFace->F_V2Offs->V_Z;

		A0 = X2 - X0;
		A1 = Y2 - Y0;
		A2 = Z2 - Z0;

		B0 = X1 - X0;
		B1 = Y1 - Y0;
		B2 = Z1 - Z0;

		NVX = A1*B2-A2*B1;
		NVY = A2*B0-A0*B2;
		NVZ = A0*B1-A1*B0;
		DotProduct = NVX*NVX + NVY*NVY + NVZ*NVZ;
		if ( DotProduct == 0 ) Length = 1;
			else Length = sqrt(NVX*NVX + NVY*NVY + NVZ*NVZ);

		TempFace->F_NVX = NVX / Length;
		TempFace->F_NVY = NVY / Length;
		TempFace->F_NVZ = NVZ / Length;

		TempFace++;

	}//for

}//Race03_TailPlugin


