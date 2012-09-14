#include "interface.h"
red::red()
{
	delete [] meter;
	
	meter = new int[5];
	
	airHead = new actionTrie;
	head = new actionTrie;
	build("Red", "Red");
	backup = new red(this);
}

red::red(red* b)
{
	delete [] meter;
	
	meter = new int[5];
	
	airHead = new actionTrie;
	head = new actionTrie;
	build("Red", "Red");
	backup = b;
}

void red::tick()
{
	character::tick();
	if(meter[3] < 540) meter[3]++;
	if(meter[3] < 0) meter[3] = 0;
}

void red::step(action *& cMove, int &f, int &freeze)
{
	if(meter[4] > 0) meter[4]--;
	character::step(cMove, f, freeze);
}

void red::drawMeters(int ID, float scalingFactor)
{
//	int color1, color2;
	character::drawMeters(ID, scalingFactor);
/*	SDL_Rect c1, c2;
	if(meter[3] >= 0){
		c1.w = meter[3]; 
	} else c1.w = 1; 
	if(meter[3] > 270){
		c1.w = 270; c2.w = (meter[3] - 270);
	} else c2.w = 0;
	if(ID == 1){
		c1.x = 265; 
		c2.x = 265;
	}
	else { 
		c1.x = 1065 + (270 - c1.w);
		c2.x = 1065 + (270 - c2.w);
	}
	c1.h = 10; c2.h = 10;
	c1.y = 876; c2.y = 876;
	if(meter[3] >= 270 && meter[4] < 1) color1 = 255;
	else color1 = 127;
	if(meter[3] >= 540 && meter[4] < 1) color2 = 255;
	else color2 = 127;
	glColor4f(0.0f, 0.0f, (float)color1, 1.0f);
	glRectf((GLfloat)(c1.x)*scalingFactor, (GLfloat)(c1.y)*scalingFactor, (GLfloat)(c1.x + c1.w)*scalingFactor, (GLfloat)(c1.y + c1.h)*scalingFactor);
	glColor4f((float)color2, 0.0f, (float)color2, 1.0f);
	glRectf((GLfloat)(c2.x)*scalingFactor, (GLfloat)(c2.y)*scalingFactor, (GLfloat)(c2.x + c2.w)*scalingFactor, (GLfloat)(c2.y + c2.h)*scalingFactor);
//	SDL_FillRect(screen, &c1, SDL_MapRGB(screen->format, 0, 0, color1));
//	SDL_FillRect(screen, &c2, SDL_MapRGB(screen->format, color2, 0, color2));*/
}

void red::init(action *& cMove)
{
	character::init(cMove);
	meter[3] = 540;
	meter[4] = 0;
}

action * red::createMove(char * fullName)
{
	char * token;
	char type[2] = {fullName[0], fullName[1]};
	char actionName[151];
	char buffer[101];
	strcpy (buffer, fullName);

	token = strtok(fullName, " \t-@?_%$!\n");
	sprintf(actionName, "%s/%s", name, token);

	action * m;
	switch(type[0]){
	case '$':
		if(type[1] == '!') m = new redSuper(actionName);
		else m = new redCancel(actionName);
		break;
	default:
		m = character::createMove(buffer);
		break;
	}
	return m;
}

redCancel::redCancel(const char* n) 
{ 
	build(n); 
}

bool redCancel::check(SDL_Rect& p, int resource[])
{
//	if(resource[0] < cost) return 0;
//	if(resource[3] < 270) return 0;
	if(resource[4] > 0) return 0;
	return action::check(p, resource);
}

void redCancel::execute(action * last, int *& resource)
{
	resource[1] = 1;
	resource[2] = 1;
	resource[3] -= 270;
	resource[4] = 16;
	action::execute(last, resource);
}

int redSuper::arbitraryPoll(int q, int f)
{
	if(q == 31) return 11;
	else return super::arbitraryPoll(q, f);
}

redCancel::~redCancel() {}
redSuper::~redSuper() {}
