;旼컴컴컴컴컴컴컴컴컴컴컴컴 ASSEMBLER SOURCE 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
;쿙ame         : V03.ASM
;쿌uthor       : Cyberfish/TLS & Toe/DU
;쿗ast update  : 10.03.1998
;쿌ction       : Object routine for CVEC_03 (objtype 5) (flatshading)
;�
;쿙otes :
;읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴

.386
.Model flat, C
.code

public V03

include w:\general\var.inc
include w:\vectors\V03macro.inc
include w:\render\render.inc
include w:\vectors\lights.inc


;旼컴컴컴컴컴컴컴컴컴컴컴컴 ASSEMBLER ROUTINE 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
;쿙ame         : V03
;쿔D           : V03
;쿟ype         : Procedure
;쿗ast update  : 10.03.1998
;쿌ction       : Object routine for CVEC_03 (perspective mapping)
;쿚ptimized    : Nope
;�
;쿔nput variables : SYS_OOffs = Offset object
;�
;쿝egisters changed : All!
;�
;쿙otes : FPU stack must be empty.
;읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴

XSin            DD 0.0
XCos            DD 0.0
YSin            DD 0.0
YCos            DD 0.0
ZSin            DD 0.0
ZCos            DD 0.0


One             DD 1.0
Zero            DD 0.0
RFlag           DD 0
PFlag           DD 0
Fixed032        DD 4294967295.0
_31p0		DD 31.0

FPDump		DD 0

V03           PROC
                mov    esi, SYS_OOffs
                inc    RFlag

                mov    ecx, [esi].H_Faces
                mov    edi, [esi].H_FaceOffs
VecLoop:
                mov    ebp, [edi].F_V0Offs
                mov    eax, [ebp].V_RFlag
                cmp    eax, RFlag
                je     V0Done

                CalculateVert                   ;[ebp] = vertice structure
V0Done:
;---- Rotating normalvector X
                fld    [edi].F_NVX
                fmul   [esi].H_M00
                fld    [edi].F_NVY
                fmul   [esi].H_M01
                fld    [edi].F_NVZ
                fmul   [esi].H_M02
                faddp  st(1), st
                faddp  st(1), st
		fst	LIT_NVX
                fmul   [ebp].V_XRotated         ;Mul by "cameravector"

;---- Rotating normalvector Y
                fld    [edi].F_NVX
                fmul   [esi].H_M10
                fld    [edi].F_NVY
                fmul   [esi].H_M11
                fld    [edi].F_NVZ
                fmul   [esi].H_M12
                faddp  st(1), st
                faddp  st(1), st
		fst	LIT_NVY
                fmul   [ebp].V_YRotated         ;Mul by "cameravector"

;---- Rotating normalvector Z
                fld    [edi].F_NVX
                fmul   [esi].H_M20
                fld    [edi].F_NVY
                fmul   [esi].H_M21
                fld    [edi].F_NVZ
                fmul   [esi].H_M22
                faddp  st(1), st
                faddp  st(1), st
		fst	LIT_NVZ
                fmul   [ebp].V_ZRotated         ;Mul by "cameravector"

;---- Calculating dot-product
                faddp  st(1), st
                faddp  st(1), st                ;stack = NormalV dot CameraV


                fcomp  Zero                     ;Face visible?
                fstsw  ax
                sahf
                jae    DumpFace                 ;jae coz the sign is wrong...

                mov    eax, [ebp].V_ZFlag       ;Update poly's Z-flag
                mov    [edi].F_ZFlag, eax       ;/
                mov    eax, [ebp].V_PFlag       ;Update poly's P-flag
                mov    PFlag, eax               ;/

                mov    ebp, [edi].F_V1Offs
                mov    eax, [ebp].V_RFlag
                cmp    eax, RFlag
                je     V1Done

                CalculateVert
V1Done:
                mov    eax, [ebp].V_ZFlag       ;Update poly's Z-flag
                add    [edi].F_ZFlag, eax       ;/
                mov    eax, [ebp].V_PFlag       ;Update poly's P-flag
                and    PFlag, eax               ;/

                mov    ebp, [edi].F_V2Offs
                mov    eax, [ebp].V_RFlag
                cmp    eax, RFlag
                je     V2Done

                CalculateVert

V2Done:
                mov    eax, [ebp].V_ZFlag       ;Update poly's Z-flag
                add    [edi].F_ZFlag, eax       ;/
                cmp    [edi].F_ZFlag, 3         ;Entire polygon behind?
                je     DumpFace                 ;yes...

                mov    eax, [ebp].V_PFlag       ;Update poly's P-flag
                and    PFlag, eax               ;/
                jnz    DumpFace                 ;/entire face outside screen

                fld    [ebp].V_InvZ             ;Converting 1/Z into fixed
                fmul   Fixed032                 ;0.32 fixed point
                fistp  [edi].F_SortValue        ;Store it as integer


;---- Running lights...
;OBS! Don't change esi, edi, ecx
;eax, ebx, edx, ebp is free

                mov    LIT_ROut, 0
                mov    LIT_GOut, 0
                mov    LIT_BOut, 0

                LIT_Run

                fld    LIT_ROut
		fmul	SYS_Ambient
		fcom    _31p0
		fstsw	ax
		sahf
		jb	ROK
		fstp	FPDump
		fld	_31p0
ROK:
                fstp   [edi].F_IntensityR

                fld    LIT_GOut
		fmul	SYS_Ambient
		fcom    _31p0
		fstsw	ax
		sahf
		jb	GOK
		fstp	FPDump
		fld	_31p0
GOK:
                fstp   [edi].F_IntensityG


                fld    LIT_BOut
		fmul	SYS_Ambient
		fcom    _31p0
		fstsw	ax
		sahf
		jb	BOK
		fstp	FPDump
		fld	_31p0
BOK:
                fstp   [edi].F_IntensityB


;---- Done lights





;---- Insert polygon in part-lists...
                mov    edx, SRT_End1
                mov    eax, [edi].F_SortValue
                and    eax, 255                 ;Mask out first byte (radix)
                mov    ebx, 255                 ;sort highest first
                sub    ebx, eax                 ;/
                mov    eax, [edx + ebx*4]       ;Load end of list
                mov    [eax], edi               ;Update end-poly
                mov    [edx + ebx*4], edi       ;Update end-list

		mov	[edi].F_Priority, 250

DumpFace:

                add    edi, F_Size              ;Next face
                dec    ecx
                jnz    VecLoop

                ret
V03           ENDP

END


