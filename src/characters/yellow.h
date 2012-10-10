#ifndef YELLOW_INCLUDED
#define YELLOW_INCLUDED
#include "red.h"
class yellow : public character {
public:
	yellow();
	void resetAirOptions();
	action * createMove(char*);
	int takeHit(action *&, hStat&, int, int&, int&, int&, int&, bool&);
	void drawMeters(int, float, int);
	void tick();
	void init(action *&);
	void step(action *&, int&, int&);
};

class flashStep : public airMove {
public:
	flashStep();
	flashStep(const char*);
	bool check(SDL_Rect&, int[]);
	void execute(action *, int *&, int&, int&, int&);
	bool setParameter(char*);
	virtual void zero() { flashMeterCost = 0; airMove::zero(); }
private:
	int flashMeterCost;
};

class flashSummon : public special {
public:
	flashSummon();
	flashSummon(const char*);
	bool check(SDL_Rect&, int[]);
	void execute(action *, int *&, int&, int&, int&);
	bool setParameter(char*);
	void step(int *&, int&);
	virtual void zero() { flashMeterGain = 0; special::zero(); }
private:
	bool uFlag;
	int flashMeterGain;
};
#endif
