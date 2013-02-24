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

struct inputContainer {
	unsigned int dir:4;
	int A:2;
	int B:2;
	int C:2;
	int D:2;
	int E:2;
	bool Start:1;
};

typedef union {
	inputContainer raw;
	int i;
} inputs;

struct frame {
	inputs n;
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
	void push(int i, frame);
	void push(frame);
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
