#include "move.h"
utility::utility(char * n)
{
	build(n);
	init();
}

bool utility::check(bool pos[5], bool neg[5], int t, int f)
{
	for(int i = 0; i < 5; i++){
		if(button[i] == 1){
			if(!pos[i]) return 0;
		} else if(pos[i]) return 0;
	}
	if(t > tolerance) return 0;
	if(f > activation) return 0;
	return 1;
}

looping::looping(char * n)
{
	build(n);
	init();
}

void looping::step()
{
	currentFrame++;
	if(currentHit < hits-1 && currentFrame > totalStartup[currentHit+1]) currentHit++;
	if(currentFrame >= frames) init();
}
