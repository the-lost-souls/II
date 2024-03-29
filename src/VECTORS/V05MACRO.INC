;旼컴컴컴컴컴컴컴컴컴컴컴컴 ASSEMBLER SOURCE 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
;쿙ame         : V01MACRO.INC
;쿌uthor       : Cyberfish/TLS & Toe/DU
;쿗ast update  : 25.09.97
;쿌ction       : Div macros for V01
;�
;쿙otes :
;읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴




;旼컴컴컴컴컴컴컴컴컴컴컴컴 ASSEMBLER ROUTINE 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
;쿙ame         : CalculateVert
;쿔D           : -
;쿟ype         : Macro
;쿗ast update  : 25.09.97
;쿌ction       : Verticerotation
;쿚ptimized    : Nope
;�
;쿔nput variables : [esi] = Object header
;�                  [ebp] = vertice-structure
;�
;쿝egisters changed : No integer registers changed
;�
;쿙otes : FPU stack must be empty.
;읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴

CalculateVert MACRO
LOCAL VertexBehind

;NewX = (X * m[0][0]) + (Y * m[0][1]) + (Z * m[0][2]) + m[0][3]
                fld    [ebp].V_X
                fmul   [esi].H_M00
                fld    [ebp].V_Y
                fmul   [esi].H_M01
                fld    [ebp].V_Z
                fmul   [esi].H_M02
                faddp  st(1), st
                faddp  st(1), st
                fadd   [esi].H_M03
                fstp   [ebp].V_XRotated


;NewY = (X * m[1][0]) + (Y * m[1][1]) + (Z * m[1][2]) + m[1][3]
                fld    [ebp].V_X
                fmul   [esi].H_M10
                fld    [ebp].V_Y
                fmul   [esi].H_M11
                fld    [ebp].V_Z
                fmul   [esi].H_M12
                faddp  st(1), st
                faddp  st(1), st
                fadd   [esi].H_M13
                fstp   [ebp].V_YRotated

;NewZ = (X * m[2][0]) + (Y * m[2][1]) + (Z * m[2][2]) + m[2][3]
                fld    [ebp].V_X
                fmul   [esi].H_M20
                fld    [ebp].V_Y
                fmul   [esi].H_M21
                fld    [ebp].V_Z
                fmul   [esi].H_M22
                faddp  st(1), st
                faddp  st(1), st
                fadd   [esi].H_M23
                fst    [ebp].V_ZRotated



                fcomp  SYS_ZClip                ;Check if vertex is behind
                fstsw  ax                       ;/
                sahf                            ;/
                mov    eax, 1                   ;eax used as Z-flag
                mov    ebx, 0                   ;ebx used as P-flag
                mov    edx, 0                   ;edx used as status-flag
                jb     VertexBehind             ;/

                fld1
                fld    [ebp].V_ZRotated
                fdivp  st(1), st
                fst    [ebp].V_InvZ

;---- Projecting and checking X...
                fld    [ebp].V_XRotated
                fmul   SYS_XPerspective
                fmul   st, st(1)
                fadd   SYS_XCenter

                fcom   SYS_FPClipLeft           ;Check left edge
                fstsw  ax
                sahf
                setb   dl
                or     ebx, edx                 ;Set bit 0 if outleft

                fcom   SYS_FPClipRight          ;Check left edge
                fstsw  ax
                sahf
                seta   dl
                shl    dl, 1
                or     ebx, edx                 ;Set bit 1 if outright

                fstp   [ebp].V_X2D              ;Store 2D-X


;---- Projecting and checking Y...
                fmul   [ebp].V_YRotated
                fmul   SYS_YPerspective
                fadd   SYS_YCenter

                fcom   SYS_FPClipTop            ;Check upper edge
                fstsw  ax
                sahf
                shl    dl, 2
                setb   dl
                or     ebx, edx                 ;Set bit 2 if outleft

                fcom   SYS_FPClipBottom
                fstsw  ax
                sahf
                seta   dl
                shl    dl, 3
                or     ebx, edx                 ;Set bit 3 if outright

                fstp   [ebp].V_Y2D

                xor    eax, eax                 ;Zero Z-flag, vertex in front

VertexBehind:
                mov    [ebp].V_PFlag, ebx       ;Set vertex's p-flag
                mov    [ebp].V_ZFlag, eax       ;Set vertex's z-flag

                inc    [ebp].V_RFlag

              ENDM