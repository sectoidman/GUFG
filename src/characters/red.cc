#include "../engine/interface.h"
red::red()
{
	airHead = new actionTrie;
	head = new actionTrie;
	build("Red", "Red");
	backup = new instance;
}

std::vector<int> red::generateMeter()
{
	std::vector<int> meter (7);
	return meter;
}

void red::tick(std::vector<int>& metre)
{
	character::tick(metre);
	if(metre[5] < 540) metre[5]++;
	if(metre[5] < 0) metre[5] = 0;
}

void red::step(status& current, std::vector<int>& metre)
{
	if(metre[6] > 0) metre[6]--;
	temporalBuffer.push_back(current);
	if(temporalBuffer.size() > 120) temporalBuffer.erase(temporalBuffer.begin());
	character::step(current, metre);
}

void red::init(std::vector<int>& metre)
{
	character::init(metre);
	metre[5] = 540;
	metre[6] = 0;
}

action * red::createMove(std::string key)
{
	tokenizer t(key, " \t-@?_%$!\n");
        t();
	action * m;
	switch(key[0]){
	case '$':
		if(key[1] == '!') m = new redSuper(name, t.current());
		else m = new redCancel(name, t.current());
		break;
	default:
		m = character::createMove(key);
		break;
	}
	if(m->typeKey == '0') m->typeKey = key[0];
	return m;
}

redCancel::redCancel(std::string dir, std::string file) 
{
	build(dir, file); 
}

bool redCancel::check(SDL_Rect& p, std::vector<int> meter)
{
	if(meter[6] > 0) return 0;
	return action::check(p, meter);
}

void redCancel::execute(action * last, std::vector<int>& meter, int &f, int &c, int &h)
{
	meter[2] = 1;
	meter[3] = 1;
	meter[6] = 16;
	action::execute(last, meter, f, c, h);
}

int redCancel::arbitraryPoll(int q, int f)
{
	if(q == 33) return 1;
	else return action::arbitraryPoll(q, f);
}
int redSuper::arbitraryPoll(int q, int f)
{
	if(q == 31) return 11;
	else if(q == 32) return 1;
	else return action::arbitraryPoll(q, f);
}

redCancel::~redCancel() {}
redSuper::~redSuper() {}
