/*Header file for character class in project Ground Up Fighting Game
 *
 *Written by Alex Kelly in 2012
 *Under MIT OSI license, see "COPYING" for details
 */
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include "trie.h"
#ifndef CHARACTER
#define CHARACTER
class avatar {
public:
	avatar() : cMove(NULL), bMove(NULL) {}
	virtual void init() = 0;
	bool spriteCheck(int);
	virtual void draw(int, int, int, int);//Pass sprite information up.
	virtual action * createMove(char*);
	virtual void processMove(action * m);
	virtual void build(const char*, const char*);
	virtual void prepHooks(int[], bool[], bool[], SDL_Rect &, int&, bool);	//Take input from the game and propagate it to the appropriate actionTrie.

	virtual void connect(hStat&);
	virtual bool step(int&);
	virtual avatar * spawn(action*);
	virtual void tick() {}
	virtual void neutralize();
	action * cMove;
	action * bMove;
	int freeze;
	char * name; //The name of the directory from which the character spawns. This is important for loading into memory
	bool aerial; //Flags whether the character is considered "in the air"
	actionTrie * head;	//Trie for ground actions
	actionTrie * airHead;	//Trie for air actions
	int * meter;
	bool dead:1;

	action * neutral;
};
class character : virtual public avatar{
public:
	character(const char*);
	character();		//Load the entire character into memory. This should happen once per player per match.
	virtual ~character();	//Free stuff

	virtual void build(const char*, const char*);//This will *eventually* be the function that parses the character constructor file.

	virtual void neutralize();
	virtual void drawMeters(int);
	virtual void init();
	virtual void resetAirOptions();
	virtual void land(int &);
	virtual int takeHit(hStat&, int, int&);
	virtual void prepHooks(int[], bool[], bool[], SDL_Rect &, int&, bool);	//Take input from the game and propagate it to the appropriate actionTrie.
	//BRB prepping my hooks

	action * airNeutral;
	action * crouch;
	hitstun * reel;
	untechState * untech;
	airLooping * fall;
	utility * down;
	mash * tech;
	hitstun * crouchReel;
	hitstun * crouchBlock;
	hitstun * standBlock;
	hitstun * airBlock;
	utility * throwBreak;

	int health;
};

class projectile : virtual public avatar {
public:
	projectile(const char* directory, const char* file);
	projectile() {}
	virtual void build(const char*, const char*);

	virtual void tick();
	virtual bool step(int&);
	virtual void init();
	action * die;
};
#endif
