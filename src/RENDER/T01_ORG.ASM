;旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴 ASSEMBLER SOURCE 컴컴컴컴컴컴컴컴컴컴컴컴컴컴
;쿙ame         : T01_ORG.ASM
;쿌uthor       : Cyberfish/TLS & Toe/DU
;쿗ast update  : 26.07.97
;쿌ction       : Draws a perspective mapped triangle. No z-buffer or shading.
;�
;쿙otes :
;쿟o be done:
;� -fix the last pixel, if scanlength=17
;읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
.386
.Model Flat, C
.code

include w:\general\var.inc
include w:\general\general.inc
include w:\vbe\vbe.inc

public T01_Tri_
public T01_MapModifier_
public T01_MapOffset
public T01_MapWidth


Edge          STRUC

X               DD ?
UdZ             DD ?
VdZ             DD ?
NdZ             DD ?

Edge          ENDS


;컴컴 In-variables...
T01_MapModifier DD 1.0                          ;FP: Modifier, mappingcoordinates
T01_MapOffset   DD 0                            ;Offset texturemap
T01_MapWidth    DD 320                          ;INT: Tmap width, in bytes





;컴컴 Locally used "constants"
Fixed16_16      DD 65536.0
Fixed16         DD 4096.0                       ;65536.0/ 16.0
Sixteen         DD 16.0
One             DD 1.0
AlmostZero      DD 0.000000000000000000001

FPDump          DD 0



;旼컴컴컴컴컴컴컴컴컴컴컴컴 ASSEMBLER ROUTINE 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
;쿙ame         : T01_Scanline
;쿔D           : T01
;쿟ype         : Macro
;쿗ast update  : 26.07.1997
;쿌ction       : Draws a perspectivemapped, horisontal scanline
;쿚ptimized    : No
;�
;쿔nput variables : [eax]  = left edge
;�                  [ebx]  = right edge
;�
;쿙otes : FPU stack must be empty. All integer registers are saved.
;읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴

ScanLength      DD 0                            ;int scanline length
Slope_1         DD 0                            ;int(USlope) + (int(VSlope)+1)*MapSize
Slope           DD 0                            ;int(USlope) + int(VSlope)*MapSize
UFrac           DD 0                            ;int U fraction (0.32)
VFrac           DD 0                            ;int V fraction (0.32)
USlopeFixed     DD 0                            ;
VSlopeFixed     DD 0
ULeftFixed      DD 0                            ;int U 16.16 fixed point start left
VLeftFixed      DD 0                            ;int V 16.16 fixed point start left

EdgeRight     Edge ?                            ;Right edge data
EdgeLeft      Edge ?                            ;Left edge data



T01_ScanLine  MACRO
                                     ;st0  st1  st2  st3  st4  st5  st6  st7

                fldcw  FPU_RoundUp

;---- Clip U/Z, V/Z and 1/Z left to next pixel...
                fld    [eax].VdZ     ;V/ZL
                fld    [eax].UdZ     ;U/ZL V/ZL
                fld    [eax].NdZ     ;1/ZL U/ZL V/ZL
                fld    [eax].X       ;XL   1/ZL U/ZL V/ZL
                fistp  EdgeLeft.X    ;1/ZL U/ZL V/ZL
                fild   EdgeLeft.X    ;XI   1/ZL U/ZL V/ZL
                fsub   [eax].X       ;XD   1/ZL U/ZL V/ZL
                fld    st            ;XD   XD   1/ZL U/ZL V/ZL
                fld    st            ;XD   XD   XD   1/ZL U/ZL V/ZL
                fmul   VZSlope       ;XV   XD   XD   1/ZL U/ZL V/ZL
                fxch   st(2)         ;XD   XD   XV   1/ZL U/ZL V/ZL
                fmul   UZSlope       ;XU   XD   XV   1/ZL U/ZL V/ZL
                fxch   st(1)         ;XD   XU   XV   1/ZL U/ZL V/ZL
                fmul   NZSlope       ;XZ   XU   XV   1/ZL U/ZL V/ZL
                faddp  st(3), st     ;XU   XV   1/ZL U/ZL V/ZL
                faddp  st(3), st     ;XV   1/ZL U/ZL V/ZL
                faddp  st(3), st     ;1/ZL U/ZL V/ZL


                fldcw  FPU_RoundDown

;---- Clip U/Z, V/Z and 1/Z right to last pixel...
                fld    [ebx].X       ;XR
                fistp  EdgeRight.X   ;
                fild   EdgeRight.X   ;XRI
                fsub   [ebx].X       ;XD

                fld    st            ;XD   XD
                fmul   UZSlope       ;XDU  XD
                fadd   [ebx].UdZ     ;UZR  XD
                fstp   EdgeRight.UdZ ;XD

                fld    st            ;XD   XD
                fmul   VZSlope       ;XDU  XD
                fadd   [ebx].VdZ     ;UZR  XD
                fstp   EdgeRight.VdZ ;XD

                fmul   NZSlope       ;XDU
                fadd   [ebx].NdZ     ;UZR
                fstp   EdgeRight.NdZ ;




;---- Get Z left...
                fld1                 ;1    1/ZL U/ZL V/ZL
                fdiv   st, st(1)     ;ZL   1/ZL U/ZL V/ZL


;---- !!!INTEGER INSTRUCTIONS HERE!!!
;---- !!!INTEGER INSTRUCTIONS HERE!!!
;---- !!!INTEGER INSTRUCTIONS HERE!!!


                pushad                          ;Save all registers

                mov    edi, MemPointer          ;Use edi as mempointer
                add    edi, EdgeLeft.X

                mov    eax, EdgeLeft.X          ;Calculate scanline length
                mov    ebx, EdgeRight.X         ;/
                sub    ebx, eax                 ;ebx = scanline length
                mov    ScanLength, ebx

                cmp    ebx, 0
                jl     Scan_Done

                cmp    ebx, 16                  ;Run leftoverloop now?
                jbe    Leftover1



;---- Get U and V left (the fdiv is finished)
                fld    st            ;ZL   ZL   1/ZL U/ZL V/ZL
                fmul   st, st(4)     ;VL   ZL   1/ZL U/ZL V/ZL
                fxch   st(1)         ;ZL   VL   1/ZL U/ZL V/ZL
                fmul   st, st(3)     ;UL   VL   1/ZL U/ZL V/ZL


;---- Get U/Z and V/Z (right)
                fxch   st(2)         ;1/ZL VL   UL   U/ZL V/ZL
                fadd   NZSlope16     ;1/ZR VL   UL   U/ZL V/ZL
                fxch   st(3)         ;U/ZL VL   UL   1/ZR V/ZL
                fadd   UZSlope16     ;U/ZR VL   UL   1/ZR V/ZL
                fxch   st(4)         ;V/ZL VL   UL   1/ZR U/ZR
                fadd   VZSlope16     ;V/ZR VL   UL   1/ZR U/ZR
                fstp   st(5)         ;VL   UL   1/ZR U/ZR V/ZR


;---- Get Z-right
                fld1                 ;1    VL   UL   1/ZR U/ZR V/ZR
                fdiv   st, st(3)     ;ZR   VL   UL   1/ZR U/ZR V/ZR





;------------------------- LOOP STARTS HERE --------------------------------

Loop16:
;---- Get U- and V-right
                fld    st            ;ZR   ZR   VL   UL   1/ZR U/ZR V/ZR
                fmul   st, st(5)     ;UR   ZR   VL   UL   1/ZR U/ZR V/ZR
                fxch   st(1)         ;ZR   UR   VL   UL   1/ZR U/ZR V/ZR
                fmul   st, st(6)     ;VR   UR   VL   UL   1/ZR U/ZR V/ZR


;---- Calculating slopes...
                fld    st            ;VR   VR   UR   VL   UL   1/ZR U/ZR V/ZR
                fsub   st, st(3)     ;VD   VR   UR   VL   UL   1/ZR U/ZR V/ZR
                fmul   Fixed16       ;VD   VR   UR   VL   UL   1/ZR U/ZR V/ZR
                fistp  VSlopeFixed   ;VR   UR   VL   UL   1/ZR U/ZR V/ZR
                fxch   st(1)         ;UR   VR   VL   UL   1/ZR U/ZR V/ZR
                fld    st            ;UR   UR   VR   VL   UL   1/ZR U/ZR V/ZR
                fsub   st, st(4)     ;UD   UR   VR   VL   UL   1/ZR U/ZR V/ZR
                fmul   Fixed16       ;UD   UR   VR   VL   UL   1/ZR U/ZR V/ZR
                fistp  USlopeFixed   ;UR   VR   VL   UL   1/ZR U/ZR V/ZR


;---- Convert U and V left...
                fxch   st(2)         ;VL   VR   UR   UL   1/ZR U/ZR V/ZR
                fmul   Fixed16_16    ;VL   VR   UR   UL   1/ZR U/ZR V/ZR
                fistp  VLeftFixed    ;VR   UR   UL   1/ZR U/ZR V/ZR
                fxch   st(2)         ;UL   UR   VR   1/ZR U/ZR V/ZR
                fmul   Fixed16_16    ;UL   UR   VR   1/ZR U/ZR V/ZR
                fistp  ULeftFixed    ;UR   VR   1/ZR U/ZR V/ZR


;---- Get U/Z and V/Z (right)
                fxch   st(2)         ;1/ZL VL   UL   U/ZL V/ZL
                fadd   NZSlope16     ;1/ZR VL   UL   U/ZL V/ZL
                fxch   st(3)         ;U/ZL VL   UL   1/ZR V/ZL
                fadd   UZSlope16     ;U/ZR VL   UL   1/ZR V/ZL
                fxch   st(4)         ;V/ZL VL   UL   1/ZR U/ZR
                fadd   VZSlope16     ;V/ZR VL   UL   1/ZR U/ZR
                fstp   st(5)         ;VL   UL   1/ZR U/ZR V/ZR


;---- Check if we need Z for end of line next time...
                cmp    ScanLength, 32
                jg     NotEOL
                ffree  st(4)         ;VL   UL   1/ZR U/ZR
                ffree  st(3)         ;VL   UL   1/ZR
                ffree  st(2)         ;VL   UL

                fld    EdgeRight.VdZ ;V/ZR VL   UL
                fld    EdgeRight.UdZ ;U/ZR V/ZR VL   UL
                fld    EdgeRight.NdZ ;1/ZR U/ZR V/ZR VL   UL

                fstp   st(5)         ;U/ZR V/ZR VL   UL   NZR
                fstp   st(5)         ;V/ZR VL   UL   1/ZR U/ZR
                fstp   st(5)         ;VL   UL   1/ZR U/ZR V/ZR
NotEOL:




;---- Setting up slopes...
                mov    eax, USlopeFixed
                mov    ebx, eax                 ;Take copy
                sar    eax, 16                  ;Get int part
                sal    ebx, 16                  ;Get fractional part
                mov    UFrac, ebx               ;Save fractional part

                mov    ecx, VSlopeFixed
                mov    edx, ecx
                sar    ecx, 16
                sal    edx, 16
                mov    VFrac, edx

                imul   ecx, T01_MapWidth
                add    ecx, eax
                mov    Slope, ecx               ;Save normal slope
                add    ecx, T01_MapWidth
                mov    [Slope - 4], ecx         ;Save slope + 1 scanline




;---- Setting up starting values...
                mov    eax, ULeftFixed          ;
                add    eax, 8000h               ;add 0.5 (16.16 fixed point)
                mov    ebx, eax                 ;Take copy
                shr    eax, 16                  ;Get int part
                shl    ebx, 16                  ;Get fractional part

                mov    esi, VLeftFixed
                add    esi, 8000h
                mov    ecx, esi
                shr    esi, 16
                shl    ecx, 16
                imul   esi, T01_MapWidth

                add    esi, eax                 ;Add U-offset
                add    esi, T01_MapOffset       ;Add textureoffset

                                                ;esi = mempointer
                                                ;ebx = U fractional part
                                                ;ecx = V fractional part



;---- Get next Z...
                fld1                 ;1    VL   UL   1/ZR U/ZR V/ZR
                fdiv   st, st(3)     ;ZR   VL   UL   1/ZR U/ZR V/ZR





;---- !!!INTEGER INSTRUCTIONS!!!
;---- !!!INTEGER INSTRUCTIONS!!!
;---- !!!INTEGER INSTRUCTIONS!!!


;---- Pixel 0

                mov    al, [esi]
                mov    [edi + 0], al



;---- Pixel 1
                add    ecx, VFrac               ;cf set if ecx > 0.5
                sbb    ebp, ebp                 ;-1 if carry

                add    ebx, UFrac               ;cf set if ebx > 0.5
                adc    esi, [Slope + ebp*4]    ;increment texturepointer

                mov    al, [esi]
                mov    [edi + 1], al

;---- Pixel 2
                add    ecx, VFrac               ;cf set if ecx > 0.5
                sbb    ebp, ebp                 ;-1 if carry

                add    ebx, UFrac               ;cf set if ebx > 0.5
                adc    esi, [Slope + ebp*4]    ;increment texturepointer

                mov    al, [esi]
                mov    [edi + 2], al

;---- Pixel 3
                add    ecx, VFrac               ;cf set if ecx > 0.5
                sbb    ebp, ebp                 ;-1 if carry

                add    ebx, UFrac               ;cf set if ebx > 0.5
                adc    esi, [Slope + ebp*4]    ;increment texturepointer

                mov    al, [esi]
                mov    [edi + 3], al



;---- Pixel 4
                add    ecx, VFrac               ;cf set if ecx > 0.5
                sbb    ebp, ebp                 ;-1 if carry

                add    ebx, UFrac               ;cf set if ebx > 0.5
                adc    esi, [Slope + ebp*4]    ;increment texturepointer

                mov    al, [esi]
                mov    [edi + 4], al

;---- Pixel 5
                add    ecx, VFrac               ;cf set if ecx > 0.5
                sbb    ebp, ebp                 ;-1 if carry

                add    ebx, UFrac               ;cf set if ebx > 0.5
                adc    esi, [Slope + ebp*4]    ;increment texturepointer

                mov    al, [esi]
                mov    [edi + 5], al

;---- Pixel 6

                add    ecx, VFrac               ;cf set if ecx > 0.5
                sbb    ebp, ebp                 ;-1 if carry

                add    ebx, UFrac               ;cf set if ebx > 0.5
                adc    esi, [Slope + ebp*4]    ;increment texturepointer

                mov    al, [esi]
                mov    [edi + 6], al

;---- Pixel 7
                add    ecx, VFrac               ;cf set if ecx > 0.5
                sbb    ebp, ebp                 ;-1 if carry

                add    ebx, UFrac               ;cf set if ebx > 0.5
                adc    esi, [Slope + ebp*4]    ;increment texturepointer

                mov    al, [esi]
                mov    [edi + 7], al

;---- Pixel 8
                add    ecx, VFrac               ;cf set if ecx > 0.5
                sbb    ebp, ebp                 ;-1 if carry

                add    ebx, UFrac               ;cf set if ebx > 0.5
                adc    esi, [Slope + ebp*4]    ;increment texturepointer

                mov    al, [esi]
                mov    [edi + 8], al

;---- Pixel 9
                add    ecx, VFrac               ;cf set if ecx > 0.5
                sbb    ebp, ebp                 ;-1 if carry

                add    ebx, UFrac               ;cf set if ebx > 0.5
                adc    esi, [Slope + ebp*4]    ;increment texturepointer

                mov    al, [esi]
                mov    [edi + 9], al

;---- Pixel 10
                add    ecx, VFrac               ;cf set if ecx > 0.5
                sbb    ebp, ebp                 ;-1 if carry

                add    ebx, UFrac               ;cf set if ebx > 0.5
                adc    esi, [Slope + ebp*4]    ;increment texturepointer

                mov    al, [esi]
                mov    [edi + 10], al

;---- Pixel 11
                add    ecx, VFrac               ;cf set if ecx > 0.5
                sbb    ebp, ebp                 ;-1 if carry

                add    ebx, UFrac               ;cf set if ebx > 0.5
                adc    esi, [Slope + ebp*4]    ;increment texturepointer

                mov    al, [esi]
                mov    [edi + 11], al

;---- Pixel 12
                add    ecx, VFrac               ;cf set if ecx > 0.5
                sbb    ebp, ebp                 ;-1 if carry

                add    ebx, UFrac               ;cf set if ebx > 0.5
                adc    esi, [Slope + ebp*4]    ;increment texturepointer

                mov    al, [esi]
                mov    [edi + 12], al

;---- Pixel 13
                add    ecx, VFrac               ;cf set if ecx > 0.5
                sbb    ebp, ebp                 ;-1 if carry

                add    ebx, UFrac               ;cf set if ebx > 0.5
                adc    esi, [Slope + ebp*4]    ;increment texturepointer

                mov    al, [esi]
                mov    [edi + 13], al

;---- Pixel 14
                add    ecx, VFrac               ;cf set if ecx > 0.5
                sbb    ebp, ebp                 ;-1 if carry

                add    ebx, UFrac               ;cf set if ebx > 0.5
                adc    esi, [Slope + ebp*4]    ;increment texturepointer

                mov    al, [esi]
                mov    [edi + 14], al

;---- Pixel 15
                add    ecx, VFrac               ;cf set if ecx > 0.5
                sbb    ebp, ebp                 ;-1 if carry

                add    ebx, UFrac               ;cf set if ebx > 0.5
                adc    esi, [Slope + ebp*4]    ;increment texturepointer

                mov    al, [esi]
                mov    [edi + 15], al


                add    edi, 16                  ;Move memory pointer
                sub    ScanLength, 16           ;Decrease counter
                cmp    ScanLength, 16           ;Finished?
                jbe    LeftOver2
                jmp    Loop16



;---- Write leftover pixels...

LeftOver1:

;---- Get U and V left (the fdiv is finished)
                fld    st            ;ZL   ZL   ---- U/ZL V/ZL
                fmul   st, st(3)     ;UL   ZL   ---- U/ZL V/ZL
                fxch   st(1)         ;ZL   UL   ---- U/ZL V/ZL
                fmul   st, st(4)     ;VL   UL   ---- U/ZL V/ZL

                ffree  st(4)         ;VL   UL   ---- U/ZL
                ffree  st(3)         ;VL   UL   ----


                fld    EdgeRight.VdZ ;V/ZR VL   UL   ----
                fld    EdgeRight.UdZ ;U/ZR V/ZR VL   UL   ----

                fstp   st(5)         ;V/ZR VL   UL   ---- U/ZR
                fstp   st(5)         ;VL   UL   ---- U/ZR V/ZR

                fld    EdgeRight.NdZ ;1/ZR VL   UL   ---- U/ZR V/ZR
                fdivr  One           ;ZR   VL   UL   ---- U/ZR V/ZR


LeftOver2:

                                     ;ZR   VL   UL   ---- U/ZR V/ZR

;---- Get U and V at right end of line...
                fld    st            ;ZR   ZR   VL   UL   ---- U/ZR V/ZR
                fmul   st, st(5)     ;UR   ZR   VL   UL   ---- U/ZR V/ZR
                fxch   st(1)         ;ZR   UR   VL   UL   ---- U/ZR V/ZR
                fmul   st, st(6)     ;VR   UR   VL   UL   ---- U/ZR V/ZR


;---- Get deltas and slopes...
                fsub   st, st(2)     ;VD   UR   VL   UL   ---- U/ZR V/ZR
                fxch   st(1)         ;UR   VD   VL   UL   ---- U/ZR V/ZR
                fsub   st, st(3)     ;UD   VD   VL   UL   ---- U/ZR V/ZR
                fild   ScanLength    ;XD   UD   VD   VL   UL   ---- U/ZR V/ZR
                fdivr  Fixed16_16    ;FP/L UD   VD   VL   UL   ---- U/ZR V/ZR
                fmul   st(1), st     ;FP/L US   VD   VL   UL   ---- U/ZR V/ZR
                fmulp  st(2), st     ;US   VS   VL   UL   ---- U/ZR V/ZR
                fistp  USlopeFixed   ;VS   VL   UL   ---- U/ZR V/ZR
                fistp  VSlopeFixed   ;VL   UL   ---- U/ZR V/ZR


;---- Convert U and V left...
                fmul   Fixed16_16    ;VL   UL   ---- U/ZR V/ZR
                fistp  VLeftFixed    ;UL   ---- U/ZR V/ZR
                fmul   Fixed16_16    ;UL   ---- U/ZR V/ZR
                fistp  ULeftFixed    ;---- U/ZR V/ZR



;---- Setting up slopes...
                mov    eax, USlopeFixed
                mov    ebx, eax                 ;Take copy
                sar    eax, 16                  ;Get int part
                sal    ebx, 16                  ;Get fractional part
                mov    UFrac, ebx               ;Save fractional part

                mov    ecx, VSlopeFixed
                mov    edx, ecx
                sar    ecx, 16
                sal    edx, 16
                mov    VFrac, edx

                imul   ecx, T01_MapWidth
                add    ecx, eax
                mov    Slope, ecx              ;Save normal slope
                add    ecx, T01_MapWidth
                mov    [Slope - 4], ecx        ;Save slope + 1 scanline




;---- Setting up starting values...
                mov    eax, ULeftFixed          ;
                add    eax, 8000h               ;add 0.5 (16.16 fixed point)
                mov    ebx, eax                 ;Take copy
                shr    eax, 16                  ;Get int part
                shl    ebx, 16                  ;Get fractional part

                mov    esi, VLeftFixed
                add    esi, 8000h
                mov    ecx, esi
                shr    esi, 16
                shl    ecx, 16
                imul   esi, T01_MapWidth

                add    esi, eax                 ;Add U-offset
                add    esi, T01_MapOffset       ;Add textureoffset

                                                ;esi = mempointer
                                                ;ebx = U fractional part
                                                ;ecx = V fractional part




                mov    ebp, ScanLength
                inc    ebp

LeftOverLoop:

                mov    al, [esi]
                mov    [edi], al

                add    ecx, VFrac               ;cf set if ecx > 0.5
                sbb    eax, eax                 ;1 if carry

                add    ebx, UFrac               ;cf set if ebx > 0.5
                adc    esi, [Slope + eax*4]     ;increment texturepointer

                inc    edi
                dec    ebp
                jnz    LeftOverLoop




Scan_Done:
                ffree  st                       ;Free FPU-stack
                ffree  st(1)
                ffree  st(2)
                ffree  st(3)
                ffree  st(4)
                ffree  st(5)
                ffree  st(6)
                ffree  st(7)
                popad                           ;Restore all general regs

              ENDM









;旼컴컴컴컴컴컴컴컴컴컴컴컴 ASSEMBLER ROUTINE 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
;쿙ame         : T01_Tri
;쿔D           : T01
;쿟ype         : Procedure
;쿗ast update  : 12.07.1997
;쿌ction       : Draws a perspectivemapped triangel
;쿚ptimized    : No
;�
;쿔nput variables : [esi]           = Offset Face STRUCT
;�                  T01_MapOffset   = Offset texturemap
;�                  T01_MapWidth    = Mapwidth in bytes (!!! INTEGER !!!)
;�                  T01_MapModifier = Mappingcoordinate-modifer (default=1.0)
;�
;쿙otes: FPU stack must be empty. General registers are destroyed.
;쿔MPORTANT: T01_MapOffset and T01_MapWidth MUST BE SET!!!
;읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴

XSlopeA         DD 0.0                          ;FP: X-slope edge A
UZSlopeA        DD 0.0                          ;FP: U/Z-slope edge A
VZSlopeA        DD 0.0                          ;FP: V/Z-slope edge A
NZSlopeA        DD 0.0                          ;FP: 1/Z-slope edge A

XSlopeB         DD 0.0                          ;FP: X-slope edge B
UZSlopeB        DD 0.0                          ;FP: U/Z-slope edge B
VZSlopeB        DD 0.0                          ;FP: V/Z-slope edge B
NZSlopeB        DD 0.0                          ;FP: 1/Z-slope edge B

UZ0             DD 0.0                          ;FP: U0/Z
VZ0             DD 0.0                          ;FP: V0/Z
UZ1             DD 0.0                          ;FP: U1/Z
VZ1             DD 0.0                          ;FP: V1/Z
UZ2             DD 0.0                          ;FP: U2/Z
VZ2             DD 0.0                          ;FP: V2/Z

Y0Int           DD 0                            ;INT: Y0 rounded
Y1Int           DD 0                            ;INT: Y1 rounded
Y2Int           DD 0                            ;INT: Y2 rounded

Mempointer      DD 0                            ;INT: Ptr current scanline

;컴컴 General variables...
UZSlope         DD 0                            ;FP: U/Z slope
VZSlope         DD 0                            ;FP: V/Z slope
NZSlope         DD 0                            ;FP: 1/Z slope
UZSlope16       DD 0                            ;FP: U/Z slope *16
VZSlope16       DD 0                            ;FP: V/Z slope *16
NZSlope16       DD 0                            ;FP: 1/Z slope *16

EdgeA         Edge ?                            ;Edge A data
EdgeB         Edge ?                            ;Edge B data

;컴컴 Flipped values... (well, sorted that is)
_V0Offs         DD 0                            ;Offset to upper vertex
_V1Offs         DD 0                            ;Offset to middle vertex
_V2Offs         DD 0                            ;Offset to lower vertex

_U0             DD 0.0                          ;FP: Sorted map. coordinates
_V0             DD 0.0
_U1             DD 0.0
_V1             DD 0.0
_U2             DD 0.0
_V2             DD 0.0


T01_Tri       PROC


;---- Load, multiply and locally store mappingcoordinates...
                fld    [esi].U0
                fmul   T01_MapModifier
                fstp   _U0
                fld    [esi].V0
                fmul   T01_MapModifier
                fstp   _V0

                fld    [esi].U1
                fmul   T01_MapModifier
                fstp   _U1
                fld    [esi].V1
                fmul   T01_MapModifier
                fstp   _V1

                fld    [esi].U2
                fmul   T01_MapModifier
                fstp   _U2
                fld    [esi].V2
                fmul   T01_MapModifier
                fstp   _V2



;---- Load offsets to vertices...
;[ebx] = V0
;[ecx] = V1
;[edx] = V2
;
;These pointers are valid until the edgeflip

                mov    ebx, [esi].V0Offs
                mov    ecx, [esi].V1Offs
                mov    edx, [esi].V2Offs




;---- Sorting vertices...
                                      ;st0  st1  st2  st3  st4  st5  st6  st7
                fld    [edx].Y2D      ;V2Y
                fld    [ecx].Y2D      ;V1Y  V2Y
                fld    [ebx].Y2D      ;V0Y  V1Y  V2Y


;---- Check Y0 <-> Y1
                fcom   st(1)          ;V0Y  V1Y  V2Y
                fstsw  ax
                sahf
                jbe    Swapped01

                mov    eax, _U0                 ;Swapping V0 and V1
                mov    edi, _U1
                mov    _U0, edi
                mov    _U1, eax
                mov    eax, _V0
                mov    edi, _V1
                mov    _V0, edi
                mov    _V1, eax

                xchg   ebx, ecx
                fxch   st(1)

Swapped01:




;---- Check Y0 <-> Y2
                fcom   st(2)
                fstsw  ax
                sahf
                jbe    Swapped02

                mov    eax, _U0                 ;Swapping V0 and V2
                mov    edi, _U2
                mov    _U0, edi
                mov    _U2, eax
                mov    eax, _V0
                mov    edi, _V2
                mov    _V0, edi
                mov    _V2, eax


                xchg   ebx, edx
                fxch   st(2)
Swapped02:




;---- Check Y1 <-> Y2
                fxch   st(2)
                fcom   st(1)
                fstsw  ax
                sahf
                jae    Swapped12

                mov    eax, _U1                 ;Swapping V1 and V2
                mov    edi, _U2
                mov    _U1, edi
                mov    _U2, eax
                mov    eax, _V1
                mov    edi, _V2
                mov    _V1, edi
                mov    _V2, eax

                xchg   ecx, edx
                fxch   st(1)
Swapped12:

                mov    _V0Offs, ebx             ;Save verticeoffsets
                mov    _V1Offs, ecx
                mov    _V2Offs, edx


;---- There... Now st(0) > st(1) > st(2)
;                  =edx    =ecx    =ebx
; "Xxxx" means X-something
; "Yxxx" means Y-something
; "xNxx" means 1/xxx
; "xDxx" means delta-something
; "xSxx" means slope-something
; "xxxA" means something with edge A = V0 - V1
; "xxxB" means something with edge B = V0 - V2


                                      ;st0  st1  st2  st3  st4  st5  st6  st7
                                      ;Y2   Y1   Y0
;---- Get 1/YDeltaA and 1/YDeltaB
                fsub   st, st(2)      ;YDB  Y1   Y0
                fadd   AlmostZero
                fxch   st(1)          ;Y1   YDB  Y0
                fsub   st, st(2)      ;YDA  YDB  Y0
                fadd   AlmostZero
                fdivr  One            ;YNDA YDB  Y0
                fxch   st(1)          ;YDB  YNDA Y0
                fdivr  One            ;YNDB YNDA Y0
                ffree  st(2)          ;YNDB YNDA




;---- Get U/ZSlope A...
                fld    _U1            ;U1   YNDB YNDA
                fmul   [ecx].InvZ     ;UZ1  YNDB YNDA
                fst    UZ1            ;UZ1  YNDB YNDA
                fld    _U0            ;U0   UZ1  YNDB YNDA
                fmul   [ebx].InvZ     ;UZ0  UZ1  YNDB YNDA
                fst    UZ0            ;UZ0  UZ1  YNDB YNDA
                fsubp  st(1), st      ;UZDA YNDB YNDA
                fmul   st, st(2)      ;UZSA YNDB YNDA
                fstp   UZSlopeA       ;YNDB YNDA


;---- Get V/ZSlope A...
                fld    _V1            ;V1   YNDB YNDA
                fmul   [ecx].InvZ     ;VZ1  YNDB YNDA
                fst    VZ1            ;VZ1  YNDB YNDA
                fld    _V0            ;V0   VZ1  YNDB YNDA
                fmul   [ebx].InvZ     ;VZ0  VZ1  YNDB YNDA
                fst    VZ0            ;VZ0  VZ1  YNDB YNDA
                fsubp  st(1), st      ;VZDA YNDB YNDA
                fmul   st, st(2)      ;VZSA YNDB YNDA
                fstp   VZSlopeA       ;YNDB YNDA


;---- Get U/ZSlope B...
                fld    _U2            ;U2   YNDB YNDA
                fmul   [edx].InvZ     ;UZ2  YNDB YNDA
                fst    UZ2            ;UZ2  YNDB YNDA
                fsub   UZ0            ;UZDB YNDB YNDA
                fmul   st, st(1)      ;UZSB YNDB YNDA
                fstp   UZSlopeB       ;YNDB YNDA


;---- Get V/ZSlope B...
                fld    _V2            ;V2   YNDB YNDA
                fmul   [edx].InvZ     ;VZ2  YNDB YNDA
                fst    VZ2            ;VZ2  YNDB YNDA
                fsub   VZ0            ;VZDB YNDB YNDA
                fmul   st, st(1)      ;VZSB YNDB YNDA
                fstp   VZSlopeB       ;YNDB YNDA


;---- Get 1/ZSlope A...
                fld    [ecx].InvZ     ;NZ1  YNDB YNDA
                fsub   [ebx].InvZ     ;NZDA YNDB YNDA
                fmul   st, st(2)      ;NZSA YNDB YNDA
                fstp   NZSlopeA       ;YNDB YNDA

;---- Get 1/ZSlope B...
                fld    [edx].InvZ     ;NZ2  YNDB YNDA
                fsub   [ebx].InvZ     ;NZDB YNDB YNDA
                fmul   st, st(1)      ;NZSB YNDB YNDA
                fstp   NZSlopeB       ;YNDB YNDA


;---- Get XSlope A and B...
                fld    [ebx].X2D      ;X0   YNDB YNDA
                fld    [ecx].X2D      ;X1   X0   YNDB YNDA
                fsub   st, st(1)      ;XDA  X0   YNDB YNDA
                fmul   st, st(3)      ;XSA  X0   YNDB YNDA
                fstp   XSlopeA        ;X0   YNDB YNDA

                fld    [edx].X2D      ;X2   X0   YNDB YNDA
                fsub   st, st(1)      ;XDB  X0   YNDB YNDA
                fmul   st, st(2)      ;XSB  X0   YNDB YNDA
                fstp   XSlopeB        ;X0   YNDB YNDA

                ffree  st
                ffree  st(1)
                ffree  st(2)


;---- Get general U/ZSlope, V/ZSlope and 1/ZSlope...
;xMxx = value at middle of edge B
                fld    [ecx].Y2D      ;Y1
                fld    [ebx].Y2D      ;Y0   Y1
                fsubp  st(1), st      ;YDA

                fld    XSlopeB        ;XSB  YDA
                fmul   st, st(1)      ;XM0B YDA
                fadd   [ebx].X2D      ;XMB  YDA
                fsub   [ecx].X2D      ;XMD  YDA
                fdivr  One            ;XNMD YDA


;!!!! INTEGER INSTRUCTIONS HERE !!!!
;!!!! INTEGER INSTRUCTIONS HERE !!!!
;!!!! INTEGER INSTRUCTIONS HERE !!!!

                fld    UZSlopeB       ;UZSB XNMD YDA
                fmul   st, st(2)      ;UZMB XNMD YDA
                fadd   UZ0            ;UZMB XNMD YDA
                fsub   UZ1            ;UZMD XNMD YDA
                fmul   st, st(1)      ;UZS  XNMD YDA
                fst    UZSlope        ;UZS  XNMD YDA
                fmul   Sixteen        ;UZS  XNMD YDA
                fstp   UZSlope16      ;XNMD YDA

                fld    VZSlopeB       ;VZSB XNMD YDA
                fmul   st, st(2)      ;VZMB XNMD YDA
                fadd   VZ0            ;VZMB XNMD YDA
                fsub   VZ1            ;VZMD XNMD YDA
                fmul   st, st(1)      ;VZS  XNMD YDA
                fst    VZSlope        ;VZS  XNMD YDA
                fmul   Sixteen        ;VZS  XNMD YDA
                fstp   VZSlope16      ;XNMD YDA

                fld    NZSlopeB       ;NZSB XNMD YDA
                fmul   st, st(2)      ;NZMB XNMD YDA
                fadd   [ebx].InvZ     ;NZMB XNMD YDA
                fsub   [ecx].InvZ     ;NZMD XNMD YDA
                fmul   st, st(1)      ;NZS  XNMD YDA
                fst    NZSlope        ;NZS  XNMD YDA
                fmul   Sixteen        ;NZS  XNMD YDA
                fstp   NZSlope16      ;XNMD YDA

                ffree  st(1)          ;XNMD
                ffree  st             ;empty


;---- Rounding Y-values...
                fldcw  FPU_RoundUp
                fld    [ebx].Y2D    ;Y0
                fistp  Y0Int          ;
                fld    [ecx].Y2D    ;Y1
                fistp  Y1Int          ;
                fld    [edx].Y2D    ;Y2
                fistp  Y2Int


;---- Calculating memoffset
                fild   Y0Int          ;Y0I
                fild   SYS_DBufLine   ;SLS  Y0I
                fmul   st, st(1)      ;Offs Y0I
                fistp  MemPointer     ;Y0I



;---- Clipping edge A to next scanline...
                fsub   [ebx].Y2D      ;YD

                fld    st             ;YD   YD
                fmul   XSlopeA        ;XDA  YD
                fadd   [ebx].X2D      ;XA   YD
                fstp   EdgeA.X        ;YD

                fld    st             ;YD   YD
                fmul   UZSlopeA       ;UZDA YD
                fadd   UZ0            ;UZA  YD
                fstp   EdgeA.UdZ      ;YD

                fld    st             ;YD   YD
                fmul   VZSlopeA       ;VZDA YD
                fadd   VZ0            ;VZA  YD
                fstp   EdgeA.VdZ      ;YD

                fld    st             ;YD   YD
                fmul   NZSlopeA       ;NZDA YD
                fadd   [ebx].InvZ     ;NZA  YD
                fstp   EdgeA.NdZ      ;YD



;---- Clipping edge B to next scanline...
                fld    st             ;YD   YD
                fmul   XSlopeB        ;XDB  YD
                fadd   [ebx].X2D      ;XB   YD
                fstp   EdgeB.X        ;YD

                fld    st             ;YD   YD
                fmul   UZSlopeB       ;UZDB YD
                fadd   UZ0            ;UZB  YD
                fstp   EdgeB.UdZ      ;YD

                fld    st             ;YD   YD
                fmul   VZSlopeB       ;VZDB YD
                fadd   VZ0            ;VZB  YD
                fstp   EdgeB.VdZ      ;YD

                fmul   NZSlopeB       ;NZDA
                fadd   [ebx].InvZ     ;NZA
                fstp   EdgeB.NdZ


;---- Setup eax and ebx as edgepointers...
;[eax] = left edge
;[ebx] = right edge

                mov    edx, OFFSET EdgeA        ;Use edx temporary
                mov    ebx, OFFSET EdgeB

                fld    XSlopeB
                fld    XSlopeA
                fcom   st(1)
                fstsw  ax
                sahf
                jbe    NoFlipAB

                xchg   edx, ebx
NoFlipAB:

                ffree  st
                ffree  st(1)

                mov    eax, edx                 ;Use eax as pointer to edge A


                add    MemPointer, 0A0000h      ;Replace with SYS_DBufOffs
                xor    edx, edx

                mov    ecx, Y1Int               ;Get number of lines to draw
                sub    ecx, Y0Int

                                                ;[eax] = left edge
                                                ;[ebx] = right edge
                                                ; ecx  = loopcounter
                                                ; edx  = flag


ScanLoop:
                or     ecx, ecx
                jz     DoEdge12


                T01_ScanLine


;---- Update edge A...
                fld    EdgeA.X
                fadd   XSlopeA
                fstp   EdgeA.X

                fld    EdgeA.UdZ
                fadd   UZSlopeA
                fstp   EdgeA.UdZ

                fld    EdgeA.VdZ
                fadd   VZSlopeA
                fstp   EdgeA.VdZ

                fld    EdgeA.NdZ
                fadd   NZSlopeA
                fstp   EdgeA.NdZ



;---- Update edge B...
                fld    EdgeB.X
                fadd   XSlopeB
                fstp   EdgeB.X

                fld    EdgeB.UdZ
                fadd   UZSlopeB
                fstp   EdgeB.UdZ

                fld    EdgeB.VdZ
                fadd   VZSlopeB
                fstp   EdgeB.VdZ

                fld    EdgeB.NdZ
                fadd   NZSlopeB
                fstp   EdgeB.NdZ

                mov    ebp, SYS_DBufLine
                add    MemPointer, ebp

                dec    ecx
                jmp    ScanLoop


DoEdge12:       or     edx, edx
                jnz    Done

                mov    ebp, _V1Offs
                mov    edi, _V2Offs

                                                ;FPU stack is empty

;---- Get 1/DeltaY...
                fld    [edi].Y2D      ;Y2
                fsub   [ebp].Y2D      ;YD
                fdivr  One            ;YNDA


;---- Get new U/ZSlope A...
                fld    UZ2            ;UZ2  YNDA
                fsub   UZ1            ;UZDA YNDA
                fmul   st, st(1)      ;UZSA YNDA
                fstp   UZSlopeA       ;YNDA


;---- Get new V/ZSlope A...
                fld    VZ2            ;VZ2  YNDA
                fsub   VZ1            ;VZDA YNDA
                fmul   st, st(1)      ;VZSA YNDA
                fstp   VZSlopeA       ;YNDA


;---- Get new 1/ZSlope A...
                fld    [edi].InvZ     ;NZ2  YNDA
                fsub   [ebp].InvZ     ;NZDA YNDA
                fmul   st, st(1)      ;NZSA YNDA
                fstp   NZSlopeA       ;YNDA

;---- Get new XSlopeA...
                fld    [edi].X2D      ;X2   YNDA
                fsub   [ebp].X2D      ;XDA  YNDA
                fmul   st, st(1)      ;XSA  YNDA
                fstp   XSlopeA
                ffree  st


;---- Clipping edge C to next scanline and setting initial values...
                fldcw  FPU_RoundUp
                fld    [ebp].Y2D      ;Y1
                fistp  Y1Int          ;Round up

                fild   Y1Int          ;Y1I
                fsub   [ebp].Y2D      ;YD

                fld    st             ;YD   YD
                fmul   XSlopeA        ;XDA  YD
                fadd   [ebp].X2D      ;XA   YD
                fstp   EdgeA.X        ;YD

                fld    st             ;YD   YD
                fmul   UZSlopeA       ;UZDA YD
                fadd   UZ1            ;UZA  YD
                fstp   EdgeA.UdZ      ;YD

                fld    st             ;YD   YD
                fmul   VZSlopeA       ;VZDA YD
                fadd   VZ1            ;VZA  YD
                fstp   EdgeA.VdZ      ;YD

                fmul   NZSlopeA       ;NZDA
                fadd   [ebp].InvZ     ;NZA
                fstp   EdgeA.NdZ

                inc    edx            ;Set flag
                mov    ecx, Y2Int
                sub    ecx, Y1Int
                jmp    ScanLoop


Done:
                ret
T01_Tri       ENDP

END
