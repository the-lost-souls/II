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

#include "h:\scenes\birdies\birdies.h"

int InitRace04(void);
void UnloadRace04(void);
void Race04Timing(void);
Header *CreateTail(float Radius, int Sectors, int Rings);
void TailPlugin(void);


typedef struct
{
	int	Sectors;
	int	Rings;
	float	TimeDelay;

	SceneObjectData *PathObject;
	SceneData *Scene;
	float	VertList[];
}TailData;


//---- Static data for part 1 (the tunnel)
Header *Race04_Tail1	    = NULL;
Header *Race04_Tail2	    = NULL;
TailData *Race04_TailData1 = NULL;
TailData *Race04_TailData2 = NULL;


char Race04File_1[256] = "scenes\\race04\\f4.scn";
int *Race04_Objects_1 	= NULL;
int *Race04_Lights_1 	= NULL;
SceneData *Race04_1 	= NULL;

char Race04File_2[256] = "scenes\\race04\\f5.scn";
int *Race04_Objects_2 	= NULL;
int *Race04_Lights_2 	= NULL;
SceneData *Race04_2 	= NULL;

char Race04File_3[256] = "scenes\\race04\\f6.scn";
int *Race04_Objects_3 	= NULL;
int *Race04_Lights_3 	= NULL;
SceneData *Race04_3 	= NULL;

char Race04File_4[256] = "scenes\\race04\\f7.scn";
int *Race04_Objects_4 	= NULL;
int *Race04_Lights_4 	= NULL;
SceneData *Race04_4 	= NULL;

char Race04File_5[256] = "scenes\\race04\\f8.scn";
int *Race04_Objects_5 	= NULL;
int *Race04_Lights_5 	= NULL;
SceneData *Race04_5 	= NULL;

char Race04File_6[256] = "scenes\\race04\\f9.scn";
int *Race04_Objects_6 	= NULL;
int *Race04_Lights_6 	= NULL;
SceneData *Race04_6 	= NULL;


FOG_Data Race04_FOG;
BLN_Data Race04_BLN;
INV_Data Race04_INV;


struct
{
	int Type;
	int Plugins[16];
} Null_Timing_Race04;

TIM_Data Race04_TimingData = 	{
				0, 0,
				7, 0,
				271,		//bytt kamera
				541,		//bytt kamera
				744,		//bytt kamera
				812,            //bytt kamera
				1082,		//bytt til siste...
				2700,
				2976,


			};

//-----------------------------------------------------

void Race04Timing(void)
{
	TIM_Data *Timer = (TIM_Data*)SYS_POffs;
	if ( Timer->StartFlag == 0 )
	{
		Timer->StartFlag = 1;

		Timer->CurrentKey = 0;
//		Timer->StartTime = SYS_TimerTick;
//		Race04->StartTime = SYS_TimerTick;
	}//if

	if ( Timer->CurrentKey == Timer->NrKeys ) Timer->CurrentKey = -1;

	if ( Timer->CurrentKey != -1 )
	{
		int Time = SYS_TimerTick - Timer->StartTime;
		if ( Time > Timer->TimingKeys[Timer->CurrentKey] )
		{
			switch ( Timer->CurrentKey )
			{
				case 0: Race04_2->StartTime = Timer->TimingKeys[Timer->CurrentKey] + Timer->StartTime;
					SYS_OListOffs = Race04_Objects_2;

					Race04_TailData1->PathObject = &Race04_2->ObjectData[9];
					Race04_TailData1->Scene = Race04_2;

					Race04_TailData2->PathObject = &Race04_2->ObjectData[10];
					Race04_TailData2->Scene = Race04_2;
					break;

				case 1: Race04_3->StartTime = Timer->TimingKeys[Timer->CurrentKey] + Timer->StartTime;
 					SYS_OListOffs = Race04_Objects_3;
					Race04_INV.INV_StartFlag = 0;

					Race04_TailData1->PathObject = &Race04_3->ObjectData[9];
					Race04_TailData1->Scene = Race04_3;

					Race04_TailData2->PathObject = &Race04_3->ObjectData[10];
					Race04_TailData2->Scene = Race04_3;
					break;

				case 2: Race04_4->StartTime = Timer->TimingKeys[Timer->CurrentKey] + Timer->StartTime;
					SYS_OListOffs = Race04_Objects_4;
					Race04_INV.INV_StartFlag = 0;
					Race04_INV.INV_FadingTime = 500000;

					Race04_BLN.BLN_R = 1.0;
					Race04_BLN.BLN_G = 1.0;
					Race04_BLN.BLN_B = 1.0;

					Race04_BLN.BLN_Opacity0 = 1.0;
					Race04_BLN.BLN_Opacity1 = 0.0;

					Race04_BLN.BLN_StartFlag = 0;

					Race04_TailData1->PathObject = &Race04_4->ObjectData[9];
					Race04_TailData1->Scene = Race04_4;

					Race04_TailData2->PathObject = &Race04_4->ObjectData[10];
					Race04_TailData2->Scene = Race04_4;

					break;

				case 3: Race04_5->StartTime = Timer->TimingKeys[Timer->CurrentKey] + Timer->StartTime;
					SYS_OListOffs = Race04_Objects_5;

					Race04_BLN.BLN_R = 1.0;
					Race04_BLN.BLN_G = 1.0;
					Race04_BLN.BLN_B = 1.0;

					Race04_BLN.BLN_Opacity0 = 1.0;
					Race04_BLN.BLN_Opacity1 = 0.0;

					Race04_BLN.BLN_StartFlag = 0;

					Race04_TailData1->PathObject = &Race04_5->ObjectData[9];
					Race04_TailData1->Scene = Race04_5;

					Race04_TailData2->PathObject = &Race04_5->ObjectData[10];
					Race04_TailData2->Scene = Race04_5;

					break;

				case 4: Race04_6->StartTime = Timer->TimingKeys[Timer->CurrentKey] + Timer->StartTime;
					SYS_OListOffs = Race04_Objects_6;
					Race04_INV.INV_StartFlag = 0;
					Race04_INV.INV_FadingTime = 50;

					Race04_TailData1->PathObject = &Race04_6->ObjectData[9];
					Race04_TailData1->Scene = Race04_6;

					Race04_TailData2->PathObject = &Race04_6->ObjectData[10];
					Race04_TailData2->Scene = Race04_6;

					break;

				case 5: Race04_BLN.BLN_StartFlag = 0;
					Race04_BLN.BLN_FadingTime = 100;

					Race04_BLN.BLN_R = 0.35;
					Race04_BLN.BLN_G = 0.35;
					Race04_BLN.BLN_B = 0.40;

					Race04_BLN.BLN_Opacity0 = 0.0;
					Race04_BLN.BLN_Opacity1 = 1.0;

					break;
			        case 6:
					Birdies_TimingData.StartTime = Timer->TimingKeys[Timer->CurrentKey] + Timer->StartTime;
					Birdies->StartTime =  Timer->TimingKeys[Timer->CurrentKey] + Timer->StartTime;

					SYS_OListOffs = Birdies_Objects;
					SYS_LListOffs = Birdies_Lights;

					break;
			}
			Timer->CurrentKey++;
		}

	}//if

}//Race04Timing



int InitRace04(void)
{
	int C;

//---- Setting up fog...
	Race04_FOG.FOG_Type = 17;
	Race04_FOG.FOG_Plugins[0] = -1;
	Race04_FOG.FOG_R = 0.5;
	Race04_FOG.FOG_G = 0.5;
	Race04_FOG.FOG_B = 0.5;
	Race04_FOG.FOG_Distance = 14;

//---- Setting up timerplugin...
	Null_Timing_Race04.Type = 1;
	Null_Timing_Race04.Plugins[0] = (int)&Race04Timing;
	Null_Timing_Race04.Plugins[1] = (int)&Race04_TimingData;
	Null_Timing_Race04.Plugins[2] = -1;


//---- TailObject1
	Race04_Tail1 = CreateTail(25, 6, 6);
	if ( Race04_Tail1 == 0 ) return 0;

	Race04_TailData1 = (TailData*)new unsigned char[sizeof(TailData) + Race04_Tail1->H_Vertices*12];
	if ( Race04_TailData1 == NULL )
	{
		SYS_QuitMsg = "ERR: Not enough memory\n";
		return 0;
	}//if

	Race04_TailData1->Sectors = 6;
	Race04_TailData1->Rings = 6;
	Race04_TailData1->TimeDelay = 1.1;

	for ( C = 0; C < Race04_Tail1->H_Vertices; C++ )
	{
		Race04_TailData1->VertList[C*3 + 0] = Race04_Tail1->H_VertOffs[C].V_X;
		Race04_TailData1->VertList[C*3 + 1] = Race04_Tail1->H_VertOffs[C].V_Y;
		Race04_TailData1->VertList[C*3 + 2] = Race04_Tail1->H_VertOffs[C].V_Z;
	}//for

	Race04_Tail1->H_Plugins[0] = (int)&TailPlugin;
	Race04_Tail1->H_Plugins[1] = (int)Race04_TailData1;


//---- TailObject2
	Race04_Tail2 = CreateTail(25, 6, 6);
	if ( Race04_Tail2 == 0 ) return 0;

	Race04_TailData2 = (TailData*)new unsigned char[sizeof(TailData) + Race04_Tail2->H_Vertices*12 + Race04_Tail2->H_Faces*12];
	if ( Race04_TailData2 == NULL )
	{
		SYS_QuitMsg = "ERR: Not enough memory\n";
		return 0;
	}//if

	Race04_TailData2->Sectors = 6;
	Race04_TailData2->Rings = 6;
	Race04_TailData2->TimeDelay = 1.1;

	for ( C = 0; C < Race04_Tail2->H_Vertices; C++ )
	{
		Race04_TailData2->VertList[C*3 + 0] = Race04_Tail2->H_VertOffs[C].V_X;
		Race04_TailData2->VertList[C*3 + 1] = Race04_Tail2->H_VertOffs[C].V_Y;
		Race04_TailData2->VertList[C*3 + 2] = Race04_Tail2->H_VertOffs[C].V_Z;
	}//for

	for ( C = 0; C < Race04_Tail2->H_Faces; C++ )
	{
		Race04_TailData2->VertList[(C + Race04_Tail2->H_Vertices)*3 + 0] = Race04_Tail2->H_FaceOffs[C].F_NVX;
		Race04_TailData2->VertList[(C + Race04_Tail2->H_Vertices)*3 + 1] = Race04_Tail2->H_FaceOffs[C].F_NVY;
		Race04_TailData2->VertList[(C + Race04_Tail2->H_Vertices)*3 + 2] = Race04_Tail2->H_FaceOffs[C].F_NVZ;
	}//for

	Race04_Tail2->H_Plugins[0] = (int)&TailPlugin;
	Race04_Tail2->H_Plugins[1] = (int)Race04_TailData2;



//****************************** LOADING FIRST SCENE ***********************
	int *Race04Object_1 = (int*)SCN_Load(Race04File_1, 0);
	if ( Race04Object_1 == 0 ) return 0;
	Race04_1 = (SceneData*)Race04Object_1[2];

//---- Tails...
	Race04_TailData1->PathObject = &Race04_1->ObjectData[9];
	Race04_TailData1->Scene = Race04_1;

	Race04_TailData2->PathObject = &Race04_1->ObjectData[10];
	Race04_TailData2->Scene = Race04_1;


//---- Blending...
	Race04_BLN.BLN_Type = 16;
	Race04_BLN.BLN_Plugins[0] = -1;

	Race04_BLN.BLN_R = 0.0;
	Race04_BLN.BLN_G = 0.0;
	Race04_BLN.BLN_B = 0.0;

	Race04_BLN.BLN_Opacity0 = 1.0;
	Race04_BLN.BLN_Opacity1 = 0.0;

	Race04_BLN.BLN_StartFlag = 0;
	Race04_BLN.BLN_FadingTime = 50;

//---- Creating objectlist...
	Race04_Objects_1 = new int[13 + Race04_1->NrObjects];
	if ( Race04_Objects_1 == 0 )
	{
		SYS_QuitMsg = "ERR: Not enough memory\n";
		return 0;
	}//if

	Race04_Objects_1[0] = (int)&SRT_OReset;
	Race04_Objects_1[1] = (int)Race04Object_1;
	Race04_Objects_1[2] = (int)&LIT_Init;

	for ( C = 0; C < Race04_1->NrObjects; C++ )
		Race04_Objects_1[C + 3] = (int)Race04_1->ObjectList[C];

	Race04_Objects_1[C + 3] = (int)Race04_Tail1;
	Race04_Objects_1[C + 4] = (int)Race04_Tail2;
	Race04_Objects_1[C + 5] = (int)&SRT_OSort;
	Race04_Objects_1[C + 6] = (int)&SRT_ORun;
	Race04_Objects_1[C + 7] = (int)&Race04_FOG;
	Race04_Objects_1[C + 8] = (int)&Race04_BLN;
	Race04_Objects_1[C + 9] = (int)&CNV_ONormal;
	Race04_Objects_1[C +10] = (int)&KEY_OPressed;
	Race04_Objects_1[C +11] = (int)&Null_Timing_Race04;
	Race04_Objects_1[C +12] = -1;

//---- Creating lightlist...
	Race04_Lights_1 = new int[Race04_1->NrLights + 1];
	if ( Race04_Lights_1 == 0 )
	{
		SYS_QuitMsg = "ERR: Not enough memory\n";
		return 0;
	}//if

	for ( C = 0; C < Race04_1->NrLights; C++ )
		Race04_Lights_1[C] = (int)Race04_1->LightList[C];

	Race04_Lights_1[C] = -1;



//****************************** LOADING SECOND SCENE ***********************
	int *Race04Object_2 = (int*)SCN_Load(Race04File_2, 0);
	if ( Race04Object_2 == 0 ) return 0;
	Race04_2 = (SceneData*)Race04Object_2[2];

//---- Setting up inverting-object...
	Race04_INV.INV_Type = 18;
	Race04_INV.INV_Plugins[0] = -1;
	Race04_INV.INV_StartFlag = 0;
	Race04_INV.INV_FadingTime = 50;
	Race04_INV.INV_RMul = 1.0;
	Race04_INV.INV_GMul = 1.0;
	Race04_INV.INV_BMul = 1.0;

//---- Creating objectlist...
	Race04_Objects_2 = new int[13 + Race04_2->NrObjects];
	if ( Race04_Objects_2 == 0 )
	{
		SYS_QuitMsg = "ERR: Not enough memory\n";
		return 0;
	}//if

	Race04_Objects_2[0] = (int)&SRT_OReset;
	Race04_Objects_2[1] = (int)Race04Object_2;
	Race04_Objects_2[2] = (int)&LIT_Init;

	for ( C = 0; C < Race04_2->NrObjects; C++ )
		Race04_Objects_2[C + 3] = (int)Race04_2->ObjectList[C];

	Race04_Objects_2[C + 3] = (int)Race04_Tail1;
	Race04_Objects_2[C + 4] = (int)Race04_Tail2;
	Race04_Objects_2[C + 5] = (int)&SRT_OSort;
	Race04_Objects_2[C + 6] = (int)&SRT_ORun;
	Race04_Objects_2[C + 7] = (int)&Race04_FOG;
	Race04_Objects_2[C + 8] = (int)&Race04_INV;
	Race04_Objects_2[C + 9] = (int)&CNV_ONormal;
	Race04_Objects_2[C +10] = (int)&KEY_OPressed;
	Race04_Objects_2[C +11] = (int)&Null_Timing_Race04;
	Race04_Objects_2[C +12] = -1;

//---- Creating lightlist...
	Race04_Lights_2 = new int[Race04_2->NrLights + 1];
	if ( Race04_Lights_2 == 0 )
	{
		SYS_QuitMsg = "ERR: Not enough memory\n";
		return 0;
	}//if

	for ( C = 0; C < Race04_2->NrLights; C++ )
		Race04_Lights_2[C] = (int)Race04_2->LightList[C];

	Race04_Lights_2[C] = -1;


//****************************** LOADING THIRD SCENE ***********************
	int *Race04Object_3 = (int*)SCN_Load(Race04File_3, 0);
	if ( Race04Object_3 == 0 ) return 0;
	Race04_3 = (SceneData*)Race04Object_3[2];

//---- Creating objectlist...
	Race04_Objects_3 = new int[13 + Race04_3->NrObjects];
	if ( Race04_Objects_3 == 0 )
	{
		SYS_QuitMsg = "ERR: Not enough memory\n";
		return 0;
	}//if

	Race04_Objects_3[0] = (int)&SRT_OReset;
	Race04_Objects_3[1] = (int)Race04Object_3;
	Race04_Objects_3[2] = (int)&LIT_Init;

	for ( C = 0; C < Race04_3->NrObjects; C++ )
		Race04_Objects_3[C + 3] = (int)Race04_3->ObjectList[C];

	Race04_Objects_3[C + 3] = (int)Race04_Tail1;
	Race04_Objects_3[C + 4] = (int)Race04_Tail2;
	Race04_Objects_3[C + 5] = (int)&SRT_OSort;
	Race04_Objects_3[C + 6] = (int)&SRT_ORun;
	Race04_Objects_3[C + 7] = (int)&Race04_FOG;
	Race04_Objects_3[C + 8] = (int)&Race04_INV;
	Race04_Objects_3[C + 9] = (int)&CNV_ONormal;
	Race04_Objects_3[C +10] = (int)&KEY_OPressed;
	Race04_Objects_3[C +11] = (int)&Null_Timing_Race04;
	Race04_Objects_3[C +12] = -1;

//---- Creating lightlist...
	Race04_Lights_3 = new int[Race04_3->NrLights + 1];
	if ( Race04_Lights_3 == 0 )
	{
		SYS_QuitMsg = "ERR: Not enough memory\n";
		return 0;
	}//if

	for ( C = 0; C < Race04_3->NrLights; C++ )
		Race04_Lights_3[C] = (int)Race04_3->LightList[C];

	Race04_Lights_3[C] = -1;



//****************************** LOADING FOURTH SCENE ***********************
	int *Race04Object_4 = (int*)SCN_Load(Race04File_4, 0);
	if ( Race04Object_4 == 0 ) return 0;
	Race04_4 = (SceneData*)Race04Object_4[2];

//---- Creating objectlist...
	Race04_Objects_4 = new int[14 + Race04_4->NrObjects];
	if ( Race04_Objects_4 == 0 )
	{
		SYS_QuitMsg = "ERR: Not enough memory\n";
		return 0;
	}//if

	Race04_Objects_4[0] = (int)&SRT_OReset;
	Race04_Objects_4[1] = (int)Race04Object_4;
	Race04_Objects_4[2] = (int)&LIT_Init;

	for ( C = 0; C < Race04_4->NrObjects; C++ )
		Race04_Objects_4[C + 3] = (int)Race04_4->ObjectList[C];

	Race04_Objects_4[C + 3] = (int)Race04_Tail1;
	Race04_Objects_4[C + 4] = (int)Race04_Tail2;
	Race04_Objects_4[C + 5] = (int)&SRT_OSort;
	Race04_Objects_4[C + 6] = (int)&SRT_ORun;
	Race04_Objects_4[C + 7] = (int)&Race04_FOG;
	Race04_Objects_4[C + 8] = (int)&Race04_INV;
	Race04_Objects_4[C + 9] = (int)0; //&Race04_BLN;
	Race04_Objects_4[C +10] = (int)&CNV_ONormal;
	Race04_Objects_4[C +11] = (int)&KEY_OPressed;
	Race04_Objects_4[C +12] = (int)&Null_Timing_Race04;
	Race04_Objects_4[C +13] = -1;

//---- Creating lightlist...
	Race04_Lights_4 = new int[Race04_4->NrLights + 1];
	if ( Race04_Lights_4 == 0 )
	{
		SYS_QuitMsg = "ERR: Not enough memory\n";
		return 0;
	}//if

	for ( C = 0; C < Race04_4->NrLights; C++ )
		Race04_Lights_4[C] = (int)Race04_4->LightList[C];

	Race04_Lights_4[C] = -1;


//****************************** LOADING FIFTH SCENE ***********************
	int *Race04Object_5 = (int*)SCN_Load(Race04File_5, 0);
	if ( Race04Object_5 == 0 ) return 0;
	Race04_5 = (SceneData*)Race04Object_5[2];

//---- Creating objectlist...
	Race04_Objects_5 = new int[13 + Race04_5->NrObjects];
	if ( Race04_Objects_5 == 0 )
	{
		SYS_QuitMsg = "ERR: Not enough memory\n";
		return 0;
	}//if

	Race04_Objects_5[0] = (int)&SRT_OReset;
	Race04_Objects_5[1] = (int)Race04Object_5;
	Race04_Objects_5[2] = (int)&LIT_Init;

	for ( C = 0; C < Race04_5->NrObjects; C++ )
		Race04_Objects_5[C + 3] = (int)Race04_5->ObjectList[C];

	Race04_Objects_5[C + 3] = (int)Race04_Tail1;
	Race04_Objects_5[C + 4] = (int)Race04_Tail2;
	Race04_Objects_5[C + 5] = (int)&SRT_OSort;
	Race04_Objects_5[C + 6] = (int)&SRT_ORun;
	Race04_Objects_5[C + 7] = (int)&Race04_FOG;
	Race04_Objects_5[C + 8] = (int)&Race04_BLN;
	Race04_Objects_5[C + 9] = (int)&CNV_ONormal;
	Race04_Objects_5[C +10] = (int)&KEY_OPressed;
	Race04_Objects_5[C +11] = (int)&Null_Timing_Race04;
	Race04_Objects_5[C +12] = -1;

//---- Creating lightlist...
	Race04_Lights_5 = new int[Race04_5->NrLights + 1];
	if ( Race04_Lights_5 == 0 )
	{
		SYS_QuitMsg = "ERR: Not enough memory\n";
		return 0;
	}//if

	for ( C = 0; C < Race04_5->NrLights; C++ )
		Race04_Lights_5[C] = (int)Race04_5->LightList[C];

	Race04_Lights_5[C] = -1;

//****************************** LOADING SIXTH SCENE ***********************
	int *Race04Object_6 = (int*)SCN_Load(Race04File_6, 0);
	if ( Race04Object_6 == 0 ) return 0;
	Race04_6 = (SceneData*)Race04Object_6[2];

//---- Creating objectlist...
	Race04_Objects_6 = new int[14 + Race04_6->NrObjects];
	if ( Race04_Objects_6 == 0 )
	{
		SYS_QuitMsg = "ERR: Not enough memory\n";
		return 0;
	}//if

	Race04_Objects_6[0] = (int)&SRT_OReset;
	Race04_Objects_6[1] = (int)Race04Object_6;
	Race04_Objects_6[2] = (int)&LIT_Init;

	for ( C = 0; C < Race04_6->NrObjects; C++ )
		Race04_Objects_6[C + 3] = (int)Race04_6->ObjectList[C];

	Race04_Objects_6[C + 3] = (int)Race04_Tail1;
	Race04_Objects_6[C + 4] = (int)Race04_Tail2;
	Race04_Objects_6[C + 5] = (int)&SRT_OSort;
	Race04_Objects_6[C + 6] = (int)&SRT_ORun;
	Race04_Objects_6[C + 7] = (int)&Race04_FOG;
	Race04_Objects_6[C + 8] = (int)&Race04_INV;
	Race04_Objects_6[C + 9] = (int)&Race04_BLN;
	Race04_Objects_6[C +10] = (int)&CNV_ONormal;
	Race04_Objects_6[C +11] = (int)&KEY_OPressed;
	Race04_Objects_6[C +12] = (int)&Null_Timing_Race04;
	Race04_Objects_6[C +13] = -1;

//---- Creating lightlist...
	Race04_Lights_6 = new int[Race04_6->NrLights + 1];
	if ( Race04_Lights_6 == 0 )
	{
		SYS_QuitMsg = "ERR: Not enough memory\n";
		return 0;
	}//if

	for ( C = 0; C < Race04_6->NrLights; C++ )
		Race04_Lights_6[C] = (int)Race04_6->LightList[C];

	Race04_Lights_6[C] = -1;


	return 1;
}//InitRace04



void UnloadRace04(void)
{
	if ( Race04_Tail1 != 0 ) delete [] Race04_Tail1;
	Race04_Tail1 = NULL;
	if ( Race04_TailData1 != 0 ) delete [] Race04_TailData1;
	Race04_TailData1 = NULL;
	if ( Race04_Tail2 != 0 ) delete [] Race04_Tail2;
	Race04_Tail2 = NULL;
	if ( Race04_TailData2 != 0 ) delete [] Race04_TailData2;
	Race04_TailData2 = NULL;

	if ( Race04_1 != 0 ) SCN_Unload((void*)Race04_1);
	if ( Race04_Objects_1 != 0) delete [] Race04_Objects_1;
	if ( Race04_Lights_1 != 0) delete [] Race04_Lights_1;
	Race04_1 = NULL;
	Race04_Objects_1 = NULL;
	Race04_Lights_1 = NULL;

	if ( Race04_2 != 0 ) SCN_Unload((void*)Race04_2);
	if ( Race04_Objects_2 != 0) delete [] Race04_Objects_2;
	if ( Race04_Lights_2 != 0) delete [] Race04_Lights_2;
	Race04_2 = NULL;
	Race04_Objects_2 = NULL;
	Race04_Lights_2 = NULL;

	if ( Race04_3 != 0 ) SCN_Unload((void*)Race04_3);
	if ( Race04_Objects_3 != 0) delete [] Race04_Objects_3;
	if ( Race04_Lights_3 != 0) delete [] Race04_Lights_3;
	Race04_3 = NULL;
	Race04_Objects_3 = NULL;
	Race04_Lights_3 = NULL;

	if ( Race04_4 != 0 ) SCN_Unload((void*)Race04_4);
	if ( Race04_Objects_4 != 0) delete [] Race04_Objects_4;
	if ( Race04_Lights_4 != 0) delete [] Race04_Lights_4;
	Race04_4 = NULL;
	Race04_Objects_4 = NULL;
	Race04_Lights_4 = NULL;

	if ( Race04_5 != 0 ) SCN_Unload((void*)Race04_5);
	if ( Race04_Objects_5 != 0) delete [] Race04_Objects_5;
	if ( Race04_Lights_5 != 0) delete [] Race04_Lights_5;
	Race04_5 = NULL;
	Race04_Objects_5 = NULL;
	Race04_Lights_5 = NULL;

	if ( Race04_6 != 0 ) SCN_Unload((void*)Race04_6);
	if ( Race04_Objects_6 != 0) delete [] Race04_Objects_6;
	if ( Race04_Lights_6 != 0) delete [] Race04_Lights_6;
	Race04_6 = NULL;
	Race04_Objects_6 = NULL;
	Race04_Lights_6 = NULL;

}//UnloadRace04





Header *CreateTail(float Radius, int Sectors, int Rings)
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
		TailObject->H_MatrOffs[C1].M_R = 0.5;
		TailObject->H_MatrOffs[C1].M_G = 0.5;
		TailObject->H_MatrOffs[C1].M_B = 0.5;

		TailObject->H_MatrOffs[C1].M_Luminosity = 1.0;
		TailObject->H_MatrOffs[C1].M_Diffuse = 1.0;
		TailObject->H_MatrOffs[C1].M_Transparency = C1*((float)1/(Rings-1))/2 + 0.5;
		TailObject->H_MatrOffs[C1].M_MapWidth = 0;
		TailObject->H_MatrOffs[C1].M_MapHeigth = 0;
		TailObject->H_MatrOffs[C1].M_MapOffset = NULL;

	}//for

	return TailObject;
}//CreateTail






void TailPlugin(void)
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

}//TailPlugin
