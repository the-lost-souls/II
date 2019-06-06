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

#include "h:\scenes\dance\dance.h"
#include "h:\scenes\2d\2d.h"
#include "h:\scenes\city\city.h"
#include "h:\scenes\birdies\birdies.h"
#include "h:\scenes\race04\race04.h"
#include "h:\scenes\race03\race03.h"
#include "h:\scenes\race02\race02.h"
#include "h:\scenes\race01\race01.h"
#include "h:\tunnel\tunnel.h"

extern "C" void MIDAS_CALL TimerTick();

float _46 = 47.0;
float _64 = 64.0;
int FPDump;

int WaitStart;

int ChooseMode(void);
float SetAmbient(void);

MIDASmodule Module = NULL;
MIDASmodulePlayHandle ModPlayHandle = NULL;

typedef struct RAW_FILE_Header
{
	char PAD1[8];
	unsigned char XResHi;
	unsigned char XResLo;
	unsigned char YResHi;
	unsigned char YResLo;
	char Pad2[20];
};
FILE *RAWFile = NULL;

RAW_FILE_Header RAW;


//-----------------------------------------------------




void main(int ArgC, char *ArgV[], char *EnvP[])
{
	int VideoMode = 0;

//---- Setting up MIDAS-timer-stuff...
	if ( MIDASstartup() == FALSE ) goto error;
	if ( MIDASconfig() == FALSE ) goto error;
//	if ( MIDASsetOption(MIDAS_OPTION_FORCE_NO_SOUND, 1) == FALSE) goto error;
	if ( MIDASinit() == FALSE ) goto error;

	if ( MIDASsetTimerCallbacks(SYS_TicksPerSec*1000, FALSE, &TimerTick, NULL, NULL) == FALSE) goto error;

	Aquarium();
	printf("Loading...\n");

//---- Initializing scenes...
//	if ( InitDance() == 0 ) goto error;
//	if ( InitDraw() == 0 ) goto error;
	if ( InitCity() == 0 ) goto error;
	if ( InitBirdies() == 0 ) goto error;
	if ( InitRace04() == 0 ) goto error;
	if ( InitRace03() == 0 ) goto error;
	if ( InitRace02() == 0 ) goto error;
	if ( InitRace01() == 0 ) goto error;
	if ( InitTunnel() == 0 ) goto error;

//---- Ambient light...
	SYS_Ambient = SetAmbient();

//---- Setting up videobuffer...
	if ( VBE_Init() == 0) goto error;
	VideoMode = ChooseMode();
	if ( VBE_SetupMode(VideoMode) == 0 ) goto error;

//---- Loading module...
	Module = MIDASloadModule("ending.it");
	if ( Module == 0 ) goto error;

	ModPlayHandle = MIDASplayModuleSection(Module, 0, 81, 1, TRUE);
	if ( ModPlayHandle == 0 ) goto error;

//	WaitStart = SYS_TimerTick;
//	while ( (SYS_TimerTick - WaitStart) < 750);

      	Tunnel_TimingData.StartTime = SYS_TimerTick;
      	Tunnel->StartTime = Tunnel_TimingData.StartTime;
//	Dance_TimingData.StartTime = SYS_TimerTick - 7019;
//	Dance_11->StartTime = SYS_TimerTick;



//---- Running mainloop...
	SYS_OListOffs = Tunnel_Objects;
	SYS_LListOffs = Tunnel_Lights;
//      SYS_OListOffs = Dance_Objects_11;
//      SYS_LListOffs = Dance_Lights_11;

	__asm{
		pushad
		pushfd

		call	MN_MainLoop

		popfd
		popad

	}

	RAWFile = fopen("shot.raw", "wb");
	if ( RAWFile != NULL )
	{
		memset(&RAW, 0, 32);
		RAW.XResHi = 320 >> 8;
		RAW.XResLo = 320 & 0xFF;
		RAW.YResHi = 200 >> 8;
		RAW.YResLo = 200 & 0xFF;

		RAW.PAD1[0] = 'm';
		RAW.PAD1[1] = 'h';
		RAW.PAD1[2] = 'w';
		RAW.PAD1[3] = 'a';
		RAW.PAD1[4] = 'n';
		RAW.PAD1[5] = 'h';

		RAW.PAD1[6] = 0;
		RAW.PAD1[7] = 4;


		fwrite(&RAW, 32, 1, RAWFile);

		__asm{
			pushad
			pushfd

			mov	esi, SYS_ScreenShot
			mov	edi, SYS_ZBufOffs	//using zbuf as tmpbuf

			mov	ecx, 320
			imul	ecx, 200

		Loop16_24:
			mov	ax, [esi]

			mov	ebx, 0x1F
			and	ebx, eax
			shl	ebx, 3
			mov	[edi + 2], bl		//saving B

			mov	ebx, 0x7E0
			and	ebx, eax
			shr	ebx, 5
			shl	ebx, 3
			mov	[edi + 1], bl

			mov	ebx, 0xF800
			and	ebx, eax
			shr	ebx, 10
			shl	ebx, 3
			mov	[edi + 0], bl

			add	esi, 2
			add	edi, 3
			dec	ecx
			jnz	Loop16_24

			popfd
			popad

		}//asm

		fwrite(SYS_ZBufOffs, 320*200*3, 1, RAWFile);
		fclose(RAWFile);
	}//if


//---- Cleaning up...



	delete [] SYS_DBufOffs;
	delete [] SYS_ZBufOffs;

error:
	__asm{
		push	eax

	      	mov   	ah, 0                     //Init screen number
	       	mov   	al, 3h                   //320x200x256
	       	int   	10h                       //Do it!

		pop	eax

	}//asm

	MIDASclose();

	UnloadDance();
	UnloadDraw();
	UnloadCity();
	UnloadBirdies();
	UnloadRace04();
	UnloadRace03();
	UnloadRace02();
	UnloadRace01();
	UnloadTunnel();

	int ErrorCode = MIDASgetLastError();
	char *ErrorString = MIDASgetErrorMessage(ErrorCode);
	printf("MIDAS: ");
	printf(ErrorString);
	printf("\n");
	printf(SYS_QuitMsg);

}//main




int ChooseMode(void)
{
	printf("\n---- Choose videomode:");

	printf("\n1: 320x200 15 bit    ");
	int Test = VBE_GetMode(320, 200, 15);
	if ( Test == 0 ) printf("(N/A)");
	else if ( !(VBE_ModeList[Test].Mode & 0x4000) ) printf("(N/A)");

	printf("\n2: 320x200 greyscale ");

	printf("\n:");
	char Key;
	cin >> Key;
	Key = toupper(Key);

	int Mode = 0;
	switch ( Key )
	{

	//---- 320x200x15
		case '1':	Mode = VBE_GetMode(320, 200, 15);
				if ( Mode == 0 ) Mode = ChooseMode();
				if ( !(VBE_ModeList[Mode].Mode & 0x4000) ) Mode = ChooseMode();
				VBE_SetMode(320, 200, 15);
				break;

	//---- 320x200x8 (greyscale)
		case '2':
				Mode = VBE_GetMode(320, 200, 8);
				VBE_SetMode(320, 200,  8);
				SYS_ORoutines[ 4] = (int)&CNV_RGrey;

				__asm{  			//setting palette... :)
					pushad
					pushfd

					fld	_64
					fdiv	_46
					fldcw	FPU_Round
					fldz

			                mov   	dx, 0x3C8               //Port to send out startingcolor
			                mov   	al, 0                   //Starting color = 0
			                out   	dx, al                  //Send it!
					inc   	dx                      //Next port = RGB values

					mov	ecx, 47
				GreyScalePal:
					fist	FPDump
					mov	eax, FPDump

					out	dx, al
					out	dx, al
					out	dx, al

					fadd	st, st(1)

					dec	ecx
					jnz	GreyScalePal

					ffree	st(0)
					ffree	st(1)


					popfd
					popad
				}

				break;
		default:	Mode = ChooseMode();
				break;
	}

	return Mode;
}//ChooseMode


float SetAmbient(void)
{
	printf("\n---- Set ambient light value:");

	printf("\n1: Monitor");
	printf("\n2: Projector (more light)");
	printf("\n3: Projector (even more light)");

	printf("\n:");
	char Key;
	cin >> Key;
	Key = toupper(Key);

	int Mode = 0;
	switch ( Key )
	{

	//---- No ambient light...
		case '1':       return 1.0;
				break;


	//---- Some ambient light
		case '2':	return 1.3;
				break;

	//---- Plenty ambient light
		case '3':	return 1.5;
				break;

		default:	return SetAmbient();
				break;
	}

	return Mode;

}

