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

#include "h:\scenes\city\city.h"

int InitBirdies(void);
void UnloadBirdies(void);
void BirdiesTiming(void);


//---- Static data for part 1 (the tunnel)
char BirdiesFile[256] = "scenes\\birdies\\b1.scn";
int *Birdies_Objects 	= NULL;
int *Birdies_Lights 	= NULL;
SceneData *Birdies 	= NULL;

FOG_Data Birdies_FOG;

struct
{
	int Type;
	int Plugins[16];
} Null_Timing_Birdies;

TIM_Data Birdies_TimingData = 	{
				0, 0,
				1, 0,
				750,

			};

//-----------------------------------------------------

void BirdiesTiming(void)
{
	TIM_Data *Timer = (TIM_Data*)SYS_POffs;
	if ( Timer->StartFlag == 0 )
	{
		Timer->StartFlag = 1;

		Timer->CurrentKey = 0;
//		Timer->StartTime = SYS_TimerTick;
//		Birdies->StartTime = SYS_TimerTick;
	}//if

	if ( Timer->CurrentKey == Timer->NrKeys ) Timer->CurrentKey = -1;

	if ( Timer->CurrentKey != -1 )
	{
		int Time = SYS_TimerTick - Timer->StartTime;
		if ( Time > Timer->TimingKeys[Timer->CurrentKey] )
		{
			switch ( Timer->CurrentKey )
			{
				case 0: SYS_OListOffs = City_Objects;
					SYS_LListOffs = City_Lights;

					City_TimingData.StartTime = Timer->TimingKeys[Timer->CurrentKey] + Timer->StartTime;
					City->StartTime = Timer->TimingKeys[Timer->CurrentKey] + Timer->StartTime;

					break;
			}
			Timer->CurrentKey++;
		}

	}//if

}//BirdiesTiming



int InitBirdies(void)
{
	int C;

//---- Loading scene...
	int *BirdiesObject = (int*)SCN_Load(BirdiesFile, 0);
	if ( BirdiesObject == 0 ) return 0;
	Birdies = (SceneData*)BirdiesObject[2];

//---- Setting up fog...
	Birdies_FOG.FOG_Type = 17;
	Birdies_FOG.FOG_Plugins[0] = -1;
	Birdies_FOG.FOG_R = 0.35;
	Birdies_FOG.FOG_G = 0.35;
	Birdies_FOG.FOG_B = 0.40;
	Birdies_FOG.FOG_Distance = 21;

//---- Setting up timerplugin...
	Null_Timing_Birdies.Type = 1;
	Null_Timing_Birdies.Plugins[0] = (int)&BirdiesTiming;
	Null_Timing_Birdies.Plugins[1] = (int)&Birdies_TimingData;
	Null_Timing_Birdies.Plugins[2] = -1;

//---- Creating objectlist...
	Birdies_Objects = new int[10 + Birdies->NrObjects];
	if ( Birdies_Objects == 0 )
	{
		SYS_QuitMsg = "ERR: Not enough memory\n";
		return 0;
	}//if

	Birdies_Objects[0] = (int)&SRT_OReset;
	Birdies_Objects[1] = (int)BirdiesObject;
	Birdies_Objects[2] = (int)&LIT_Init;

	for ( C = 0; C < Birdies->NrObjects; C++ )
		Birdies_Objects[C + 3] = (int)Birdies->ObjectList[C];

	Birdies_Objects[C + 3] = (int)&SRT_OSort;
	Birdies_Objects[C + 4] = (int)&SRT_ORun;
	Birdies_Objects[C + 5] = (int)&Birdies_FOG;
	Birdies_Objects[C + 6] = (int)&CNV_ONormal;
	Birdies_Objects[C + 7] = (int)&KEY_OPressed;
	Birdies_Objects[C + 8] = (int)&Null_Timing_Birdies;
	Birdies_Objects[C + 9] = -1;


//---- Creating lightlist...
	Birdies_Lights = new int[Birdies->NrLights + 1];
	if ( Birdies_Lights == 0 )
	{
		SYS_QuitMsg = "ERR: Not enough memory\n";
		return 0;
	}//if

	for ( C = 0; C < Birdies->NrLights; C++ )
		Birdies_Lights[C] = (int)Birdies->LightList[C];

	Birdies_Lights[C] = -1;

	return 1;
}//InitBirdies



void UnloadBirdies(void)
{
	if ( Birdies != 0 ) SCN_Unload((void*)Birdies);
	if ( Birdies_Objects != 0) delete [] Birdies_Objects;
	if ( Birdies_Lights != 0) delete [] Birdies_Lights;

}//UnloadBirdies




