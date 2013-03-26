#include "harness.h"
#include "thing.h"
#include "window.h"

#ifndef ___session
#define ___session

class session : public window, public harness{
public:
	virtual void init();
	virtual void draw(model&);
	virtual void readInput();
	virtual void processInput(SDL_Event&);
	virtual void camInit();
	double camX, camY, camZ, pan, tilt, spin; 
	float r, g, b;
	bool rrX, rlX, rrY, rlY, rrZ, rlZ, tuX, tuY, tuZ, tdX, tdY, tdZ;
};
#endif
