#include "action.h"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <math.h>


airMove::airMove(const char * n)
{
	build(n);
	init();
}

action * airMove::land()
{
	init();
	return landing;
}

void airMove::build(const char * n)
{
	action::build(n);
	landing = NULL;
}

void airMove::feed(action * c, int i)
{
	if(i == 0) next = c;
	if(i == 1) landing = c;
}

airUtility::airUtility(const char * n)
{
	airMove::build(n);
	airMove::init();
}

bool airUtility::check(bool* pos, bool* neg, int t, int f, int* resource, SDL_Rect &p) //Check to see if the action is possible right now.
{
	if(abs(delta[0][0].y) > abs(delta[0][0].x) && resource[1] < 1) return 0;
	else if(abs(delta[0][0].y) < abs(delta[0][0].x) && resource[2] < 1) return 0;
	return utility::check(pos, neg, t, f, resource, p);
}

void airUtility::execute(action * last, int *& resource){
	if(abs(delta[0][0].y) > abs(delta[0][0].x)) resource[1]--;
	else if(abs(delta[0][0].y) < abs(delta[0][0].x)) resource[2]--;
	action::execute(last, resource);
}

airLooping::airLooping(const char * n)
{
	airMove::build(n);
	airMove::init();
}

untechState::untechState(const char* n)
{
	airMove::build(n);
	currentFrame = 0;
}
