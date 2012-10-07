#include "player.h"
class frame {
public:
	frame();
	int dir:4;
	bool neg[5];
	int pos[5];
	frame * next;
	virtual void cull();
};

class sequence {
public:
	sequence();
	frame * start;
	frame * curr;
	virtual void append(frame*);
	~sequence();
};

class replay {
public:
	replay()
	int selection[2];
	sequence * start[2];
	virtual void append(frame*, frame*);
	virtual void load(const char*);
	virtual void write();
	~replay();
}

class  : public sequence {
public:
	combo();
	combo(action*);
	int posX, posY;
	int proxX, proxY;
	int damage;
	int cost;
	char * starter;
	virtual void write();
	~combo();
};
