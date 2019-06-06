#include "w:\general\var.h"
#include <stdio.h>

void Aquarium(void)
{
	__asm{
		push	eax

	      	mov   	ah, 0                     //Init screen number
	       	mov   	al, 3h                   //
	       	int   	10h                       //Do it!

		pop	eax
	}//asm

	printf("Aquarium ---- Cyberfish/TLS 1998\n\n");

//---- Setting FPU control-data...
	__asm{

		push	eax

                fstcw  FPU_Original             ;Setting controlword-values
                fldcw  FPU_24
                mov    ax, FPU_24
                or     FPU_Round, ax
                or     FPU_Chop, ax
                or     FPU_RoundUp, ax
                or     FPU_RoundDown, ax

		pop	eax

	}//asm



}//Aquarium
