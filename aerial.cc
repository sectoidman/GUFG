#include "move.h"
#include <stdio.h>
#include <iostream>
#include <fstream>
using namespace std;

airMove::airMove(char * n)
{
	build(n);
	init();
}

void airMove::land(move *& c)
{
	init();
	c = landing;
}

void airMove::build(char * n)
{
	move::build(n);
	landing = NULL;
}
