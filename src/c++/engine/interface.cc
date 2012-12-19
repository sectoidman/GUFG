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
#include <vector>

interface::interface()
{
	numChars = 2;
	matchup = new int*[numChars+1];
	for(int i = 0; i < numChars+1; i++) matchup[i] = new int[numChars+1];
	shortcut = false;
	continuous = false;
	analytics = false;
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
	p[0] = NULL;
	p[1] = NULL;

	read.open(".config/resolution.conf");
	if(read.fail()){ 
		scalingFactor = 0.5;
		fullscreen = false;
	} else {
		read >> scalingFactor;
		read.ignore(100, '\n');
		read >> fullscreen;
	}
	read.close();
	sf = scalingFactor;

	/*Game and round end conditions*/
	gameover = 0;
	numRounds = 2;

	for(int i = 0; i < 2; i++){
		sAxis[i] = new bool[4];
		posEdge[i] = new int[6]; 
		negEdge[i] = new bool[6];
		counter[i] = 0;
	}

	for(int i = 0; i < 6; i++){
		posEdge[0][i] = 0;
		posEdge[1][i] = 0;
		negEdge[0][i] = 0;
		negEdge[1][i] = 0;
		if(i < 4){
			sAxis[0][i] = 0;
			sAxis[1][i] = 0;
		}
	}
	currentMatch = NULL;
}

void interface::createPlayers()
{
	/*Initialize players.*/
	for(int i = 0; i < 2; i++){
		p[i] = new player(i+1);
		select[i] = 0;
		selection[i] = 1+i;
		menu[i] = 0;
		configMenu[i] = 0;
	}
}

void interface::createDemons()
{
	srand(time(NULL));
	for(int i = 0; i < 2; i++){
		p[i] = new demon(i+1); 
		selection[i] = rand()%numChars + 1;
		p[i]->characterSelect(selection[i]); 
		printf("p%i selected %s\n", i+1, p[i]->pick()->name);
		select[i] = 1;
		menu[i] = 0;
		configMenu[i] = 0;
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
		p[i] = new demon(i+1, script->start[i]);
		selection[i] = script->selection[i];
		p[i]->characterSelect(selection[i]);
		select[i] = 1;
		menu[i] = 0;
		configMenu[i] = 0;
	}
	loadMatchBackground();
	single = true;
}

void interface::loadMatchBackground()
{
	char buffer[100];
	if(selection[0] == selection[1]) p[1]->secondInstance = true;

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
	things = NULL;
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
	for(int i = 0; i < 2; i++){
		if(!p[i]->readConfig(i+1)) initialConfig(i);
	}
	readMatchupChart();
	announceRound[0] = Mix_LoadWAV("content/sound/announcer/Round1.ogg");
	announceRound[1] = Mix_LoadWAV("content/sound/announcer/Round2.ogg");
	announceRound[2] = Mix_LoadWAV("content/sound/announcer/RoundF.ogg");
	announceDraw[1] = Mix_LoadWAV("content/sound/announcer/Draw.ogg");
	announceFight = Mix_LoadWAV("content/sound/announcer/Fight.ogg");
	announceEnd[0] = Mix_LoadWAV("content/sound/announcer/Timeout.ogg");
	announceEnd[1] = Mix_LoadWAV("content/sound/announcer/Down.ogg");
	announceSelect = Mix_LoadWAV("content/sound/announcer/Select.ogg");
}

void interface::readMatchupChart()
{
	std::ifstream read;
	char buffer[500];
	char* token;
	bool fresh = false;
	read.open(".data/.matchups.csv");
	if(read.fail()) fresh = true;
	for(int i = 0; i < numChars + 1; i++){
		if(!fresh){ 
			read.get(buffer, 400, '\n'); read.ignore();
			token = strtok(buffer, "\n,");
		}
		for(int j = 1; j < numChars + 1; j++){
			if(fresh) matchup[i][j] = 0;
			else{
				token = strtok(NULL, "\n, ");
				if(i == j) matchup[i][j] = 0;
				else matchup[i][j] = atoi(token);
			}
		}
	}
	read.close();
}

void interface::writeMatchupChart()
{
	std::ofstream write;
	write.open(".data/.matchups.csv");
	write << " ";
	for(int j = 1; j < numChars + 1; j++){
		write << ",";
		write << j;
	}
	write << "\n";
	for(int i = 1; i < numChars + 1; i++){
		write << i;
		for(int j = 1; j < numChars + 1; j++){
			write << ",";
			if(i == j) write << "-";
			else write << matchup[i][j];
		}
		write << "\n";
	}
	write.close();
}

/*Initialize SDL and openGL, creating a window, among other things*/
bool gameInstance::screenInit()
{
	/*Initialize SDL*/
	if(SDL_Init(SDL_INIT_EVERYTHING) < 0) return false;
	Mix_OpenAudio(44100, AUDIO_S16, 2, 2048);
	/*WM stuff*/
	int h, w;
	if(scalingFactor == 1.0){ 
		w = screenWidth; h = screenHeight;
	} else {
		h = 450; w = 800;
	}
	if(screen){ 
		SDL_FreeSurface(screen);
		screen = NULL;
	}
	SDL_WM_SetCaption("GUFG", "GUFG");
	if(!fullscreen){
		if((screen = SDL_SetVideoMode(w, h, 32, SDL_OPENGL)) == NULL)
			return false;
	} else {
		if((screen = SDL_SetVideoMode(w, h, 32, SDL_OPENGL | SDL_FULLSCREEN)) == NULL)
			return false;
	}
	SDL_ShowCursor(SDL_DISABLE);

	/*Set up input buffers and joysticks*/
	for(int i = 0; i < SDL_NumJoysticks(); i++)
		SDL_JoystickOpen(i);
//	glDisable (GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable (GL_BLEND);
	glEnable (GL_POINT_SMOOTH);
	glEnable (GL_LINE_SMOOTH);
	glEnable (GL_POLYGON_SMOOTH);

	glHint (GL_POLYGON_SMOOTH_HINT, GL_NICEST);

	glClearColor(0, 0, 0, 0);
	glClearDepth(1.0f);
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, w, h, 0, 1, -1);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	initd = true;
	return true;
}

void interface::initialConfig(int ID)
{
	char buffer[200];
	char pident[30];
	sprintf(pident, "Player %i", ID + 1);
	for(int i = 0; i < 10; i++){
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
	p[0]->rounds = 0;
	p[1]->rounds = 0;
	p[0]->secondInstance = 0;
	p[1]->secondInstance = 0;
	if(!select[0] || !select[1]){
		Mix_VolumeMusic(100);
		Mix_PlayMusic(menuMusic,-1);
		printf("Please select a character:\n");
	}
	q = 0;
	while (SDL_PollEvent(&event));
}

/*Sets stuff up for a new round. This initializes the characters, the timer, and the background.*/
void interface::roundInit()
{
	roundEnd = false;
	if(things){ 
		delete [] things;
	}
	things = NULL;
	thingComplexity = 0;
	globals = NULL;
	attractorComplexity = 0;
	for(int i = 0; i < 2; i++)
		addThing(p[i]);
	thingComplexity = 2;
	bg.x = 800;
	bg.y = -900;

	for(int i = 0; i < 2; i++){
		p[i]->posY = floor;
		p[i]->roundInit();
	}
	/*Initialize input containers*/
	for(int i = 0; i < 2; i++){
		for(int j = 0; j < 6; j++){
			if(j < 4) sAxis[i][j] = 0;
			posEdge[i][j] = 0;
			negEdge[i][j] = 0;
		}
		combo[i] = 0;
		damage[i] = 0;
		illegit[i] = 0;
	}

	grav = -6;
	timer = 60 * 101;
	endTimer = 60 * 5;
//	if(p[0]->rounds + p[1]->rounds < 1) timer += 60 * 6;
	prox.w = 200;
	prox.h = 0;
	freeze = 0;
}

/*Pretty simple timer modifier*/
void interface::runTimer()
{
	if(p[0]->rounds == 0 && p[1]->rounds == 0 && timer == 99 * 60 + 2){
		Mix_VolumeMusic(100);
		Mix_PlayMusic(matchMusic,-1);
	}
	int plus;
	for(int i = 0; i < 2; i++){
		if(select[i] == true){
			if(p[i]->cMove != NULL)
			{
				plus = (p[i]->cMove->arbitraryPoll(31, p[i]->currentFrame));
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
			p[0]->momentumComplexity = 0;
			p[1]->momentumComplexity = 0;
			if(p[0]->rounds == numRounds || p[1]->rounds == numRounds){
				if(p[0]->rounds == p[1]->rounds);
				else if(p[0]->rounds == numRounds){ 
					p[0]->wins++;
					printf("P1: %i wins\n", p[0]->wins);
				} else {
					p[1]->wins++;
					printf("P2: %i wins\n", p[1]->wins);
				}
				if(p[0]->rounds == p[1]->rounds);
				else if(selection[0] != selection[1]){
					if(p[0]->rounds == numRounds) matchup[selection[0]][selection[1]]++;
					else matchup[selection[1]][selection[0]]++;
					printf("Matchup: %f\n", (float)matchup[selection[0]][selection[1]] / 
					       ((float)matchup[selection[0]][selection[1]] + (float)matchup[selection[1]][selection[0]]));
				} else printf("Mirror\n");
				if(shortcut) rMenu = 1;
				else{
					if(!continuous){
						delete p[0]->pick();
						delete p[1]->pick();
						select[0] = 0;
						select[1] = 0;
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
	else {
		if(timer > 99 * 60){
			for(int i = 0; i < 2; i++){
				if(timer == 106 * 60) p[i]->inputBuffer[0] = 0;
				if(timer == 106 * 60 - 1) p[i]->inputBuffer[0] = i;
				if(timer == 106 * 60 - 2) p[i]->inputBuffer[0] = selection[(i+1)%2] / 10;
				if(timer == 106 * 60 - 3) p[i]->inputBuffer[0] = selection[(i+1)%2] % 10;
				if(timer == 106 * 60 - 4) p[i]->inputBuffer[0] = 0;
				else(p[i]->inputBuffer[0] = 5);
				for(int j = 0; j < 6; j++){
					posEdge[i][j] = 0;
					negEdge[i][j] = 0;
				}
			}
		} else { 
			for(int i = 0; i < thingComplexity; i++) things[i]->pushInput(sAxis[things[i]->ID - 1]);
		}
		p[1]->getMove(posEdge[1], negEdge[1], prox, 1);
		for(int i = 0; i < thingComplexity; i++){
			if(i < 2){
				if(p[(i+1)%2]->aerial) prox.y = 1;
				else prox.y = 0;
				prox.x = p[(i+1)%2]->throwInvuln;
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

		for(int i = 0; i < thingComplexity; i++)
			things[i]->updateRects();

		resolveThrows();
		doSuperFreeze();

		for(int i = 0; i < thingComplexity; i++)
			things[i]->updateRects();

		for(int i = 0; i < thingComplexity; i++){
			if(!things[i]->freeze){
				if(things[i]->cMove->stop & 4);
				else { 
					things[i]->pullVolition();
					things[i]->combineDelta();
					things[i]->enforceGravity(grav, floor);
				}
				for(int j = 0; j < attractorComplexity; j++){
					if(globals[j]->ID != things[i]->ID){
						if((i < 2 && (globals[j]->effectCode & 1)) || (i > 2 && (globals[j]->effectCode & 2))){
 							things[i]->enforceAttractor(globals[j]);
						}
					}
				}
			}
		}

		/*Really rudimentary camera logic. Really just scrolls the background (Which characters are drawn relative to)
		 *appropriately, attempting to adjust to approximately be looking at the point in the middle of the two characters.
		 */
		int dx = p[1]->dragBG(bg.x + wall, bg.x + screenWidth - wall) + p[0]->dragBG(bg.x + wall, bg.x + screenWidth - wall);
		/*If a character leaves the camera boundaries, follow them immediately*/
		if(!dx){
			dx = -(((bg.x + screenWidth/2) - p[0]->posX) + ((bg.x + screenWidth/2) - p[1]->posX));
			dx /= 10;
			/*Otherwise follow the middle at a rate of (disparity from middle view)/10.
			 *Chosen by trial and error, this rate feels most natural
			 */
		}
		dragBG(dx);
		for(int i = 0; i < 2; i++){
			p[i]->enforceFloor(floor);
			p[i]->checkCorners(bg.x + wall, bg.x + screenWidth - wall);
		}

		unitCollision();

		if(p[0]->cMove->state[p[0]->connectFlag].i & 1 && p[0]->cMove != p[0]->pick()->airNeutral) 
			p[0]->checkFacing(p[1]);
		if(p[1]->cMove->state[p[1]->connectFlag].i & 1 && p[1]->cMove != p[1]->pick()->airNeutral) 
			p[1]->checkFacing(p[0]);

		for(int i = 0; i < 2; i++){
			if(!p[i]->aerial) { p[i]->deltaX = 0; p[i]->deltaY = 0; }

			if(!roundEnd){
				switch (p[i]->pick()->comboState(p[i]->cMove)){ 
				case -2: 
					illegit[(i+1)%2] = 1;
					break;
				case 0:
					combo[(i+1)%2] = 0;
					damage[(i+1)%2] = 0;
					p[i]->elasticX = 0;
					p[i]->elasticY = 0;
					illegit[(i+1)%2] = 0;
					break;
				}
			}
		}

		for(int i = 0; i < thingComplexity; i++){
			if(things[i]->hitbox[0].w > 0){
				if(!freeze) p[(things[i]->ID)%2]->checkBlocking();
			}
		}

		//Check if moves hit. This will probably be a function at some point
		resolveHits();

		/*Draw the sprites*/
	}	
}

void interface::cleanup()
{
	if(!rMenu && select[0] && select[1]){
		for(int i = 0; i < thingComplexity; i++){
			things[i]->step();
			if(i > 1 && things[i]->dead) cullThing(i);
		}
		for(int i = 0; i < attractorComplexity; i++){
			if(globals[i]->length <= 0) cullAttractor(i);
			else globals[i]->length--;
		}
		resolveSummons();
		if(!roundEnd) checkWin();
		runTimer();
	}
	/*Reinitialize inputs*/
	if(analytics && currentMatch) currentMatch->append(new frame(sAxis[0], posEdge[0], negEdge[0]), new frame(sAxis[1], posEdge[1], negEdge[1]));
	for(int i = 0; i < 2; i++){
		for(int j = 0; j < 6; j++){
			if(posEdge[i][j] > 0) posEdge[i][j]++;
			negEdge[i][j] = 0;
		}
	}
	for(int i = 0; i < 2; i++) if(counter[i] > 0) counter[i]--;
}

void interface::resolveSummons()
{
	action * temp;
	attractor * tvec = NULL, * avec = NULL;
	instance * larva;
	int x, y, f;
	for(int i = 0; i < thingComplexity; i++){
		if(things[i]->cMove){
			temp = things[i]->cMove;
			if(temp->arbitraryPoll(50, things[i]->currentFrame)){
				larva = things[i]->pick()->spawn(temp);
				larva->ID = things[i]->ID;
				if(temp->arbitraryPoll(51, things[i]->currentFrame)){
					x = p[(things[i]->ID)%2]->posX;
					f = p[(things[i]->ID)%2]->facing;
				} else {
					x = p[(things[i]->ID)-1]->posX;
					f = p[(things[i]->ID)-1]->facing;
				}
				if(temp->arbitraryPoll(52, things[i]->currentFrame))
					y = p[(things[i]->ID)%2]->posY;
				else if(temp->arbitraryPoll(53, things[i]->currentFrame))
					y = 0;
				else
					y = p[(things[i]->ID)-1]->posY;
				x += temp->arbitraryPoll(54, things[i]->currentFrame)*f;
				y += temp->arbitraryPoll(55, things[i]->currentFrame);
				larva->facing = f;
				larva->setPosition(x, y);
				addThing(larva);
				larva->init();
			}
		}
	}
	for(int i = 0; i < thingComplexity; i++){
		if(things[i]->cMove && things[i]->currentFrame == things[i]->cMove->distortSpawn) tvec = things[i]->cMove->distortion;
		if(tvec != NULL){ 
			avec = new attractor;
			avec->x = tvec->x;
			avec->y = tvec->y;
			avec->type = tvec->type;
			avec->length = tvec->length;
			avec->radius = tvec->radius;
			avec->effectCode = tvec->effectCode;
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
			addAttractor(avec);
			avec = NULL;
			tvec = NULL;
		}
	}
}

/*Check if someone won*/
void interface::checkWin()
{
	if(p[0]->meter[0] == 0 || p[1]->meter[0] == 0 || timer == 0){
		roundEnd = true;
		if(p[0]->meter[0] > p[1]->meter[0]) {
			p[0]->rounds++;
		}
		else if(p[1]->meter[0] > p[0]->meter[0]) {
			p[1]->rounds++;
		}
		else {
			if(p[0]->rounds < numRounds - 1 || continuous) p[0]->rounds++;
			if(p[1]->rounds < numRounds - 1 || continuous) p[1]->rounds++;
		}
	}
}

void interface::genInput()
{
	for(int i = 0; i < 2; i++){
		p[i]->genEvent(sAxis[i], posEdge[i], negEdge[i]);
	}
}

/*Read the input that's happened this frame*/
void gameInstance::readInput()
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
				printf("Sigterm received!\n");
				gameover = 1;
				break;
				/*Keyboard handler. Maybe I'll optimize such that the knows if it even needs to check this (EG if sticks are used)*/
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
				case SDLK_q:
				case SDLK_ESCAPE:
					gameover = 1;
					break;
				case SDLK_F10:
					if(scalingFactor == 1.0) sf = 0.5;
					else sf = 1.0;
					initd = false;
					break;
				case SDLK_F11:
					fullscreen = !fullscreen;
					initd = false;
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
	if(!initd){ 
		std::ofstream write;
		write.open(".config/resolution.conf");
		write << sf << '\n' << fullscreen;
		write.close();
		scalingFactor = sf;
		assert(screenInit() != false);
	}

	for(int i = 0; i < 2; i++){
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

	glClear(GL_COLOR_BUFFER_BIT);

	glColor4f(0.1f, 0.1f, 0.1f, 1.0f);
	glRectf(0.0f*scalingFactor, 0.0f*scalingFactor, (GLfloat)screenWidth*scalingFactor, (GLfloat)screenHeight*scalingFactor);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	for(int i = 0; i < 2; i++) if(menu[i] > 0) mainMenu(i);

	if(select[0] && select[1]){
		p[0]->characterSelect(selection[0]);
		p[1]->characterSelect(selection[1]);

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
	if(menu[ID] > 5) menu[ID] = 1;
	else if(menu[ID] < 1) menu[ID] = 5;
	for(int i = 0; i < 5; i++){
		if(posEdge[ID][i] == 1 && !counter[ID]){
			switch(menu[ID]){
			case 1:
				if(analytics)
					analytics = false;
				else
					analytics = true;
				break;
			case 2:
				glDisable( GL_TEXTURE_2D );
				initialConfig(ID);
				glEnable( GL_TEXTURE_2D );
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

void interface::keyConfig()
{



}

void interface::dragBG(int deltaX)
{
	int dy = 900;
	bg.x += deltaX;
	if(bg.x < 0) bg.x = 0;
	else if(bg.x > 1600) bg.x = 1600;
	for(int i = 0; i < 2; i++){
		if(dy < p[i]->posY + p[i]->collision.h){
			dy = p[i]->posY + p[i]->collision.h;
		}
	}
	bg.y = dy - bg.h;
}

void interface::rematchMenu()
{
	for(int j = 0; j < 2; j++){
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
					delete p[0]->pick();
					delete p[1]->pick();
					select[0] = 0;
					select[1] = 0;
					delete [] p[0]->meter;
					delete [] p[1]->meter;
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
	if(select[0]) delete p[0]->pick();
	if(select[1]) delete p[1]->pick();
	if(p[0] != NULL) delete p[0];
	if(p[1] != NULL) delete p[1];
	if(menuMusic != NULL) Mix_FreeMusic(menuMusic);
	SDL_FreeSurface(screen);
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
	prox.w = right->posX - left->posX;
	prox.h = abs(right->posY - left->posY);
}

void interface::resolveThrows()
{
	bool isThrown[2] = {false, false};
	for(int i = 0; i < 2; i++){
		if(p[i]->cMove->arbitraryPoll(28, p[i]->currentFrame)){ 
			isThrown[(i+1)%2] = true;
		}
	}
	if(isThrown[0] && isThrown[1]){
		p[0]->cMove = p[0]->pick()->throwBreak;
		p[1]->cMove = p[1]->pick()->throwBreak;
	} else {
		for(int i = 0; i < 2; i++){
			if(isThrown[i]){
				p[i]->getThrown(p[(i+1)%2]->cMove, p[(i+1)%2]->posX*p[(i+1)%2]->facing, p[(i+1)%2]->posY);
				p[i]->checkFacing(p[(i+1)%2]);
			}
		}
	}
}

void interface::resolveHits()
{
	std::vector<hStat> s(thingComplexity);
	std::vector<int> hit(thingComplexity);
	std::vector<bool> connect(thingComplexity);
	std::vector<bool> taken(thingComplexity);
	std::vector<int> hitBy(thingComplexity);
	int h;
	int push[2];
	for(int i = 0; i < thingComplexity; i++){
		taken[i] = 0;
		hit[i] = 0;
		connect[i] = 0;
		hitBy[i] = -1;
	}
	SDL_Rect residual = {0, 0, 1, 0};
	for(int i = 0; i < thingComplexity; i++){
		for(int h = 0; h < thingComplexity; h++){
			if(h != i && !taken[h]){
				for(int j = 0; j < things[i]->hitComplexity; j++){
					for(int k = 0; k < things[h]->regComplexity; k++){
						if(aux::checkCollision(things[i]->hitbox[j], things[h]->hitreg[k])){
							if(things[i]->acceptTarget(things[h])){
								connect[i] = 1;
								things[i]->cMove->pollStats(s[i], things[i]->currentFrame, things[h]->CHState());
								if(i < 2) push[i] = s[i].push;
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

	for(int i = 0; i < thingComplexity; i++){
		if(connect[i]){
			things[i]->connect(combo[things[i]->ID-1], s[i]);
			if(i < 2 && p[i]->cMove->allowed.i < 128 && !p[i]->aerial) p[i]->checkFacing(p[(i+1)%2]);
		}
	}

	for(int i = 0; i < thingComplexity; i++){ 
		if(taken[i]){
			h = p[things[i]->ID-1]->meter[0];
			hit[hitBy[i]] = things[i]->takeHit(combo[things[hitBy[i]]->ID-1], s[hitBy[i]], prox);
			if(i < 2 && hitBy[i] < 2){
				if(p[i]->particleType == -2){
					hStat ths;
					ths.damage = s[hitBy[i]].chip;
					ths.ghostHit = true;
					ths.stun = 0;
					p[hitBy[i]]->takeHit(combo[i], ths, prox);
				}
			}
			combo[(i+1)%2] += hit[hitBy[i]];
			if(hit[hitBy[i]] == 1) things[hitBy[i]]->hitFlag = things[hitBy[i]]->connectFlag;
			p[(i+1)%2]->enforceFloor(floor);
			p[(i+1)%2]->checkCorners(bg.x + wall, bg.x + screenWidth - wall);
			if(things[i]->facing * things[(i+1)%2]->facing == 1) things[i]->invertVectors(1);
			if(i < 2) damage[(i+1)%2] += h - p[i]->meter[0];
		}
	}

	for(int i = 0; i < 2; i++){ 
		if(connect[i]){
			if(p[i]->aerial) residual.y = -8;
			else{ 
				if(p[(i+1)%2]->aerial) residual.x = -2;
				else {
					if(combo[i] > 1) residual.x = -3*(abs(combo[i]-1));
					if(p[(i+1)%2]->particleType == -2) residual.x -= push[i];
					else if(p[(i+1)%2]->rCorner || p[(i+1)%2]->lCorner){
						residual.x -= 2;
						residual.x -= s[i].push/2;
						residual.x -= abs(combo[i]);
					}
				}
				residual.x *= p[i]->facing;
				p[i]->addVector(residual);
			}
		}
	}

	if(connect[0] || connect[1]){
		unitCollision();
	}

	for(int i = 0; i < 2; i++) {
		p[i]->throwInvuln--;
		p[i]->hover--;
	}
	for(int i = 0; i < 2; i++) {
		if(p[i]->meter[0] <= 0 && endTimer >= 5 * 60){ 
			i = 2;
			p[0]->freeze = 30;
			p[1]->freeze = 30;
		}
	}
}

void interface::doSuperFreeze()
{
	int go[2] = {0, 0};
	for(int i = 0; i < 2; i++){
		go[i] = p[i]->cMove->arbitraryPoll(2, p[i]->currentFrame);
		if(go[i] > 0){ 
			p[(i+1)%2]->checkBlocking();
			p[(i+1)%2]->freeze += go[i];
		}
	}
	if(go[0] > 0 || go[1] > 0)
		freeze = std::max(go[0], go[1]);
}

void interface::addThing(instance *v)
{
	int i;
	instance ** temp;
	temp = new instance*[thingComplexity+1];
	for(i = 0; i < thingComplexity; i++)
		temp[i] = things[i];
	temp[i] = v;
	if(thingComplexity > 0) delete [] things;
	things = temp;
	thingComplexity++;
}

void interface::cullThing(int q)
{
	for(int i = q; i < thingComplexity - 1; i++)
		things[i] = things[i+1];
	thingComplexity--;
}

void interface::addAttractor(attractor *v)
{
	int i;
	attractor ** temp;
	temp = new attractor*[attractorComplexity+1];
	for(i = 0; i < attractorComplexity; i++)
		temp[i] = globals[i];
	temp[i] = v;
	if(attractorComplexity > 0) delete [] globals;
	globals = temp;
	attractorComplexity++;
}

void interface::cullAttractor(int q)
{
	delete globals[q];
	for(int i = q; i < attractorComplexity - 1; i++)
		globals[i] = globals[i+1];
	attractorComplexity--;
}