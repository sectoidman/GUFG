#include <SDL/SDL.h>
#include <vector>
#ifndef CONTROLLER
#define CONTROLLER
struct keySetting{
	keySetting();
	SDL_Event trigger;
	int effect;
};

class controller{
public:
	virtual void setKey(int);
	virtual void readEvent(SDL_Event &, std::vector<bool>&, std::vector<int>&, std::vector<bool>&);
	virtual void genEvent(std::vector<bool>&, std::vector<int>&, std::vector<bool>&) {}
	void writeConfig(int);
	bool readConfig(int);
	virtual bool setKey(int, SDL_Event);
	virtual bool same(SDL_Event);
	virtual int tap(SDL_Event);
	virtual void swapKey(int, SDL_Event);
	std::vector<keySetting*> input;
	std::vector<const char*> inputName;//Input names. This is really just for housekeeping.
};
#endif
