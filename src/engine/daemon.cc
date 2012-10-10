#include "analytics.h"
#include <ctime>

demon::demon()
{
	init();
	script = NULL;
}

demon::demon(int id)
{
	wins = 0;
	ID = id;
	init();
	script = NULL;
}

demon::demon(int id, frame * s)
{
	wins = 0;
	ID = id;
	init();
	script = s;
}

void demon::genEvent(bool *& sAxis, int *& posEdge, bool *& negEdge) 
{
	if(script){
		for(int i = 0; i < 5; i++){
			if(i < 4) sAxis[i] = script->axis[i];
			posEdge[i] = script->pos[i];
			negEdge[i] = script->neg[i];
		}
		script = script->next;
	} else {
		for(int i = 0; i < 5; i++){
			srand(time(NULL));
			if(i < 2) sAxis[i] = rand()%2;
			srand(time(NULL));
			posEdge[i] = rand()%2;
			srand(time(NULL));
			if(!posEdge[i]) negEdge[i] = rand()%2;
			else negEdge[i] = 0;
		}
		srand(time(NULL));
		if(facing == 1){ 
			sAxis[3] = rand()%2; sAxis[2] = 0;
		} else {
			sAxis[2] = rand()%2; sAxis[3] = 0;
		}
	}
}
