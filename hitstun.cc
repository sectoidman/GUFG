#include "move.h"

void hitstun::init(int n)
{	
	counter = n;
}

void hitstun::step(int *& resource)
{
	if(counter <= 0) currentFrame++;
	else counter--;
}
	
void hitstun::blockSuccess(int st){
	init(st);
}

bool hitstun::takeHit(hStat & s){
	if(s.blockMask.i & blockState.i){
		init(s.stun);
		return 0;
	}
	else return 1;
}

hitstun::hitstun(const char * n)
{
	build(n);
	currentFrame = 0;
}
