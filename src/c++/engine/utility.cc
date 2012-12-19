#include "action.h"
utility::utility(const char * n)
{
	build(n);
}

bool utility::activate(int pos[5], bool neg[5], int pattern, int t, int f, int * meter, SDL_Rect &p)
{
	for(int i = 0; i < 5; i++){
		if(pattern & (1 << i)){
			if(pos[i] < minHold) return 0;
			if(maxHold && pos[i] > maxHold) return 0;
		} else if(pos[i] == 1) return 0;
	}
	if(t > tolerance) return 0;
	if(f > activation) return 0;
	return check(p, meter);
}

looping::looping(const char * n)
{
	build(n);
}

void looping::step(int *& meter, int &f)
{
	action::step(meter, f);
	if(f != 0){ 
		if(meter[1] + gain[0] < 300) meter[1] += gain[0];
		else meter[1] = 300;
	}
	if(f >= frames) f = 0;
}
