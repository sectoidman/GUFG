#include "character.h"
class red : public character{
public:
	red();
	void touch(void*);
	void drawMeters(SDL_Surface *&, int);
	void tick();
	void init();
};

class redCancel : public special{
public:
	redCancel();
	bool check(bool[], bool[], int, int, int*);
	void execute(move*, int*&);
};
