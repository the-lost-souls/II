.386
.Model flat, C
.code

public MakeMipmaps_


;ebp = offset of table with 9 mipmap-offsets...
;[ebp + 0] = offset of source-picture...
;eax = xsize of org picture
;edx = ysize of org picture

XSize           DD 0
YSize		DD 0
TableOffs	DD 0

MakeMipmaps_  PROC
		mov	TableOffs, ebp
		mov	XSize, eax
		mov	YSize, edx

;---- Calculating mipmap[1]
		mov	esi, [ebp]
		mov	edi, [ebp + 4]
		mov	ecx, YSize
		xor	eax, eax

MipLoop1Y:
		push	ecx

		mov	ebp, XSize
		shr	ebp, 1
		or	ebp, ebp
		jz	DoneMip1X

MipLoop1X:
		mov	ax, [esi]

		mov	ebx, 011111b
		mov	ecx, 01111100000b
		mov	edx, 0111110000000000b

		and	ebx, eax
		and	ecx, eax
		and	edx, eax

		shr	ecx, 5
		shr	edx, 10

		mov	ax, [esi + 2]
		and	eax, 011111b
		add	ebx, eax

		mov	ax, [esi + 2]
		and	eax, 01111100000b
		shr	eax, 5
		add	ecx, eax

		mov	ax, [esi + 2]
		and	eax, 0111110000000000b
		shr	eax, 10
		add	edx, eax

		shr     ebx, 1
		adc 	ebx, 0

		shr	ecx, 1
		adc 	ecx, 0
		shl	ecx, 5

		shr	edx, 1
		adc 	edx, 0
		shl	edx, 10

		or      ebx, ecx
		or	ebx, edx

		mov	[edi], bx
;		mov	word ptr [edi], 01111100000b

		add	esi, 4
		add	edi, 2

		dec	ebp
		jnz	MipLoop1X

DoneMip1X:

		pop	ecx

		mov	edx, 1
		and	edx, XSize
		shl	edx, 1
		add	esi, edx

		dec	ecx
		jnz	MipLoop1Y


;---- Calculating mipmap[2]
		mov     ebp, TableOffs
		mov	esi, [ebp]
		mov	edi, [ebp + 8]
		mov	ecx, YSize
		xor	eax, eax

MipLoop2Y:
		push	ecx

		mov	ebp, XSize
		shr	ebp, 2
		or	ebp, ebp
		jz	DoneMip2X

MipLoop2X:

						;Getting 1. pixel
		mov	ax, [esi]

		mov	ebx, 011111b
		mov	ecx, 01111100000b
		mov	edx, 0111110000000000b

		and	ebx, eax
		and	ecx, eax
		and	edx, eax

		shr	ecx, 5
		shr	edx, 10

		mov	ax, [esi + 2]		;Getting 2. pixel
		and	eax, 011111b
		add	ebx, eax

		mov	ax, [esi + 2]
		and	eax, 01111100000b
		shr	eax, 5
		add	ecx, eax

		mov	ax, [esi + 2]
		and	eax, 0111110000000000b
		shr	eax, 10
		add	edx, eax

		mov	ax, [esi + 4]		;Getting 3. pixel
		and	eax, 011111b
		add	ebx, eax

		mov	ax, [esi + 4]
		and	eax, 01111100000b
		shr	eax, 5
		add	ecx, eax

		mov	ax, [esi + 4]
		and	eax, 0111110000000000b
		shr	eax, 10
		add	edx, eax

		mov	ax, [esi + 6]		;Getting 4. pixel
		and	eax, 011111b
		add	ebx, eax

		mov	ax, [esi + 6]
		and	eax, 01111100000b
		shr	eax, 5
		add	ecx, eax

		mov	ax, [esi + 6]
		and	eax, 0111110000000000b
		shr	eax, 10
		add	edx, eax

		shr     ebx, 2
		adc 	ebx, 0

		shr	ecx, 2
		adc 	ecx, 0
		shl	ecx, 5

		shr	edx, 2
		adc 	edx, 0
		shl	edx, 10

		or      ebx, ecx
		or	ebx, edx

		mov	[edi], bx
 ;		mov	word ptr [edi], 011111b

		add	esi, 8
		add	edi, 2

		dec	ebp
		jnz	MipLoop2X

DoneMip2X:

		mov	ebp, 011b
		and	ebp, XSize
		shl	ebp, 1
		add	esi, ebp

		pop	ecx
		dec	ecx
		jnz	MipLoop2Y


;---- Calculating mipmap[3]
		mov     ebp, TableOffs
		mov	esi, [ebp]
		mov	edi, [ebp + 12]
		mov	ecx, YSize
		shr	ecx, 1
		xor	eax, eax

MipLoop3Y:
		push	ecx

		mov	ebp, XSize
		or	ebp, ebp
		jz	DoneMip3X

MipLoop3X:
		push	ebp
		mov	ebp, XSize

						;Getting 1. pixel
		mov	ax, [esi]

		mov	ebx, 011111b
		mov	ecx, 01111100000b
		mov	edx, 0111110000000000b

		and	ebx, eax
		and	ecx, eax
		and	edx, eax

		shr	ecx, 5
		shr	edx, 10

		mov	ax, [esi + ebp*2]	;Getting 2. pixel
		and	eax, 011111b
		add	ebx, eax

		mov	ax, [esi + ebp*2]
		and	eax, 01111100000b
		shr	eax, 5
		add	ecx, eax

		mov	ax, [esi + ebp*2]
		and	eax, 0111110000000000b
		shr	eax, 10
		add	edx, eax

		shr     ebx, 1
		adc 	ebx, 0

		shr	ecx, 1
		adc 	ecx, 0
		shl	ecx, 5

		shr	edx, 1
		adc 	edx, 0
		shl	edx, 10

		or      ebx, ecx
		or	ebx, edx

		mov	[edi], bx
;		mov	word ptr [edi], 01111100000b

		add	esi, 2
		add	edi, 2

		pop	ebp
		dec	ebp
		jnz	MipLoop3X

DoneMip3X:

		pop	ecx
		mov	ebp, XSize
		lea	esi, [esi + ebp*2]

		dec	ecx
		jnz	MipLoop3Y


;---- Calculating mipmap[4]
		mov     ebp, TableOffs
		mov	esi, [ebp]
		mov	edi, [ebp + 16]
		mov	ecx, YSize
		shr	ecx, 1
		xor	eax, eax

MipLoop4Y:
		push	ecx

		mov	ebp, XSize
		shr	ebp, 1
		or	ebp, ebp
		jz	DoneMip4X

MipLoop4X:
		push	ebp
		mov	ebp, XSize

						;Getting 1. pixel
		mov	ax, [esi]

		mov	ebx, 011111b
		mov	ecx, 01111100000b
		mov	edx, 0111110000000000b

		and	ebx, eax
		and	ecx, eax
		and	edx, eax

		shr	ecx, 5
		shr	edx, 10

		mov	ax, [esi + 2]		;Getting 2. pixel
		and	eax, 011111b
		add	ebx, eax

		mov	ax, [esi + 2]
		and	eax, 01111100000b
		shr	eax, 5
		add	ecx, eax

		mov	ax, [esi + 2]
		and	eax, 0111110000000000b
		shr	eax, 10
		add	edx, eax

		mov	ax, [esi + ebp*2]	;Getting 3. pixel
		and	eax, 011111b
		add	ebx, eax

		mov	ax, [esi + ebp*2]
		and	eax, 01111100000b
		shr	eax, 5
		add	ecx, eax

		mov	ax, [esi + ebp*2]
		and	eax, 0111110000000000b
		shr	eax, 10
		add	edx, eax

		mov	ax, [esi + ebp*2 + 2]	;Getting 4. pixel
		and	eax, 011111b
		add	ebx, eax

		mov	ax, [esi + ebp*2 + 2]
		and	eax, 01111100000b
		shr	eax, 5
		add	ecx, eax

		mov	ax, [esi + ebp*2 + 2]
		and	eax, 0111110000000000b
		shr	eax, 10
		add	edx, eax

		shr     ebx, 2
		adc 	ebx, 0

		shr	ecx, 2
		adc 	ecx, 0
		shl	ecx, 5

		shr	edx, 2
		adc 	edx, 0
		shl	edx, 10

		or      ebx, ecx
		or	ebx, edx

		mov	[edi], bx
;		mov	word ptr [edi], 011111b

		add	esi, 4
		add	edi, 2

		pop	ebp
		dec	ebp
		jnz	MipLoop4X

DoneMip4X:

		pop	ecx
		mov	ebp, XSize

		mov	edx, 01b
		and	edx, XSize
		shl	edx, 1

		lea	esi, [esi + ebp*2]
		add	esi, edx

		dec	ecx
		jnz	MipLoop4Y


;---- Calculating mipmap[5]
		mov     ebp, TableOffs
		mov	esi, [ebp]
		mov	edi, [ebp + 20]
		mov	ecx, YSize
		shr	ecx, 1
		xor	eax, eax

MipLoop5Y:
		push	ecx

		mov	ebp, XSize
		shr	ebp, 2
		or	ebp, ebp
		jz	DoneMip5X

MipLoop5X:
		push	ebp
		mov	ebp, XSize

						;Getting 1. pixel
		mov	ax, [esi]

		mov	ebx, 011111b
		mov	ecx, 01111100000b
		mov	edx, 0111110000000000b

		and	ebx, eax
		and	ecx, eax
		and	edx, eax

		shr	ecx, 5
		shr	edx, 10

		mov	ax, [esi + 2]		;Getting 2. pixel
		and	eax, 011111b
		add	ebx, eax

		mov	ax, [esi + 2]
		and	eax, 01111100000b
		shr	eax, 5
		add	ecx, eax

		mov	ax, [esi + 2]
		and	eax, 0111110000000000b
		shr	eax, 10
		add	edx, eax

		mov	ax, [esi + 4]		;Getting 3. pixel
		and	eax, 011111b
		add	ebx, eax

		mov	ax, [esi + 4]
		and	eax, 01111100000b
		shr	eax, 5
		add	ecx, eax

		mov	ax, [esi + 4]
		and	eax, 0111110000000000b
		shr	eax, 10
		add	edx, eax
		mov	ax, [esi + 6]		;Getting 4. pixel
		and	eax, 011111b
		add	ebx, eax

		mov	ax, [esi + 6]
		and	eax, 01111100000b
		shr	eax, 5
		add	ecx, eax

		mov	ax, [esi + 6]
		and	eax, 0111110000000000b
		shr	eax, 10
		add	edx, eax

		mov	ax, [esi + ebp*2]	;Getting 5. pixel
		and	eax, 011111b
		add	ebx, eax

		mov	ax, [esi + ebp*2]
		and	eax, 01111100000b
		shr	eax, 5
		add	ecx, eax

		mov	ax, [esi + ebp*2]
		and	eax, 0111110000000000b
		shr	eax, 10
		add	edx, eax

		mov	ax, [esi + ebp*2 + 2]	;Getting 6. pixel
		and	eax, 011111b
		add	ebx, eax

		mov	ax, [esi + ebp*2 + 2]
		and	eax, 01111100000b
		shr	eax, 5
		add	ecx, eax

		mov	ax, [esi + ebp*2 + 2]
		and	eax, 0111110000000000b
		shr	eax, 10
		add	edx, eax

		mov	ax, [esi + ebp*2 + 4]	;Getting 7. pixel
		and	eax, 011111b
		add	ebx, eax

		mov	ax, [esi + ebp*2 + 4]
		and	eax, 01111100000b
		shr	eax, 5
		add	ecx, eax

		mov	ax, [esi + ebp*2 + 4]
		and	eax, 0111110000000000b
		shr	eax, 10
		add	edx, eax

		mov	ax, [esi + ebp*2 + 6]	;Getting 8. pixel
		and	eax, 011111b
		add	ebx, eax

		mov	ax, [esi + ebp*2 + 6]
		and	eax, 01111100000b
		shr	eax, 5
		add	ecx, eax

		mov	ax, [esi + ebp*2 + 6]
		and	eax, 0111110000000000b
		shr	eax, 10
		add	edx, eax


		shr     ebx, 3
		adc 	ebx, 0

		shr	ecx, 3
		adc 	ecx, 0
		shl	ecx, 5

		shr	edx, 3
		adc 	edx, 0
		shl	edx, 10

		or      ebx, ecx
		or	ebx, edx

		mov	[edi], bx
;		mov	word ptr [edi], 011111b

		add	esi, 8
		add	edi, 2

		pop	ebp
		dec	ebp
		jnz	MipLoop5X

DoneMip5X:

		pop	ecx
		mov	ebp, XSize

		mov	edx, 011b
		and	edx, XSize
		shl	edx, 1

		lea	esi, [esi + ebp*2]
		add	esi, edx

		dec	ecx
		jnz	MipLoop5Y


;---- Calculating mipmap[6]
		mov     ebp, TableOffs
		mov	esi, [ebp]
		mov	edi, [ebp + 24]
		xor	eax, eax

		mov	ecx, YSize
		shr	ecx, 2
		or	ecx, ecx
		jz	DoneMip6

MipLoop6Y:
		push	ecx
		mov	ebp, XSize

MipLoop6X:
		push	ebp
						;Getting 1. pixel
		mov	ax, [esi]

		mov	ebx, 011111b
		mov	ecx, 01111100000b
		mov	edx, 0111110000000000b

		and	ebx, eax
		and	ecx, eax
		and	edx, eax

		shr	ecx, 5
		shr	edx, 10

		mov	ebp, XSize
		mov	ax, [esi + ebp*2]	;Getting 2. pixel
		and	eax, 011111b
		add	ebx, eax

		mov	ax, [esi + ebp*2]
		and	eax, 01111100000b
		shr	eax, 5
		add	ecx, eax

		mov	ax, [esi + ebp*2]
		and	eax, 0111110000000000b
		shr	eax, 10
		add	edx, eax

		add	ebp, XSize
		mov	ax, [esi + ebp*2]	;Getting 3. pixel
		and	eax, 011111b
		add	ebx, eax

		mov	ax, [esi + ebp*2]
		and	eax, 01111100000b
		shr	eax, 5
		add	ecx, eax

		mov	ax, [esi + ebp*2]
		and	eax, 0111110000000000b
		shr	eax, 10
		add	edx, eax

		add	ebp, XSize
		mov	ax, [esi + ebp*2]	;Getting 4. pixel
		and	eax, 011111b
		add	ebx, eax

		mov	ax, [esi + ebp*2]
		and	eax, 01111100000b
		shr	eax, 5
		add	ecx, eax

		mov	ax, [esi + ebp*2]
		and	eax, 0111110000000000b
		shr	eax, 10
		add	edx, eax

		shr     ebx, 2
		adc 	ebx, 0

		shr	ecx, 2
		adc 	ecx, 0
		shl	ecx, 5

		shr	edx, 2
		adc 	edx, 0
		shl	edx, 10

		or      ebx, ecx
		or	ebx, edx

		mov	[edi], bx
;		mov	word ptr [edi], 011111b

		add	esi, 2
		add	edi, 2

		pop	ebp
		dec	ebp
		jnz	MipLoop6X

		pop	ecx

		mov	ebp, XSize
		shl	ebp, 1
		lea	ebp, [ebp*2 + ebp]
		add	esi, ebp

		dec	ecx
		jnz	MipLoop6Y
DoneMip6:


;---- Calculating mipmap[7]
		mov     ebp, TableOffs
		mov	esi, [ebp]
		mov	edi, [ebp + 28]
		xor	eax, eax

		mov	ecx, YSize
		shr	ecx, 2
		or	ecx, ecx
		jz	DoneMip7

MipLoop7Y:
		push	ecx
		mov	ebp, XSize
		shr	ebp, 1
		or	ebp, ebp
		jz	DoneMip7X

MipLoop7X:
		push	ebp
						;Getting 1. pixel
		mov	ax, [esi]

		mov	ebx, 011111b
		mov	ecx, 01111100000b
		mov	edx, 0111110000000000b

		and	ebx, eax
		and	ecx, eax
		and	edx, eax

		shr	ecx, 5
		shr	edx, 10

		mov	ax, [esi + 2]		;Getting 2. pixel
		and	eax, 011111b
		add	ebx, eax

		mov	ax, [esi + 2]
		and	eax, 01111100000b
		shr	eax, 5
		add	ecx, eax

		mov	ax, [esi + 2]
		and	eax, 0111110000000000b
		shr	eax, 10
		add	edx, eax




		mov	ebp, XSize
		mov	ax, [esi + ebp*2]	;Getting 3. pixel
		and	eax, 011111b
		add	ebx, eax

		mov	ax, [esi + ebp*2]
		and	eax, 01111100000b
		shr	eax, 5
		add	ecx, eax

		mov	ax, [esi + ebp*2]
		and	eax, 0111110000000000b
		shr	eax, 10
		add	edx, eax

		mov	ax, [esi + ebp*2 + 2]	;Getting 4. pixel
		and	eax, 011111b
		add	ebx, eax

		mov	ax, [esi + ebp*2 + 2]
		and	eax, 01111100000b
		shr	eax, 5
		add	ecx, eax

		mov	ax, [esi + ebp*2 + 2]
		and	eax, 0111110000000000b
		shr	eax, 10
		add	edx, eax




		add	ebp, XSize
		mov	ax, [esi + ebp*2]	;Getting 5. pixel
		and	eax, 011111b
		add	ebx, eax

		mov	ax, [esi + ebp*2]
		and	eax, 01111100000b
		shr	eax, 5
		add	ecx, eax

		mov	ax, [esi + ebp*2]
		and	eax, 0111110000000000b
		shr	eax, 10
		add	edx, eax

		mov	ax, [esi + ebp*2 + 2]	;Getting 6. pixel
		and	eax, 011111b
		add	ebx, eax

		mov	ax, [esi + ebp*2 + 2]
		and	eax, 01111100000b
		shr	eax, 5
		add	ecx, eax

		mov	ax, [esi + ebp*2 + 2]
		and	eax, 0111110000000000b
		shr	eax, 10
		add	edx, eax




		add	ebp, XSize
		mov	ax, [esi + ebp*2]	;Getting 7. pixel
		and	eax, 011111b
		add	ebx, eax

		mov	ax, [esi + ebp*2]
		and	eax, 01111100000b
		shr	eax, 5
		add	ecx, eax

		mov	ax, [esi + ebp*2]
		and	eax, 0111110000000000b
		shr	eax, 10
		add	edx, eax

		mov	ax, [esi + ebp*2 + 2]	;Getting 8. pixel
		and	eax, 011111b
		add	ebx, eax

		mov	ax, [esi + ebp*2 + 2]
		and	eax, 01111100000b
		shr	eax, 5
		add	ecx, eax

		mov	ax, [esi + ebp*2 + 2]
		and	eax, 0111110000000000b
		shr	eax, 10
		add	edx, eax


		shr     ebx, 3
		adc 	ebx, 0

		shr	ecx, 3
		adc 	ecx, 0
		shl	ecx, 5

		shr	edx, 3
		adc 	edx, 0
		shl	edx, 10

		or      ebx, ecx
		or	ebx, edx

		mov	[edi], bx
;		mov	word ptr [edi], 011111b

		add	esi, 4
		add	edi, 2

		pop	ebp
		dec	ebp
		jnz	MipLoop7X

DoneMip7X:

		pop	ecx

		mov	ebp, XSize
		shl	ebp, 1
		lea	ebp, [ebp*2 + ebp]
		add	esi, ebp

		mov	ebp, XSize
		and	ebp, 1
		shl	ebp, 1
		add	esi, ebp

		dec	ecx
		jnz	MipLoop7Y
DoneMip7:




;---- Calculating mipmap[8]
		mov     ebp, TableOffs
		mov	esi, [ebp]
		mov	edi, [ebp + 32]
		xor	eax, eax

		mov	ecx, YSize
		shr	ecx, 2
		or	ecx, ecx
		jz	DoneMip8

MipLoop8Y:
		push	ecx
		mov	ebp, XSize
		shr	ebp, 2
		or	ebp, ebp
		jz	DoneMip8X

MipLoop8X:
		push	ebp
						;Getting 1. pixel
		mov	ax, [esi]

		mov	ebx, 011111b
		mov	ecx, 01111100000b
		mov	edx, 0111110000000000b

		and	ebx, eax
		and	ecx, eax
		and	edx, eax

		shr	ecx, 5
		shr	edx, 10

		mov	ax, [esi + 2]		;Getting 2. pixel
		and	eax, 011111b
		add	ebx, eax

		mov	ax, [esi + 2]
		and	eax, 01111100000b
		shr	eax, 5
		add	ecx, eax

		mov	ax, [esi + 2]
		and	eax, 0111110000000000b
		shr	eax, 10
		add	edx, eax

		mov	ax, [esi + 4]		;Getting 3. pixel
		and	eax, 011111b
		add	ebx, eax

		mov	ax, [esi + 4]
		and	eax, 01111100000b
		shr	eax, 5
		add	ecx, eax

		mov	ax, [esi + 4]
		and	eax, 0111110000000000b
		shr	eax, 10
		add	edx, eax

		mov	ax, [esi + 6]		;Getting 4. pixel
		and	eax, 011111b
		add	ebx, eax

		mov	ax, [esi + 6]
		and	eax, 01111100000b
		shr	eax, 5
		add	ecx, eax

		mov	ax, [esi + 6]
		and	eax, 0111110000000000b
		shr	eax, 10
		add	edx, eax




		mov	ebp, XSize
		mov	ax, [esi + ebp*2]	;Getting 5. pixel
		and	eax, 011111b
		add	ebx, eax

		mov	ax, [esi + ebp*2]
		and	eax, 01111100000b
		shr	eax, 5
		add	ecx, eax

		mov	ax, [esi + ebp*2]
		and	eax, 0111110000000000b
		shr	eax, 10
		add	edx, eax

		mov	ax, [esi + ebp*2 + 2]	;Getting 6. pixel
		and	eax, 011111b
		add	ebx, eax

		mov	ax, [esi + ebp*2 + 2]
		and	eax, 01111100000b
		shr	eax, 5
		add	ecx, eax

		mov	ax, [esi + ebp*2 + 2]
		and	eax, 0111110000000000b
		shr	eax, 10
		add	edx, eax

		mov	ax, [esi + ebp*2 + 4]	;Getting 7. pixel
		and	eax, 011111b
		add	ebx, eax

		mov	ax, [esi + ebp*2 + 4]
		and	eax, 01111100000b
		shr	eax, 5
		add	ecx, eax

		mov	ax, [esi + ebp*2 + 4]
		and	eax, 0111110000000000b
		shr	eax, 10
		add	edx, eax

		mov	ax, [esi + ebp*2 + 6]	;Getting 8. pixel
		and	eax, 011111b
		add	ebx, eax

		mov	ax, [esi + ebp*2 + 6]
		and	eax, 01111100000b
		shr	eax, 5
		add	ecx, eax

		mov	ax, [esi + ebp*2 + 6]
		and	eax, 0111110000000000b
		shr	eax, 10
		add	edx, eax




		add	ebp, XSize
		mov	ax, [esi + ebp*2]	;Getting 9. pixel
		and	eax, 011111b
		add	ebx, eax

		mov	ax, [esi + ebp*2]
		and	eax, 01111100000b
		shr	eax, 5
		add	ecx, eax

		mov	ax, [esi + ebp*2]
		and	eax, 0111110000000000b
		shr	eax, 10
		add	edx, eax

		mov	ax, [esi + ebp*2 + 2]	;Getting 10. pixel
		and	eax, 011111b
		add	ebx, eax

		mov	ax, [esi + ebp*2 + 2]
		and	eax, 01111100000b
		shr	eax, 5
		add	ecx, eax

		mov	ax, [esi + ebp*2 + 2]
		and	eax, 0111110000000000b
		shr	eax, 10
		add	edx, eax

		mov	ax, [esi + ebp*2 + 4]	;Getting 11. pixel
		and	eax, 011111b
		add	ebx, eax

		mov	ax, [esi + ebp*2 + 4]
		and	eax, 01111100000b
		shr	eax, 5
		add	ecx, eax

		mov	ax, [esi + ebp*2 + 4]
		and	eax, 0111110000000000b
		shr	eax, 10
		add	edx, eax

		mov	ax, [esi + ebp*2 + 6]	;Getting 12. pixel
		and	eax, 011111b
		add	ebx, eax

		mov	ax, [esi + ebp*2 + 6]
		and	eax, 01111100000b
		shr	eax, 5
		add	ecx, eax

		mov	ax, [esi + ebp*2 + 6]
		and	eax, 0111110000000000b
		shr	eax, 10
		add	edx, eax




		add	ebp, XSize
		mov	ax, [esi + ebp*2]	;Getting 13. pixel
		and	eax, 011111b
		add	ebx, eax

		mov	ax, [esi + ebp*2]
		and	eax, 01111100000b
		shr	eax, 5
		add	ecx, eax

		mov	ax, [esi + ebp*2]
		and	eax, 0111110000000000b
		shr	eax, 10
		add	edx, eax

		mov	ax, [esi + ebp*2 + 2]	;Getting 14. pixel
		and	eax, 011111b
		add	ebx, eax

		mov	ax, [esi + ebp*2 + 2]
		and	eax, 01111100000b
		shr	eax, 5
		add	ecx, eax

		mov	ax, [esi + ebp*2 + 2]
		and	eax, 0111110000000000b
		shr	eax, 10
		add	edx, eax

		mov	ax, [esi + ebp*2 + 4]	;Getting 15. pixel
		and	eax, 011111b
		add	ebx, eax

		mov	ax, [esi + ebp*2 + 4]
		and	eax, 01111100000b
		shr	eax, 5
		add	ecx, eax

		mov	ax, [esi + ebp*2 + 4]
		and	eax, 0111110000000000b
		shr	eax, 10
		add	edx, eax

		mov	ax, [esi + ebp*2 + 6]	;Getting 16. pixel
		and	eax, 011111b
		add	ebx, eax

		mov	ax, [esi + ebp*2 + 6]
		and	eax, 01111100000b
		shr	eax, 5
		add	ecx, eax

		mov	ax, [esi + ebp*2 + 6]
		and	eax, 0111110000000000b
		shr	eax, 10
		add	edx, eax


		shr     ebx, 4
		adc 	ebx, 0

		shr	ecx, 4
		adc 	ecx, 0
		shl	ecx, 5

		shr	edx, 4
		adc 	edx, 0
		shl	edx, 10

		or      ebx, ecx
		or	ebx, edx

		mov	[edi], bx
;		mov	word ptr [edi], 011111b

		add	esi, 8
		add	edi, 2

		pop	ebp
		dec	ebp
		jnz	MipLoop8X

DoneMip8X:

		pop	ecx

		mov	ebp, XSize
		shl	ebp, 1
		lea	ebp, [ebp*2 + ebp]
		add	esi, ebp

		mov	ebp, XSize
		and	ebp, 11b
		shl	ebp, 1
		add	esi, ebp

		dec	ecx
		jnz	MipLoop8Y
DoneMip8:

		ret
MakeMipmaps_  ENDP



END

