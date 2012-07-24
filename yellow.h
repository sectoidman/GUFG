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

class flashStep : public airMove {
public:
	flashStep();
	flashStep(const char*);
	bool check(bool[], bool[], int, int, int*, SDL_Rect&);
	void execute(action *, int *&);
	bool setParameter(char*);
private:
	int flashMeterCost;
};

class flashSummon : public special {
public:
	flashSummon();
	flashSummon(const char*);
	bool check(bool[], bool[], int, int, int*, SDL_Rect&);
	bool setParameter(char*);
	void step(int *&);
private:
	int flashMeterGain;
};
