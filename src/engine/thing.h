#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <vector>
#include "gl-compat.h"
#ifndef THING_INCLUDED
#define THING_INCLUDED
class thing{
public:
	virtual void draw() = 0;
};

class model{
public:
	virtual void draw();
	virtual bool readModel();
	virtual bool readModel(const char*);
	std::vector<double> vertices [3];
	std::vector< std::vector<int> > faces;
};
#endif
