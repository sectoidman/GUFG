#include "yellow.h"
yellow::yellow(){
	head = new actionTrie;
	airHead = new actionTrie;
	build("Yellow");
}

void yellow::resetAirOptions()
{
	meter[1] = 1;
	meter[2] = 2;
}
