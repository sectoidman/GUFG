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
	std::vector<int> meter (5);
	return meter;
}

void yellow::init(std::vector<int>& meter)
{
	character::init(meter);
	meter[4] = 0;
}

void yellow::tick(std::vector<int>& meter)
{
	character::tick(meter);
	if(meter[4] > 0){ 
		meter[4]--;
		if(meter[4] == 0){ 
			meter[4] = -360;
		}
	}
}

void yellow::step(status &current, std::vector<int>& meter)
{
	if(meter[4] < 0) meter[4]++;
	character::step(current, meter);
}

action * yellow::createMove(char * fullName)
{
	char * token;
	char type[2] = {fullName[0], fullName[1]};
	char actionName[151];
	char buffer[101];
	strcpy (buffer, fullName);

	token = strtok(fullName, " \t-@?_%&$!\n");
	sprintf(actionName, "%s/%s", name, token);

	action * m;
	switch(type[0]){
	case '$':
		m = new flashStep(actionName);
		break;
	case '&':
		m = new flashSummon(actionName);
		break;
	default:
		m = character::createMove(buffer);
		break;
	}
	return m;
}

void yellow::drawMeters(int ID, int hidden, std::vector<int> meter)
{
	int color;
	character::drawMeters(ID, hidden, meter);
	SDL_Rect c1;
	if(meter[4] >= 0){
		c1.w = meter[4]/3*2; 
		color = 255;
	} else {
		c1.w = 360 + (meter[4]);
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
	if(hitType == 1 && meter[4] > 0) meter[4] = 0;
	return x;
}

flashStep::flashStep() {}
flashSummon::flashSummon() {}

flashStep::flashStep(const char * n)
{
	build(n);
}

flashSummon::flashSummon(const char * n)
{
	build(n);
}

bool flashStep::setParameter(char * buffer)
{
	char savedBuffer[100];
	strcpy(savedBuffer, buffer);

	char * token = strtok(buffer, "\t: \n-");

	if(!strcmp("FlashCost", token)){
		token = strtok(NULL, "\t: \n-");
		flashMeterCost = atoi(token); 
		return 1;
	} else return airMove::setParameter(savedBuffer);
}

bool flashSummon::setParameter(char * buffer)
{
	char savedBuffer[100];
	strcpy(savedBuffer, buffer);

	char * token = strtok(buffer, "\t: \n-");
	
	if(!strcmp("FlashGain", token)){
		token = strtok(NULL, "\t: \n-");
		flashMeterGain = atoi(token); 
		return 1;
	} else return action::setParameter(savedBuffer);
}

bool flashStep::check(SDL_Rect& p, std::vector<int> meter)
{
	if(meter[4] < 1) return 0;
	else return action::check(p, meter);
}

bool flashSummon::check(SDL_Rect& p, std::vector<int> meter)
{
	if(meter[4] < 0) return 0;
	else return action::check(p, meter);
}

void flashSummon::execute(action * last, std::vector<int>& meter, int &f, int &c, int &h)
{
	if(meter[4] > 0) uFlag = 1;
	else uFlag = 0;
	action::execute(last, meter, f, c, h);
}

void flashStep::execute(action * last, std::vector<int>& meter, int &f, int &c, int &h)
{
	meter[4] -= flashMeterCost;
	if(meter[4] > 540) meter[4] = 540;
	else if(meter[4] < 0) meter[4] = -360;
	action::execute(last, meter, f, c, h);
}

void flashSummon::step(std::vector<int>& meter, int &f, int &c, int &h)
{
	if(uFlag){
		if(f == frames - 1) meter[4] = 0;
	} else meter[4] += flashMeterGain / frames + 1;
	if(meter[4] > 540) meter[4] = 540;
	action::step(meter, f, c, h);
}
