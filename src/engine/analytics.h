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
	script() {}
	script(char*);
	void init(int);
	void init(std::vector<int>);
	std::vector<std::vector<frame>> command;
	std::vector<int> selection;
	bool test();
	bool genEvent(int, int, frame&);
	virtual void load(char*);
	virtual void write(char*);
	virtual void write();
	virtual ~script();
};
#endif
