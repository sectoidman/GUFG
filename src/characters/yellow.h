#ifndef YELLOW_INCLUDED
#define YELLOW_INCLUDED
#include "red.h"
class yellow : public character {
public:
	yellow();
	void resetAirOptions(std::vector<int>&);
	action * createMove(char*);
	int takeHit(status&, hStat&, int, int&, std::vector<int>&);
	void drawMeters(int, int, std::vector<int>);
	void tick(std::vector<int>&);
	void init(std::vector<int>&);
	std::vector<int> generateMeter();
	void step(status&, std::vector<int>&);
};

class flashStep : public airMove {
public:
	flashStep();
	flashStep(const char*);
	bool check(SDL_Rect&, std::vector<int>);
	void execute(action *, std::vector<int>&, int&, int&, int&);
private:
	int flashMeterCost;
};

class flashSummon : public special {
public:
	flashSummon();
	flashSummon(const char*);
	bool check(SDL_Rect&, std::vector<int>);
	void execute(action *, std::vector<int>&, int&, int&, int&);
	bool setParameter(char*);
	virtual void zero();
	void step(std::vector<int>&, int&, int&, int&);
private:
	bool uFlag;
	int flashMeterGain;
};
#endif
