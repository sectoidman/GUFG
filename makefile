MODELVIEW:
	g++ -O2 -g -Wno-overloaded-virtual -Wno-format -Wno-format-contains-nul -Wctor-dtor-privacy -std=c++11 -lSDL -lGL -lSDL_image -lSDLmain -lSDL_mixer *.cc -o modelview

ALL: MODELVIEW

