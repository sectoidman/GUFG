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

bool negNormal::activate(bool pos[5], bool neg[5], int t, int f, int resource[], SDL_Rect &p)
{
	for(int i = 0; i < 5; i++){
		if(button[i] == 1){
			if(!neg[i]) return 0;
		}
	}
	if(t > tolerance) return 0;
	if(f > activation) return 0;
	return check(p, resource);
}

bool special::activate(bool pos[5], bool neg[5], int t, int f, int resource[], SDL_Rect &p)
{
	for(int i = 0; i < 5; i++){
		if(button[i] == 1){
			if(!pos[i] && !neg[i]) return 0;
		}
	}
	if(t > tolerance) return 0;
	if(f > activation) return 0;
	return check(p, resource);
}

super::super(const char * n)
{
	build(n);
}

bool mash::activate(bool pos[5], bool neg[5], int t, int f, int resource[], SDL_Rect &p)
{
	bool go = false;
	if(t > tolerance) return 0;
	if(f > activation) return 0;
	for(int i = 0; i < 5; i++){
		if(pos[i]) go = true;
	}
	if(go) return check(p, resource);
	else return 0;
}

int super::arbitraryPoll(int q, int f)
{
	if(q == 2 && f == freezeFrame) return freezeLength;
	else return 0;
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

bool werf::check(SDL_Rect &p, int resource[])
{
	if(p.y != 0) return 0;
	if(p.x > 0) return 0;
	return action::check(p, resource);
}

bool luftigeWerf::check(SDL_Rect &p, int resource[])
{
	if(p.y == 0) return 0;
	if(p.x > 0) return 0;
	return action::check(p, resource);
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
