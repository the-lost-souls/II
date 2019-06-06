/*
旼컴컴컴컴컴컴컴컴컴컴컴컴 WATCOM C++ INCLUDEFILE 컴컴컴컴컴컴컴컴컴컴컴컴컴
쿙ame         : VAR.H
쿌uthor       : Cyberfish/TLS & Toe/DU
쿗ast update  : 13.02.1998
쿌ction       : Contains external definitions of variables + standard structs.
�
쿙otes :
읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴*/

#ifndef __VARH
#define __VARH

#define ASM_CALL __cdecl

extern		void	Aquarium(void);
extern "C"	float   SYS_Ambient;

extern "C" 	char   *SYS_QuitMsg;
extern "C"	void   *SYS_OOffs;
extern "C"	void   *SYS_LOffs;
extern "C"      int     SYS_ORoutines[];	//
extern "C"      int     SYS_PRoutines[];	//
extern "C"      int     SYS_LRoutines[];	//

extern "C"	int	SYS_QuitFlag;

extern "C"	void   *SYS_OListOffs;         	//ObjectListOffset
extern "C"      void   *SYS_LListOffs; 		//LightListOffset
extern "C"      void   *SYS_POffs;     		//Plugin data-offset
extern "C"	void   *SYS_PCaller;

extern "C"	void   *SYS_ScreenShot;		//Offset doublebuffer

extern "C"	void   *SYS_DBufOffs;  		//Offset doublebuffer
extern "C"	int	SYS_DBufSize;   	//Size doublebuffer
extern "C"	int	SYS_DBufLine;   	//Bytes per line, doublebuffer
extern "C"	void   *SYS_ZBufOffs;  		//Offset Z-buffer
extern "C"	int	SYS_ZBufSize;   	//Size Z-buffer
extern "C"	int	SYS_ZBufLine;   	//Bytes per line, Z-buffer
extern "C"	void   *SYS_VideoMem;
extern "C"	float   SYS_XCenter;
extern "C"	float   SYS_YCenter;
extern "C"	float	SYS_XPerspective;       //Used for 3D->2D projection
extern "C"	float	SYS_YPerspective;       //
extern "C"	float	SYS_FPClipRight;
extern "C"	float	SYS_FPClipLeft;
extern "C"	float	SYS_FPClipTop;
extern "C"	float	SYS_FPClipBottom;
extern "C"	float	SYS_ZClip;

extern "C"	short int     	FPU_Original;		//The original controlword
extern "C"      short int	FPU_24;			//
extern "C"	short int	FPU_Round;		//Controlword for round
extern "C"	short int	FPU_Chop;		//Controlword for chop
extern "C"	short int	FPU_RoundUp;		//Controlword for round up
extern "C"	short int	FPU_RoundDown;		//Controlword for round down

extern "C"	int	SYS_TimerTick;
const int SYS_TicksPerSec = 100;



//-----------------------------------------
//Datastructure for standard material
const int M_Size = 4*9;
typedef struct{
	float	M_R;
	float	M_G;
	float	M_B;
	float	M_Luminosity;
	float	M_Diffuse;
	float 	M_Transparency;
	int	M_MapWidth;
	int	M_MapHeigth;
	void    *M_MapOffset;

} Material;




//-----------------------------------------
//Datastructure for standard vertex

const int V_Size = 96;
typedef struct{
	float	V_X;			//original coordinate
	float	V_Y;			//
	float	V_Z;			//
	float	V_InvZ;			//1/Z
	float	V_X2D;			//projected coordinate
	float	V_Y2D;			//
	float 	V_XRotated;		//rotated coordinate
	float	V_YRotated;
	float	V_ZRotated;
	float	V_U;			//mapping coordinate
	float	V_V;
	float	V_NVX;			//normalvector
	float	V_NVY;
	float	V_NVZ;
	float	V_IntensityR;		//calculated lightintensity
	float	V_IntensityG;
	float	V_IntensityB;
	int	V_ZFlag;		//Z-flag (in front of/behind camera)
	int	V_RFlag;		//Rotation flag
	int	V_PFlag;                //Position flag outside/inside screen
	int	V_Pad[4];

} Vertice;




//-----------------------------------------
//Datastructure for standard face

const int F_Size = 96;
typedef struct{
	int 	F_NextOffset;		//Offset next part
	int	F_SortValue;		//1/Z 0.32 fixed point
	int	F_Type;			//Part-type
	int	F_Priority;
	float   F_NVX;			//Normal vector
	float	F_NVY;			//
	float	F_NVZ;			//
	Vertice *F_V0Offs;		//Offset vertice0
	Vertice	*F_V1Offs;		//
	Vertice *F_V2Offs;		//
	Material*F_Material;             //
	float	F_IntensityR;		//Calculated light-intensity
	float	F_IntensityG;		//
	float	F_IntensityB;		//
	float	F_U0;			//Mapping coordinates
	float	F_V0;			//
	float	F_U1;			//
	float	F_V1;			//
	float	F_U2;			//
	float	F_V2;			//
	int	F_ZFlag;		//

	int	F_Pad[3];
} Face;



//-----------------------------------------
//Datastructure for standard vectorobject header

const int H_Size = 256;
typedef struct{
	int 	H_Type;			//Objecttype
	int 	H_Plugins[16];		//Plugins...
 	float 	H_Matrix[12];		//Transformation matrix
	int	H_Vertices;		//Number of vertices
	Vertice *H_VertOffs;		//Offset vertices
	int	H_Faces;		//Number of faces
	Face	*H_FaceOffs;		//Offset faces
	int	H_Materials;		//Number of materials
	Material *H_MatrOffs;		//Offset materials
	int	H_Memory;		//Number of bytes used by object

} Header;



//-----------------------------------------
//Datastructure for standard light

typedef struct{
	int	L_Type;
	int	L_Plugins[16];
	float   L_Matrix[12];
	float	L_Pos[3];
	float	L_R;
	float	L_G;
	float	L_B;
	float	L_Intensity;
	float	L_MaxRange;
	float	L_Falloff;
	float	L_ConeAngle;
	float	L_EdgeAngle;
} Light;

#ifndef TRUE
#define TRUE    1
#endif
#ifndef FALSE
#define FALSE   0
#endif

typedef signed   char   sint8;
typedef unsigned char   uint8;
typedef signed   short  sint16;
typedef unsigned short  uint16;
typedef signed   long   sint32;
typedef unsigned long   uint32;

#define b (BYTE PTR )
#define w (WORD PTR )
#define d (DWORD PTR )
#define q (QWORD PTR )

const float Pi = 3.14159265359;


typedef struct
{
	int StartFlag;
	int StartTime;
	int NrKeys;
	int CurrentKey;
	int TimingKeys[200];
}TIM_Data;


#endif
