#include "interface.h"
orange::orange()
{
	head = new actionTrie;
	airHead = new actionTrie;
	build("Orange", "Orange");
}

void orange::init(action *& cMove)
{
	character::init(cMove);
	matrixComplexity = 0;
	matrix = NULL;
}
