#include "player.h"
#ifndef ANALYTICS
#define ANALYTICS
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

struct frame {
	std::vector<bool> axis;
	std::vector<bool> neg;
	std::vector<int> pos;
};

class script {
public:
	const char *name;
	script(int players, int buttons);
	script(std::vector<int> s, int buttons);
	script(const char*);
	void init(int players, int buttons);
	std::vector<std::vector<frame>> command;
	std::vector<int> selection;
	virtual void load(const char*);
	virtual void write();
};
#endif
