#include "character.h"
class red : public character{
public:
	red();
	void touch(void*);
	move * createMove(char*, char*);
	void drawMeters(SDL_Surface *&, int);
	void tick();
	void init();
};

