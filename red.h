#ifndef RED_INCLUDED
#define RED_INCLUDED
#include "character.h"
class red : public character{
public:
	red();
	red(red*);
	void touch(void*);
	action * createMove(char*);
	void drawMeters(int);
	void tick();
	bool step(int&);
	void init();
	red * backup;
};

class redCancel : virtual public special{
public:
	redCancel();
	~redCancel();
	redCancel(const char* n);
	bool check(bool[], bool[], int, int, int*, SDL_Rect&);
	void execute(action *, int *&);
};

class redSuper : virtual public super{
public:
	redSuper();
	~redSuper();
	redSuper(const char* n) { build(n); init(); }
	int arbitraryPoll(int, int);
};
#endif
