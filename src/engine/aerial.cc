#include "action.h"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <math.h>


airMove::airMove(const char * n)
{
	build(n);
}

action * airMove::land(int &f, int &c, int &h)
{
	f = 0;
	c = 0;
	h = 0;
	return landing;
}

void airMove::build(const char * n)
{
	action::build(n);
	landing = NULL;
}

void airMove::feed(action * c, int code, int i)
{
	if(code == 1){ 
		landing = c;
		if(tempLanding){ 
			delete [] tempLanding;
		}
	} else action::feed(c, code, i);
}

char * airMove::request(int code, int i)
{
	if(code == 1){
		return tempLanding;
	}
	else return action::request(code, i); 
}

bool airMove::setParameter(char * buffer)
{
	char savedBuffer[100];
	strcpy(savedBuffer, buffer);

	char * token = strtok(buffer, "\t: \n");

	if(!strcmp("Landing", token)){
		token = strtok(NULL, "\t: \n");
		tempLanding = new char[strlen(token)+1];
		strcpy(tempLanding, token);
		return 1;
	} else return action::setParameter(savedBuffer);
}

airUtility::airUtility(const char * n)
{
	airMove::build(n);
}

bool airUtility::check(SDL_Rect &p, int resource[]) //Check to see if the action is possible right now.
{
	if(abs(delta[0][0].y) > abs(delta[0][0].x) && resource[1] < 1) return 0;
	else if(abs(delta[0][0].y) < abs(delta[0][0].x) && resource[2] < 1) return 0;
	return action::check(p, resource);
}

void airUtility::execute(action * last, int *& resource, int &f, int &c, int &h){
	if(abs(delta[0][0].y) > abs(delta[0][0].x)) resource[1]--;
	else if(abs(delta[0][0].y) < abs(delta[0][0].x)) resource[2]--;
	action::execute(last, resource, f, c, h);
}

airLooping::airLooping(const char * n)
{
	airMove::build(n);
}

untechState::untechState(const char* n)
{
	airMove::build(n);
}
