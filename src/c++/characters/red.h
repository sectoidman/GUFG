#ifndef RED_INCLUDED
#define RED_INCLUDED
#include "../engine/character.h"
class red : public character{
public:
	red();
	void touch(void*);
	action * createMove(char*);
	void drawMeters(int, float, int, int*);
	void tick(int *&);
	int * generateMeter();
	void step(action *&, int&, int&, int*&);
	void init(int *&);
	instance * backup;
};

class redCancel : virtual public special{
public:
	redCancel();
	~redCancel();
	redCancel(const char* n);
	bool check(SDL_Rect&, int[]);
	void execute(action *, int *&, int&, int&, int&);
};

class redSuper : virtual public super{
public:
	redSuper();
	~redSuper();
	redSuper(const char* n) { build(n); }
	int arbitraryPoll(int, int);
};
#endif
