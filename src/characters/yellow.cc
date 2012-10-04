#include "yellow.h"
yellow::yellow(){
	head = new actionTrie;
	airHead = new actionTrie;
	meter = new int[5];
	build("Yellow", "Yellow");
}

void yellow::resetAirOptions()
{
	meter[2] = 1;
	meter[3] = 2;
}

void yellow::init(action *& cMove)
{
	character::init(cMove);
	meter[4] = 0;
}

void yellow::tick()
{
	character::tick();
	if(meter[4] > 0){ 
		meter[4]--;
		if(meter[4] == 0){ 
			meter[4] = -360;
		}
	}
}

void yellow::step(action *& cMove, int &f, int &freeze)
{
	if(meter[4] < 0) meter[4]++;
	character::step(cMove, f, freeze);
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

void yellow::drawMeters(int ID, float scalingFactor, int hidden)
{
	int color;
	character::drawMeters(ID, scalingFactor, hidden);
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
	glRectf((GLfloat)(c1.x)*scalingFactor, (GLfloat)(c1.y)*scalingFactor, (GLfloat)(c1.x + c1.w)*scalingFactor, (GLfloat)(c1.y + c1.h)*scalingFactor);
//	SDL_FillRect(screen, &c1, SDL_MapRGB(screen->format, 0, 0, color1));
//	SDL_FillRect(screen, &c2, SDL_MapRGB(screen->format, color2, 0, color2)); 
}

int yellow::takeHit(action *& cMove, hStat & s, int b, int &f, int &c, int &h, int &p)
{
	int x = character::takeHit(cMove, s, b, f, c, h, p);
	if(p == 1 && meter[4] > 0) meter[4] = 0;
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

bool flashStep::check(SDL_Rect& p, int meter[])
{
	if(meter[4] < 1) return 0;
	else return action::check(p, meter);
}

bool flashSummon::check(SDL_Rect& p, int meter[])
{
	if(meter[4] < 0) return 0;
	else return action::check(p, meter);
}

void flashSummon::execute(action * last, int *& meter, int &f, int &c, int &h)
{
	if(meter[4] > 0) uFlag = 1;
	else uFlag = 0;
	action::execute(last, meter, f, c, h);
}

void flashStep::execute(action * last, int *& meter, int &f, int &c, int &h)
{
	meter[4] -= flashMeterCost;
	if(meter[4] > 540) meter[4] = 540;
	else if(meter[4] < 0) meter[4] = -360;
	action::execute(last, meter, f, c, h);
}

void flashSummon::step(int *& meter, int &f)
{
	if(uFlag){
		if(f == frames - 1) meter[4] = 0;
	} else meter[4] += flashMeterGain / frames + 1;
	if(meter[4] > 540) meter[4] = 540;
	action::step(meter, f);
}
