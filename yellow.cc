#include "yellow.h"
yellow::yellow(){
	name = "Yellow";
	delete head;
	delete airHead;
	delete neutral;
	delete crouch;
	delete standBlock;
	delete crouchBlock;
	delete airBlock;
	delete reel;
	delete fall;
	delete crouchReel;
	head = new moveTrie;
	airHead = new moveTrie;
	build(name);
	
}

void yellow::init()
{
	character::init();
	jumpOptions = 3;
}
