####### Compiler, tools and options


#---- Release
CXXCFG   	= -O3
LFLAGS		= -L../lib
OUT_DIR		= bin64
#---- Debug
#CXXCFG   	= -g -D_DEBUG
#LFLAGS		= -Wl -L../lib/debug64
#OUT_DIR	= debug64


CXX      	= g++
CXXFLAGS 	= $(CXXCFG) -Wall -fno-strict-aliasing
INCPATH  	= -I../include $(shell pkg-config --libs sdl2)
LIBS     	= ../lib/libAntTweakBar.a $(shell pkg-config --cflags sdl2) -lGL -lX11

DEL_FILE 	= rm -f
DEL_DIR  	= rmdir
NO_STDERR	= 2> /dev/null
EXP_SH      = '\#!/bin/sh'
EXP_PATH	= 'export LD_LIBRARY_PATH=`dirname $$0`/../../lib ; $$0.out'

####### Files


#SRC_FILES	= TwSimpleGLFW.c TwSimpleGLUT.c TwSimpleSDL.c TwAdvanced1.cpp TwString.cpp TwDualGLUT.c
SRC_FILES	= TwGLCoreSDL.c


####### Build rules


#first:	depend all
first:  all

all: 	Makefile $(SRC_FILES)

	@echo "===== Build TwGLCoreSDL ===="
	$(CXX) $(CXXFLAGS) $(INCPATH) TwGLCoreSDL.c $(LFLAGS) $(LIBS) -o $(OUT_DIR)/TwGLCoreSDL.out
	@echo $(EXP_SH) > $(OUT_DIR)/TwGLCoreSDL
	@echo $(EXP_PATH) >> $(OUT_DIR)/TwGLCoreSDL
	@chmod +x $(OUT_DIR)/TwGLCoreSDL
	@echo "To execute the example, run " $(OUT_DIR)/TwGLCoreSDL

# append dependencies to this Makefile
#depend:
#	@echo "===== Make dependencies ====="
#	makedepend -Y
#	makedepend -a -Y -- $(CXXFLAGS) $(INCPATH) -- $(SRC_FILES) $(NO_STDERR)


# clean temporary files
clean:
	@echo "===== Clean ====="
	-$(DEL_FILE) *.o
	-$(DEL_FILE) *~ core *.core *.stackdump
	-$(DEL_FILE) debug64/*
	-$(DEL_DIR) debug64

	
####### DEPENDENCIES

TwGLCoreSDL.o:  ../include/AntTweakBar.h

