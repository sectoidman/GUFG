#include "interface.h"
orange::orange()
{
	head = new actionTrie;
	airHead = new actionTrie;
	build("Orange");
	init();
//	projectile *matrix;
}
