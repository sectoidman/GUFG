#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <GL/gl.h>
#include <GL/glut.h>
#ifndef THING_INCLUDED
#define THING_INCLUDED
class thing{
public:
	virtual void draw();
};
#endif
