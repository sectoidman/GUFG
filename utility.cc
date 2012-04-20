#include "utility.h"
utility::utility(char * n)
{
	build(n);
	init();
	xLock = 0;
	yLock = 0;
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
	if(tolerance > 1 && tolerance < 30) printf("%s: %i, %i - %i, %i\n", name, tolerance, activation, t, f);
	return 1;
}
