#include "gl-compat.h"
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

#ifndef ___window
#define ___window

class window{
public:
	window();
	virtual bool screenInit();
	virtual void setLighting();
	SDL_Surface * screen;
	int h, w;
};
#endif
