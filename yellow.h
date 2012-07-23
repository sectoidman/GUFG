#include "red.h"
class yellow : public character {
public:
	yellow();
	void resetAirOptions();
	action * createMove(char*);
	void drawMeters(int);
	void tick();
	void init();
	void step();
};

class flashStep : public airSpecial {
public:
	flashStep();
	flashStep(char*);
	bool setParameter(char*);
	bool check(bool[], bool[], int, int, int*, SDL_Rect&);
	void execute(action *, int *&);
	int flashMeterCost;
	int flashMeterGain;
	void step(int *&);
};
