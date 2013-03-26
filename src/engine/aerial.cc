#include "action.h"
#include "tokenizer.h"
#include <fstream>
#include <iostream>
#include <math.h>

airMove::airMove(string dir, string file)
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

string airMove::request(int code, int i)
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

airUtility::airUtility(string dir, string file)
{
	airMove::build(dir, file);
}

bool airUtility::check(SDL_Rect &p, vector<int> meter) //Check to see if the action is possible right now.
{
	if(abs(delta[0][0].y) > abs(delta[0][0].x) && meter[2] < 1) return 0;
	else if(abs(delta[0][0].y) < abs(delta[0][0].x) && meter[3] < 1) return 0;
	return action::check(p, meter);
}

void airUtility::execute(action * last, vector<int>& meter, int &f, int &c, int &h){
	if(abs(delta[0][0].y) > abs(delta[0][0].x)) meter[2]--;
	else if(abs(delta[0][0].y) < abs(delta[0][0].x)) meter[3]--;
	action::execute(last, meter, f, c, h);
}

airLooping::airLooping(string dir, string file)
{
	airMove::build(dir, file);
}

untechState::untechState(string dir, string file)
{
	airMove::build(dir, file);
}
