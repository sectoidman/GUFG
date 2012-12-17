#ifndef YELLOW_INCLUDED
#define YELLOW_INCLUDED
#include "red.h"
class yellow : public character {
public:
	yellow();
	void resetAirOptions(int*&);
	action * createMove(char*);
	int takeHit(action *&, hStat&, int, int&, int&, int&, int&, bool&, int*&);
	void drawMeters(int, float, int, int*);
	void tick(int *&);
	void init(int *&);
	int * generateMeter();
	void step(action *&, int&, int&, int*&);
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
