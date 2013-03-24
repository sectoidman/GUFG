#include "action.h"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include "tokenizer.h"

airMove::airMove(std::string dir, std::string file)
{
	build(dir, file);
}

action * airMove::land(int &f, int &c, int &h)
{
	f = 0;
	c = 0;
	h = 0;
	return landing;
}

void airMove::zero()
{
	landing = nullptr;
	tempLanding.clear();
	action::zero();
}

void airMove::feed(action * c, int code, int i)
{
	if(code == 1){ 
		landing = c;
	} else action::feed(c, code, i);
}

std::string airMove::request(int code, int i)
{
	if(code == 1) return tempLanding;
	else return action::request(code, i); 
}

bool airMove::setParameter(string buffer)
{
	tokenizer t(buffer, "\t: \n");
	if(t() == "Landing"){
		tempLanding = t("\t: \n");
		return true;
	} else return action::setParameter(buffer);
}

airUtility::airUtility(std::string dir, std::string file)
{
	airMove::build(dir, file);
}

bool airUtility::check(SDL_Rect &p, std::vector<int> meter) //Check to see if the action is possible right now.
{
	if(abs(delta[0][0].y) > abs(delta[0][0].x) && meter[2] < 1) return 0;
	else if(abs(delta[0][0].y) < abs(delta[0][0].x) && meter[3] < 1) return 0;
	return action::check(p, meter);
}

void airUtility::execute(action * last, std::vector<int>& meter, int &f, int &c, int &h){
	if(abs(delta[0][0].y) > abs(delta[0][0].x)) meter[2]--;
	else if(abs(delta[0][0].y) < abs(delta[0][0].x)) meter[3]--;
	action::execute(last, meter, f, c, h);
}

airLooping::airLooping(std::string dir, std::string file)
{
	airMove::build(dir, file);
}

untechState::untechState(std::string dir, std::string file)
{
	airMove::build(dir, file);
}
