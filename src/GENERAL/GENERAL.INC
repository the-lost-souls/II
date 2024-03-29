;旼컴컴컴컴컴컴컴컴컴컴컴컴 ASSEMBLER INCLUDEFILE 컴컴컴컴컴컴컴컴컴컴컴컴컴�
;쿙ame         : GENERAL.INC
;쿌uthor       : Cyberfish/TLS & Toe/DU
;쿗ast update  : 28.09.97
;쿌ction       : Div general procedures.
;�
;쿙otes :
;읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴

extrn  D32_Setup	    :PROC        ;Sets up several "SYS"-variables
extrn  PD_PDec		    :PROC        ;Prints an usigned decimal
extrn  PC_PCount            :PROC        ;Prints a string ecx bytes long
extrn  PZ_PZero             :PROC        ;Prints a 0-terminated string
extrn  PH_PHex              :PROC        ;Prints a number in hex
extrn  CN_8_16              :PROC        ;Converts an 8-bit picture to 16-bit
extrn  CN_8_15              :PROC        ;Converts an 8-bit picture to 15-bit
extrn  DOS_Int              :PROC        ;Simulates a realmode int-call
extrn  DOS_es               :WORD        ;/es and ds must be passed here
extrn  DOS_ds		    :WORD        ;/
extrn  M44_Mul              :PROC        ;Multiplies to 4x4 matrices




;旼컴컴컴컴컴컴컴컴컴컴컴컴 ASSEMBLER ROUTINE 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
;쿙ame         : PS_PString
;쿔D           : PS
;쿟ype         : macro
;쿗ast update  : 20.05.1997
;쿌ction       : Prints a string, terminated by '$'
;쿚ptimized    : -
;�
;쿔nput variables : String
;�
;쿝egisters changed : eax, edx
;�
;쿙otes : Resolution has to be given in _BYTES_, not pixels.
;읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴

PS_PString    MACRO String
                mov    ax, 0900h
                mov    edx, OFFSET String
                int    21h
              ENDM
