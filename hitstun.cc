#include "action.h"
#include <math.h>

void hitstun::init(int n)
{
	counter = n;
}

void hitstun::step(int *& resource, int &f)
{
	if(counter <= 0) f++;
	else counter--;
}

action * hitstun::blockSuccess(int st){
	init(st);
	return this;
}

int hitstun::takeHit(hStat & s, int b, int &f){
	if(s.blockMask.i & blockState.i){
		if(b == 1){ 
			f = 0;
			init(s.stun - 1  - s.stun/5);
			s.push = (s.push*4)/5;
			return -1;
		} else {
			f = 0;
			init(s.stun - std::max(0, 1 - s.stun/15));
			return 0;
		}
	}
	else return 1;
}

hitstun::hitstun(const char * n)
{
	build(n);
}

int hitstun::arbitraryPoll(int q, int f)
{
	if(q == 1) return counter;
	else return 0;
}
