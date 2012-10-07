#include "player.h"
struct frame {
	int dir:4;
	bool neg[5];
	int pos[5];
	frame * next;
};

class replay {
public:
	replay();
	int selection[2];
	frame * head[2];
	frame * curr[2];
	virtual void append(frame*);
	virtual void load(const char*);
	virtual void write();
	~replay();
};

class combo : public replay {
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
