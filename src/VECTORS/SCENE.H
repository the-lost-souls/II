/*
旼컴컴컴컴컴컴컴컴컴컴컴컴 WATCOM C++ INCLUDEFILE 컴컴컴컴컴컴컴컴컴컴컴컴컴
쿙ame         : SCENE.H
쿌uthor       : Cyberfish/TLS
쿗ast update  : 19.02.1998
쿌ction       : Definitions for different scenedata
�
쿙otes :
읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴*/

typedef struct
{
	int	Frame;
	float	Tension, Continuity, Bias;
	int     Linear;
	float	XPos, YPos, ZPos;
	float	XAng, YAng, ZAng;
	float   XPosTan_a, YPosTan_a, ZPosTan_a;
	float   XPosTan_b, YPosTan_b, ZPosTan_b;
	float	XAngTan_a, YAngTan_a, ZAngTan_a;
	float	XAngTan_b, YAngTan_b, ZAngTan_b;
}KeyFrame;

typedef struct
{
	int	Frame;
	float	Tension, Continuity, Bias;
	int     Linear;
	float	Value;
	float   Tan_a, Tan_b;
}KeyFrame2;

typedef struct
{
	int	EndBehavior;
	int	NrKeyFrames;
	KeyFrame2 *KeyFrames;
	float	*Value;
}Envelope;


typedef struct
{
	float	InternalMatrix[12];
	float	ExternalMatrix[12];

	float	XPivot, YPivot, ZPivot;

	void    *ParentObject;

	int	EndBehavior;
	int	PathAlignment;

	int	NrKeyFrames;
	KeyFrame *KeyFrames;

	void	*Offset;
}SceneObjectData;


typedef struct
{
	float	InternalMatrix[12];
	float	ExternalMatrix[12];

	float	XPivot, YPivot, ZPivot;

	void  	*ParentObject;

	int	EndBehavior;
	int	PathAlignment;

	int	NrKeyFrames;
	KeyFrame *KeyFrames;

	void	*Offset;
}SceneLightData;

typedef struct
{
	float	InternalMatrix[12];
	float	ExternalMatrix[12];

	float	XPivot, YPivot, ZPivot;

	void  	*ParentObject;

	int	EndBehavior;
	int	PathAlignment;

	int	NrKeyFrames;
	KeyFrame *KeyFrames;

	void	*TargetObject;
}SceneCameraData;


typedef struct
{
	int	FirstFrame;
	int	LastFrame;
	float	FramesPerSecond;

	int	NrObjects;
	SceneObjectData *ObjectData;
	int     *ObjectList;

	int	NrLights;
	SceneLightData *LightData;
	int	*LightList;

	int	NrEnvelopes;
	Envelope *Envelopes;

	int	StartTime;

	SceneCameraData *Camera;
	float	CurrentFrame;
}SceneData;

void SceneLightPlugin(void);
void GetObjectMatrix(float Frame, SceneObjectData *Object, float *Matrix);
void GetCameraMatrix(float InFrame, SceneCameraData *Camera, float *Matrix);
void ScenePlugin(void);
void ZeroMatrix(float *Matrix);
void MulMatrix(const float *MatrixA, const float *MatrixB, float *MatrixC);
void SetRotationMatrix(float *Matrix, float XAng, float YAng, float ZAng);
float GetAngle(float A, float B);
void RotatePoint(float X0,
		 float Y0,
		 float Z0,
		 float XAng,
		 float YAng,
		 float ZAng,
		 float &X1,
		 float &Y1,
		 float &Z1);
void SceneEnvelope(float InFrame, Envelope *Env);




