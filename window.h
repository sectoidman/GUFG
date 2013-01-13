#include <stdio.h>
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <GL/gl.h>
#include <GL/glut.h>
class window{
public:
	window() : screen(NULL) {}
	bool screenInit();
	SDL_Surface *& screen;
};
