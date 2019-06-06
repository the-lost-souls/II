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

#include "h:\scenes\race03\race03.h"

int InitRace02(void);
void UnloadRace02(void);
void Race02Timing(void);

//---- Static data for part 1 (the tunnel)
int *Race02_Objects_1 	= NULL;
int *Race02_Lights_1 	= NULL;
SceneData *Race02_1 	= NULL;
int *Race02_Objects_2 	= NULL;
int *Race02_Lights_2 	= NULL;
SceneData *Race02_2 	= NULL;
int *Race02_Objects_3 	= NULL;
int *Race02_Lights_3 	= NULL;
SceneData *Race02_3 	= NULL;

BLN_Data Race02_BLN;

struct
{
	int Type;
	int Plugins[16];
} Null_Timing_Race02;

TIM_Data Race02_TimingData = 	{
				0, 0,
				5, 0,
				540,
				574,
				608,
				700,
				743,
			};

void Race02Timing(void)
{
	TIM_Data *Timer = (TIM_Data*)SYS_POffs;
	if ( Timer->StartFlag == 0 )
	{
		Timer->StartFlag = 1;

		Timer->CurrentKey = 0;
//		Timer->StartTime = SYS_TimerTick;
//		Race01->StartTime = SYS_TimerTick;
	}//if

	if ( Timer->CurrentKey == Timer->NrKeys ) Timer->CurrentKey = -1;

	if ( Timer->CurrentKey != -1 )
	{
		int Time = SYS_TimerTick - Timer->StartTime;
		if ( Time > Timer->TimingKeys[Timer->CurrentKey] )
		{
			switch ( Timer->CurrentKey )
			{
				case 0: SYS_OListOffs = Race02_Objects_2;
					Race02_2->StartTime = Timer->TimingKeys[Timer->CurrentKey] + Timer->StartTime;
					break;

				case 1: SYS_OListOffs = Race02_Objects_3;
					Race02_3->StartTime = Timer->TimingKeys[Timer->CurrentKey] + Timer->StartTime;
					break;

				case 2: SYS_OListOffs = Race02_Objects_1;
					Race02_1->StartTime = Timer->TimingKeys[Timer->CurrentKey] + Timer->StartTime - 5.5*SYS_TicksPerSec;

					Race02_BLN.BLN_Opacity0 = 0.0;
					Race02_BLN.BLN_Opacity1 = 1.0;

					Race02_BLN.BLN_StartFlag = 0;
					Race02_BLN.BLN_FadingTime = 500000;

					break;

				case 3:	Race02_BLN.BLN_StartFlag = 0;
					Race02_BLN.BLN_FadingTime = 50;
					break;

				case 4: SYS_OListOffs = Race03_Objects_1;
					SYS_LListOffs = Race03_Lights_1;

					Race03_TimingData.StartTime = Timer->TimingKeys[Timer->CurrentKey] + Timer->StartTime;
					Race03_1->StartTime = Timer->TimingKeys[Timer->CurrentKey] + Timer->StartTime;
					break;

			}
			Timer->CurrentKey++;
		}

	}//if

}//Race02Timing


const char Race02File_1[] = "scenes\\race02\\r8.scn";
const char Race02File_2[] = "scenes\\race02\\r9.scn";
const char Race02File_3[] = "scenes\\race02\\r10.scn";

int InitRace02(void)
{
	int C;

//---- Setting up timerplugin...
	Null_Timing_Race02.Type = 1;
	Null_Timing_Race02.Plugins[0] = (int)&Race02Timing;
	Null_Timing_Race02.Plugins[1] = (int)&Race02_TimingData;
	Null_Timing_Race02.Plugins[2] = -1;

//---- Blending...
	Race02_BLN.BLN_Type = 16;
	Race02_BLN.BLN_Plugins[0] = -1;

	Race02_BLN.BLN_R = 0.0;
	Race02_BLN.BLN_G = 0.0;
	Race02_BLN.BLN_B = 0.0;

	Race02_BLN.BLN_Opacity0 = 1.0;
	Race02_BLN.BLN_Opacity1 = 0.0;

	Race02_BLN.BLN_StartFlag = 0;
	Race02_BLN.BLN_FadingTime = 20;




//------------------- LOADING FIRST SCENE...... -------------------------
	int *Race02Object_1 = (int*)SCN_Load(Race02File_1, 0);
	if ( Race02Object_1 == 0 ) return 0;
	Race02_1 = (SceneData*)Race02Object_1[2];


//---- Creating objectlist...
	Race02_Objects_1 = new int[10 + Race02_1->NrObjects];
	if ( Race02_Objects_1 == 0 )
	{
		SYS_QuitMsg = "ERR: Not enough memory\n";
		return 0;
	}//if

	Race02_Objects_1[0] = (int)&SRT_OReset;
	Race02_Objects_1[1] = (int)Race02Object_1;
	Race02_Objects_1[2] = (int)&LIT_Init;

	for ( C = 0; C < Race02_1->NrObjects; C++ )
		Race02_Objects_1[C + 3] = (int)Race02_1->ObjectList[C];

	Race02_Objects_1[C + 3] = (int)&SRT_OSort;
	Race02_Objects_1[C + 4] = (int)&SRT_ORun;
	Race02_Objects_1[C + 5] = (int)&Race02_BLN;
	Race02_Objects_1[C + 6] = (int)&CNV_ONormal;
	Race02_Objects_1[C + 7] = (int)&KEY_OPressed;
	Race02_Objects_1[C + 8] = (int)&Null_Timing_Race02;
	Race02_Objects_1[C + 9] = -1;

//---- Creating lightlist...
	Race02_Lights_1 = new int[Race02_1->NrLights + 1];
	if ( Race02_Lights_1 == 0 )
	{
		SYS_QuitMsg = "ERR: Not enough memory\n";
		return 0;
	}//if

	for ( C = 0; C < Race02_1->NrLights; C++ )
		Race02_Lights_1[C] = (int)Race02_1->LightList[C];

	Race02_Lights_1[C] = -1;



//------------------- LOADING SECOND SCENE...... -------------------------
	int *Race02Object_2 = (int*)SCN_Load(Race02File_2, 0);
	if ( Race02Object_2 == 0 ) return 0;
	Race02_2 = (SceneData*)Race02Object_2[2];


//---- Creating objectlist...
	Race02_Objects_2 = new int[10 + Race02_2->NrObjects];
	if ( Race02_Objects_2 == 0 )
	{
		SYS_QuitMsg = "ERR: Not enough memory\n";
		return 0;
	}//if

	Race02_Objects_2[0] = (int)&SRT_OReset;
	Race02_Objects_2[1] = (int)Race02Object_2;
	Race02_Objects_2[2] = (int)&LIT_Init;

	for ( C = 0; C < Race02_2->NrObjects; C++ )
		Race02_Objects_2[C + 3] = (int)Race02_2->ObjectList[C];

	Race02_Objects_2[C + 3] = (int)&SRT_OSort;
	Race02_Objects_2[C + 4] = (int)&SRT_ORun;
	Race02_Objects_2[C + 5] = 0;
	Race02_Objects_2[C + 6] = (int)&CNV_ONormal;
	Race02_Objects_2[C + 7] = (int)&KEY_OPressed;
	Race02_Objects_2[C + 8] = (int)&Null_Timing_Race02;
	Race02_Objects_2[C + 9] = -1;

//---- Creating lightlist...
	Race02_Lights_2 = new int[Race02_2->NrLights + 1];
	if ( Race02_Lights_2 == 0 )
	{
		SYS_QuitMsg = "ERR: Not enough memory\n";
		return 0;
	}//if

	for ( C = 0; C < Race02_2->NrLights; C++ )
		Race02_Lights_2[C] = (int)Race02_2->LightList[C];

	Race02_Lights_2[C] = -1;



//------------------- LOADING THIRD SCENE...... -------------------------
	int *Race02Object_3 = (int*)SCN_Load(Race02File_3, 0);
	if ( Race02Object_3 == 0 ) return 0;
	Race02_3 = (SceneData*)Race02Object_3[2];


//---- Creating objectlist...
	Race02_Objects_3 = new int[10 + Race02_3->NrObjects];
	if ( Race02_Objects_3 == 0 )
	{
		SYS_QuitMsg = "ERR: Not enough memory\n";
		return 0;
	}//if

	Race02_Objects_3[0] = (int)&SRT_OReset;
	Race02_Objects_3[1] = (int)Race02Object_3;
	Race02_Objects_3[2] = (int)&LIT_Init;

	for ( C = 0; C < Race02_3->NrObjects; C++ )
		Race02_Objects_3[C + 3] = (int)Race02_3->ObjectList[C];

	Race02_Objects_3[C + 3] = (int)&SRT_OSort;
	Race02_Objects_3[C + 4] = (int)&SRT_ORun;
	Race02_Objects_3[C + 5] = (int)&Race02_BLN;
	Race02_Objects_3[C + 6] = (int)&CNV_ONormal;
	Race02_Objects_3[C + 7] = (int)&KEY_OPressed;
	Race02_Objects_3[C + 8] = (int)&Null_Timing_Race02;
	Race02_Objects_3[C + 9] = -1;

//---- Creating lightlist...
	Race02_Lights_3 = new int[Race02_3->NrLights + 1];
	if ( Race02_Lights_3 == 0 )
	{
		SYS_QuitMsg = "ERR: Not enough memory\n";
		return 0;
	}//if

	for ( C = 0; C < Race02_3->NrLights; C++ )
		Race02_Lights_3[C] = (int)Race02_3->LightList[C];

	Race02_Lights_3[C] = -1;

	return 1;
}//InitRace02


void UnloadRace02(void)
{
	if ( Race02_1 != 0 ) SCN_Unload((void*)Race02_1);
	Race02_1 = NULL;
	if ( Race02_Objects_1 != 0) delete [] Race02_Objects_1;
	Race02_Objects_1 = NULL;
	if ( Race02_Lights_1 != 0) delete [] Race02_Lights_1;
	Race02_Lights_1 = NULL;

	if ( Race02_2 != 0 ) SCN_Unload((void*)Race02_2);
	Race02_2 = NULL;
	if ( Race02_Objects_2 != 0) delete [] Race02_Objects_2;
	Race02_Objects_2 = NULL;
	if ( Race02_Lights_2 != 0) delete [] Race02_Lights_2;
	Race02_Lights_2 = NULL;

	if ( Race02_3 != 0 ) SCN_Unload((void*)Race02_3);
	Race02_3 = NULL;
	if ( Race02_Objects_3 != 0) delete [] Race02_Objects_3;
	Race02_Objects_3 = NULL;
	if ( Race02_Lights_3 != 0) delete [] Race02_Lights_3;
	Race02_Lights_3 = NULL;

}//UnloadRace02
