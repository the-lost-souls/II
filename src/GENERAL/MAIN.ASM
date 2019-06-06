;�������������������������� ASSEMBLER SOURCE ��������������������������������
;�Name         : MAIN.ASM
;�Author       : Cyberfish/TLS & Toe/DU
;�Last update  : 10.02.1998
;�Action       :
;�
;�Notes :
;����������������������������������������������������������������������������
.386
.Model flat, C
.code

include w:\general\var.inc

public MN_MainLoop                      ;mainloop-procedure
public ZERO_Object                      ;zeroobject, routine with just a ret
public KEY_OPressed                     ;Keypressed object
public KEY_RPressed                     ;Keypressed routine

public BRK_OFrames      		;Object
public BRK_RFrames

;�������������������������� ASSEMBLER ROUTINE �������������������������������
;�Name         : MN_MainLoop
;�ID           : MN
;�Type         : Procedure
;�Last update  : 10.02.1998
;�Action       : The main loop (including plugins)
;�Optimized    : -
;�
;�Input variables : SYS_OListOffs
;�
;�Notes : List ends with -1 (FFFFFFFFh)
;����������������������������������������������������������������������������

MN_MainLoop   PROC
int 3
MN_MainL:       mov    esi, SYS_OListOffs

MN_OLoop:       mov    eax, DWORD PTR [ esi ]   ;Load objectoffset
                cmp    eax, -1                  ;End of list?
                je     MN_MainL                 ;Done loop
                cmp    eax, 0                   ;Fake object?
                je     MN_FakeObject            ;Dump object

                push   esi                      ;Save list-pointer
                mov    SYS_OOffs, eax           ;Save object's offset





;���� Running plugins...
                xor    ecx, ecx
                lea    ebx, [ eax + 04 ]        ;Point ebx to plugin-list

MN_PluginL:     mov    eax, [ ebx ]             ;Load pluginoffset
                cmp    eax, -1                  ;End of pluginlist?
                je     MN_DonePlugins           ;Done plugins
                cmp    eax, 0                   ;Fake plugin?
                je     MN_FakePlugin            ;Dump plugin

                push   ebx
                push   ecx

                mov    eax, [ ebx + 04 ]        ;Load plugindataoffset
                mov    SYS_POffs, eax           ;Setting up system variables
                mov    eax, SYS_OOffs           ;/
                mov    SYS_PCaller, eax         ;/

		pushad				;Save all registers in case
						;/of C++ calls...

                call   [ ebx ]                  ;Call plugin!

		popad

                pop    ecx
                pop    ebx

MN_FakePlugin:  add    ebx, 8                   ;Next plugin
                inc    ecx
                cmp    ecx, 8
                je     MN_DonePlugins
                jmp    MN_PluginL

MN_DonePlugins:

;���� Done plugins...



                mov    eax, SYS_OOffs           ;Restore objectoffset
                mov    ebx, [ eax ]             ;Load object-type
                call   SYS_ORoutines[ ebx*4 ]
                pop    esi                      ;Restore listpointer

MN_FakeObject:  add    esi, 4
                cmp    SYS_QuitFlag, 0FFFFFFFFh;Test quit-flag
                je     MN_Done                 ;Quit loop!
                jmp    MN_OLoop
MN_Done:

MN_MainLoop   ENDP




;�������������������������� ASSEMBLER ROUTINE �������������������������������
;�Name         : ZERO_Object
;�ID           : ZERO
;�Type         : Procedure (object 0000)
;�Last update  : 10.02.1998
;�Action       : Does nothing. Just returns.
;�Optimized    : -
;�
;�Notes :
;����������������������������������������������������������������������������

ZERO_Object   PROC
                ret
ZERO_Object   ENDP






;�������������������������� ASSEMBLER ROUTINE �������������������������������
;�Name         : KEY_Routine
;�ID           : KEY
;�Type         : Procedure
;�Last update  :
;�Action       : Sets SYS_QuitFlag if keypressed.
;�Optimized    : -
;�
;�Notes :
;����������������������������������������������������������������������������

KEY_OPressed  DD C_KeyPressed                   ;Keypressed
              DD -1                             ;No plugins

KEY_QuitMsg   DB 'KEY: Key pressed. Terminating...', 0Dh, 0Ah, 0, '$'

KEY_RPressed  PROC
                mov    ah, 0Bh                  ;
                int    21h                      ;
                cmp    al, 0                    ;Keypressed?
                jz     KEY_Nothing              ;Nope. Return

                mov    SYS_QuitFlag, -1         ;Set quitflag
                mov    SYS_QuitMsg, OFFSET KEY_QuitMsg

KEY_Nothing:	ret
KEY_RPressed  ENDP



;�������������������������� ASSEMBLER ROUTINE �������������������������������
;�Name         : BRK_Frames
;�ID           : BRK
;�Type         : Procedure
;�Last update  : 10.02.1998
;�Action       : Sets SYS_QuitFlag if keypressed.
;�Optimized    : -
;�
;�Notes :
;����������������������������������������������������������������������������

BRK_OFrames   DD C_BreakFrames                  ;Keypressed
              DD -1                             ;No plugins
	      DD 15 DUP (0)
	      DD 0

BRK_QuitMsg   DB 'BRK: Breaking...', 0Dh, 0Ah, 0, '$'

BRK_RFrames   PROC
		mov	esi, SYS_OOffs
		dec	d[esi + 68]
		jnz	BRK_Nothing

		mov	SYS_QuitFlag, -1
		mov	SYS_QuitMsg, OFFSET BRK_QuitMsg

BRK_Nothing:	ret
BRK_RFrames   ENDP


END