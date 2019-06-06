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

#include "race01.h"
#include "h:\scenes\race02\race02.h"

int InitRace01(void);
void UnloadRace01(void);
void Race01Timing(void);


//---- Static data for part 1 (the tunnel)
char Race01File[256] = "scenes\\race01\\g4.scn";
int *Race01_Objects 	= NULL;
int *Race01_Lights 	= NULL;
SceneData *Race01 	= NULL;

GLW_PluginData *Race01_Glow1;
GLW_PluginData Race01_Glow2;
GLW_PluginData Race01_Glow3;
GLW_PluginData Race01_Glow4;

unsigned char *Race01_Worms = NULL;
GLW_PluginData Worm_Glow;
void Worm_Plugin(void);

ANIM_Data Worm_AnimData;

int Worm_NULL[116];

struct
{
	int Type;
	int Plugins[16];
} Null_Timing_Race01;

TIM_Data Race01_TimingData = 	{
				0, 0,
				4, 0,
				400,
				800,
				1200,
				1820,

			};


void Race01Timing(void)
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
				case 0: Worm_AnimData.StartTime = Timer->TimingKeys[Timer->CurrentKey] + Timer->StartTime;
					Worm_AnimData.AnimStart = Race01_Worms + 120*50*40;
					break;
				case 1: Worm_AnimData.StartTime = Timer->TimingKeys[Timer->CurrentKey] + Timer->StartTime;
					Worm_AnimData.AnimStart = Race01_Worms + 120*50*40*2;
					break;
				case 2: Worm_AnimData.StartTime = Timer->TimingKeys[Timer->CurrentKey] + Timer->StartTime;
					Worm_AnimData.AnimStart = Race01_Worms + 120*50*40*3;
					break;
				case 3: SYS_OListOffs = Race02_Objects_1;
					SYS_LListOffs = Race02_Lights_1;

					Race02_TimingData.StartTime = Timer->TimingKeys[Timer->CurrentKey] + Timer->StartTime;
					Race02_1->StartTime = Timer->TimingKeys[Timer->CurrentKey] + Timer->StartTime;
					break;
			}
			Timer->CurrentKey++;
		}

	}//if

}//Race01Timing



int InitRace01(void)
{
	int C;

//---- Loading scene...
	int *Race01Object = (int*)SCN_Load(Race01File, 0);
	if ( Race01Object == 0 ) return 0;
	Race01 = (SceneData*)Race01Object[2];

//---- Setting up timerplugin...
	Null_Timing_Race01.Type = 1;
	Null_Timing_Race01.Plugins[0] = (int)&Race01Timing;
	Null_Timing_Race01.Plugins[1] = (int)&Race01_TimingData;
	Null_Timing_Race01.Plugins[2] = -1;

	Race01_Worms = new unsigned char[120*50*40*4];
	unsigned char *TempWorms = Race01_Worms;
	if ( Race01_Worms == 0 )
	{
		SYS_QuitMsg = "ERR: Not enough memory\n";
		return 0;
	}//if

	for ( int Names = 0; Names < 4; Names++ )
	{
	//---- Loading worms-picture...
		unsigned char *Picture = 0;
		switch ( Names )
		{
			case 0: Picture = TGA_Load("gfx\\cfish.tga", 0, 0);
				break;
			case 1: Picture = TGA_Load("gfx\\raw.tga", 0, 0);
				break;
			case 2: Picture = TGA_Load("gfx\\kez.tga", 0, 0);
				break;
			case 3: Picture = TGA_Load("gfx\\toe.tga", 0, 0);
				break;
		}

		if ( Picture == 0 ) return 0;
		__asm{
			pushad
			pushfd

			mov	ecx, TGA_Width
			imul	ecx, TGA_Heigth
			mov	esi, Picture
			mov	edi, TempWorms

		GreyScaleLoop:
			movzx   eax, w[esi]		//load pixel

			mov	ebx, 0x1F 		//mask out blue
			and	ebx, eax
			mov	edx, 0x3E0              //mask out green
			and	edx, eax
			shr	edx, 5
			and	eax, 0x7C00           	//mask out red
			shr	eax, 10

			add	eax, ebx
			add	eax, edx

			xor	edx, edx		//get average greyscalecolor...
			mov	ebx, 3
			idiv	ebx			//divide by 3

			shr	eax, 1			//range 0-15

			mov	b[edi], al

			add	esi, 2
			inc	edi

			dec	ecx
			jnz	GreyScaleLoop

			popfd
			popad
		}//asm

		delete [] Picture;

	//---- Precalcing worm-effect...
		srand((unsigned int)time(NULL));

		for ( int C2 = 1; C2 < 40; C2++ )
		{
			int From = (C2-1)*TGA_Width*TGA_Heigth;
			int To   = C2*TGA_Width*TGA_Heigth;
			memcpy(&TempWorms[To], &TempWorms[From], TGA_Width*TGA_Heigth);

			if ( C2 > 15 )
			{
				for ( C = 0; C < TGA_Width*TGA_Heigth; C++ )
				{
					switch ( rand()%4 )
					{
						//Left
						case 0: if ( (C%TGA_Width) != 0  )
								TempWorms[To + C] = TempWorms[To + C -1];
							break;
						//Upper
						case 1: if ( C > TGA_Width  )
								TempWorms[To + C] = TempWorms[To + C -TGA_Width];
							break;
						//Right
						case 2: if ( (C%TGA_Width) != (TGA_Width - 1) )
								TempWorms[To + C] = TempWorms[To + C +1];
							break;
						//Lower
						case 3: if ( C < (TGA_Width*(TGA_Heigth-1)) )
								TempWorms[To + C] = TempWorms[To + C +TGA_Width];
							break;

					}//switch

				}//for
				for ( C = 0; C < TGA_Width*TGA_Heigth; C++ )
					TempWorms[To + C] = TempWorms[To + C]*((float)(25-(C2-15))/25);
			}//if
			else
			{

				for ( C = 0; C < TGA_Width*TGA_Heigth; C++ )
					TempWorms[To + C] = TempWorms[C]*(((float)C2)/15);
			}//else

		}//for

		for (C = 0; C < TGA_Width*TGA_Heigth; C++) TempWorms[C] = 0;

		TempWorms += 120*50*40;
	}//for

//---- Setting up NULL-object for worm-effect
	Header *Temp = (Header*)&Worm_NULL;
	Temp->H_Type = 1;
	Temp->H_Plugins[0] = (int)&GLW_Plugin;
	Temp->H_Plugins[1] = (int)&Worm_Glow;
	Temp->H_Plugins[2] = (int)&Worm_Plugin;
	Temp->H_Plugins[3] = (int)&Worm_AnimData;
	Temp->H_Plugins[4] = (int)-1;

	Temp->H_Matrix[ 0] = 1;
	Temp->H_Matrix[ 1] = 0;
	Temp->H_Matrix[ 2] = 0;
	Temp->H_Matrix[ 3] = 0;

	Temp->H_Matrix[ 4] = 0;
	Temp->H_Matrix[ 5] = 1;
	Temp->H_Matrix[ 6] = 0;
	Temp->H_Matrix[ 7] = 0;

	Temp->H_Matrix[ 8] = 0;
	Temp->H_Matrix[ 9] = 0;
	Temp->H_Matrix[10] = 1;
	Temp->H_Matrix[11] = 2.56;


//---- Setting up glow-plugin...
	Worm_Glow.GLW_Type = 7;
	Worm_Glow.GLW_Priority = 240;
	Worm_Glow.GLW_Scale = 0.01;
	Worm_Glow.GLW_R = 1.0;
	Worm_Glow.GLW_G = 1.0;
	Worm_Glow.GLW_B = 1.0;
	Worm_Glow.GLW_MapXSize = 120;
	Worm_Glow.GLW_MapYSize = 50;
	Worm_Glow.GLW_Map = Race01_Worms;


//---- Setting animation...
	Worm_AnimData.StartTime = SYS_TimerTick;
	Worm_AnimData.NrFrames = 39;
	Worm_AnimData.FramesPerSecond = 10;
	Worm_AnimData.PictureSize = 120*50;
	Worm_AnimData.AnimStart = Race01_Worms;
	Worm_AnimData.Pointer = (int*)&Worm_Glow.GLW_Map;



//---- Creating objectlist...
	Race01_Objects = new int[11 + Race01->NrObjects];
	if ( Race01_Objects == 0 )
	{
		SYS_QuitMsg = "ERR: Not enough memory\n";
		return 0;
	}//if

	Race01_Objects[0] = (int)&SRT_OReset;
	Race01_Objects[1] = (int)Race01Object;
	Race01_Objects[2] = (int)&LIT_Init;

	for ( C = 0; C < Race01->NrObjects; C++ )
		Race01_Objects[C + 3] = (int)Race01->ObjectList[C];

	Race01_Objects[C + 3] = (int)&Worm_NULL;
	Race01_Objects[C + 4] = (int)&SRT_OSort;
	Race01_Objects[C + 5] = (int)&SRT_ORun;
	Race01_Objects[C + 6] = (int)0;
	Race01_Objects[C + 7] = (int)&CNV_ONormal;
	Race01_Objects[C + 8] = (int)&KEY_OPressed;
	Race01_Objects[C + 9] = (int)&Null_Timing_Race01;
	Race01_Objects[C +10] = -1;


//---- Creating lightlist...
	Race01_Lights = new int[Race01->NrLights + 1];
	if ( Race01_Lights == 0 )
	{
		SYS_QuitMsg = "ERR: Not enough memory\n";
		return 0;
	}//if

	for ( C = 0; C < Race01->NrLights; C++ )
		Race01_Lights[C] = (int)Race01->LightList[C];

	Race01_Lights[C] = -1;


//---- Setting up flares...
	Light *LightP = NULL;

	LightP = (Light*)Race01_Lights[0];
	Race01_Glow1 = GLW_Load("maps\\flare3.tga", 0, 0);
	if ( Race01_Glow1 == 0 ) return 0;
	LightP->L_Plugins[2] = (int)&GLW_LightPlugin;
	LightP->L_Plugins[3] = (int)Race01_Glow1;
	LightP->L_Plugins[4] = -1;
	Race01_Glow1->GLW_Scale = 0.8;
	Race01_Glow1->GLW_R = LightP->L_R;
	Race01_Glow1->GLW_G = LightP->L_G;
	Race01_Glow1->GLW_B = LightP->L_B;

	LightP = (Light*)Race01_Lights[1];
	Race01_Glow2 = *Race01_Glow1;
	LightP->L_Plugins[2] = (int)&GLW_LightPlugin;
	LightP->L_Plugins[3] = (int)&Race01_Glow2;
	LightP->L_Plugins[4] = -1;
	Race01_Glow2.GLW_Scale = 0.8;
	Race01_Glow2.GLW_R = LightP->L_R;
	Race01_Glow2.GLW_G = LightP->L_G;
	Race01_Glow2.GLW_B = LightP->L_B;

	LightP = (Light*)Race01_Lights[2];
	Race01_Glow3 = *Race01_Glow1;
	LightP->L_Plugins[2] = (int)&GLW_LightPlugin;
	LightP->L_Plugins[3] = (int)&Race01_Glow3;
	LightP->L_Plugins[4] = -1;
	Race01_Glow3.GLW_Scale = 0.8;
	Race01_Glow3.GLW_R = LightP->L_R;
	Race01_Glow3.GLW_G = LightP->L_G;
	Race01_Glow3.GLW_B = LightP->L_B;

	LightP = (Light*)Race01_Lights[3];
	Race01_Glow4 = *Race01_Glow1;
	LightP->L_Plugins[2] = (int)&GLW_LightPlugin;
	LightP->L_Plugins[3] = (int)&Race01_Glow4;
	LightP->L_Plugins[4] = -1;
	Race01_Glow4.GLW_Scale = 0.8;
	Race01_Glow4.GLW_R = LightP->L_R;
	Race01_Glow4.GLW_G = LightP->L_G;
	Race01_Glow4.GLW_B = LightP->L_B;

	return 1;
}//InitRace01


void Worm_Plugin(void)
{
	ANIM_Data *ANIM = (ANIM_Data*)SYS_POffs;

	float Time = ( (float)(SYS_TimerTick - ANIM->StartTime) )/SYS_TicksPerSec;
	Time = Time*ANIM->FramesPerSecond;
	int Frame = (int)(Time + 0.5);
	if ( (Frame > 15) && (Frame < 30) )
	{
		Frame = 15;
	}
	if ( (Frame > 15) && (Frame >= 30) )
	{
		Frame = Frame - 15;
	}

	if ( Frame > ANIM->NrFrames )
	{
		Frame = ANIM->NrFrames;
	}

	*ANIM->Pointer = (int)( (Frame*ANIM->PictureSize) + (int)ANIM->AnimStart );

}//Worm_Plugin



void UnloadRace01(void)
{
	if ( Race01 != 0 ) SCN_Unload((void*)Race01);
	Race01 = 0;
	if ( Race01_Objects != 0) delete [] Race01_Objects;
	Race01_Objects = 0;
	if ( Race01_Lights != 0) delete [] Race01_Lights;
	Race01_Lights = 0;
	if ( Race01_Worms != 0 ) delete [] Race01_Worms;
	Race01_Worms = 0;
	if ( Race01_Glow1 != 0 ) GLW_Unload(Race01_Glow1);
	Race01_Glow1 = 0;

}//UnloadRace01
