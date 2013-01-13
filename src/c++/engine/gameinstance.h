#include "thing.h"
#include "window.h"
#include "harness.h"
#ifndef GAMEINSTANCE
#define GAMEINSTANCE
class gameInstance : public window, public harness{
public:
	virtual void init();
	virtual void draw(thing&);
};
#endif
