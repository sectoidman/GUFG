#ifndef YELLOW_INCLUDED
#define YELLOW_INCLUDED
#include "red.h"
class yellow : public character {
public:
	yellow();
	void resetAirOptions();
	action * createMove(char*);
	int takeHit(hStat&, int);
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
	virtual void zero() { flashMeterCost = 0; airMove::zero(); }
private:
	int flashMeterCost;
};

class flashSummon : public special {
public:
	flashSummon();
	flashSummon(const char*);
	bool check(bool[], bool[], int, int, int*, SDL_Rect&);
	void execute(action *, int *&);
	bool setParameter(char*);
	void step(int *&);
	virtual void zero() { flashMeterGain = 0; special::zero(); }
private:
	bool uFlag;
	int flashMeterGain;
};
#endif
