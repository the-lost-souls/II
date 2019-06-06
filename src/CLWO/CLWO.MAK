#######################################################################
# Makefile for all CLWO.EXE
#
# Code:
#	Cyberfish/ TLS
#
#######################################################################



#######################################################################
#Programs used:

CPP = wpp386.exe
EXE = wlink.exe

#######################################################################
#Program flags

CPPFLAGS = -d2
EXEFLAGS = debug dwarf
CPPINCLUDE = -ic:\code\prog\watcom11\h


#####################################################################

clwo.exe: clwo.obj div.obj savelwo.obj
       wlink $(EXEFLAGS) @clwo.lnk

clwo.obj: clwo.cpp div.h
	$(CPP) $(CPPFLAGS) $(CPPINCLUDE) $*.cpp

div.obj: div.cpp div.h
	$(CPP) $(CPPFLAGS) $(CPPINCLUDE) $*.cpp

savelwo.obj: savelwo.cpp savelwo.h div.h
	$(CPP) $(CPPFLAGS) $(CPPINCLUDE) $*.cpp

