#include "yellow.h"
yellow::yellow(){
	delete head;
	delete airHead;
	delete neutral;
	delete crouch;
	delete standBlock;
	delete crouchBlock;
	delete airBlock;
	delete reel;
	delete untech;
	delete crouchReel;
	head = new moveTrie;
	airHead = new moveTrie;
	build("Yellow");
}

void yellow::init()
{
	character::init();
	meter[2] = 2;
}

void yellow::resetAirOptions()
{
	meter[1] = 1;
	meter[2] = 2;
}
