/*Interface header for GUFG
 *The interface class/struct is intended as an abstraction between the player and the engine
 *This is where things pertaining to inputs should go, as well as sprsibly information like the
 *round timer.
 *
 *Written by Alex Kelly in 2012
 *Licensed under MIT OSI: Additional information in the file "COPYING"
 */
#include <SDL/SDL.h>
#include "sdl-compat.h"
#include <stdio.h>
#include <vector>
#include "analytics.h"
#include "window.h"
#include "harness.h"
#ifndef INTERFACE
#define INTERFACE
class gameInstance : public window, public arcadeHarness{
public:
	virtual void resolve() = 0;	/*Every game type requires a resolve loop.*/
	int drawGlyph(const char *, int, int, int, int, int);
	virtual void loadMisc();
	virtual bool screenInit();
	virtual bool screenInit(int, int);
	virtual void processInput(SDL_Event&);	/*Accepts input into input containers, for use by anything that wants it*/
	virtual void initialConfig(int);
	virtual void unitCollision(instance*, instance*);
	virtual void genInput();
	script *oldReplay;
	unsigned int replayIterator;
	virtual void print();

//Input layer stuff. Players, inputs, etc.
	std::vector<instance*> things;
	std::vector<player*> P;

//Meta-interface stuff. Gameover state, screen size, etc.
	int screenHeight, screenWidth, floor, wall;
	int freeze;
	bool pauseEnabled:1;
	bool scripting:1;
	float scalingFactor, sf;
	bool initd:1;
	GLuint glyph[91];
};

class interface : public gameInstance{
public:
	interface();
	~interface();
	void readInput();
	void loadMisc();
	void processInput(SDL_Event&);	/*Accepts input into input containers, for use by anything that wants it*/
	void createPlayers();
	void createPlayers(char*);
	void startGame();
	void loadMatchBackground();
/*Important interface functions that will remain interface functions*/

/*The main game loop runs readInput() and resolve() each exactly once per frame loop.*/
	void resolve();		/*The main loop of the game*/
	void resolveInputs();
	void resolvePhysics();
	void resolveCollision();
	void resolveCamera();
	void resolveCombos();
	void resolveHits();
	void resolveThrows();
	void resolveSummons();
	void summonAttractors();
	void draw();		/*The primary function in charge of drawing things on the screen.*/
	void drawHUD();
	void drawGame();
	void drawCSelect();
	void drawMainMenu(int);
	void drawConfigMenu(int);
	void drawRematchMenu();
	void drawPauseMenu();
	void cleanup();
	void runTimer();	/*Currently just a decrementer. May always just be a decrementer.*/
	void roundInit();
	void matchInit();
	void cSelectMenu();
	void mainMenu(int);
	void keyConfig(int);
	void rematchMenu();
	void pauseMenu();
	void checkWin();
	void dragBG(int);
	void doSuperFreeze();
	void readMatchupChart();
	void writeMatchupChart();
	void writeImage(const char*, int, action*);

	bool select[2];
	std::vector<int> selection;
	int menu[2];
	int configMenu[2];
	int rMenu;
	int pMenu;
	SDL_Rect bg;
	SDL_Rect prox;
	bool firstFrame:1;
	bool shortcut:1;
	bool continuous:1;
	bool single:1;
	bool analytics:1;
	int combo[2];
	int damage[2];
	bool illegit[2];
	float prorate[2];
	chart *stats;
	int numRounds;
	int grav;	//Gravitational constant. 

	GLuint background;
	Mix_Music *matchMusic;
	Mix_Music *menuMusic;
	Mix_Chunk **announceWinner;
	Mix_Chunk *announceRound[3];
	Mix_Chunk *announceDraw[2];
	Mix_Chunk *announceFight;
	Mix_Chunk *announceEnd[2];
	Mix_Chunk *announceSelect;

	std::vector<attractor *> globals;

	script *replay;

//Variables for cSelectMenu 
	int numChars;
	GLuint selectScreen; 
	SDL_Rect wheel;

	std::vector<std::vector<int> > buttons;
	int timer;
	bool roundEnd:1;
	bool killTimer:1;
	int endTimer;
};
#endif
