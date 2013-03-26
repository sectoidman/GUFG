#include "interface.h"
#include <fstream>
#include <iostream>

special::special(string dir, string file)
{
	build(dir, file);
}

negNormal::negNormal(string dir, string file)
{
	build(dir, file);
}

bool negNormal::activate(vector<int> inputs, int pattern, int t, int f, vector<int> meter, SDL_Rect &p)
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

bool special::activate(vector<int> inputs, int pattern, int t, int f, vector<int> meter, SDL_Rect &p)
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

bool mash::activate(vector <int> inputs, int pattern, int t, int f, vector<int> meter, SDL_Rect &p)
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

bool releaseCheck::activate(vector<int> inputs, int pattern, int t, int f, vector<int> meter, SDL_Rect &p){
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

bool mash::setParameter(string buffer)
{
	tokenizer t(buffer, "\t: \n-");
	if(t() == "Buttons"){
		buttons = stoi(t("\t: \n-"));
		return true;
	} else return action::setParameter(buffer);
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

bool werf::check(SDL_Rect &p, vector<int> meter)
{
	if(p.y != 0) return 0;
	if(p.x > 0) return 0;
	return action::check(p, meter);
}

bool luftigeWerf::check(SDL_Rect &p, vector<int> meter)
{
	if(p.y == 0) return 0;
	if(p.x > 0) return 0;
	return action::check(p, meter);
}

bool werf::setParameter(string buffer)
{
	tokenizer t(buffer, "\t: \n-");
	if (t() == "Position"){
		startPosX = stoi(t());
		startPosY = stoi(t("\t: \n"));
		return true;
	} else return action::setParameter(buffer);
}

bool luftigeWerf::setParameter(string buffer)
{
	tokenizer t(buffer, "\t: \n-");
	if(t() == "Landing") return airMove::setParameter(buffer);
	else return werf::setParameter(buffer);
}
