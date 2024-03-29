/*
旼컴컴컴컴컴컴컴컴컴컴컴컴 WATCOM C++ SOURCE 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
쿙ame         : SCENE.CPP
쿌uthor       : Cyberfish/TLS
쿗ast update  : 28.02.1998
쿌ction       : Pluginroutine for setting up all scene-matrixes...
�
쿙otes :
읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴*/

#include "w:\general\var.h"
#include "w:\vectors\scene.h"
#include <math.h>
#include <mem.h>

/*
旼컴컴컴컴컴컴컴컴컴컴컴컴 ASSEMBLER ROUTINE 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
쿙ame         : LightPlugin01
쿗ast update  : 28.02.1998
쿌ction       : Plugin for setting up light-vector
�
쿔nput variables: SYS_POffs, SYS_PCaller
�
�
쿙otes :
읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴*/
void SceneLightPlugin(void)
{
	Light *LightP = (Light*)SYS_PCaller;

	LightP->L_Falloff = LightP->L_Intensity/LightP->L_MaxRange;

	switch ( LightP->L_Type )
	{
		case 1:	LightP->L_Pos[0] = -LightP->L_Matrix[ 2];
			LightP->L_Pos[1] = -LightP->L_Matrix[ 6];
			LightP->L_Pos[2] = -LightP->L_Matrix[10];
			break;

		case 2: LightP->L_Pos[0] = LightP->L_Matrix[3];
			LightP->L_Pos[1] = LightP->L_Matrix[7];
			LightP->L_Pos[2] = LightP->L_Matrix[11];

		case 3: /*LightP->L_Pos[0] = LightP->L_Matrix[3];
			LightP->L_Pos[1] = LightP->L_Matrix[7];
			LightP->L_Pos[2] = LightP->L_Matrix[11];*/
			break;



	}//switch

}//SceneLightPlugin



/*
旼컴컴컴컴컴컴컴컴컴컴컴컴 ASSEMBLER ROUTINE 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
쿙ame         : GetObjectMatrix
쿗ast update  : 06.03.1998
쿌ction       : Routine for getting a matrix for a object at frame t
�
쿔nput variables: Frame, Objectdata, output-matrix
�
쿙otes :
읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴*/
void GetObjectMatrix(float InFrame, SceneObjectData *Object, float *Matrix)
{
	int C1, C2;
	int K1, K2;
	float t, t2, t3;
	float P1, P2, R1, R2;
	float XPos, YPos, ZPos, XAng, YAng, ZAng;
	float Frame = InFrame;

	float PivotMatrix[12];
	float RotationMatrix[12];
	float UniverseMatrix[12];
	float ParentMatrix[12];
	float FinalMatrix[12];
	float TempMatrixA[12];
	float TempMatrixB[12];

//---- Checking EndBehavior
	int MaxFrame = Object->KeyFrames[Object->NrKeyFrames - 1].Frame;
	if ( Frame > MaxFrame )
	{
		switch ( Object->EndBehavior )
		{
			case 0 : break;
			case 1 : Frame = MaxFrame;
			   	 break;
			case 2 : Frame = Frame - MaxFrame*((int)(Frame/MaxFrame));
				 break;
		}//switch
	}//if

//---- Translate around pivotpoint...
	ZeroMatrix(PivotMatrix);
	PivotMatrix[ 3] = -Object->XPivot;
	PivotMatrix[ 7] = -Object->YPivot;
	PivotMatrix[11] = -Object->ZPivot;


//---- Find object's position and angles...
	if ( Object->NrKeyFrames > 1 )
	{
	//---- Find key-segment...
		K2 = 0;
		for ( C2 = 0; C2 < (Object->NrKeyFrames-1); C2++ )
			if ( Object->KeyFrames[C2].Frame <= Frame ) K2++;
		K1 = K2 - 1;


	//---- Calculating t = (T - T0)/(T1 - T0)
		t = 	(float)
			(Frame - (float)Object->KeyFrames[K1].Frame)
			/(Object->KeyFrames[K2].Frame - Object->KeyFrames[K1].Frame);

		t2 = t*t;
		t3 = t2*t;

	//---- Calculating X angle...
		P1 = Object->KeyFrames[K1].XAng;
		P2 = Object->KeyFrames[K2].XAng;

		if ( Object->KeyFrames[K2].Linear == 1 ) XAng = (P2-P1)*t + P1;
		else
		{
			R1 = Object->KeyFrames[K1].XAngTan_b;
			R2 = Object->KeyFrames[K2].XAngTan_a;
			XAng =    t3*(2*P1 + R1 - 2*P2 + R2)
				+ t2*(3*P2 - 3*P1 - 2*R1 - R2)
				+ t*R1 + P1;
		}

	//---- Calculating Y angle...
		P1 = Object->KeyFrames[K1].YAng;
		P2 = Object->KeyFrames[K2].YAng;
		if ( Object->KeyFrames[K2].Linear == 1 ) YAng = (P2-P1)*t + P1;
		else
		{
			R1 = Object->KeyFrames[K1].YAngTan_b;
			R2 = Object->KeyFrames[K2].YAngTan_a;
			YAng =    t3*(2*P1 + R1 - 2*P2 + R2)
				+ t2*(3*P2 - 3*P1 - 2*R1 - R2)
				+ t*R1 + P1;
		}

	//---- Calculating Z angle...
		P1 = Object->KeyFrames[K1].ZAng;
		P2 = Object->KeyFrames[K2].ZAng;
		if ( Object->KeyFrames[K2].Linear == 1 ) ZAng = (P2-P1)*t + P1;
		else
		{
			R1 = Object->KeyFrames[K1].ZAngTan_b;
			R2 = Object->KeyFrames[K2].ZAngTan_a;
			ZAng =    t3*(2*P1 + R1 - 2*P2 + R2)
				+ t2*(3*P2 - 3*P1 - 2*R1 - R2)
				+ t*R1 + P1;
		}

	//---- Calculating X position...
		P1 = Object->KeyFrames[K1].XPos;
		P2 = Object->KeyFrames[K2].XPos;
		if ( Object->KeyFrames[K2].Linear == 1 ) XPos = (P2-P1)*t + P1;
		else
		{
			R1 = Object->KeyFrames[K1].XPosTan_b;
			R2 = Object->KeyFrames[K2].XPosTan_a;
			XPos =    t3*(2*P1 + R1 - 2*P2 + R2)
				+ t2*(3*P2 - 3*P1 - 2*R1 - R2)
				+ t*R1 + P1;
		}

	//---- Calculating Y position...
		P1 = Object->KeyFrames[K1].YPos;
		P2 = Object->KeyFrames[K2].YPos;
		if ( Object->KeyFrames[K2].Linear == 1 ) YPos = (P2-P1)*t + P1;
		else
		{
			R1 = Object->KeyFrames[K1].YPosTan_b;
			R2 = Object->KeyFrames[K2].YPosTan_a;
			YPos =    t3*(2*P1 + R1 - 2*P2 + R2)
				+ t2*(3*P2 - 3*P1 - 2*R1 - R2)
				+ t*R1 + P1;
		}

	//---- Calculating Z position...
		P1 = Object->KeyFrames[K1].ZPos;
		P2 = Object->KeyFrames[K2].ZPos;
		if ( Object->KeyFrames[K2].Linear == 1 ) ZPos = (P2-P1)*t + P1;
		else
		{
			R1 = Object->KeyFrames[K1].ZPosTan_b;
			R2 = Object->KeyFrames[K2].ZPosTan_a;
			ZPos =    t3*(2*P1 + R1 - 2*P2 + R2)
				+ t2*(3*P2 - 3*P1 - 2*R1 - R2)
				+ t*R1 + P1;
		}
	}//if
	else
	{
		XPos = Object->KeyFrames[0].XPos;
		YPos = Object->KeyFrames[0].YPos;
		ZPos = Object->KeyFrames[0].ZPos;
		XAng = Object->KeyFrames[0].XAng;
		YAng = Object->KeyFrames[0].YAng;
		ZAng = Object->KeyFrames[0].ZAng;
	}//else if only one keyframe...

//---- Pathalignment...
	if ( Object->PathAlignment != 0 )
	{
		float LookAheadFrames = Object->PathAlignment;
		Object->PathAlignment = 0;

		ZeroMatrix(TempMatrixA);
		GetObjectMatrix(InFrame, Object, TempMatrixA);
		ZeroMatrix(TempMatrixB);
		GetObjectMatrix(InFrame + LookAheadFrames, Object, TempMatrixB);

		float XPos0, YPos0, ZPos0;
		float XPos1, YPos1, ZPos1;
		float XPos2, YPos2, ZPos2;

		XPos1 = TempMatrixA[ 3];
		YPos1 = TempMatrixA[ 7];
		ZPos1 = TempMatrixA[11];

		XPos2 = TempMatrixB[ 3];
		YPos2 = TempMatrixB[ 7];
		ZPos2 = TempMatrixB[11];

		XPos0 = XPos2 - XPos1;
		YPos0 = YPos2 - YPos1;
		ZPos0 = ZPos2 - ZPos1;

		YAng = GetAngle(XPos0, ZPos0);
		RotatePoint(XPos0, YPos0, ZPos0, 0, -YAng, 0, XPos0, YPos0, ZPos0);
		XAng = -GetAngle(YPos0, ZPos0);

		Object->PathAlignment = LookAheadFrames;

		ZeroMatrix(ParentMatrix);
		if ( Object->ParentObject != 0 )
		{
			XPos = XPos1;
			YPos = YPos1;
			ZPos = ZPos1;
		}

	}//if PathAlignment
	else
	{
	//---- Matrix from Parentobject
		ZeroMatrix(ParentMatrix);
		if ( Object->ParentObject != 0 )
			GetObjectMatrix(InFrame, (SceneObjectData*)Object->ParentObject, ParentMatrix);
	}


	ZeroMatrix(RotationMatrix);
	ZeroMatrix(TempMatrixA);
	SetRotationMatrix(TempMatrixA, 0, 0, ZAng);
	MulMatrix(TempMatrixA, RotationMatrix, RotationMatrix);
	ZeroMatrix(TempMatrixA);
	SetRotationMatrix(TempMatrixA, XAng, 0, 0);
	MulMatrix(TempMatrixA, RotationMatrix, RotationMatrix);
	ZeroMatrix(TempMatrixA);
	SetRotationMatrix(TempMatrixA, 0, YAng, 0);
	MulMatrix(TempMatrixA, RotationMatrix, RotationMatrix);


//---- Translation back to universe...
	ZeroMatrix(UniverseMatrix);
	UniverseMatrix[ 3] = XPos;
	UniverseMatrix[ 7] = YPos;
	UniverseMatrix[11] = ZPos;

	ZeroMatrix(FinalMatrix);
	MulMatrix(PivotMatrix, FinalMatrix, FinalMatrix);
	MulMatrix(RotationMatrix, FinalMatrix, FinalMatrix);
	MulMatrix(UniverseMatrix, FinalMatrix, FinalMatrix);
	MulMatrix(ParentMatrix, FinalMatrix, FinalMatrix);

	memcpy(Matrix, FinalMatrix, 12*4);
}//GetObjectMatrix


/*
旼컴컴컴컴컴컴컴컴컴컴컴컴 ASSEMBLER ROUTINE 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
쿙ame         : GetCameraMatrix
쿗ast update  : 06.03.1998
쿌ction       : Routine for getting a matrix for camera at frame t
�
쿔nput variables: Frame, Cameradata, output-matrix
�
쿙otes :
읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴*/
void GetCameraMatrix(float InFrame, SceneCameraData *Camera, float *Matrix)
{
	float XPos, YPos, ZPos, XAng, YAng, ZAng;

	float PivotMatrix[12];
	float RotationMatrix[12];
	float UniverseMatrix[12];
	float ParentMatrix[12];
	float FinalMatrix[12];
	float TempMatrixA[12];
	float TempMatrixB[12];



//-------------------------- GetObjectMatrix ---------------------------
	int C1, C2;
	int K1, K2;
	float t, t2, t3;
	float P1, P2, R1, R2;
	float Frame = InFrame;

//---- Checking EndBehavior
	int MaxFrame = Camera->KeyFrames[Camera->NrKeyFrames - 1].Frame;
	if ( Frame > MaxFrame )
	{
		switch ( Camera->EndBehavior )
		{
			case 0 : break;
			case 1 : Frame = MaxFrame;
			   	 break;
			case 2 : Frame = Frame - MaxFrame*((int)(Frame/MaxFrame));
				 break;
		}//switch
	}//if


//---- Translate around pivot...
	ZeroMatrix(PivotMatrix);
	PivotMatrix[ 3] = -Camera->XPivot;
	PivotMatrix[ 7] = -Camera->YPivot;
	PivotMatrix[11] = -Camera->ZPivot;


//---- Find object's position and angles...
	if ( Camera->NrKeyFrames > 1 )
	{
	//---- Find key-segment...
		K2 = 0;
		for ( C2 = 0; C2 < (Camera->NrKeyFrames-1); C2++ )
			if ( Camera->KeyFrames[C2].Frame <= Frame ) K2++;
		K1 = K2 - 1;


	//---- Calculating t = (T - T0)/(T1 - T0)
		t = 	(float)
			(Frame - (float)Camera->KeyFrames[K1].Frame)
			/(Camera->KeyFrames[K2].Frame - Camera->KeyFrames[K1].Frame);

		t2 = t*t;
		t3 = t2*t;

	//---- Calculating X angle...
		P1 = Camera->KeyFrames[K1].XAng;
		P2 = Camera->KeyFrames[K2].XAng;
		if ( Camera->KeyFrames[K2].Linear == 1 ) XAng = (P2-P1)*t + P1;
		else
		{
			R1 = Camera->KeyFrames[K1].XAngTan_b;
			R2 = Camera->KeyFrames[K2].XAngTan_a;
			XAng =    t3*(2*P1 + R1 - 2*P2 + R2)
				+ t2*(3*P2 - 3*P1 - 2*R1 - R2)
				+ t*R1 + P1;
		}


	//---- Calculating Y angle...
		P1 = Camera->KeyFrames[K1].YAng;
		P2 = Camera->KeyFrames[K2].YAng;
		if ( Camera->KeyFrames[K2].Linear == 1 ) YAng = (P2-P1)*t + P1;
		else
		{
			R1 = Camera->KeyFrames[K1].YAngTan_b;
			R2 = Camera->KeyFrames[K2].YAngTan_a;
			YAng =    t3*(2*P1 + R1 - 2*P2 + R2)
				+ t2*(3*P2 - 3*P1 - 2*R1 - R2)
				+ t*R1 + P1;
		}

	//---- Calculating Z angle...
		P1 = Camera->KeyFrames[K1].ZAng;
		P2 = Camera->KeyFrames[K2].ZAng;
		if ( Camera->KeyFrames[K2].Linear == 1 ) ZAng = (P2-P1)*t + P1;
		else
		{
			R1 = Camera->KeyFrames[K1].ZAngTan_b;
			R2 = Camera->KeyFrames[K2].ZAngTan_a;
			ZAng =    t3*(2*P1 + R1 - 2*P2 + R2)
				+ t2*(3*P2 - 3*P1 - 2*R1 - R2)
				+ t*R1 + P1;
		}

	//---- Calculating X position...
		P1 = Camera->KeyFrames[K1].XPos;
		P2 = Camera->KeyFrames[K2].XPos;
		if ( Camera->KeyFrames[K2].Linear == 1 ) XPos = (P2-P1)*t + P1;
		else
		{
			R1 = Camera->KeyFrames[K1].XPosTan_b;
			R2 = Camera->KeyFrames[K2].XPosTan_a;
			XPos =    t3*(2*P1 + R1 - 2*P2 + R2)
				+ t2*(3*P2 - 3*P1 - 2*R1 - R2)
				+ t*R1 + P1;
		}

	//---- Calculating Y position...
		P1 = Camera->KeyFrames[K1].YPos;
		P2 = Camera->KeyFrames[K2].YPos;
		if ( Camera->KeyFrames[K2].Linear == 1 ) YPos = (P2-P1)*t + P1;
		else
		{
			R1 = Camera->KeyFrames[K1].YPosTan_b;
			R2 = Camera->KeyFrames[K2].YPosTan_a;
			YPos =    t3*(2*P1 + R1 - 2*P2 + R2)
				+ t2*(3*P2 - 3*P1 - 2*R1 - R2)
				+ t*R1 + P1;
		}

	//---- Calculating Z position...
		P1 = Camera->KeyFrames[K1].ZPos;
		P2 = Camera->KeyFrames[K2].ZPos;
		if ( Camera->KeyFrames[K2].Linear == 1 ) ZPos = (P2-P1)*t + P1;
		else
		{
			R1 = Camera->KeyFrames[K1].ZPosTan_b;
			R2 = Camera->KeyFrames[K2].ZPosTan_a;
			ZPos =    t3*(2*P1 + R1 - 2*P2 + R2)
				+ t2*(3*P2 - 3*P1 - 2*R1 - R2)
				+ t*R1 + P1;
		}
	}//if
	else
	{
		XPos = Camera->KeyFrames[0].XPos;
		YPos = Camera->KeyFrames[0].YPos;
		ZPos = Camera->KeyFrames[0].ZPos;
		XAng = Camera->KeyFrames[0].XAng;
		YAng = Camera->KeyFrames[0].YAng;
		ZAng = Camera->KeyFrames[0].ZAng;
	}//else if only one keyframe...

//---- Pathalignment...
	if ( Camera->PathAlignment != 0 )
	{
		float LookAheadFrames = Camera->PathAlignment;
		Camera->PathAlignment = 0;

		ZeroMatrix(TempMatrixA);
		GetObjectMatrix(InFrame, (SceneObjectData*)Camera, TempMatrixA);
		ZeroMatrix(TempMatrixB);
		GetObjectMatrix(InFrame + LookAheadFrames, (SceneObjectData*)Camera, TempMatrixB);

		float XPos0, YPos0, ZPos0;
		float XPos1, YPos1, ZPos1;
		float XPos2, YPos2, ZPos2;

		XPos1 = TempMatrixA[ 3];
		YPos1 = TempMatrixA[ 7];
		ZPos1 = TempMatrixA[11];

		XPos2 = TempMatrixB[ 3];
		YPos2 = TempMatrixB[ 7];
		ZPos2 = TempMatrixB[11];

		XPos0 = XPos2 - XPos1;
		YPos0 = YPos2 - YPos1;
		ZPos0 = ZPos2 - ZPos1;

		YAng = GetAngle(XPos0, ZPos0);
		RotatePoint(XPos0, YPos0, ZPos0, 0, -YAng, 0, XPos0, YPos0, ZPos0);
		XAng = -GetAngle(YPos0, ZPos0);


		Camera->PathAlignment = LookAheadFrames;

		ZeroMatrix(ParentMatrix);
		if ( Camera->ParentObject != 0 )
		{
			XPos = XPos1;
			YPos = YPos1;
			ZPos = ZPos1;
		}
	}//if PathAlignment

	if ( Camera->TargetObject != 0 )
	{
		ZeroMatrix(TempMatrixA);
		GetObjectMatrix(InFrame, (SceneObjectData*)Camera, TempMatrixA);
		ZeroMatrix(TempMatrixB);
		GetObjectMatrix(InFrame, (SceneObjectData*)Camera->TargetObject, TempMatrixB);

		float XPos0, YPos0, ZPos0;
		float XPos1, YPos1, ZPos1;
		float XPos2, YPos2, ZPos2;

		XPos1 = TempMatrixA[ 3];
		YPos1 = TempMatrixA[ 7];
		ZPos1 = TempMatrixA[11];

		XPos2 = TempMatrixB[ 3];
		YPos2 = TempMatrixB[ 7];
		ZPos2 = TempMatrixB[11];

		XPos0 = XPos2 - XPos1;
		YPos0 = YPos2 - YPos1;
		ZPos0 = ZPos2 - ZPos1;

		YAng = GetAngle(XPos0, ZPos0);
		RotatePoint(XPos0, YPos0, ZPos0, 0, -YAng, 0, XPos0, YPos0, ZPos0);
		XAng = -GetAngle(YPos0, ZPos0);

		ZeroMatrix(ParentMatrix);
		if ( Camera->ParentObject != 0 )
		{
			XPos = XPos1;
			YPos = YPos1;
			ZPos = ZPos1;
		}

	}

	if ( (Camera->PathAlignment == 0) && (Camera->TargetObject == 0) )
	{
	//---- Matrix from Parentobject
		ZeroMatrix(ParentMatrix);
		if ( Camera->ParentObject != 0 )
			GetObjectMatrix(InFrame, (SceneObjectData*)Camera->ParentObject, ParentMatrix);

	}


	ZeroMatrix(RotationMatrix);
	ZeroMatrix(TempMatrixA);
	SetRotationMatrix(TempMatrixA, 0, 0, ZAng);
	MulMatrix(TempMatrixA, RotationMatrix, RotationMatrix);
	ZeroMatrix(TempMatrixA);
	SetRotationMatrix(TempMatrixA, XAng, 0, 0);
	MulMatrix(TempMatrixA, RotationMatrix, RotationMatrix);
	ZeroMatrix(TempMatrixA);
	SetRotationMatrix(TempMatrixA, 0, YAng, 0);
	MulMatrix(TempMatrixA, RotationMatrix, RotationMatrix);


//---- Translation back to universe...
	ZeroMatrix(UniverseMatrix);
	UniverseMatrix[ 3] = XPos;
	UniverseMatrix[ 7] = YPos;
	UniverseMatrix[11] = ZPos;


	ZeroMatrix(FinalMatrix);
	MulMatrix(PivotMatrix, FinalMatrix, FinalMatrix);
	MulMatrix(RotationMatrix, FinalMatrix, FinalMatrix);
	MulMatrix(UniverseMatrix, FinalMatrix, FinalMatrix);
	MulMatrix(ParentMatrix, FinalMatrix, FinalMatrix);

//-------------------------- GetObjectMatrix [END] ---------------------


	ZeroMatrix(PivotMatrix);
	PivotMatrix[ 3] = -FinalMatrix[ 3];
	PivotMatrix[ 7] = -FinalMatrix[ 7];
	PivotMatrix[11] = -FinalMatrix[11];

//---- Rotating a cameravector of [0, 0, 1] to find Y/X-angle
	XPos = FinalMatrix[ 2];
	YPos = FinalMatrix[ 6];
	ZPos = FinalMatrix[10];

	YAng = -GetAngle(XPos, ZPos);

	RotatePoint(XPos, YPos, ZPos, 0, YAng, 0, XPos, YPos, ZPos);
	XAng =  GetAngle(YPos, ZPos);
	RotatePoint(XPos, YPos, ZPos, XAng, 0, 0, XPos, YPos, ZPos);

//---- Rotating a cameravector of [1, 0, 0] to find Z-angle
	XPos = FinalMatrix[ 0];
	YPos = FinalMatrix[ 4];
	ZPos = FinalMatrix[ 8];
	RotatePoint(XPos, YPos, ZPos, 0, YAng, 0, XPos, YPos, ZPos);
	RotatePoint(XPos, YPos, ZPos, XAng, 0, 0, XPos, YPos, ZPos);
	ZAng = -GetAngle(YPos, XPos);

	ZeroMatrix(RotationMatrix);
	SetRotationMatrix(RotationMatrix, 0, YAng, 0);
	MulMatrix(RotationMatrix, PivotMatrix, FinalMatrix);
	ZeroMatrix(RotationMatrix);
	SetRotationMatrix(RotationMatrix, XAng, 0, 0);
	MulMatrix(RotationMatrix, FinalMatrix, FinalMatrix);
	ZeroMatrix(RotationMatrix);
	SetRotationMatrix(RotationMatrix, 0, 0, ZAng);
	MulMatrix(RotationMatrix, FinalMatrix, FinalMatrix);

	memcpy(Matrix, FinalMatrix, 12*4);
}//void GetCameraMatrix


/*
旼컴컴컴컴컴컴컴컴컴컴컴컴 ASSEMBLER ROUTINE 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
쿙ame         : ScenePlugin
쿗ast update  : 06.03.1998
쿌ction       : Plugin for setting up all object/light/camera-matrixes/envelopes
�
쿔nput variables: SYS_POffs, SYS_PCaller
�
쿙otes :
읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴*/

void ScenePlugin(void)
{
	SceneData *Scene;
	Scene = (SceneData*)SYS_POffs;

	int CurrentTime = SYS_TimerTick;


	float Seconds = (float)(CurrentTime - Scene->StartTime)/(float)SYS_TicksPerSec;
	float InFrame = Scene->FirstFrame + Seconds*Scene->FramesPerSecond;

	Scene->CurrentFrame = InFrame;

	int C1, C2;

	float TempMatrixA[12];
	float TempMatrixB[12];
	float TempMatrixC[12];


//---- Setting up internal/external matrices for all objects...
	for ( C1 = 0; C1 < Scene->NrObjects; C1++ )
		GetObjectMatrix(InFrame, (SceneObjectData*)&Scene->ObjectData[C1], (float*)&Scene->ObjectData[C1].InternalMatrix);

	GetCameraMatrix(InFrame, Scene->Camera, Scene->Camera->ExternalMatrix);


//---- Setting up internal matrices for all lights...
	for ( C1 = 0; C1 < Scene->NrLights; C1++ )
	{
		GetObjectMatrix(InFrame, (SceneObjectData*)&Scene->LightData[C1], TempMatrixB);

		MulMatrix(Scene->Camera->ExternalMatrix, TempMatrixB, Scene->LightData[C1].InternalMatrix);

		memcpy( ((char*)Scene->LightData[C1].Offset + 68), Scene->LightData[C1].InternalMatrix, 12*4);
	}//for


//---- Translating objects around camera...
	for ( C1 = 0; C1 < Scene->NrObjects; C1++ )
	{
		MulMatrix(Scene->Camera->ExternalMatrix, Scene->ObjectData[C1].InternalMatrix, TempMatrixA);
		memcpy(Scene->ObjectData[C1].InternalMatrix, TempMatrixA, 12*4);
		memcpy( ((char*)Scene->ObjectData[C1].Offset + 68), TempMatrixA, 12*4);
	}//for

	__asm int 3

//---- Running envelopes...
	for ( C1 = 0; C1 < Scene->NrEnvelopes; C1++ )
		SceneEnvelope(InFrame, (Envelope*)&Scene->Envelopes[C1]);
}//ScenePlugin



/*
旼컴컴컴컴컴컴컴컴컴컴컴컴 ASSEMBLER ROUTINE 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
쿙ame         : SetRotationMatrix
쿗ast update  : 29.01.1998
쿌ction       : Sets up a 4x3 (actually 4x4) matrix with rotationformulas
�
쿔nput variables: Angles, output-matrix
�
쿙otes :
읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴*/

void SetRotationMatrix(float *Matrix, float XAng, float YAng, float ZAng)
{
	float XSin, XCos, YSin, YCos, ZSin, ZCos;
	XSin = sin(XAng);
	XCos = cos(XAng);
	YSin = sin(YAng);
	YCos = cos(YAng);
	ZSin = sin(ZAng);
	ZCos = cos(ZAng);

	Matrix[ 0] = XSin*ZSin*YSin + ZCos*YCos;
	Matrix[ 1] = ZCos*XSin*YSin - ZSin*YCos;
	Matrix[ 2] = XCos*YSin;
	Matrix[ 4] = ZSin*XCos;
	Matrix[ 5] = ZCos*XCos;
	Matrix[ 6] = -XSin;
	Matrix[ 8] = XSin*ZSin*YCos - ZCos*YSin;
	Matrix[ 9] = ZCos*XSin*YCos + ZSin*YSin;
	Matrix[10] = XCos*YCos;
}//SetRotationMatrix


/*
旼컴컴컴컴컴컴컴컴컴컴컴컴 ASSEMBLER ROUTINE 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
쿙ame         : ZeroMatrix
쿗ast update  : 29.01.1998
쿌ction       : Zeroes out a 4x3-matrix...
�
쿔nput variables: matrix
�
쿙otes :
읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴*/

void ZeroMatrix(float *Matrix)
{
	float const One = 1.0;
	__asm
	{
		pushad
		pushfd

		mov	edi, Matrix
		mov	ecx, 12
		xor	eax, eax
		rep	stosd

		mov	eax, One
		mov	edi, Matrix
		mov	d[edi], eax
		mov	d[edi + 20], eax
		mov	d[edi + 40], eax

		popfd
		popad
	}
}//ZeroMatrix

/*
旼컴컴컴컴컴컴컴컴컴컴컴컴 ASSEMBLER ROUTINE 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
쿙ame         : MulMatrix
쿗ast update  : 29.01.1998
쿌ction       : Multiplies two 4x3-matrixes
�
쿔nput variables: MatrixA, MatrixB, output-matrix
�
쿙otes :
읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴*/

void MulMatrix(const float *MatrixA, const float *MatrixB, float *MatrixC)
{
	float TMatrix[12];

	TMatrix[0] =   MatrixA[0] * MatrixB[ 0]
		     + MatrixA[1] * MatrixB[ 4]
		     + MatrixA[2] * MatrixB[ 8];

	TMatrix[1] =   MatrixA[0] * MatrixB[ 1]
		     + MatrixA[1] * MatrixB[ 5]
		     + MatrixA[2] * MatrixB[ 9];

	TMatrix[2] =   MatrixA[0] * MatrixB[ 2]
		     + MatrixA[1] * MatrixB[ 6]
		     + MatrixA[2] * MatrixB[10];

	TMatrix[3] =   MatrixA[0] * MatrixB[ 3]
		     + MatrixA[1] * MatrixB[ 7]
		     + MatrixA[2] * MatrixB[11]
		     + MatrixA[3];


	TMatrix[4] =   MatrixA[4] * MatrixB[ 0]
		     + MatrixA[5] * MatrixB[ 4]
		     + MatrixA[6] * MatrixB[ 8];

	TMatrix[5] =   MatrixA[4] * MatrixB[ 1]
		     + MatrixA[5] * MatrixB[ 5]
		     + MatrixA[6] * MatrixB[ 9];

	TMatrix[6] =   MatrixA[4] * MatrixB[ 2]
		     + MatrixA[5] * MatrixB[ 6]
		     + MatrixA[6] * MatrixB[10];

	TMatrix[7] =   MatrixA[4] * MatrixB[ 3]
		     + MatrixA[5] * MatrixB[ 7]
		     + MatrixA[6] * MatrixB[11]
		     + MatrixA[7];


	TMatrix[ 8]=   MatrixA[ 8] * MatrixB[ 0]
		     + MatrixA[ 9] * MatrixB[ 4]
		     + MatrixA[10] * MatrixB[ 8];

	TMatrix[ 9]=   MatrixA[ 8] * MatrixB[ 1]
		     + MatrixA[ 9] * MatrixB[ 5]
		     + MatrixA[10] * MatrixB[ 9];

	TMatrix[10]=   MatrixA[ 8] * MatrixB[ 2]
		     + MatrixA[ 9] * MatrixB[ 6]
		     + MatrixA[10] * MatrixB[10];

	TMatrix[11]=   MatrixA[ 8] * MatrixB[ 3]
		     + MatrixA[ 9] * MatrixB[ 7]
		     + MatrixA[10] * MatrixB[11]
		     + MatrixA[11];

	memcpy(MatrixC, TMatrix, 12*4);

}//MulMatrix




/*
旼컴컴컴컴컴컴컴컴컴컴컴컴 ASSEMBLER ROUTINE 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
쿙ame         : RotatePoint
쿗ast update  : 29.01.1998
쿌ction       : Rotates a position
�
쿔nput variables: In-point, angles, out-point
�
쿙otes :
읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴*/
void RotatePoint(float X0,
		 float Y0,
		 float Z0,
		 float XAng,
		 float YAng,
		 float ZAng,
		 float &X1,
		 float &Y1,
		 float &Z1)
{
	float XSin, XCos, YSin, YCos, ZSin, ZCos;
	XSin = sin(XAng);
	XCos = cos(XAng);
	YSin = sin(YAng);
	YCos = cos(YAng);
	ZSin = sin(ZAng);
	ZCos = cos(ZAng);

	float Matrix[9];

	Matrix[ 0] = XSin*ZSin*YSin + ZCos*YCos;
	Matrix[ 1] = ZCos*XSin*YSin - ZSin*YCos;
	Matrix[ 2] = XCos*YSin;
	Matrix[ 3] = ZSin*XCos;
	Matrix[ 4] = ZCos*XCos;
	Matrix[ 5] = -XSin;
	Matrix[ 6] = XSin*ZSin*YCos - ZCos*YSin;
	Matrix[ 7] = ZCos*XSin*YCos + ZSin*YSin;
	Matrix[ 8] = XCos*YCos;

	float XTemp = Matrix[0]*X0 + Matrix[1]*Y0 + Matrix[2]*Z0;
	float YTemp = Matrix[3]*X0 + Matrix[4]*Y0 + Matrix[5]*Z0;
	float ZTemp = Matrix[6]*X0 + Matrix[7]*Y0 + Matrix[8]*Z0;

	X1 = XTemp;
	Y1 = YTemp;
	Z1 = ZTemp;

}//RotatePoint


/*
旼컴컴컴컴컴컴컴컴컴컴컴컴 ASSEMBLER ROUTINE 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
쿙ame         : GetAngle
쿗ast update  : 29.01.1998
쿌ction       : Gets an angle out of two numbers (2d-coordinate)
�
쿔nput variables: A, B.
�
쿙otes : Returns angle in radians.
읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴*/
float GetAngle(float A, float B)
{
	float Angle;
	if ( B != 0 )
		Angle = atan(fabs(A/B));
	else Angle = 0.5*Pi;

	if ( (A >= 0) && (B < 0) ) Angle = Pi - Angle;
	if ( (A < 0) && (B > 0) ) Angle = 2*Pi - Angle;
	if ( (A < 0) && (B <= 0) ) Angle += Pi;
	if ( (A == 0) && (B == 0) ) Angle = 0;

	return Angle;
}//GetAngle




/*
旼컴컴컴컴컴컴컴컴컴컴컴컴 ASSEMBLER ROUTINE 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
쿙ame         : SceneEnvelope
쿗ast update  : 27.02.1998
쿌ction       : Routine for running an envelope
�
쿔nput variables: Frame, Envelope
�
쿙otes :
읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴*/
void SceneEnvelope(float InFrame, Envelope *Env)
{
	int C1, C2;
	int K1, K2;
	float t, t2, t3;
	float P1, P2, R1, R2;
	float Value;
	float Frame = InFrame;

//---- Checking EndBehavior
	int MaxFrame = Env->KeyFrames[Env->NrKeyFrames - 1].Frame;
	if ( Frame > MaxFrame )
	{
		switch ( Env->EndBehavior )
		{
			case 0 : break;
			case 1 : Frame = MaxFrame;
			   	 break;
			case 2 : Frame = Frame - MaxFrame*((int)(Frame/MaxFrame));
				 break;
		}//switch
	}//if


//---- Find object's position and angles...
	if ( Env->NrKeyFrames > 1 )
	{
	//---- Find key-segment...
		K2 = 0;
		for ( C2 = 0; C2 < (Env->NrKeyFrames-1); C2++ )
			if ( Env->KeyFrames[C2].Frame <= Frame ) K2++;
		K1 = K2 - 1;


	//---- Calculating t = (T - T0)/(T1 - T0)
		t = 	(float)
			(Frame - (float)Env->KeyFrames[K1].Frame)
			/(Env->KeyFrames[K2].Frame - Env->KeyFrames[K1].Frame);

		t2 = t*t;
		t3 = t2*t;

	//---- Calculating value...
		P1 = Env->KeyFrames[K1].Value;
		P2 = Env->KeyFrames[K2].Value;
		if ( Env->KeyFrames[K2].Linear == 1 ) *Env->Value = (P2 - P1)*t + P1;
		else
		{
			R1 = Env->KeyFrames[K1].Tan_b;
			R2 = Env->KeyFrames[K2].Tan_a;
			*Env->Value =    t3*(2*P1 + R1 - 2*P2 + R2)
				      + t2*(3*P2 - 3*P1 - 2*R1 - R2)
				      + t*R1 + P1;
		}



	}//if
	else	*Env->Value = Env->KeyFrames[0].Value;

}//SceneEnvelope
