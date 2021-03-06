/*Copyright Somnambulant Studios 2012*/
#include "interface.h"
#include <SDL/SDL_opengl.h>
#include <algorithm>
#include <assert.h>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include <math.h>

using std::cout;
using std::ifstream;
using std::ofstream;
using std::max;
using std::min;

interface::interface()
{
	numChars = 2;
	stats = new chart(numChars);
	killTimer = false;
	shortcut = true;
	continuous = false;
	analytics = true;
	scripting = false;
	pauseEnabled = false;
	single = false;
	ifstream read;

	/*Initialize some pseudo-constants*/
	screenWidth = 1600; /*screen{Width,Height} describe the size of the window holding the game.*/
	screenHeight = 900;
	screen = nullptr;
	bg.w = 3200; /*The screen gives a partial view of the background, which is the area available for character movement.*/
	bg.h = 1800;
	floor = 50; /*Value of the floor. This is the maximum distance downward that characters can travel.*/
	wall = 50; /*The size of the offset at which characters start to scroll the background, and get stuck.*/
	menuMusic = nullptr;

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
	oldReplay = nullptr;
	replayIterator = 0;
	replay = nullptr;
}

void interface::createPlayers(string rep)
{
	oldReplay = new script(rep);
	createPlayers();
	if(oldReplay->selection.size() != 2){
		printf("Invalid Replay\n");
		delete oldReplay;
		oldReplay = nullptr;
	} else {
		single = true;
		analytics = false;
		for(int i = 0; i < 2; i++){
			selection[i] = oldReplay->selection[i];
			select[i] = 1;
			P[i]->characterSelect(selection[i]);
			if(scripting) P[i]->readScripts();
		}
		loadMatchBackground();
	}
}

void interface::createPlayers()
{
	/*Initialize players.*/
	for(int i = 0; i < 2; i++){
		P.push_back(new player(i+1));
		p.push_back(P[i]);
		select[i] = 0;
		selection.push_back(1+i);
		menu[i] = 0;
		counterHit[i] = 0;
		configMenu[i] = 0;
		things.push_back(P[i]);
		P[i]->boxen = false;
		P[i]->sprite = true;
	}
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
	w = screenWidth*sf; h = screenHeight*sf;
	if(screen){
		SDL_FreeSurface(screen);
		screen = nullptr;
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
	glPushMatrix();
		glScalef(scalingFactor, scalingFactor, 1.0f);
		sprintf(pident, "Player %i", ID + 1);
		p[ID]->input.clear();
		for(unsigned int i = 0; i < p[ID]->inputName.size(); i++){
			glClear(GL_COLOR_BUFFER_BIT);
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
			glRectf(0.0f, 0.0f, (GLfloat)screenWidth, (GLfloat)screenHeight);
			glEnable( GL_TEXTURE_2D );
			glColor4f(0.1f, 0.1f, 0.1f, 1.0f);
			drawGlyph(pident, 0, screenWidth, 300, 80, 1);
			drawGlyph("Please enter a", 0, screenWidth, 400, 80, 1);
			sprintf(buffer, "command for %s", p[ID]->inputName[i].c_str());
			drawGlyph(buffer, 0, screenWidth, 500, 80, 1);
			SDL_GL_SwapBuffers();
			glDisable( GL_TEXTURE_2D );
			glClear(GL_COLOR_BUFFER_BIT);
			p[ID]->setKey(1 << i);
		}
		p[ID]->writeConfig(ID+1);
	glPopMatrix();
}

	/*This functions sets things up for a new match. Initializes some things and draws the background*/
void interface::matchInit()
{
	SDL_Event event;
	rMenu = 0;
	for(player* i:P){
		i->rounds = 0;
		i->secondInstance = 0;
	}
	pMenu = 0;
	if(!select[0] || !select[1]){
		Mix_VolumeMusic(100);
		Mix_PlayMusic(menuMusic, -1);
		//printf("\n");
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

	for(player* i:P){
		i->current.posY = floor;
		i->roundInit();
	}
	/*Initialize input containers*/
	initContainers();
	for(unsigned int i = 0; i < P.size(); i++){
		combo[i] = 0;
		prorate[i] = 1.0;
		damage[i] = 0;
		illegit[i] = 0;
		counterHit[i] = 0;
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
	if(P[0]->rounds == 0 && P[1]->rounds == 0 && timer == 101 * 60){
		Mix_VolumeMusic(100);
		Mix_PlayMusic(matchMusic,-1);
	}
	int plus;
	for(unsigned int i = 0; i < P.size(); i++){
		if(select[i] == true){
			if(things[i]->current.move != nullptr){
				plus = (things[i]->current.move->arbitraryPoll(31, things[i]->current.frame));
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
			things[0]->momentum.clear();
			things[1]->momentum.clear();
			if(P[0]->rounds == numRounds || P[1]->rounds == numRounds){
				if(!oldReplay){
					if(P[0]->rounds == P[1]->rounds);
					else if(P[0]->rounds == numRounds){ 
						P[0]->wins++;
						//printf("P1: %i wins\n", P[0]->wins);
					} else {
						P[1]->wins++;
						//printf("P2: %i wins\n", P[1]->wins);
					}
					if(P[0]->rounds == P[1]->rounds);
					else{
						if(P[0]->rounds == numRounds) stats->recordWin(selection[0], selection[1]);
						else stats->recordWin(selection[1], selection[0]);
					}
					//printf("Matchup: %f\n", stats->matchup(selection[0], selection[1]));
				}
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
					if(analytics && replay){
						replay->write();
						delete replay;
						replay = nullptr;
					}
					for(player *i:P){
						if(i->record){
							char buffer[200];
							sprintf(buffer, "%i-%s.sh", i->ID, i->pick()->name.c_str());
							i->record->write(buffer);
							delete i->record;
							i->record = nullptr;
						}
					}
					if(oldReplay){
						delete oldReplay;
						oldReplay= nullptr;
					}
					if(single) gameover = true;
					else{
						matchInit();
						if(select[0] && select[1]){
							if(analytics){
								replay = new script;
								replay->init(selection);
							}
							roundInit();
						}
					}
				}
			}
			else roundInit();
		}
	} else if(!killTimer || timer > 99 * 60) timer--;
}

void gameInstance::print()
{
	cout << "\x1b[A" << "\x1b[A";
	for(int i = 0; i < 2; i++){
		for(int j = 0; j < 80; j++) cout << " ";
		cout << ('\n');
	}
	for(player* i:P) i->print();
}

/*Main function for a frame. This resolves character spritions, background scrolling, and hitboxes*/
void interface::resolve()
{
	if(!select[0] || !select[1]) cSelectMenu();
	else if(rMenu) rematchMenu();
	else if(pMenu) pauseMenu();
	else {
		resolveInputs();
		for(instance *i:things) i->updateRects();
		resolveThrows();
		doSuperFreeze();
		for(instance *i:things) i->updateRects();
		resolvePhysics();
		resolveCamera();
		resolveCollision();
		resolveCombos();
		resolveHits();
	}
}

void interface::resolveCombos()
{
	for(unsigned int i = 0; i < P.size(); i++){
		if(!roundEnd){
			switch (P[i]->pick()->comboState(things[i]->current.move)){ 
			case -2: 
				illegit[(i+1)%2] = 1;
				counterHit[(i+1)%2] = 0;
				break;
			case 0:
				combo[(i+1)%2] = 0;
				damage[(i+1)%2] = 0;
				prorate[(i+1)%2] = 1.0;
				P[i]->elasticX = 0;
				P[i]->elasticY = 0;
				illegit[(i+1)%2] = 0;
				counterHit[(i+1)%2] = 0;
				break;
			}
		}
	}
}

void interface::resolveCamera()
{
	/*Really rudimentary camera logic. Really just scrolls the background (Which characters are drawn relative to)
	 *appropriately, attempting to adjust to approximately be looking at the point in the middle of the two characters.
	 */
	int dx = things[1]->dragBG(bg.x + wall, bg.x + screenWidth - wall) + things[0]->dragBG(bg.x + wall, bg.x + screenWidth - wall);
	/*If a character leaves the camera boundaries, follow them immediately*/
	if(!dx){
		dx = -(((bg.x + screenWidth/2) - things[0]->current.posX) + ((bg.x + screenWidth/2) - things[1]->current.posX));
		dx /= 10;
		/*Otherwise follow the middle at a rate of (disparity from middle view)/10.
		 *Chosen by trial and error, this rate feels most natural
		 */
	}
	dragBG(dx);
}

void interface::resolveInputs()
{
	if(timer > 99 * 60){
		for(unsigned int i = 0; i < P.size(); i++){
			if(timer == 106 * 60) things[i]->inputBuffer[0] = 0;
			else if(timer == 106 * 60 - 1) things[i]->inputBuffer[0] = i;
			else if(timer == 106 * 60 - 2) things[i]->inputBuffer[0] = selection[(i+1)%2] / 10;
			else if(timer == 106 * 60 - 3) things[i]->inputBuffer[0] = selection[(i+1)%2] % 10;
			else if(timer == 106 * 60 - 4) things[i]->inputBuffer[0] = 0;
			else(things[i]->inputBuffer[0] = 5);
			for(int j:currentFrame[i].buttons) j = 0;
		}
	} else {
		int flop[2] = {0, 0};
		for(unsigned int i = 0; i < currentFrame.size(); i++){
			if(P[i]->current.facing == -1){
				if(currentFrame[i].n.raw.dir % 3 == 0) flop[i] -= 2;
				else if(currentFrame[i].n.raw.dir % 3 == 1) flop[i] += 2; 
			}
		}
		for(unsigned int i = 0; i < things.size(); i++)
			things[i]->pushInput(currentFrame[things[i]->ID - 1].n.raw.dir + flop[things[i]->ID -1]);
		for(unsigned int i = 0; i < P.size(); i++){
			bool test = 1;
			P[i]->getMove(currentFrame[i].buttons, prox, test);
			if(!test && !P[i]->current.aerial){ 
				P[i]->checkFacing(P[(i+1)%2]);
			}
		}
		for(unsigned int i = 0; i < things.size(); i++){
			if(i < P.size()){
				if(things[(i+1)%2]->current.aerial) prox.y = 1;
				else prox.y = 0;
				prox.x = things[(i+1)%2]->current.throwInvuln;
			}
			bool d = 0;
			things[i]->getMove(currentFrame[things[i]->ID - 1].buttons, prox, d);
		}
	}
	for(unsigned int i = 0; i < P.size(); i++){
		if(analytics)
			replay->push(i, currentFrame[i]);
		if(P[i]->record) 
			P[i]->record->push(currentFrame[i]);
	}
}

void interface::resolvePhysics()
{
	for(unsigned int i = 0; i < things.size(); i++){
		if(!things[i]->current.freeze){
			if(things[i]->current.move->stop & 4);
			else { 
				things[i]->pullVolition();
				if(things[i]->ID) things[i]->follow(things[(things[i]->ID)%2]);
				things[i]->combineDelta();
				things[i]->enforceGravity(grav, floor);
			}
			for(unsigned int j = 0; j < globals.size(); j++){
				if(globals[j]->ID != things[i]->ID){
					if(i < P.size()){
						if(globals[j]->effectCode & 1){
							things[i]->enforceAttractor(globals[j]);
						}
					} else {
						if(globals[j]->effectCode & 2){
							things[i]->enforceAttractor(globals[j]);
						}
					}
				}
			}
		}
	}
}

void interface::cleanup()
{
	//if(select[0] && select[1]) print();
	if(!pMenu){
		for(unsigned int i = 0; i < P.size(); i++) {
			things[i]->current.throwInvuln--;
			P[i]->hover--;
		}
		for(instance *i:things){
			if(i->current.posX > bg.w + 300 || i->current.posX < -300 || i->current.posY < -300 || i->current.posY > bg.h){
				i->current.dead = true;
			}
		}
		if(!rMenu && select[0] && select[1]){
			for(unsigned int i = 0; i < things.size(); i++){
				things[i]->step();
				if(i > 1 && things[i]->current.dead){ 
					things.erase(things.begin()+i);
					i--;
				}
			}
			for(unsigned int i = 0; i < globals.size(); i++){
				if(globals[i]->length <= 0)
					globals.erase(globals.begin()+i);
				else globals[i]->length--;
			}
			resolveSummons();
			summonAttractors();
			if(!roundEnd) checkWin();
			runTimer();
		}
	}
	for(unsigned int i = 0; i < P.size(); i++){
		if(currentFrame[i].n.raw.Start && counter[i] <= 0){
			if(pauseEnabled && !roundEnd){
				if(pMenu) pMenu = 0;
				else pMenu = 1;
			}
		}
	}
	for(unsigned int i = 0; i < P.size(); i++){
		for(unsigned int j = 0; j < currentFrame[i].buttons.size(); j++){
			if(currentFrame[i].buttons[j] != 0) currentFrame[i].buttons[j]++;
		}
		currentFrame[i].n.raw.Start = 0;
		currentFrame[i].buttons[5] = 0;
		if(counter[i] > 0) counter[i]--;
	}
}

void interface::resolveSummons()
{
	action * temp;
	instance * larva;
	int x, y, f;
	for(unsigned int i = 0; i < things.size(); i++){
		if(things[i]->current.move){
			temp = things[i]->current.move;
			if(temp->arbitraryPoll(50, things[i]->current.frame)){
				larva = things[i]->pick()->spawn(temp);
				if(larva){
					switch (temp->arbitraryPoll(56, things[i]->current.frame)){
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
					if(temp->arbitraryPoll(51, things[i]->current.frame)){
						x = things[(things[i]->ID)%2]->current.posX;
						f = things[(things[i]->ID)%2]->current.facing;
					} else {
						x = things[(things[i]->ID)-1]->current.posX;
						f = things[(things[i]->ID)-1]->current.facing;
					}
					if(temp->arbitraryPoll(52, things[i]->current.frame))
						y = things[(things[i]->ID)%2]->current.posY;
					else if(temp->arbitraryPoll(53, things[i]->current.frame))
						y = 0;
					else
						y = things[(things[i]->ID)-1]->current.posY;
					x += temp->arbitraryPoll(54, things[i]->current.frame)*f;
					y += temp->arbitraryPoll(55, things[i]->current.frame);
					if(x > bg.w + 100) x = bg.w + 100;
					if(y > bg.h - 50) y = bg.h - 50;
					if(x < -100) x = -100;
					larva->current.facing = f;
					larva->setPosition(x, y);
					things.push_back(larva);
					larva->init();
				}
			}
		}
	}
}

void interface::summonAttractors()
{
	attractor * tvec = nullptr, * avec = nullptr;
	for(unsigned int i = 0; i < things.size(); i++){
		if(things[i]->current.move && things[i]->current.frame == things[i]->current.move->distortSpawn) tvec = things[i]->current.move->distortion;
		if(tvec != nullptr){ 
			avec = new attractor;
			avec->x = tvec->x;
			avec->y = tvec->y;
			avec->type = tvec->type;
			avec->length = tvec->length;
			avec->radius = tvec->radius;
			avec->effectCode = tvec->effectCode;
			avec->eventHorizon = tvec->eventHorizon;
			if(things[i]->current.facing == 1) avec->posX = things[i]->collision.x + things[i]->collision.w / 2;
			else avec->posX = things[i]->collision.x + things[i]->collision.w / 2 + things[i]->collision.w % 2;
			avec->posY = things[i]->collision.y + things[i]->collision.h/2;
			if(avec->type == 0) avec->x *= things[i]->current.facing;
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
			avec = nullptr;
			tvec = nullptr;
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
	if(oldReplay){
		for(unsigned int i = 0; i < p.size(); i++)
			oldReplay->genEvent(i, replayIterator, currentFrame[i]);
		replayIterator++;
		if(replayIterator > oldReplay->command[0].size()){
			delete oldReplay;
			oldReplay = nullptr;
			replayIterator = 0;
		}
	} else {
		if(!pauseEnabled){
			for(unsigned int i = 0; i < P.size(); i++){
				if(P[i]->currentMacro){
					if(!P[i]->currentMacro->genEvent(0, P[i]->iterator, currentFrame[i])){
						P[i]->currentMacro = nullptr;
					} else {
						if(P[i]->current.facing == -1){
							bool s = currentFrame[i].axis[2];
							currentFrame[i].axis[2] = currentFrame[i].axis[3];
							currentFrame[i].axis[3] = s;
						}
						P[i]->iterator++;
					}
				}
			}
		}
	}
}

/*Read the input that's happened this frame*/
void interface::processInput(SDL_Event &event)
{
	/*Do stuff with event*/
	for(unsigned int i = 0; i < p.size(); i++){
		int t = p[i]->tap(event);
		if(t == 0) t = p[(i+1)%2]->tap(event);
		if((t < 1 || t > 8) && (t < 512) && event.type != SDL_JOYHATMOTION){
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

void interface::readInput()
{
	vector<SDL_Event> events;
	SDL_Event event;
	while(SDL_PollEvent(&event)){
		events.push_back(event);
		processInput(event);
	}
	if(select[0] && select[1]){
		if(scripting){
			for(player* i:P){
				for(unsigned int j = 0; j < events.size(); j++){
					if(!oldReplay){
						bool r = false;
						if(i->record) r = true;
						i->macroCheck(events[j]);
						if(i->record){
							if(!r) j = events.size();
							i->search = false;
						}
					}
				}
				if(i->search){
					for(unsigned int j = 0; j < events.size(); j++){
						if(i->currentMacro = i->patternMatch(abs(i->tap(events[j])))){
							j = events.size();
							i->search = false;
							i->iterator = 0;
						}
					}
				}
			}
		}
		for(frame i:currentFrame){
			for(int j:i.axis) j = 0;
			i.n.i = 0;
		}
	}
	if(scripting || oldReplay) genInput();
	for(SDL_Event i:events){
		for(unsigned int j = 0; j < p.size(); j++){
			if(!p[j]->currentMacro) P[j]->readEvent(i, currentFrame[j]);
		}
	}
}

void gameInstance::processInput(SDL_Event &event)
{
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
		ofstream write;
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
				if(currentFrame[i].axis[2] && !select[i] && counter[i] == 0){
					selection[i]--;
					if(selection[i] < 1) selection[i] = numChars;
					counter[i] = 10;
				}
				if(currentFrame[i].axis[3] && !select[i] && counter[i] == 0){
					selection[i]++;
					if(selection[i] > numChars) selection[i] = 1;
					counter[i] = 10;
				}
				for(int j = 0; j < 5; j++){
					if(currentFrame[i].buttons[j] == 1 && !select[i]){
						select[i] = 1;
					}
				}
				if(currentFrame[i].n.raw.Start){
					if(!select[i]) menu[i] = 3;
					else select[i] = 0;
					counter[i] = 10;
				}
			}
		}
	}

	for(unsigned int i = 0; i < P.size(); i++){
		if(configMenu[i] > 0) keyConfig(i);
		else if(menu[i] > 0) mainMenu(i);
	}

	if(select[0] && select[1]){
		//cout << "2 6\n" << selection[0] << " " << selection[1] << '\n';
		for(unsigned int i = 0; i < P.size(); i++){
			P[i]->characterSelect(selection[i]);
		}
		loadAssets();
		if(analytics){
			replay = new script;
			replay->init(selection);
		}

		Mix_HaltMusic();

		roundInit();
	}
}

void interface::loadAssets()
{
	unsigned int b = SDL_WasInit(SDL_INIT_VIDEO);
	if(b != 0){
		for(player *i:P) i->loadAssets();
		loadMatchBackground();
	}
}

void interface::mainMenu(int ID)
{
	if(currentFrame[ID].axis[0] && !counter[ID]){
		menu[ID]--;
		counter[ID] = 10;
	} else if(currentFrame[ID].axis[1] && !counter[ID]){
		menu[ID]++;
		counter[ID] = 10;
	}
	if(menu[ID] > 8) menu[ID] = 1;
	else if(menu[ID] < 1) menu[ID] = 8;
	for(unsigned int i = 0; i < currentFrame[ID].buttons.size()-1; i++){
		if(currentFrame[ID].buttons[i] == 1 && !counter[ID]){
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
				if(scripting)
					scripting = false;
				else{
					pauseEnabled = false;
					scripting = true;
				}
				break;
			case 6:
				if(pauseEnabled)
					pauseEnabled = false;
				else{
					scripting = false;
					pauseEnabled = true;
				}
				break;
			case 7:
				if(P[ID]->boxen && P[ID]->sprite) P[ID]->sprite = false;
				else if(P[ID]->boxen){
					P[ID]->sprite = true;
					P[ID]->boxen = false;
				} else P[ID]->boxen = true;
				break;
			case 8:
				gameover = 1;
				break;
			}
			counter[ID] = 10;
		}
	}
	if(currentFrame[ID].n.raw.Start && !counter[ID]){ 
		counter[ID] = 10;
		menu[ID] = 0;
	}
}

void interface::keyConfig(int ID)
{
	if(currentFrame[ID].axis[0] && !counter[ID]){
		configMenu[ID]--;
		counter[ID] = 10;
	} else if(currentFrame[ID].axis[1] && !counter[ID]){
		configMenu[ID]++;
		counter[ID] = 10;
	}
	if(configMenu[ID] > 7) configMenu[ID] = 1;
	else if(configMenu[ID] < 1) configMenu[ID] = 7;
	for(unsigned int i = 0; i < currentFrame[ID].buttons.size()-1; i++){
		if(currentFrame[ID].buttons[i] == 1 && !counter[ID]){
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
	if(currentFrame[ID].n.raw.Start == 1 && !counter[ID]){ 
		counter[ID] = 10;
		configMenu[ID] = 0;
		menu[ID] = 0;
		p[ID]->writeConfig(ID+1);
	}
}

void interface::dragBG(int dx)
{
	int dy = 900;
	bg.x += dx;
	if(bg.x < 0) bg.x = 0;
	else if(bg.x > bg.w - screenWidth) bg.x = bg.w - screenWidth;
	for(unsigned int i = 0; i < P.size(); i++){
		if(dy < things[i]->current.posY + things[i]->collision.h){
			dy = things[i]->current.posY + things[i]->collision.h;
			if(dy > bg.h) dy = bg.h;
		}
	}
	bg.y = dy - bg.h;
}

void interface::pauseMenu()
{
	for(unsigned int j = 0; j < p.size(); j++){
		if(currentFrame[j].axis[0] && !counter[j]){
			pMenu--;
			counter[j] = 10;
		} else if(currentFrame[j].axis[1] && !counter[j]){ 
			pMenu++;
			counter[j] = 10;
		}
		if(pMenu > 3) pMenu = 1;
		else if(pMenu < 1) pMenu = 3;
		for(unsigned int i = 0; i < currentFrame[j].buttons.size()-1; i++){
			if(currentFrame[j].buttons[i] == 1){
				switch(pMenu){
				case 1:
					pMenu = 0;
					break;
				case 2:
					for(unsigned int i = 0; i < P.size(); i++){
						delete P[i]->pick();
						select[i] = 0;
						things[i]->meter.clear();
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
				break;
			}
		}
		if(currentFrame[j].n.raw.Start && !counter[j]) pMenu = 0;
	}
}

void interface::rematchMenu()
{
	for(unsigned int j = 0; j < P.size(); j++){
		if(currentFrame[j].axis[0] && !counter[j]){
			rMenu--;
			counter[j] = 10;
		} else if(currentFrame[j].axis[1] && !counter[j]){ 
			rMenu++;
			counter[j] = 10;
		}
		if(rMenu > 3) rMenu = 1;
		else if(rMenu < 1) rMenu = 3;
		for(unsigned int i = 0; i < currentFrame[j].buttons.size(); i++){
			if(currentFrame[j].buttons[i] == 1){
				switch(rMenu){
				case 1:
					Mix_HaltMusic();
					rMenu = 0;
					matchInit();
					break;
				case 2:
					for(unsigned int k = 0; k < P.size(); k++){
						delete P[k]->pick();
						select[k] = 0;
						things[k]->meter.clear();
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
				j = P.size();
				break;
			}
		}
	}
}

interface::~interface()
{
	if(select[0]) delete P[0]->pick();
	if(select[1]) delete P[1]->pick();
	if(menuMusic != nullptr) Mix_FreeMusic(menuMusic);
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
		if(a->current.facing == 1 && b->current.facing == -1){ left = a; right = b; }
		else if(b->current.facing == 1 && a->current.facing == -1){ left = b; right = a; }
	}

	/*Collision between players. Unfortunately a lot of specialcasing necessary here.*/

	int rLOffset = right->current.posX - right->collision.x;
	int rROffset = right->current.posX - (right->collision.x + right->collision.w);
	int lLOffset = left->current.posX - left->collision.x;
	int lROffset = left->current.posX - (left->collision.x + left->collision.w);
	int dOffset = (left->current.deltaX - right->current.deltaX) % 2;
	int totalMiddle = (right->collision.x + left->collision.x + left->collision.w)/2;
	if(abs(left->current.deltaX) > abs(right->current.deltaX)) totalMiddle += dOffset;

	if(left->current.lCorner){ 
		right->current.posX = left->collision.x + left->collision.w + rLOffset;
	}
	else if(right->current.rCorner) left->current.posX = right->collision.x + lROffset;
	else {
		right->current.posX = totalMiddle + right->collision.w + rROffset;
		left->current.posX = totalMiddle - left->collision.w + lLOffset;
	}
	if(left->collision.x < 50) {
		left->updateRects();
		right->current.posX = left->collision.x + left->collision.w + rLOffset;
	} else if (right->collision.x + right->collision.w > 3150) {
		right->updateRects();
		left->current.posX = right->collision.x + lROffset;
	}
	right->updateRects();
	left->updateRects();
}

void interface::resolveCollision()
{
	vector<SDL_Rect> temp;
	vector<int> dx;
	for(player *i:P){
		i->enforceFloor(floor);
		temp.push_back(i->collision);
		dx.push_back(i->current.deltaX);
		temp.back().x -= dx.back();
	}

	unsigned int localMaximum = min(temp[0].w, temp[1].w) / 5 - 1;
	unsigned int j[2] = {0, 0};
	while(j[0] < abs(dx[0]) || j[1] < abs(dx[1])){
		if(aux::checkCollision(temp[0], temp[1])){
			unsigned int k[2] = {j[0], j[1]};
			while(dx[0] || dx[1]){
				for(unsigned int i = 0; i < P.size(); i++){
					if(dx[i]){
						P[i]->current.posX += temp[i].x - P[i]->collision.x;
						P[i]->updateRects();
						dx[i] -= (dx[i] > 0) ? j[i] : -j[i];
						j[i] = 0;
					}
				}
				for(player *i:P) i->checkCorners(bg.x + wall, bg.x + screenWidth - wall);
				unitCollision(P[0], P[1]);
				for(unsigned int i = 0; i < P.size(); i++){
					if(localMaximum < abs(dx[i]) - j[i]){
						j[i] += localMaximum;
						temp[i].x += (dx[i] > 0) ? localMaximum : -localMaximum;
					} else {
						j[i] = abs(dx[i]);
						temp[i].x += (dx[i] > 0) ? (j[i] - k[i]) : -(j[i] - k[i]);
					}
				}
			}
		} else {
			for(unsigned int i = 0; i < P.size(); i++){
				if(j[i] < abs(dx[i])) {
					if(dx[i] < 0){
						temp[i].x--;
					} else if(dx[i] > 0){
						temp[i].x++;
					}
					j[i]++;
				}
			}
		}
	}

	prox.w = abs(things[0]->current.posX - things[1]->current.posX);
	prox.h = abs(things[0]->current.posY - things[1]->current.posY);

	for(unsigned int i = 0; i < things.size(); i++){
		if(things[i]->current.move->track){
			things[i]->checkFacing(P[(things[i]->ID)%2]);
		}
		if(i < 2 && !things[i]->current.aerial) {
			things[i]->current.deltaX = 0; 
			things[i]->current.deltaY = 0; 
		}
	}

	for(player *i:P){
		i->enforceFloor(floor);
		i->checkCorners(bg.x + wall, bg.x + screenWidth - wall);
	}

	//Some issues arise if you don't have this second pass
	if (aux::checkCollision(P[0]->collision, P[1]->collision))
		unitCollision(P[0], P[1]);
	prox.w = abs(things[0]->current.posX - things[1]->current.posX);
	prox.h = abs(things[0]->current.posY - things[1]->current.posY);
}

void interface::resolveThrows()
{
	bool isThrown[2] = {false, false};
	for(unsigned int i = 0; i < P.size(); i++){
		if(things[i]->current.move->arbitraryPoll(28, things[i]->current.frame)){ 
			isThrown[(i+1)%2] = true;
		}
	}
	if(isThrown[0] && isThrown[1]){
		things[0]->current.move = P[0]->pick()->throwBreak;
		things[1]->current.move = P[1]->pick()->throwBreak;
	} else {
		for(unsigned int i = 0; i < P.size(); i++){
			if(isThrown[i]){
				P[i]->getThrown(things[(i+1)%2]->current.move, things[(i+1)%2]->current.posX*things[(i+1)%2]->current.facing, things[(i+1)%2]->current.posY);
				P[i]->checkFacing(P[(i+1)%2]);
			}
		}
	}
}

void interface::resolveHits()
{
	vector<hStat> s(things.size());
	vector<int> hit(things.size());
	vector<bool> connect(things.size());
	vector<bool> taken(things.size());
	vector<int> hitBy(things.size());
	int push[2];
	for(player *i:P) i->checkCorners(bg.x + wall, bg.x + screenWidth - wall);
	for(unsigned int i = 0; i < things.size(); i++){
		taken[i] = 0;
		hit[i] = 0;
		connect[i] = 0;
		hitBy[i] = -1;
	}
	for(unsigned int i = 0; i < things.size(); i++){
		if(!things[i]->hitbox.empty()){
			if(!freeze) P[(things[i]->ID)%2]->checkBlocking();
		}
	}
	for(unsigned int i = 0; i < things.size(); i++){
		for(int m = things.size()-1; m >= 0; m--){
			for(unsigned int j = 0; j < things[i]->hitbox.size(); j++){
				for(unsigned int k = 0; k < things[m]->hitreg.size(); k++){
					if(aux::checkCollision(things[i]->hitbox[j], things[m]->hitreg[k])){
						if(things[i]->acceptTarget(things[m])){
							if(m != (int)i && !taken[m] && !connect[i]){
								connect[i] = 1;
								things[i]->current.counter = things[m]->CHState();
								if(things[i]->current.counter > 0) counterHit[things[i]->ID-1] = s[i].stun + (s[i].pause > 0) ? s[i].pause : (s[i].stun/4 + 10);
								things[i]->pollStats(s[i]);
								if(i < P.size()) push[i] = s[i].push;
								k = things[m]->hitreg.size();
								j = things[i]->hitbox.size();
								taken[m] = 1;
								hitBy[m] = i;
								break;
							}
						}
					}
				}
			}
		}
	}


	for(unsigned int i = 0; i < things.size(); i++){ 
		if(taken[i]){
			int health = things[things[i]->ID-1]->meter[0];
			bool actuallyDoesDamage = (s[hitBy[i]].damage != 0);
			s[hitBy[i]].damage *= prorate[things[hitBy[i]]->ID-1];
			if(actuallyDoesDamage && s[hitBy[i]].damage == 0) s[hitBy[i]].damage = 1;
			hit[hitBy[i]] = things[i]->takeHit(combo[things[hitBy[i]]->ID-1], s[hitBy[i]], prox);
			if(i < P.size()){
				if(things[i]->particleType == -2){
					hStat ths;
					ths.damage = s[hitBy[i]].chip ? s[hitBy[i]].chip : 1;
					ths.ghostHit = true;
					ths.stun = 0;
					ths.push = s[hitBy[i]].push;
					if(things[i]->current.aerial){
						if(P[things[hitBy[i]]->ID-1]->current.aerial) ths.push += s[hitBy[i]].blowback*5;
						else ths.push += s[hitBy[i]].blowback;
					}
					P[things[hitBy[i]]->ID-1]->takeHit(combo[i], ths, prox);
				}
				if(s[hitBy[i]].stun) combo[(i+1)%2] += hit[hitBy[i]];
			}
			P[(i+1)%2]->enforceFloor(floor);
			P[(i+1)%2]->checkCorners(bg.x + wall, bg.x + screenWidth - wall);
			if(things[i]->current.facing * things[hitBy[i]]->current.facing == 1) things[i]->invertVectors(1);
			if(i < P.size()) damage[(i+1)%2] += health - P[i]->meter[0];
		}
	}

	for(unsigned int i = 0; i < things.size(); i++){
		if(connect[i]){
			things[i]->connect(combo[things[i]->ID-1], s[i]);
			if(hit[i] == 1){
				things[i]->current.hit = things[i]->current.connect;
				prorate[things[i]->ID-1] *= s[i].prorate;
			}
			if(!things[i]->current.aerial){
				for(int j = 0; j < 6; j++){
					if(2 << j & things[i]->current.move->state[things[i]->current.hit].i){
						P[i]->checkFacing(P[(things[i]->ID)%2]);
						break;
					}
				}
			}
		}
	}

	SDL_Rect residual = {0, 0, 1, 0};
	for(unsigned int i = 0; i < P.size(); i++){ 
		if(connect[i]){
			if(P[i]->current.aerial){ 
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
				if(things[(i+1)%2]->current.aerial){
					if(P[(i+1)%2]->current.rCorner || P[(i+1)%2]->current.lCorner) residual.x -= combo[i];
					if(P[(i+1)%2]->stick) residual.x -= s[i].push/2 + combo[i];
					residual.x -= 2;
				}
				else {
					if(combo[i] > 1) residual.x = -3*(abs(combo[i]-1));
					if(things[(i+1)%2]->particleType == -2) residual.x -= push[i];
					else if(P[(i+1)%2]->current.rCorner || P[(i+1)%2]->current.lCorner){
						residual.x -= 2;
						residual.x -= s[i].push/2;
						residual.x -= abs(combo[i]);
					}
				}
				residual.x *= things[i]->current.facing;
			}
			things[i]->momentum.push_back(residual);
		}
	}
	if(connect[0] || connect[1]){
		resolveCollision();
	}

	for(unsigned int i = 0; i < P.size(); i++) {
		if(things[i]->meter[0] <= 0 && endTimer >= 5 * 60){ 
			i = 2;
			for(unsigned int j = 0; j < things.size(); j++)
				things[j]->current.freeze = 30;
		}
	}
}

void interface::doSuperFreeze()
{
	int go[2] = {0, 0};
	for(unsigned int i = 0; i < P.size(); i++){
		if(!things[i]->current.move->arbitraryPoll(33, 0) || freeze <= 0)
			go[i] = things[i]->current.move->arbitraryPoll(2, things[i]->current.frame);
		if(go[i] > 0){ 
			P[(i+1)%2]->checkBlocking();
			things[(i+1)%2]->current.freeze += go[i];
			if(things[i]->current.move->arbitraryPoll(32, 0)){
				for(unsigned int j = 2; j < things.size(); j++) things[j]->current.freeze += go[i];
			}
		}
	}
	if(go[0] > 0 || go[1] > 0)
		freeze = max(go[0], go[1]);
}

