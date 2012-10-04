/*Drawing routines for project: Ground Up Fighting Game
 *
 *Written by Alex Kelly in 2012, under MIT OSI
 *For detailed license information, see the file COPYING in this directory
 */

#include "interface.h"
#include <math.h>
#include <iomanip>
#include <sstream>
#include <string>
#include <SDL/SDL_opengl.h>
void interface::draw()
{

	char buffer[200];
	glClear(GL_COLOR_BUFFER_BIT);

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable( GL_TEXTURE_2D );
	glBindTexture(GL_TEXTURE_2D, background);
	glBegin(GL_QUADS);
		glTexCoord2i(0, 0);
		glVertex3f((GLfloat)(-bg.x)*scalingFactor, (GLfloat)(bg.y)*scalingFactor, 0.f);

		glTexCoord2i(1, 0);
		glVertex3f((GLfloat)(bg.w - bg.x)*scalingFactor, (GLfloat)(bg.y)*scalingFactor, 0.f);

		glTexCoord2i(1, 1);
		glVertex3f((GLfloat)(bg.w - bg.x)*scalingFactor, (GLfloat)(bg.h + bg.y)*scalingFactor, 0.f);

		glTexCoord2i(0, 1);
		glVertex3f((GLfloat)(-bg.x)*scalingFactor, (GLfloat)(bg.h + bg.y)*scalingFactor, 0.f);
	glEnd();

	if(timer / 60 > 99) sprintf(buffer, "99");
	else if(timer / 60 < 10) sprintf(buffer, "0%i", timer / 60);
	else sprintf(buffer, "%i", timer / 60);

	drawGlyph(buffer, 700, 200, 0, 90, 1);
	for(int i = 0; i < 2; i++){
		drawGlyph(p[i]->pick()->name, 100+800*i, 600, 30, 40, 0+2*i);
		if(combo[i] > 1){
			glColor4f(1.0, 1.0-.5*illegit[i], 1.0-.5*illegit[i], 1.0);
			sprintf(buffer, "%i hits", combo[i]);
			drawGlyph(buffer, 100+800*i, 600, 400, 75, 0+2*i);
			sprintf(buffer, "%i damage", damage[i]);
			drawGlyph(buffer, 100+800*i, 600, 475, 35, 0+2*i);
			glColor4f(1.0, 1.0, 1.0, 1.0);
		}
	}

	if(timer > 100 * 60 && timer < 100 * 60 + 31){ 
		int l = p[0]->rounds + p[1]->rounds + 1;
		sprintf(buffer, "Round %i", l);
		if(timer == 100 * 60 + 30)
			Mix_PlayChannel(3, announceRound[l - 1], 0);
		drawGlyph(buffer, 0, 1600, 375, 150, 1);
	}
	if(timer > 99 * 60 && timer < 99 * 60 + 31){ 
		drawGlyph("FIGHT", 0, 1600, 375, 150, 1);
		if(timer == 99 * 60 + 30)
			Mix_PlayChannel(3, announceFight, 0);
	}

	if(roundEnd && endTimer > 5 * 60 - 31){ 
		if(p[0]->pick()->meter[0] > 0 && p[1]->pick()->meter[0] > 0){
			drawGlyph("TIME OUT", 0, 1600, 300, 200, 1);
			if(endTimer == 5 * 60 - 1)
				Mix_PlayChannel(3, announceEnd[0], 0);
		} else {
			drawGlyph("DOWN", 0, 1600, 375, 150, 1);
			if(endTimer == 5 * 60 - 1)
				Mix_PlayChannel(3, announceEnd[1], 0);
		}
	}
	if(endTimer > 3 * 60 + 29 && endTimer < 4 * 60){ 
		if(p[0]->pick()->meter[0] > p[1]->pick()->meter[0]){ 
			sprintf(buffer, "%s", p[0]->pick()->name);
			drawGlyph(buffer, 0, 1600, 300, 150, 1);
			drawGlyph("Wins", 0, 1600, 450, 150, 1);
			if(endTimer == 4 * 60 - 1)
				Mix_PlayChannel(3, announceWinner[selection[0]], 0);
		} else if(p[1]->pick()->meter[0] > p[0]->pick()->meter[0]){
			sprintf(buffer, "%s", p[1]->pick()->name);
			drawGlyph(buffer, 0, 1600, 300, 150, 1);
			drawGlyph("Wins", 0, 1600, 450, 150, 1);
			if(endTimer == 4 * 60 - 1)
				Mix_PlayChannel(3, announceWinner[selection[1]], 0);
		} else if(p[0]->pick()->meter[0] <= 0){ 
			sprintf(buffer, "Double KO");
			drawGlyph(buffer, 0, 1600, 375, 150, 1);
			if(endTimer == 4 * 60 - 1)
				Mix_PlayChannel(3, announceDraw[0], 0);
		} else {
			sprintf(buffer, "Draw");
			drawGlyph(buffer, 0, 1600, 375, 150, 1);
			if(endTimer == 4 * 60 - 1)
				Mix_PlayChannel(3, announceDraw[1], 0);
		}
	}
	glDisable( GL_TEXTURE_2D );
	for(int i = 0; i < 2; i++){
		p[i]->drawMeters(numRounds, scalingFactor);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	}

	glEnable( GL_TEXTURE_2D );
	for(int i = 0; i < thingComplexity; i++){
		if(things[i]->spriteCheck()) 
			things[i]->draw(bg.x, bg.y, scalingFactor);
		glDisable(GL_TEXTURE_2D);
		if(!things[i]->spriteCheck() || boxen)
			things[i]->drawBoxen(bg.x, bg.y, scalingFactor);
		if(i < 2)
			p[i]->drawHitParticle(bg.x, bg.y, scalingFactor);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glEnable( GL_TEXTURE_2D );
	}
	glDisable( GL_TEXTURE_2D );
	if(freeze > 0){
		glColor4f(0.0f, 0.0f, 0.0f, 0.4f);
		glRectf(0.0f, 0.0f, (GLfloat)screenWidth*scalingFactor, (GLfloat)screenHeight*scalingFactor);
		freeze--;
	}
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	if(rMenu != 0) reMenu();
	SDL_GL_SwapBuffers();
}

void player::drawMeters(int n, float scalingFactor)
{
	SDL_Rect r[n];
	for(int i = 0; i < n; i++){
		r[i].y = 24; r[i].w = 20; r[i].h = 10;
		if(ID == 1) r[i].x = 680 - 24 * i; 
		else r[i].x = 900 + 24 * i;
	}
	for(int i = 0; i < n; i++){
		if(rounds > i) glColor4f(0.0f, 1.0f, 1.0f, 1.0f);
		else glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
		glRectf((GLfloat)(r[i].x)*scalingFactor, (GLfloat)(r[i].y)*scalingFactor, (GLfloat)(r[i].x + r[i].w)*scalingFactor, (GLfloat)(r[i].y + r[i].h)*scalingFactor);
	}
	glFlush();
	int h = 0;
	if(cMove->hidesMeter) 
		h = cMove->cost;
	pick()->drawMeters(ID, scalingFactor, h);
	glFlush();
}

void character::drawMeters(int ID, float scalingFactor, int hidden)
{
	SDL_Rect m;
	SDL_Rect h;
	if(meter[0] >= 0) h.w = meter[0]; else h.w = 1; 

	if(ID == 1) h.x = 100 + (600 - h.w); 
	else h.x = 900;
	h.h = 10;
	h.y = 10;

	int R = 0, G = 255, B = 0;
	if(meter[1] >= 0) m.w = (meter[1]+hidden)*2; else m.w = 0; 
	if(ID == 1) m.x = 100;
	else m.x = 900 + (600 - m.w);
	m.h = 10; m.y = 860;

	if(m.w < 300) R = 191;
	else if(m.w < 600) B = 255;
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
	glRectf((GLfloat)(h.x)*scalingFactor, (GLfloat)(h.y)*scalingFactor, (GLfloat)(h.x + h.w)*scalingFactor, (GLfloat)(h.y + h.h)*scalingFactor);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glColor4f((float)R, (float)G, (float)B, 1.0f);
	glRectf((GLfloat)(m.x)*scalingFactor, (GLfloat)(m.y)*scalingFactor, (GLfloat)(m.x + m.w)*scalingFactor, (GLfloat)(m.y + m.h)*scalingFactor);
}

void instance::drawBoxen(int x, int y, float scalingFactor)
{
	glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
	glRectf((GLfloat)(collision.x - x)*scalingFactor, (GLfloat)(-collision.y - y)*scalingFactor, (GLfloat)(collision.x + collision.w - x)*scalingFactor, (GLfloat)(-collision.y - collision.h - y)*scalingFactor);
	for(int i = 0; i < regComplexity; i++){
		glFlush();
		glColor4f(0.0f, 1.0f, (GLfloat)(ID - 1.0f)/2.0f, 0.5f);
		glRectf((GLfloat)(hitreg[i].x - x)*scalingFactor, (GLfloat)(-hitreg[i].y - y)*scalingFactor, (GLfloat)(hitreg[i].x + hitreg[i].w - x)*scalingFactor, (GLfloat)(-hitreg[i].y - hitreg[i].h - y)*scalingFactor);
	}
	for(int i = 0; i < hitComplexity; i++){
		glFlush();
		glColor4f(1.0f, 0.0f, (GLfloat)(ID - 1.0f)/2.0f, 0.5f);
		glRectf((GLfloat)(hitbox[i].x - x)*scalingFactor, (GLfloat)(-hitbox[i].y - y)*scalingFactor, (GLfloat)(hitbox[i].x + hitbox[i].w - x)*scalingFactor, (GLfloat)(-hitbox[i].y - hitbox[i].h - y)*scalingFactor);
	}
	glFlush();
	glDisable( GL_TEXTURE_2D );
}

void instance::draw(int x, int y, float scalingFactor)
{
	int realPosY = collision.y;
	int realPosX = posX;

	for(int i = 0; i < hitComplexity; i++){
		if(hitbox[i].y < realPosY) realPosY = hitbox[i].y;
		if(facing == 1){
			if(hitbox[i].x < realPosX) realPosX = hitbox[i].x;
		} else {
			if(hitbox[i].x + hitbox[i].w > realPosX) realPosX = hitbox[i].x + hitbox[i].w;
		}
	}
	for(int i = 0; i < regComplexity; i++){
		if(hitreg[i].y < realPosY) realPosY = hitreg[i].y;
		if(facing == 1){
			if(hitreg[i].x < realPosX) realPosX = hitreg[i].x;
		} else {
			if(hitreg[i].x + hitreg[i].w > realPosX) realPosX = hitreg[i].x + hitreg[i].w;
		}
	}
	if(secondInstance)
		glColor4f(0.75f, 0.5f, 0.85f, 1.0f);
	pick()->draw(cMove, facing, realPosX - x, -realPosY - y, currentFrame, scalingFactor);
}

void player::drawHitParticle(int x, int y, float scalingFactor)
{
	/*Stand-in for now, just to indicate block type*/
	if(particleLife > 0){
		switch (particleType){
		case 1:
			glColor4f(1.0f, 0.0f, 0.0f, 0.7f);
			break;
		case 0:
			glColor4f(0.0f, 0.0f, 0.0f, 0.7f);
			break;
		case -1:
			glColor4f(1.0f, 1.0f, 1.0f, 0.7f);
			break;
		case -2:
			glColor4f(1.0f, 1.0f, 0.0f, 0.7f);
			break;
		}
		glRectf((GLfloat)(posX - 10*facing - x)*scalingFactor, (GLfloat)(-collision.y - collision.h - y)*scalingFactor, (GLfloat)(posX - 50 * facing - x)*scalingFactor, (GLfloat)(-collision.y - collision.h - 40 - y)*scalingFactor);
		particleLife--;
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	} else blockType = 0;
}

void avatar::draw(action *& cMove, int facing, int x, int y, int f, float scalingFactor)
{
	cMove->draw(facing, x, y, f, scalingFactor);
}

int interface::drawGlyph(const char * string, int x, int space, int y, int height, int just)
{
	int w, h, width = 0, padding = 0, totalWidth = 0;
	if(just != 0){	
		for(unsigned int i = 0; i < strlen(string); i++){
			if(string[i] == ' ') totalWidth += w * sf / 2;
			else if(string[i] == '\0');
			else{
				glBindTexture(GL_TEXTURE_2D, glyph[toupper(string[i])]);

				glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
				glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);
				sf = (float)height / (float)h;
				totalWidth += w * sf;
			}
		}
		if(just == 2) padding = space - totalWidth;
		else padding = (space - totalWidth) / 2;
	}

	float sf;
	for(unsigned int i = 0; i < strlen(string); i++){
		if(string[i] == ' ') x += (float)width / 2.0;
		else{
			glBindTexture(GL_TEXTURE_2D, glyph[toupper(string[i])]);

			glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
			glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);
			sf = (float)height / (float)h;
			width = w * sf;
			glBegin(GL_QUADS);
			glTexCoord2i(0, 0);
			glVertex3f((x + padding) *scalingFactor, y * scalingFactor, 0.f);

			glTexCoord2i(1, 0);
			glVertex3f((x + padding) * scalingFactor + width * scalingFactor, y * scalingFactor, 0.f);

			glTexCoord2i(1, 1);
			glVertex3f((x + padding) * scalingFactor + width * scalingFactor, y * scalingFactor + height * scalingFactor, 0.f);

			glTexCoord2i(0, 1);
			glVertex3f((x + padding) * scalingFactor, y * scalingFactor + height * scalingFactor, 0.f);
			glEnd();
			x += (float)width;
		}
	}
	return x;
}

void action::draw(int facing, int x, int y, int f, float scalingFactor)
{
	if(sprite[f]){
		glBindTexture(GL_TEXTURE_2D, sprite[f]);
		glBegin(GL_QUADS);
		if(facing == 1){
			glTexCoord2i(0, 0);
			glVertex3f((GLfloat)(x)*scalingFactor, (GLfloat)(y - height[f])*scalingFactor, 0.f);

			glTexCoord2i(1, 0);
			glVertex3f((GLfloat)(x + width[f])*scalingFactor, (GLfloat)(y - height[f])*scalingFactor, 0.f);

			glTexCoord2i(1, 1);
			glVertex3f((GLfloat)(x + width[f])*scalingFactor, (GLfloat)(y)*scalingFactor, 0.f);

			glTexCoord2i(0, 1);
			glVertex3f((GLfloat)(x)*scalingFactor, (GLfloat)(y)*scalingFactor, 0.f);
		} else {
			glTexCoord2i(0, 0);
			glVertex3f((GLfloat)(x)*scalingFactor, (GLfloat)(y - height[f])*scalingFactor, 0.f);

			glTexCoord2i(1, 0);
			glVertex3f((GLfloat)(x - width[f])*scalingFactor, (GLfloat)(y - height[f])*scalingFactor, 0.f);

			glTexCoord2i(1, 1);
			glVertex3f((GLfloat)(x - width[f])*scalingFactor, (GLfloat)(y)*scalingFactor, 0.f);

			glTexCoord2i(0, 1);
			glVertex3f((GLfloat)(x)*scalingFactor, (GLfloat)(y)*scalingFactor, 0.f);
		}
		glEnd();
	}
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

bool instance::spriteCheck()
{
	return cMove->spriteCheck(currentFrame);
}
bool avatar::spriteCheck(action *& cMove, int f)
{
	if(cMove == NULL) return 0;
	else return cMove->spriteCheck(f);
}

bool action::spriteCheck(int f)
{
	if(sprite[f] != 0) { 
		return 1;
	}
	else return 0;
}

void interface::writeImage(const char * movename, int frame, action * move)
{
	int realPosY = move->collision[frame].y;
	int realPosX = 0;
	SDL_Surface * image = NULL;
	int maxY = move->collision[frame].y + move->collision[frame].h, 
	    maxX = move->collision[frame].x + move->collision[frame].w;
	for(int i = 0; i < move->regComplexity[frame]; i++){
		if(move->hitreg[frame][i].y < realPosY) 
			realPosY = move->hitreg[frame][i].y;
		if(move->hitreg[frame][i].x < realPosX) 
			realPosX = move->hitreg[frame][i].x;
		if(move->hitreg[frame][i].x + move->hitreg[frame][i].w > maxX)
			maxX = move->hitreg[frame][i].x + move->hitreg[frame][i].w;
		if(move->hitreg[frame][i].y + move->hitreg[frame][i].h > maxY)
			maxY = move->hitreg[frame][i].y + move->hitreg[frame][i].h;
	}
	for(int i = 0; i < move->hitComplexity[frame]; i++){
		if(move->hitbox[frame][i].y < realPosY) 
			realPosY = move->hitbox[frame][i].y;
		if(move->hitbox[frame][i].x < realPosX) 
			realPosX = move->hitbox[frame][i].x;
		if(move->hitbox[frame][i].x + move->hitbox[frame][i].w > maxX)
			maxX = move->hitbox[frame][i].x + move->hitbox[frame][i].w;
		if(move->hitbox[frame][i].y + move->hitbox[frame][i].h > maxY)
			maxY = move->hitbox[frame][i].y + move->hitbox[frame][i].h;
	}
	char fname[200];
	int w = maxX - realPosX;
	int h = maxY;
	int x = 0;
	int y = 0;
	if(realPosY < 0){ 
		h -= realPosY;
		y = realPosY;
	}
	if(realPosX < 0){
		x = realPosX;
	}
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	Uint32 rmask = 0xff000000;
	Uint32 gmask = 0x00ff0000;
	Uint32 bmask = 0x0000ff00;
	Uint32 amask = 0x000000ff;
#else
	Uint32 rmask = 0x000000ff;
	Uint32 gmask = 0x0000ff00;
	Uint32 bmask = 0x00ff0000;
	Uint32 amask = 0xff000000;
#endif
	image = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 32,
				 rmask, gmask, bmask, amask);
	screenInit(w, h);

	sprintf(fname, "%s#%i.bmp", movename, frame);

	glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
	glRectf(0.0f, 0.0f, (GLfloat)w, (GLfloat)h);

	move->drawBoxen(frame, x, y - h);

	glReadPixels(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, image->pixels);

	SDL_GL_SwapBuffers();

	if(SDL_SaveBMP(image, fname)) printf("You dun fucked up\n");
}

void action::drawBoxen(int frame, int x, int y){
	glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
	glRectf((GLfloat)(collision[frame].x - x), (GLfloat)(-collision[frame].y - y), (GLfloat)(collision[frame].x + collision[frame].w - x), (GLfloat)(-collision[frame].y - collision[frame].h - y));
	for(int i = 0; i < regComplexity[frame]; i++){
		glFlush();
		glColor4f(0.0f, 1.0f, 0.0f, 0.5f);
		glRectf((GLfloat)(hitreg[frame][i].x - x), (GLfloat)(-hitreg[frame][i].y - y), (GLfloat)(hitreg[frame][i].x + hitreg[frame][i].w - x), (GLfloat)(-hitreg[frame][i].y - hitreg[frame][i].h - y));
	}
	for(int i = 0; i < hitComplexity[frame]; i++){
		glFlush();
		glColor4f(1.0f, 0.0f, 0.0f, 0.5f);
		glRectf((GLfloat)(hitbox[frame][i].x - x), (GLfloat)(-hitbox[frame][i].y - y), (GLfloat)(hitbox[frame][i].x + hitbox[frame][i].w - x), (GLfloat)(-hitbox[frame][i].y - hitbox[frame][i].h - y));
	}
	glFlush();
}

bool interface::screenInit(int w, int h)
{
	/*Initialize SDL*/
	if(SDL_Init(SDL_INIT_EVERYTHING) < 0) return false;
	/*WM stuff*/
	if(screen){ 
		SDL_FreeSurface(screen);
		screen = NULL;
	}
	SDL_WM_SetCaption("GUFG", "GUFG");
	if((screen = SDL_SetVideoMode(w, h, 32, SDL_OPENGL)) == NULL)
		return false;
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
