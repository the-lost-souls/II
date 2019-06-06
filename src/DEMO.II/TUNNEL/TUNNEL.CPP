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

#include "h:\scenes\race01\race01.h"

//---- Static data for part 1 (the tunnel)
char TunnelFile[256] = "tunnel\\t4.scn";

int    		*Tunnel_Objects = NULL;
int    		*Tunnel_Lights 	= NULL;
SceneData 	*Tunnel 	= NULL;
GLW_PluginData 	*Tunnel_Glow 	= NULL;

PIC_Data Tunnel_Picture;
BLN_Data Tunnel_Blending;


struct
{
	int Type;
	int Plugins[16];
} Null_Timing_Tunnel;

TIM_Data Tunnel_TimingData = 	{
				0, 0,
				3, 0,
				1180,
				1250,
				1680
			};

//-----------------------------------------------------

void TunnelTiming(void)
{
	TIM_Data *Timer = (TIM_Data*)SYS_POffs;
	if ( Timer->StartFlag == 0 )
	{
		Timer->StartFlag = 1;

		Timer->CurrentKey = 0;
//		Timer->StartTime = SYS_TimerTick;
//		Tunnel->StartTime = SYS_TimerTick;
	}//if

	if ( Timer->CurrentKey == Timer->NrKeys ) Timer->CurrentKey = -1;

	if ( Timer->CurrentKey != -1 )
	{
		int Time = SYS_TimerTick - Timer->StartTime;
		if ( Time > Timer->TimingKeys[Timer->CurrentKey] )
		{
			switch ( Timer->CurrentKey )
			{
				case 0: Tunnel_Objects[6] = (int)&Tunnel_Picture;
					Tunnel_Objects[7] = (int)&Tunnel_Blending;
					break;
				case 1:
				//---- Setting up Tunnel_Blendingobject...

					Tunnel_Blending.BLN_R = 0.0;
					Tunnel_Blending.BLN_G = 0.0;
					Tunnel_Blending.BLN_B = 0.0;

					Tunnel_Blending.BLN_Opacity0 = 0.0;
					Tunnel_Blending.BLN_Opacity1 = 1.0;

					Tunnel_Blending.BLN_StartFlag = 0;
					Tunnel_Blending.BLN_FadingTime = 300;

					break;
				case 2: Race01_TimingData.StartTime = Timer->TimingKeys[Timer->CurrentKey] + Timer->StartTime;
					Race01->StartTime = Timer->TimingKeys[Timer->CurrentKey] + Timer->StartTime;
					Worm_AnimData.StartTime = Timer->TimingKeys[Timer->CurrentKey] + Timer->StartTime;

					SYS_OListOffs = Race01_Objects;
					SYS_LListOffs = Race01_Lights;
					break;


			}
			Timer->CurrentKey++;
		}

	}//if

}//TunnelTiming



int InitTunnel(void)
{
	int C;

	Tunnel_Picture.PIC_Picture = NULL;

//---- Setting up timingstuff...
	Null_Timing_Tunnel.Type = 1;
	Null_Timing_Tunnel.Plugins[0] = (int)&TunnelTiming;
	Null_Timing_Tunnel.Plugins[1] = (int)&Tunnel_TimingData;
	Null_Timing_Tunnel.Plugins[2] = -1;


//---- Loading scene...
	int *TunnelObject = (int*)SCN_Load(TunnelFile, 0);
	if ( TunnelObject == 0 ) return 0;
	Tunnel = (SceneData*)TunnelObject[2];


//---- Loading logo...
	Tunnel_Picture.PIC_Type = 15;
	Tunnel_Picture.PIC_Plugins[0] = -1;

	Tunnel_Picture.PIC_Picture = TGA_Load("gfx\\logo01.tga", 0, 0);
	if ( Tunnel_Picture.PIC_Picture == 0 ) return 0;
	Tunnel_Picture.PIC_Width = TGA_Width;
	Tunnel_Picture.PIC_Heigth = TGA_Heigth;
	Tunnel_Picture.PIC_XPos = (VBE_XRes - TGA_Width) / 2;
	Tunnel_Picture.PIC_YPos = (VBE_YRes - TGA_Heigth) / 2;


//---- Creating objectlist...
	Tunnel_Objects = new int[11 + Tunnel->NrObjects];
	if ( Tunnel_Objects == 0 )
	{
		SYS_QuitMsg = "ERR: Not enough memory\n";
		return 0;
	}//if

	Tunnel_Objects[0] = (int)&SRT_OReset;
	Tunnel_Objects[1] = (int)TunnelObject;
	Tunnel_Objects[2] = (int)&LIT_Init;

	for ( C = 0; C < Tunnel->NrObjects; C++ )
		Tunnel_Objects[C + 3] = (int)Tunnel->ObjectList[C];

	Tunnel_Objects[C + 3] = (int)&SRT_OSort;
	Tunnel_Objects[C + 4] = (int)&SRT_ORun;
	Tunnel_Objects[C + 5] = 0;
	Tunnel_Objects[C + 6] = 0;
	Tunnel_Objects[C + 7] = (int)&CNV_ONormal;
	Tunnel_Objects[C + 8] = (int)&KEY_OPressed;
	Tunnel_Objects[C + 9] = (int)&Null_Timing_Tunnel;
	Tunnel_Objects[C +10] = -1;



//---- Creating lightlist...
	Tunnel_Lights = new int[Tunnel->NrLights + 1];
	if ( Tunnel_Lights == 0 )
	{
		SYS_QuitMsg = "ERR: Not enough memory\n";
		return 0;
	}//if

	for ( C = 0; C < Tunnel->NrLights; C++ )
		Tunnel_Lights[C] = (int)Tunnel->LightList[C];

	Tunnel_Lights[C] = -1;



//---- Setting up flares...
	Light *LightP = NULL;

	LightP = (Light*)Tunnel_Lights[0];
	Tunnel_Glow = GLW_Load("maps\\flare2.tga", 0, 0);
	if ( Tunnel_Glow == 0 ) return 0;
	LightP->L_Plugins[2] = (int)&GLW_LightPlugin;
	LightP->L_Plugins[3] = (int)Tunnel_Glow;
	LightP->L_Plugins[4] = -1;
	Tunnel_Glow->GLW_Scale = 0.025;
	Tunnel_Glow->GLW_R = LightP->L_R;
	Tunnel_Glow->GLW_G = LightP->L_G;
	Tunnel_Glow->GLW_B = LightP->L_B;


//---- Setting up Tunnel_Blendingobject...
	Tunnel_Blending.BLN_Type = 16;
	Tunnel_Blending.BLN_Plugins[0] = -1;

	Tunnel_Blending.BLN_R = 1.0;
	Tunnel_Blending.BLN_G = 1.0;
	Tunnel_Blending.BLN_B = 1.0;

	Tunnel_Blending.BLN_Opacity0 = 1.0;
	Tunnel_Blending.BLN_Opacity1 = 0.0;

	Tunnel_Blending.BLN_StartFlag = 0;
	Tunnel_Blending.BLN_FadingTime = 100;

	return 1;
}//InitTunnel


void UnloadTunnel(void)
{
	if ( Tunnel != 0 ) SCN_Unload((void*)Tunnel);
	Tunnel = NULL;

	if ( Tunnel_Objects != 0) delete [] Tunnel_Objects;
	Tunnel_Objects = NULL;

	if ( Tunnel_Lights != 0) delete [] Tunnel_Lights;
	Tunnel_Lights = NULL;

	if ( Tunnel_Glow != 0 ) delete [] Tunnel_Glow;
	Tunnel_Glow = 0;

	if ( Tunnel_Picture.PIC_Picture != 0 ) delete [] Tunnel_Picture.PIC_Picture;
	Tunnel_Picture.PIC_Picture = 0;

}//UnloadTunnel












