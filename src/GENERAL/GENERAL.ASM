;旼컴컴컴컴컴컴컴컴컴컴컴컴 ASSEMBLER SOURCE 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
;쿙ame         : GENERAL.ASM
;쿌uthor       : Cyberfish/TLS & Toe/DU
;쿗ast update  : 22.07.1997
;쿌ction       :
;�
;쿙otes :
;읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
.386
.model flat
.code

include w:\general\var.inc

public D32_Setup
public PD_PDec
public PC_PCount
public PZ_PZero
public PH_PHex
public CN_8_16
public CN_8_15
public DOS_Int
public DOS_es
public DOS_ds
public M44_Mul


;旼컴컴컴컴컴컴컴컴컴컴컴컴 ASSEMBLER ROUTINE 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
;쿙ame         : D32_Setup
;쿔D           : D32
;쿟ype         : Procedure
;쿗ast update  : 06.07.1997
;쿌ction       : Sets up different variables
;쿚ptimized    : -
;�
;쿔nput variables : SYS_TempSize (default 128k)
;�
;쿙otes :
;읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴

D32_Err1      DB 'D32: Error allocating memory for tempbuffer', 0Dh, 0Ah, '$'
D32_Err2      DB 'D32: No coprocessor available', 0Dh, 0Ah, '$'

D32_Setup     PROC

;---- !!!Check for coprocessor here!!!
;---- !!!Check for coprocessor here!!!
;---- !!!Check for coprocessor here!!!

                ffree  st                       ;Freeing all FPU-registers
                ffree  st(1)
                ffree  st(2)
                ffree  st(3)
                ffree  st(4)
                ffree  st(5)
                ffree  st(6)
                ffree  st(7)

                fstcw  FPU_Original             ;Setting controlword-values
                fldcw  FPU_24
                mov    ax, FPU_24
                or     FPU_Round, ax
                or     FPU_Chop, ax
                or     FPU_RoundUp, ax
                or     FPU_RoundDown, ax

                mov    ax, 0EE02h               ;DOS32: address info
                int    31h                      ;ebx = program segment adress

                mov    SYS_ProgBase, ebx
                mov    SYS_buf_es, ax

                and    eax, 0FFFFh
                shl    eax, 4                   ;*16
                sub    eax, ebx
                mov    SYS_buf_flat, eax        ;save near offset to buffer

                mov    edx, SYS_TempSize        ;edx = Blocksize
                mov    ax, 0EE42h
                int    31h
                jc     D32_MemErr

                mov    SYS_TempBuf, edx
                clc                             ;Everything OK.
                ret

D32_MemErr:     mov    SYS_QuitMsg, OFFSET D32_Err1
                stc                             ;Failed!
                ret
D32_Setup     ENDP




;旼컴컴컴컴컴컴컴컴컴컴컴컴 ASSEMBLER ROUTINE 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
;쿙ame         : PD_PDec
;쿔D           : PD
;쿟ype         : Procedure
;쿗ast update  : 29.05.1997
;쿌ction       : Prints the number in eax as unsigned decimal
;쿚ptimized    : -
;�
;쿔nput variables : eax = number
;�
;쿙otes :
;읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴

PD_PDec       PROC
                push   eax
                push   ebx
                push   ecx
                push   edx

                push   DWORD PTR 0FFFFFFFFh     ;Set end-sign

                mov    ecx, 10
PD_1:
                xor    edx, edx
                div    ecx                      ;Get one digit in edx
                add    edx, 30h                 ;Add asciinumber for "0"
                push   edx                      ;Push character
                or     eax, eax                 ;Done with number?
                jnz    PD_1

PD_2:
                pop    edx                      ;Restore character
                cmp    edx, 0FFFFFFFFh          ;End of string?
                je     PD_Done
                mov    eax, 00000200h           ;Print the character in dl
                int    21h
                jmp    PD_2
PD_Done:
                pop    edx
                pop    ecx
                pop    ebx
                pop    eax

                ret
PD_PDec       ENDP






;旼컴컴컴컴컴컴컴컴컴컴컴컴 ASSEMBLER ROUTINE 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
;쿙ame         : PC_PCount
;쿔D           : PC
;쿟ype         : Procedure
;쿗ast update  : 12.07.1997
;쿌ction       : Prints a string, ecx bytes long
;쿚ptimized    : -
;�
;쿔nput variables : [edx] = Offset string
;�                   ecx  = string length
;�
;쿙otes :
;읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴

PC_PCount     PROC
                mov    esi, edx
PC_Print:
                movzx  edx, BYTE PTR [ esi ]
                mov    eax, 00000200h
                int    21h

                inc    esi
                dec    ecx
                jnz    PC_Print

                ret
PC_PCount     ENDP





;旼컴컴컴컴컴컴컴컴컴컴컴컴 ASSEMBLER ROUTINE 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
;쿙ame         : PZ_PZero
;쿔D           : PZ
;쿟ype         : Procedure
;쿗ast update  : 29.05.1997
;쿌ction       : Prints a string terminated with 0
;쿚ptimized    : -
;�
;쿔nput variables : [edx] = Offset string
;�
;쿙otes :
;읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴

PZ_PZero      PROC
                mov    esi, edx
PZ_Print:
                movzx  edx, BYTE PTR [ esi ]
                or     edx, edx
                jz     PZ_Done

                mov    eax, 00000200h           ;Print character
                int    21h

                inc    esi
                dec    ecx
                jnz    PZ_Print

PZ_Done:

                ret
PZ_PZero      ENDP





;旼컴컴컴컴컴컴컴컴컴컴컴컴 ASSEMBLER ROUTINE 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
;쿙ame         : PH_PHex
;쿔D           : PH
;쿟ype         : Procedure
;쿗ast update  : 29.05.1997
;쿌ction       : Prints the number in eax as hexadecimal
;쿚ptimized    : -
;�
;쿔nput variables : eax = number
;�
;쿙otes :
;읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴

PH_HexString  DB '0123456789ABCDEF'

PH_PHex       PROC
                push   eax
                push   ebx
                push   ecx
                push   edx

                push   DWORD PTR 0FFFFFFFFh

PH_1:
                shrd   edx, eax, 4
                shr    eax, 4

                shr    edx, 28
                movzx  edx, BYTE PTR PH_HexString[ edx ]
                push   edx
                or     eax, eax
                jnz    PH_1

PH_2:
                pop    edx
                cmp    edx, 0FFFFFFFFh
                je     PH_Done
                mov    eax, 00000200h
                int    21h
                jmp    PH_2
PH_Done:
                pop    edx
                pop    ecx
                pop    ebx
                pop    eax

                ret
PH_PHex       ENDP






;旼컴컴컴컴컴컴컴컴컴컴컴컴 ASSEMBLER ROUTINE 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
;쿙ame         : CN_8_16
;쿔D           : CN
;쿟ype         : Procedure
;쿗ast update  : 29.05.1997
;쿌ction       : Converts an 8 bit picture into a 16 bit hicolor picture
;쿚ptimized    : -
;�
;쿔nput variables : [esi] = picture to convert
;�                  [edi] = where to store converted picture
;�                  [ebp] = 256-color palette for 8-bit picture
;�                   ecx  = size of picture (in pixels)
;�
;쿙otes :
;읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
CN_8_16       PROC

CN_Loop16:
                movzx  eax, BYTE PTR [ esi ]
                lea    ebx, [ eax + eax*2 ]
                add    ebx, ebp

                movzx  eax, BYTE PTR [ ebx + 2 ]         ;Load R
                shr    eax, 1
                movzx  edx, BYTE PTR [ ebx + 1 ]         ;Load G
                shl    edx, 5
                or     eax, edx
                movzx  edx, BYTE PTR [ ebx + 0 ]         ;Load B
                shr    edx, 1
                shl    edx, 11
                or     eax, edx
                mov    [ edi ], ax

                inc    esi
                add    edi, 2

                dec    ecx
                jnz    CN_Loop16

                ret
CN_8_16       ENDP


;旼컴컴컴컴컴컴컴컴컴컴컴컴 ASSEMBLER ROUTINE 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
;쿙ame         : CN_8_15
;쿔D           : CN
;쿟ype         : Procedure
;쿗ast update  : 22.07.1997
;쿌ction       : Converts an 8 bit picture into a 15 bit hicolor picture
;쿚ptimized    : -
;�
;쿔nput variables : [esi] = picture to convert
;�                  [edi] = where to store converted picture
;�                  [ebp] = 256-color palette for 8-bit picture
;�                   ecx  = size of picture (in pixels)
;�
;쿙otes :
;읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
CN_8_15       PROC

CN_Loop15:
                movzx  eax, BYTE PTR [ esi ]             ;Load pixel
                lea    ebx, [ eax + eax*2 ]              ;Mul by 3
                add    ebx, ebp                          ;[ebx] = color

                movzx  eax, BYTE PTR [ ebx + 2 ]         ;Red -> bit 0-4
                shr    eax, 1                            ;/


                movzx  edx, BYTE PTR [ ebx + 1 ]         ;Green -> bit 5-9
                shr    edx, 1                            ;/
                shl    edx, 5                            ;/
                or     eax, edx                          ;/

                movzx  edx, BYTE PTR [ ebx + 0 ]         ;Blue -> bit 10-15
                shr    edx, 1                            ;/
                shl    edx, 10                           ;/
                or     eax, edx                          ;/
                mov    [ edi ], ax                       ;/

                inc    esi
                add    edi, 2

                dec    ecx
                jnz    CN_Loop15

                ret
CN_8_15       ENDP



;------------------------------------------------------------
; Procudure to call a DOS interrupt.
;
; Expects the intrrupt number pushed on the stack.
;
;  e.g    push  10h
;         call  DOSinterrupt
;         jc   error
; Real mode ES and DS registers are passed via varibles Real_ES and Real_DS.
;
;
;------------------------------------------------------------
DOSinterrupt PROC
        lea     esp, [esp - 12 ]        ;ignore ss, sp, cs, ip, fs, gs
        push    DOS_segs                ;push DS and ES

        pushfw                          ;Save flags
        pushad                          ;Save extende regeisters

        mov     edi, esp
        mov     ax, 0300h
        xor     cx, cx
        movzx   ebx, byte ptr [esp+36h] ;Get int number from stack param
        int     31h                     ;Emulate Real Mode Interrupt

        popad
        popfw
        pop     [DOS_segs]              ;get DS and ES

        lea     esp,[esp+12]            ;Ignore SS,SP,CS,IP,FS,GS
        ret     4                       ;return ingnoring parameter
DOSinterrupt ENDP



;旼컴컴컴컴컴컴컴컴컴컴컴컴 ASSEMBLER ROUTINE 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
;쿙ame         : DOS_Int
;쿔D           : DOS
;쿟ype         : Procedure
;쿗ast update  : 29.05.1997
;쿌ction       : Runs a realmode interrupt-callback
;쿚ptimized    : -
;�
;쿔nput variables : [esp] = interrupt number
;�                  es    = DOS_es
;�                  ds    = DOS_ds
;�                  else registers as a normal call
;�
;쿙otes : If the interrupt requires es/ds as input, this must be set in
;쿏OS_es og DOS_ds (both words).
;읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴

DOS_segs LABEL DWORD
DOS_es        DW  ?
DOS_ds        DW  ?

DOS_Int       PROC
                lea     esp, [esp - 12]         ;Ignore ss, sp, cs, ip, fs, gs
                push    [DOS_segs]              ;push DS and ES

                pushfw                          ;Save flags
                pushad                          ;Save extended registers

                mov     edi, esp
                mov     ax, 0300h               ;DOS32: Simulate a rm-int
                xor     cx, cx
                movzx   ebx, BYTE PTR [esp + 54];Get int number
                int     31h

                popad
                popfw
                pop     [DOS_segs]              ;get DS and ES

                lea     esp,[esp + 12]          ;Ignore SS,SP,CS,IP,FS,GS
                ret     4                       ;dump input-parameter
DOS_Int       ENDP


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
