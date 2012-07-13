/*Interface class for GUFG
 *This will run all the main game functions within GUFG
 *
 *Written by Alex Kelly in 2012
 *License to come
 */

#include "interface.h"
#include <cstring>
#include <math.h>
#include <assert.h>
#include <SDL/SDL_opengl.h>
#include <algorithm>
interface::interface()
{

	numChars = 2;
	char buffer[50];
	/*Initialize some pseudo-constants*/
	screenWidth = 800; //By screen, I mean the window the game occurs in.
	screenHeight = 450;
	bg.w = 1600;       //By background, I mean the thing the characters actually move on. Bigger than the screen.
	bg.h = 900;
	floor = bg.h - 25; //Value of the floor. This is the maximum distance downward that characters can travel.
	wall = 25;         //The size of the offset at which characters start to scroll the background, and get stuck.

	assert(screenInit() != false);

	/*Initialize players.*/
	for(int i = 0; i < 2; i++){
		p[i] = new player(i+1);
		sAxis[i] = new bool[4];
		posEdge[i] = new bool[5]; 
		negEdge[i] = new bool[5];
		sprintf(buffer, "Misc/P%iSelect0.png", i+1);
		cursor[i] = aux::load_texture(buffer);
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
		select[i] = 0;
		selection[i] = 0;
	}

	/*Game and round end conditions*/
	gameover = 0;
	numRounds = 2;

	SDL_Event temp;
	while(SDL_PollEvent(&temp));
	/*Select characters.*/
	selectScreen = aux::load_texture("Misc/Select.png");

	/*Start a match*/
	matchInit();
}

bool interface::screenInit()
{
	/*Initialize SDL*/
	if(SDL_Init(SDL_INIT_EVERYTHING) < 0) return false;
	/*WM stuff*/
	SDL_WM_SetCaption("GUFG", "GUFG");
	if((screen = SDL_SetVideoMode(screenWidth, screenHeight, 32, SDL_OPENGL)) == NULL)
		return false;
	SDL_ShowCursor(SDL_DISABLE);

	/*Set up input buffers and joysticks*/
	for(int i = 0; i < SDL_NumJoysticks(); i++)
		SDL_JoystickOpen(i);
//	glDisable (GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable( GL_BLEND );
	glEnable (GL_POINT_SMOOTH);
	glEnable (GL_LINE_SMOOTH);
	glEnable (GL_POLYGON_SMOOTH);

	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

	glClearColor(0, 0, 0, 0);
	glClearDepth(1.0f);
	glViewport(0, 0, screenWidth, screenHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, screenWidth, screenHeight, 0, 1, -1);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	return true;
}

/*This functions sets things up for a new match. Initializes some things and draws the background*/
void interface::matchInit()
{
	SDL_Event event;
	select[0] = 0;
	select[1] = 0;
	printf("Please select a character:\n");
	p[0]->rounds = 0;
	p[1]->rounds = 0;
	p[0]->secondInstance = 0;
	p[1]->secondInstance = 0;
	background = aux::load_texture("Misc/BG1.png");
	q = 0;
	while (SDL_PollEvent(&event));
}

/*Sets stuff up for a new round. This initializes the characters, the timer, and the background.*/
void interface::roundInit()
{
	bg.x = 400;
	bg.y = 450;

	for(int i = 0; i < 2; i++){
		p[i]->posY = floor - p[i]->pick->neutral->collision[0].h;
		p[i]->roundInit();
	}
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
	timer = 60 * 99;
	prox.w = 200;
	prox.h = 0;
	
	draw();
}

/*Pretty simple timer modifier*/
void interface::runTimer()
{
	for(int i = 0; i < 2; i++){
		if(select[i] == true){
			if(p[i]->pick->cMove != NULL)
			{
				timer += (p[i]->pick->cMove->arbitraryPoll(31));
				if(timer > 60*99) timer = 60*99;
			}
		}
	}
	if(timer > 0) timer--;
//	if(timer % 60 == 0) printf("%i seconds remaining\n", timer / 60);
//	printf("%i frames remaining\n", timer);
}

/*Main function for a frame. This resolves character spritions, background scrolling, and hitboxes*/
void interface::resolve()
{
	if(!select[0] || !select[1]) cSelectMenu(); 
	else {
		for(int i = 0; i < 2; i++) p[i]->pushInput(sAxis[i]);
		p[1]->getMove(posEdge[1], negEdge[1], prox, 1);
		for(int i = 0; i < 2; i++){
			if(p[(i+1)%2]->pick->aerial) prox.y = 1;
			else prox.y = 0;
			prox.x = p[(i+1)%2]->throwInvuln;
			p[i]->getMove(posEdge[i], negEdge[i], prox, 0);
		}
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

		resolveThrows();
		doSuperFreeze();

		p[0]->updateRects();
		p[1]->updateRects();
		for(int i = 0; i < 2; i++){
			if(!p[i]->pick->freeze){
				p[i]->pullVolition();
				p[i]->combineDelta();
				p[i]->enforceGravity(grav, floor);
			}
		}

		dragBG(p[1]->dragBG(bg.x + wall, bg.x + screenWidth - wall) +
			p[0]->dragBG(bg.x + wall, bg.x + screenWidth - wall) );
		p[0]->checkCorners(floor, bg.x + wall, bg.x + screenWidth - wall);
		p[1]->checkCorners(floor, bg.x + wall, bg.x + screenWidth - wall);
		
		unitCollision();
		
		if(p[0]->pick->cMove->state[p[0]->pick->cMove->cFlag].i & 1 && p[0]->pick->cMove != p[0]->pick->airNeutral) 
			p[0]->checkFacing(p[1]);
		if(p[1]->pick->cMove->state[p[1]->pick->cMove->cFlag].i & 1 && p[1]->pick->cMove != p[1]->pick->airNeutral) 
			p[1]->checkFacing(p[0]);

		for(int i = 0; i < 2; i++){
			if(!p[i]->pick->aerial) { p[i]->deltaX = 0; p[i]->deltaY = 0; }

			if(p[i]->pick->cMove != p[i]->pick->reel && p[i]->pick->cMove != p[i]->pick->untech && p[i]->pick->cMove != p[i]->pick->crouchReel){
				combo[(i+1)%2] = 0;
				p[i]->elasticX = 0;
				p[i]->elasticY = 0;
			}

		}
		if(p[1]->hitbox[0].w > 0) p[0]->checkBlocking();
		if(p[0]->hitbox[0].w > 0) p[1]->checkBlocking();

		//Check if moves hit. This will probably be a function at some point
		resolveHits();

		/*Draw the sprites*/
		draw();
		for(int i = 0; i < 2; i++){
			p[i]->pick->step();
		}
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
			cursor[i] = aux::load_texture(base[i]);
			counter[i] = 10;
		}
		if(sAxis[i][3] && !select[i] && counter[i] == 0){
			selection[i]++;
			if(selection[i] > numChars) selection[i] = 0;
			sprintf(base[i], "Misc/P%iSelect%i.png", i+1, selection[i]);
			cursor[i] = aux::load_texture(base[i]);
			counter[i] = 10;
		}
		for(int j = 0; j < 5; j++){
			if(posEdge[i][j] && !select[i]){
				select[i] = 1;
				p[i]->characterSelect(selection[i]);
			}
		}
	}
	
	glClear(GL_COLOR_BUFFER_BIT);

	glColor4f(0.1f, 0.1f, 0.1f, 1.0f);
	glRectf(0.0f, 0.0f, (GLfloat)(screenWidth), (GLfloat)(screenHeight));
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	glEnable( GL_TEXTURE_2D );
	glBindTexture(GL_TEXTURE_2D, selectScreen);
	glBegin(GL_QUADS);
		glTexCoord2i(0, 0);
		glVertex3f(175.0f, 0.0f, 0.f);

		glTexCoord2i(1, 0);
		glVertex3f(625.0f, 0.0f, 0.f);

		glTexCoord2i(1, 1);
		glVertex3f(625.0f, 450.0f, 0.f);

		glTexCoord2i(0, 1);
		glVertex3f(175.0f, 450.0f, 0.f);
	glEnd();
	
	for(int i = 0; i < 2; i++){
		glBindTexture(GL_TEXTURE_2D, cursor[i]);
		glBegin(GL_QUADS);
			glTexCoord2i(0, 0);
			glVertex3f(175.0f, 0.0f, 0.f);

			glTexCoord2i(1, 0);
			glVertex3f(625.0f, 0.0f, 0.f);

			glTexCoord2i(1, 1);
			glVertex3f(625.0f, 450.0f, 0.f);

			glTexCoord2i(0, 1);
			glVertex3f(175.0f, 450.0f, 0.f);
		glEnd();
	}
	glDisable( GL_TEXTURE_2D );

	for(int i = 0; i < 2; i++) if(counter[i] > 0) counter[i]--;
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	SDL_GL_SwapBuffers();
	if(select[0] && select[1]){
		if(selection[0] == selection[1]) p[1]->secondInstance = true;
		roundInit();
	}
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
	if(select[0]) delete p[0]->pick;
	if(select[1]) delete p[1]->pick;
	delete p[0];
	delete p[1];
	SDL_Quit();
}

void interface::unitCollision()
{
	player *right = p[1], *left = p[0];
	int middle[2];
	for(int i = 0; i < 2; i++){
		if(p[i]->facing == 1) middle[i] = p[i]->collision.x + p[i]->collision.w / 2;
		else middle[i] = p[i]->collision.x + p[i]->collision.w / 2 + p[i]->collision.w % 2;
	}
	if(middle[0] > middle[1]){ right = p[0]; left = p[1]; }
	else if(middle[0] < middle[1]){ right = p[1]; left = p[0]; }
	else {
		if(p[0]->facing == 1 && p[1]->facing == -1){ left = p[0]; right = p[1]; }
		else if(p[1]->facing == 1 && p[0]->facing == -1){ left = p[1]; right = p[0]; }
	}
	if (aux::checkCollision(p[0]->collision, p[1]->collision)){

	/*Collision between players. Unfortunately a lot of specialcasing necessary here.*/

		int rLOffset = right->posX - right->collision.x;
		int rROffset = right->posX - (right->collision.x + right->collision.w);
		int lLOffset = left->posX - left->collision.x;
		int lROffset = left->posX - (left->collision.x + left->collision.w);
		int dOffset = (left->deltaX - right->deltaX) % 2;
		int totalMiddle = (right->collision.x + left->collision.x + left->collision.w)/2;
		if(abs(left->deltaX) > abs(right->deltaX)) totalMiddle += dOffset;

		if(left->lCorner){ 
			right->posX = left->collision.x + left->collision.w + rLOffset;
			if(left->elasticX && left->deltaX < 0) left->deltaX = -(left->deltaX);
		}
		else if(right->rCorner) left->posX = right->collision.x + lROffset;
		else {
			right->posX = totalMiddle + right->collision.w + rROffset;
			if(right->elasticX && right->deltaX > 0) right->deltaX = -(right->deltaX);
			left->posX = totalMiddle - left->collision.w + lLOffset;
		}
		if(left->collision.x < 25) {
			left->checkCorners(floor, bg.x + wall, bg.x + screenWidth - wall);
			left->updateRects();
			right->posX = left->collision.x + left->collision.w + rLOffset;
			left->lCorner = true;
		} else if (right->collision.x + right->collision.w > 1575) {
			right->checkCorners(floor, bg.x + wall, bg.x + screenWidth - wall);
			right->updateRects();
			left->posX = right->collision.x + lROffset;
			right->rCorner = true;
		}
		right->updateRects();
		left->updateRects();
	}
	prox.w = right->posX - left->posX;
	prox.h = abs(right->posY - left->posY);
}

void interface::resolveThrows()
{
	bool isThrown[2] = {false, false};
	for(int i = 0; i < 2; i++){
		if(p[i]->pick->cMove->arbitraryPoll(28)) isThrown[(i+1)%2] = true;
	}
	if(isThrown[0] && isThrown[1]){
		p[0]->pick->cMove = p[0]->pick->throwBreak;
		p[1]->pick->cMove = p[1]->pick->throwBreak;
	} else {
		for(int i = 0; i < 2; i++){
			if(isThrown[i]){
				p[i]->getThrown(p[(i+1)%2]->pick->cMove, p[(i+1)%2]->posX*p[(i+1)%2]->facing, p[(i+1)%2]->posY);
				p[i]->checkFacing(p[(i+1)%2]);
			}
		}
	}
}

void interface::resolveHits()
{
	hStat s[2];
	bool hit[2] = {0, 0};
	SDL_Rect residual = {0, 0, 1, 0};
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
			if(p[(i+1)%2]->CHState()){
				s[i].stun += s[i].stun / 3;
				s[i].untech += s[i].untech / 3;
			}
			p[i]->connect(combo[i], s[i]);
			if(!p[i]->pick->aerial) p[i]->checkFacing(p[(i+1)%2]);
		}
	}

	for(int i = 0; i < 2; i++){ 
		if(hit[i]){
			combo[i] += p[(i+1)%2]->takeHit(combo[i], s[i]);
			if(combo[i] > 1) printf("Player %i: %i hit combo\n", i+1, combo[i]);
			p[i]->checkCorners(floor, bg.x + wall, bg.x + screenWidth - wall);
			if(p[i]->facing * p[(i+1)%2]->facing == 1) p[(i+1)%2]->invertVectors(1);
		}
	}
	
	for(int i = 0; i < 2; i++){ 
		if(hit[i]){
			if(p[i]->pick->aerial) residual.y = -3;
			else if(p[(i+1)%2]->rCorner || p[(i+1)%2]->lCorner){
				residual.x = -(s[i].push);
				if(combo[i] > 1) residual.x -= (combo[i]-1);
				p[i]->addVector(residual);
			}
		}
	}

	if(hit[0] || hit[1]){
		unitCollision();
	}

	for(int i = 0; i < 2; i++) {
		p[i]->throwInvuln--;
		p[i]->hover--;
	}
}

void interface::doSuperFreeze()
{
	int go[2];
	for(int i = 0; i < 2; i++){
		go[i] = p[i]->pick->cMove->arbitraryPoll(2);
		if(go[i] > 0) p[(i+1)%2]->pick->freeze += go[i];
	}
	if(go[0] > 0 || go[1] > 0)
		freeze = std::max(go[0], go[1]);
}
