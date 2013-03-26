/*Copyright Somnambulent Studios 2012-2013*/
#include "trie.h"
#include <SDL/SDL.h>

#ifndef ___character
#define ___character

using std::string;
using std::vector;

class instance;
class avatar {
public:
	avatar() {}
	virtual void setParam(int, int) {}
	virtual vector<int> generateMeter();
	virtual void init(vector<int>&) = 0;
	virtual action * moveSignal(int);
	bool spriteCheck(action *&, int);
	virtual void draw(action *&, int);
	virtual action * createMove(string);
	virtual action * dealWithMove(string);
	virtual action * mandateMove(string);
	virtual void build(const char*, const char*);
	virtual void processMove(action * m);
	virtual void sortMove(action *, string);
	virtual void prepHooks(status&, action *&, int[], vector<int>, SDL_Rect &, bool, vector<int>&);
	virtual action * hook(int[], int, int, vector<int>, vector<int>, action *, SDL_Rect&, int&, int&, bool);
	virtual bool death(action *&, int, int) { return 0; }
	virtual int takeHit(status&, hStat&, int, int&, vector<int>&) { return 0; }
	virtual void getName(string, string);
	virtual void loadAssets();
	virtual void connect(status&, vector<int>&);
	virtual void pollStats(hStat&, status&);
	virtual void step(status&, vector<int>&);
	virtual int acceptTarget(action*, int);
	virtual instance * spawn(action*);
	virtual void tick(vector<int>&) {}
	virtual void neutralize(status&, action*&, vector<int>&);
	virtual bool turn(int&) { return 0; }
	string name;
	actionTrie * head;
	int lifespan;

	action * die;
	action * neutral;
	bool headless;
	vector<action*> moveList;
	vector<string> moveNames;
};

class character : virtual public avatar{
public:
	bool dFlag:1;
	character(const char*);
	actionTrie * airHead;
	character();
	virtual ~character();

	virtual void build(const char*, const char*);
	virtual int comboState(action *);
	virtual void neutralize(status&, action*&, vector<int>&);
	virtual void drawMeters(int, int, vector<int>);
	virtual void init(vector<int> &);
	virtual int checkBlocking(status&, int[]);
	virtual void block(status&, int, bool);
	virtual void resetAirOptions(vector<int>&);
	virtual void land(action *&, int &, int &, int &, vector<int>&);
	virtual void sortMove(action *, string);
	virtual int takeHit(status&, hStat&, int, int&, vector<int>&);
	virtual action * hook(int[], int, int, vector<int>, vector<int>, action *, SDL_Rect&, int&, int&, bool);

	action * dead;
	action * airNeutral;
	action * crouch;
	action * reel;
	action * untech;
	action * fall;
	action * down;
	action * crouchReel;
	action * crouchBlock;
	action * standBlock;
	action * airBlock;
	action * throwBreak;
};

class projectile : virtual public avatar {
public:
	projectile(string, string);
	projectile() {}
	virtual void build(string, string);
	virtual int acceptTarget(action*, int);
	virtual void pollStats(hStat&, status&);
	virtual int takeHit(status&, hStat&, int, int&, vector<int>&);
	virtual bool turn(int&);

	virtual void processMove(action * m);
	virtual bool death(action *&, int, int);
	virtual void init(vector<int>&);
};
#endif
