#include "character.h"
class red : public character{
public:
	red();
	void touch(void*);
	move * createMove(char*);
	void drawMeters(int);
	void tick();
	void init();
};

class redCancel : virtual public special{
public:
	redCancel();
	~redCancel();
	redCancel(char* n);
	bool check(bool[], bool[], int, int, int*, SDL_Rect&);
	void execute(move *, int *&);
};
