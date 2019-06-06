#include "w:\general\var.h"
#include "w:\midas112\include\midasdll.h"
#include <stdio.h>
#include <iostream.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>
#include <fstream.h>

extern "C" void MIDAS_CALL TimerTick();

MIDASmodule Module = NULL;
MIDASmodulePlayHandle ModPlayHandle = NULL;

typedef struct{
	int Order;
	int Time;
}Data;


Data Timing[5000];
int C = 0;


ofstream InFile;


MIDASplayStatus Playstatus;

void main(int ArgC, char *ArgV[], char *EnvP[])
{
//---- Setting up MIDAS-timer-stuff...
	if ( MIDASstartup() == FALSE ) goto error;
	if ( MIDASloadConfig("soundset") == FALSE ) goto error;
//	if ( MIDASsetOption(MIDAS_OPTION_FORCE_NO_SOUND, 1) == FALSE) goto error;
	if ( MIDASinit() == FALSE ) goto error;

	if ( MIDASsetTimerCallbacks(100*1000, FALSE, &TimerTick, NULL, NULL) == FALSE) goto error;

//---- Starting backgroundplay...
	if ( MIDASstartBackgroundPlay(0) == FALSE ) goto error;


//---- Loading module...
	Module = MIDASloadModule("h:\\tekonly7.it");
	if ( Module == 0 ) goto error;

	ModPlayHandle = MIDASplayModuleSection(Module, 0, 81, 1, TRUE);
	if ( ModPlayHandle == 0 ) goto error;

	Timing[C].Time = SYS_TimerTick;
	Timing[C].Order = 0;

	Playstatus.position = 0;
	while ( Playstatus.position <= 80 )
	{
		MIDASgetPlayStatus(ModPlayHandle, &Playstatus);
		if ( (Playstatus.row == 0) ) {

			if (Timing[C].Order != Playstatus.position )
			{
				C++;
				Timing[C].Time = SYS_TimerTick;
				Timing[C].Order = Playstatus.position;
			}
		}//if

	}


error:

	MIDASclose();

	int ErrorCode = MIDASgetLastError();
	char *ErrorString = MIDASgetErrorMessage(ErrorCode);
	printf("MIDAS: ");
	printf(ErrorString);

//---- Opening file...
	InFile.open("time.txt", ios::out);
	if ( InFile.fail() )
	{
		printf("ERR: Couldn't open file ");
	}//if
	else
	{
		for ( int C2 = 0; C2 < C; C2++ )
		{
			InFile << "Order " << Timing[C2].Order << ": " << Timing[C2].Time << endl;
		}
		InFile.close();
	}

}//main
