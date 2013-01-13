#include "thing.h"
#include "window.h"
#include "harness.h"
#ifndef SESSION_INCLUDED
#define SESSION_INCLUDED
class session : public window, public harness{
public:
	virtual void init();
	virtual void draw(thing&);
	virtual void readInput();
	virtual void processInput(SDL_Event&);
	double camX, camY, camZ, pan, tilt, spin, size;
	bool rrX, rlX, rrY, rlY, rrZ, rlZ, tuX, tuY, tuZ, tdX, tdY, tdZ;
};
#endif
