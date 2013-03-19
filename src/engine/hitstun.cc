#include "action.h"
#include <math.h>
void hitstun::step(std::vector<int>& meter, status &current)
{
	if(current.counter >= 0){
		current.frame = frames-1;
		action::step(meter, current);
	} else {
		if(current.frame < frames - 1) action::step(meter, current);
		current.counter++;
	}
}

int hitstun::takeHit(hStat& s, int b, status& current)
{
	if(!s.stun) return 1;
	if(s.blockMask.i & blockState.i){
		switch (b){
		case -2:
			current.frame = 0;
			current.counter = -(s.stun - 2 - s.stun/4);
			s.push = 0;
			return -2;
		case -1:
			current.frame = 0;
			current.counter = -(s.stun - 1  - s.stun/5);
			s.push = (s.push*4)/5;
			return -1;
		case 0:
			current.frame = 0;
			current.counter = -(s.stun - std::max(1, s.stun/14));
			return 0;
		}
	}
	return 1;
}

hitstun::hitstun(const char * n)
{
	build(n);
}
