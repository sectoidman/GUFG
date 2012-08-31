#ifndef GREY_INCLUDED
#define GREY_INCLUDED
#include "yellow.h"
class grey : public character {
public:
	grey();
	int takeHit(action *&, hStat&, int, int&, int&, int&, int&);
	void tick();
	void init(action*&);
	action * noirActivate;
};
#endif
