#include "move.h"
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <assert.h>
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
	for(int i = 0; i < 5; i++){
		if(button[i] == 1){
			if(!pos[i]) return 0;
		}
	}
	if(t > tolerance) return 0;
	if(f > activation) return 0;
	if(resource[0] < cost) return 0;
	if(p.w > xRequisite) return 0;
	if(p.y != 0) return 0;
	if(p.x > 0) return 0;
	return 1;
}

bool luftigeWerf::check(bool pos[5], bool neg[5], int t, int f, int* resource, SDL_Rect &p)
{
	for(int i = 0; i < 5; i++){
		if(button[i] == 1){
			if(!pos[i]) return 0;
		}
	}
	if(t > tolerance) return 0;
	if(f > activation) return 0;
	if(resource[0] < cost) return 0;
	if(p.w > xRequisite) return 0;
	if(p.h > yRequisite) return 0;
	if(p.y == 0) return 0;
	if(p.x > 0) return 0;
	return 1;
}

void werf::build(const char * n)
{
	ifstream read;
	int startup, recovery, countFrames = -1;
	char fname[40];
	char buffer[100];
	buffer[0] = '\0';

	sprintf(fname, "%s.mv", n);
	read.open(fname);
	assert(!read.fail());
	
	while(read.get() != ':'); read.ignore();
	name = new char[strlen(n)+1];
	sprintf(name, "%s", n);

	while(read.get() != ':'); read.ignore();
	read >> tolerance;
	while(read.get() != ':'); read.ignore();
	read >> activation;
	while(read.get() != ':'); read.ignore();
	read >> hits;

	state = new cancelField[hits+1];
	gain = new int[hits+1];

	while(read.get() != ':'); read.ignore();
	read >> frames;
	
	if(hits > 0) {
		totalStartup = new int[hits];
		stats = new hStat[hits];
	} else {
		totalStartup = NULL;
		stats = NULL;
	}
	int active[hits];

	for(int i = 0; i < hits; i++){
		while(read.get() != ':'); read.ignore();
		read >> startup;
		countFrames += startup;
		totalStartup[i] = countFrames;
		while(read.get() != ':'); read.ignore();
		read >> active[i];
		countFrames += active[i];
	}
 
	while(read.get() != ':'); read.ignore();
	read >> recovery;

	while(read.get() != ':'); read.ignore();
	read >> allowed.i;

	for(int i = 0; i < hits+1; i++){
		while(read.get() != ':'); read.ignore();
		read >> state[i].i;
	}
	
	for(int i = 0; i < hits; i++){
		while(read.get() != ':'); read.ignore();
		read >> stats[i].damage;
	}
	
	for(int i = 0; i < hits; i++){
		while(read.get() != ':'); read.ignore();
		read >> stats[i].stun;
	}

	for(int i = 0; i < hits; i++){
		while(read.get() != ':'); read.ignore();
		read >> stats[i].untech;
	}

	for(int i = 0; i < hits; i++){
		while(read.get() != ':'); read.ignore();
		read >> stats[i].push;
	}

	for(int i = 0; i < hits; i++){
		while(read.get() != ':'); read.ignore();
		read >> stats[i].lift;
	}

	for(int i = 0; i < hits; i++){
		while(read.get() != ':'); read.ignore();
		read >> stats[i].blowback;
	}

	for(int i = 0; i < hits+1; i++){
		while(read.get() != ':'); read.ignore();
		read >> gain[i];
	}

	while(read.get() != ':'); read.ignore();
	read >> cost;

	while(read.get() != ':'); read.ignore();
	read >> xRequisite;

	while(read.get() != ':'); read.ignore();
	read >> yRequisite;

	while(read.get() != ':'); read.ignore();
	read >> startPosX;

	while(read.get() != ':'); read.ignore();
	read >> startPosY;

	while(read.get() != ':'); read.ignore();
	read >> blockState.i;

	while(read.get() != ':'); read.ignore();
	read.getline(buffer, 100);

	parseProperties(buffer);

	collision = new SDL_Rect[frames];
	hitbox = new SDL_Rect*[frames];
	hitComplexity = new int[frames];
	hitreg = new SDL_Rect*[frames];
	regComplexity = new int[frames];
	delta = new SDL_Rect*[frames];
	deltaComplexity = new int[frames];

	currentHit = 0;

	for(int i = 0; i < frames; i++){
		while(read.get() != '$'); read.ignore(2);
		read >> collision[i].x >> collision[i].y >> collision[i].w >> collision[i].h;
		while(read.get() != '$'); read.ignore(2);
		read.get(buffer, 100, '\n');
		regComplexity[i] = aux::defineRectArray(buffer, hitreg[i]);
		while(read.get() != '$'); read.ignore(2);
		read.get(buffer, 100, '\n');
		deltaComplexity[i] = aux::defineRectArray(buffer, delta[i]);
		if(hits > 0){
			if(i > totalStartup[currentHit] && i <= totalStartup[currentHit]+active[currentHit]){
				while(read.get() != '$'); read.ignore(2);
				read.get(buffer, 100, '\n');
				hitComplexity[i] = aux::defineRectArray(buffer, hitbox[i]);
				if(i == totalStartup[currentHit]+active[currentHit]) currentHit++;
			} else {
				hitComplexity[i] = 1;
				hitbox[i] = new SDL_Rect[1];
				hitbox[i][0].x = 0; hitbox[i][0].y = 0; hitbox[i][0].w = 0; hitbox[i][0].h = 0;
			}
		} else {
			hitComplexity[i] = 1;
			hitbox[i] = new SDL_Rect[1];
			hitbox[i][0].x = 0; hitbox[i][0].y = 0; hitbox[i][0].w = 0; hitbox[i][0].h = 0;
		}
	}
	next = NULL;
	read.close();

	for(int i = 0; i < 5; i++)
		button[i] = 0;
	int r = strlen(name);
	for(int i = 0; i < r; i++){
		switch(name[i]){
		case 'A':
			button[0] = 1;
			break;
		case 'B':
			button[1] = 1;
			break;
		case 'C':
			button[2] = 1;
			break;
		case 'D':
			button[3] = 1;
			break;
		case 'E':
			button[4] = 1;
			break;
		default:
			break;
		}

	}
	SDL_Surface *temp;
	width = new int[frames];
	height = new int[frames];
	sprite = new GLuint[frames];
	for(int i = 0; i < frames; i++){
		sprintf(fname, "%s#%i.png", n, i);
		temp = aux::load_image(fname);
		if(!temp){
			width[i] = 0;
			height[i] = 0;
			sprite[i] = 0;
		} else {
			width[i] = temp->w;
			height[i] = temp->h;
			sprite[i] = aux::surface_to_texture(temp);
		}
//		sprintf(fname, "%s#%iF.png", n, i);
//		fSprite[i] = aux::load_image(fname);
	}
}
