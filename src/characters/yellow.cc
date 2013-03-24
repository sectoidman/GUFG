#include "yellow.h"
yellow::yellow(){
	head = new actionTrie;
	airHead = new actionTrie;
	build("Yellow", "Yellow");
}

void yellow::resetAirOptions(std::vector<int>& meter)
{
	meter[2] = 1;
	meter[3] = 2;
}

std::vector<int> yellow::generateMeter()
{
	std::vector<int> meter (6);
	return meter;
}

void yellow::init(std::vector<int>& meter)
{
	character::init(meter);
	meter[5] = 0;
}

void yellow::tick(std::vector<int>& meter)
{
	character::tick(meter);
	if(meter[5] > 0){ 
		meter[5]--;
		if(meter[5] == 0){ 
			meter[5] = -360;
		}
	}
}

void yellow::step(status &current, std::vector<int>& meter)
{
	if(meter[5] < 0) meter[5]++;
	character::step(current, meter);
}

action * yellow::createMove(std::string key)
{
	tokenizer t(key, " \t-@?_%&$!\n");
        string token = t.current();
	action * m;
	switch(key[0]){
	case '$':
		m = new flashStep(name, token);
		break;
	case '&':
		m = new flashSummon(name, token);
		break;
	default:
		m = character::createMove(key);
		break;
	}
	if(m->typeKey == '0') m->typeKey = key[0];
	return m;
}

void yellow::drawMeters(int ID, int hidden, std::vector<int> meter)
{
	int color;
	character::drawMeters(ID, hidden, meter);
	SDL_Rect c1;
	if(meter[5] >= 0){
		c1.w = meter[5]/3*2; 
		color = 255;
	} else {
		c1.w = 360 + (meter[5]);
		color = 0;
	}
	if(ID == 1){
		c1.x = 220; 
	} else { 
		c1.x = 1020 + (360 - c1.w);
	}
	c1.h = 10;
	c1.y = 876;
	glColor4f(1.0f, (float)color, 0.0f, 1.0f);
	glRectf((GLfloat)(c1.x), (GLfloat)(c1.y), (GLfloat)(c1.x + c1.w), (GLfloat)(c1.y + c1.h));
}

int yellow::takeHit(status& current, hStat & s, int blockType, int &hitType, std::vector<int> & meter)
{
	int x = character::takeHit(current, s, blockType, hitType, meter);
	if(hitType == 1 && meter[5] > 0) meter[5] = 0;
	return x;
}

flashStep::flashStep() {}
flashSummon::flashSummon() {}

flashStep::flashStep(std::string dir, std::string file)
{
	build(dir, file);
}

flashSummon::flashSummon(std::string dir, std::string file)
{
	build(dir, file);
}

void flashSummon::zero()
{
	flashMeterGain = 0;
	action::zero();
	uFlag = 0;
}

bool flashSummon::setParameter(string buffer)
{
	tokenizer t(buffer, "\t:+\n");
	if(t() == "FlashGain"){
		flashMeterGain = stoi(t("\t:\n"));
		return 1;
	} else return action::setParameter(buffer);
}

bool flashStep::check(SDL_Rect& p, std::vector<int> meter)
{
	if(meter[5] < 1) return 0;
	else return action::check(p, meter);
}

bool flashSummon::check(SDL_Rect& p, std::vector<int> meter)
{
	if(meter[5] < 0) return 0;
	if(meter[5] > 0) uFlag = 1;
	if(uFlag) meter[1] += cost;
	bool ret = action::check(p, meter);
	if(uFlag) meter[1] -= cost;
	uFlag = 0;
	return ret;
}

int flashSummon::arbitraryPoll(int q, int f)
{
	if(uFlag && q == 2) return 0;
	else return action::arbitraryPoll(q,f);
}

void flashSummon::execute(action * last, std::vector<int>& meter, int &f, int &c, int &h)
{
	if(meter[5] > 0) uFlag = 1;
	else uFlag = 0;
	action::execute(last, meter, f, c, h);
	if(uFlag){
		meter[1] += cost;
		meter[4] -= cost;
	}
}

void flashStep::execute(action * last, std::vector<int>& meter, int &f, int &c, int &h)
{
	meter[5] -= 10;
	if(meter[5] > 540) meter[5] = 540;
	else if(meter[5] < 0) meter[5] = -360;
	action::execute(last, meter, f, c, h);
}

void flashSummon::step(std::vector<int>& meter, status &current)
{
	if(uFlag){
		if(current.frame == frames - 1) meter[5] = 0;
	} else meter[5] += flashMeterGain / frames + 1;
	if(meter[5] > 540) meter[5] = 540;
	action::step(meter, current);
}
