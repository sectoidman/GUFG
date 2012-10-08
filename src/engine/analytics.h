#include "player.h"
class frame {
public:
	frame();
	frame(bool[], int[], bool[]);
	bool axis[4];
	bool neg[5];
	int pos[5];
	frame * next;
	virtual void cull();
	virtual void append(frame*);
	virtual ~frame() {}
};

class replay {
public:
	replay();
	replay(int, int);
	int selection[2];
	frame * start[2];
	frame * current[2];
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
