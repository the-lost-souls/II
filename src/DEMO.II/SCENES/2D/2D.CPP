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
#include "w:\general\texture.h"
#include <stdio.h>
#include <iostream.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "2d.h"
#include "h:\scenes\dance\dance.h"

void MapCoordPlugin(void);
void Rotate2D(float X0, float Y0, float Angle, float &X1, float &Y1);
void DrawDings(int X, int Y, float R, float G, float B, void *Brush);

//---- Static data for part 1 (the tunnel)
char DrawFile[256] = "scenes\\2d\\3.scn";
int *Draw_Objects 	= NULL;
int *Draw_Lights 	= NULL;
SceneData *Draw 	= NULL;
BLN_Data BLN_2d;
GLW_PluginData *DrawGlow01 = NULL;
GLW_PluginData DrawGlow02;
GLW_PluginData DrawGlow03;
GLW_PluginData DrawGlow04;
GLW_PluginData DrawGlow05;
GLW_PluginData DrawGlow06;
GLW_PluginData DrawGlow07;
GLW_PluginData DrawGlow08;

void *DrawMap   = NULL;
void *DrawBrush = NULL;

#define BrushSizeX 20
#define BrushSizeY 20

MapCoordPluginData MapCoordData;

struct
{
	int Type;
	int Plugins[16];
} Null_Timing_2d;

TIM_Data Draw_TimingData = 	{
				0, 0,
				2, 0,
				2400,
				2552,
			};

//-----------------------------------------------------
void DrawTiming(void)
{
	TIM_Data *Timer = (TIM_Data*)SYS_POffs;
	if ( Timer->StartFlag == 0 )
	{
		Timer->StartFlag = 1;

		Timer->CurrentKey = 0;
//		Timer->StartTime = SYS_TimerTick;
//		Draw->StartTime = SYS_TimerTick;
	}//if

	if ( Timer->CurrentKey == Timer->NrKeys ) Timer->CurrentKey = -1;

	if ( Timer->CurrentKey != -1 )
	{
		int Time = SYS_TimerTick - Timer->StartTime;
		if ( Time > Timer->TimingKeys[Timer->CurrentKey] )
		{
			switch ( Timer->CurrentKey )
			{
				case 0: BLN_2d.BLN_StartFlag = 0;
					BLN_2d.BLN_FadingTime = 100;
					break;

				case 1: Dance_TimingData.StartTime = Timer->TimingKeys[Timer->CurrentKey] + Timer->StartTime;
					Dance_01->StartTime = Timer->TimingKeys[Timer->CurrentKey] + Timer->StartTime;

					SYS_PRoutines[3] = (int)&T05_Call;

					SYS_OListOffs = Dance_Objects_01;
					SYS_LListOffs = Dance_Lights_01;
					break;

			}
			Timer->CurrentKey++;
		}

	}//if

}//DrawTiming


void Rotate2D(float X0, float Y0, float Angle, float &X1, float &Y1)
{
	float XOut, YOut;
	__asm{
		fld	Angle
		fcos
		fld	Angle
		fsin

		fld	X0
		fmul    st, st(2)
		fld	Y0
		fmul	st, st(2)
		fsubp	st(1), st
		fstp	XOut

		fmul    X0
		fxch	st(1)
		fmul	Y0
		faddp	st(1), st
		fstp	YOut

	}//asm
	X1 = XOut;
	Y1 = YOut;

//	X1 = X0*cos(Angle) - Y0*sin(Angle);
//	Y1 = Y0*cos(Angle) + X0*sin(Angle);
}//Rotate2D


void MapCoordPlugin(void)
{
	int C1, C2;
	Header *VHeader = (Header*)SYS_PCaller;
	Vertice *Vert;
	Face *FaceP;
	MapCoordPluginData *Data = (MapCoordPluginData*)SYS_POffs;


//---- Fixing mappingcoords...
	Data->Angle = (SYS_TimerTick - Data->StartTime)*0.003;


	Vert = VHeader->H_VertOffs;
	C2 = VHeader->H_Vertices;
	for ( C1 = 0; C1 < C2; C1++)
	{
		float U, V, Distance, UCenter, VCenter;

		U = Data->MapCoords[C1*2 + 0];
		V = Data->MapCoords[C1*2 + 1];

		U = U + sin(U*10)/20;
		V = V + sin(V*10)/20;

		Rotate2D(U, V, Data->Angle*1.23, U, V);

	//---- Swirl 1
		Rotate2D(Data->UCenter1, Data->VCenter1, Data->Angle, UCenter, VCenter);
		U -= UCenter;
		V -= VCenter;
		Distance = sqrt((U*U) + (V*V)); //max = 0.5

		float Angle = (0.20 - Distance);
		if ( Angle < 0 ) Angle = 0;

		Angle = Angle*Pi*4*sin(Data->Angle*1.26);


		Rotate2D(	U,
				V,
				Angle,
				U,
				V);

		U += UCenter;
		V += VCenter;

	//---- Swirl 2
		Rotate2D(Data->UCenter2, Data->VCenter2, -1.68*Data->Angle, UCenter, VCenter);
		U -= UCenter;
		V -= VCenter;
		Distance = sqrt((U*U) + (V*V)); //max = 0.5

		Angle = (0.20 - Distance);
		if ( Angle < 0 ) Angle = 0;

		Angle = Angle*Pi*6*sin(Data->Angle*2.234);


		Rotate2D(	U,
				V,
				Angle,
				U,
				V);

		U += UCenter;
		V += VCenter;


	//---- Transforming...

		U = (U + 0.5)*500;
		V = (V + 0.5)*500;
		Vert[C1].V_U = U;
		Vert[C1].V_V = V;
	}//for

//---- Copying mapping coords...
	FaceP = VHeader->H_FaceOffs;
	C2 = VHeader->H_Faces;
	for ( C1 = 0; C1 < C2; C1++ )
	{
		Vert = FaceP[C1].F_V0Offs;
		FaceP[C1].F_U0 = Vert->V_U;
		FaceP[C1].F_V0 = Vert->V_V;
		Vert = FaceP[C1].F_V1Offs;
		FaceP[C1].F_U1 = Vert->V_U;
		FaceP[C1].F_V1 = Vert->V_V;
		Vert = FaceP[C1].F_V2Offs;
		FaceP[C1].F_U2 = Vert->V_U;
		FaceP[C1].F_V2 = Vert->V_V;
	}//for



//---- Draw-stuff...
	int CurrentTime = SYS_TimerTick;
	float Seconds = (float)(CurrentTime - Draw->StartTime)/(float)SYS_TicksPerSec;
	float InFrame = Draw->FirstFrame + Seconds*Draw->FramesPerSecond;

	float TempMatrix[12];

	for ( C1 = 1; C1 <= 8; C1++ )
	{
		ZeroMatrix(TempMatrix);
		GetObjectMatrix(InFrame, (SceneObjectData*)&Draw->ObjectData[C1], TempMatrix);


		float U = ( (TempMatrix[11] /200) + 0.5);
		float V = ( (TempMatrix[ 3] /200) + 0.5);

		if ( (U <= 1) && (U >= 0) && (V <= 1) && (V >= 0) )
		{
			U *= 33;
			V *= 33;
			Vertice *Vert = (Vertice*)&VHeader->H_VertOffs[(int)U + ( ((int)V)*33 )];

			U -= (int)U;
			V -= (int)V;

			float USlopeL = Vert[33].V_U - Vert[0].V_U;
			float USlopeR = Vert[34].V_U - Vert[1].V_U;

			float VSlopeL = Vert[33].V_V - Vert[0].V_V;
			float VSlopeR = Vert[34].V_V - Vert[1].V_V;

			float ULeft   = USlopeL*V + Vert[0].V_U;
			float URight  = USlopeR*V + Vert[1].V_U;

			float VLeft   = VSlopeL*V + Vert[0].V_V;
			float VRight  = VSlopeR*V + Vert[1].V_V;

			float USlope = URight - ULeft;
			float VSlope = VRight - VLeft;

			int FinalU = (int)((ULeft + USlope*U)+0.5);
			int FinalV = (int)((VLeft + VSlope*U)+0.5);

			switch ( C1 )
			{
				case 1: DrawDings(FinalU, FinalV, 0.2, 0.2, 0.8, DrawBrush);
					break;
				case 2: DrawDings(FinalU, FinalV, 0.8, 0.8, 0.2, DrawBrush);
					break;
				case 3: DrawDings(FinalU, FinalV, 1.0, 0.0, 0.4, DrawBrush);
					break;
				case 4: DrawDings(FinalU, FinalV, 0.8, 0.4, 0.2, DrawBrush);
					break;

				case 5: DrawDings(FinalU, FinalV, 0.2, 0.2, 0.8, DrawBrush);
				      	break;
				case 6: DrawDings(FinalU, FinalV, 0.8, 0.8, 0.2, DrawBrush);
					break;
				case 7: DrawDings(FinalU, FinalV, 0.8, 0.2, 0.2, DrawBrush);
					break;
				case 8: DrawDings(FinalU, FinalV, 0.8, 0.4, 0.2, DrawBrush);
					break;
			}//switch
		}//if
	}//for


}//DrawPlugin



int InitDraw(void)
{
	int C;

//---- Loading scene...
	int *DrawObject = (int*)SCN_Load(DrawFile, 0);
	if ( DrawObject == 0 ) return 0;
	Draw = (SceneData*)DrawObject[2];

	DrawMap = new unsigned char[512*512*2];
	if ( DrawMap == 0 )
	{
		SYS_QuitMsg = "ERR: Not enough memory\n";
		return 0;
	}

	Header *VHeader = (Header*)Draw->ObjectData[0].Offset;
	Material *TempMaterial = &VHeader->H_MatrOffs[0];
	TempMaterial->M_MapOffset = DrawMap;
	TempMaterial->M_MapWidth = 512;
	TempMaterial->M_MapHeigth = 512;

	for ( C = 0; C < VHeader->H_Faces; C++ )
	{
		Face *TempFace = &VHeader->H_FaceOffs[C];
		TempFace->F_U0 = (TempFace->F_U0/4) *511;
		TempFace->F_V0 = (TempFace->F_V0/4) *511;

		TempFace->F_U1 = (TempFace->F_U1/4) *511;
		TempFace->F_V1 = (TempFace->F_V1/4) *511;

		TempFace->F_U2 = (TempFace->F_U2/4) *511;
		TempFace->F_V2 = (TempFace->F_V2/4) *511;
	}

	TXT_UnloadTexture("maps\\grid.tga"); //Get the old map out...

	__asm{
		pushad

		mov	edi, DrawMap
		mov	ecx, 131072
		mov	eax, 0
		rep	stosd

		popad
	}





//---- Setting up blendingobject...
	BLN_2d.BLN_Type = 16;
	BLN_2d.BLN_Plugins[0] = -1;

	BLN_2d.BLN_R = 0.0;
	BLN_2d.BLN_G = 0.0;
	BLN_2d.BLN_B = 0.0;

	BLN_2d.BLN_Opacity0 = 0.0;
	BLN_2d.BLN_Opacity1 = 1.0;

	BLN_2d.BLN_StartFlag = 0;
	BLN_2d.BLN_FadingTime = 50000000;


//---- Setting up timerplugin...
	Null_Timing_2d.Type = 1;
	Null_Timing_2d.Plugins[0] = (int)&DrawTiming;
	Null_Timing_2d.Plugins[1] = (int)&Draw_TimingData;
	Null_Timing_2d.Plugins[2] = -1;

//---- Creating objectlist...
	Draw_Objects = new int[11 + Draw->NrObjects];
	if ( Draw_Objects == 0 )
	{
		SYS_QuitMsg = "ERR: Not enough memory\n";
		return 0;
	}//if

	Draw_Objects[0] = (int)&Null_Timing_2d;
	Draw_Objects[1] = (int)&SRT_OReset;
	Draw_Objects[2] = (int)DrawObject;
	Draw_Objects[3] = (int)&LIT_Init;

	for ( C = 0; C < Draw->NrObjects; C++ )
		Draw_Objects[C + 4] = (int)Draw->ObjectList[C];

	Draw_Objects[C + 4] = 0;
	Draw_Objects[C + 5] = (int)&SRT_OSort;
	Draw_Objects[C + 6] = (int)&SRT_ORun;
	Draw_Objects[C + 7] = (int)&BLN_2d;
	Draw_Objects[C + 8] = (int)&CNV_ONormal;
	Draw_Objects[C + 9] = (int)&KEY_OPressed;
	Draw_Objects[C + 10] = -1;


//---- Creating lightlist...
	Draw_Lights = new int[Draw->NrLights + 1];
	if ( Draw_Lights == 0 )
	{
		SYS_QuitMsg = "ERR: Not enough memory\n";
		return 0;
	}//if

	for ( C = 0; C < Draw->NrLights; C++ )
		Draw_Lights[C] = (int)Draw->LightList[C];

	Draw_Lights[C] = -1;



//---- Mapcoord-plugin-stuff...
	Header 	*HeaderP= (Header*)Draw->ObjectData[0].Offset;
	MapCoordData.MapCoords = new float[HeaderP->H_Vertices*2];
	if ( MapCoordData.MapCoords == 0 )
	{
		SYS_QuitMsg = "ERR: Not enough memory\n";
		return 0;
	}//if

	MapCoordData.Angle = 0;
	MapCoordData.UCenter1 = 0.1;
	MapCoordData.VCenter1 = 0;
	MapCoordData.UCenter2 = 0.2;
	MapCoordData.VCenter2 = 0;


	Face 	*FaceP 	= HeaderP->H_FaceOffs;
	Vertice *VertP 	= NULL;

	for ( C = 0; C < HeaderP->H_Faces; C++ )
	{
		VertP = FaceP[C].F_V0Offs;
		VertP->V_U = FaceP[C].F_U0;
		VertP->V_V = FaceP[C].F_V0;

		VertP = FaceP[C].F_V1Offs;
		VertP->V_U = FaceP[C].F_U1;
		VertP->V_V = FaceP[C].F_V1;

		VertP = FaceP[C].F_V2Offs;
		VertP->V_U = FaceP[C].F_U2;
		VertP->V_V = FaceP[C].F_V2;
	}//for

	VertP = HeaderP->H_VertOffs;
	for ( C = 0; C < HeaderP->H_Vertices; C++ )
	{
		float U = VertP[C].V_U/700;
		float V = VertP[C].V_V/700;
		U -= 0.5;
		V -= 0.5;

		U = U*0.707106781187;
		V = V*0.707106781187;

		MapCoordData.MapCoords[C*2 + 0] = U;
		MapCoordData.MapCoords[C*2 + 1] = V;
	}//for

	HeaderP->H_Plugins[0] = (int)&MapCoordPlugin;
	HeaderP->H_Plugins[1] = (int)&MapCoordData;
	HeaderP->H_Plugins[2] = -1;


//---- Setting up flares...
	HeaderP = (Header*)Draw->ObjectData[1].Offset;
	DrawGlow01 = GLW_Load("maps\\flare4.tga", 0, 0);
	if ( DrawGlow01 == 0 ) return 0;
	HeaderP->H_Plugins[0] = (int)&GLW_Plugin;
	HeaderP->H_Plugins[1] = (int)DrawGlow01;
	HeaderP->H_Plugins[2] = -1;
	DrawGlow01->GLW_Scale = 0.3;
	DrawGlow01->GLW_R = 0.2;
	DrawGlow01->GLW_G = 0.2;
	DrawGlow01->GLW_B = 0.8;

	HeaderP = (Header*)Draw->ObjectData[2].Offset;
	DrawGlow02 = *DrawGlow01;
	HeaderP->H_Plugins[0] = (int)&GLW_Plugin;
	HeaderP->H_Plugins[1] = (int)&DrawGlow02;
	HeaderP->H_Plugins[2] = -1;
	DrawGlow02.GLW_R = 0.8;
	DrawGlow02.GLW_G = 0.8;
	DrawGlow02.GLW_B = 0.2;

	HeaderP = (Header*)Draw->ObjectData[3].Offset;
	DrawGlow03 = *DrawGlow01;
	HeaderP->H_Plugins[0] = (int)&GLW_Plugin;
	HeaderP->H_Plugins[1] = (int)&DrawGlow03;
	HeaderP->H_Plugins[2] = -1;
	DrawGlow03.GLW_R = 1.0;
	DrawGlow03.GLW_G = 0.0;
	DrawGlow03.GLW_B = 0.4;

	HeaderP = (Header*)Draw->ObjectData[4].Offset;
	DrawGlow04 = *DrawGlow01;
	HeaderP->H_Plugins[0] = (int)&GLW_Plugin;
	HeaderP->H_Plugins[1] = (int)&DrawGlow04;
	HeaderP->H_Plugins[2] = -1;
	DrawGlow04.GLW_R = 0.8;
	DrawGlow04.GLW_G = 0.4;
	DrawGlow04.GLW_B = 0.2;

	HeaderP = (Header*)Draw->ObjectData[5].Offset;
	DrawGlow05 = *DrawGlow01;
	HeaderP->H_Plugins[0] = (int)&GLW_Plugin;
	HeaderP->H_Plugins[1] = (int)&DrawGlow05;
	HeaderP->H_Plugins[2] = -1;
	DrawGlow05.GLW_R = 0.2;
	DrawGlow05.GLW_G = 0.2;
	DrawGlow05.GLW_B = 0.8;

	HeaderP = (Header*)Draw->ObjectData[6].Offset;
	DrawGlow06 = *DrawGlow01;
	HeaderP->H_Plugins[0] = (int)&GLW_Plugin;
	HeaderP->H_Plugins[1] = (int)&DrawGlow06;
	HeaderP->H_Plugins[2] = -1;
	DrawGlow06.GLW_R = 0.8;
	DrawGlow06.GLW_G = 0.8;
	DrawGlow06.GLW_B = 0.2;

	HeaderP = (Header*)Draw->ObjectData[7].Offset;
	DrawGlow07 = *DrawGlow01;
	HeaderP->H_Plugins[0] = (int)&GLW_Plugin;
	HeaderP->H_Plugins[1] = (int)&DrawGlow07;
	HeaderP->H_Plugins[2] = -1;
	DrawGlow07.GLW_R = 1.0;
	DrawGlow07.GLW_G = 0.0;
	DrawGlow07.GLW_B = 0.4;

	HeaderP = (Header*)Draw->ObjectData[8].Offset;
	DrawGlow08 = *DrawGlow01;
	HeaderP->H_Plugins[0] = (int)&GLW_Plugin;
	HeaderP->H_Plugins[1] = (int)&DrawGlow08;
	HeaderP->H_Plugins[2] = -1;
	DrawGlow08.GLW_R = 0.8;
	DrawGlow08.GLW_G = 0.4;
	DrawGlow08.GLW_B = 0.2;


//---- Making brushes....

//---- Loading TGA for transparency map...

	unsigned char *TGATemp = TGA_Load("maps\\brush4.tga", 0, 0);
	if ( TGATemp == 0 ) return 0;

	int NrPixels = TGA_Width*TGA_Heigth;

//---- Converting 16-bit TGA-picture to 4-bit greyscale (saved as bytes)
	DrawBrush = new unsigned char[TGA_Width*TGA_Heigth];
	if ( DrawBrush == 0 )
	{
		SYS_QuitMsg = "ERR: Not enough memory.\n";
		delete [] TGATemp;
		return 0;
	}

	void *PointerA = (void*)TGATemp;
	void *PointerB = (void*)DrawBrush;

	__asm{
		pushad
		pushfd

		mov	ecx, NrPixels
		mov	esi, PointerA
		mov	edi, PointerB

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

		sub	eax, 3
		cmp	eax, 0
		jge	OK
		xor	eax, eax
	OK:
		mov	b[edi], al


		add	esi, 2
		inc	edi

		dec	ecx
		jnz	GreyScaleLoop

		popfd
		popad
	}//asm

	delete [] TGATemp;

	return 1;
}//InitDraw


//----------- Hurry... :)

float FPTemp1, FPTemp2;
float _31p0 = 31.0;
float _16p0 = 16.0;

short int LUT[32*16*3];

void DrawDings(int X, int Y, float R, float G, float B, void *Brush)
{
	__asm{
		pushad
		pushfd

		fldcw	FPU_Round
//---- Setting red lut...
		mov	edi, OFFSET LUT
		xor	edx, edx

RColLoop:
		xor	ecx, ecx

RTranspLoop:
						;st0  st1  st2  st3
		fld	R			;R
		fmul	_31p0
		mov	FPTemp1, edx
		fisub	FPTemp1  		;R-C

		fdiv	_16p0   		;slope
		mov	FPTemp2, ecx
		fimul	FPTemp2
		fiadd   FPTemp1

		fistp	FPTemp1 		;round and store

		mov	eax, FPTemp1    	;store into lut...
		shl	eax, 10
		mov	[edi], ax		;

		add	edi, 2

		inc	ecx
		cmp	ecx, 16
		jb	RTranspLoop

		inc	edx
		cmp	edx, 32
		jb	RColLoop


//---- Setting green lut...
		xor	edx, edx

	GColLoop:
		xor	ecx, ecx

	GTranspLoop:
						;st0  st1  st2  st3
		fld	G 			;R
		fmul	_31p0
		mov	FPTemp1, edx
		fisub	FPTemp1  		;R-C

		fdiv	_16p0   		;slope
		mov	FPTemp2, ecx
		fimul	FPTemp2
		fiadd   FPTemp1

		fistp	FPTemp1 		;round and store

		mov	eax, FPTemp1    	;store into lut...
		shl	eax, 5
		mov	[edi], ax		;

		add	edi, 2

		inc	ecx
		cmp	ecx, 16
		jb	GTranspLoop

		inc	edx
		cmp	edx, 32
		jb	GColLoop


//---- Setting blue lut...
		xor	edx, edx

	BColLoop:
		xor	ecx, ecx

	BTranspLoop:
						;st0  st1  st2  st3
		fld	B 			;R
		fmul	_31p0
		mov	FPTemp1, edx
		fisub	FPTemp1  		;R-C

		fdiv	_16p0   		;slope
		mov	FPTemp2, ecx
		fimul	FPTemp2
		fiadd   FPTemp1

		fistp	FPTemp1 		;round and store

		mov	eax, FPTemp1    	;store into lut...
		mov	[edi], ax		;

		add	edi, 2

		inc	ecx
		cmp	ecx, 16
		jb	BTranspLoop

		inc	edx
		cmp	edx, 32
		jb	BColLoop

		mov	esi, Brush

		mov	eax, X
		sub	eax, 10
		mov	edi, Y
		sub	edi, 10
		imul	edi, 512
		add	edi, eax
		shl	edi, 1
		add	edi, DrawMap




		xor	eax, eax
		mov	edx, BrushSizeY
YLoop:
		push	edx
		mov	ecx, BrushSizeX
XLoop:

		mov     al, [esi]		//Load transparency-value

		xor	edx, edx
		mov	dx, [edi] 		//Get pixel

		xor	ebp, ebp
		mov	ebx, 0x07C00  		//Get new red value
		and	ebx, edx
		shr	ebx, 6
		add	ebx, eax
		mov	bp, LUT[ebx*2]

		mov	ebx, 0x3E0              //Get new green value
		and	ebx, edx
		shr	ebx, 1
		add	ebx, eax
		or	bp, LUT[1024 + ebx*2]

		mov	ebx, 0x1F   		//Get new blue value
		and	ebx, edx
		shl	ebx, 4
		add	ebx, eax
		or	bp, LUT[ebx*2 + 2048]

		mov	[edi], bp

		add	edi, 2
		inc	esi

		dec	ecx
		jnz	XLoop

		add	edi, 1024 - 40

		pop	edx
		dec	edx
		jnz	YLoop

		popfd
		popad
	}//asm
}//DrawDings




void UnloadDraw(void)
{
        if (Draw != NULL)
	{
		Header *VHeader = (Header*)Draw->ObjectData[0].Offset;
	     	Material *TempMaterial = &VHeader->H_MatrOffs[0];
	     	TempMaterial->M_MapOffset = NULL;
	}//if

	if ( Draw != 0 ) SCN_Unload((void*)Draw);
	if ( Draw_Objects != 0) delete [] Draw_Objects;
	if ( Draw_Lights != 0) delete [] Draw_Lights;
	if ( DrawGlow01 != 0 ) GLW_Unload(DrawGlow01);

}//UnloadDraw
