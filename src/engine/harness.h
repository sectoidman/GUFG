#include "analytics.h"
#include "controller.h"
#include <SDL/SDL.h>

#ifndef ___harness
#define ___harness

using std::vector;

class harness{
public:
	harness();
	virtual void init();
	virtual void readInput();
	virtual void processInput(SDL_Event&);
	bool gameover;
	vector<controller*> p;
};

class arcadeHarness : public harness{
public:
	vector<frame> currentFrame;
	vector<int> counter; //Basically just a delay for menu interaction
	virtual void initContainers(int, int); //Spawn the containers needed for input
	virtual void initContainers();	//Reinitialize input containers to 0
//	virtual void processInput(SDL_Event&);	/*Accepts input into input containers, for use by anything that wants it*/
};
#endif
