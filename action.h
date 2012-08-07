/*Header file for "action" class in project Ground Up Fighting Game
 *
 *Written by Alex Kelly in 2012.
 *Licensed under MIT OSI, see "COPYING" for details
 */

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include "auxil.h"
#include "masks.h"

struct hStat{
	hStat() : damage(0), stun(0), push(0), lift(0), untech(0), blowback(0), hover(0), launch(0), ghostHit(0), wallBounce(0), floorBounce(0), slide(0), stick(0) {}
	int damage;          //How much damage the action does
	int stun;            //How much stun the action does
	int push;            //How much pushback the action does
	int lift;            //How much the action lifts an aerial opponent.
	int untech;
	int blowback;
	int hover;
	bool launch:1;
	bool ghostHit:1;
	bool wallBounce:1;
	bool floorBounce:1;
	bool slide:1;
	bool stick:1;
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
	virtual bool check(SDL_Rect&); //Check to see if the action is possible right now.
	virtual action * blockSuccess(int);
	virtual int arbitraryPoll(int q) {return 0;}

	//Return the relevant information needed for interface::resolve(), then step to the next frame.
	void pollRects(SDL_Rect&, SDL_Rect*&, int&, SDL_Rect*&, int&);
	virtual void pollStats(hStat&);
	bool operator>(action*); //Cancel allowed check. Essentially: is action Lvalue allowed given the current state of action Rvalue?
	virtual void init();           //Really just sets current frame to 0. I wanted current frame to be private for now, so I don't break anything.
	virtual void step(int *&);
	virtual action * land() { return this; }
	virtual action * connect(int *&, action *&);
	virtual void hitConfirm(int);
	virtual int takeHit(hStat&, int); 
	bool spriteCheck();

	virtual void feed(action *, int, int);
	virtual char* request(int, int);

	bool CHState();
	virtual void draw(int, int, int);

	hStat *stats = NULL;
	int stop = 0;
	int throwinvuln = 0;
	bool crouch = 0;
	int armorStart = 0; int armorLength = 0;
	int guardStart = 0; int guardLength = 0;

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
	int currentFrame;//The frame that is currently running.
	int currentHit;
	int * totalStartup;
	int * active;
	int cFlag;
	int hFlag;
	
	//SDL_Surface *sprite, *hit, *hitreg, *collision;
	int button[5];
	char * name;
	int cost = 0;
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
	int attemptStart = 0;
	int attemptEnd = 0;
	bool window();

	char * tempNext = NULL;
	char ** tempOnConnect;
	char * tempAttempt = NULL;

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

	virtual bool setParameter(char*);
	virtual void parseProperties(char*);

};

class hitstun : virtual public action {
public:
	hitstun() {}
	void init(int);
	int counter;
	virtual void step(int *&);
	virtual action * blockSuccess(int);
	virtual int takeHit(hStat &, int);
	virtual int arbitraryPoll(int);
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
	virtual void step(int *&);
};

class airMove : virtual public action {
public:
	airMove() {}
	airMove(const char*);
	virtual void build (const char *);
	virtual action * land();
	char * tempLanding = NULL;
	virtual bool setParameter(char*);
	virtual void feed(action *, int, int);
	virtual char* request(int, int);
	action * landing = NULL;
};

class untechState : public airMove, public hitstun {
public:
	untechState() {}
	untechState(const char*);
};


class airSpecial : public airMove, public special {
public:
	airSpecial() {}
	airSpecial(const char* n) {build(n); init();}
};

class airNegNormal : public airMove, public negNormal {
public:
	airNegNormal() {}
	airNegNormal(const char* n) {build(n); init();}
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
	virtual int arbitraryPoll(int);
	virtual bool setParameter(char*);
	int freezeFrame;
	int freezeLength;
};

class airSuper : public airMove, public super {
public:
	airSuper() {}
	airSuper(const char* n) {build(n); init();}
	virtual bool setParameter(char*);
};

class mash : virtual public action {
public:
	mash() {}
	mash(const char* n) {build(n); init();}
	virtual bool check(bool[], bool[], int, int, int[], SDL_Rect&); //Check to see if the action is possible right now.
};

class werf : virtual public action {
public:
	werf() {}
	werf(const char* n) {build(n); init();}
	virtual bool setParameter(char *n);
	virtual bool check(SDL_Rect&); //Check to see if the action is possible right now.
	virtual int arbitraryPoll(int n);
	int startPosX;
	int startPosY;
	int xRequisite;
	int yRequisite;
};

class luftigeWerf : public airMove, public werf {
public:
	luftigeWerf() {}
	luftigeWerf(const char* n) {build(n); init();}
	virtual bool setParameter(char *n);
	void build(const char *n) {werf::build(n);}
	virtual bool check(SDL_Rect&); //Check to see if the action is possible right now.
};
