#ifndef ORANGE_INCLUDED
#define ORANGE_INCLUDED
#include "yellow.h"
class orange : public character {
public:
	orange();
	void init(action *&);

	void cull(int);
	void step(action *&, int&, int&);
	instance ** matrix;

	int matrixComplexity;
	instance * spawn(action *);
};
#endif
