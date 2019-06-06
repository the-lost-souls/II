;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ ASSEMBLER SOURCE ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
;³Name         : GLOW.ASM
;³Author       : Cyberfish/TLS
;³Last update  : 14.03.1998
;³Action       : Plugin/part for "glow"-map. For lights and stuff.
;³
;³Notes :
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
.386
.model flat, C
.code

public GLW_LightPlugin
public GLW_Plugin
public GLW

include w:\general\var.inc
include w:\vbe\vbe.inc

ALIGN 4

Fixed032        DD 4294967295.0
_1p0		DD 1.0
_0p5		DD 0.5
_65536p0	DD 65536.0

GlowData      STRUC

GLW_NextOffset	DD ?
GLW_SortValue	DD ?
GLW_Type   	DD ?
GLW_Priority	DD ?

GLW_XPos  	DD ?
GLW_YPos  	DD ?
GLW_ZPos  	DD ?

GLW_Scale	DD ?

GLW_R		DD ?
GLW_G		DD ?
GLW_B		DD ?

GLW_MapXSize	DD ?
GLW_MapYSize    DD ?

GLW_Map 	DD ?

	      ENDS


;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ ASSEMBLER ROUTINE ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
;³Name         : GLW_LightPlugin
;³ID           : GLW
;³Type         : Procedure
;³Last update  : 14.03.1998
;³Action       : Gets the caller's position and intensity, sets up the part
;³Optimized    : -
;³
;³Input variables : SYS_PCaller, SYS_POffs
;³
;³Notes : Must be called after caller's matrix is set
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

GLW_LightPlugin PROC
		mov	esi, SYS_PCaller
		mov	edi, SYS_POffs

		fld	[esi].L_R
		fmul	[esi].L_Intensity
		fstp	[edi].GLW_R
		fld	[esi].L_G
		fmul	[esi].L_Intensity
		fstp	[edi].GLW_G
		fld	[esi].L_B
		fmul	[esi].L_Intensity
		fstp	[edi].GLW_B

		fldcw   FPU_Round

;---- Setting up position...
		mov	esi, SYS_PCaller
		mov	edi, SYS_POffs

		fld	[esi].L_Matrix[3*4]
		fstp	[edi].GLW_XPos
		fld	[esi].L_Matrix[7*4]
		fstp	[edi].GLW_YPos
		fld	[esi].L_Matrix[11*4]
		fstp	[edi].GLW_ZPos

                fld	[edi].GLW_ZPos
		fdivr	_1p0    		;Converting 1/Z into fixed
                fmul	Fixed032		;0.32 fixed point
                fistp	[edi].GLW_SortValue	;Store it as integer
		mov	[edi].GLW_SortValue, 1


;---- Insert flare in part-lists...
                mov	edx, SRT_End1
                mov	eax, [edi].GLW_SortValue
                and	eax, 255                ;Mask out first byte (radix)
                mov	ebx, 255                ;sort highest first
                sub	ebx, eax                ;/
                mov	eax, [edx + ebx*4]      ;Load end of list
                mov	[eax], edi              ;Update end-poly
                mov	[edx + ebx*4], edi      ;Update end-list

		ret
GLW_LightPlugin ENDP

;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ ASSEMBLER ROUTINE ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
;³Name         : GLW_Plugin
;³ID           : GLW
;³Type         : Procedure
;³Last update  : 14.03.1998
;³Action       : Gets the caller's position and sets up a glow-part
;³Optimized    : -
;³
;³Input variables : SYS_PCaller, SYS_POffs
;³
;³Notes : Must be called after caller's matrix is set
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

GLW_Plugin    PROC
		mov	esi, SYS_PCaller
		mov	edi, SYS_POffs

		fldcw   FPU_Round

;---- Setting up position...
		mov	esi, SYS_PCaller
		mov	edi, SYS_POffs

		fld	[esi].H_M03
		fstp	[edi].GLW_XPos
		fld	[esi].H_M13
		fstp	[edi].GLW_YPos
		fld	[esi].H_M23
		fstp	[edi].GLW_ZPos

                fld	[edi].GLW_ZPos
		fdivr	_1p0    		;Converting 1/Z into fixed
                fmul	Fixed032		;0.32 fixed point
                fistp	[edi].GLW_SortValue	;Store it as integer
		mov	[edi].GLW_SortValue, 1


;---- Insert flare in part-lists...
                mov	edx, SRT_End1
                mov	eax, [edi].GLW_SortValue
                and	eax, 255                ;Mask out first byte (radix)
                mov	ebx, 255                ;sort highest first
                sub	ebx, eax                ;/
                mov	eax, [edx + ebx*4]      ;Load end of list
                mov	[eax], edi              ;Update end-poly
                mov	[edx + ebx*4], edi      ;Update end-list

		ret
GLW_Plugin    ENDP





;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ ASSEMBLER ROUTINE ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
;³Name         : GLW_
;³ID           : GLW
;³Type         : Procedure
;³Last update  : 28.02.1998
;³Action       : Draws the glow-map
;³Optimized    : -
;³
;³Input variables : esi
;³
;³Notes :
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

ALIGN 4

X0		DD ?
Y0		DD ?
X1		DD ?
Y1		DD ?
U0		DD ?
V0		DD ?
U0Frac		DD ?
V0Frac		DD ?

USlope		DD ?
VSlope		DD ?
UFrac		DD ?
VFrac		DD ?

X0Int		DD ?
Y0Int		DD ?
X1Int		DD ?
Y1Int		DD ?

MapXSizeInt	DD ?

LUT 		DD 16*32*3 DUP (?)		;transparencylevels*
						;colors*components
_16p0		DD 16.0
_31p0		DD 31.0
FPTemp1		DD 0
FPTemp2		DD 0

NZInt		DD 0


GLW           PROC
		fldcw	FPU_Round
;---- Setting red lut...
		mov	edi, OFFSET LUT
		xor	edx, edx

RColLoop:
		xor	ecx, ecx

RTranspLoop:
						;st0  st1  st2  st3
		fld	[esi].GLW_R		;R
		fmul	_31p0
		mov	FPTemp1, edx
		fisub	FPTemp1  		;R-C

		fdiv	_16p0   		;slope
		mov	FPTemp2, ecx
		fimul	FPTemp2
		fiadd   FPTemp1

		fistp	FPTemp1 		;round and store

		mov	eax, FPTemp1    	;store into lut...
		shl	eax, 10
		mov	[edi], eax		;

		add	edi, 4

		inc	ecx
		cmp	ecx, 16
		jb	RTranspLoop

		inc	edx
		cmp	edx, 32
		jb	RColLoop


;---- Setting green lut...
		xor	edx, edx

	GColLoop:
		xor	ecx, ecx

	GTranspLoop:
						;st0  st1  st2  st3
		fld	[esi].GLW_G		;R
		fmul	_31p0
		mov	FPTemp1, edx
		fisub	FPTemp1  		;R-C

		fdiv	_16p0   		;slope
		mov	FPTemp2, ecx
		fimul	FPTemp2
		fiadd   FPTemp1

		fistp	FPTemp1 		;round and store

		mov	eax, FPTemp1    	;store into lut...
		shl	eax, 5
		mov	[edi], eax		;

		add	edi, 4

		inc	ecx
		cmp	ecx, 16
		jb	GTranspLoop

		inc	edx
		cmp	edx, 32
		jb	GColLoop


;---- Setting blue lut...
		xor	edx, edx

	BColLoop:
		xor	ecx, ecx

	BTranspLoop:
						;st0  st1  st2  st3
		fld	[esi].GLW_B		;R
		fmul	_31p0
		mov	FPTemp1, edx
		fisub	FPTemp1  		;R-C

		fdiv	_16p0   		;slope
		mov	FPTemp2, ecx
		fimul	FPTemp2
		fiadd   FPTemp1

		fistp	FPTemp1 		;round and store

		mov	eax, FPTemp1    	;store into lut...
		mov	[edi], eax		;

		add	edi, 4

		inc	ecx
		cmp	ecx, 16
		jb	BTranspLoop

		inc	edx
		cmp	edx, 32
		jb	BColLoop



;---- Lookuptables done...

                                              	;st0  st1  st2  st3  st4  st5  st6  st7
		fld	[esi].GLW_ZPos		;Z

		fcom	SYS_ZClip		;Behind camera?
		fstsw	ax
		sahf
		jbe	GlowDone

		fdivr	_1p0    		;1/Z
		fld	st(0)
		fmul	Fixed032
		fistp	NZInt

;---- Projecting X-coordinates...
		fld	[esi].GLW_MapXSize	;Sz   1/Z
		fist 	MapXSizeInt
		fmul	_0p5			;Sz/2 1/Z
		fmul	[esi].GLW_Scale

		fld	st			;Sz/2 Sz/2 1/Z
		fsubr   [esi].GLW_XPos		;X0   Sz/2 1/Z
		fmul	SYS_XPerspective	;X0   Sz/2 1/Z
		fmul	st, st(2)		;X0   Sz/2 1/Z
		fadd	SYS_XCenter		;X0   Sz/2 1/Z
		fst 	X0			;Sz/2 Sz/2 1/Z

		fcom	SYS_FPClipRight		;outside screen?
		fstsw	ax
		sahf
		jae	GlowDone

		fcom    SYS_FPClipLeft          ;clipping?
		fstsw	ax
		sahf
		jae     NoClipLeft

		fstp	X0Int			;Dump...
		fld	SYS_FPClipLeft
NoClipLeft:

		fldcw	FPU_RoundUp
		fistp	X0Int

		fadd	[esi].GLW_XPos		;X1   1/Z
		fmul	SYS_XPerspective	;X1   1/Z
		fmul	st, st(1)		;X1   1/Z
		fadd	SYS_XCenter		;X1   1/Z
		fst 	X1			;X1   1/Z

		fcom	SYS_FPClipLeft          ;outside screen?
		fstsw	ax
		sahf
		jbe	GlowDone

		fcom    SYS_FPClipRight         ;clipping?
		fstsw	ax
		sahf
		jbe     NoClipRight

		fstp	X1Int			;Dump...
		fld	SYS_FPClipRight
NoClipRight:

		fldcw	FPU_RoundDown
		fistp	X1Int


;---- Projecting Y-coordinates...
		fld	[esi].GLW_MapYSize	;Sz   1/Z
		fmul	_0p5			;Sz/2 1/Z
		fmul	[esi].GLW_Scale

		fld	st			;Sz/2 Sz/2 1/Z
		fsubr   [esi].GLW_YPos		;Y0   Sz/2 1/Z
		fmul	SYS_YPerspective	;Y0   Sz/2 1/Z
		fmul	st, st(2)		;Y0   Sz/2 1/Z
		fadd	SYS_YCenter		;Y0   Sz/2 1/Z
		fst 	Y1			;Sz/2 Sz/2 1/Z

		fcom	SYS_FPClipTop           ;outside screen?
		fstsw	ax
		sahf
		jbe	GlowDone

		fcom    SYS_FPClipBottom        ;clipping?
		fstsw	ax
		sahf
		jbe     NoClipBottom

		fstp	Y1Int			;Dump...
		fld	SYS_FPClipBottom
		fadd	_1p0
NoClipBottom:


		fldcw	FPU_RoundDown
		fistp	Y1Int

		fadd	[esi].GLW_YPos		;Y1   1/Z
		fmul	SYS_YPerspective	;Y1   1/Z
		fmulp   st(1), st		;Y1
		fadd	SYS_YCenter		;Y1
		fst 	Y0			;Y1

		fcom	SYS_FPClipBottom        ;outside screen?
		fstsw	ax
		sahf
		jae	GlowDone

		fcom    SYS_FPClipTop           ;clipping?
		fstsw	ax
		sahf
		jae     NoClipTop

		fstp	Y0Int
		fld	SYS_FPClipTop
NoClipTop:

		fldcw	FPU_RoundUp
		fistp	Y0Int

;---- Getting mapping slopes and mapcoords...
		fld     X1
		fsub	X0
		fld	[esi].GLW_MapXSize
		fsub	_1p0
		fdivrp  st(1), st		;USlope

		fild	X0Int			;subpixel accuracy
		fsub	X0                      ;/
		fmul	st, st(1)               ;/
		fmul	_65536p0                ;16.16 fixed point
		fistp	U0

		fmul	_65536p0		;16.16 fixed point
		fistp	USlope

		fld	Y1
		fsub	Y0
		fld	[esi].GLW_MapYSize
		fsub	_1p0
		fdivrp  st(1), st               ;VSlope

		fild	Y0Int			;subpixel accuracy
		fsub	Y0                      ;/
		fmul	st, st(1)               ;/
		fmul	_65536p0                ;16.16 fixed point
		fistp	V0

		fmul	_65536p0                ;16.16 fixed point
		fistp	VSlope

		add	U0, 32768		;add 0.5 fixed point
		add	V0, 32768		;/

		mov	eax, U0			;Setting starting-frac U/V
		mov	edx, V0
		shl	eax, 16
		shl	edx, 16
		mov	U0Frac, eax
		mov	V0Frac, edx

		mov	eax, U0			;Setting starting-frac U/V
		mov	edx, V0
		shr	eax, 16
		shr	edx, 16
		mov	U0, eax
		mov	V0, edx

		mov	eax, USlope
		mov	edx, USlope
		shl	eax, 16
		shr	edx, 16
		mov	UFrac, eax		;Setting up 0.32 fixed frac
		mov	USlope, edx             ;...and int-part

		mov	eax, VSlope
		mov	edx, VSlope
		shl	eax, 16
		shr	edx, 16
		mov	VFrac, eax		;Setting up 0.32 fixed frac
		mov	VSlope, edx		;...and int-part

		mov	eax, [esi].GLW_Map	;setting fixed mapoffset
		mov	d[MapOffs - 4], eax

		mov	eax, SYS_DBufOffs	;setting fixed dbuf-offset
		mov	d[DBufOffs1 - 4], eax
		mov	d[DBufOffs2 - 4], eax

		mov	eax, SYS_ZBufOffs	;setting fixed dbuf-offset
		mov	d[ZBufOffs1 - 4], eax

;---- Rendering-loops...
		mov	edx, Y1Int		;edx = y-counter
		sub	edx, Y0Int

		mov	ebp, VFrac		;ebp = V fractional part

YLoop:						;Vertical loop
		cmp	edx, 0
		jle	GlowDone

		mov	edi, Y0Int		;edi = dbuf/zbuf-pointer
		imul	edi, VBE_XRes
		add	edi, X0Int

		mov	esi, V0			;esi = map-pointer
		imul	esi, MapXSizeInt
		add	esi, U0

		mov	ecx, X1Int		;ecx = xcounter
		sub	ecx, X0Int

		mov	ebp, U0Frac
		xor	eax, eax


		push	edx

		cmp	ecx, 0
		jle	GlowDoneX


;---- Horisontal loop...

XLoop:
		mov	edx, NZInt
		cmp	edx, [edi*4 + 12345678h]
ZBufOffs1:
		jbe     DumpPix

		mov     al, [esi + 12345678h]	;Load transparency-value
MapOffs:
		shl	eax, 2
		mov	dx, [edi*2 + 12345678h] ;Get pixel
DBufOffs1:

		mov	ebp, 0111110000000000b  ;Get new red value
		mov	ebx, 01111100000b	;Get new green value
		and	ebp, edx
		and	ebx, edx
		shr	ebp, 6
		shr	ebx, 1
		and	edx, 011111b		;Get new blue value
		shl	edx, 4
		mov	ebp, LUT[ebp*4 + eax]
		mov	ebx, LUT[ebx*4 + 2048 + eax]
		or	ebp, LUT[edx*4 + 4096 + eax]
		or	ebp, ebx

		mov	[edi*2 + 12345678h], bp
DBufOffs2:
DumpPix:

		mov	ebp, U0Frac
		add	ebp, UFrac
		adc	esi, USlope
		mov	U0Frac, ebp

		inc	edi
		dec	ecx
		jnz	XLoop

GlowDoneX:

		mov	eax, V0
		mov	ebx, V0Frac
		add	ebx, VFrac
		adc	eax, VSlope
		mov	V0, eax
		mov	V0Frac, ebx

		inc	Y0Int

		pop	edx
		dec	edx
		jmp	YLoop

GlowDone:

		ffree	st(0)
		ffree	st(1)
		ffree	st(2)
		ffree	st(3)
		ffree	st(4)

		ret
GLW           ENDP


END 
