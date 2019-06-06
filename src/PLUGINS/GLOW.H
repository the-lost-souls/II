/*
旼컴컴컴컴컴컴컴컴컴컴컴컴 WATCOM C++ INCLUDEFILE 컴컴컴컴컴컴컴컴컴컴컴컴컴
쿙ame         : GLOW.H
쿌uthor       : Cyberfish/TLS
쿗ast update  : 13.03.1998
쿌ction       : External definitions for glow-plugin
�
쿙otes :
읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴*/


typedef struct
{
	void   *GLW_NextOffset;
	int	GLW_SortValue;
	int	GLW_Type;
	int	GLW_Priority;

	float	GLW_XPos;
	float	GLW_YPos;
	float	GLW_ZPos;

	float	GLW_Scale;

	float	GLW_R;
	float	GLW_G;
	float	GLW_B;

	float	GLW_MapXSize;
	float	GLW_MapYSize;

	unsigned char 	*GLW_Map;


} GLW_PluginData;


GLW_PluginData *GLW_Load(const char *TGAFile, int FOffset, int FSize);
void GLW_Unload(GLW_PluginData *Glow);

extern "C" 	void ASM_CALL 	GLW_LightPlugin(void);
extern "C" 	void ASM_CALL 	GLW_Plugin(void);
extern "C" 	void ASM_CALL 	GLW(void);

