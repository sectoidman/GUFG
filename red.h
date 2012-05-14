#include "character.h"
class red : public character{
public:
	red();
	void touch(void*);
	int cancelMeter;
	void drawMeters(SDL_Surface *&, int);
	void tick();
	void init();
};
