#include "thing.h"
#include "window.h"
#include "harness.h"
#ifndef GAMEINSTANCE
#define GAMEINSTANCE
class gameInstance : public window, public harness{
public:
	virtual void init();
	virtual void draw(thing&);
	virtual void readInput();
	virtual void processInput(SDL_Event&);
	double camX, camY, camZ, pan, tilt, spin, size;
	bool rrX, rlX, rrY, rlY, rrZ, rlZ, tuX, tuY, tuZ, tdX, tdY, tdZ;
};
#endif
