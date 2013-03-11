#ifndef RED_INCLUDED
#define RED_INCLUDED
#include "../engine/character.h"
class red : public character{
public:
	red();
	void touch(void*);
	action * createMove(char*);
	void tick(std::vector<int>&);
	std::vector<int> generateMeter();
	void step(status&, std::vector<int>&);
	void init(std::vector<int>&);
	std::vector<status> temporalBuffer;
	instance * backup;
};

class redCancel : virtual public special{
public:
	redCancel();
	~redCancel();
	redCancel(const char* n);
	bool check(SDL_Rect&, std::vector<int>);
	int arbitraryPoll(int, int);
	void execute(action *, std::vector<int>&, int&, int&, int&);
};

class redSuper : virtual public special{
public:
	redSuper();
	~redSuper();
	redSuper(const char* n) { build(n); }
	int arbitraryPoll(int, int);
};
#endif
