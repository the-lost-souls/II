
void *TXT_LoadTexture(	const char *FileName,
			int FileOffset,
			int FileSize,
			int &Width,
			int &Heigth);
void TXT_UnloadTexture(void *TextureOffset);

extern 	int TXT_Textures[];
