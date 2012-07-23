#include "red.h"
class yellow : public character {
public:
	yellow();
	void resetAirOptions();
	void tick();
	void init();
};

class flashStep : public special {
public:
	flashStep();
	flashStep(char*);
	bool setParameter(char*);
	bool check(bool[], bool[], int, int, int*, SDL_Rect&);
	void execute(action *, int *&);
	int flashMeterCost;
};
