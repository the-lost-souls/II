;旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴 ASSEMBLER SOURCE 컴컴컴컴컴컴컴컴컴컴컴컴컴컴
;쿙ame         : T01_TEST.ASM
;쿌uthor       : Cyberfish/TLS & Toe/DU
;쿗ast update  : 12.07.97
;쿌ction       : Tests T01 and T01_Org
;�
;쿙otes : Just link with T01.OBJ or T01_ORG.OBJ
;읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
.386
.Model Flat
.code
.stack 100h

include w:\general\main.inc
include w:\general\var.inc
include w:\general\general.inc
include w:\gfx\gif.inc
include w:\render\render.inc
include w:\general\mem.inc





Tri            DD 0
               DD OFFSET Vert0
               DD OFFSET Vert1
               DD OFFSET Vert2
               DD 0                             ;Material
               DD 0                             ;R, G, B, 0
               DD 0.0                        ;U0
               DD 0.0                           ;V0
               DD 320.0                         ;U1
               DD 0.0                           ;V1
               DD 0.0                          ;U2
               DD 200.0                          ;V2

Vert0          DD 0
               DD 0
               DD 20.0                          ;X, Y, Z
               DD 0.0                           ;1/Z
               DD 0, 0, 0                       ;X, Y, Z rotated
               DD 0.0                        ;2D-X
               DD 0.0                ;2D-Y

Vert1          DD 0, 0, 20.0                   ;X, Y, Z
               DD 0.0                           ;1/Z
               DD 0, 0, 0                       ;X, Y, Z rotated
               DD 0.0                  ;2D-X
               DD 50.0                                    ;2D-Y

Vert2          DD 0, 0, 10.0                    ;X, Y, Z
               DD 0.0                           ;1/Z
               DD 0, 0, 0                       ;X, Y, Z rotated
               DD 50.0                           ;2D-X
               DD 0.0                         ;2D-Y


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

                mov    eax, -500*1024
                call   MEM_Init


                mov    ah, 0                    ;Initialize screenmode
                mov    al, 13h                  ;320x200x256
                int    10h                      ;VGA-services

                mov    edx, OFFSET Gif          ;Load texturemap
                xor    esi, esi
                xor    ecx, ecx
                call   GIF_Load
                jc     Error
                mov    T01_MapOffset, edi
                mov    T01_MapWidth, 320

                mov    dx,3C8h                  ;Port to send out startingcolor
                mov    al, 0                    ;Starting color = 0
                out    dx, al                   ;Send it!
                inc    dx                       ;Next port = RGB values
                mov    ecx, 768
                mov    esi, OFFSET GIF_Palette  ;Point to palette
                rep    outsb                    ;Set palette

                fld1
                fdiv   Vert0.V_Z
                fstp   Vert0.V_InvZ
                fld1
                fdiv   Vert1.V_Z
                fstp   Vert1.V_InvZ
                fld1
                fdiv   Vert2.V_Z
                fstp   Vert2.V_InvZ

                mov    SYS_DBufLine, 320
                mov    SYS_XRes, 320


;컴컴 Un-comment this when speedtest...
;                jmp    Timing

                mov    esi, OFFSET Tri
                call   T01_Tri

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
                call   T01_Tri

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