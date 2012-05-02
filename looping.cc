#include "looping.h"

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
