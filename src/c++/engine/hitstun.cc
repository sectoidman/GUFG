#include "action.h"
#include <math.h>

void hitstun::init(int n)
{
	if(n > 0) counter = n;
}

void hitstun::step(int *& meter, int &f, int &c, int &h)
{
	if(counter <= 0){ 
		action::step(meter, f, c, h);
	} else {
		if(f < frames - 1) action::step(meter, f, c, h);
		counter--;
	}
}

action * hitstun::blockSuccess(int st){
	if(st > 0) init(st);
	return this;
}

int hitstun::takeHit(hStat& s, int b, int& f, int& c, int& h)
{
	if(!s.stun) return 1;
	if(s.blockMask.i & blockState.i){
		switch (b){
		case -2:
			f = 0;
			init(s.stun - 2 - s.stun/4);
			s.push = 0;
			return -2;
		case -1:
			f = 0;
			init(s.stun - 1  - s.stun/5);
			s.push = (s.push*4)/5;
			return -1;
		case 0:
			f = 0;
			init(s.stun - std::max(0, 1 - s.stun/15));
			return 0;
		}
	}
	return 1;
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
