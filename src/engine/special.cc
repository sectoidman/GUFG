#include "interface.h"
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <assert.h>
using namespace std;

special::special(const char * n)
{
	build(n);
}

negNormal::negNormal(const char * n)
{
	build(n);
}

bool negNormal::activate(std::vector<int> inputs, int pattern, int t, int f, std::vector<int> meter, SDL_Rect &p)
{
	for(unsigned int i = 0; i < inputs.size(); i++){
		if(pattern & (1 << i)){
			if(inputs[i] != -1) return 0;
		}
	}
	if(t > tolerance) return 0;
	if(f > activation) return 0;
	return check(p, meter);
}

bool special::activate(std::vector<int> inputs, int pattern, int t, int f, std::vector<int> meter, SDL_Rect &p)
{
	for(unsigned int i = 0; i < inputs.size(); i++){
		if(pattern & (1 << i)){
			if(abs(inputs[i]) != 1) return 0;
		}
	}
	if(t > tolerance) return 0;
	if(f > activation) return 0;
	return check(p, meter);
}

bool mash::activate(std::vector <int> inputs, int pattern, int t, int f, std::vector<int> meter, SDL_Rect &p)
{
	int go = 0;
	if(action::activate(inputs, pattern, t, f, meter, p)){
		for(unsigned int i = 0; i < inputs.size(); i++){
			if(inputs[i] >= minHold){
				if(inputs[i] <= maxHold || !maxHold) go++;
			}
		}
		if(go >= buttons) return 1;
	}
	return 0;
}

bool releaseCheck::activate(std::vector<int> inputs, int pattern, int t, int f, std::vector<int> meter, SDL_Rect &p){
	for(unsigned int i = 0; i < inputs.size(); i++){
		if(inputs[i] > 0) return 0;
	}
	return check(p, meter);
}

void mash::zero()
{
	action::zero();
	buttons = 1;
}

bool mash::setParameter(char * buffer)
{
	char savedBuffer[100];
	strcpy(savedBuffer, buffer);

	char * token = strtok(buffer, "\t: \n-");

	if(!strcmp("Buttons", token)){
		token = strtok(NULL, "\t: \n-");
		buttons = atoi(token); 
		return 1;
	} else return action::setParameter(savedBuffer);
}

int werf::arbitraryPoll(int n, int f)
{
	switch (n){
	case 28:
		if(f == 0) return 1;
		break;
	case 27:
		return startPosX;
	case 26:
		return startPosY;
	default:
		break;
	}
	return action::arbitraryPoll(n, f);
}

bool werf::check(SDL_Rect &p, std::vector<int> meter)
{
	if(p.y != 0) return 0;
	if(p.x > 0) return 0;
	return action::check(p, meter);
}

bool luftigeWerf::check(SDL_Rect &p, std::vector<int> meter)
{
	if(p.y == 0) return 0;
	if(p.x > 0) return 0;
	return action::check(p, meter);
}

bool werf::setParameter(char * buffer)
{
	char savedBuffer[100];
	strcpy(savedBuffer, buffer);

	char * token = strtok(buffer, "\t: \n-");

	if (!strcmp("Position", token)){
		token = strtok(NULL, "\t: \n");
		startPosX = atoi(token); 

		token = strtok(NULL, "\t: \n");
		startPosY = atoi(token); 
		return 1;
	} else return action::setParameter(savedBuffer);
}

bool luftigeWerf::setParameter(char * buffer)
{
	char savedBuffer[100];
	strcpy(savedBuffer, buffer);
	char * token = strtok(buffer, "\t: \n-");
	if(!strcmp("Landing", token)) return airMove::setParameter(savedBuffer);
	else return werf::setParameter(savedBuffer);
}
