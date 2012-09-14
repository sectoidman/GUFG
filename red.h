#ifndef RED_INCLUDED
#define RED_INCLUDED
#include "character.h"
class red : public character{
public:
	red();
	red(red*);
	void touch(void*);
	action * createMove(char*);
	void drawMeters(int, float);
	void tick();
	void step(action *&, int&, int&);
	void init(action *&);
	red * backup;
};

class redCancel : virtual public special{
public:
	redCancel();
	~redCancel();
	redCancel(const char* n);
	bool check(SDL_Rect&, int[]);
	void execute(action *, int *&);
};

class redSuper : virtual public super{
public:
	redSuper();
	~redSuper();
	redSuper(const char* n) { build(n); }
	int arbitraryPoll(int, int);
};
#endif
