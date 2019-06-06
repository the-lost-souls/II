#######################################################################
# Makefile for all CLWS.EXE
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

clws.exe: clws.obj savelws.obj div.obj savelwo.obj
	wlink $(EXEFLAGS) @clws.lnk

clws.obj: clws.cpp savelws.h div.h
	$(CPP) $(CPPFLAGS) $(CPPINCLUDE) $*.cpp

div.obj: div.cpp div.h
	$(CPP) $(CPPFLAGS) $(CPPINCLUDE) $*.cpp

savelws.obj: savelws.cpp savelws.h div.h
	$(CPP) $(CPPFLAGS) $(CPPINCLUDE) $*.cpp

savelwo.obj: savelwo.cpp savelwo.h div.h
	$(CPP) $(CPPFLAGS) $(CPPINCLUDE) $*.cpp


