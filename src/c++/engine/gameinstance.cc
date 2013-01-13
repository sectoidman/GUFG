#include "gameinstance.h"
#include <assert.h>
void gameInstance::init()
{
	assert(screenInit());
	harness::init();
}
