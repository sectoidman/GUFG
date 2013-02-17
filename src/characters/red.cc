#include "../engine/interface.h"
red::red()
{
	airHead = new actionTrie;
	head = new actionTrie;
	build("Red", "Red");
	backup = new instance;
}

int * red::generateMeter()
{
	int * meter;
	meter = new int[6];
	return meter;
}

void red::tick(int *& metre)
{
	character::tick(metre);
	if(metre[4] < 540) metre[4]++;
	if(metre[4] < 0) metre[4] = 0;
}

void red::step(status& current, int *& metre)
{
	if(metre[5] > 0) metre[5]--;
	character::step(current, metre);
}

void red::init(int *& metre)
{
	character::init(metre);
	metre[4] = 540;
	metre[5] = 0;
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

bool redCancel::check(SDL_Rect& p, int meter[])
{
//	if(meter[1] < cost) return 0;
//	if(meter[4] < 270) return 0;
	if(meter[5] > 0) return 0;
	return action::check(p, meter);
}

void redCancel::execute(action * last, int *& meter, int &f, int &c, int &h)
{
	meter[2] = 1;
	meter[3] = 1;
//	meter[4] -= 270;
	meter[5] = 16;
	action::execute(last, meter, f, c, h);
}

int redSuper::arbitraryPoll(int q, int f)
{
	if(q == 31) return 11;
	else return action::arbitraryPoll(q, f);
}

redCancel::~redCancel() {}
redSuper::~redSuper() {}
