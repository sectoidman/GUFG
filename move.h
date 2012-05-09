/*Header file for "move" class in project Ground Up Fighting Game
 *
 *Written by Alex Kelly in 2012. 
 *Licensed under MIT OSI, see "COPYING" for details
 */

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include "auxil.h"
#include "masks.h"

class move{
public:
	move();
	move(char*);
	virtual ~move();
	virtual void build(char *);

	//Okay so, hopefully the idea here is that we can init()
	//the move we're cancelling out of in the usual case, and, well
	//Do other stuff sometimes.
	void execute(move *);
	virtual bool check(bool[], bool[], int, int); //Check to see if the move is possible right now.
	virtual void blockSuccess(int);

	//Return the relevant information needed for interface::resolve(), then step to the next frame.
	void pollRects(SDL_Rect&, SDL_Rect*&, int&, SDL_Rect*&, int&);
	bool operator>(move*); //Cancel allowed check. Essentially: is move Lvalue allowed given the current state of move Rvalue?
	void init();           //Really just sets current frame to 0. I wanted current frame to be private for now, so I don't break anything.
	virtual void step();
	virtual void land(move *&) {}
	void connect();
	SDL_Surface * draw(int, bool);

	bool launch;         //Whether or not the move launches on hit

	//Properties of a hit. These will only exist for moves that hit.
	int *damage;          //How much damage the move does
	int *stun;            //How much stun the move does
	int *push;            //How much pushback the move does
	int *lift;            //How much the move lifts an aerial opponent.

	//Low, High, Air Block. Basically a 3-digit binary number expressed as an integer from 0-7.
	//Digits from low to high: Blockable low, Blockable high, Blockable in the air, use a bitmask

	blockField *blockMask, blockState;

	//Cancel states, as defined in masks.h. Eventually this will probably be an array.

	cancelField *state, allowed;

	//Hooks for touching other things in more upper layers, like interface or player

	virtual void touch(void*) {}
		
	bool xLock, yLock;
	int frames;	 //Number of frames.
	int hits;
	int currentFrame;//The frame that is currently running.
	int currentHit;
	int cFlag;
	int * totalStartup;
	
	//SDL_Surface *sprite, *hit, *hitreg, *collision;
	int button[5];
	char * name;
	int cost;

	//Tolerance refers to the individual size of the input buffer allowed for this move.
	//Default is 30 (The entire input buffer)
	int tolerance;

	//Activation refers to the most recent accepted last command. So an activation of 0
	//Implies that the most recent input has to have been *this frame* for the move to be
	//Accepted. Default is 30 (the entire input buffer)
	int activation;

	SDL_Rect * collision;   //This will be an array of rects that are the collision boxes for the move per frame
	SDL_Rect ** hitbox;     //Same but for hitboxes
	SDL_Rect ** hitreg;     //Same but for hitreg boxes
	SDL_Rect ** delta;       //Same but for position on the screen.
	int * hitComplexity;
	int * regComplexity;
	int * deltaComplexity;
	SDL_Surface **sprite, **fSprite;
};

class hitstun : public move {
public:
	hitstun() {}
	void init(int);
	int counter;
	void step();
	void blockSuccess(int);
	hitstun(char *, int);
	hitstun(char *);
};

class special : virtual public move {
public:
	special() {}
	special(char*);
	bool check(bool[], bool[], int, int);
	int chip;
};

class utility : virtual public move {
public:
	utility() {}
	utility(char *);
	bool check(bool[], bool[], int, int);
};

class looping : virtual public utility {
public:
	looping() {}
	looping(char*);
	void step();
};

class airMove : virtual public move {
public:
	airMove() {}
	airMove(char*);
	void build (char *);
	void land(move *&);
	void setLR(move *);
	move * landing;
};

class airSpecial : public airMove, public special {
public:
	airSpecial() {}
	airSpecial(char*);
};

class airUtility : public airMove, public utility {
public:
	airUtility() {}
	airUtility(char*);
};

class airLooping : public airMove, public looping {
public:
	airLooping() {}
	airLooping(char*);
};

class projectile {
public:
	projectile(char*);
	~projectile();

	int ID;
	int posX, posY;
	
	int hitComplexity, regComplexity, momentumComplexity;
	SDL_Rect *momentum, *hitbox, *hitreg, collision, spr;

	SDL_Surface *sprite;
	move *cMove;
};

class summon : virtual public special {
public:
	summon() {}
	summon(char*);
	projectile * payload;
	projectile * spawnProjectile();
	int spawnFrame;
};

class airSummon : public airMove, public summon {
public:
	airSummon() {}
	airSummon(char*);
};
