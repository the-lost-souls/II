#include "w:\general\var.h"

typedef struct{
	int StartTime;
	int NrFrames;
	int FramesPerSecond;
	int PictureSize;
	void *AnimStart;
	int *Pointer;
}ANIM_Data;

int InitRace01(void);
void UnloadRace01(void);

extern 		int 	*Race01_Objects;
extern 		int 	*Race01_Lights;
extern		TIM_Data Race01_TimingData;
extern		SceneData   *Race01;
extern		ANIM_Data Worm_AnimData;


