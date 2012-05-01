#include "red.h"
red::red()
{
	name = "Red";
	delete head;
	head = new moveTrie;
	build(name);
	
	delete neutral;
	neutral = new utility("Red/NS");
	head->insert(neutral);
	cMove = neutral;
}
