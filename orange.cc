#include "interface.h"
orange::orange()
{
	head = new actionTrie;
	airHead = new actionTrie;
	build("Orange");
	init();
}

void orange::init()
{
	character::init();
	matrixComplexity = 0;
	matrix = NULL;
}

void orange::prepHooks(int inputBuffer[30], bool down[5], bool up[5], SDL_Rect &p, bool dryrun)
{
	character::prepHooks(inputBuffer, down, up, p, dryrun);
	for(int i = 0; i < matrixComplexity; i++);
//		matrix->prepHooks(inputBuffer, down, up, p, dryrun);
}
