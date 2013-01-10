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

bool negNormal::activate(int pos[5], bool neg[5], int pattern, int t, int f, int meter[], SDL_Rect &p)
{
	for(int i = 0; i < 5; i++){
		if(pattern & (1 << i)){
			if(!neg[i]) return 0;
		}
	}
	if(t > tolerance) return 0;
	if(f > activation) return 0;
	return check(p, meter);
}

bool special::activate(int pos[5], bool neg[5], int pattern, int t, int f, int meter[], SDL_Rect &p)
{
	for(int i = 0; i < 5; i++){
		if(pattern & (1 << i)){
			if(pos[i] != 1 && !neg[i]) return 0;
		}
	}
	if(t > tolerance) return 0;
	if(f > activation) return 0;
	return check(p, meter);
}

super::super(const char * n)
{
	build(n);
}

bool mash::activate(int pos[5], bool neg[5], int pattern, int t, int f, int meter[], SDL_Rect &p)
{
	int go = 0;
	if(action::activate(pos, neg, pattern, t, f, meter, p)){
		for(int i = 0; i < 5; i++){
			if(pos[i] >= minHold){
				if(pos[i] <= maxHold || !maxHold) go++;
			}
		}
		if(go >= buttons) return 1;
	}
	return 0;
}

bool releaseCheck::activate(int pos[5], bool neg[5], int pattern, int t, int f, int meter[], SDL_Rect &p)
{
	for(int i = 0; i < 5; i++){
		if(pos[i] > 0) return 0;
	}
	return check(p, meter);
}

int super::arbitraryPoll(int q, int f)
{
	if(q == 2 && f == freezeFrame) return freezeLength;
	else return 0;
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

bool super::setParameter(char * buffer)
{
	char savedBuffer[100];
	strcpy(savedBuffer, buffer);

	char * token = strtok(buffer, "\t: \n-");

	if(!strcmp("SuperFreeze", token)){
		token = strtok(NULL, "\t: \n-");
		freezeFrame = atoi(token); 

		token = strtok(NULL, "\t: \n-");
		freezeLength = atoi(token); 
		freezeLength = freezeLength - freezeFrame;
		return 1;
	} else return action::setParameter(savedBuffer);
}

bool airSuper::setParameter(char * buffer)
{
	bool x;
	char savedBuffer[100];
	strcpy(savedBuffer, buffer);
	x = super::setParameter(buffer);
	if (!x) x = airMove::setParameter(savedBuffer);
	return x;
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
	return 0;
}

bool werf::check(SDL_Rect &p, int meter[])
{
	if(p.y != 0) return 0;
	if(p.x > 0) return 0;
	return action::check(p, meter);
}

bool luftigeWerf::check(SDL_Rect &p, int meter[])
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
