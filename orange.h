#ifndef ORANGE_INCLUDED
#define ORANGE_INCLUDED
#include "yellow.h"
class beam : public projectile {
public:
	beam();
	beam(const char*, const char*);
	virtual void build(const char*, const char*);
	virtual void init();
	virtual action * createMove(char*);
};

class orange : public character {
public:
	orange();
	void init(action *&);

	beam * matrix;
	int matrixComplexity;
	avatar * spawn();
};

class beamStop : public special {
public:
	beamStop();
	beamStop(const char*);
	virtual void execute(action *, int *&);
	virtual bool check(bool[], bool[], int, int, int[], SDL_Rect&); //Check to see if the action is possible right now.
};
#endif
