void GotoXY(unsigned char X, unsigned char Y);
void KeyPressed(void);
void ClrScr(unsigned char TextColor, unsigned char BackColor);
void Box(	int X1, int Y1, int X2, int Y2,
		unsigned char TextColor, unsigned char BackColor);
void *ReadFile(const char *FileName);
int CInt(void *P);
int CSInt(void *P);
int Str2Int(const char *String, int &Number, char EndChar);
int Str2Float(const char *String, float &Number, char EndChar);
int SearchStr(const char *String, const char Letter);
int strcmp_c(const char *S1, const char *S2, int Count);
void Int2Str(int Number, char *String, int Length);

typedef char TextHeader[66];

const int MainID  = 'NIAM';	//MAIN
const int VertID  = 'TREV';	//VERT
const int FaceID  = 'SLOP';	//POLS
const int MatrID  = 'RTAM';	//MATR
const int SpecID  = 'CPSM';	//MSPC

//------- Global parameters...
#ifndef GlobalParameters
#define GlobalParameters

extern	int NoPath;

#endif




typedef struct
{
	int	ID;
	int	Size;
} Chunk;

typedef struct
{
	int	Version;
	int	ObjectType;
	int	FileSize;
	int	CheckSum;
	int	Vertices;
	int	Faces;
	int	Materials;

}FileHeader;

typedef struct
{
	float	X;
	float	Y;
	float	Z;
}Vertice;

typedef struct
{
	int	Vert0, Vert1, Vert2;
	float	U0, V0, U1, V1, U2, V2;
	int	Material;
}Face;

typedef struct
{
	int     ID;
	int	Size;
	float   R, G, B;
	float	Luminosity, Diffuse, Transparency;
	int	TextureFOffset;
	int	TextureFSize;
	char    TextureFile[];

}Material;

const int Flat = 0;
const int Sylindrical = 1;
const int Spherical = 2;
const int XAxis = 1;
const int YAxis = 2;
const int ZAxis = 4;

