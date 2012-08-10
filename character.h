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
	bool spriteCheck();
	virtual void draw(int, int, int);//Pass sprite information up.
	virtual action * createMove(char*);
	virtual void processMove(action * m);
	virtual void build(const char*, const char*);
	virtual void prepHooks(int[], bool[], bool[], SDL_Rect &, bool) {}	//Take input from the game and propagate it to the appropriate actionTrie.

	virtual void connect(hStat&);
	virtual void step();
	virtual void tick() {}
	action * cMove;
	action * bMove;
	int freeze;
	char * name; //The name of the directory from which the character spawns. This is important for loading into memory
	bool aerial; 	//Flags whether the character is considered "in the air"
	actionTrie * head;	//Trie for ground actions
	actionTrie * airHead;	//Trie for air actions
	int * meter;

	action * neutral;
};
class character : public avatar{
public:
	character(const char*);
	character();		//Load the entire character into memory. This should happen once per player per match.
	virtual ~character();	//Free stuff

	virtual void build(const char*, const char*);//This will *eventually* be the function that parses the character constructor file.

	virtual void drawMeters(int);
	virtual void init();
	virtual void resetAirOptions();
	virtual void land();
	virtual int takeHit(hStat&, int);
	//BRB prepping my hooks
	virtual void prepHooks(int[], bool[], bool[], SDL_Rect &, bool);	//Take input from the game and propagate it to the appropriate actionTrie.

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
	projectile(const char* directory, const char* file) 
		{ build(directory, file); }
	projectile() {}

	void init() {}
};

class summon : virtual public action {
public:
	summon() {}
	summon(const char*);
	virtual int arbitraryPoll(int);
	virtual bool setParameter(char*);
	virtual void generate(const char*, const char*);
	virtual char* request(int, int);
	avatar * spawn();
	virtual void zero();

	avatar * payload;
	char * tempPayload;
	int spawnFrame;
	int spawnPosX;
	int spawnPosY;
	bool spawnTrackX:1;
	bool spawnTrackY:1;
};

class airSummon : virtual public airMove, virtual public summon {
public:
	airSummon() {}
	airSummon(const char*);
	virtual void zero();
	virtual bool setParameter(char*);
	virtual char* request(int, int);
};
#endif
