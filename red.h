#include "character.h"
class red : public character{
public:
	red();
	void touch(void*);
	action * createMove(char*);
	void drawMeters(int);
	void tick();
	void step();
	void init();
};

class redCancel : virtual public special{
public:
	redCancel();
	~redCancel();
	redCancel(char* n);
	bool check(bool[], bool[], int, int, int*, SDL_Rect&);
	void execute(action *, int *&);
};

class redSuper : virtual public super{
public:
	redSuper();
	~redSuper();
	redSuper(char* n) { build(n); init(); }
	int arbitraryPoll(int);
};
