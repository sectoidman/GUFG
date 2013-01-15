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
//	virtual void writeConfig(int);
//	virtual bool readConfig(int);
	virtual bool setKey(int, SDL_Event);
	std::vector<keySetting*> input;
	std::vector<const char*> inputName;//Input names. This is really just for housekeeping.
};
#endif
