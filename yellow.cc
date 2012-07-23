#include "yellow.h"
yellow::yellow(){
	head = new actionTrie;
	airHead = new actionTrie;
	meter = new int[4];
	build("Yellow");
}

void yellow::resetAirOptions()
{
	meter[1] = 1;
	meter[2] = 2;
}

void yellow::init()
{
	character::init();
	meter[3] = 0;
}

void yellow::tick()
{
	character::tick();
	if(meter[3] > 0){ 
		meter[3]--;
		if(meter[3] == 0){ 
			meter[3] = -450;
		}
	}
	else if(meter[3] < 0) meter[3]++;
}

flashStep::flashStep(char * n)
{
	build(n);
	init();
}

bool flashStep::setParameter(char * buffer)
{
	char savedBuffer[100];
	strcpy(savedBuffer, buffer);

	char * token = strtok(buffer, "\t: \n-");

	if(!strcmp("FlashCost", token)){
		token = strtok(NULL, "\t: \n-");
		flashMeterCost = atoi(token); 
		return 1;
	} else return action::setParameter(savedBuffer);
}

bool flashStep::check(bool pos[5], bool neg[5], int t, int f, int* resource, SDL_Rect&)
{
	for(int i = 0; i < 5; i++){
		if(button[i] == 1){
			if(!pos[i] && !neg[i]) return 0;
		}
	}
	if(t > tolerance) return 0;
	if(f > activation) return 0;
	if(resource[0] < cost) return 0;
	if(resource[3] < 1 && flashMeterCost > 0) return 0;
	if(resource[4] > 0) return 0;
	return 1;
}

void flashStep::execute(action * last, int *& resource)
{
	resource[3] -= flashMeterCost;
	if(resource[3] > 510) resource[3] = 510;
	else if(resource[3] < 0) resource[3] = -450;
	action::execute(last, resource);
}
