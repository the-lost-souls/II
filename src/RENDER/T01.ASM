;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ ASSEMBLER SOURCE ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
;³Name         : T01.ASM
;³Author       : Cyberfish/TLS & Toe/DU
;³Last update  : 12.07.97
;³Action       : Draws a perspective mapped triangle. No z-buffer or shading.
;³
;³Notes :
;³To be done:
;³ -fix the last pixel, if scanlength=17
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
.386
.Model Flat
.code

include w:\general\var.inc
include w:\vbe\vbe.inc

public T01_Tri
public T01_MapModifier
public T01_MapOffset
public T01_MapWidth


Edge          STRUC

X               DD ?
UdZ             DD ?
VdZ             DD ?
NdZ             DD ?

Edge          ENDS


;ÄÄÄÄ In-variables...
T01_MapModifier DD 1.0                          ;FP: Modifier, mappingcoordinates
T01_MapOffset   DD 0                            ;Offset texturemap
T01_MapWidth    DD 320                          ;INT: Tmap width, in bytes





;ÄÄÄÄ Locally used "constants"
Fixed16_16      DD 65536.0
Fixed16         DD 4096.0                       ;65536.0/ 16.0
Sixteen         DD 16.0
One             DD 1.0
AlmostZero      DD 0.000000000000000000001

FPDump          DD 0





;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ ASSEMBLER ROUTINE ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
;³Name         : T01_Scanline
;³ID           : T01
;³Type         : Macro
;³Last update  : 12.07.1997
;³Action       : Draws a perspectivemapped, horisontal scanline
;³Optimized    : Oh yeah!
;³
;³Input variables : [eax]  = left edge
;³                  [ebx]  = right edge
;³
;³Notes : FPU stack must be empty. All integer registers are saved.
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

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

;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ OPTIMIZED BLOCK A (PL_Ax.TXT) ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
;³* 57 cycles
;³* Clipping U/Z, V/Z and 1/Z left to next pixel
;³* Clipping U/Z, V/Z and 1/Z right to last pixel
;³* Calculating Z left
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

                                              ;st0  st1  st2  st3  st4  st5  st6  st7
                fldcw  FPU_RoundDown ;
                fld    [ebx].X       ;vr0      XR
                fld    [eax].X       ;vr3      XL   XR
                fxch   st(1)         ;         XR   XL
                fistp  EdgeRight.X   ;vr0      XL
                fild   EdgeRight.X   ;vr0      XRI  XL
                fldcw  FPU_RoundUp   ;         XRI  XL
                fsub   [ebx].X       ;vr0      XDR  XL
                fxch   st(1)         ;         XL   XDR
                fistp  EdgeLeft.X    ;vr3      XDR
                fild   EdgeLeft.X    ;vr3      XLI  XDR
                fld    st(1)         ;vr0->vr1 XDR  XLI  XDR
                fld    st(2)         ;vr0->vr2 XDR  XDR  XLI  XDR
                fxch   st(1)         ;         XDR  XDR  XLI  XDR
                fmul   UZSlope       ;vr1      UZDR XDR  XLI  XDR
                fxch   st(2)         ;         XLI  XDR  UZDR XDR
                fsub   [eax].X       ;vr3      XDL  XDR  UZDR XDR
                fxch   st(1)         ;         XDR  XDL  UZDR XDR
                fmul   VZSlope       ;vr2      VZDR XDL  UZDR XDR
                fxch   st(2)         ;         UZDR XDL  VZDR XDR
                fadd   [ebx].UdZ     ;vr1      UZR  XDL  VZDR XDR
                fxch   st(3)         ;         XDR  XDL  VZDR UZR
                fmul   NZSlope       ;vr0      NZDR XDL  VZDR UZR
                fxch   st(2)         ;         VZDR XDL  NZDR UZR
                fadd   [ebx].VdZ     ;vr2      VZR  XDL  NZDR UZR
                fld    st(1)         ;vr3->vr4 XDL  VZR  XDL  NZDR UZR
                fld    st(2)         ;vr3->vr5 XDL  XDL  VZR  XDL  NZDR UZR
                fxch   st(4)         ;         NZDR XDL  VZR  XDL  XDL  UZR
                fadd   [ebx].NdZ     ;vr0      NZR  XDL  VZR  XDL  XDL  UZR
                fxch   st(5)         ;         UZR  XDL  VZR  XDL  XDL  NZR
                fstp   EdgeRight.UdZ ;vr1      XDL  VZR  XDL  XDL  NZR
                fmul   NZSlope       ;vr3      NZDL VZR  XDL  XDL  NZR
                fxch   st(1)         ;         VZR  NZDL XDL  XDL  NZR
                fstp   EdgeRight.VdZ ;vr2      NZDL XDL  XDL  NZR
                fxch   st(1)         ;         XDL  NZDL XDL  NZR
                fmul   VZSlope       ;vr4      VZDL NZDL XDL  NZR
                fxch   st(3)         ;         NZR  NZDL XDL  VZDL
                fstp   EdgeRight.NdZ ;vr0      NZDL XDL  VZDL
                fxch   st(1)         ;         XDL  NZDL VZDL
                fmul   UZSlope       ;vr5      UZDL NZDL VZDL
                fxch   st(1)         ;         NZDL UZDL VZDL
                fadd   [eax].NdZ     ;vr3      NZL  UZDL VZDL
                fxch   st(2)         ;         VZDL UZDL NZL
                fadd   [eax].VdZ     ;vr4      VZL  UZDL NZL
                fxch   st(2)         ;         NZL  UZDL VZL
                fld    One           ;vr6      1    NZL  UZDL VZL
                fxch   st(2)         ;         UZDL NZL  1    VZL
                fadd   [eax].UdZ     ;vr5      UZL  NZL  1    VZL
                fxch   st(2)         ;         1    NZL  UZL  VZL
                fdiv   st, st(1)     ;vr6, vr3 ZL   NZL  UZL  VZL

;ÄÄÄÄ END BLOCK A

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


;ÄÄÄÄ Get U and V left (the fdiv is finished)
                fld    st            ;ZL   ZL   1/ZL U/ZL V/ZL
                fmul   st, st(4)     ;VL   ZL   1/ZL U/ZL V/ZL
                fxch   st(1)         ;ZL   VL   1/ZL U/ZL V/ZL
                fmul   st, st(3)     ;UL   VL   1/ZL U/ZL V/ZL


;ÄÄÄÄ Get U/Z and V/Z (right)
                fxch   st(2)         ;1/ZL VL   UL   U/ZL V/ZL
                fadd   NZSlope16     ;1/ZR VL   UL   U/ZL V/ZL
                fxch   st(3)         ;U/ZL VL   UL   1/ZR V/ZL
                fadd   UZSlope16     ;U/ZR VL   UL   1/ZR V/ZL
                fxch   st(4)         ;V/ZL VL   UL   1/ZR U/ZR
                fadd   VZSlope16     ;V/ZR VL   UL   1/ZR U/ZR
                fstp   st(5)         ;VL   UL   1/ZR U/ZR V/ZR


;ÄÄÄÄ Get Z-right
                fld    One           ;1    VL   UL   1/ZR U/ZR V/ZR
                fdiv   st, st(3)     ;ZR   VL   UL   1/ZR U/ZR V/ZR


Loop16:


;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ OPTIMIZED BLOCK B (PL_Bx.TXT) ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
;³* 45 cycles
;³* Calculating U and V right
;³* Calculating DeltaU and DeltaV
;³* Calculating slopes and stores them as 16.16 fixed point integers
;³* Saving U and V left as 16.16 fixed point integers
;³* Calculating U/Z, V/Z and 1/Z at next 16. pixel
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
                                               ;st0  st1  st2  st3  st4  st5  st6
                                               ;ZR   VL   UL   NZR  UZR  VZR

                fld    st            ;vr0->vr1  ZR   ZR   VL   UL   NZR  UZR  VZR
                fmul   st, st(5)     ;vr1, vr7  UR   ZR   VL   UL   NZR  UZR  VZR
                fxch   st(4)         ;          NZR  ZR   VL   UL   UR   UZR  VZR
                fadd   NZSlope16     ;vr6       NZRR ZR   VL   UL   UR   UZR  VZR
                fxch   st(1)         ;          ZR   NZRR VL   UL   UR   UZR  VZR
                fmul   st, st(6)     ;vr0, vr8  VR   NZRR VL   UL   UR   UZR  VZR
                fld    st(4)         ;vr1->vr3  UR   VR   NZRR VL   UL   UR   UZR  VZR
                fsub   st, st(4)     ;vr3, vr5  UD   VR   NZRR VL   UL   UR   UZR  VZR
                fxch   st(4)         ;          UL   VR   NZRR VL   UD   UR   UZR  VZR
                fmul   Fixed16_16    ;vr5       ULF  VR   NZRR VL   UD   UR   UZR  VZR
                fxch   st(6)         ;          UZR  VR   NZRR VL   UD   UR   ULF  VZR
                fadd   UZSlope16     ;vr7       UZRR VR   NZRR VL   UD   UR   ULF  VZR
                fxch   st(4)         ;          UD   VR   NZRR VL   UZRR UR   ULF  VZR
                fmul   Fixed16       ;vr3       US   VR   NZRR VL   UZRR UR   ULF  VZR
                fxch   st(7)         ;          VZR  VR   NZRR VL   UZRR UR   ULF  US
                fadd   VZSlope16     ;vr8       VZRR VR   NZRR VL   UZRR UR   ULF  US
                fxch   st(6)         ;          ULF  VR   NZRR VL   UZRR UR   VZRR US
                fistp  ULeftFixed    ;vr5       VR   NZRR VL   UZRR UR   VZRR US
                fld    st            ;vr0->vr2  VR   VR   NZRR VL   UZRR UR   VZRR US
                fsub   st, st(3)     ;vr2, vr4  VD   VR   NZRR VL   UZRR UR   VZRR US
                fxch   st(3)         ;          VL   VR   NZRR VD   UZRR UR   VZRR US
                fmul   Fixed16_16    ;vr4       VLF  VR   NZRR VD   UZRR UR   VZRR US
                fxch   st(7)         ;          US   VR   NZRR VD   UZRR UR   VZRR VLF
                fistp  USlopeFixed   ;vr3       VR   NZRR VD   UZRR UR   VZRR VLF
                fxch   st(2)         ;          VD   NZRR VR   UZRR UR   VZRR VLF
                fmul   Fixed16       ;vr2       VS   NZRR VR   UZRR UR   VZRR VLF
                fxch   st(6)         ;          VLF  NZRR VR   UZRR UR   VZRR VS
                fistp  VLeftFixed    ;vr4       NZRR VR   UZRR UR   VZRR VS
                fxch   st(5)         ;          VS   VR   UZRR UR   VZRR NZRR
                fistp  VSlopeFixed   ;vr2       VR   UZRR UR   VZRR NZRR

                fxch   st(1)         ;          UZRR VR   UR   VZRR NZRR
                fxch   st(4)         ;          NZRR VR   UR   VZRR UZRR
                fxch   st(3)         ;          VZRR VR   UR   NZRR UZRR
                fstp   st(5)         ;          VR   UR   NZRR UZRR VZRR


;ÄÄÄÄ END BLOCK B




;ÄÄÄÄ Check if we need Z for end of line next time...
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



;ÄÄÄÄ Setting up slopes and initial values...
                mov    eax, USlopeFixed         ;U:1 (A1) !!!Cache-delay!!!
                mov    ecx, VSlopeFixed         ;V:2 (B1) /

                mov    ebx, eax                 ;U:3 (A2) Take copy
                mov    edx, ecx                 ;V:3 (B2) /

                sal    ebx, 16                  ;U:4 (A3) Get fractional part
                mov    esi, VLeftFixed          ;V:4 (D1)

                sar    eax, 16                  ;U:5 (A3) Get int part
                mov    UFrac, ebx               ;V:5 (A4) Save fractional part

                sal    edx, 16                  ;U:6 (B3)
                add    esi, 8000h               ;V:6 (D2)

                sar    ecx, 16                  ;U:7 (B3)
                mov    VFrac, edx               ;V:7 (B4)

                imul   ecx, T01_MapWidth        ;U:8-19 (B4)

                add    ecx, eax                 ;U:20 (AB5)
                mov    ebp, T01_MapWidth        ;V:20 (AB6)

                mov    Slope, ecx               ;U:21 (AB6) Save normal slope
                mov    eax, ULeftFixed          ;V:21 (C1)

                add    ecx, ebp                 ;U:22 (AB7)
                add    eax, 8000h               ;V:22 (C2) add 0.5 (16.16 fixed point)

                mov    ebx, eax                 ;U:23 (C3) Take copy
                mov    [Slope - 4], ecx         ;V:23 (AB8) Save slope + 1 scanline

                mov    ecx, esi                 ;U:24 (D3)

                shr    esi, 16                  ;U:25 (D4)

                imul   esi, T01_MapWidth        ;U:26-37 (D5)

                shr    eax, 16                  ;U:38 (C4) Get int part
                mov    ebp, T01_MapOffset       ;V:38

                shl    ebx, 16                  ;U:39 (C4) Get fractional part
                add    esi, eax                 ;V:39 Add U-offset


                shl    ecx, 16                  ;U:40 (D5)
                add    esi, ebp                 ;V:40 Add textureoffset

                                                ;esi = mempointer
                                                ;ebx = U fractional part
                                                ;ecx = V fractional part



;ÄÄÄÄ Get next Z...
                fld    One           ;1    VL   UL   1/ZR U/ZR V/ZR
                fdiv   st, st(3)     ;ZR   VL   UL   1/ZR U/ZR V/ZR

                mov    edx, UFrac               ;U:01
                mov    al, [esi]                ;V:01

                add    ecx, VFrac               ;U:02-03

                sbb    ebp, ebp                 ;U:04
                add    ebx, edx                 ;V:04

                adc    esi, [Slope + ebp*4]     ;U:05-06
                add    ecx, VFrac               ;V:05-06
;ÄÄÄÄ
                sbb    ebp, ebp                 ;U:07
                mov    [edi + 0], al            ;V:07

                mov    al, [esi]                ;U:08
                add    ebx, edx                 ;V:08

                adc    esi, [Slope + ebp*4]     ;U:09-10
                add    ecx, VFrac               ;V:09-10
;ÄÄÄÄ
                sbb    ebp, ebp                 ;U:11
                mov    [edi + 1], al            ;V:11

                mov    al, [esi]                ;U:12
                add    ebx, edx                 ;V:12

                adc    esi, [Slope + ebp*4]     ;U:13-14
                add    ecx, VFrac               ;V:13-14
;ÄÄÄÄ
                sbb    ebp, ebp                 ;U:15
                mov    [edi + 2], al            ;V:15

                mov    al, [esi]                ;U:16
                add    ebx, edx                 ;V:16

                adc    esi, [Slope + ebp*4]     ;U:17-18
                add    ecx, VFrac               ;V:17-18
;ÄÄÄÄ
                sbb    ebp, ebp                 ;U:19
                mov    [edi + 3], al            ;V:19

                mov    al, [esi]                ;U:20
                add    ebx, edx                 ;V:20

                adc    esi, [Slope + ebp*4]     ;U:21-22
                add    ecx, VFrac               ;V:21-22
;ÄÄÄÄ
                sbb    ebp, ebp                 ;U:23
                mov    [edi + 4], al            ;V:23

                mov    al, [esi]                ;U:24
                add    ebx, edx                 ;V:24

                adc    esi, [Slope + ebp*4]     ;U:25-26
                add    ecx, VFrac               ;V:25-26
;ÄÄÄÄ
                sbb    ebp, ebp                 ;U:27
                mov    [edi + 5], al            ;V:27

                mov    al, [esi]                ;U:28
                add    ebx, edx                 ;V:28

                adc    esi, [Slope + ebp*4]     ;U:29-30
                add    ecx, VFrac               ;V:29-30
;ÄÄÄÄ
                sbb    ebp, ebp                 ;U:31
                mov    [edi + 6], al            ;V:31

                mov    al, [esi]                ;U:32
                add    ebx, edx                 ;V:32

                adc    esi, [Slope + ebp*4]     ;U:33-34
                add    ecx, VFrac               ;V:33-34
;ÄÄÄÄ
                sbb    ebp, ebp                 ;U:35
                mov    [edi + 7], al            ;V:35

                mov    al, [esi]                ;U:36
                add    ebx, edx                 ;V:36

                adc    esi, [Slope + ebp*4]     ;U:37-38
                add    ecx, VFrac               ;V:37-38
;ÄÄÄÄ
                sbb    ebp, ebp                 ;U:39
                mov    [edi + 8], al            ;V:39

                mov    al, [esi]                ;U:40
                add    ebx, edx                 ;V:40

                adc    esi, [Slope + ebp*4]     ;U:41-42
                add    ecx, VFrac               ;V:41-42
;ÄÄÄÄ
                sbb    ebp, ebp                 ;U:43
                mov    [edi + 9], al            ;V:43

                mov    al, [esi]                ;U:44
                add    ebx, edx                 ;V:44

                adc    esi, [Slope + ebp*4]     ;U:45-46
                add    ecx, VFrac               ;V:45-46
;ÄÄÄÄ
                sbb    ebp, ebp                 ;U:47
                mov    [edi + 10], al           ;V:47

                mov    al, [esi]                ;U:48
                add    ebx, edx                 ;V:48

                adc    esi, [Slope + ebp*4]     ;U:49-50
                add    ecx, VFrac               ;V:49-50
;ÄÄÄÄ
                sbb    ebp, ebp                 ;U:51
                mov    [edi + 11], al           ;V:51

                mov    al, [esi]                ;U:52
                add    ebx, edx                 ;V:52

                adc    esi, [Slope + ebp*4]     ;U:53-54
                add    ecx, VFrac               ;V:53-54
;ÄÄÄÄ
                sbb    ebp, ebp                 ;U:55
                mov    [edi + 12], al           ;V:55

                mov    al, [esi]                ;U:56
                add    ebx, edx                 ;V:56

                adc    esi, [Slope + ebp*4]     ;U:57-58
                add    ecx, VFrac               ;V:57-58
;ÄÄÄÄ
                sbb    ebp, ebp                 ;U:59
                mov    [edi + 13], al           ;V:59

                mov    al, [esi]                ;U:60
                add    ebx, edx                 ;V:60

;ÄÄÄÄ Saving pixel 14, setting up for pixel 15 and checking for EOL
                adc    esi, [Slope + ebp*4]     ;U:61-62
                mov    ebx, ScanLength          ;V:61

                mov    [edi + 14], al           ;U:62
                sub    ebx, 16                  ;V:62

                mov    dl, [esi]                ;U:63
                mov    ScanLength, ebx          ;V:63

                mov    [edi + 15], dl           ;U:64
                cmp    ebx, 16                  ;V:64

                lea    edi, [edi + 16]          ;U:65 Move memorypointer
                jbe    LeftOver2                ;V:65

                jmp    Loop16                   ;U:66












;ÄÄÄÄ Write leftover pixels...

LeftOver1:

;ÄÄÄÄ Get U and V left
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
;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ OPTIMIZED BLOCK C (PL_Cx.TXT) ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
;³* 38 cycles
;³* Calculating U and V right
;³* Calculating DeltaU and DeltaV
;³* Calculating slopes and stores them as 16.16 fixed point integers
;³* Saving U and V left as 16.16 fixed point integers
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

                                              ;st0  st1  st2  st3  st4  st5  st6
                                              ;ZR   VL   UL   ---- UZR  VZR

                fld    st            ;vr0->vr1 ZR   ZR   VL   UL   ---- UZR  VZR
                fmul   st, st(5)     ;vr1      UR   ZR   VL   UL   ---- UZR  VZR
                fild   ScanLength    ;->vr2    SL   UR   ZR   VL   UL   ---- UZR  VZR
                fxch   st(2)         ;         ZR   UR   SL   VL   UL   ---- UZR  VZR
                fmul   st, st(7)     ;vr0      VR   UR   SL   VL   UL   ---- UZR  VZR
                fxch   st(1)         ;         UR   VR   SL   VL   UL   ---- UZR  VZR
                fsub   st, st(4)     ;vr1      UD   VR   SL   VL   UL   ---- UZR  VZR
                fxch   st(4)         ;         UL   VR   SL   VL   UD   ---- UZR  VZR
                fmul   Fixed16_16    ;vr3      ULF  VR   SL   VL   UD   ---- UZR  VZR
                fxch   st(1)         ;         VR   ULF  SL   VL   UD   ---- UZR  VZR
                fsub   st, st(3)     ;vr0      VD   ULF  SL   VL   UD   ---- UZR  VZR
                fxch   st(3)         ;         VL   ULF  SL   VD   UD   ---- UZR  VZR
                fmul   Fixed16_16    ;vr4      VLF  ULF  SL   VD   UD   ---- UZR  VZR
                fxch   st(1)         ;         ULF  VLF  SL   VD   UD   ---- UZR  VZR
                fistp  ULeftFixed    ;vr3      VLF  SL   VD   UD   ---- UZR  VZR
                fistp  VLeftFixed    ;vr4      SL   VD   UD   ---- UZR  VZR
                fdivr  Fixed16_16    ;vr2      FP/L VD   UD   ---- UZR  VZR
                fmul   st(1), st     ;vr0      FP/L VS   UD   ---- UZR  VZR
                fmulp  st(2), st     ;vr1      VS   US   ---- UZR  VZR
                fistp  VSlopeFixed   ;vr0      US   ---- UZR  VZR
                fistp  USlopeFixed   ;vr1      ---- UZR  VZR



;ÄÄÄÄ Setting up slopes and initial values...
                mov    eax, USlopeFixed         ;U:1 (A1) !!!Cache-delay!!!
                mov    ecx, VSlopeFixed         ;V:2 (B1) /

                mov    ebx, eax                 ;U:3 (A2) Take copy
                mov    edx, ecx                 ;V:3 (B2) /

                sal    ebx, 16                  ;U:4 (A3) Get fractional part
                mov    esi, VLeftFixed          ;V:4 (D1)

                sar    eax, 16                  ;U:5 (A3) Get int part
                mov    UFrac, ebx               ;V:5 (A4) Save fractional part

                sal    edx, 16                  ;U:6 (B3)
                add    esi, 8000h               ;V:6 (D2)

                sar    ecx, 16                  ;U:7 (B3)
                mov    VFrac, edx               ;V:7 (B4)

                imul   ecx, T01_MapWidth        ;U:8-19 (B4)

                add    ecx, eax                 ;U:20 (AB5)
                mov    ebp, T01_MapWidth        ;V:20 (AB6)

                mov    Slope, ecx               ;U:21 (AB6) Save normal slope
                mov    eax, ULeftFixed          ;V:21 (C1)

                add    ecx, ebp                 ;U:22 (AB7)
                add    eax, 8000h               ;V:22 (C2) add 0.5 (16.16 fixed point)

                mov    ebx, eax                 ;U:23 (C3) Take copy
                mov    [Slope - 4], ecx         ;V:23 (AB8) Save slope + 1 scanline

                mov    ecx, esi                 ;U:24 (D3)

                shr    esi, 16                  ;U:25 (D4)

                imul   esi, T01_MapWidth        ;U:26-37 (D5)

                shr    eax, 16                  ;U:38 (C4) Get int part
                mov    ebp, T01_MapOffset       ;V:38


                shl    ebx, 16                  ;U:39 (C4) Get fractional part
                add    esi, eax                 ;V:39 Add U-offset


                shl    ecx, 16                  ;U:40 (D5)
                add    esi, ebp                 ;V:40 Add textureoffset

                                                ;esi = mempointer
                                                ;ebx = U fractional part
                                                ;ecx = V fractional part

                mov    ebp, ScanLength
                inc    ebp

                mov    edx, UFrac

LeftOverLoop:
                mov    al, [esi]                ;U:1

                mov    [edi], al                ;U:2
                add    ecx, VFrac               ;V:2-3 cf set if ecx > 0.5

                sbb    eax, eax                 ;U:4   -1 if carry
                add    ebx, edx                 ;V:4   cf set if ebx > 0.5

                adc    esi, [Slope + eax*4]     ;U:5-6 increment texturepointer
                inc    edi                      ;V:5

                dec    ebp                      ;U:7
                jnz    LeftOverLoop             ;V:7


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









;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ ASSEMBLER ROUTINE ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
;³Name         : T01_Tri
;³ID           : T01
;³Type         : Procedure
;³Last update  : 12.07.1997
;³Action       : Draws a perspectivemapped triangel
;³Optimized    : Just the scanloop. Nothing else is pipelined. Should be done.
;³
;³Input variables : [esi]           = Offset Face STRUCT
;³                  T01_MapOffset   = Offset texturemap
;³                  T01_MapWidth    = Mapwidth in bytes (!!! INTEGER !!!)
;³                  T01_MapModifier = Mappingcoordinate-modifer (default=1.0)
;³
;³Notes: FPU stack must be empty. General registers are destroyed.
;³IMPORTANT: T01_MapOffset and T01_MapWidth MUST BE SET!!!
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

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

;ÄÄÄÄ General variables...
UZSlope         DD 0                            ;FP: U/Z slope
VZSlope         DD 0                            ;FP: V/Z slope
NZSlope         DD 0                            ;FP: 1/Z slope
UZSlope16       DD 0                            ;FP: U/Z slope *16
VZSlope16       DD 0                            ;FP: V/Z slope *16
NZSlope16       DD 0                            ;FP: 1/Z slope *16

EdgeA         Edge ?                            ;Edge A data
EdgeB         Edge ?                            ;Edge B data

;ÄÄÄÄ Flipped values... (well, sorted that is)
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

;ÄÄÄÄ Load, multiply and locally store mappingcoordinates...
                fld    [esi].F_U0
                fmul   T01_MapModifier
                fstp   _U0
                fld    [esi].F_V0
                fmul   T01_MapModifier
                fstp   _V0

                fld    [esi].F_U1
                fmul   T01_MapModifier
                fstp   _U1
                fld    [esi].F_V1
                fmul   T01_MapModifier
                fstp   _V1

                fld    [esi].F_U2
                fmul   T01_MapModifier
                fstp   _U2
                fld    [esi].F_V2
                fmul   T01_MapModifier
                fstp   _V2


;ÄÄÄÄ Load offsets to vertices...
;[ebx] = V0
;[ecx] = V1
;[edx] = V2
;
;These pointers are valid until the edgeflip

                mov    ebx, [esi].F_V0Offs
                mov    ecx, [esi].F_V1Offs
                mov    edx, [esi].F_V2Offs




;ÄÄÄÄ Sorting vertices...
                                      ;st0  st1  st2  st3  st4  st5  st6  st7
                fld    [edx].V_Y2D    ;V2Y
                fld    [ecx].V_Y2D    ;V1Y  V2Y
                fld    [ebx].V_Y2D    ;V0Y  V1Y  V2Y


;ÄÄÄÄ Check Y0 <-> Y1
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




;ÄÄÄÄ Check Y0 <-> Y2
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




;ÄÄÄÄ Check Y1 <-> Y2
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


;ÄÄÄÄ There... Now st(0) > st(1) > st(2)
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
;ÄÄÄÄ Get 1/YDeltaA and 1/YDeltaB
                fsub   st, st(2)      ;YDB  Y1   Y0
                fadd   AlmostZero     ;
                fxch   st(1)          ;Y1   YDB  Y0
                fsub   st, st(2)      ;YDA  YDB  Y0
                fadd   AlmostZero     ;
                fdivr  One            ;YNDA YDB  Y0
                fxch   st(1)          ;YDB  YNDA Y0
                fdivr  One            ;YNDB YNDA Y0
                ffree  st(2)          ;YNDB YNDA




;ÄÄÄÄ Get U/ZSlope A...
                fld    _U1            ;U1   YNDB YNDA
                fmul   [ecx].V_InvZ   ;UZ1  YNDB YNDA
                fst    UZ1            ;UZ1  YNDB YNDA
                fld    _U0            ;U0   UZ1  YNDB YNDA
                fmul   [ebx].V_InvZ   ;UZ0  UZ1  YNDB YNDA
                fst    UZ0            ;UZ0  UZ1  YNDB YNDA
                fsubp  st(1), st      ;UZDA YNDB YNDA
                fmul   st, st(2)      ;UZSA YNDB YNDA
                fstp   UZSlopeA       ;YNDB YNDA


;ÄÄÄÄ Get V/ZSlope A...
                fld    _V1            ;V1   YNDB YNDA
                fmul   [ecx].V_InvZ   ;VZ1  YNDB YNDA
                fst    VZ1            ;VZ1  YNDB YNDA
                fld    _V0            ;V0   VZ1  YNDB YNDA
                fmul   [ebx].V_InvZ   ;VZ0  VZ1  YNDB YNDA
                fst    VZ0            ;VZ0  VZ1  YNDB YNDA
                fsubp  st(1), st      ;VZDA YNDB YNDA
                fmul   st, st(2)      ;VZSA YNDB YNDA
                fstp   VZSlopeA       ;YNDB YNDA


;ÄÄÄÄ Get U/ZSlope B...
                fld    _U2            ;U2   YNDB YNDA
                fmul   [edx].V_InvZ   ;UZ2  YNDB YNDA
                fst    UZ2            ;UZ2  YNDB YNDA
                fsub   UZ0            ;UZDB YNDB YNDA
                fmul   st, st(1)      ;UZSB YNDB YNDA
                fstp   UZSlopeB       ;YNDB YNDA


;ÄÄÄÄ Get V/ZSlope B...
                fld    _V2            ;V2   YNDB YNDA
                fmul   [edx].V_InvZ   ;VZ2  YNDB YNDA
                fst    VZ2            ;VZ2  YNDB YNDA
                fsub   VZ0            ;VZDB YNDB YNDA
                fmul   st, st(1)      ;VZSB YNDB YNDA
                fstp   VZSlopeB       ;YNDB YNDA


;ÄÄÄÄ Get 1/ZSlope A...
                fld    [ecx].V_InvZ   ;NZ1  YNDB YNDA
                fsub   [ebx].V_InvZ   ;NZDA YNDB YNDA
                fmul   st, st(2)      ;NZSA YNDB YNDA
                fstp   NZSlopeA       ;YNDB YNDA

;ÄÄÄÄ Get 1/ZSlope B...
                fld    [edx].V_InvZ   ;NZ2  YNDB YNDA
                fsub   [ebx].V_InvZ   ;NZDB YNDB YNDA
                fmul   st, st(1)      ;NZSB YNDB YNDA
                fstp   NZSlopeB       ;YNDB YNDA


;ÄÄÄÄ Get XSlope A and B...
                fld    [ebx].V_X2D    ;X0   YNDB YNDA
                fld    [ecx].V_X2D    ;X1   X0   YNDB YNDA
                fsub   st, st(1)      ;XDA  X0   YNDB YNDA
                fmul   st, st(3)      ;XSA  X0   YNDB YNDA
                fstp   XSlopeA        ;X0   YNDB YNDA

                fld    [edx].V_X2D    ;X2   X0   YNDB YNDA
                fsub   st, st(1)      ;XDB  X0   YNDB YNDA
                fmul   st, st(2)      ;XSB  X0   YNDB YNDA
                fstp   XSlopeB        ;X0   YNDB YNDA

                ffree  st
                ffree  st(1)
                ffree  st(2)


;ÄÄÄÄ Get general U/ZSlope, V/ZSlope and 1/ZSlope...
;xMxx = value at middle of edge B
                fld    [ecx].V_Y2D    ;Y1
                fld    [ebx].V_Y2D    ;Y0   Y1
                fsubp  st(1), st      ;YDA

                fld    XSlopeB        ;XSB  YDA
                fmul   st, st(1)      ;XM0B YDA
                fadd   [ebx].V_X2D    ;XMB  YDA
                fsub   [ecx].V_X2D    ;XMD  YDA
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
                fadd   [ebx].V_InvZ   ;NZMB XNMD YDA
                fsub   [ecx].V_InvZ   ;NZMD XNMD YDA
                fmul   st, st(1)      ;NZS  XNMD YDA
                fst    NZSlope        ;NZS  XNMD YDA
                fmul   Sixteen        ;NZS  XNMD YDA
                fstp   NZSlope16      ;XNMD YDA

                ffree  st(1)          ;XNMD
                ffree  st             ;empty


;ÄÄÄÄ Rounding Y-values...
                fldcw  FPU_RoundUp
                fld    [ebx].V_Y2D    ;Y0
                fistp  Y0Int          ;
                fld    [ecx].V_Y2D    ;Y1
                fistp  Y1Int          ;
                fld    [edx].V_Y2D    ;Y2
                fistp  Y2Int


;ÄÄÄÄ Calculating memoffset
                fild   Y0Int          ;Y0I
                fild   SYS_DBufSize   ;SLS  Y0I
                fmul   st, st(1)      ;Offs Y0I
                fistp  MemPointer     ;Y0I



;ÄÄÄÄ Clipping edge A to next scanline...
                fsub   [ebx].V_Y2D    ;YD

                fld    st             ;YD   YD
                fmul   XSlopeA        ;XDA  YD
                fadd   [ebx].V_X2D    ;XA   YD
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
                fadd   [ebx].V_InvZ   ;NZA  YD
                fstp   EdgeA.NdZ      ;YD



;ÄÄÄÄ Clipping edge B to next scanline...
                fld    st             ;YD   YD
                fmul   XSlopeB        ;XDB  YD
                fadd   [ebx].V_X2D    ;XB   YD
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
                fadd   [ebx].V_InvZ   ;NZA
                fstp   EdgeB.NdZ


;ÄÄÄÄ Setup eax and ebx as edgepointers...
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


;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ OPTIMIZED BLOCK D (PL_Dx.TXT) ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
;³* 32 cycles
;³* Updating variables for both edges
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
                                        ;st0  st1  st2  st3  st4  st5  st6  st7

                fld    EdgeA.X     ;vr0  XA
                fadd   XSlopeA     ;vr0  XAn
                fld    EdgeA.UdZ   ;vr1  UZA  XAn
                fadd   UZSlopeA    ;vr1  UZAn XAn
                fxch   st(1)       ;     XAn  UZAn
                fld    EdgeA.VdZ   ;vr2  VZA  XAn UZAn
                fxch   st(1)       ;     XAn  VZA UZAn
                fstp   EdgeA.X     ;vr0  VZA  UZAn
                fadd   VZSlopeA    ;vr2  VZAn UZAn
                fxch   st(1)       ;     UZAn VZAn
                fstp   EdgeA.UdZ   ;vr1  VZAn
                fld    EdgeA.NdZ   ;vr3  NZA  VZAn
                fadd   NZSlopeA    ;vr3  NZAn VZAn
                fxch   st(1)       ;     VZAn NZAn
                fstp   EdgeA.VdZ   ;vr2  NZAn
                fld    EdgeB.X     ;vr4  XB   NZAn
                fadd   XSlopeB     ;vr4  XBn  NZAn
                fxch   st(1)       ;     NZAn XBn
                fstp   EdgeA.NdZ   ;vr3  XBn
                fld    EdgeB.UdZ   ;vr5  UZB  XBn
                fadd   UZSlopeB    ;vr5  UZBn XBn
                fxch   st(1)       ;     XBn  UZBn
                fstp   EdgeB.X     ;vr4  UZBn
                fld    EdgeB.VdZ   ;vr6  VZB  UZBn
                fadd   VZSlopeB    ;vr6  VZBn UZBn
                fxch   st(1)       ;     UZBn VZBn
                fstp   EdgeB.UdZ   ;vr5  VZBn
                fld    EdgeB.NdZ   ;vr7  NZB  VZBn
                fadd   NZSlopeB    ;vr7  NZBn VZBn
                fxch   st(1)       ;     VZBn NZBn
                fstp   EdgeB.VdZ   ;vr6  NZBn
                fstp   EdgeB.NdZ   ;vr7  empty

                dec    ecx                   ;U:1
                mov    ebp, VBE_XRes         ;V:1

                add    MemPointer, ebp       ;U:2-3
                jmp    ScanLoop              ;V:2


DoEdge12:       or     edx, edx
                jnz    Done




                mov    ebp, _V1Offs
                mov    edi, _V2Offs

                                                ;FPU stack is empty

;ÄÄÄÄ Get 1/DeltaY...
                fld    [edi].V_Y2D    ;Y2
                fsub   [ebp].V_Y2D    ;YD
                fdivr  One            ;YNDA


;ÄÄÄÄ Get new U/ZSlope A...
                fld    UZ2            ;UZ2  YNDA
                fsub   UZ1            ;UZDA YNDA
                fmul   st, st(1)      ;UZSA YNDA
                fstp   UZSlopeA       ;YNDA


;ÄÄÄÄ Get new V/ZSlope A...
                fld    VZ2            ;VZ2  YNDA
                fsub   VZ1            ;VZDA YNDA
                fmul   st, st(1)      ;VZSA YNDA
                fstp   VZSlopeA       ;YNDA


;ÄÄÄÄ Get new 1/ZSlope A...
                fld    [edi].V_InvZ   ;NZ2  YNDA
                fsub   [ebp].V_InvZ   ;NZDA YNDA
                fmul   st, st(1)      ;NZSA YNDA
                fstp   NZSlopeA       ;YNDA

;ÄÄÄÄ Get new XSlopeA...
                fld    [edi].V_X2D    ;X2   YNDA
                fsub   [ebp].V_X2D    ;XDA  YNDA
                fmul   st, st(1)      ;XSA  YNDA
                fstp   XSlopeA
                ffree  st


;ÄÄÄÄ Clipping edge C to next scanline and setting initial values...
                fldcw  FPU_RoundUp
                fld    [ebp].V_Y2D    ;Y1
                fistp  Y1Int          ;Round up

                fild   Y1Int          ;Y1I
                fsub   [ebp].V_Y2D    ;YD

                fld    st             ;YD   YD
                fmul   XSlopeA        ;XDA  YD
                fadd   [ebp].V_X2D    ;XA   YD
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
                fadd   [ebp].V_InvZ   ;NZA
                fstp   EdgeA.NdZ

                inc    edx            ;Set flag
                mov    ecx, Y2Int
                sub    ecx, Y1Int
                jmp    ScanLoop


Done:
                ret
T01_Tri       ENDP

END