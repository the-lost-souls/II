;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ ASSEMBLER SOURCE ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
;³Name         : CANVAS.ASM
;³Author       : Cyberfish/TLS
;³Last update  : 10.02.1998
;³Action       : Routines for viewing the canvas.
;³
;³Notes :
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
.386
.model flat, C
.code

include w:\general\var.inc
include w:\vbe\vbe.inc

public	CNV_RNormal_Banked
public	CNV_RNormal_Linear
public	CNV_RStrange_Linear
public	CNV_ONormal
public	CNV_RGrey
public	CNV_OGrey
public	CNV_R8bitRGB
public	CNV_O8bitRGB
public	CNV_RStretch
public	CNV_OStretch


CNV_ONormal     DD 0004
                DD -1                           ;No plugins

;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ ASSEMBLER ROUTINE ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
;³Name         : CNV_Normal_Banked_
;³ID           : CNV
;³Type         : Procedure
;³Last update  : 10.02.1998
;³Action       : Views dbuf, blanks dbuf, blanks zbuf, 32k, bank switched
;³Optimized    : -
;³
;³Input variables : SYS_VideoMem
;³
;³Registers changed :
;³
;³Notes :
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
CNV_RNormal_Banked      PROC
		mov	esi, SYS_DBufOffs
		mov	edi, SYS_ScreenShot
		mov	ecx, SYS_DBufSize
		shr	ecx, 2
		rep	movsd

		mov	esi, SYS_DBufOffs
		mov	ebx, SYS_DBufSize
		mov	ebp, SYS_ZBufOffs
		xor	edx, edx
		fldz


Normal_Banked1:

		mov	ecx, ebx
		cmp	ecx, 65536
		jbe	Normal_Banked3
		mov	ecx, 65536
Normal_Banked3:
		sub	ebx, ecx
		shr	ecx, 6
		mov	edi, 0A0000h

		push	ebx
		push	edx

                mov   	ah, 4Fh                 ;SVGA services
                mov   	al, 05h                 ;SVGA videomem window control
                mov   	bh, 0                   ;SVGA bank select
                mov   	bl, 0                   ;window A
                                                ;edx = bank
                int   	10h

		xor	edx, edx
Normal_Banked2:
		fst	q[ebp +   0 ]
                mov    	eax, [ esi + 00 ]
                mov    	ebx, [ esi + 32 ]
                mov    	[ esi + 00 ], edx
                mov    	[ esi + 32 ], edx
		fst	q[ebp +   8 ]
                mov    	[ edi + 00 ], eax
                mov    	[ edi + 32 ], ebx

		fst	q[ebp +  16 ]
                mov    	eax, [ esi + 04 ]
                mov    	ebx, [ esi + 36 ]
                mov    	[ esi + 04 ], edx
                mov    	[ esi + 36 ], edx
		fst	q[ebp +  24 ]
                mov    	[ edi + 04 ], eax
                mov    	[ edi + 36 ], ebx

		fst	q[ebp +  32 ]
                mov    	eax, [ esi + 08 ]
                mov    	ebx, [ esi + 40 ]
                mov    	[ esi + 08 ], edx
                mov    	[ esi + 40 ], edx
		fst	q[ebp +  40 ]
                mov    	[ edi + 08 ], eax
                mov    	[ edi + 40 ], ebx

		fst	q[ebp +  48 ]
                mov    	eax, [ esi + 12 ]
                mov    	ebx, [ esi + 44 ]
                mov    	[ esi + 12 ], edx
                mov    	[ esi + 44 ], edx
		fst	q[ebp +  56 ]
                mov    	[ edi + 12 ], eax
                mov    	[ edi + 44 ], ebx

		fst	q[ebp +  64 ]
                mov    	eax, [ esi + 16 ]
                mov    	ebx, [ esi + 48 ]
                mov    	[ esi + 16 ], edx
                mov    	[ esi + 48 ], edx
		fst	q[ebp +  72 ]
                mov    	[ edi + 16 ], eax
                mov    	[ edi + 48 ], ebx

		fst	q[ebp +  80 ]
                mov    	eax, [ esi + 20 ]
                mov    	ebx, [ esi + 52 ]
                mov    	[ esi + 20 ], edx
                mov    	[ esi + 52 ], edx
		fst	q[ebp +  88 ]
                mov    	[ edi + 20 ], eax
                mov    	[ edi + 52 ], ebx

		fst	q[ebp +  96 ]
                mov    	eax, [ esi + 24 ]
                mov    	ebx, [ esi + 56 ]
                mov    	[ esi + 24 ], edx
                mov    	[ esi + 56 ], edx
		fst	q[ebp + 104 ]
                mov    	[ edi + 24 ], eax
                mov    	[ edi + 56 ], ebx

		fst	q[ebp + 112 ]
                mov    	eax, [ esi + 28 ]
                mov    	ebx, [ esi + 60 ]
                mov    	[ esi + 28 ], edx
                mov    	[ esi + 60 ], edx
		fst	q[ebp + 120 ]
                mov    	[ edi + 28 ], eax
                mov    	[ edi + 60 ], ebx

		add	ebp, 128
                add    	esi, 64
                add    	edi, 64
                dec    	ecx
                jnz    	Normal_Banked2

		pop	edx
		pop	ebx

		inc	edx			;Next bank

		or	ebx, ebx
		jnz	Normal_Banked1

		ret
CNV_RNormal_Banked      ENDP


;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ ASSEMBLER ROUTINE ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
;³Name         : CNV_Normal_Linear
;³ID           : CNV
;³Type         : Procedure
;³Last update  : 10.02.1998
;³Action       : Views dbuf, blanks dbuf, blanks zbuf, 32k, LFB
;³Optimized    : -
;³
;³Input variables : SYS_VideoMem
;³
;³Registers changed :
;³
;³Notes :
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

CNV_RNormal_Linear      PROC
		mov	esi, SYS_DBufOffs
		mov	edi, SYS_ScreenShot
		mov	ecx, SYS_DBufSize
		shr	ecx, 2
		rep	movsd

		mov	esi, SYS_DBufOffs
		mov	edi, SYS_VideoMem
		mov	ebp, SYS_ZBufOffs
		fldz

		mov	ecx, SYS_DBufSize
		shr	ecx, 6			;64 bytes each iteration

;---- Copying canvas to LFB, blanking canvas, blanking zbuffer
                xor    edx, edx
Normal_Linear:
		fst	q[ebp +   0 ]
                mov    	eax, [ esi + 00 ]
                mov    	ebx, [ esi + 32 ]
                mov    	[ esi + 00 ], edx
                mov    	[ esi + 32 ], edx
		fst	q[ebp +   8 ]
                mov    	[ edi + 00 ], eax
                mov    	[ edi + 32 ], ebx

		fst	q[ebp +  16 ]
                mov    	eax, [ esi + 04 ]
                mov    	ebx, [ esi + 36 ]
                mov    	[ esi + 04 ], edx
                mov    	[ esi + 36 ], edx
		fst	q[ebp +  24 ]
                mov    	[ edi + 04 ], eax
                mov    	[ edi + 36 ], ebx

		fst	q[ebp +  32 ]
                mov    	eax, [ esi + 08 ]
                mov    	ebx, [ esi + 40 ]
                mov    	[ esi + 08 ], edx
                mov    	[ esi + 40 ], edx
		fst	q[ebp +  40 ]
                mov    	[ edi + 08 ], eax
                mov    	[ edi + 40 ], ebx

		fst	q[ebp +  48 ]
                mov    	eax, [ esi + 12 ]
                mov    	ebx, [ esi + 44 ]
                mov    	[ esi + 12 ], edx
                mov    	[ esi + 44 ], edx
		fst	q[ebp +  56 ]
                mov    	[ edi + 12 ], eax
                mov    	[ edi + 44 ], ebx

		fst	q[ebp +  64 ]
                mov    	eax, [ esi + 16 ]
                mov    	ebx, [ esi + 48 ]
                mov    	[ esi + 16 ], edx
                mov    	[ esi + 48 ], edx
		fst	q[ebp +  72 ]
                mov    	[ edi + 16 ], eax
                mov    	[ edi + 48 ], ebx

		fst	q[ebp +  80 ]
                mov    	eax, [ esi + 20 ]
                mov    	ebx, [ esi + 52 ]
                mov    	[ esi + 20 ], edx
                mov    	[ esi + 52 ], edx
		fst	q[ebp +  88 ]
                mov    	[ edi + 20 ], eax
                mov    	[ edi + 52 ], ebx

		fst	q[ebp +  96 ]
                mov    	eax, [ esi + 24 ]
                mov    	ebx, [ esi + 56 ]
                mov    	[ esi + 24 ], edx
                mov    	[ esi + 56 ], edx
		fst	q[ebp + 104 ]
                mov    	[ edi + 24 ], eax
                mov    	[ edi + 56 ], ebx

		fst	q[ebp + 112 ]
                mov    	eax, [ esi + 28 ]
                mov    	ebx, [ esi + 60 ]
                mov    	[ esi + 28 ], edx
                mov    	[ esi + 60 ], edx
		fst	q[ebp + 120 ]
                mov    	[ edi + 28 ], eax
                mov    	[ edi + 60 ], ebx

		add	ebp, 128
                add    	esi, 64
                add    	edi, 64
                dec    	ecx
                jnz    	Normal_Linear

		ffree	st(0)

		ret
CNV_RNormal_Linear      ENDP

CNV_RStrange_Linear	PROC
		mov	esi, SYS_DBufOffs
		mov	edi, SYS_ScreenShot
		mov	ecx, SYS_DBufSize
		shr	ecx, 2
		rep	movsd

		mov	ebp, SYS_ZBufOffs
		mov	esi, SYS_DBufOffs
		mov	edi, SYS_VideoMem

		fldz

		mov	edx, VBE_YRes

YLoop:
		xor	eax, eax
		mov	ecx, VBE_XRes
		shr	ecx, 1
XLoop:
		dec	ecx

		mov	ebx, [esi + ecx*4]
		mov	[edi + ecx*4], ebx

		mov	[esi + ecx*4], eax
		fst	q[ebp + ecx*8]

		or 	ecx, ecx
		jnz	XLoop

		mov	eax, VBE_BytesPerScanline
		mov	ebx, VBE_XRes
		shl	ebx, 1
		sub	eax, ebx
		add	edi, eax

		mov	eax, VBE_XRes
		shl	eax, 1
		add	esi, eax
		add	edi, eax
		shl	eax, 1
		add	ebp, eax

		dec	edx
		jnz	YLoop



	        ret
CNV_RStrange_Linear	ENDP





CNV_OGrey	DD 0015
		DD -1

CNV_RGrey     PROC
		mov	esi, SYS_DBufOffs
		mov	edi, SYS_VideoMem
		mov	ebp, SYS_ZBufOffs
		fldz

		mov	ecx, VBE_XRes
		imul	ecx, VBE_YRes

;---- Copying canvas to LFB, blanking canvas, blanking zbuffer
		xor	eax, eax
Grey:
		fst	d[ebp +   0 ]
		mov     ax, [esi]
		mov	ebx, 011111b		;ebx = R
		mov     edx, 01111100000b	;edx = G
		and	ebx, eax
		shr	edx, 5
		and	edx, eax
		and	eax, 0111110000000000b	;eax = B
		shr	eax, 10

		add	eax, ebx
		add	eax, edx

		shr	eax, 1
		mov	[edi], al

		mov	WORD PTR [esi], 0

		add	ebp, 4
                add    	esi, 2
		inc	edi
                dec    	ecx
                jnz    	Grey

		ffree	st(0)

		ret
CNV_RGrey     ENDP



CNV_O8bitRGB    DD 0016
		DD -1

CNV_R8bitRGB  PROC
		mov	esi, SYS_DBufOffs
		mov	edi, SYS_VideoMem
		mov	ebp, SYS_ZBufOffs
		fldz

		mov	ecx, SYS_DBufSize

;---- Copying canvas to LFB, blanking canvas, blanking zbuffer
		xor	eax, eax
RGB8:
		fst	d[ebp +   0 ]
		mov     ax, [esi]

		mov	ebx, 011000b		;ebx = B
		and	ebx, eax
		shr	ebx, 3
		test	eax, 0100b


		mov     edx, 01100000000b	;edx = G
		and	edx, eax
		shr	edx, 5

		and	eax, 0110000000000000b	;eax = R
		shr	eax, 9

		or 	eax, ebx
		or 	eax, edx

		mov	[edi], al

		mov	WORD PTR [esi], 0

		add	ebp, 4
                add    	esi, 2
		inc	edi
                dec    	ecx
                jnz    	RGB8

		ffree	st(0)

		ret
CNV_R8bitRGB  ENDP



USlopeInt	DD 0
VSlopeInt	DD 0
UFrac   	DD 0
VFrac   	DD 0

UFracInt	DD 0
VFracInt	DD 0

XRes		DD 320.0
YRes		DD 200.0
XRes2		DD 640
YRes2		DD 400
_65536p0	DD 65536.0

CNV_OStretch	DD 17
		DD -1

CNV_RStretch  PROC
		int 3
		mov	eax, SYS_DBufOffs
		mov	d(StretchDBufA - 4), eax
		mov	d(StretchDBufB - 4), eax
		mov	eax, SYS_VideoMem
		mov	d(StretchVideo - 4), eax

		fld	XRes
		fidiv	XRes2
		fmul	_65536p0
		fistp	USlopeInt

		fld	YRes
		fidiv	YRes2
		fmul	_65536p0
		fistp	VSlopeInt

		mov	eax, USlopeInt
		mov	ebx, USlopeInt
		shr	eax, 16
		shl	ebx, 16
		mov	USlopeInt, eax
		mov	UFrac, ebx

		mov	eax, VSlopeInt
		mov	ebx, VSlopeInt
		shr	eax, 16
		shl	ebx, 16
		mov	VSlopeInt, eax
		mov	VFrac, ebx

		xor	eax, eax		;vint counter
		xor	ebx, ebx		;vfrac counter
		xor	ecx, ecx		;loopcounter
StretchYLoop:
		push	eax
		push	ebx
		push	ecx

		mov	esi, eax		;ucounter
		imul	esi, VBE_XRes

		mov	edi, ecx		;dest
		imul	edi, XRes2

		mov	ecx, XRes2
		shr	ecx, 1

		xor	ebx, ebx		;ufrac counter
		mov	edx, UFrac
		mov	ebp, USlopeInt

StretchXLoop:
		mov	ax, [esi*2 + 12345678h]
StretchDBufA:
		add	ebx, edx
		adc	esi, ebp

		shl	eax, 16
		mov	ax, [esi*2 + 12345678h]
StretchDBufB:


		mov	[edi*2 + 12345678h], eax
StretchVideo:

		add	ebx, edx
		adc	esi, ebp

		add	edi, 2

		dec	ecx
		jnz	StretchXLoop

		pop	ecx
		pop	ebx
		pop	eax

		add	ebx, VFrac
		adc	eax, VSlopeInt

		inc	ecx
		cmp	ecx, YRes2
		jb 	StretchYLoop

		mov	edi, SYS_DBufOffs
		mov	ebp, SYS_ZBufOffs
		mov	ecx, VBE_XRes
		imul	ecx, VBE_YRes

		xor	eax, eax
		fldz
StretchBlank:
		fst	q[ebp]
		mov	[edi     ], eax
		mov	[edi + 32], eax

		fst	q[ebp +  8]
		mov	[edi +  4], eax
		mov	[edi + 36], eax

		fst	q[ebp + 16]
		mov	[edi +  8], eax
		mov	[edi + 40], eax

		fst	q[ebp + 24]
		mov	[edi + 12], eax
		mov	[edi + 44], eax

		fst	q[ebp + 32]
		mov	[edi + 16], eax
		mov	[edi + 48], eax

		fst	q[ebp + 40]
		mov	[edi + 20], eax
		mov	[edi + 52], eax

		fst	q[ebp + 48]
		mov	[edi + 24], eax
		mov	[edi + 56], eax

		fst	q[ebp + 56]
		mov	[edi + 28], eax
		mov	[edi + 60], eax

		add	edi, 64
		add	ebp, 64
		dec	ecx
		jnz	StretchBlank

		ffree	st(0)

		ret
CNV_RStretch  ENDP


END
