#include "div.h"
#include "savelwo.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

int AnalyzeParameters(int ArgC, char *ArgV[], char *EnvP[]);

char InFile[256] = "\0";
char OutFile[256] = "\0";
int Type;

void main(int ArgC, char *ArgV[], char *EnvP[])
{
	ClrScr(7, 0);
	printf("LWO-converter v2.00 (011) Cyberfish/TLS\n");

	if (AnalyzeParameters(ArgC, ArgV, EnvP) == 0) return;

	printf("Opening ");
	printf(InFile);
	printf("\n");
	void *LWO;
	LWO = ReadFile(InFile);
	if ( LWO == 0 )
	{
		printf("(");
		printf(InFile);
		printf(")\n");
		return;
	}//if

	unsigned char *Bin;
	Bin = (unsigned char*)SaveLWO(LWO, Type);

	delete [] LWO;

	if ( Bin == 0 )
	{
		printf("(");
		printf(InFile);
		printf(")\n");
		return;
	}//if


//---- Saving file...
	FILE *BINFile;
	BINFile = fopen(OutFile, "wb");
	if (BINFile == NULL)
	{
		printf("ERR: Error opening file (");
		printf(OutFile);
		printf(")\n");
		delete [] Bin;
		return;
	}

	if (fwrite(Bin, ((FileHeader*)&Bin[74])->FileSize, 1, BINFile) < 1)
	{
		printf("ERR: Error writing file (");
		printf(OutFile);
		printf(")\n");
		delete [] Bin;
		fclose(BINFile);
		return;
	}

	delete [] Bin;
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
				case 'T' :      if (Str2Int(&ArgV[C1][2], Type, '\0') != 0)
						{
							printf("ERR: Error in type-parameter\n");
							return 0;
						}//if
						break;

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
		printf("ERR: No .lwo-file defined\n");
		return 0;
	}//if


//---- Extending filenames...

	if ( SearchStr(InFile, '.') == 0) *(int*)&InFile[strlen(InFile)] = 'OWL.';
	if ( OutFile[0] == '\0' )
	{
		strcpy(OutFile, InFile);
		*(int*)&OutFile[strlen(InFile) - 4] = 'NIB.';
	}

	return 1;
}//AnalyzeParameters
