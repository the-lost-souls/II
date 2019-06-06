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

#include "race04.h"

extern "C" void MIDAS_CALL TimerTick();

float _46 = 47.0;
float _64 = 64.0;
int FPDump;

int ChooseMode(void);

MIDASmodule Module = NULL;
MIDASmodulePlayHandle ModPlayHandle = NULL;

int WaitStart;

//-----------------------------------------------------




void main(int ArgC, char *ArgV[], char *EnvP[])
{
	Aquarium();

	int VideoMode = 0;

//---- Initializing scene...
	if ( InitRace04() == 0 ) goto error;

//---- Setting up videobuffer...
	if ( VBE_Init() == 0) goto error;
	VideoMode = ChooseMode();
	if ( VBE_SetupMode(VideoMode) == 0 ) goto error;


//---- Setting up MIDAS-timer-stuff...
	if ( MIDASstartup() == FALSE ) goto error;
	if ( MIDASloadConfig("soundset") == FALSE ) goto error;
//	if ( MIDASsetOption(MIDAS_OPTION_FORCE_NO_SOUND, 1) == FALSE) goto error;
	if ( MIDASinit() == FALSE ) goto error;

	if ( MIDASsetTimerCallbacks(SYS_TicksPerSec*1000, FALSE, &TimerTick, NULL, NULL) == FALSE) goto error;

//---- Starting backgroundplay...
	if ( MIDASstartBackgroundPlay(0) == FALSE ) goto error;


//---- Loading module...
	Module = MIDASloadModule("h:\\tekonly7.it");
	if ( Module == 0 ) goto error;

	ModPlayHandle = MIDASplayModuleSection(Module, 20, 50, 1, TRUE);
	if ( ModPlayHandle == 0 ) goto error;

//---- Waiting to start...
	Race04_TimingData.StartTime = SYS_TimerTick;
	Race04_1->StartTime = Race04_TimingData.StartTime;

//---- Running mainloop...
	SYS_OListOffs = Race04_Objects_1;
	SYS_LListOffs = Race04_Lights_1;

	__asm{
		pushad
		pushfd

		call	MN_MainLoop

		popfd
		popad

	}


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

	UnloadRace04();

	int ErrorCode = MIDASgetLastError();
	char *ErrorString = MIDASgetErrorMessage(ErrorCode);
	printf("MIDAS: ");
	printf(ErrorString);
	printf("\n");
	printf(SYS_QuitMsg);

}//main


int ChooseMode(void)
{
	printf("\nChoose videomode :\n");

	printf("\n1: 320x200 15 bit    ");
	if ( VBE_GetMode(320, 200, 15) == 0 ) printf("(N/A)");

	printf("\n2: 320x240 15 bit    ");
	if ( VBE_GetMode(320, 240, 15) == 0 ) printf("(N/A)");

	printf("\n3: 320x400 15 bit    ");
	if ( VBE_GetMode(320, 400, 15) == 0 ) printf("(N/A)");

	printf("\n4: 320x480 15 bit    ");
	if ( VBE_GetMode(320, 480, 15) == 0 ) printf("(N/A)");

	printf("\n5: 512x384 15 bit    ");
	if ( VBE_GetMode(512, 384, 15) == 0 ) printf("(N/A)");

	printf("\n6: 640x350 15 bit    ");
	if ( VBE_GetMode(640, 350, 15) == 0 ) printf("(N/A)");

	printf("\n7: 640x400 15 bit    ");
	if ( VBE_GetMode(640, 400, 15) == 0 ) printf("(N/A)");

	printf("\n8: 640x480 15 bit    ");
	if ( VBE_GetMode(640, 480, 15) == 0 ) printf("(N/A)");

	printf("\n9: 800x600 15 bit    ");
	if ( VBE_GetMode(800, 600, 15) == 0 ) printf("(N/A)");

	printf("\nA: 320x200 greyscale ");

//	printf("\nB: 320x200 RGB fake ");

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
				if ( VBE_ModeList[Mode].Mode & 0x4000 ) SYS_ORoutines[ 4] = (int)&CNV_RNormal_Linear;
				VBE_SetMode(320, 200, 15);
				break;

	//---- 320x240x15
		case '2':	Mode = VBE_GetMode(320, 240, 15);
				if ( Mode == 0 ) Mode = ChooseMode();
				if ( VBE_ModeList[Mode].Mode & 0x4000 ) SYS_ORoutines[ 4] = (int)&CNV_RNormal_Linear;
				VBE_SetMode(320, 240, 15);
				break;

	//---- 320x400x15
		case '3':	Mode = VBE_GetMode(320, 400, 15);
				if ( Mode == 0 ) Mode = ChooseMode();
				if ( VBE_ModeList[Mode].Mode & 0x4000 ) SYS_ORoutines[ 4] = (int)&CNV_RNormal_Linear;
				VBE_SetMode(320, 400, 15);
				break;

	//---- 320x480x15
		case '4':	Mode = VBE_GetMode(320, 480, 15);
				if ( Mode == 0 ) Mode = ChooseMode();
				if ( VBE_ModeList[Mode].Mode & 0x4000 ) SYS_ORoutines[ 4] = (int)&CNV_RNormal_Linear;
				VBE_SetMode(320, 480, 15);
				break;

	//---- 512x384x15
		case '5':	Mode = VBE_GetMode(512, 384, 15);
				if ( Mode == 0 ) Mode = ChooseMode();
				if ( VBE_ModeList[Mode].Mode & 0x4000 ) SYS_ORoutines[ 4] = (int)&CNV_RNormal_Linear;
				VBE_SetMode(512, 384, 15);
				break;

	//---- 640x350x15
		case '6':	Mode = VBE_GetMode(640, 350, 15);
				if ( Mode == 0 ) Mode = ChooseMode();
				if ( VBE_ModeList[Mode].Mode & 0x4000 ) SYS_ORoutines[ 4] = (int)&CNV_RNormal_Linear;
				VBE_SetMode(640, 350, 15);
				break;

	//---- 640x400x15
		case '7':	Mode = VBE_GetMode(640, 400, 15);
				if ( Mode == 0 ) Mode = ChooseMode();
				if ( VBE_ModeList[Mode].Mode & 0x4000 ) SYS_ORoutines[ 4] = (int)&CNV_RNormal_Linear;
				VBE_SetMode(640, 400, 15);
				break;

	//---- 640x480x15
		case '8':	Mode = VBE_GetMode(640, 480, 15);
				if ( Mode == 0 ) Mode = ChooseMode();
				if ( VBE_ModeList[Mode].Mode & 0x4000 ) SYS_ORoutines[ 4] = (int)&CNV_RNormal_Linear;
				VBE_SetMode(640, 480, 15);
				break;

	//---- 800x600x15
		case '9':	Mode = VBE_GetMode(800, 600, 15);
				if ( Mode == 0 ) Mode = ChooseMode();
				if ( VBE_ModeList[Mode].Mode & 0x4000 ) SYS_ORoutines[ 4] = (int)&CNV_RNormal_Linear;
				VBE_SetMode(800, 600, 15);
				break;

	//---- 320x200x8 (greyscale)
		case ('A'):
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

/*		case ('B'):
				Mode = VBE_GetMode(320, 200, 8);
				VBE_SetMode(320, 200,  8);
				SYS_ORoutines[ 4] = (int)&CNV_R8bitRGB;

				__asm{  			//setting palette... :)
					pushad
					pushfd

					mov   	dx, 0x3C8               //Port to send out startingcolor
					mov   	al, 0                   //Starting color = 0
					out   	dx, al                  //Send it!
					inc   	dx                      //Next port = RGB values

					xor	ebx, ebx
				RLoop:
					xor	ecx, ecx
				GLoop:
					xor	ebp, ebp
				BLoop:
					mov     eax, ebx
					shl	eax, 3
					out	dx, al

					mov	eax, ecx
					shl	eax, 4
					out	dx, al

					mov	eax, ebp
					shl	eax, 3
					out	dx, al

					inc	ebp
					cmp	ebp, 8
					jb 	BLoop

					inc	ecx
					cmp	ecx, 4
					jb	GLoop

					inc	ebx
					cmp	ebx, 8
					jb	RLoop

					popfd
					popad
				}

				break;*/
		default:	Mode = ChooseMode();
				break;
	}

	return Mode;
}//ChooseMode





