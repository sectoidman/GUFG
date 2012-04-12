# 2012-04-11 - Fixed the Makefile.

OBJS = \
  aux.o \
  utility.cc \
  keytest.o \
  interface.o \
  move.o \
  character.o \
  hitstun.o \
  trie.o \
  player.o \
  draw.o \
  red.o

FLAGS = -g -O2 -Wno-write-strings `sdl-config --cflags`

LIBS = `sdl-config --libs` -lSDL_image

all: keytest

keytest: $(OBJS)
	g++ -o keytest $(OBJS) $(FLAGS) $(LIBS)

keytest.o:
	g++ -c keytest.cc $(FLAGS)

interface.o:
	g++ -c interface.cc $(FLAGS)

move.o:
	g++ -c move.cc $(FLAGS)

character.o:
	g++ -c character.cc $(FLAGS)

player.o:
	g++ -c player.cc $(FLAGS)

draw.o:
	g++ -c draw.cc $(FLAGS)

red.o:
	g++ -c red.cc $(FLAGS)

aux.o:
	g++ -c aux.cc $(FLAGS)

utility.o:
	g++ -c utility.cc $(FLAGS)

# -f Is needed to avoid errors with missing files.
clean:
	rm -f keytest *.o
