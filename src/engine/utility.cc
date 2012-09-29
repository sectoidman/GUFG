#include "action.h"
utility::utility(const char * n)
{
	build(n);
}

bool utility::activate(int pos[5], bool neg[5], int pattern, int t, int f, int * resource, SDL_Rect &p)
{
	for(int i = 0; i < 5; i++){
		if(pattern & (1 << i)){
			if(pos[i] != 1 && !neg[i]) return 0;
		} else if(pos[i] == 1) return 0;
	}
	if(t > tolerance) return 0;
	if(f > activation) return 0;
	return check(p, resource);
}

looping::looping(const char * n)
{
	build(n);
}

void looping::step(int *& resource, int &f)
{
	action::step(resource, f);
	if(f >= frames) f = 0;
}

