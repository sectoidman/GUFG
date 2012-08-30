/*Header file for "action" class in project Ground Up Fighting Game
 *
 *Written by Alex Kelly in 2012.
 *Licensed under MIT OSI, see "COPYING" for details
 */

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include "auxil.h"
#include "masks.h"
#ifndef ACTION
#define ACTION
class avatar;
class instance;

struct hStat{
	hStat() : damage(0), stun(0), push(0), lift(0), untech(0), blowback(0), hover(0), launch(0), ghostHit(0), wallBounce(0), floorBounce(0), slide(0), stick(0), hitsProjectile() {}
	int damage;         //How much damage the action does
	int stun;           //How much stun the action does
	int push;           //How much pushback the action does
	int lift;           //How much the action lifts an aerial opponent.
	int untech;
	int blowback;
	int hover;
	bool launch:1;
	bool ghostHit:1;
	bool wallBounce:1;
	bool floorBounce:1;
	bool slide:1;
	bool stick:1;
	bool hitsProjectile:1;
	blockField blockMask;
	cancelField hitState;
};

class action{
public:
	action();
	action(const char*);
	virtual ~action();
	virtual void build(const char *);

	//Okay so, hopefully the idea here is that we can init()
	//the action we're cancelling out of in the usual case, and, well
	//Do other stuff sometimes.
	virtual void execute(action *, int *&);
	virtual bool check(bool[], bool[], int, int, int[], SDL_Rect&); //Check to see if the action is possible right now.
	virtual void generate(const char*, const char*) {}
	virtual bool check(SDL_Rect&); //Check to see if the action is possible right now.
	virtual action * blockSuccess();
	virtual int arbitraryPoll(int q, int f) {return 0;}

	//Return the relevant information needed for interface::resolve(), then step to the next frame.
	void pollRects(SDL_Rect&, SDL_Rect*&, int&, SDL_Rect*&, int&, int, int);
	virtual void pollStats(hStat&, int, bool);
	virtual bool cancel(action*, int&, int&); //Cancel allowed check. Essentially: is action Lvalue allowed given the current state of action Rvalue?
	virtual void step(int *&, int&);
	virtual action * land(int &f, int &h, int &c) { return this; }
	virtual action * connect(int *&, int&, int);
	virtual instance * spawn() { return NULL; }
	virtual int takeHit(hStat&, int, int&, int&, int&); 
	bool spriteCheck(int);

	virtual void feed(action *, int, int);
	virtual char* request(int, int);

	bool CHState(int);
	virtual void draw(int, int, int, int, float);
	virtual void drawBoxen(int, int, int);

	hStat *stats, *CHStats;
	int stop;
	int throwinvuln;
	bool crouch:1;
	int armorStart; int armorLength;
	int armorHits;
	int guardStart; int guardLength;

	//Properties of a hit. These will only exist for actions that hit.
	
	//Low, High, Air Block. Basically a 3-digit binary number expressed as an integer from 0-7.
	//Digits from low to high: Blockable low, Blockable high, Blockable in the air, use a bitmask

	blockField blockState;

	//Cancel states, as defined in masks.h. Eventually this will probably be an array.

	cancelField *state, allowed;

	//Hooks for touching other things in more upper layers, like interface or player

	virtual void touch(void*) {}

	int frames;	//Number of frames.
	int hits;
	int * totalStartup;
	int * active;
	bool fch:1;
	bool dies:1;

	//SDL_Surface *sprite, *hit, *hitreg, *collision;
	int button[5];
	char * name;
	int cost;
	int * gain;

	//Tolerance refers to the individual size of the input buffer allowed for this action.
	//Default is 30 (The entire input buffer)
	int tolerance;

	//Activation refers to the most recent accepted last command. So an activation of 0
	//Implies that the most recent input has to have been *this frame* for the action to be
	//Accepted. Default is 30 (the entire input buffer)
	int activation;

	action * next;
	action ** onConnect;
	action * attempt;
	action * riposte;
	int attemptStart;
	int attemptEnd;
	bool window(int);
	int calcCurrentHit(int);

	char * tempNext;
	char ** tempOnConnect;
	char * tempAttempt;
	char * tempRiposte;

	SDL_Rect * collision;   //This will be an array of rects that are the collision boxes for the action per frame
	SDL_Rect ** hitbox;     //Same but for hitboxes
	SDL_Rect ** hitreg;     //Same but for hitreg boxes
	SDL_Rect ** delta;       //Same but for position on the screen.
	SDL_Rect *spritebox, fSpritebox;
	int * hitComplexity;
	int * regComplexity;
	int * deltaComplexity;

	int *width, *height;
	GLuint *sprite;

	bool isProjectile:1;
	virtual bool setParameter(char*);
	virtual void parseProperties(char*);
	virtual void zero();
};

class hitstun : virtual public action {
public:
	hitstun() {}
	void init(int);
	int counter;
	virtual void step(int *&, int&);
	virtual action * blockSuccess(int);
	virtual int takeHit(hStat&, int, int&, int&, int&); 
	virtual int arbitraryPoll(int, int);
	hitstun(char *, int);
	hitstun(const char *);
};

class special : virtual public action {
public:
	special() {} 
	special(const char*);
	virtual bool check(bool[], bool[], int, int, int[], SDL_Rect&); //Check to see if the action is possible right now.
	int chip;
};

class negNormal : virtual public action {
public:
	negNormal() {}
	negNormal(const char *);
	virtual bool check(bool[], bool[], int, int, int[], SDL_Rect&); //Check to see if the action is possible right now.
};

class utility : virtual public action {
public:
	utility() {}
	utility(const char *);
	virtual bool check(bool[], bool[], int, int, int[], SDL_Rect&); //Check to see if the action is possible right now.
};

class looping : virtual public utility {
public:
	looping() {}
	looping(const char*);
	virtual void step(int *&, int&);
};

class airMove : virtual public action {
public:
	airMove() {}
	airMove(const char*);
	virtual void build (const char *);
	virtual action * land(int&, int&, int&);
	char * tempLanding;
	virtual bool setParameter(char*);
	virtual void feed(action *, int, int);
	virtual char* request(int, int);
	action * landing;
	virtual void zero() { tempLanding = NULL; landing = NULL; action::zero(); }
};

class untechState : public airMove, public hitstun {
public:
	untechState() {}
	untechState(const char*);
};


class airSpecial : public airMove, public special {
public:
	airSpecial() {}
	airSpecial(const char* n) {build(n); }
};

class airNegNormal : public airMove, public negNormal {
public:
	airNegNormal() {}
	airNegNormal(const char* n) {build(n); }
};

class airUtility : public airMove, public utility {
public:
	airUtility() {}
	airUtility(const char*);
	virtual bool check(bool[], bool[], int, int, int[], SDL_Rect&); //Check to see if the action is possible right now.
	virtual void execute(action *, int *&);	
};

class airLooping : public airMove, public looping {
public:
	airLooping() {}
	airLooping(const char*);
};

class super : public special {
public:
	super() {}
	super(const char*);
	virtual int arbitraryPoll(int, int);
	virtual bool setParameter(char*);
	int freezeFrame;
	int freezeLength;
};

class airSuper : public airMove, public super {
public:
	airSuper() {}
	airSuper(const char* n) {build(n); }
	virtual bool setParameter(char*);
};

class mash : virtual public action {
public:
	mash() {}
	mash(const char* n) {build(n); }
	virtual bool check(bool[], bool[], int, int, int[], SDL_Rect&); //Check to see if the action is possible right now.
};

class werf : virtual public action {
public:
	werf() {}
	werf(const char* n) {build(n); }
	virtual bool setParameter(char *n);
	virtual bool check(SDL_Rect&); //Check to see if the action is possible right now.
	virtual int arbitraryPoll(int, int);
	int startPosX;
	int startPosY;
	int xRequisite;
	int yRequisite;
};

class luftigeWerf : public airMove, public werf {
public:
	luftigeWerf() {}
	luftigeWerf(const char* n) {build(n); }
	virtual bool setParameter(char *n);
	void build(const char *n) {werf::build(n);}
	virtual bool check(SDL_Rect&); //Check to see if the action is possible right now.
};

class summon : virtual public action {
public:
	summon() {}
	summon(const char*);
	virtual int arbitraryPoll(int, int);
	virtual bool setParameter(char*);
	virtual void generate(const char*, const char*);
	virtual char* request(int, int);
	instance * spawn();
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
