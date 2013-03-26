/*Copyright Somnambulent Studios 2012-2013*/
#include "analytics.h"
#include "harness.h"
#include "sdl-compat.h"
#include "window.h"
#include <SDL/SDL.h>
#include <string>
#include <vector>

#ifndef ___interface
#define ___interface

using std::string;
using std::vector;

class gameInstance : public window, public arcadeHarness{
public:
	virtual void resolve() = 0;
	int drawGlyph(string, int, int, int, int, int);
	virtual void loadMisc();
	virtual bool screenInit();
	virtual bool screenInit(int, int);
	virtual void processInput(SDL_Event&);
	virtual void initialConfig(int);
	virtual void unitCollision(instance*, instance*);
	virtual void genInput();
	script *oldReplay;
	unsigned int replayIterator;
	virtual void print();

	vector<instance*> things;
	vector<player*> P;

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
	virtual void loadAssets();
	void processInput(SDL_Event&);
	void createPlayers();
	void createPlayers(string);
	void startGame();
	void loadMatchBackground();

	void resolve();
	void resolveInputs();
	void resolvePhysics();
	void resolveCollision();
	void resolveCamera();
	void resolveCombos();
	void resolveHits();
	void resolveThrows();
	void resolveSummons();
	void summonAttractors();
	void draw();
	void drawHUD();
	void drawGame();
	void drawCSelect();
	void drawMainMenu(int);
	void drawConfigMenu(int);
	void drawRematchMenu();
	void drawPauseMenu();
	void cleanup();
	void runTimer();
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
	void writeImage(string, int, action*);

	bool select[2];
	vector<int> selection;
	int menu[2];
	int configMenu[2];
	int counterHit[2];
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
	int grav;

	GLuint background;
	Mix_Music *matchMusic;
	Mix_Music *menuMusic;
	Mix_Chunk **announceWinner;
	Mix_Chunk *announceRound[3];
	Mix_Chunk *announceDraw[2];
	Mix_Chunk *announceFight;
	Mix_Chunk *announceEnd[2];
	Mix_Chunk *announceSelect;

	vector<attractor *> globals;

	script *replay;

	int numChars;
	GLuint selectScreen; 
	SDL_Rect wheel;

	vector<vector<int> > buttons;
	int timer;
	bool roundEnd:1;
	bool killTimer:1;
	int endTimer;
};
#endif
