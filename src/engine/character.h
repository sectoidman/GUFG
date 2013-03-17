/*Header file for character class in project Ground Up Fighting Game
 *
 *Written by Alex Kelly in 2012
 *Under MIT OSI license, see "COPYING" for details
 */
#include <SDL/SDL.h>
#include "trie.h"
#ifndef CHARACTER
#define CHARACTER
class instance;
class avatar {
public:
	avatar() {}
	virtual void setParam(int, int) {}
	virtual std::vector<int> generateMeter();
	virtual void init(std::vector<int>&) = 0;
	virtual action * moveSignal(int);
	bool spriteCheck(action *&, int);
	virtual void draw(action *&, int);//Pass sprite information up.
	virtual action * createMove(char*);
	virtual void build(const char*, const char*);
	virtual void processMove(action * m);
	virtual void sortMove(action *, char*);
	virtual void prepHooks(status&, action *&, int[], std::vector<int>, SDL_Rect &, bool, std::vector<int>&);	//Take input from the game and propagate it to the appropriate actionTrie.
	//BRB prepping my hooks
	virtual action * hook(int[], int, int, std::vector<int>, std::vector<int>, action *, SDL_Rect&, int&, int&, bool);
	virtual bool death(action *&, int, int) { return 0; }
	virtual int takeHit(status&, hStat&, int, int&, std::vector<int>&) { return 0; }
	virtual void getName(const char*, const char*);
	virtual void connect(status&, std::vector<int>&);
	virtual void pollStats(hStat&, status&, bool);
	virtual void step(status&, std::vector<int>&);
	virtual int acceptTarget(action*, int);
	virtual instance * spawn(action*);
	virtual void tick(std::vector<int>&) {}
	virtual void neutralize(status&, action*&, std::vector<int>&);
	virtual bool turn(int&) { return 0; }
	char * name; //The name of the directory from which the character spawns. This is important for loading into memory
	actionTrie * head;	//Trie for ground actions
	int lifespan;

	action * die;
	action * neutral;
	bool headless;
	std::vector<action*> movesByName;
	std::vector<char*> moveNames;
};

class character : virtual public avatar{
public:
	bool dFlag:1;
	character(const char*);
	actionTrie * airHead;	//Trie for air actions
	character();		//Load the entire character into memory. This should happen once per player per match.
	virtual ~character();	//Free stuff

	virtual void build(const char*, const char*);//This will *eventually* be the function that parses the character constructor file.

	virtual int comboState(action *);
	virtual void neutralize(status&, action*&, std::vector<int>&);
	virtual void drawMeters(int, int, std::vector<int>);
	virtual void init(std::vector<int> &);
	virtual int checkBlocking(status&, int[]);
	virtual void block(status&, int, bool);
	virtual void resetAirOptions(std::vector<int>&);
	virtual void land(action *&, int &, int &, int &, std::vector<int>&);
	virtual void sortMove(action *, char*);
	virtual int takeHit(status&, hStat&, int, int&, std::vector<int>&);
	virtual action * hook(int[], int, int, std::vector<int>, std::vector<int>, action *, SDL_Rect&, int&, int&, bool);

	looping * dead;
	action * airNeutral;
	action * crouch;
	hitstun * reel;
	untechState * untech;
	airLooping * fall;
	utility * down;
	hitstun * crouchReel;
	hitstun * crouchBlock;
	hitstun * standBlock;
	hitstun * airBlock;
	utility * throwBreak;
};

class projectile : virtual public avatar {
public:
	projectile(const char* directory, const char* file);
	projectile() {}
	virtual void build(const char*, const char*);
	virtual int acceptTarget(action*, int);
	virtual void pollStats(hStat&, status&, bool);
	virtual int takeHit(status&, hStat&, int, int&, std::vector<int>&);
	virtual bool turn(int&);

	virtual void processMove(action * m);
	virtual bool death(action *&, int, int);
	virtual void init(std::vector<int>&);
};
#endif
