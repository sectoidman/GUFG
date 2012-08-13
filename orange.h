#ifndef ORANGE_INCLUDED
#define ORANGE_INCLUDED
#include "yellow.h"
class orange : public character {
public:
	orange();
	void init(action *&);

	avatar ** matrix;
	int matrixComplexity;
	avatar * spawn(action *);
};
#endif
