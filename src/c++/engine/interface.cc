/*Interface class for GUFG.
 *This will run all the main game functions within GUFG.
 *No kidding.
 *Written by Alex Kelly in 2012.
 *Mangled by H Forrest Alexander in the autumn of that same year.
 *I think there's a license somewhere.
 */

#include "interface.h"
#include <SDL/SDL_opengl.h>
#include <algorithm>
#include <assert.h>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include <math.h>
interface::interface()
{
	numChars = 2;
	stats = new chart(numChars);
	shortcut = false;
	continuous = false;
	analytics = false;
	pauseEnabled = false;
	single = false;
	boxen = false;
	std::ifstream read;

	/*Initialize some pseudo-constants*/
	screenWidth = 1600; /*screen{Width,Height} describe the size of the window holding the game.*/
	screenHeight = 900;
	screen = NULL;
	bg.w = 3200; /*The screen gives a partial view of the background, which is the area available for character movement.*/
	bg.h = 1800;
	floor = 50; /*Value of the floor. This is the maximum distance downward that characters can travel.*/
	wall = 50; /*The size of the offset at which characters start to scroll the background, and get stuck.*/
	menuMusic = NULL;

	select[0] = 0;
	select[1] = 0;

	read.open(".config/resolution.conf");
	if(read.fail()){ 
		scalingFactor = 0.5;
	} else {
		read >> scalingFactor;
		read.ignore(100, '\n');
	}
	read.close();
	sf = scalingFactor;

	/*Game and round end conditions*/
	gameover = 0;
	numRounds = 2;

	initContainers(2, 6);

	currentMatch = NULL;
}

void interface::createPlayers()
{
	/*Initialize players.*/
	for(int i = 0; i < 2; i++){
		P.push_back(new player(i+1));
		p.push_back(P[i]);
		select[i] = 0;
		selection[i] = 1+i;
		menu[i] = 0;
		configMenu[i] = 0;
		things.push_back(P[i]);
	}
}

void interface::createDemons()
{
	srand(time(NULL));
	for(int i = 0; i < 2; i++){
		P.push_back(new demon(i+1));
		p.push_back(P[i]);
		selection[i] = rand()%numChars + 1;
		P[i]->characterSelect(selection[i]); 
		printf("p%i selected %s\n", i+1, things[i]->pick()->name);
		select[i] = 1;
		menu[i] = 0;
		configMenu[i] = 0;
		things.push_back(P[i]);
	}
	continuous = true;
	analytics = true;
	numRounds = 1;
}

void interface::createDemons(replay * script)
{
	srand(time(NULL));
	numRounds = script->rcount;
	for(int i = 0; i < 2; i++){
		P.push_back(new demon(i+1, script->start[i]));
		selection[i] = script->selection[i];
		P[i]->characterSelect(selection[i]);
		select[i] = 1;
		menu[i] = 0;
		configMenu[i] = 0;
		things.push_back(P[i]);
	}
	loadMatchBackground();
	single = true;
}

void interface::loadMatchBackground()
{
	char buffer[100];
	if(selection[0] == selection[1]) P[1]->secondInstance = true;

	sprintf(buffer, "content/stages/%i/bg.png", selection[0]);
	background = aux::load_texture(buffer);

	if(selection[0] == selection[1]) sprintf(buffer, "content/sound/Mirror.ogg");
	else sprintf(buffer, "content/sound/%i.ogg", selection[1]);
	matchMusic = Mix_LoadMUS(buffer);
}

void interface::startGame()
{
	SDL_Event temp;
	while(SDL_PollEvent(&temp));

	/*Start a match*/
	//Mix_PlayChannel(3, announceSelect, 0);
	matchInit();
	if(select[0] && select[1]){ 
		if(analytics) currentMatch = new replay(selection[0], selection[1], numRounds);
		roundInit();
	}
}

/*This function loads a few miscellaneous things the game will need in all cases*/
void gameInstance::loadMisc()
{
	char buffer[200];
	for(int i = 0; i < 91; i++){
		sprintf(buffer, "content/glyphs/%i.png", i);
		glyph[i] = aux::load_texture(buffer);
	}
}

void interface::loadMisc()
{
	gameInstance::loadMisc();
	char buffer[200];
	selectScreen = aux::load_texture("content/menu/Select.png");
	menuMusic = Mix_LoadMUS("content/sound/Menu.ogg");
	announceWinner = new Mix_Chunk*[numChars + 1];
	for(int i = 0; i < numChars + 1; i++){
		sprintf(buffer, "content/sound/announcer/Win%i.ogg", i);
		announceWinner[i] = Mix_LoadWAV(buffer);
	}
	for(unsigned int i = 0; i < p.size(); i++){
		if(!p[i]->readConfig(i+1)) initialConfig(i);
	}
	stats->init();
	announceRound[0] = Mix_LoadWAV("content/sound/announcer/Round1.ogg");
	announceRound[1] = Mix_LoadWAV("content/sound/announcer/Round2.ogg");
	announceRound[2] = Mix_LoadWAV("content/sound/announcer/RoundF.ogg");
	announceDraw[1] = Mix_LoadWAV("content/sound/announcer/Draw.ogg");
	announceFight = Mix_LoadWAV("content/sound/announcer/Fight.ogg");
	announceEnd[0] = Mix_LoadWAV("content/sound/announcer/Timeout.ogg");
	announceEnd[1] = Mix_LoadWAV("content/sound/announcer/Down.ogg");
	announceSelect = Mix_LoadWAV("content/sound/announcer/Select.ogg");
}

/*Initialize SDL and openGL, creating a window, among other things*/
bool gameInstance::screenInit()
{
	if(scalingFactor == 1.0){ 
		w = screenWidth; h = screenHeight;
	} else {
		h = 450; w = 800;
	}
	if(screen){ 
		SDL_FreeSurface(screen);
		screen = NULL;
	}
	bool ret = window::screenInit();
	glDisable(GL_DEPTH_TEST);
	glDisable (GL_LIGHTING);
	SDL_WM_SetCaption("GUFG", "GUFG");
	Mix_OpenAudio(44100, AUDIO_S16, 2, 2048);
	SDL_ShowCursor(SDL_DISABLE);

	/*Set up input buffers and joysticks*/
	init();
	initd = ret;
	return ret;
}

void gameInstance::initialConfig(int ID)
{
	char buffer[200];
	char pident[30];
	sprintf(pident, "Player %i", ID + 1);
	for(unsigned int i = 0; i < p[ID]->input.size(); i++) p[ID]->input.pop_back();
	for(unsigned int i = 0; i < posEdge[ID].size()+4; i++){
		glClear(GL_COLOR_BUFFER_BIT);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glRectf(0.0f*scalingFactor, 0.0f*scalingFactor, (GLfloat)screenWidth*scalingFactor, (GLfloat)screenHeight*scalingFactor);
		glEnable( GL_TEXTURE_2D );
		glColor4f(0.1f, 0.1f, 0.1f, 1.0f);
		drawGlyph(pident, 0, 1600, 300, 80, 1);
		drawGlyph("Please enter a", 0, 1600, 400, 80, 1);
		sprintf(buffer, "command for %s", p[ID]->inputName[i]);
		drawGlyph(buffer, 0, 1600, 500, 80, 1);
		SDL_GL_SwapBuffers();
		glDisable( GL_TEXTURE_2D );
		glClear(GL_COLOR_BUFFER_BIT);
		p[ID]->setKey(1 << i);
	}
	p[ID]->writeConfig(ID+1);
}

/*This functions sets things up for a new match. Initializes some things and draws the background*/
void interface::matchInit()
{
	SDL_Event event;
	rMenu = 0;
	for(unsigned int i = 0; i < P.size(); i++){
		P[i]->rounds = 0;
		P[i]->secondInstance = 0;
	}
	pMenu = 0;
	if(!select[0] || !select[1]){
		Mix_VolumeMusic(100);
		Mix_PlayMusic(menuMusic,-1);
		printf("Please select a character:\n");
	}
	while (SDL_PollEvent(&event));
}

/*Sets stuff up for a new round. This initializes the characters, the timer, and the background.*/
void interface::roundInit()
{
	roundEnd = false;
	while(things.size() > P.size())
		things.pop_back();
	while(globals.size() > 0)
		globals.pop_back();
	bg.x = 800;
	bg.y = -900;

	for(unsigned int i = 0; i < P.size(); i++){
		things[i]->posY = floor;
		P[i]->roundInit();
	}
	/*Initialize input containers*/
	initContainers();
	for(unsigned int i = 0; i < P.size(); i++){
		combo[i] = 0;
		damage[i] = 0;
		illegit[i] = 0;
	}

	grav = -6;
	timer = 60 * 101;
	endTimer = 60 * 5;
	prox.w = 200;
	prox.h = 0;
	freeze = 0;
}

/*Pretty simple timer modifier*/
void interface::runTimer()
{
	if(P[0]->rounds == 0 && P[1]->rounds == 0 && timer == 99 * 60 + 2){
		Mix_VolumeMusic(100);
		Mix_PlayMusic(matchMusic,-1);
	}
	int plus;
	for(int i = 0; i < P.size(); i++){
		if(select[i] == true){
			if(things[i]->cMove != NULL)
			{
				plus = (things[i]->cMove->arbitraryPoll(31, things[i]->currentFrame));
				if(plus != 0){ 
					timer += plus;
					if(timer > 60*99) timer = 60*99 + 1;
				}
			}
		}
	}

	if(roundEnd){ 
		if(endTimer <= 60 * 3) endTimer = 0; 
		/*This is a temporary measure since we don't have winposes yet.
		 *This will eventually be replaced by the *ability* to skip them,
		 *But currently does what would happen if you skipped them every time.
		 */
		if(endTimer > 0) endTimer--;
		else{
			things[0]->momentumComplexity = 0;
			things[1]->momentumComplexity = 0;
			if(P[0]->rounds == numRounds || P[1]->rounds == numRounds){
				if(P[0]->rounds == P[1]->rounds);
				else if(P[0]->rounds == numRounds){ 
					P[0]->wins++;
					printf("P1: %i wins\n", P[0]->wins);
				} else {
					P[1]->wins++;
					printf("P2: %i wins\n", P[1]->wins);
				}
				if(P[0]->rounds == P[1]->rounds);
				else if(selection[0] != selection[1]){
					if(P[0]->rounds == numRounds) stats->recordWin(selection[0], selection[1]);
					else stats->recordWin(selection[1], selection[0]);
					printf("Matchup: %f\n", stats->matchup(selection[0], selection[1]));
				} else printf("Mirror\n");
				if(shortcut) rMenu = 1;
				else{
					if(!continuous){
						for(unsigned int i = 0; i < P.size(); i++){
							delete P[i]->pick();
							select[i] = 0;
						}
					}
					if(SDL_WasInit(SDL_INIT_VIDEO) != 0){
						Mix_HaltMusic();
						Mix_FreeMusic(matchMusic);
					}
					if(analytics && currentMatch){
						currentMatch->write();
						delete currentMatch;
						currentMatch = NULL;
					}
					if(single) gameover = true;
					else{
						matchInit();
						if(select[0] && select[1]){
							if(analytics) currentMatch = new replay(selection[0], selection[1], numRounds);
							roundInit();
						}
					}
				}
			}
			else roundInit();
		}
	} else timer--;
}

/*Main function for a frame. This resolves character spritions, background scrolling, and hitboxes*/
void interface::resolve()
{
	if(!select[0] || !select[1]) cSelectMenu(); 
	else if(rMenu) rematchMenu();
	else if(pMenu) pauseMenu();
	else {
		if(timer > 99 * 60){
			for(unsigned int i = 0; i < P.size(); i++){
				if(timer == 106 * 60) things[i]->inputBuffer[0] = 0;
				if(timer == 106 * 60 - 1) things[i]->inputBuffer[0] = i;
				if(timer == 106 * 60 - 2) things[i]->inputBuffer[0] = selection[(i+1)%2] / 10;
				if(timer == 106 * 60 - 3) things[i]->inputBuffer[0] = selection[(i+1)%2] % 10;
				if(timer == 106 * 60 - 4) things[i]->inputBuffer[0] = 0;
				else(things[i]->inputBuffer[0] = 5);
				for(int j = 0; j < 6; j++){
					posEdge[i][j] = 0;
					negEdge[i][j] = 0;
				}
			}
		} else { 
			for(unsigned int i = 0; i < things.size(); i++) things[i]->pushInput(sAxis[things[i]->ID - 1]);
		}
		things[1]->getMove(posEdge[1], negEdge[1], prox, 1);
		for(unsigned int i = 0; i < things.size(); i++){
			if(i < P.size()){
				if(things[(i+1)%2]->aerial) prox.y = 1;
				else prox.y = 0;
				prox.x = things[(i+1)%2]->throwInvuln;
			}
			things[i]->getMove(posEdge[things[i]->ID - 1], negEdge[things[i]->ID - 1], prox, 0);
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

		for(unsigned int i = 0; i < things.size(); i++)
			things[i]->updateRects();

		resolveThrows();
		doSuperFreeze();

		for(unsigned int i = 0; i < things.size(); i++)
			things[i]->updateRects();

		for(unsigned int i = 0; i < things.size(); i++){
			if(!things[i]->freeze){
				if(things[i]->cMove->stop & 4);
				else { 
					things[i]->pullVolition();
					things[i]->combineDelta();
					things[i]->enforceGravity(grav, floor);
				}
				for(unsigned int j = 0; j < globals.size(); j++){
					if(globals[j]->ID != things[i]->ID){
						if((i < P.size() && (globals[j]->effectCode & 1)) || (i > 2 && (globals[j]->effectCode & 2))){
 							things[i]->enforceAttractor(globals[j]);
						}
					}
				}
			}
		}

		/*Really rudimentary camera logic. Really just scrolls the background (Which characters are drawn relative to)
		 *appropriately, attempting to adjust to approximately be looking at the point in the middle of the two characters.
		 */
		int dx = things[1]->dragBG(bg.x + wall, bg.x + screenWidth - wall) + things[0]->dragBG(bg.x + wall, bg.x + screenWidth - wall);
		/*If a character leaves the camera boundaries, follow them immediately*/
		if(!dx){
			dx = -(((bg.x + screenWidth/2) - things[0]->posX) + ((bg.x + screenWidth/2) - things[1]->posX));
			dx /= 10;
			/*Otherwise follow the middle at a rate of (disparity from middle view)/10.
			 *Chosen by trial and error, this rate feels most natural
			 */
		}
		dragBG(dx);
		for(unsigned int i = 0; i < P.size(); i++){
			P[i]->enforceFloor(floor);
			P[i]->checkCorners(bg.x + wall, bg.x + screenWidth - wall);
		}

		resolveCollision();

		if(things[0]->cMove->state[things[0]->connectFlag].i & 1 && things[0]->cMove != P[0]->pick()->airNeutral)
			P[0]->checkFacing(P[1]);
		if(things[1]->cMove->state[things[1]->connectFlag].i & 1 && things[1]->cMove != P[1]->pick()->airNeutral) 
			P[1]->checkFacing(P[0]);

		for(int i = 0; i < P.size(); i++){
			if(!things[i]->aerial) { things[i]->deltaX = 0; things[i]->deltaY = 0; }

			if(!roundEnd){
				switch (P[i]->pick()->comboState(things[i]->cMove)){ 
				case -2: 
					illegit[(i+1)%2] = 1;
					break;
				case 0:
					combo[(i+1)%2] = 0;
					damage[(i+1)%2] = 0;
					P[i]->elasticX = 0;
					P[i]->elasticY = 0;
					illegit[(i+1)%2] = 0;
					break;
				}
			}
		}

		for(unsigned int i = 0; i < things.size(); i++){
			if(things[i]->hitbox[0].w > 0){
				if(!freeze) P[(things[i]->ID)%2]->checkBlocking();
			}
		}

		//Check if moves hit. This will probably be a function at some point
		resolveHits();

		/*Draw the sprites*/
	}
}

void interface::cleanup()
{
	if(!pMenu){
		if(!rMenu && select[0] && select[1]){
			for(unsigned int i = 0; i < things.size(); i++){
				things[i]->step();
				if(i > 1 && things[i]->dead){ 
					things.erase(things.begin()+i);
					i--;
				}
			}
			for(unsigned int i = 0; i < globals.size(); i++){
				if(globals[i]->length <= 0) globals.erase(globals.begin()+i);
				else globals[i]->length--;
			}
			resolveSummons();
			if(!roundEnd) checkWin();
			runTimer();
		}
	/*Reinitialize inputs*/
		if(analytics && currentMatch) currentMatch->append(new frame(sAxis[0], posEdge[0], negEdge[0]), new frame(sAxis[1], posEdge[1], negEdge[1]));
	}
	if(pauseEnabled && !roundEnd){
		for(unsigned int i = 0; i < P.size(); i++){
			if(posEdge[i][5] == 1 && counter[i] <= 0){
				if(pMenu) pMenu = 0;
				else pMenu = 1;
			}
		}
	}
	for(unsigned int i = 0; i < P.size(); i++){
		for(unsigned int j = 0; j < posEdge[i].size(); j++){
			if(posEdge[i][j] > 0) posEdge[i][j]++;
			negEdge[i][j] = 0;
		}
	}
	for(unsigned int i = 0; i < P.size(); i++) if(counter[i] > 0) counter[i]--;
}

void interface::resolveSummons()
{
	action * temp;
	attractor * tvec = NULL, * avec = NULL;
	instance * larva;
	int x, y, f;
	for(unsigned int i = 0; i < things.size(); i++){
		if(things[i]->cMove){
			temp = things[i]->cMove;
			if(temp->arbitraryPoll(50, things[i]->currentFrame)){
				larva = things[i]->pick()->spawn(temp);
				switch (temp->arbitraryPoll(56, things[i]->currentFrame)){
				case 0:
					larva->ID = 0;
					break;
				case 1:
					larva->ID = things[i]->ID;
					break;
				case 2:
					larva->ID = (things[i]->ID)%2+1;
					break;
				}
				if(temp->arbitraryPoll(51, things[i]->currentFrame)){
					x = things[(things[i]->ID)%2]->posX;
					f = things[(things[i]->ID)%2]->facing;
				} else {
					x = things[(things[i]->ID)-1]->posX;
					f = things[(things[i]->ID)-1]->facing;
				}
				if(temp->arbitraryPoll(52, things[i]->currentFrame))
					y = things[(things[i]->ID)%2]->posY;
				else if(temp->arbitraryPoll(53, things[i]->currentFrame))
					y = 0;
				else
					y = things[(things[i]->ID)-1]->posY;
				x += temp->arbitraryPoll(54, things[i]->currentFrame)*f;
				y += temp->arbitraryPoll(55, things[i]->currentFrame);
				larva->facing = f;
				larva->setPosition(x, y);
				things.push_back(larva);
				larva->init();
			}
		}
	}
	for(unsigned int i = 0; i < things.size(); i++){
		if(things[i]->cMove && things[i]->currentFrame == things[i]->cMove->distortSpawn) tvec = things[i]->cMove->distortion;
		if(tvec != NULL){ 
			avec = new attractor;
			avec->x = tvec->x;
			avec->y = tvec->y;
			avec->type = tvec->type;
			avec->length = tvec->length;
			avec->radius = tvec->radius;
			avec->effectCode = tvec->effectCode;
			avec->eventHorizon = tvec->eventHorizon;
			if(things[i]->facing == 1) avec->posX = things[i]->collision.x + things[i]->collision.w / 2;
			else avec->posX = things[i]->collision.x + things[i]->collision.w / 2 + things[i]->collision.w % 2;
			avec->posY = things[i]->collision.y + things[i]->collision.h/2;
			if(avec->type == 0) avec->x *= things[i]->facing;
			switch(tvec->ID){
			case 1:
				avec->ID = things[i]->ID;
				break;
			case 2:
				avec->ID = (things[i]->ID % 2) + 1;
				break;
			default:
				avec->ID = 0;
				break;
			}
			globals.push_back(avec);
			avec = NULL;
			tvec = NULL;
		}
	}
}

/*Check if someone won*/
void interface::checkWin()
{
	if(P[0]->meter[0] == 0 || P[1]->meter[0] == 0 || timer == 0){
		roundEnd = true;
		if(P[0]->meter[0] > P[1]->meter[0]) {
			P[0]->rounds++;
		}
		else if(P[1]->meter[0] > P[0]->meter[0]) {
			P[1]->rounds++;
		}
		else {
			if(P[0]->rounds < numRounds - 1 || continuous) P[0]->rounds++;
			if(P[1]->rounds < numRounds - 1 || continuous) P[1]->rounds++;
		}
	}
}

void gameInstance::genInput()
{
	for(unsigned int i = 0; i < p.size(); i++){
		p[i]->genEvent(sAxis[i], posEdge[i], negEdge[i]);
	}
}

/*Read the input that's happened this frame*/
void interface::processInput(SDL_Event &event)
{
	/*Do stuff with event*/
	for(unsigned int i = 0; i < p.size(); i++){
		int t = p[i]->tap(event);
		if(t == 0) t = p[(i+1)%2]->tap(event);
		if((t < 1 || t > 8) && t < 512){
			if(p[i]->same(event)){
				if(configMenu[i] > 1 && configMenu[i] < 7){
					p[i]->swapKey(1 << (configMenu[i]+2), event);
					configMenu[i]++;
				}
			}
		}
	}
	gameInstance::processInput(event);
}

void gameInstance::processInput(SDL_Event &event)
{
	for(unsigned int i = 0; i < p.size(); i++)
		p[i]->readEvent(event, sAxis[i], posEdge[i], negEdge[i]);
	switch (event.type){
	case SDL_KEYDOWN:
		switch (event.key.keysym.sym) {
		case SDLK_F10:
			if(scalingFactor == 1.0) sf = 0.5;
			else sf = 1.0;
			initd = false;
			break;
		case SDLK_F11:
			initd = false;
			break;
		default:
			harness::processInput(event);
			break;
		}
		break;
	default:
		harness::processInput(event);
		break;
	}
}

void interface::cSelectMenu()
{
	/*The plan is that this is eventually a menu, preferably pretty visual, in which players can select characters.*/
	if(!initd){ 
		std::ofstream write;
		write.open(".config/resolution.conf");
		write << sf << '\n';
		write.close();
		scalingFactor = sf;
		assert(screenInit() != false);
	}

	for(unsigned int i = 0; i < P.size(); i++){
		if(numChars < 2){
			select[i] = 1;
			selection[i] = 1;
		} else {
			if(!menu[i]){
				if(sAxis[i][2] && !select[i] && counter[i] == 0){
					selection[i]--;
					if(selection[i] < 1) selection[i] = numChars;
					counter[i] = 10;
				}
				if(sAxis[i][3] && !select[i] && counter[i] == 0){
					selection[i]++;
					if(selection[i] > numChars) selection[i] = 1;
					counter[i] = 10;
				}
				for(int j = 0; j < 5; j++){
					if(posEdge[i][j] == 1 && !select[i]){
						select[i] = 1;
					}
				}
				if(posEdge[i][5] == 1){
					if(!select[i]) menu[i] = 3;
					else select[i] = 0;
					counter[i] = 10;
				}
			}
		}
	}

	glClear(GL_COLOR_BUFFER_BIT);

	glColor4f(0.1f, 0.1f, 0.1f, 1.0f);
	glRectf(0.0f*scalingFactor, 0.0f*scalingFactor, (GLfloat)screenWidth*scalingFactor, (GLfloat)screenHeight*scalingFactor);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	for(unsigned int i = 0; i < P.size(); i++){
		if(configMenu[i] > 0) keyConfig(i);
		else if(menu[i] > 0) mainMenu(i);
	}

	if(select[0] && select[1]){
		P[0]->characterSelect(selection[0]);
		P[1]->characterSelect(selection[1]);

		loadMatchBackground();
		Mix_HaltMusic();
		if(analytics) if(analytics) currentMatch = new replay(selection[0], selection[1], numRounds);

		roundInit();
	}
}

void interface::mainMenu(int ID)
{
	if(sAxis[ID][0] && !counter[ID]){
		menu[ID]--;
		counter[ID] = 10;
	} else if(sAxis[ID][1] && !counter[ID]){
		menu[ID]++;
		counter[ID] = 10;
	}
	if(menu[ID] > 6) menu[ID] = 1;
	else if(menu[ID] < 1) menu[ID] = 6;
	for(unsigned int i = 0; i < posEdge.size(); i++){
		if(posEdge[ID][i] == 1 && !counter[ID]){
			switch(menu[ID]){
			case 1:
				if(analytics)
					analytics = false;
				else
					analytics = true;
				break;
			case 2:
				configMenu[ID] = 7;
				break;
			case 3:
				menu[ID] = 0;
				break;
			case 4:
				if(shortcut) 
					shortcut = false;
				else 
					shortcut = true;
				break;
			case 5:
				if(pauseEnabled)
					pauseEnabled = false;
				else
					pauseEnabled = true;
				break;
			case 6:
				gameover = 1;
				break;
			}
			
			counter[ID] = 10;
		}
	}
	if(posEdge[ID][5] == 1 && !counter[ID]){ 
		counter[ID] = 10;
		menu[ID] = 0;
	}
}

void interface::keyConfig(int ID)
{
	if(sAxis[ID][0] && !counter[ID]){
		configMenu[ID]--;
		counter[ID] = 10;
	} else if(sAxis[ID][1] && !counter[ID]){
		configMenu[ID]++;
		counter[ID] = 10;
	}
	if(configMenu[ID] > 7) configMenu[ID] = 1;
	else if(configMenu[ID] < 1) configMenu[ID] = 7;
	for(unsigned int i = 0; i < posEdge.size(); i++){
		if(posEdge[ID][i] == 1 && !counter[ID]){
			switch(configMenu[ID]){
			case 1:
				glDisable( GL_TEXTURE_2D );
				initialConfig(ID);
				glEnable( GL_TEXTURE_2D );
				configMenu[ID] = 7;
				break;
			case 7:
				configMenu[ID] = 0;
				menu[ID] = 2;
				p[ID]->writeConfig(ID+1);
				break;
			}
			counter[ID] = 10;
		}
	}
	if(posEdge[ID][5] == 1 && !counter[ID]){ 
		counter[ID] = 10;
		configMenu[ID] = 0;
		menu[ID] = 0;
		p[ID]->writeConfig(ID+1);
	}
}

void interface::dragBG(int deltaX)
{
	int dy = 900;
	bg.x += deltaX;
	if(bg.x < 0) bg.x = 0;
	else if(bg.x > bg.w - screenWidth) bg.x = bg.w - screenWidth;
	for(unsigned int i = 0; i < P.size(); i++){
		if(dy < things[i]->posY + things[i]->collision.h){
			dy = things[i]->posY + things[i]->collision.h;
			if(dy > bg.h) dy = bg.h;
		}
	}
	bg.y = dy - bg.h;
}

void interface::pauseMenu()
{
	for(int j = 0; j < p.size(); j++){
		if(sAxis[j][0] && !counter[j]){
			pMenu--;
			counter[j] = 10;
		} else if(sAxis[j][1] && !counter[j]){ 
			pMenu++;
			counter[j] = 10;
		}
		if(pMenu > 3) pMenu = 1;
		else if(pMenu < 1) pMenu = 3;
		for(unsigned int i = 0; i < posEdge.size(); i++){
			if(posEdge[j][i] == 1){
				switch(pMenu){
				case 1:
					pMenu = 0;
					break;
				case 2:
					for(unsigned int i = 0; i < P.size(); i++){
						delete P[i]->pick();
						select[i] = 0;
						delete [] things[i]->meter;
					}
					Mix_HaltMusic();
					Mix_FreeMusic(matchMusic);
					//Mix_PlayChannel(3, announceSelect, 0);
					matchInit();
					break;
				case 3:
					Mix_HaltMusic();
					Mix_FreeMusic(matchMusic);
					gameover = 1;
					break;
				}
				j = 2;
			}
		}
	}
}

void interface::rematchMenu()
{
	for(int j = 0; j < P.size(); j++){
		if(sAxis[j][0] && !counter[j]){
			rMenu--;
			counter[j] = 10;
		} else if(sAxis[j][1] && !counter[j]){ 
			rMenu++;
			counter[j] = 10;
		}
		if(rMenu > 3) rMenu = 1;
		else if(rMenu < 1) rMenu = 3;
		for(int i = 0; i < 6; i++){
			if(posEdge[j][i] == 1){
				switch(rMenu){
				case 1:
					Mix_HaltMusic();
					rMenu = 0;
					matchInit();
					break;
				case 2:
					for(unsigned int i = 0; i < P.size(); i++){
						delete P[i]->pick();
						select[i] = 0;
						delete [] things[i]->meter;
					}
					Mix_HaltMusic();
					Mix_FreeMusic(matchMusic);
					//Mix_PlayChannel(3, announceSelect, 0);
					matchInit();
					break;
				case 3:
					Mix_HaltMusic();
					Mix_FreeMusic(matchMusic);
					gameover = 1;
					break;
				}
				j = 2;
			}
		}
	}
}

interface::~interface()
{
	if(select[0]) delete P[0]->pick();
	if(select[1]) delete P[1]->pick();
	if(menuMusic != NULL) Mix_FreeMusic(menuMusic);
	delete stats;
	SDL_FreeSurface(screen);
	SDL_Quit();
}

void gameInstance::unitCollision(instance *a, instance *b)
{
	instance *right = b, *left = a;
	if(a->middle() > b->middle()){ right = a; left = b; }
	else if(a->middle() < b->middle()){ right = b; left = a; }
	else {
		if(a->facing == 1 && b->facing == -1){ left = a; right = b; }
		else if(b->facing == 1 && a->facing == -1){ left = b; right = a; }
	}

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
	}
	else if(right->rCorner) left->posX = right->collision.x + lROffset;
	else {
		right->posX = totalMiddle + right->collision.w + rROffset;
		left->posX = totalMiddle - left->collision.w + lLOffset;
	}
	if(left->collision.x < 50) {
		left->updateRects();
		right->posX = left->collision.x + left->collision.w + rLOffset;
	} else if (right->collision.x + right->collision.w > 3150) {
		right->updateRects();
		left->posX = right->collision.x + lROffset;
	}
	right->updateRects();
	left->updateRects();
}

void interface::resolveCollision()
{
	if (aux::checkCollision(P[0]->collision, P[1]->collision))
		unitCollision(P[0], P[1]);
	prox.w = abs(things[0]->posX - things[1]->posX);
	prox.h = abs(things[0]->posY - things[1]->posY);
}

void interface::resolveThrows()
{
	bool isThrown[2] = {false, false};
	for(unsigned int i = 0; i < P.size(); i++){
		if(things[i]->cMove->arbitraryPoll(28, things[i]->currentFrame)){ 
			isThrown[(i+1)%2] = true;
		}
	}
	if(isThrown[0] && isThrown[1]){
		things[0]->cMove = P[0]->pick()->throwBreak;
		things[1]->cMove = P[1]->pick()->throwBreak;
	} else {
		for(unsigned int i = 0; i < P.size(); i++){
			if(isThrown[i]){
				P[i]->getThrown(things[(i+1)%2]->cMove, things[(i+1)%2]->posX*things[(i+1)%2]->facing, things[(i+1)%2]->posY);
				P[i]->checkFacing(P[(i+1)%2]);
			}
		}
	}
}

void interface::resolveHits()
{
	std::vector<hStat> s(things.size());
	std::vector<int> hit(things.size());
	std::vector<bool> connect(things.size());
	std::vector<bool> taken(things.size());
	std::vector<int> hitBy(things.size());
	int h;
	int push[2];
	for(unsigned int i = 0; i < things.size(); i++){
		taken[i] = 0;
		hit[i] = 0;
		connect[i] = 0;
		hitBy[i] = -1;
	}
	SDL_Rect residual = {0, 0, 1, 0};
	for(unsigned int i = 0; i < things.size(); i++){
		for(int h = (int)things.size()-1; h >= 0; h--){
			if(h != i && !taken[h] && !connect[i]){
				for(int j = 0; j < things[i]->hitComplexity; j++){
					for(int k = 0; k < things[h]->regComplexity; k++){
						if(aux::checkCollision(things[i]->hitbox[j], things[h]->hitreg[k])){
							if(things[i]->acceptTarget(things[h])){
								connect[i] = 1;
								things[i]->cMove->pollStats(s[i], things[i]->currentFrame, things[h]->CHState());
								if(i < P.size()) push[i] = s[i].push;
								k = things[h]->regComplexity;
								j = things[i]->hitComplexity;
								taken[h] = 1;
								hitBy[h] = i;
								break;
							}
						}
					}
				}
			}
		}
	}

	for(unsigned int i = 0; i < things.size(); i++){
		if(connect[i]){
			things[i]->connect(combo[things[i]->ID-1], s[i]);
			if(i < P.size() && things[i]->cMove->allowed.i < 128 && !things[i]->aerial) P[i]->checkFacing(P[(i+1)%2]);
		}
	}

	for(unsigned int i = 0; i < things.size(); i++){ 
		if(taken[i]){
			h = things[things[i]->ID-1]->meter[0];
			hit[hitBy[i]] = things[i]->takeHit(combo[things[hitBy[i]]->ID-1], s[hitBy[i]], prox);
			if(i < P.size() && hitBy[i] < P.size()){
				if(things[i]->particleType == -2){
					hStat ths;
					ths.damage = s[hitBy[i]].chip;
					ths.ghostHit = true;
					ths.stun = 0;
					ths.push = s[hitBy[i]].push;
					if(things[i]->aerial) ths.push += s[hitBy[i]].blowback;
					things[hitBy[i]]->takeHit(combo[i], ths, prox);
				}
			}
			if(i < P.size() && s[hitBy[i]].stun) combo[(i+1)%2] += hit[hitBy[i]];
			if(hit[hitBy[i]] == 1) things[hitBy[i]]->hitFlag = things[hitBy[i]]->connectFlag;
			P[(i+1)%2]->enforceFloor(floor);
			P[(i+1)%2]->checkCorners(bg.x + wall, bg.x + screenWidth - wall);
			if(things[i]->facing * things[(i+1)%2]->facing == 1) things[i]->invertVectors(1);
			if(i < P.size()) damage[(i+1)%2] += h - P[i]->meter[0];
		}
	}

	for(unsigned int i = 0; i < P.size(); i++){ 
		if(connect[i]){
			if(P[i]->aerial){ 
				switch(s[i].pause){
				case -1:
					residual.y = 6 + (s[i].stun/4+9)/4;
					break;
				case 0:
					residual.y = 0;
					break;
				default:
					residual.y = 6 + (s[i].pause - 1)/4;
					break;
				}
			} else { 
				if(things[(i+1)%2]->aerial) residual.x = -2;
				else {
					if(combo[i] > 1) residual.x = -3*(abs(combo[i]-1));
					if(things[(i+1)%2]->particleType == -2) residual.x -= push[i];
					else if(P[(i+1)%2]->rCorner || P[(i+1)%2]->lCorner){
						residual.x -= 2;
						residual.x -= s[i].push/2;
						residual.x -= abs(combo[i]);
					}
				}
				residual.x *= things[i]->facing;
			}
			things[i]->addVector(residual);
		}
	}

	if(connect[0] || connect[1]){
		resolveCollision();
	}

	for(unsigned int i = 0; i < P.size(); i++) {
		things[i]->throwInvuln--;
		P[i]->hover--;
	}
	for(unsigned int i = 0; i < P.size(); i++) {
		if(things[i]->meter[0] <= 0 && endTimer >= 5 * 60){ 
			i = 2;
			for(unsigned int j = 0; j < things.size(); j++)
				things[j]->freeze = 30;
		}
	}
}

void interface::doSuperFreeze()
{
	int go[2] = {0, 0};
	for(unsigned int i = 0; i < P.size(); i++){
		go[i] = things[i]->cMove->arbitraryPoll(2, things[i]->currentFrame);
		if(go[i] > 0){ 
			P[(i+1)%2]->checkBlocking();
			things[(i+1)%2]->freeze += go[i];
		}
	}
	if(go[0] > 0 || go[1] > 0)
		freeze = std::max(go[0], go[1]);
}

