#include "character.h"
class red : public character{
public:
	red();
	void touch(void*);
	move * createMove(char*);
	void drawMeters(SDL_Surface *&, int);
	void tick();
	void init();
};

class redCancel : public special{
public:
	redCancel();
	~redCancel();
	redCancel(char* n);
	virtual bool check(bool[], bool[], int, int, int*);
	void execute(move *, int *&);
};
