#include "interface.h"
red::red()
{
	name = "Red";
	delete head;
	delete airHead;
	delete neutral;
//	delete crouch;
	delete standBlock;
	delete crouchBlock;
	delete airBlock;
	delete reel;
	delete fall;
//	delete crouchReel;
	
	airHead = new moveTrie;
	head = new moveTrie;
	build(name);
	
	cMove = neutral;
}

void red::touch(void * target)
{
	((interface*)target)->timer+=290;
}
