.386
.Model flat, C
.code

include w:\vectors\vectors.inc
include w:\render\render.inc
include w:\general\main.inc
include w:\general\sort.inc
include w:\vectors\lights.inc
include w:\plugins\glow.inc
include w:\general\canvas.inc
include w:\general\2d.inc

public        SYS_ORoutines                     ;The list of objectroutines
public        SYS_PRoutines                     ;The list of part-routines
public        SYS_LRoutines                     ;The list of lights-routines

SYS_ORoutines   DD 0
                DD ZERO_Object                  ;001
                DD KEY_RPressed                 ;002
                DD V01                          ;003
                DD CNV_RNormal_Banked           ;004
                DD V03  			;005
                DD SRT_RReset                   ;006
                DD SRT_RSort                    ;007
                DD SRT_RRun                     ;008
                DD LIT_InitP                    ;009
                DD V04                          ;010
		DD BRK_RFrames			;011
		DD V05 				;012
		DD V06 				;013
		DD V07 				;014
		DD PIC				;015
		DD BLN				;016
		DD FOG				;017
		DD INV				;018


SYS_PRoutines   DD 0				;00
                DD T02_Call 			;01 perspective mapped
                DD T04_Call 			;02 flatfilled
                DD T05_Call 			;03 flat, perspective mapped
		DD T06_Call 			;04 flat, transparent
		DD T07_Call 			;05 linear mapped
		DD T08_Call 			;06 flat, linear mapped
		DD GLW        			;07

SYS_LRoutines   DD 0
                DD L01
                DD L02
		DD L03

END
