#include "move.h"
#include <iostream>
#include <stdio.h>
#include <fstream>
using namespace std;

special::special(const char * n)
{
	build(n);
	init();
}

bool special::check(bool pos[5], bool neg[5], int t, int f, int* resource, SDL_Rect &p)
{
	for(int i = 0; i < 5; i++){
		if(button[i] == 1){
			if(!pos[i] && !neg[i]) return 0;
		}
	}
	if(t > tolerance) return 0;
	if(f > activation) return 0;
	if(resource[0] < cost) return 0;
	return 1;
}

super::super(const char * n)
{
	build(n);
	readFreeze(n);
	init();
}

bool mash::check(bool pos[5], bool neg[5], int t, int f, int* resource, SDL_Rect &p)
{
	if(t > tolerance) return 0;
	if(f > activation) return 0;
	for(int i = 0; i < 5; i++){
		if(pos[i]) return 1;
	}
	return 0;
}

int super::arbitraryPoll(int q)
{
	if(q == 2 && currentFrame == freezeFrame) return freezeLength;
	else return 0;
}

void super::readFreeze(const char * n)
{
	ifstream read;
	char fname[40];

	sprintf(fname, "%s.mv", n);
	read.open(fname);
	
	while(read.get() != '|'); read.ignore();
	read >> freezeFrame; 

	while(read.get() != '-'); read.ignore();
	read >> freezeLength;

	freezeLength = freezeLength - freezeFrame;
	read.close();
}

int werf::arbitraryPoll(int n)
{
	switch (n){
	case 28:
		if(currentFrame == 0) return 1;
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

bool werf::check(bool pos[5], bool neg[5], int t, int f, int* resource, SDL_Rect &p)
{
	bool throwable = true;
	for(int i = 0; i < 5; i++){
		if(button[i] == 1){
			if(!pos[i]) return 0;
		}
	}
	if(t > tolerance) return 0;
	if(f > activation) return 0;
	if(resource[0] < cost) return 0;
	if(p.w > xRequisite) return 0;
	if(p.y > yRequisite) return 0;
	if(!throwable) return 0;
	return 1;
}

