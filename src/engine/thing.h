#include "gl-compat.h"
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <string>
#include <vector>

#ifndef ___thing
#define ___thing

using std::string;
using std::vector;

class thing{
public:
	virtual void draw() = 0;
};

class model{
public:
	virtual void draw();
	virtual bool readModel();
	virtual bool readModel(string);
	vector<double> vertices [3];
	vector<vector<int>> faces;
};
#endif
