#include "yellow.h"
yellow::yellow(){
	head = new actionTrie;
	airHead = new actionTrie;
	meter = new int[4];
	build("Yellow", "Yellow");
}

void yellow::resetAirOptions()
{
	meter[1] = 1;
	meter[2] = 2;
}

void yellow::init(action *& cMove)
{
	character::init(cMove);
	meter[3] = 0;
}

void yellow::tick()
{
	character::tick();
	if(meter[3] > 0){ 
		meter[3]--;
		if(meter[3] == 0){ 
			meter[3] = -360;
		}
	}
}

void yellow::step(action *& cMove, int &f, int &freeze)
{
	if(meter[3] < 0) meter[3]++;
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

void yellow::drawMeters(int ID, float scalingFactor)
{
	int color;
	character::drawMeters(ID, scalingFactor);
	SDL_Rect c1;
	if(meter[3] >= 0){
		c1.w = meter[3]/3*2; 
		color = 255;
	} else {
		c1.w = 360 + (meter[3]);
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
	if(x == 1 && meter[3] > 0) meter[3] = 0;
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

bool flashStep::check(SDL_Rect& p, int resource[])
{
	if(resource[3] < 1) return 0;
	else return action::check(p, resource);
}

bool flashSummon::check(SDL_Rect& p, int resource[])
{
	if(resource[3] < 0) return 0;
	else return action::check(p, resource);
}

void flashSummon::execute(action * last, int *& resource)
{
	if(resource[3] > 0) uFlag = 1;
	else uFlag = 0;
	action::execute(last, resource);
}

void flashStep::execute(action * last, int *& resource)
{
	resource[3] -= flashMeterCost;
	if(resource[3] > 540) resource[3] = 540;
	else if(resource[3] < 0) resource[3] = -360;
	action::execute(last, resource);
}

void flashSummon::step(int *& resource, int &f)
{
	if(uFlag){
		if(f == frames - 1) resource[3] = 0;
	} else resource[3] += flashMeterGain / frames + 1;
	if(resource[3] > 540) resource[3] = 540;
	action::step(resource, f);
}
