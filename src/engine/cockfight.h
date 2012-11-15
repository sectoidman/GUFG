#include "interface.h"
#include "trainer.h"
#ifndef COCKFIGHT
#define COCKFIGHT
class cockfight : public gameInstance {
public:
	cockfight();
	void matchInit();
	void resolve();
	void checkWin();
	void draw();
	//void cleanup();

	trainer * p[2];
	int winner;
	bool gameover;
};
#endif
