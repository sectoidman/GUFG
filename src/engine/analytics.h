#include "player.h"
struct frame {
	int dir;
	int pos[5];
	bool neg[5];
	frame * next;
};

class replay {
public:
	int selection[2];
	frame * head[2];
};
