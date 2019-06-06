#include "savelws.h"
#include "div.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

int AnalyzeParameters(int ArgC, char *ArgV[], char *EnvP[]);

char InFile[256] = "\0";
char OutFile[256] = "\0";


void main(int ArgC, char *ArgV[], char *EnvP[])
{
	ClrScr(7, 0);
	printf("LWS-converter v1.00 (021) Cyberfish/TLS\n");

	if (AnalyzeParameters(ArgC, ArgV, EnvP) == 0) return;


	char *SceneFile;
	SceneFile = (char*)SaveLWS(InFile);
	if ( SceneFile == 0 )
	{
		printf("(");
		printf(InFile);
		printf(")");
		return;
	}

//---- Saving file...
	FILE *BINFile;
	BINFile = fopen(OutFile, "wb");
	if (SceneFile == NULL)
	{
		printf("ERR: Error opening file (");
		printf(OutFile);
		printf(")\n");
		delete [] SceneFile;
		return;
	}

	if (fwrite(SceneFile, *(int*)&SceneFile[268], 1, BINFile) < 1)
	{
		printf("ERR: Error writing file (");
		printf(OutFile);
		printf(")\n");
		delete [] SceneFile;
		fclose(BINFile);
		return;
	}

	delete [] SceneFile;
	fclose(BINFile);

}//main




int AnalyzeParameters(int ArgC, char *ArgV[], char *EnvP[])
{
	for ( int C1 = 1; C1 < ArgC; C1++ )
	{

	//---- Making parameter uppercase...
		int C3 = strlen(ArgV[C1]);
		for ( int C2 = 0; C2 < C3; C2++ ) ArgV[C1][C2] = toupper(ArgV[C1][C2]);


	//---- Switch?
		if ( ArgV[C1][0] == '-' )
		{
			switch ( ArgV[C1][1] )
			{
				case 'P' :	NoPath = 1;
						break;

				default: 	printf("ERR: Stupid parameter detected!\n");
						return 0;

			}//switch
		}//if

		else if ( InFile[0] == 0 ) strcpy(InFile, ArgV[C1]);
		else if ( OutFile[0] == 0 ) strcpy(OutFile, ArgV[C1]);
		else
		{
			printf("ERR: Stupid parameter detected!\n");
			return 0;
		}//else
	}//for


	if (InFile[0] == '\0')
	{
		printf("ERR: No .lws-file defined\n");
		return 0;
	}//if


//---- Extending filenames...

	if ( SearchStr(InFile, '.') == 0) *(int*)&InFile[strlen(InFile)] = 'SWL.';
	if ( OutFile[0] == '\0' )
	{
		strcpy(OutFile, InFile);
		*(int*)&OutFile[strlen(InFile) - 4] = 'NCS.';
	}

	return 1;
}//AnalyzeParameters

