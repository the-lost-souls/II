.386
.model flat, C
.code

include w:\general\var.inc

public RotateObj
public SetVectorL1

RMatrix       Matrix44 ?
TMatrix       Matrix44 ?
XSin          DD 0.0
XCos          DD 0.0
YSin          DD 0.0
YCos          DD 0.0
ZSin          DD 0.0
ZCos          DD 0.0

One           DD 1.0


RotateObj     PROC
                mov    edi, SYS_PCaller
                mov    esi, SYS_POffs

;---- Increase angles...
                fld    d[esi + 00]
                fadd   d[esi + 24]
                fstp   d[esi + 00]
                fld    d[esi + 04]
                fadd   d[esi + 28]
                fstp   d[esi + 04]
                fld    d[esi + 08]
                fadd   d[esi + 32]
                fstp   d[esi + 08]

;---- Getting sines and cosines...
                fld    d[esi + 00]
                fsin
                fstp   XSin
                fld    d[esi + 00]
                fcos
                fstp   XCos

                fld    d[esi + 04]
                fsin
                fstp   YSin
                fld    d[esi + 04]
                fcos
                fstp   YCos

                fld    d[esi + 08]
                fsin
                fstp   ZSin
                fld    d[esi + 08]
                fcos
                fstp   ZCos


;---- Calculating constants...
;int 3
;m[0][0] =  cos(y)*cos(z)
                fld    YCos
                fmul   ZCos

                fstp   RMatrix.M44_00


;m[0][1] = -sin(z)*cos(y)
                fld    ZSin
                fchs
                fmul   YCos
                fstp   RMatrix.M44_01


;m[0][2] =  sin(y)
                fld    YSin
                fstp   RMatrix.M44_02


;m[1][0] =  sin(x)*sin(y)*cos(z) + cos(x)*sin(z)
                fld    XSin
                fmul   YSin
                fmul   ZCos

                fld    XCos
                fmul   ZSin

                faddp  st(1), st
                fstp   RMatrix.M44_10


;m[1][1] = -sin(z)*sin(x)*sin(y) + cos(x)*cos(z)
;int 3
                fld    XCos
                fmul   ZCos

                fld    ZSin
                fmul   XSin
                fmul   YSin

                fsubp  st(1), st
                fstp   RMatrix.M44_11


;m[1][2] = -sin(x)*cos(y)
                fld    XSin
                fmul   YCos
                fchs

                fstp   RMatrix.M44_12


;m[2][0] = -sin(y)*cos(x)*cos(z) + sin(x)*sin(z)
                fld    XSin
                fmul   ZSin

                fld    YSin
                fmul   XCos
                fmul   ZCos

                fsubp  st(1), st
                fstp   RMatrix.M44_20


;m[2][1] =  sin(y)*cos(x)*sin(z) + cos(z)*sin(x)
                fld    YSin
                fmul   XCos
                fmul   ZSin

                fld    ZCos
                fmul   XSin

                faddp  st(1), st
                fstp   RMatrix.M44_21


;m[2][2] =  cos(x)*cos(y)
;int 3
                fld    XCos
                fmul   YCos
                fstp   RMatrix.M44_22


                mov    RMatrix.M44_03, 0        ;Zero column 3
                mov    RMatrix.M44_13, 0
                mov    RMatrix.M44_23, 0

;---- Setting up translation matrix
                mov    esi, SYS_POffs
                mov    eax, [esi + 12]
                mov    TMatrix.M44_03, eax
                mov    eax, [esi + 16]
                mov    TMatrix.M44_13, eax
                mov    eax, [esi + 20]
                mov    TMatrix.M44_23, eax

                mov    eax, One
                mov    TMatrix.M44_00, eax
                mov    TMatrix.M44_01, 0
                mov    TMatrix.M44_02, 0

                mov    TMatrix.M44_10, 0
                mov    TMatrix.M44_11, eax
                mov    TMatrix.M44_12, 0

                mov    TMatrix.M44_20, 0
                mov    TMatrix.M44_21, 0
                mov    TMatrix.M44_22, eax

                mov    eax, OFFSET TMatrix
                mov    ebx, OFFSET RMatrix
                mov    edi, SYS_PCaller
                add    edi, H_M00
                call   M44_Mul

                mov    esi, SYS_PCaller
                fld    [esi].H_M22
                ffree  st

                ret
RotateObj     ENDP


SetVectorL1   PROC
;int 3
                mov    esi, SYS_PCaller
                fld    [esi].H_M02
                fstp   [esi].L_Pos[0]
                fld    [esi].H_M12
                fstp   [esi].L_Pos[4]
                fld    [esi].H_M22
                fstp   [esi].L_Pos[8]

                ret
SetVectorL1   ENDP


;旼컴컴컴컴컴컴컴컴컴컴컴컴 ASSEMBLER ROUTINE 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
;쿙ame         : M44_Mul
;쿔D           : M44
;쿟ype         : Procedure
;쿗ast update  : 28.09.1997
;쿌ction       : Multiplies to 4x4 matrices (A*B = C)
;쿚ptimized    : -
;�
;쿔nput variables : [eax] = matrix A
;�                  [ebx] = matrix B
;�                  [edi] = matrix C
;�
;쿙otes : Structure is set up row by row. 4th row assumed to be 0, 0, 0, 1.
;읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴

M44_Mul       PROC

;---- M[0][0] = Row A[0], column B[0]
                fld    [eax].M44_00
                fmul   [ebx].M44_00

                fld    [eax].M44_01
                fmul   [ebx].M44_10

                fld    [eax].M44_02
                fmul   [ebx].M44_20

                faddp  st(1), st
                faddp  st(1), st
                fstp   [edi].M44_00


;---- M[0][1] = Row A[0], column B[1]
                fld    [eax].M44_00
                fmul   [ebx].M44_01

                fld    [eax].M44_01
                fmul   [ebx].M44_11

                fld    [eax].M44_02
                fmul   [ebx].M44_21

                faddp  st(1), st
                faddp  st(1), st
                fstp   [edi].M44_01


;---- M[0][2] = Row A[0], column B[2]
                fld    [eax].M44_00
                fmul   [ebx].M44_02

                fld    [eax].M44_01
                fmul   [ebx].M44_12

                fld    [eax].M44_02
                fmul   [ebx].M44_22

                faddp  st(1), st
                faddp  st(1), st
                fstp   [edi].M44_02


;---- M[0][3] = Row A[0], column B[3]
                fld    [eax].M44_00
                fmul   [ebx].M44_03

                fld    [eax].M44_01
                fmul   [ebx].M44_13

                fld    [eax].M44_02
                fmul   [ebx].M44_23

                faddp  st(1), st
                faddp  st(1), st
                fadd   [eax].M44_03
                fstp   [edi].M44_03


;---- M[1][0] = Row A[1], column B[0]
                fld    [eax].M44_10
                fmul   [ebx].M44_00

                fld    [eax].M44_11
                fmul   [ebx].M44_10

                fld    [eax].M44_12
                fmul   [ebx].M44_20

                faddp  st(1), st
                faddp  st(1), st
                fstp   [edi].M44_10


;---- M[1][1] = Row A[1], column B[1]
                fld    [eax].M44_10
                fmul   [ebx].M44_01

                fld    [eax].M44_11
                fmul   [ebx].M44_11

                fld    [eax].M44_12
                fmul   [ebx].M44_21

                faddp  st(1), st
                faddp  st(1), st
                fstp   [edi].M44_11


;---- M[1][2] = Row A[1], column B[2]
                fld    [eax].M44_10
                fmul   [ebx].M44_02

                fld    [eax].M44_11
                fmul   [ebx].M44_12

                fld    [eax].M44_12
                fmul   [ebx].M44_22

                faddp  st(1), st
                faddp  st(1), st
                fstp   [edi].M44_12


;---- M[1][3] = Row A[1], column B[3]
                fld    [eax].M44_10
                fmul   [ebx].M44_03

                fld    [eax].M44_11
                fmul   [ebx].M44_13

                fld    [eax].M44_12
                fmul   [ebx].M44_23

                faddp  st(1), st
                faddp  st(1), st
                fadd   [eax].M44_13
                fstp   [edi].M44_13


;---- M[2][0] = Row A[2], column B[0]
                fld    [eax].M44_20
                fmul   [ebx].M44_00

                fld    [eax].M44_21
                fmul   [ebx].M44_10

                fld    [eax].M44_22
                fmul   [ebx].M44_20

                faddp  st(1), st
                faddp  st(1), st
                fstp   [edi].M44_20


;---- M[2][1] = Row A[2], column B[1]
                fld    [eax].M44_20
                fmul   [ebx].M44_01

                fld    [eax].M44_21
                fmul   [ebx].M44_11

                fld    [eax].M44_22
                fmul   [ebx].M44_21

                faddp  st(1), st
                faddp  st(1), st
                fstp   [edi].M44_21


;---- M[2][2] = Row A[2], column B[2]
                fld    [eax].M44_20
                fmul   [ebx].M44_02

                fld    [eax].M44_21
                fmul   [ebx].M44_12

                fld    [eax].M44_22
                fmul   [ebx].M44_22

                faddp  st(1), st
                faddp  st(1), st
                fstp   [edi].M44_22


;---- M[2][3] = Row A[2], column B[3]
                fld    [eax].M44_20
                fmul   [ebx].M44_03

                fld    [eax].M44_21
                fmul   [ebx].M44_13

                fld    [eax].M44_22
                fmul   [ebx].M44_23

                faddp  st(1), st
                faddp  st(1), st

                fadd   [eax].M44_23
                fstp   [edi].M44_23

                ret
M44_Mul       ENDP

END
