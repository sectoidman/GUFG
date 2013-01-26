#include "player.h"
class chart{
public:
	chart(int);
	std::vector<std::vector<int>> wins;
	void recordWin(int, int);
	float matchup(int, int);
	void write();
	void init();
	~chart();
};

class frame {
public:
	frame(std::vector<bool>, std::vector<int>, std::vector<bool>);
	std::vector<bool> axis;
	std::vector<bool> neg;
	std::vector<int> pos;
};

class replay {
public:
	replay(const char*);
	int selection[2];
	std::vector<frame*> script[2];
	virtual void load(const char*);
	virtual void write();
};
