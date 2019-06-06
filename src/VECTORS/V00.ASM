;旼컴컴컴컴컴컴컴컴컴컴컴컴 ASSEMBLER SOURCE 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
;쿙ame         : V00.ASM
;쿌uthor       : Cyberfish/TLS & Toe/DU
;쿗ast update  : 25.09.97
;쿌ction       : Loads a vectorobject.
;�
;쿙otes :
;읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
.386
.model flat
.code

include w:\general\var.inc
include w:\general\mem.inc
include w:\gfx\gif.inc
include w:\gfx\tga.inc

public V00_Load


Main_Chunk    STRUC
Main_ID         DD ?
Main_Size       DD ?
Main_Version    DD ?
Main_Type       DD ?
Main_FSize      DD ?
Main_Check      DD ?
Main_Vertices   DD ?
Main_Faces      DD ?
Main_Materials  DD ?
              ENDS

Handle          DW 0
FileBuffer      DD 0
Materials       DD 0
MapDisplacement DD 0
Object          DD 0
PartType        DD 0
FPDump          DD 0
_255            DD 255.0

PartLookup      DD 0
                DD 0                            ;Perspective mapping
                DD 0
                DD C_T02                        ;Flat-filled
                DD 0
                DD C_T04
                DD 0
                DD 0
                DD 0
                DD 0
                DD C_T05
                DD 0

;旼컴컴컴컴컴컴컴컴컴컴컴컴 ASSEMBLER ROUTINE 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
;쿙ame         : V00_Load
;쿔D           : V00
;쿟ype         : Procedure
;쿗ast update  : 25.09.97
;쿌ction       : Loads a vectorobject (.bin-file)
;쿚ptimized    : -
;�
;쿔nput variables : [edx]  = filename
;�                   esi   = offset in file to load from
;�
;쿝egisters changed : All!
;�
;쿙otes : FPU stack must be empty.
;읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴

V00_Load      PROC

;---- Opening and verifying file...
                mov    ah, 3Dh                  ;Open file
                mov    al, 0                    ;Just read access
                int    21h                      ;edx = offset filename
                jc     Error1                   ;Couldn't open file

                mov    Handle, ax               ;Save filehandle
                lea    edx, [esi + 66]          ;Dump textheader
                mov    ah, 42h                  ;Move filepointer
                mov    al, 0                    ;..from start of file
                mov    bx, Handle               ;Load filehandle
                int    21h
                jc     Error2                   ;Couldn't move filepointer

                mov    edx, SYS_TempBuf
                mov    ecx, 9*4                 ;Load mainblock
                mov    ah, 3Fh
                mov    bx, Handle
                int    21h
                jc     Error3                   ;Error reading file

                mov    ebp, SYS_TempBuf

                cmp    [ebp].Main_ID, 'NIAM'    ;= 'MAIN'
                jne    Error4



;---- Reading entire file into memory...
                mov    eax, [ebp].Main_FSize    ;Allocate temp. memory
                call   MEM_Alloc
                jc     Error0
                mov    FileBuffer, edx

                mov    edx, esi
                mov    ah, 42h                  ;Move filepointer
                mov    al, 0                    ;..from start of file
                mov    bx, Handle               ;Load filehandle
                int    21h
                jc     Error2                   ;Couldn't move filepointer

                mov    edx, FileBuffer          ;Read entire file...
                mov    ecx, [ebp].Main_FSize    ;
                mov    ah, 3Fh
                mov    bx, Handle
                int    21h
                jc     Error3                   ;Error reading file

                mov    ah, 3Eh                  ;Close filehandle
                mov    bx, Handle               ;Load filehandle
                int    21h
                jc     Error5

;---- Testing checksum
                xor    eax, eax
                mov    ecx, [ebp].Main_FSize
                mov    esi, FileBuffer
                mov    DWORD PTR [esi + 86], 0  ;Zero checksum

CheckSum:
                movzx  edx, BYTE PTR [esi]
                add    eax, edx
                inc    esi
                dec    ecx
                jnz    CheckSum

                cmp    eax, [ebp].Main_Check
                jne    Error6                   ;Invalid checksum


;---- Allocating memory for object...
                mov    eax, [ebp].Main_Vertices ;Number of vertices
                mov    edx, V_Size              ;size each vertice
                imul   eax, edx                 ;/
                mov    ebx, [ebp].Main_Faces    ;Number of faces
                mov    edx, F_Size              ;size each face
                imul   ebx, edx                 ;/
                mov    ecx, [ebp].Main_Materials;Number of materials
                mov    Materials, ecx
                mov    edx, M_Size              ;Size in bytes each material
                imul   ecx, edx

                lea    eax, [eax + ebx + 256]   ;256 bytes for header
                add    eax, ecx                 ;eax = memory needed
                call   MEM_Alloc
                jc     Error7
                mov    Object, edx              ;Save offset to header

                mov    edi, Object              ;[edi] = object header
                mov    [edi].H_Memory, eax      ;Save blocksize

                xor    eax, eax                 ;Blank buffer
                mov    ecx, [edi].H_Memory
                rep    stosb

;---- Setup object header...
                mov    edi, Object
                                                ;eax = memory used
                mov    ebx, [ebp].Main_Vertices ;ebx = number of vertices
                mov    ecx, [ebp].Main_Faces    ;ecx = number of faces
                mov    edx, [ebp].Main_Type     ;edx = object's type

                mov    eax, PartLookup[edx*4]   ;Load parttype
                mov    PartType, eax

                mov    [edi].H_Vertices, ebx
                mov    [edi].H_Faces, ecx
                mov    [edi].H_Type, edx

                lea    ebp, [edi + 256]         ;ebp = Offset vertices
                mov    [edi].H_VertOffs, ebp    ;Save offset

                mov    ebp, [edi].H_Vertices
                mov    edx, V_Size              ;bytes each vertice
                imul   ebp, edx                 ;/
                add    ebp, [edi].H_VertOffs    ;eax = Offset faces
                mov    [edi].H_FaceOffs, ebp    ;Save offset



;---- Setting up vertices...
                mov    edi, Object
                mov    ebp, [edi].H_VertOffs
                mov    esi, FileBuffer
                add    esi, 66

SearchVERT:
                cmp    [esi], 'TREV'            ;Search for [VERT] chunk
                je     FoundVERT
                mov    eax, [esi + 4]
                lea    esi, [esi + eax + 8]     ;Next chunk
                jmp    SearchVERT
FoundVERT:

                mov    ecx, [edi].H_Vertices
                add    esi, 8
SetupVERT:
                mov    eax, [esi + 0]           ;Load X
                mov    ebx, [esi + 4]           ;Load Y
                mov    edx, [esi + 8]           ;Load Z
                mov    [ebp].V_X, eax           ;Save X
                mov    [ebp].V_Y, ebx           ;Save Y
                mov    [ebp].V_Z, edx           ;Save Z

                mov    [ebp].V_RFlag, 0

                add    esi, 12
                add    ebp, V_Size
                dec    ecx
                jnz    SetupVERT


;---- Setting up faces...
                mov    edi, Object
                mov    ebp, [edi].H_FaceOffs
                mov    esi, FileBuffer
                add    esi, 66

SearchFACE:
                cmp    [esi], 'SLOP'            ;Search for [POLS] chunk
                je     FoundFACE
                mov    eax, [esi + 4]
                lea    esi, [esi + eax + 8]     ;Next chunk
                jmp    SearchFACE
FoundFACE:

                mov    ecx, [edi].H_Faces       ;ecx = number of faces
                add    esi, 8                   ;dump chunkheader
SetupFACE:
                mov    eax, [esi + 36]          ;Load material
                mov    [ebp].F_Material, eax    ;Save...

                mov    eax, [esi + 0]           ;Load V0
                mov    ebx, [esi + 4]           ;Load V1
                mov    edx, [esi + 8]           ;Load V2
                mov    [ebp].F_V0Offs, eax      ;Save V0
                mov    [ebp].F_V1Offs, ebx      ;Save V1
                mov    [ebp].F_V2Offs, edx      ;Save V2

                                                ;Calculating normalvector

                mov    edx, V_Size
                imul   eax, edx                 ;Mul by V_Size
                imul   ebx, edx                 ;/
                imul   edx, [esi + 8]           ;/
                add    eax, [edi].H_VertOffs    ;Linear pointers to vertices
                add    ebx, [edi].H_VertOffs    ;/
                add    edx, [edi].H_VertOffs    ;/


                                                ;Creating coordinates into
                                                ;vectors with v0 as origo

                                      ;st0  st1  st2  st3  st4  st5  st6  st7
                fld    [ebx].V_X      ;X1
                fsub   [eax].V_X      ;VX1
                fld    [ebx].V_Y      ;Y1   VX1
                fsub   [eax].V_Y      ;VY1  VX1
                fld    [ebx].V_Z      ;Z1   VY1  VX1
                fsub   [eax].V_Z      ;VZ1  VY1  VX1

                fld    [edx].V_X      ;X2   VZ1  VY1  VX1
                fsub   [eax].V_X      ;VX2  VZ1  VY1  VX1
                fld    [edx].V_Y      ;Y2   VX2  VZ1  VY1  VX1
                fsub   [eax].V_Y      ;VY2  VX2  VZ1  VY1  VX1
                fld    [edx].V_Z      ;Z2   VY2  VX2  VZ1  VY1  VX1
                fsub   [eax].V_Z      ;VZ2  VY2  VX2  VZ1  VY1  VX1

                fld    st(4)          ;VY1  VZ2  VY2  VX2  VZ1  VY1  VX1
                fmul   st, st(1)      ;Y1Z2 VZ2  VY2  VX2  VZ1  VY1  VX1
                fld    st(4)          ;VZ1  Y1Z2 VZ2  VY2  VX2  VZ1  VY1  VX1
                fmul   st, st(3)      ;Z1Y2 Y1Z2 VZ2  VY2  VX2  VZ1  VY1  VX1
                fsubp  st(1), st      ;NVX  VZ2  VY2  VX2  VZ1  VY1  VX1
                fstp   [ebp].F_NVX    ;VZ2  VY2  VX2  VZ1  VY1  VX1

                fld    st(3)          ;VZ1  VZ2  VY2  VX2  VZ1  VY1  VX1
                fmul   st, st(3)      ;Z1X2 VZ2  VY2  VX2  VZ1  VY1  VX1
                fld    st(6)          ;VX1  Z1X2 VZ2  VY2  VX2  VZ1  VY1  VX1
                fmul   st, st(2)      ;X1Z2 Z1X2 VZ2  VY2  VX2  VZ1  VY1  VX1
                fsubp  st(1), st      ;NVY  VZ2  VY2  VX2  VZ1  VY1  VX1
                fstp   [ebp].F_NVY    ;VZ2  VY2  VX2  VZ1  VY1  VX1

                fld    st(5)          ;VX1  VZ2  VY2  VX2  VZ1  VY1  VX1
                fmul   st, st(2)      ;X1Y2 VZ2  VY2  VX2  VZ1  VY1  VX1
                fld    st(5)          ;VY1  X1Y2 VZ2  VY2  VX2  VZ1  VY1  VX1
                fmul   st, st(4)      ;Y1X2 X1Y2 VZ2  VY2  VX2  VZ1  VY1  VX1
                fsubp  st(1), st      ;NVZ  VZ2  VY2  VX2  VZ1  VY1  VX1
                fstp   [ebp].F_NVZ    ;VZ2  VY2  VX2  VZ1  VY1  VX1

                ffree  st(0)
                ffree  st(1)
                ffree  st(2)
                ffree  st(3)
                ffree  st(4)
                ffree  st(5)          ;empty


                                      ;Getting normalvector's length
                fld    [ebp].F_NVX    ;NVX
                fmul   st, st         ;NVX2
                fld    [ebp].F_NVY    ;NVY  NVX2
                fmul   st, st         ;NVY2 NVX2
                fld    [ebp].F_NVZ    ;NVZ  NVY2 NVX2
                fmul   st, st         ;NVZ2 NVY2 NVX2
                faddp  st(2)          ;
                faddp  st(1)          ;
                fsqrt                 ;|NV|
                fld1                  ;1    |NV|
                fdiv   st, st(1)      ;k    |NV|
                ffree  st(1)          ;k

                                      ;Making normalvector unit length
                fld    st             ;k    k
                fmul   [ebp].F_NVX    ;NVX  k
                fstp   [ebp].F_NVX    ;k
                fld    st             ;k    k
                fmul   [ebp].F_NVY    ;NVY  k
                fstp   [ebp].F_NVY    ;k
                fmul   [ebp].F_NVZ    ;NVZ
                fstp   [ebp].F_NVZ    ;

                                                ;Copy mappingcoordinates
                mov    eax, [esi + 12]          ;Load U0
                mov    ebx, [esi + 16]          ;Load V0
                mov    [ebp].F_U0, eax          ;Save U0
                mov    [ebp].F_V0, ebx          ;Save V0
                mov    eax, [esi + 20]          ;Load U1
                mov    ebx, [esi + 24]          ;Load V1
                mov    [ebp].F_U1, eax          ;Save U1
                mov    [ebp].F_V1, ebx          ;Save V1
                mov    eax, [esi + 28]          ;Load U2
                mov    ebx, [esi + 32]          ;Load V2
                mov    [ebp].F_U2, eax          ;Save U2
                mov    [ebp].F_V2, ebx          ;Save V2


                mov    eax, PartType
                mov    [ebp].F_Type, eax        ;Perspective tmapped, 15bit

                add    ebp, F_Size
                add    esi, 40
                dec    ecx
                jnz    SetupFACE

;---- Setting up materials
                                                ;[ebp] = after faces
                mov    ebx, SYS_Materials       ;Find end of materiallist
                xor    eax, eax
SearchFree:
                cmp    DWORD PTR [ebx], 0       ;Search for free records
                je     FoundFree
                add    ebx, 4
                inc    eax
                jmp    SearchFree
FoundFree:
                mov    MapDisplacement, eax


                mov    esi, FileBuffer          ;Find [MATR] chunk
                add    esi, 66
SearchMATR:
                cmp    [esi], 'RTAM'            ;Search for [MATR] chunk
                je     FoundMATR
                mov    eax, [esi + 4]
                lea    esi, [esi + eax + 8]     ;Next chunk
                jmp    SearchMATR
FoundMATR:

                mov    ecx, [esi + 4]           ;ecx = chunksize
                add    esi, 8                   ;Dump chunkheader
                add    ecx, esi                 ;ecx = end of MATR-chunk


SetupMATR:
                cmp    [esi], 'CPSM'            ;[MSPC] subchunk?
                jne    DumpMSPC

                movzx  eax, b[esi + 8]          ;Load and store R
                mov    FPDump, eax
                fild   FPDump
                fdiv   _255
                fstp   [ebp].M_R

                movzx  eax, b[esi + 9]          ;Load and store G
                mov    FPDump, eax
                fild   FPDump
                fdiv   _255
                fstp   [ebp].M_G

                movzx  eax, b[esi + 10]         ;Load and store B
                mov    FPDump, eax
                fild   FPDump
                fdiv   _255
                fstp   [ebp].M_B

                mov    eax, [esi + 11]          ;Luminosity
                mov    [ebp].M_Luminosity, eax
                mov    eax, [esi + 15]          ;Diffuse
                mov    [ebp].M_Diffuse, eax
                mov    eax, [esi + 19]          ;Transparency
                mov    [ebp].M_Transparency, eax

                xor    eax, eax                 ;ebx = counter
NameLength:
                cmp    BYTE PTR [esi + eax + 23], 0  ;end of filenamestring?
                je     DoneNameLength
                inc    eax
                jmp    NameLength
DoneNameLength:                                 ;ebx = length of string
                cmp    eax, 0                   ;No string?
                je     NoTexture


                lea    edx, [esi + 23]          ;[edx] = filename
                                                ;Checking fileextension...
                cmp    [edx + eax - 4], 'AGT.'  ;TGA?
                je     LoadTGA
;                cmp    [edx + eax - 5], 'FIG.'  ;GIF?
;                je     LoadGIF
                jmp    Error8
LoadTGA:


                                                ;Loading texture (TGA)
                push   esi
                push   ecx
                push   ebx
                push   ebp
                                                ;[edx] = filename
                xor    ecx, ecx                 ;Read entire file...
                xor    esi, esi                 ;...from the beginning
                call   TGA_Load

                pop    ebp
                pop    ebx
                pop    ecx
                pop    esi

                mov    [ebp].M_MapOffset, edi   ;Save texture's offset

                jc     Error7                   ;Error loading map?

                mov    eax, TGA_Width
                mov    [ebp].M_MapWidth, eax    ;Save texture's width
                mov    eax, TGA_Heigth
                mov    [ebp].M_MapHeigth, eax   ;Save texture's heigth
                jmp    NoTexture



NoTexture:

                mov    [ebx], ebp               ;Save offset to material
                add    ebx, 4
                add    ebp, M_Size              ;Next material

DumpMSPC:
                mov    eax, [esi + 4]           ;eax = subchunksize
                lea    esi, [esi + eax + 8]     ;Dump subchunk + subchunkhead

                cmp    esi, ecx                 ;End of MATR-chunk?
                jb     SetupMATR



;---- Calculating "vertice" normalvectors...
                mov    edi, Object
                mov    ebp, [edi].H_VertOffs    ;Offset vertices
                xor    edx, edx
VertLoop:
                mov    esi, [edi].H_FaceOffs    ;Offset faces
                mov    ecx, [edi].H_Faces
                                                ;st0  st1  st2
                fldz                            ;NVZ
                fldz                            ;NVY  NVZ
                fldz                            ;NVX  NVY  NVZ
FaceLoop:
                cmp    [esi].F_V0Offs, edx
                je     AddNormal
                cmp    [esi].F_V1Offs, edx
                je     AddNormal
                cmp    [esi].F_V2Offs, edx
                je     AddNormal
                jmp    DumpNormal
AddNormal:
                fadd   [esi].F_NVX
                fstp   st(3)
                fadd   [esi].F_NVY
                fstp   st(3)
                fadd   [esi].F_NVZ
                fstp   st(3)

DumpNormal:
                add    esi, F_Size
                dec    ecx
                jnz    FaceLoop


                fld    st(2)                    ;NVZ  NVX  NVY  NVZ
                fmul   st, st                   ;NVZ2 NVX  NVY  NVZ
                fld    st(2)                    ;NVY  NVZ2 NVX  NVY  NVZ
                fmul   st, st                   ;NVY2 NVZ2 NVX  NVY  NVZ
                fld    st(2)                    ;NVX  NVY2 NVZ2 NVX  NVY  NVZ
                fmul   st, st                   ;NVX2 NVY2 NVZ2 NVX  NVY  NVZ

                faddp  st(2), st                ;NVY2 NVZ2 NVX  NVY  NVZ
                faddp  st(1), st                ;NVZ2 NVX  NVY  NVZ
                fsqrt                           ;|NV| NVX  NVY  NVZ
                fld1                            ;1    |NV| NVX  NVY  NVZ
                fdiv   st, st(1)                ;k    |NV| NVX  NVY  NVZ

                fld    st                       ;k    k    |NV| NVX  NVY  NVZ
                fmul   st, st(3)
                fstp   [ebp].V_NVX
                fld    st                       ;k    k    |NV| NVX  NVY  NVZ
                fmul   st, st(4)
                fstp   [ebp].V_NVY
                fmul   st, st(4)                ;k    |NV| NVX  NVY  NVZ
                fstp   [ebp].V_NVZ              ;|NV| NVX  NVY  NVZ

                ffree  st
                ffree  st(1)
                ffree  st(2)
                ffree  st(3)

                add    ebp, V_Size
                inc    edx
                cmp    edx, [edi].H_Vertices
                jb     VertLoop


;---- Converting all vertice pointers (in faces) to offsets...
                mov    edi, Object
                mov    esi, [edi].H_FaceOffs
                mov    ecx, [edi].H_Faces
                mov    ebx, MapDisplacement
ConvertFaces:
                mov    eax, [esi].F_Material
                cmp    ax, 0
                jg     MatOK
                mov    eax, 0
MatOK:
                add    eax, ebx
                mov    [esi].F_Material, eax

                mov    edx, V_Size

                mov    eax, [esi].F_V0Offs
                imul   eax, edx
                add    eax, [edi].H_VertOffs
                mov    [esi].F_V0Offs, eax

                mov    eax, [esi].F_V1Offs
                imul   eax, edx
                add    eax, [edi].H_VertOffs
                mov    [esi].F_V1Offs, eax

                mov    eax, [esi].F_V2Offs
                imul   eax, edx
                add    eax, [edi].H_VertOffs
                mov    [esi].F_V2Offs, eax

                mov    eax, [esi].F_Material
                shl    eax, 2
                add    eax, SYS_Materials
                mov    eax, [eax]               ;eax = offset material

                fild   [eax].M_MapWidth
                fild   [eax].M_MapHeigth
                fld1
                fsub   st(1), st
                fsubp  st(2), st

                fld    [esi].F_U0
                fmul   st, st(2)
                fstp   [esi].F_U0
                fld    [esi].F_V0
                fmul   st, st(1)
                fstp   [esi].F_V0
                fld    [esi].F_U1
                fmul   st, st(2)
                fstp   [esi].F_U1
                fld    [esi].F_V1
                fmul   st, st(1)
                fstp   [esi].F_V1
                fld    [esi].F_U2
                fmul   st, st(2)
                fstp   [esi].F_U2
                fld    [esi].F_V2
                fmul   st, st(1)
                fstp   [esi].F_V2

                ffree  st(1)
                ffree  st

                add    esi, F_Size
                dec    ecx
                jnz    ConvertFaces

                mov    edi, Object

                clc
                ret
V00_Load      ENDP





ErrStr1         DB 'VL: Error opening file', 0Dh, 0Ah, '$'
ErrStr2         DB 'VL: Error moving filepointer', 0Dh, 0Ah, '$'
ErrStr3         DB 'VL: Error reading file', 0Dh, 0Ah, '$'
ErrStr4         DB 'VL: Unable to find [MAIN] chunk', 0Dh, 0Ah, '$'
ErrStr5         DB 'VL: Unable to close file', 0Dh, 0Ah, '$'
ErrStr6         DB 'VL: Corrupt file (checksum failure)', 0Dh, 0Ah, '$'
ErrStr8         DB 'VL: Error loading texture. Unknown picture format', 0Dh, 0Ah, '$'

Error0:                                         ;General errror (closing file)
                mov    ah, 3Eh                  ;Close filehandle
                mov    bx, Handle               ;Load filehandle
                int    21h
                jc     Error5
                stc
                ret

Error1:
                mov    SYS_QuitMsg, OFFSET ErrStr1
                stc
                ret
Error2:
                mov    ah, 3Eh                  ;Close filehandle
                mov    bx, Handle               ;Load filehandle
                int    21h
                jc     Error5
                mov    SYS_QuitMsg, OFFSET ErrStr2
                stc
                ret
Error3:
                mov    ah, 3Eh                  ;Close filehandle
                mov    bx, Handle               ;Load filehandle
                int    21h
                jc     Error5
                mov    SYS_QuitMsg, OFFSET ErrStr3
                stc
                ret
Error4:
                mov    ah, 3Eh                  ;Close filehandle
                mov    bx, Handle               ;Load filehandle
                int    21h
                jc     Error5
                mov    SYS_QuitMsg, OFFSET ErrStr4
                stc
                ret
Error5:
                mov    SYS_QuitMsg, OFFSET ErrStr5
                stc
                ret
Error6:
                mov    SYS_QuitMsg, OFFSET ErrStr6
                stc
                ret
Error7:
                stc
                ret
Error8:
                mov    SYS_QuitMsg, OFFSET ErrStr8
                stc
                ret

END
