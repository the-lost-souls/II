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
#include <time.h>
#include <math.h>

int InitDance(void);
void UnloadDance(void);
void DanceTiming(void);
Header *Dance_CreateTail(float Radius, int Sectors, int Rings);
void Dance_TailPlugin(void);

typedef struct
{
	int	Sectors;
	int	Rings;
	float	TimeDelay;

	SceneObjectData *PathObject;
	SceneData *Scene;
	float	VertList[];
}TailData;

//---- Tails...
Header *Dance_Tails[5]	    = {NULL, NULL, NULL, NULL, NULL};
TailData *Dance_TailsData[5]  = {NULL, NULL, NULL, NULL, NULL};


//---- Static data for part 1 (the tunnel)
char DanceFile01[256] = "scenes\\dance\\t01.scn";
char DanceFile02[256] = "scenes\\dance\\t02.scn";
char DanceFile03[256] = "scenes\\dance\\t03.scn";
char DanceFile04[256] = "scenes\\dance\\t11.scn";
char DanceFile05[256] = "scenes\\dance\\t12.scn";
char DanceFile06[256] = "scenes\\dance\\t13.scn";
char DanceFile07[256] = "scenes\\dance\\t14.scn";
char DanceFile08[256] = "scenes\\dance\\t15.scn";
char DanceFile09[256] = "scenes\\dance\\t16.scn";
char DanceFile10[256] = "scenes\\dance\\t17.scn";
char DanceFile11[256] = "scenes\\dance\\t18.scn";
char DanceFile12[256] = "scenes\\dance\\twirl.scn";



int *Dance_Objects_01   = NULL;
int *Dance_Lights_01 	= NULL;
SceneData *Dance_01 	= NULL;

int *Dance_Objects_02   = NULL;
int *Dance_Lights_02 	= NULL;
SceneData *Dance_02 	= NULL;

int *Dance_Objects_03   = NULL;
int *Dance_Lights_03 	= NULL;
SceneData *Dance_03 	= NULL;

int *Dance_Objects_04   = NULL;
int *Dance_Lights_04 	= NULL;
SceneData *Dance_04 	= NULL;

int *Dance_Objects_05   = NULL;
int *Dance_Lights_05 	= NULL;
SceneData *Dance_05 	= NULL;

int *Dance_Objects_06   = NULL;
int *Dance_Lights_06 	= NULL;
SceneData *Dance_06 	= NULL;

int *Dance_Objects_07   = NULL;
int *Dance_Lights_07 	= NULL;
SceneData *Dance_07 	= NULL;

int *Dance_Objects_08   = NULL;
int *Dance_Lights_08 	= NULL;
SceneData *Dance_08 	= NULL;

int *Dance_Objects_09   = NULL;
int *Dance_Lights_09 	= NULL;
SceneData *Dance_09 	= NULL;

int *Dance_Objects_10   = NULL;
int *Dance_Lights_10 	= NULL;
SceneData *Dance_10 	= NULL;

int *Dance_Objects_11   = NULL;
int *Dance_Lights_11 	= NULL;
SceneData *Dance_11 	= NULL;

int *Dance_Objects_12   = NULL;
int *Dance_Lights_12 	= NULL;
SceneData *Dance_12 	= NULL;

int *Dance_Objects_13   = NULL;

BLN_Data Dance_BLN;
INV_Data Dance_INV;
PIC_Data Dance_PIC;


struct
{
	int Type;
	int Plugins[16];
} Null_Timing_Dance;


TIM_Data Dance_TimingData = 	{
				0, 0,
			        17, 0,
				538,            //Fading out outdoor scene 1
				638,            //Switching scenes, fading in
				1226,           //Fading out outdoor scene 2
				1276,		//Switching scenes
				2480,		//Fading out indoor scene 1
				2552,		//Switching scenes
				3190,		//Switching scenes
				3828,		//Switching scenes
				4466,		//Switching scenes
				5104,		//Switching scenes
				6380,		//Switching scenes
				7018,           //Switching scenes
				7654,		//Switching scenes
				8930,		//Switching scenes
				9800,		//picture
			       10200,		//fade out
			       10600,		//quit

			};


void DanceTiming(void)
{
	TIM_Data *Timer = (TIM_Data*)SYS_POffs;
	if ( Timer->StartFlag == 0 )
	{
		Timer->StartFlag = 1;

		Timer->CurrentKey = 0;
//		Timer->StartTime = SYS_TimerTick;
//		Dance->StartTime = SYS_TimerTick;
	}//if

	if ( Timer->CurrentKey == Timer->NrKeys ) Timer->CurrentKey = -1;

	if ( Timer->CurrentKey != -1 )
	{
		int Time = SYS_TimerTick - Timer->StartTime;
		if ( Time > Timer->TimingKeys[Timer->CurrentKey] )
		{
			switch ( Timer->CurrentKey )
			{

			//---- Fading out outdoor scene 1
				case 0: Dance_BLN.BLN_Opacity0 = 0.0;
					Dance_BLN.BLN_Opacity1 = 1.0;
					Dance_BLN.BLN_StartFlag = 0;
					Dance_BLN.BLN_FadingTime = 50;
					break;

			//---- Setting up outdoor scene 2 and fading in
				case 1: Dance_BLN.BLN_Opacity0 = 1.0;
					Dance_BLN.BLN_Opacity1 = 0.0;
					Dance_BLN.BLN_StartFlag = 0;
					Dance_BLN.BLN_FadingTime = 50;

					Dance_02->StartTime = Timer->TimingKeys[Timer->CurrentKey] + Timer->StartTime;

					SYS_OListOffs = Dance_Objects_02;
					SYS_LListOffs = Dance_Lights_02;

					break;

			//---- Fading out outdoor scene 2
				case 2: Dance_BLN.BLN_Opacity0 = 0.0;
					Dance_BLN.BLN_Opacity1 = 1.0;
					Dance_BLN.BLN_StartFlag = 0;
					Dance_BLN.BLN_FadingTime = 100;

					break;

			//---- Setting up indoor scene 1
				case 3: Dance_03->StartTime = Timer->TimingKeys[Timer->CurrentKey] + Timer->StartTime;

					SYS_OListOffs = Dance_Objects_03;
					SYS_LListOffs = Dance_Lights_03;

					Dance_BLN.BLN_Opacity0 = 0.0;
					Dance_BLN.BLN_Opacity1 = 1.0;
					Dance_BLN.BLN_StartFlag = 1;
					Dance_BLN.BLN_FadingTime = 50000000;

					break;

			//---- Fading out indoor scene 1
				case 4: Dance_BLN.BLN_Opacity0 = 0.0;
					Dance_BLN.BLN_Opacity1 = 1.0;
					Dance_BLN.BLN_StartFlag = 0;
					Dance_BLN.BLN_FadingTime = 50;

					break;

			//---- Setting up indoor scene 2
				case 5: Dance_04->StartTime = Timer->TimingKeys[Timer->CurrentKey] + Timer->StartTime;

					SYS_OListOffs = Dance_Objects_04;
					SYS_LListOffs = Dance_Lights_04;

					Dance_BLN.BLN_Opacity0 = 0.0;
					Dance_BLN.BLN_Opacity1 = 1.0;
					Dance_BLN.BLN_StartFlag = 1;
					Dance_BLN.BLN_FadingTime = 50000000;

					break;

			//---- Setting up indoor scene 3
				case 6: Dance_05->StartTime = Timer->TimingKeys[Timer->CurrentKey] + Timer->StartTime;

					SYS_OListOffs = Dance_Objects_05;
					SYS_LListOffs = Dance_Lights_05;

					Dance_INV.INV_RMul = 0.5;
					Dance_INV.INV_GMul = 0.5;
					Dance_INV.INV_BMul = 0.5;
					Dance_INV.INV_StartFlag = 0;
					Dance_INV.INV_FadingTime = 50;

					break;

			//---- Setting up indoor scene 4
				case 7: Dance_06->StartTime = Timer->TimingKeys[Timer->CurrentKey] + Timer->StartTime;

					SYS_OListOffs = Dance_Objects_06;
					SYS_LListOffs = Dance_Lights_06;

					/*Dance_INV.INV_RMul = 0.5;
					Dance_INV.INV_GMul = 0.5;
					Dance_INV.INV_BMul = 0.5;
					Dance_INV.INV_StartFlag = 0;
					Dance_INV.INV_FadingTime = 50;*/

					break;

			//---- Setting up indoor scene 5
				case 8: Dance_07->StartTime = Timer->TimingKeys[Timer->CurrentKey] + Timer->StartTime;

					SYS_OListOffs = Dance_Objects_07;
					SYS_LListOffs = Dance_Lights_07;

					/*Dance_INV.INV_RMul = 0.5;
					Dance_INV.INV_GMul = 0.5;
					Dance_INV.INV_BMul = 0.5;
					Dance_INV.INV_StartFlag = 0;
					Dance_INV.INV_FadingTime = 50;*/

					break;

			//---- Setting up indoor scene 6
				case 9: Dance_08->StartTime = Timer->TimingKeys[Timer->CurrentKey] + Timer->StartTime;

					SYS_OListOffs = Dance_Objects_08;
					SYS_LListOffs = Dance_Lights_08;

					/*Dance_INV.INV_RMul = 0.5;
					Dance_INV.INV_GMul = 0.5;
					Dance_INV.INV_BMul = 0.5;
					Dance_INV.INV_StartFlag = 0;
					Dance_INV.INV_FadingTime = 50;*/

					break;

			//---- Setting up indoor scene 7
				case 10: Dance_09->StartTime = Timer->TimingKeys[Timer->CurrentKey] + Timer->StartTime;

					SYS_OListOffs = Dance_Objects_09;
					SYS_LListOffs = Dance_Lights_09;

					/*Dance_INV.INV_RMul = 0.5;
					Dance_INV.INV_GMul = 0.5;
					Dance_INV.INV_BMul = 0.5;
					Dance_INV.INV_StartFlag = 0;
					Dance_INV.INV_FadingTime = 50;*/

					break;

			//---- Setting up indoor scene 8
				case 11: Dance_10->StartTime = Timer->TimingKeys[Timer->CurrentKey] + Timer->StartTime;

					SYS_OListOffs = Dance_Objects_10;
					SYS_LListOffs = Dance_Lights_10;

					/*Dance_INV.INV_RMul = 0.5;
					Dance_INV.INV_GMul = 0.5;
					Dance_INV.INV_BMul = 0.5;
					Dance_INV.INV_StartFlag = 0;
					Dance_INV.INV_FadingTime = 50;*/

					break;

			//---- Setting up indoor scene 9
				case 12: Dance_11->StartTime = Timer->TimingKeys[Timer->CurrentKey] + Timer->StartTime;

					SYS_OListOffs = Dance_Objects_11;
					SYS_LListOffs = Dance_Lights_11;

					break;

			//---- Setting up twirl concubine...
				case 13: Dance_12->StartTime = Timer->TimingKeys[Timer->CurrentKey] + Timer->StartTime;

					SYS_OListOffs = Dance_Objects_12;
					SYS_LListOffs = Dance_Lights_12;

					Dance_INV.INV_RMul = 0.5;
					Dance_INV.INV_GMul = 0.5;
					Dance_INV.INV_BMul = 0.5;
					Dance_INV.INV_StartFlag = 0;
					Dance_INV.INV_FadingTime = 50;

					break;

				case 14:

					SYS_OListOffs = Dance_Objects_13;

					Dance_BLN.BLN_Opacity0 = 0.0;
					Dance_BLN.BLN_Opacity1 = 1.0;
					Dance_BLN.BLN_R = 0.0;
					Dance_BLN.BLN_G = 0.0;
					Dance_BLN.BLN_B = 0.0;
					Dance_BLN.BLN_StartFlag = 0;
					Dance_BLN.BLN_FadingTime = 5000000;


					Dance_INV.INV_RMul = 0.5;
					Dance_INV.INV_GMul = 0.5;
					Dance_INV.INV_BMul = 0.5;
					Dance_INV.INV_StartFlag = 0;
					Dance_INV.INV_FadingTime = 50;

					break;

				case 15:
					Dance_BLN.BLN_StartFlag = 0;
					Dance_BLN.BLN_FadingTime = 300;

					Dance_BLN.BLN_Opacity0 = 0.0;
					Dance_BLN.BLN_Opacity1 = 1.0;

					Dance_BLN.BLN_R = 0;
					Dance_BLN.BLN_G = 0;
					Dance_BLN.BLN_B = 0;

					break;

				case 16:
					SYS_QuitFlag = -1;
					SYS_QuitMsg = "The Lost Souls 1998\n";

					break;



			}
			Timer->CurrentKey++;
		}

	}//if

}//DanceTiming



int InitDance(void)
{
	int C;

//---- Setting up timerplugin...
	Null_Timing_Dance.Type = 1;
	Null_Timing_Dance.Plugins[0] = (int)&DanceTiming;
	Null_Timing_Dance.Plugins[1] = (int)&Dance_TimingData;
	Null_Timing_Dance.Plugins[2] = -1;

//---- Blending...
	Dance_BLN.BLN_Type = 16;
	Dance_BLN.BLN_Plugins[0] = -1;

	Dance_BLN.BLN_R = 0.0;
	Dance_BLN.BLN_G = 0.0;
	Dance_BLN.BLN_B = 0.0;

	Dance_BLN.BLN_Opacity0 = 1.0;
	Dance_BLN.BLN_Opacity1 = 0.0;

	Dance_BLN.BLN_StartFlag = 0;
	Dance_BLN.BLN_FadingTime = 100;

//---- Setting up inverting-object...
	Dance_INV.INV_Type = 18;
	Dance_INV.INV_Plugins[0] = -1;
	Dance_INV.INV_StartFlag = 0;
	Dance_INV.INV_FadingTime = 500000;
	Dance_INV.INV_RMul = 1;
	Dance_INV.INV_GMul = 1;
	Dance_INV.INV_BMul = 1;



//------------------ LOADING FIRST SCENE ----------------------

	int *DanceObject_01 = (int*)SCN_Load(DanceFile01, 0);
	if ( DanceObject_01 == 0 ) return 0;
	Dance_01 = (SceneData*)DanceObject_01[2];

//---- Creating objectlist...
	Dance_Objects_01 = new int[11 + Dance_01->NrObjects];
	if ( Dance_Objects_01 == 0 )
	{
		SYS_QuitMsg = "ERR: Not enough memory\n";
		return 0;
	}//if

	Dance_Objects_01[0] = (int)&SRT_OReset;
	Dance_Objects_01[1] = (int)DanceObject_01;
	Dance_Objects_01[2] = (int)&LIT_Init;

	for ( C = 0; C < Dance_01->NrObjects; C++ )
		Dance_Objects_01[C + 3] = (int)Dance_01->ObjectList[C];

	Dance_Objects_01[C + 3] = (int)0;
	Dance_Objects_01[C + 4] = (int)&SRT_OSort;
	Dance_Objects_01[C + 5] = (int)&SRT_ORun;
	Dance_Objects_01[C + 6] = (int)&Dance_BLN;
	Dance_Objects_01[C + 7] = (int)&CNV_ONormal;
	Dance_Objects_01[C + 8] = (int)&KEY_OPressed;
	Dance_Objects_01[C + 9] = (int)&Null_Timing_Dance;
	Dance_Objects_01[C +10] = -1;


//---- Creating lightlist...
	Dance_Lights_01 = new int[Dance_01->NrLights + 1];
	if ( Dance_Lights_01 == 0 )
	{
		SYS_QuitMsg = "ERR: Not enough memory\n";
		return 0;
	}//if

	for ( C = 0; C < Dance_01->NrLights; C++ )
		Dance_Lights_01[C] = (int)Dance_01->LightList[C];

	Dance_Lights_01[C] = -1;

//------------------ LOADING SECOND SCENE ----------------------
	int *DanceObject_02 = (int*)SCN_Load(DanceFile02, 0);
	if ( DanceObject_02 == 0 ) return 0;
	Dance_02 = (SceneData*)DanceObject_02[2];

//---- Creating objectlist...
	Dance_Objects_02 = new int[11 + Dance_02->NrObjects];
	if ( Dance_Objects_02 == 0 )
	{
		SYS_QuitMsg = "ERR: Not enough memory\n";
		return 0;
	}//if

	Dance_Objects_02[0] = (int)&SRT_OReset;
	Dance_Objects_02[1] = (int)DanceObject_02;
	Dance_Objects_02[2] = (int)&LIT_Init;

	for ( C = 0; C < Dance_02->NrObjects; C++ )
		Dance_Objects_02[C + 3] = (int)Dance_02->ObjectList[C];

	Dance_Objects_02[C + 3] = (int)0;
	Dance_Objects_02[C + 4] = (int)&SRT_OSort;
	Dance_Objects_02[C + 5] = (int)&SRT_ORun;
	Dance_Objects_02[C + 6] = (int)&Dance_BLN;
	Dance_Objects_02[C + 7] = (int)&CNV_ONormal;
	Dance_Objects_02[C + 8] = (int)&KEY_OPressed;
	Dance_Objects_02[C + 9] = (int)&Null_Timing_Dance;
	Dance_Objects_02[C +10] = -1;


//---- Creating lightlist...
	Dance_Lights_02 = new int[Dance_02->NrLights + 1];
	if ( Dance_Lights_02 == 0 )
	{
		SYS_QuitMsg = "ERR: Not enough memory\n";
		return 0;
	}//if

	for ( C = 0; C < Dance_02->NrLights; C++ )
		Dance_Lights_02[C] = (int)Dance_02->LightList[C];

	Dance_Lights_02[C] = -1;

//------------------ LOADING THIRD SCENE ----------------------
	int *DanceObject_03 = (int*)SCN_Load(DanceFile03, 0);
	if ( DanceObject_03 == 0 ) return 0;
	Dance_03 = (SceneData*)DanceObject_03[2];

//---- Creating objectlist...
	Dance_Objects_03 = new int[11 + Dance_03->NrObjects];
	if ( Dance_Objects_03 == 0 )
	{
		SYS_QuitMsg = "ERR: Not enough memory\n";
		return 0;
	}//if

	Dance_Objects_03[0] = (int)&SRT_OReset;
	Dance_Objects_03[1] = (int)DanceObject_03;
	Dance_Objects_03[2] = (int)&LIT_Init;

	for ( C = 0; C < Dance_03->NrObjects; C++ )
		Dance_Objects_03[C + 3] = (int)Dance_03->ObjectList[C];

	Dance_Objects_03[C + 3] = (int)0;
	Dance_Objects_03[C + 4] = (int)&SRT_OSort;
	Dance_Objects_03[C + 5] = (int)&SRT_ORun;
	Dance_Objects_03[C + 6] = (int)&Dance_BLN;
	Dance_Objects_03[C + 7] = (int)&CNV_ONormal;
	Dance_Objects_03[C + 8] = (int)&KEY_OPressed;
	Dance_Objects_03[C + 9] = (int)&Null_Timing_Dance;
	Dance_Objects_03[C +10] = -1;


//---- Creating lightlist...
	Dance_Lights_03 = new int[Dance_03->NrLights + 1];
	if ( Dance_Lights_03 == 0 )
	{
		SYS_QuitMsg = "ERR: Not enough memory\n";
		return 0;
	}//if

	for ( C = 0; C < Dance_03->NrLights; C++ )
		Dance_Lights_03[C] = (int)Dance_03->LightList[C];

	Dance_Lights_03[C] = -1;


//------------------ LOADING FOURTH SCENE ----------------------
	int *DanceObject_04 = (int*)SCN_Load(DanceFile04, 0);
	if ( DanceObject_04 == 0 ) return 0;
	Dance_04 = (SceneData*)DanceObject_04[2];

//---- Creating objectlist...
	Dance_Objects_04 = new int[11 + Dance_04->NrObjects];
	if ( Dance_Objects_04 == 0 )
	{
		SYS_QuitMsg = "ERR: Not enough memory\n";
		return 0;
	}//if

	Dance_Objects_04[0] = (int)&SRT_OReset;
	Dance_Objects_04[1] = (int)DanceObject_04;
	Dance_Objects_04[2] = (int)&LIT_Init;

	for ( C = 0; C < Dance_04->NrObjects; C++ )
		Dance_Objects_04[C + 3] = (int)Dance_04->ObjectList[C];

	Dance_Objects_04[C + 3] = (int)0;
	Dance_Objects_04[C + 4] = (int)&SRT_OSort;
	Dance_Objects_04[C + 5] = (int)&SRT_ORun;
	Dance_Objects_04[C + 6] = (int)&Dance_BLN;
	Dance_Objects_04[C + 7] = (int)&CNV_ONormal;
	Dance_Objects_04[C + 8] = (int)&KEY_OPressed;
	Dance_Objects_04[C + 9] = (int)&Null_Timing_Dance;
	Dance_Objects_04[C +10] = -1;


//---- Creating lightlist...
	Dance_Lights_04 = new int[Dance_04->NrLights + 1];
	if ( Dance_Lights_04 == 0 )
	{
		SYS_QuitMsg = "ERR: Not enough memory\n";
		return 0;
	}//if

	for ( C = 0; C < Dance_04->NrLights; C++ )
		Dance_Lights_04[C] = (int)Dance_04->LightList[C];

	Dance_Lights_04[C] = -1;


//------------------ LOADING FIFTH SCENE ----------------------
	int *DanceObject_05 = (int*)SCN_Load(DanceFile05, 0);
	if ( DanceObject_05 == 0 ) return 0;
	Dance_05 = (SceneData*)DanceObject_05[2];

//---- Creating objectlist...
	Dance_Objects_05 = new int[11 + Dance_05->NrObjects];
	if ( Dance_Objects_05 == 0 )
	{
		SYS_QuitMsg = "ERR: Not enough memory\n";
		return 0;
	}//if

	Dance_Objects_05[0] = (int)&SRT_OReset;
	Dance_Objects_05[1] = (int)DanceObject_05;
	Dance_Objects_05[2] = (int)&LIT_Init;

	for ( C = 0; C < Dance_05->NrObjects; C++ )
		Dance_Objects_05[C + 3] = (int)Dance_05->ObjectList[C];

	Dance_Objects_05[C + 3] = (int)0;
	Dance_Objects_05[C + 4] = (int)&SRT_OSort;
	Dance_Objects_05[C + 5] = (int)&SRT_ORun;
	Dance_Objects_05[C + 6] = (int)&Dance_INV;
	Dance_Objects_05[C + 7] = (int)&CNV_ONormal;
	Dance_Objects_05[C + 8] = (int)&KEY_OPressed;
	Dance_Objects_05[C + 9] = (int)&Null_Timing_Dance;
	Dance_Objects_05[C +10] = -1;


//---- Creating lightlist...
	Dance_Lights_05 = new int[Dance_05->NrLights + 1];
	if ( Dance_Lights_05 == 0 )
	{
		SYS_QuitMsg = "ERR: Not enough memory\n";
		return 0;
	}//if

	for ( C = 0; C < Dance_05->NrLights; C++ )
		Dance_Lights_05[C] = (int)Dance_05->LightList[C];

	Dance_Lights_05[C] = -1;


//------------------ LOADING SIXTH SCENE ----------------------
	int *DanceObject_06 = (int*)SCN_Load(DanceFile06, 0);
	if ( DanceObject_06 == 0 ) return 0;
	Dance_06 = (SceneData*)DanceObject_06[2];

//---- Creating objectlist...
	Dance_Objects_06 = new int[11 + Dance_06->NrObjects];
	if ( Dance_Objects_06 == 0 )
	{
		SYS_QuitMsg = "ERR: Not enough memory\n";
		return 0;
	}//if

	Dance_Objects_06[0] = (int)&SRT_OReset;
	Dance_Objects_06[1] = (int)DanceObject_06;
	Dance_Objects_06[2] = (int)&LIT_Init;

	for ( C = 0; C < Dance_06->NrObjects; C++ )
		Dance_Objects_06[C + 3] = (int)Dance_06->ObjectList[C];

	Dance_Objects_06[C + 3] = (int)0;
	Dance_Objects_06[C + 4] = (int)&SRT_OSort;
	Dance_Objects_06[C + 5] = (int)&SRT_ORun;
	Dance_Objects_06[C + 6] = (int)&Dance_INV;
	Dance_Objects_06[C + 7] = (int)&CNV_ONormal;
	Dance_Objects_06[C + 8] = (int)&KEY_OPressed;
	Dance_Objects_06[C + 9] = (int)&Null_Timing_Dance;
	Dance_Objects_06[C +10] = -1;


//---- Creating lightlist...
	Dance_Lights_06 = new int[Dance_06->NrLights + 1];
	if ( Dance_Lights_06 == 0 )
	{
		SYS_QuitMsg = "ERR: Not enough memory\n";
		return 0;
	}//if

	for ( C = 0; C < Dance_06->NrLights; C++ )
		Dance_Lights_06[C] = (int)Dance_06->LightList[C];

	Dance_Lights_06[C] = -1;


//------------------ LOADING SEVENTH SCENE ----------------------
	int *DanceObject_07 = (int*)SCN_Load(DanceFile07, 0);
	if ( DanceObject_07 == 0 ) return 0;
	Dance_07 = (SceneData*)DanceObject_07[2];

//---- Creating objectlist...
	Dance_Objects_07 = new int[11 + Dance_07->NrObjects];
	if ( Dance_Objects_07 == 0 )
	{
		SYS_QuitMsg = "ERR: Not enough memory\n";
		return 0;
	}//if

	Dance_Objects_07[0] = (int)&SRT_OReset;
	Dance_Objects_07[1] = (int)DanceObject_07;
	Dance_Objects_07[2] = (int)&LIT_Init;

	for ( C = 0; C < Dance_07->NrObjects; C++ )
		Dance_Objects_07[C + 3] = (int)Dance_07->ObjectList[C];

	Dance_Objects_07[C + 3] = (int)0;
	Dance_Objects_07[C + 4] = (int)&SRT_OSort;
	Dance_Objects_07[C + 5] = (int)&SRT_ORun;
	Dance_Objects_07[C + 6] = (int)&Dance_INV;
	Dance_Objects_07[C + 7] = (int)&CNV_ONormal;
	Dance_Objects_07[C + 8] = (int)&KEY_OPressed;
	Dance_Objects_07[C + 9] = (int)&Null_Timing_Dance;
	Dance_Objects_07[C +10] = -1;


//---- Creating lightlist...
	Dance_Lights_07 = new int[Dance_07->NrLights + 1];
	if ( Dance_Lights_07 == 0 )
	{
		SYS_QuitMsg = "ERR: Not enough memory\n";
		return 0;
	}//if

	for ( C = 0; C < Dance_07->NrLights; C++ )
		Dance_Lights_07[C] = (int)Dance_07->LightList[C];

	Dance_Lights_07[C] = -1;


//------------------ LOADING EIGTH SCENE ----------------------
	int *DanceObject_08 = (int*)SCN_Load(DanceFile08, 0);
	if ( DanceObject_08 == 0 ) return 0;
	Dance_08 = (SceneData*)DanceObject_08[2];

//---- Creating objectlist...
	Dance_Objects_08 = new int[11 + Dance_08->NrObjects];
	if ( Dance_Objects_08 == 0 )
	{
		SYS_QuitMsg = "ERR: Not enough memory\n";
		return 0;
	}//if

	Dance_Objects_08[0] = (int)&SRT_OReset;
	Dance_Objects_08[1] = (int)DanceObject_08;
	Dance_Objects_08[2] = (int)&LIT_Init;

	for ( C = 0; C < Dance_08->NrObjects; C++ )
		Dance_Objects_08[C + 3] = (int)Dance_08->ObjectList[C];

	Dance_Objects_08[C + 3] = (int)0;
	Dance_Objects_08[C + 4] = (int)&SRT_OSort;
	Dance_Objects_08[C + 5] = (int)&SRT_ORun;
	Dance_Objects_08[C + 6] = (int)&Dance_INV;
	Dance_Objects_08[C + 7] = (int)&CNV_ONormal;
	Dance_Objects_08[C + 8] = (int)&KEY_OPressed;
	Dance_Objects_08[C + 9] = (int)&Null_Timing_Dance;
	Dance_Objects_08[C +10] = -1;


//---- Creating lightlist...
	Dance_Lights_08 = new int[Dance_08->NrLights + 1];
	if ( Dance_Lights_08 == 0 )
	{
		SYS_QuitMsg = "ERR: Not enough memory\n";
		return 0;
	}//if

	for ( C = 0; C < Dance_08->NrLights; C++ )
		Dance_Lights_08[C] = (int)Dance_08->LightList[C];

	Dance_Lights_08[C] = -1;


//------------------ LOADING 9. SCENE ----------------------
	int *DanceObject_09 = (int*)SCN_Load(DanceFile09, 0);
	if ( DanceObject_09 == 0 ) return 0;
	Dance_09 = (SceneData*)DanceObject_09[2];

//---- Creating objectlist...
	Dance_Objects_09 = new int[11 + Dance_09->NrObjects];
	if ( Dance_Objects_09 == 0 )
	{
		SYS_QuitMsg = "ERR: Not enough memory\n";
		return 0;
	}//if

	Dance_Objects_09[0] = (int)&SRT_OReset;
	Dance_Objects_09[1] = (int)DanceObject_09;
	Dance_Objects_09[2] = (int)&LIT_Init;

	for ( C = 0; C < Dance_09->NrObjects; C++ )
		Dance_Objects_09[C + 3] = (int)Dance_09->ObjectList[C];

	Dance_Objects_09[C + 3] = (int)0;
	Dance_Objects_09[C + 4] = (int)&SRT_OSort;
	Dance_Objects_09[C + 5] = (int)&SRT_ORun;
	Dance_Objects_09[C + 6] = (int)&Dance_INV;
	Dance_Objects_09[C + 7] = (int)&CNV_ONormal;
	Dance_Objects_09[C + 8] = (int)&KEY_OPressed;
	Dance_Objects_09[C + 9] = (int)&Null_Timing_Dance;
	Dance_Objects_09[C +10] = -1;


//---- Creating lightlist...
	Dance_Lights_09 = new int[Dance_09->NrLights + 1];
	if ( Dance_Lights_09 == 0 )
	{
		SYS_QuitMsg = "ERR: Not enough memory\n";
		return 0;
	}//if

	for ( C = 0; C < Dance_09->NrLights; C++ )
		Dance_Lights_09[C] = (int)Dance_09->LightList[C];

	Dance_Lights_09[C] = -1;


//------------------ LOADING 10. SCENE ----------------------
	int *DanceObject_10 = (int*)SCN_Load(DanceFile10, 0);
	if ( DanceObject_10 == 0 ) return 0;
	Dance_10 = (SceneData*)DanceObject_10[2];

//---- Creating objectlist...
	Dance_Objects_10 = new int[11 + Dance_10->NrObjects];
	if ( Dance_Objects_10 == 0 )
	{
		SYS_QuitMsg = "ERR: Not enough memory\n";
		return 0;
	}//if

	Dance_Objects_10[0] = (int)&SRT_OReset;
	Dance_Objects_10[1] = (int)DanceObject_10;
	Dance_Objects_10[2] = (int)&LIT_Init;

	for ( C = 0; C < Dance_10->NrObjects; C++ )
		Dance_Objects_10[C + 3] = (int)Dance_10->ObjectList[C];

	Dance_Objects_10[C + 3] = (int)0;
	Dance_Objects_10[C + 4] = (int)&SRT_OSort;
	Dance_Objects_10[C + 5] = (int)&SRT_ORun;
	Dance_Objects_10[C + 6] = (int)&Dance_INV;
	Dance_Objects_10[C + 7] = (int)&CNV_ONormal;
	Dance_Objects_10[C + 8] = (int)&KEY_OPressed;
	Dance_Objects_10[C + 9] = (int)&Null_Timing_Dance;
	Dance_Objects_10[C +10] = -1;


//---- Creating lightlist...
	Dance_Lights_10 = new int[Dance_10->NrLights + 1];
	if ( Dance_Lights_10 == 0 )
	{
		SYS_QuitMsg = "ERR: Not enough memory\n";
		return 0;
	}//if

	for ( C = 0; C < Dance_10->NrLights; C++ )
		Dance_Lights_10[C] = (int)Dance_10->LightList[C];

	Dance_Lights_10[C] = -1;


//------------------ LOADING 11. SCENE ----------------------
	int *DanceObject_11 = (int*)SCN_Load(DanceFile11, 0);
	if ( DanceObject_11 == 0 ) return 0;
	Dance_11 = (SceneData*)DanceObject_11[2];

//---- Creating objectlist...
	Dance_Objects_11 = new int[11 + Dance_11->NrObjects];
	if ( Dance_Objects_11 == 0 )
	{
		SYS_QuitMsg = "ERR: Not enough memory\n";
		return 0;
	}//if

	Dance_Objects_11[0] = (int)&SRT_OReset;
	Dance_Objects_11[1] = (int)DanceObject_11;
	Dance_Objects_11[2] = (int)&LIT_Init;

	for ( C = 0; C < Dance_11->NrObjects; C++ )
		Dance_Objects_11[C + 3] = (int)Dance_11->ObjectList[C];

	Dance_Objects_11[C + 3] = (int)0;
	Dance_Objects_11[C + 4] = (int)&SRT_OSort;
	Dance_Objects_11[C + 5] = (int)&SRT_ORun;
	Dance_Objects_11[C + 6] = (int)&Dance_INV;
	Dance_Objects_11[C + 7] = (int)&CNV_ONormal;
	Dance_Objects_11[C + 8] = (int)&KEY_OPressed;
	Dance_Objects_11[C + 9] = (int)&Null_Timing_Dance;
	Dance_Objects_11[C +10] = -1;


//---- Creating lightlist...
	Dance_Lights_11 = new int[Dance_11->NrLights + 1];
	if ( Dance_Lights_11 == 0 )
	{
		SYS_QuitMsg = "ERR: Not enough memory\n";
		return 0;
	}//if

	for ( C = 0; C < Dance_11->NrLights; C++ )
		Dance_Lights_11[C] = (int)Dance_11->LightList[C];

	Dance_Lights_11[C] = -1;


//------------------ LOADING 13. SCENE ----------------------
	int C1, C2;
	int *DanceObject_12 = (int*)SCN_Load(DanceFile12, 0);
	if ( DanceObject_12 == 0 ) return 0;
	Dance_12 = (SceneData*)DanceObject_12[2];

	Dance_Objects_12 = new int[11 + Dance_12->NrObjects + 5];
	if ( Dance_Objects_12 == 0 )
	{
		SYS_QuitMsg = "ERR: Not enough memory\n";
		return 0;
	}//if


	Dance_Objects_12[0] = (int)&SRT_OReset;
	Dance_Objects_12[1] = (int)DanceObject_12;
	Dance_Objects_12[2] = (int)&LIT_Init;

	for ( C1 = 0; C1 < Dance_12->NrObjects; C1++ )
		Dance_Objects_12[C1 + 3] = (int)Dance_12->ObjectList[C1];

	//Setting up tails...
	for ( C1 = 0; C1 < 5; C1++ )
	{

	//---- TailObjects
		Dance_Tails[C1] = Dance_CreateTail(0.5, 4, 6);
		if ( Dance_Tails[C1] == 0 ) return 0;

		Dance_TailsData[C1] = (TailData*)new unsigned char[sizeof(TailData) + Dance_Tails[C1]->H_Vertices*12];
		if ( Dance_TailsData[C1] == NULL )
		{
			SYS_QuitMsg = "ERR: Not enough memory\n";
			return 0;
		}//if

		Dance_TailsData[C1]->Sectors = 4;
		Dance_TailsData[C1]->Rings = 6;
		Dance_TailsData[C1]->TimeDelay = 2.0;
		Dance_TailsData[C1]->PathObject = &Dance_12->ObjectData[3 + C1];
		Dance_TailsData[C1]->Scene = Dance_12;

		for ( C2 = 0; C2 < Dance_Tails[C1]->H_Vertices; C2++ )
		{
			Dance_TailsData[C1]->VertList[C2*3 + 0] = Dance_Tails[C1]->H_VertOffs[C2].V_X;
			Dance_TailsData[C1]->VertList[C2*3 + 1] = Dance_Tails[C1]->H_VertOffs[C2].V_Y;
			Dance_TailsData[C1]->VertList[C2*3 + 2] = Dance_Tails[C1]->H_VertOffs[C2].V_Z;
		}//for

		Dance_Tails[C1]->H_Plugins[0] = (int)&Dance_TailPlugin;
		Dance_Tails[C1]->H_Plugins[1] = (int)Dance_TailsData[C1];


		Dance_Objects_12[C1 + Dance_12->NrObjects + 3] = (int)Dance_Tails[C1];
//		Dance_Objects_12[C1 + Dance_12->NrObjects + 3] = 0;
	}//for

	Dance_Objects_12[C1 + Dance_12->NrObjects + 3] = (int)&SRT_OSort;
	Dance_Objects_12[C1 + Dance_12->NrObjects + 4] = (int)&SRT_ORun;
	Dance_Objects_12[C1 + Dance_12->NrObjects + 5] = (int)0;
	Dance_Objects_12[C1 + Dance_12->NrObjects + 6] = (int)&Dance_INV;
	Dance_Objects_12[C1 + Dance_12->NrObjects + 7] = (int)&CNV_ONormal;
	Dance_Objects_12[C1 + Dance_12->NrObjects + 8] = (int)&KEY_OPressed;
	Dance_Objects_12[C1 + Dance_12->NrObjects + 9] = (int)&Null_Timing_Dance;
	Dance_Objects_12[C1 + Dance_12->NrObjects +10] = -1;


//---- Creating lightlist...
	Dance_Lights_12 = new int[Dance_12->NrLights + 1];
	if ( Dance_Lights_12 == 0 )
	{
		SYS_QuitMsg = "ERR: Not enough memory\n";
		return 0;
	}//if

	for ( C = 0; C < Dance_12->NrLights; C++ )
		Dance_Lights_12[C] = (int)Dance_12->LightList[C];

	Dance_Lights_12[C] = -1;



//Setting up for picture......
//---- Creating objectlist...
	Dance_Objects_13 = new int[7];
	if ( Dance_Objects_13 == 0 )
	{
		SYS_QuitMsg = "ERR: Not enough memory\n";
		return 0;
	}//if

	Dance_Objects_13[0] = (int)&Dance_PIC;
	Dance_Objects_13[1] = (int)0; //&Dance_INV;
	Dance_Objects_13[2] = (int)0; //&Dance_BLN;
	Dance_Objects_13[3] = (int)&CNV_ONormal;
	Dance_Objects_13[4] = (int)&KEY_OPressed;
	Dance_Objects_13[5] = (int)&Null_Timing_Dance;
	Dance_Objects_13[6] = -1;

//---- Loading logo...
	Dance_PIC.PIC_Type = 15;
	Dance_PIC.PIC_Plugins[0] = -1;

	Dance_PIC.PIC_Picture = TGA_Load("gfx\\endpic.tga", 0, 0);
	if ( Dance_PIC.PIC_Picture == 0 ) return 0;
	Dance_PIC.PIC_Width = TGA_Width;
	Dance_PIC.PIC_Heigth = TGA_Heigth;
	Dance_PIC.PIC_XPos = 0;
	Dance_PIC.PIC_YPos = 0;


	return 1;
}//InitDance


void UnloadDance(void)
{
	if ( Dance_01 != 0 ) SCN_Unload((void*)Dance_01);
	if ( Dance_Objects_01 != 0) delete [] Dance_Objects_01;
	if ( Dance_Lights_01 != 0) delete [] Dance_Lights_01;

	if ( Dance_02 != 0 ) SCN_Unload((void*)Dance_02);
	if ( Dance_Objects_02 != 0) delete [] Dance_Objects_02;
	if ( Dance_Lights_02 != 0) delete [] Dance_Lights_02;

	if ( Dance_03 != 0 ) SCN_Unload((void*)Dance_03);
	if ( Dance_Objects_03 != 0) delete [] Dance_Objects_03;
	if ( Dance_Lights_03 != 0) delete [] Dance_Lights_03;

	if ( Dance_04 != 0 ) SCN_Unload((void*)Dance_04);
	if ( Dance_Objects_04 != 0) delete [] Dance_Objects_04;
	if ( Dance_Lights_04 != 0) delete [] Dance_Lights_04;

	if ( Dance_05 != 0 ) SCN_Unload((void*)Dance_05);
	if ( Dance_Objects_05 != 0) delete [] Dance_Objects_05;
	if ( Dance_Lights_05 != 0) delete [] Dance_Lights_05;

	if ( Dance_06 != 0 ) SCN_Unload((void*)Dance_06);
	if ( Dance_Objects_06 != 0) delete [] Dance_Objects_06;
	if ( Dance_Lights_06 != 0) delete [] Dance_Lights_06;

	if ( Dance_07 != 0 ) SCN_Unload((void*)Dance_07);
	if ( Dance_Objects_07 != 0) delete [] Dance_Objects_07;
	if ( Dance_Lights_07 != 0) delete [] Dance_Lights_07;

	if ( Dance_08 != 0 ) SCN_Unload((void*)Dance_08);
	if ( Dance_Objects_08 != 0) delete [] Dance_Objects_08;
	if ( Dance_Lights_08 != 0) delete [] Dance_Lights_08;

	if ( Dance_09 != 0 ) SCN_Unload((void*)Dance_09);
	if ( Dance_Objects_09 != 0) delete [] Dance_Objects_09;
	if ( Dance_Lights_09 != 0) delete [] Dance_Lights_09;

	if ( Dance_10 != 0 ) SCN_Unload((void*)Dance_10);
	if ( Dance_Objects_10 != 0) delete [] Dance_Objects_10;
	if ( Dance_Lights_10 != 0) delete [] Dance_Lights_10;

	if ( Dance_11 != 0 ) SCN_Unload((void*)Dance_11);
	if ( Dance_Objects_11 != 0) delete [] Dance_Objects_11;
	if ( Dance_Lights_11 != 0) delete [] Dance_Lights_11;

	if ( Dance_12 != 0 ) SCN_Unload((void*)Dance_12);
	if ( Dance_Objects_12 != 0) delete [] Dance_Objects_12;
	if ( Dance_Lights_12 != 0) delete [] Dance_Lights_12;

	if ( Dance_Objects_13 != 0) delete [] Dance_Objects_13;

	int C;
	for ( C = 0; C < 5; C++ )
	{
		if ( Dance_Tails[C] != NULL ) delete [] Dance_Tails[C];
		Dance_Tails[C] = NULL;
	}

	for ( C = 0; C < 5; C++ )
	{
		if ( Dance_TailsData[C] != NULL ) delete [] Dance_TailsData[C];
		Dance_TailsData[C] = NULL;
	}


}//UnloadDance


Header *Dance_CreateTail(float Radius, int Sectors, int Rings)
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
}//Race03_CreateTail






void Dance_TailPlugin(void)
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

