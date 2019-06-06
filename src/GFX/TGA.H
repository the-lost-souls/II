/*
旼컴컴컴컴컴컴컴컴컴컴컴컴 WATCOM C++ INCLUDEFILE 컴컴컴컴컴컴컴컴컴컴컴컴컴
쿙ame         : TGA.H
쿌uthor       : Cyberfish/TLS
쿗ast update  : 22.12.97
쿌ction       : Definition of TGA_Load-routine + public variables.
�
쿙otes :
읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴*/

#ifndef TGA_Def
  #define TGA_Def

unsigned char *TGA_Load(const char *FName, int FOffset, int FSize);

extern  int 	TGA_Width;              //Image width
extern  int 	TGA_Heigth;             //Image heigth
extern  int 	TGA_XPos;               //Image horisontal position
extern  int 	TGA_YPos;               //Image vertical position
extern  int 	TGA_Bits;               //Bits per pixel

#endif

