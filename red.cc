#include "interface.h"
red::red()
{
	name = "Red";
	delete head;
	delete airHead;
	delete neutral;
//	delete crouch;
	delete standBlock;
	delete crouchBlock;
	delete airBlock;
	delete reel;
	delete fall;
//	delete crouchReel;
	
	airHead = new moveTrie;
	head = new moveTrie;
	build(name);
	
	cMove = neutral;
}

void red::touch(void * target)
{
	((interface*)target)->timer+=290;
}

void red::tick(){
	if(cancelMeter < 528) cancelMeter++;
}

void red::drawMeters(SDL_Surface *& screen, int ID)
{
	character::drawMeters(screen, ID);
	SDL_Rect c1, c2;
	if(cancelMeter >= 0){
		c1.w = cancelMeter/2; 
	} else c1.w = 1; 
	if(cancelMeter > 264){
		c1.w = 132; c2.w = (cancelMeter - 264)/2;
	} else c2.w = 0;
	if(ID == 0){
		c1.x = 134; 
		c2.x = 134;
	}
	else { 
		c1.x = 534 + (132 - c1.w);
		c2.x = 534 + (132 - c2.w);
	}
	c1.h = 5; c2.h = 5;
	c1.y = 587; c2.y = 587;
	
	SDL_FillRect(screen, &c1, SDL_MapRGB(screen->format, 0, 0, 255));
	SDL_FillRect(screen, &c2, SDL_MapRGB(screen->format, 255, 0, 255)); 
}

void red::init()
{
	character::init();
	cancelMeter = 528;
}
