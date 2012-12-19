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

void demon::characterSelect(int i)
{
	v = NULL;
	switch(i){
	case 1:
		v = new red;
		break;
	case 2:
		v = new yellow;
		break;
	default:
		v = new character("White");
		break;
	}
	meter = pick()->generateMeter();
}

void demon::roundInit()
{
	instance::init();
	pick()->neutralize(cMove, aerial, meter);
	if(v) pick()->init(meter);
	updateRects();
	lCorner = 0;
	rCorner = 0;
	elasticX = 0;
	elasticY = 0;
	blockType = 0;
	slide = 0;
	stick = 0;
	hover = 0;
	throwInvuln = 0;
	particleLife = 0;
	particleType = 0;
	if(ID == 1){ 
		facing = 1;
		posX = 1400;
	} else {
		facing = -1;
		posX = 1800;
	}
	updateRects();
}
