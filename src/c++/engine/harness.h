#include <SDL/SDL.h>
#include "controller.h"
#ifndef HARNESS_INCLUDED
#define HARNESS_INCLUDED
class harness{
public:
	harness();
	virtual void init();
	virtual void readInput();
	virtual void processInput(SDL_Event&);
	bool gameover;
	std::vector<controller*> p;
};

class arcadeHarness : public harness{
public:
	std::vector<std::vector<bool>> sAxis;
	std::vector<std::vector<int>> posEdge;
	std::vector<std::vector<bool>> negEdge;
	std::vector<int> counter;
	virtual void initContainers(int, int);
//	virtual void processInput(SDL_Event&);	/*Accepts input into input containers, for use by anything that wants it*/
};

#endif
