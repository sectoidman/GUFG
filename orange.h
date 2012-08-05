#ifndef ORANGE_INCLUDED
#define ORANGE_INCLUDED
#include "yellow.h"
class orange : public character {
public:
	orange();
	virtual void prepHooks(int[], bool[], bool[], SDL_Rect &, bool);	//Take input from the game and propagate it to the appropriate actionTrie.
	void init();

	projectile * matrix;
	int matrixComplexity;
};
#endif
