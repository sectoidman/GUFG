#include <SDL/SDL.h>
#ifndef HARNESS_INCLUDED
#define HARNESS_INCLUDED
class harness{
public:
	harness();
	void init();
	virtual void readInput();
	virtual void processInput(SDL_Event&);
	bool gameover;
	double camX, camY, camZ, pan, tilt, size;
	bool rrX, rlX, rrY, rlY, tuX, tuY, tuZ, tdX, tdY, tdZ;
};
#endif
