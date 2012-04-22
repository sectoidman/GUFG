#include "special.h"

special::special(char * n)
{
	build(n);
	int r = strlen(name);
	xLock = 0;
	yLock = 0;
	init();
}

bool special::check(bool pos[5], bool neg[5], int t, int f)
{
	for(int i = 0; i < 5; i++){
		if(button[i] == 1){
			if(!pos[i] && !neg[i]) return 0;
		}
	}
	if(t > tolerance) return 0;
	if(f > activation) return 0;
	return 1;
}
