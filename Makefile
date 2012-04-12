# 2012-04-11 - No Seriously guys, your makefile sucked.
# 2012-04-11 - Fixed the Makefile.

OBJS = \
  aux.o \
  utility.o \
  keytest.o \
  interface.o \
  move.o \
  character.o \
  hitstun.o \
  trie.o \
  player.o \
  draw.o \
  red.o

#using settings recomended from: http://schneide.wordpress.com/2009/03/09/make-friends-with-your-compiler/
#comment out to turn off debugging
CXXFLAGS = -O2 -Wno-write-strings `sdl-config --cflags` -g -Wall -Woverloaded-virtual -Wctor-dtor-privacy -g

#no debugging flags
#CXXFLAGS = -O2 -Wno-write-strings `sdl-config --cflags`

LDFLAGS = `sdl-config --libs` -lSDL_image -lm
CC=g++

all: keytest

keytest: $(OBJS)

#TODO list dependencies, otherwise Make rule generated automatically
#keytest.o:
#interface.o:
#move.o:
#character.o:
#player.o:
#draw.o:
#red.o:
#aux.o:
#utility.o: utility.h utility.cpp

# -f Is needed to avoid errors with missing files.
clean:
	rm -f keytest $(OBJS)
