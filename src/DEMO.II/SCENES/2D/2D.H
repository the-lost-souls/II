#include "w:\general\var.h"

int InitDraw(void);
void UnloadDraw(void);

typedef struct
{
	float	*MapCoords;
	float	Angle;
	float	UCenter1, VCenter1, UCenter2, VCenter2;
	int	StartTime;
}MapCoordPluginData;

extern 		int 	*Draw_Objects;
extern 		int 	*Draw_Lights;
extern		TIM_Data Draw_TimingData;
extern		SceneData   *Draw;
extern		MapCoordPluginData MapCoordData;


