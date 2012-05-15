#include "move.h"
utility::utility(char * n)
{
	build(n);
	init();
}

bool utility::check(bool pos[5], bool neg[5], int t, int f, int * resource)
{
	for(int i = 0; i < 5; i++){
		if(button[i] == 1){
			if(!pos[i]) return 0;
		} else if(pos[i]) return 0;
	}
	if(t > tolerance) return 0;
	if(f > activation) return 0;
	if(resource[0] < cost) return 0;
	return 1;
}

looping::looping(char * n)
{
	build(n);
	init();
}

void looping::step(int *& resource)
{
	move::step(resource);
	if(currentFrame >= frames) init();
}
