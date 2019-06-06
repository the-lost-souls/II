;旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴 ASSEMBLER SOURCE 컴컴컴컴컴컴컴컴컴컴컴컴컴컴
;쿙ame         : T02_TEST.ASM
;쿌uthor       : Cyberfish/TLS & Toe/DU
;쿗ast update  : 12.07.97
;쿌ction       : Tests T02 and T02_Org
;�
;쿙otes : Just link with T02.OBJ or T02_ORG.OBJ
;읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
.386
.Model Flat
.code
.stack 100h

include w:\general\main.inc
include w:\general\var.inc
include w:\general\general.inc
include w:\gfx\gif.inc
include w:\general\mem.inc
include w:\render\render.inc
include w:\vesa\vbe.inc





Tri            DD 0                             ;Parttype
               DD 0                             ;Next poly
               DD 0                             ;Sortvalue
               DD 0.0                           ;NVX
               DD 0.0                           ;NVY
               DD 0.0                           ;NVZ
               DD OFFSET Vert0
               DD OFFSET Vert1
               DD OFFSET Vert2
               DD 0                             ;Material

               DD 0                             ;R, G, B, 0
               DD 0.0                           ;U0
               DD 0.0                           ;V0
               DD 319.0                         ;U1
               DD 0.0                           ;V1
               DD 0.0                           ;U2
               DD 199.0                         ;V2

Vert0          DD 0
               DD 0
               DD 20.0                          ;X, Y, Z
               DD 0.0                           ;1/Z
               DD -10.0                          ;2D-X
               DD -10.0                          ;2D-Y
               DD 0, 0, 0                       ;X, Y, Z rotated

Vert1          DD 0, 0, 20.0                    ;X, Y, Z
               DD 0.0                           ;1/Z
               DD 350.0                           ;2D-X
               DD 290.0                          ;2D-Y
               DD 0, 0, 0                       ;X, Y, Z rotated

Vert2          DD 0, 0, 10.0                    ;X, Y, Z
               DD 0.0                           ;1/Z
               DD -70.0                          ;2D-X
               DD 70.0                         ;2D-Y
               DD 0, 0, 0                       ;X, Y, Z rotated



Tri2           DD 0
               DD OFFSET Vert3
               DD OFFSET Vert4
               DD OFFSET Vert5
               DD 0                             ;Material
               DD 0                             ;R, G, B, 0
               DD 0.5                        ;U0
               DD 0.0                           ;V0
               DD 319.0                         ;U1
               DD 0.0                           ;V1
               DD 319.0                          ;U2
               DD 199.0                          ;V2


Vert3          DD 0
               DD 0
               DD 10.0                          ;X, Y, Z
               DD 0.0                           ;1/Z
               DD 0, 0, 0                       ;X, Y, Z rotated
               DD 0.0                        ;2D-X
               DD 0.0                ;2D-Y

Vert4          DD 0, 0, 20.0                   ;X, Y, Z
               DD 0.0                           ;1/Z
               DD 0, 0, 0                       ;X, Y, Z rotated
               DD 50.0                  ;2D-X
               DD 0.0                                   ;2D-Y

Vert5          DD 0, 0, 20.0                    ;X, Y, Z
               DD 0.0                           ;1/Z
               DD 0, 0, 0                       ;X, Y, Z rotated
               DD 50.0                          ;2D-X
               DD 50.0                         ;2D-Y



Old_INT8        DF 0
Ticks           DD 0
Triangles       DD 1

NrString        DB 'Number of triangles drawn : $'
EOL             DB 0Dh, 0Ah, '$'
Gif             DB 'chess.gif',0

extrn debug:proc
extrn debug_run:PROC

Main          PROC
                call   debug_run
                call   D32_Setup
                jc     Error

                mov    eax, -1024*800           ;Set away 800k
                call   MEM_Init
                jc     Error

;                int 3

                call   VBE_GetInfo
                jc     Error

;---- Loading texturemap...
                mov    edx, OFFSET Gif          ;Load texturemap
                xor    esi, esi
                xor    ecx, ecx
                call   GIF_Load
                jc     Error
                mov    T02_MapOffset, edi

                mov    esi, T02_MapOffset       ;Convert texturemap
                mov    edi, SYS_TempBuf
                mov    ebp, OFFSET GIF_Palette
                mov    ecx, 320*200
                call   CN_8_15

                mov    T02_MapWidth, 320
                mov    eax, SYS_TempBuf
                mov    T02_MapOffset, eax

                mov    ecx, 320
                mov    edx, 200
                mov    ebx, 15
                mov    edi, 0
                xor    eax, eax
                call   VBE_FindMode

                call   VBE_SetMode
                jc     Error

                mov    SYS_DBufOffs, 0A0000h

                call   T02_Init


;---- Calculating vertices' 1/Z
                fld1
                fdiv   Vert0.V_Z
                fstp   Vert0.V_InvZ
                fld1
                fdiv   Vert1.V_Z
                fstp   Vert1.V_InvZ
                fld1
                fdiv   Vert2.V_Z
                fstp   Vert2.V_InvZ
                fld1
                fdiv   Vert3.V_Z
                fstp   Vert3.V_InvZ
                fld1
                fdiv   Vert4.V_Z
                fstp   Vert4.V_InvZ
                fld1
                fdiv   Vert5.V_Z
                fstp   Vert5.V_InvZ


;컴컴 Un-comment this when speedtest...
;                jmp    Timing

                mov    esi, OFFSET Tri
                call   T02_Tri

;                mov    esi, OFFSET Tri2
;                call   T02_Tri

                mov    ah, 01h
                int    21h

                jmp    Error





Timing:
                mov    al,00110100b            ;Zero Timer Value
                out    43h,al
                mov    al,0
                out    40h,al
                out    40h,al

                mov    ax, 204h                ;Save old interrupt-address
                mov    bl, 8
                int    31h
                mov    DWORD PTR OLD_INT8, edx ;
                mov    WORD PTR OLD_INT8[4], cx

                mov    ax, 205h
                mov    edx, OFFSET Timer       ;Put new offset
                mov    cx, cs
                int    31h

                mov    Ticks, 0
                mov    Triangles, 0
TimerL:
                mov    esi, OFFSET Tri
                call   T02_Tri

                inc    Triangles

                cmp    Ticks, 18
                jb     TimerL

                mov    ax, 205h                 ;Restore old interrupt
                mov    edx, dword ptr old_INT8
                mov    cx, word ptr old_INT8[4]
                mov    bl, 8
                int    31h







Error:
                mov    ah, 0                    ;Initialize screenmode
                mov    al, 03h                  ;320x200x256
                int    10h                      ;VGA-services


                mov    edx, OFFSET NrString
                mov    ah, 09h                  ;Write string
                int    21h

                mov    eax, Triangles           ;Write number
                call   PD_PDec

                mov    edx, OFFSET EOL          ;New line...
                mov    ah, 09h
                int    21h


                mov    edx, SYS_QuitMsg         ;System quitmsg
                mov    ah, 09h
                int    21h

                mov    ax, 4C00h                ;Halt program
                int    21h
Main          ENDP


Timer         PROC
                push    eax
                inc     Ticks
                mov     al, 20h
                out     20h, al
                pop     eax
                iretd
Timer         ENDP


END           Main