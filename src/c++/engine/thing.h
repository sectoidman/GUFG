#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <vector>
#ifndef THING_INCLUDED
#define THING_INCLUDED
class thing{
public:
	virtual void draw();
	virtual bool readModel();
	virtual bool readModel(const char*);
	std::vector<double> vertices[3];
	~thing();
};
#endif
