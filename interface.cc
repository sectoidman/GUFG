/*Interface class for GUFG
 *This will run all the main game functions within GUFG
 *
 *Written by Alex Kelly in 2012
 *License to come
 */

#include "interface.h"
#include <cstring>
#include <math.h>
interface::interface()
{
	numChars = 2;
	char buffer[50];
	/*Initialize some pseudo-constants*/
	screenWidth = 800; //By screen, I mean the window the game occurs in.
	screenHeight = 600;
	bg.w = 1600;       //By background, I mean the thing the characters actually move on. Bigger than the screen.
	bg.h = 900;
	floor = bg.h - 25; //Value of the floor. This is the maximum distance downward that characters can travel.
	wall = 25;         //The size of the offset at which characters start to scroll the background, and get stuck.

	/*Initialize SDL*/
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Init(SDL_INIT_JOYSTICK);
	/*WM stuff*/
	SDL_WM_SetCaption("GUFG", "GUFG");
	screen = SDL_SetVideoMode(screenWidth, screenHeight, 0, 0);
//	SDL_ShowCursor(SDL_DISABLE);

	/*Set up input buffers and joysticks*/
	for(int i = 0; i < SDL_NumJoysticks(); i++)
		SDL_JoystickOpen(i);
	/*Initialize players.*/
	for(int i = 0; i < 2; i++){
		p[i] = new player(i+1);
		sAxis[i] = new bool[4];
		posEdge[i] = new bool[5]; 
		negEdge[i] = new bool[5];
		sprintf(buffer, "Misc/P%iSelect0.png", i+1);
		cursor[i] = aux::load_image(buffer);
		counter[i] = 0;
	}
	for(int i = 0; i < 5; i++){
		posEdge[0][i] = 0;
		posEdge[1][i] = 0;
		negEdge[0][i] = 0;
		negEdge[1][i] = 0;
		if(i < 4){
			sAxis[0][i] = 0;
			sAxis[1][i] = 0;
		}
	}

	/*Game and round end conditions*/
	gameover = 0;
	numRounds = 2;

	SDL_Event temp;
	while(SDL_PollEvent(&temp));
	/*Select characters.*/
	selectScreen = aux::load_image("Misc/Select.png");
	wheel.x = 100; wheel.y = 0;

	/*Start a match*/
	matchInit();
}

/*This functions sets things up for a new match. Initializes some things and draws the background*/
void interface::matchInit()
{
	select[0] = 0;
	select[1] = 0;
	printf("Please select a character:\n");
	p[0]->rounds = 0;
	p[1]->rounds = 0;
	background = IMG_Load("Misc/BG1.png");
	q = 0;
}

/*Sets stuff up for a new round. This initializes the characters, the timer, and the background.*/
void interface::roundInit()
{
	bg.x = 400;
	bg.y = 300;
	
	for (int i = 0; i < 2; i++) {
		p[i]->pick->init();
		p[i]->deltaX = 0;
		p[i]->deltaY = 0;
		p[i]->sprite = NULL;
	}

	for(int i = 0; i < 2; i++){
		if(p[i]->pick->cMove != p[i]->pick->neutral) {
			if(p[i]->pick->cMove) p[i]->pick->cMove->init();
			p[i]->pick->neutral->init();
			p[i]->pick->cMove = p[i]->pick->neutral;
		}
	}

	
	/*Background color, temporary until we have backgrounds*/
	SDL_FillRect(screen, &screen->clip_rect, SDL_MapRGB(screen->format, 255, 212, 120));

	/*Initialize input containers*/
	for(int i = 0; i < 4; i++) 
	{
		sAxis[0][i] = 0;
		sAxis[1][i] = 0;
	}
	for(int i = 0; i < 5; i++){
		posEdge[0][i] = 0;
		negEdge[0][i] = 0;
		posEdge[1][i] = 0;
		negEdge[1][i] = 0;
	}

	combo[0] = 0;
	combo[1] = 0;
	grav = 3;
	timer = 5824;
	p[0]->facing = 1;
	p[1]->facing = -1;
	p[0]->posX = 700;
	p[1]->posX = 900;
	
	for (int i = 0; i < 2; i++) {
		p[i]->posY = floor - p[i]->pick->neutral->collision[0].h;
		p[i]->spriteInit();
		p[i]->updateRects();
	}
	draw();
}

/*Pretty simple timer modifier*/
void interface::runTimer()
{
	if(timer > 0) timer--;
}

/*Main function for a frame. This resolves character spritions, background scrolling, and hitboxes*/
void interface::resolve()
{
	if(!select[0] || !select[1]) cSelectMenu(); else {
		p[0]->pushInput(sAxis[0], posEdge[0], negEdge[0]);
		p[1]->pushInput(sAxis[1], posEdge[1], negEdge[1]);

	/*Current plan for this function: Once I've got everything reasonably functionally abstracted into player members,
	the idea is to do the procedure as follows:
		1. Update to current rectangles. Since the actual step is part of the draw routine, this should happen first.
		2. Figure out all deltas that should currently apply to sprite spritions. Basically move the sprites to where
		   they'd be if there were no collision rules.
		3. Check for things like hits and blocks. Enact all the effects of these, including stun, damage, etc.
		4. Check for collision against the opponent or against boundaries such as the floor or the corners.
		   Haven't decided if there should be a cieling yet.
		5. Apply any changes to deltas that need to be made before the next frame
		6. Initialize sprites.
	*/

		p[0]->updateRects();
		p[1]->updateRects();
		p[0]->pullVolition();
		p[1]->pullVolition();
	
		p[0]->updateRects();
		p[1]->updateRects();

		if(!p[0]->pick->freeze){
			p[0]->combineDelta();
			p[0]->enforceGravity(grav, floor);
		}
		if(!p[1]->pick->freeze){
			p[1]->combineDelta();
			p[1]->enforceGravity(grav, floor);
		}

		dragBG(p[1]->dragBG(bg.x + wall, bg.x + screenWidth - wall) +
			p[0]->dragBG(bg.x + wall, bg.x + screenWidth - wall) );
		p[0]->checkCorners(floor, bg.x + wall, bg.x + screenWidth - wall);
		p[1]->checkCorners(floor, bg.x + wall, bg.x + screenWidth - wall);
		
		if (aux::checkCollision(p[0]->collision, p[1]->collision)){
			unitCollision();
		}
		
		if(p[0]->pick->cMove->state[p[0]->pick->cMove->cFlag].i & 1 && p[0]->pick->cMove != p[0]->pick->airNeutral) 
			p[0]->checkFacing(p[1]);
		if(p[1]->pick->cMove->state[p[1]->pick->cMove->cFlag].i & 1 && p[1]->pick->cMove != p[1]->pick->airNeutral) 
			p[1]->checkFacing(p[0]);
		
		
		if(!p[0]->pick->aerial) { p[0]->deltaX = 0; p[0]->deltaY = 0; }
		if(!p[1]->pick->aerial) { p[1]->deltaX = 0; p[1]->deltaY = 0; }
	
		if(p[0]->pick->cMove != p[0]->pick->reel && p[0]->pick->cMove != p[0]->pick->fall && p[0]->pick->cMove != p[0]->pick->crouchReel) combo[1] = 0;
		if(p[1]->pick->cMove != p[1]->pick->reel && p[1]->pick->cMove != p[1]->pick->fall && p[1]->pick->cMove != p[1]->pick->crouchReel) combo[0] = 0;
	
		if(p[1]->hitbox[0].w > 0) p[0]->checkBlocking();
		if(p[0]->hitbox[0].w > 0) p[1]->checkBlocking();
	
		//Check if moves hit. This will probably be a function at some point
		resolveHits();	

		/*Draw the sprites*/
		p[0]->spriteInit();
		p[1]->spriteInit();
		draw();
		for(int i = 0; i < 2; i++)
			p[i]->pick->tick();
		checkWin();
		runTimer();
	}
	/*Reinitialize inputs*/
	for(int i = 0; i < 5; i++){
		posEdge[0][i] = 0;
		posEdge[1][i] = 0;
		negEdge[0][i] = 0;
		negEdge[1][i] = 0;
	}

}


/*Check if someone won*/
void interface::checkWin()
{
	if(p[0]->pick->health == 0 || p[1]->pick->health == 0 || timer == 0){
		for(int i = 0; i < 2; i++) p[i]->pick->cMove->init();
		if(p[0]->pick->health > p[1]->pick->health) {
			printf("Player 1 wins!\n");
			p[0]->rounds++;
		}
		else if(p[1]->pick->health > p[0]->pick->health) {
			printf("Player 2 wins!\n");
			p[1]->rounds++;
		}
		else {
			printf("Draw!\n");
			if(p[0]->rounds < numRounds - 1) p[0]->rounds++;
			if(p[1]->rounds < numRounds - 1) p[1]->rounds++;
		}
		p[0]->momentumComplexity = 0;
		p[1]->momentumComplexity = 0;
		if(p[0]->rounds == numRounds || p[1]->rounds == numRounds){
			delete p[0]->pick;
			delete p[1]->pick;
			matchInit();
		}
		else roundInit();
	}
}

/*Read the input that's happened this frame*/
void interface::readInput()
{
	/*Make our dummy event for polls*/
	SDL_Event event;
	for(int i = 0; i < 20; i++){
		if (SDL_PollEvent(&event)){
			/*Do stuff with event*/
			for(int i = 0; i < 2; i++)
				p[i]->readEvent(event, sAxis[i], posEdge[i], negEdge[i]);
			switch (event.type){
			/*Kill handler*/
			case SDL_QUIT:
				gameover = 1;
				break;
				/*Keyboard handler. Maybe I'll optimize such that the knows if it even needs to check this (EG if sticks are used)*/
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
				case SDLK_q:
				case SDLK_ESCAPE:
					gameover = 1;
					break;
				default:
					break;
				}
				break;
			}
		}
	}
}



void interface::cSelectMenu()
{
	/*The plan is that this is eventually a menu, preferably pretty visual, in which players can select characters.*/
	char base[2][40];

	for(int i = 0; i < 2; i++){
		if(sAxis[i][2] && !select[i] && counter[i] == 0){
			selection[i]--;
			if(selection[i] < 0) selection[i] = numChars;
			sprintf(base[i], "Misc/P%iSelect%i.png", i+1, selection[i]);
			cursor[i] = aux::load_image(base[i]);
			counter[i] = 10;
		}
		if(sAxis[i][3] && !select[i] && counter[i] == 0){
			selection[i]++;
			if(selection[i] > numChars) selection[i] = 0;
			sprintf(base[i], "Misc/P%iSelect%i.png", i+1, selection[i]);
			cursor[i] = aux::load_image(base[i]);
			counter[i] = 10;
		}
		for(int j = 0; j < 5; j++){
			if(posEdge[i][j]){
				select[i] = 1;
				p[i]->characterSelect(selection[i]);
			}
		}
	}
	SDL_FillRect(screen, &screen->clip_rect, SDL_MapRGB(screen->format, 100, 100, 100));
	SDL_BlitSurface(selectScreen, NULL, screen, &wheel);
	SDL_BlitSurface(cursor[0], NULL, screen, &wheel);
	SDL_BlitSurface(cursor[1], NULL, screen, &wheel);
	SDL_UpdateRect(screen, 0, 0, 0, 0);	
	for(int i = 0; i < 2; i++) if(counter[i] > 0) counter[i]--;
	if(select[0] && select[1]) roundInit();
}

void interface::dragBG(int deltaX)
{
	bg.x += deltaX;
	if(bg.x < 0) bg.x = 0;
	else if(bg.x > 800) bg.x = 800;
}

interface::~interface()
{
	SDL_FreeSurface(screen);
	SDL_FreeSurface(background);
	SDL_FreeSurface(selectScreen);
	SDL_FreeSurface(cursor[0]);
	SDL_FreeSurface(cursor[1]);
	if(select[0]) delete p[0]->pick;
	if(select[1]) delete p[1]->pick;
	delete p[0];
	delete p[1];
	SDL_Quit();
}

void interface::unitCollision()
{
	player *right, *left;
	/*Collision between players. Unfortunately a lot of specialcasing necessary here.*/
	if(p[0]->posX > p[1]->posX){ right = p[0]; left = p[1]; }
	else if(p[0]->posX < p[1]->posX) { right = p[1]; left = p[0]; }
	else {
		if(p[1]->facing == 1) { right = p[0]; left = p[1]; }
		else { right = p[1]; left = p[0]; }
	}
	
	int rLOffset = right->posX - right->collision.x;
	int rROffset = right->posX - (right->collision.x + right->collision.w);
	int lLOffset = left->posX - left->collision.x;
	int lROffset = left->posX - (left->collision.x + left->collision.w);
	int dOffset = (left->deltaX - right->deltaX) % 2;
	int middle = (right->collision.x + left->collision.x + left->collision.w)/2;
	if(abs(left->deltaX) > abs(right->deltaX)) middle += dOffset;

	if(left->lCorner) right->posX = left->collision.x + left->collision.w + rLOffset;
	else if(right->rCorner) left->posX = right->collision.x + lROffset;
	else if(middle - left->collision.w < 25) {
		left->posX = 25 + lLOffset;
		right->posX = left->collision.x + left->collision.w + rLOffset;
		left->lCorner = true;
	} else if (middle + right->collision.w > 1575) {
		right->posX = 1575 + rROffset;
		left->posX = right->collision.x + lROffset;
		right->rCorner = true;
	}
	else {
		right->posX = middle + right->collision.w + rROffset;
		left->posX = middle - left->collision.w + lLOffset;
	}
	right->updateRects();
	left->updateRects();
}

void interface::resolveHits()
{
	hStat s[2];
	bool hit[2] = {0, 0};
	for(int i = 0; i < 2; i++){
		for(int j = 0; j < p[i]->hitComplexity; j++){
			for(int k = 0; k < p[(i+1)%2]->regComplexity; k++){
				if(aux::checkCollision(p[i]->hitbox[j], p[(i+1)%2]->hitreg[k])){
					hit[i] = 1;
					p[i]->pick->cMove->pollStats(s[i]);
					k = p[(i+1)%2]->regComplexity;
					j = p[i]->hitComplexity;
				}
			}
		}
	}
	for(int i = 0; i < 2; i++){ 
		if(hit[i]){
			p[i]->connect(combo[i], s[i]);
		}
	}
	for(int i = 0; i < 2; i++){ 
		if(hit[i]){
			combo[i] += p[(i+1)%2]->takeHit(combo[i], s[i]);
			if(combo[i] > 1) printf("Player %i: %i hit combo\n", i+1, combo[i]);
		}
	}
}
