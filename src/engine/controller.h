#include <SDL/SDL.h>
#include <string>
#include <vector>

#ifndef ___controller
#define ___controller

using std::string;
using std::vector;

struct frame;
class script;

struct keySetting{
	keySetting();
	SDL_Event trigger;
	int effect;
};

class controller{
public:
	virtual void setKey(int);
	virtual void readEvent(SDL_Event &, frame&);
	void writeConfig(int);
	bool readConfig(int);
	virtual bool setKey(int, SDL_Event);
	virtual bool same(SDL_Event);
	virtual int tap(SDL_Event);
	virtual void swapKey(int, SDL_Event);
	vector<keySetting*> input;
	vector<string> inputName;//This is really just for housekeeping.
	vector<script*> macro;
	vector<int> pattern;
	script * patternMatch(int);
	script * currentMacro;
	int iterator;
};
#endif
