#include "charlist.h"
class player{
public:
	player();

	SDL_Event input[10];	//Inputs. These are the SDL_Events tied to the 10 buttons in the actual game
	frame * current;
	SDL_Surface * sprite;
	move * cMove;
	character * pick;
	int deltaX, deltaY;
	char * inputName[10];	//Input names. This is really just for housekeeping.
	bool aerial;
	bool sFlag;
	int gameover;
	int facing;
	void init();
	void keyConfig(int);
	void characterSelect(int);
	void pushInput(bool*, bool*, bool*);
	int colorkey;
	void draw();
	void spriteInit();
	SDL_Rect pos;
private:
	int inputBuffer[29];
};
