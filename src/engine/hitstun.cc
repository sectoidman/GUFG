#include "action.h"
#include <math.h>

void hitstun::init(int n)
{
	counter = n;
	if(next != NULL) next->init(n - frames);
}

void hitstun::step(std::vector<int>& meter, int &f, int &c, int &h)
{
	if(counter <= 0){
		f = frames-1;
		action::step(meter, f, c, h);
	} else {
		if(f < frames - 1) action::step(meter, f, c, h);
		counter--;
	}
}

int hitstun::takeHit(hStat& s, int b, status& current)
{
	if(!s.stun) return 1;
	if(s.blockMask.i & blockState.i){
		switch (b){
		case -2:
			current.frame = 0;
			init(s.stun - 2 - s.stun/4);
			s.push = 0;
			return -2;
		case -1:
			current.frame = 0;
			init(s.stun - 1  - s.stun/5);
			s.push = (s.push*4)/5;
			return -1;
		case 0:
			current.frame = 0;
			init(s.stun - std::max(1, s.stun/14));
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
