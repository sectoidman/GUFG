#include "grey.h"
grey::grey(){
	head = new actionTrie;
	airHead = new actionTrie;
	meter = new int[5];
	build("Grey", "Grey");
	noirActivate = neutral;
}

int grey::takeHit(action *& cMove, hStat & s, int b, int &f, int &c, int &h, int &p)
{
	int freeze = character::takeHit(cMove, s, b, f, c, h, p);
	if(p == -1) meter[4] += 205;
	if(meter[4] >= 1020){
		meter[4] = 1023;
		freeze = 0;
		cMove = noirActivate;
	}
	return freeze;
}

void grey::tick()
{
	if(meter[4] > 0) meter[4]--;
}

void grey::init(action *& cMove)
{
	meter[4] = 0;
	character::init(cMove);
}
