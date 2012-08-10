#include "interface.h"
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

negNormal::negNormal(const char * n)
{
	build(n);
	init();
}

bool negNormal::check(bool pos[5], bool neg[5], int t, int f, int* resource, SDL_Rect &p)
{
	for(int i = 0; i < 5; i++){
		if(button[i] == 1){
			if(!neg[i]) return 0;
		}
	}
	if(t > tolerance) return 0;
	if(f > activation) return 0;
	if(resource[0] < cost) return 0;
	return 1;
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

bool werf::check(SDL_Rect &p)
{
	if(p.w > xRequisite) return 0;
	if(p.y != 0) return 0;
	if(p.x > 0) return 0;
	return 1;
}

bool luftigeWerf::check(SDL_Rect &p)
{
	if(p.w > xRequisite) return 0;
	if(p.h > yRequisite) return 0;
	if(p.y == 0) return 0;
	if(p.x > 0) return 0;
	return 1;
}

bool werf::setParameter(char * buffer)
{
	char savedBuffer[100];
	strcpy(savedBuffer, buffer);

	char * token = strtok(buffer, "\t: \n-");

	if(!strcmp("Proximity", token)){
		token = strtok(NULL, "\t: \n");
		xRequisite = atoi(token); 

		token = strtok(NULL, "\t: \n");
		yRequisite = atoi(token); 
		return 1;
	} else if (!strcmp("Position", token)){
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

summon::summon(const char * n)
{
	build(n);
	init();
}

airSummon::airSummon(const char * n)
{
	build(n);
	init();
}

avatar * summon::spawn()
{
	return payload;
}

void summon::zero()
{
	spawnFrame = 0;
	spawnTrackY = 0;
	spawnTrackX = 0;
	spawnPosY = 0;
	spawnPosX = 0;
	action::zero();
}

bool summon::setParameter(char * buffer)
{
	char savedBuffer[100];
	strcpy(savedBuffer, buffer);

	char * token = strtok(buffer, "\t: \n-");

	if(!strcmp("SpawnPosition", token)){
		token = strtok(NULL, "\t: \n");
		spawnPosX = atoi(token);

		token = strtok(NULL, "\t: \n");
		spawnPosY = atoi(token);
		return 1;
	} else if(!strcmp("SpawnsOn", token)){
		token = strtok(NULL, "\t: \n");
		spawnFrame = atoi(token);
		return 1;
	} else if(!strcmp("Payload", token)){
		token = strtok(NULL, "\t: \n");
		tempPayload = new char[strlen(token)+1];
		strcpy(tempPayload, token);
		return 1;
	} else return action::setParameter(savedBuffer);
}

int summon::arbitraryPoll(int q)
{
	if(currentFrame == spawnFrame){
		switch(q){
		case 50:
			return 1;
		case 51:
			if(spawnTrackX) return 1;
			else break;
		case 52:
			if(spawnTrackY) return 1;
			else break;
		case 53:
			return spawnPosX;
		case 54:
			return spawnPosY;
		default:
			break;
		}
	}
	return action::arbitraryPoll(q);
}

void airSummon::zero()
{
	summon::zero();
	airMove::zero();
}

bool airSummon::setParameter(char * buffer)
{
	bool x;
	char savedBuffer[100];
	strcpy(savedBuffer, buffer);
	x = summon::setParameter(buffer);
	if (!x) x = airMove::setParameter(savedBuffer);
	return x;
}

char * summon::request(int code, int i){
	if(code == 4)
		return tempPayload;
	else return action::request(code, i);
}

void summon::generate(const char* directory, const char* name)
{
	payload = new projectile(directory, name);
}

char* airSummon::request(int code, int i)
{
	char * x;
	x = summon::request(code, i);
	if(x == NULL) x = airMove::request(code, i);
	return x;
}
