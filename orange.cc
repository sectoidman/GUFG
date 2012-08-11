#include "interface.h"
orange::orange()
{
	head = new actionTrie;
	airHead = new actionTrie;
	build("Orange", "Orange");
	init();
}

void orange::init()
{
	character::init();
	matrixComplexity = 0;
	matrix = NULL;
}
