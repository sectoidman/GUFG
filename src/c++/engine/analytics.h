#include "player.h"
class chart{
public:
	chart(int);
	std::vector<std::vector<int>> wins;
	void recordWin(int, int);
	float matchup(int, int);
	void write();
	void init();
	~chart();
};

class frame {
public:
	frame();
	frame(std::vector<bool>, std::vector<int>, std::vector<bool>);
	std::vector<bool> axis;
	std::vector<bool> neg;
	std::vector<int> pos;
	frame * next;
	virtual void cull();
	virtual void append(frame*);
	virtual ~frame() {}
};

class replay {
public:
	replay();
	replay(int, int, int);
	replay(const char*);
	int selection[2];
	frame * start[2];
	frame * current[2];
	int fcounter;
	int rcount;
	virtual void append(frame*, frame*);
	virtual void load(const char*);
	virtual void write();
	virtual ~replay();
};

class combo {
public:
	combo();
	combo(action*);
	frame * start;
	frame * current;
	int posX, posY;
	int proxX, proxY;
	int damage;
	int cost;
	char * starter;
	virtual void write();
	virtual ~combo();
};
