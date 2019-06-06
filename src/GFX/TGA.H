/*
�������������������������� WATCOM C++ INCLUDEFILE ��������������������������
�Name         : TGA.H
�Author       : Cyberfish/TLS
�Last update  : 22.12.97
�Action       : Definition of TGA_Load-routine + public variables.
�
�Notes :
����������������������������������������������������������������������������*/

#ifndef TGA_Def
  #define TGA_Def

unsigned char *TGA_Load(const char *FName, int FOffset, int FSize);

extern  int 	TGA_Width;              //Image width
extern  int 	TGA_Heigth;             //Image heigth
extern  int 	TGA_XPos;               //Image horisontal position
extern  int 	TGA_YPos;               //Image vertical position
extern  int 	TGA_Bits;               //Bits per pixel

#endif
