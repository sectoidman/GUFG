/*Drawing routines for project: Ground Up Fighting Game
 *
 *Written by Alex Kelly in 2012, under MIT OSI
 *For detailed license information, see the file COPYING in this directory
 */
#include "session.h"
#include "thing.h"
#include "auxil.h"
#define _USE_MATH_DEFINES
#include "interface.h"
#include <math.h>
#include <iomanip>
#include <sstream>
#include <string>
#include <GL/glu.h>
#include <GL/glut.h>
#include <SDL/SDL_opengl.h>
#include <vector>
void interface::draw()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glPushMatrix();
		glScalef(scalingFactor, scalingFactor, 0.0f);
		glViewport(0, 0, screenWidth*scalingFactor, screenHeight*scalingFactor);
		if(!select[0] || !select[1]) drawCSelect();
		else drawGame();
		if(rMenu != 0) drawRematchMenu();
		else if(pMenu != 0) drawPauseMenu();
	glPopMatrix();
	SDL_GL_SwapBuffers();
}

void interface::drawCSelect()
{
	char buffer[20];
	int x, y;
	glColor4f(0.1f, 0.1f, 0.1f, 1.0f);
	glRectf(0.0f, 0.0f, (GLfloat)screenWidth, (GLfloat)screenHeight);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	for(int i = 0; i < 2; i++){
		if(configMenu[i]) drawConfigMenu(i);
		else if (menu[i]) drawMainMenu(i);
	}
	glEnable( GL_TEXTURE_2D );

	glBindTexture(GL_TEXTURE_2D, selectScreen);
	glBegin(GL_QUADS);
		glTexCoord2i(0, 0);
		glVertex3f(350.0f, 0.0f, 0.f);

		glTexCoord2i(1, 0);
		glVertex3f(1250.0f, 0.0f, 0.f);

		glTexCoord2i(1, 1);
		glVertex3f(1250.0f, 900.0f, 0.f);

		glTexCoord2i(0, 1);
		glVertex3f(350.0f, 900.0f, 0.f);
	glEnd();

	for(int i = 0; i < 2; i++){
		if(!menu[i]){
			sprintf(buffer, "P%i", i+1);
			x = ((float)screenWidth/2.0 + ((float)screenHeight/3.0) * cos(((M_PI*2.0)/(float)numChars)*(float)selection[i]+M_PI/4.0)) - 100.0;
			y = ((float)screenHeight/2.0 + ((float)screenHeight/3.0) * sin(((M_PI*2.0)/(float)numChars)*(float)selection[i]+M_PI/4.0));
			glColor4f(0.0, 0.3+i*0.3, 0.3+(1-i)*0.3, 1.0-select[i]*0.5);
			drawGlyph(buffer, x, 200, y, 50, i*2);
		}
	}

	glDisable( GL_TEXTURE_2D );

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

void interface::drawMainMenu(int ID)
{
	glColor4f(0.0f, 0.0f, 0.0f, 0.8f);
	char buffer[200];
	glRectf(0.0f + 800.0 * ID, 0.0, (screenWidth/2*ID) + (GLfloat)screenWidth/2.0, (GLfloat)screenHeight);
	glEnable( GL_TEXTURE_2D );
	glColor4f(0.0, 0.0, 1.0, 0.4 + (float)(menu[ID] == 1)*0.4);
	if(analytics) sprintf(buffer, "Replay");
	else sprintf(buffer, "No Replay");
	drawGlyph(buffer, 20 + 1260*ID, 300, 310, 40, 2*ID);
	glColor4f(0.0, 0.0, 1.0, 0.4 + (float)(menu[ID] == 2)*0.4);
	drawGlyph("Key Config", 20 + 1260*ID, 300, 350, 40, 2*ID);
	glColor4f(0.0, 0.0, 1.0, 0.4 + (float)(menu[ID] == 3)*0.4);
	drawGlyph("Exit Menu", 20 + 1260*ID, 300, 390, 40, 2*ID);
	glColor4f(0.0, 0.0, 1.0, 0.4 + (float)(menu[ID] == 4)*0.4);
	if(shortcut) sprintf(buffer, "Rematch");
	else sprintf(buffer, "Reselect");
	drawGlyph(buffer, 20 + 1260*ID, 300, 430, 40, 2*ID);
	glColor4f(0.0, 0.0, 1.0, 0.4 + (float)(menu[ID] == 5)*0.4);
	if(scripting) sprintf(buffer, "Scripts on");
	else sprintf(buffer, "Scripts off");
	drawGlyph(buffer, 20 + 1260*ID, 300, 470, 40, 2*ID);
	glColor4f(0.0, 0.0, 1.0, 0.4 + (float)(menu[ID] == 6)*0.4);
	if(pauseEnabled) sprintf(buffer, "Pause on");
	else sprintf(buffer, "Pause off");
	drawGlyph(buffer, 20 + 1260*ID, 300, 510, 40, 2*ID);
	glColor4f(0.0, 0.0, 1.0, 0.4 + (float)(menu[ID] == 7)*0.4);
	drawGlyph("Quit Game", 20 + 1260*ID, 300, 550, 40, 2*ID);
	glDisable( GL_TEXTURE_2D );
	glColor4f(1.0, 1.0, 1.0, 1.0f);
}

void interface::drawConfigMenu(int ID)
{
	int i;
	glColor4f(0.0f, 0.0f, 0.0f, 0.8f);
	char buffer[200];
	glRectf(0.0f + 800.0 * ID, 0.0, (screenWidth/2*ID) + (GLfloat)screenWidth/2.0, (GLfloat)screenHeight);
	glEnable( GL_TEXTURE_2D );
	switch(p[ID]->input[0]->trigger.type){
	case SDL_KEYDOWN:
		sprintf(buffer, "Keyboard");
		break;
	case SDL_JOYBUTTONDOWN:
		sprintf(buffer, "Joy %i", p[ID]->input[0]->trigger.jaxis.which);
		break;
	case SDL_JOYAXISMOTION:
		sprintf(buffer, "Joy %i", p[ID]->input[0]->trigger.jbutton.which);
		break;
	}
	glColor4f(1.0, 1.0, 0.0, 0.4 + (float)(configMenu[ID] == 1)*0.4);
	drawGlyph(buffer, 20 + 1260*ID, 300, 310, 40, 2*ID);
	for(i = 2; i < 7; i++){
		sprintf(buffer, "%s", p[ID]->inputName[i+2]);
		glColor4f(0.0, 0.0, 1.0, 0.4 + (float)(configMenu[ID] == i)*0.4);
		drawGlyph(buffer, 20 + 1230*ID, 300, 310+40*(i-1), 40, 0);
		for(unsigned int j = 0; j < p[ID]->input.size(); j++)
			if(p[ID]->input[j]->effect == 1 << (i+2)){
				switch(p[ID]->input[j]->trigger.type){
				case SDL_KEYDOWN:
					sprintf(buffer, "%s", SDL_GetKeyName(p[ID]->input[j]->trigger.key.keysym.sym));
					break;
				case SDL_JOYBUTTONDOWN:
					sprintf(buffer, "B%i", p[ID]->input[j]->trigger.jbutton.button);
					break;
				case SDL_JOYAXISMOTION:
					sprintf(buffer, "Axis %i %i", p[ID]->input[j]->trigger.jaxis.axis,
						p[ID]->input[j]->trigger.jaxis.value);
					break;
			}
		}
		glColor4f(1.0, 1.0, 0.0, 0.4 + (float)(configMenu[ID] == i)*0.4);
		drawGlyph(buffer, 70 + 1230*ID, 300, 310+40*(i-1), 40, 0);
	}
	glColor4f(0.0, 0.0, 1.0, 0.4 + (float)(configMenu[ID] == 7)*0.4);
	drawGlyph("Exit Menu", 20 + 1260*ID, 300, 310+40*(i-1), 40, 2*ID);
	glDisable( GL_TEXTURE_2D );
	glColor4f(1.0, 1.0, 1.0, 1.0f);
}

void interface::drawGame()
{
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable( GL_TEXTURE_2D );
	glBindTexture(GL_TEXTURE_2D, background);
	glPushMatrix();
		glTranslatef(-bg.x, bg.y, 0);
		glBegin(GL_QUADS);
		glTexCoord2i(0, 0);
		glVertex3f(0.0f, 0.0f, 0.f);

		glTexCoord2i(1, 0);
		glVertex3f((GLfloat)(bg.w), 0.0f, 0.f);

		glTexCoord2i(1, 1);
		glVertex3f((GLfloat)(bg.w), (GLfloat)(bg.h), 0.f);

		glTexCoord2i(0, 1);
		glVertex3f(0.0f, (GLfloat)(bg.h), 0.f);
		glEnd();
	glPopMatrix();
	drawHUD();
	glPushMatrix();
		glTranslatef(-bg.x, (bg.y+bg.h), 0);
		for(unsigned int i = 0; i < things.size(); i++){
			things[i]->draw();
			if(i < 2)
				P[i]->drawHitParticle();
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
			glEnable( GL_TEXTURE_2D );
		}
	glPopMatrix();
	glDisable( GL_TEXTURE_2D );
	if(freeze > 0){
		glColor4f(0.0f, 0.0f, 0.0f, 0.4f);
		glRectf(0.0f, 0.0f, (GLfloat)screenWidth, (GLfloat)screenHeight);
		freeze--;
	}
}

void interface::drawHUD()
{
	char buffer[200];
	if(timer / 60 > 99) sprintf(buffer, "99");
	else if(timer / 60 < 10) sprintf(buffer, "0%i", timer / 60);
	else sprintf(buffer, "%i", timer / 60);

	drawGlyph(buffer, 700, 200, 0, 90, 1);
	for(int i = 0; i < 2; i++){
		drawGlyph(things[i]->pick()->name, 100+800*i, 600, 30, 40, 0+2*i);
		if(P[i]->record){
			glColor4f(0.5, 1.0, 1.0, 0.7);
			drawGlyph("Recording", 100+800*i, 600, 200, 55, 0+2*i);
			glColor4f(1.0, 1.0, 1.0, 1.0);
		}
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
		int l = P[0]->rounds + P[1]->rounds + 1;
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
		if(things[0]->meter[0] > 0 && things[1]->meter[0] > 0){
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
		if(things[0]->meter[0] > things[1]->meter[0]){ 
			sprintf(buffer, "%s", things[0]->pick()->name);
			drawGlyph(buffer, 0, 1600, 300, 150, 1);
			drawGlyph("Wins", 0, 1600, 450, 150, 1);
			if(endTimer == 4 * 60 - 1)
				Mix_PlayChannel(3, announceWinner[selection[0]], 0);
		} else if(things[1]->meter[0] > things[0]->meter[0]){
			sprintf(buffer, "%s", things[1]->pick()->name);
			drawGlyph(buffer, 0, 1600, 300, 150, 1);
			drawGlyph("Wins", 0, 1600, 450, 150, 1);
			if(endTimer == 4 * 60 - 1)
				Mix_PlayChannel(3, announceWinner[selection[1]], 0);
		} else if(things[0]->meter[0] <= 0){ 
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
	for(unsigned int i = 0; i < P.size(); i++){
		P[i]->drawMeters(numRounds);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	}
	glEnable( GL_TEXTURE_2D );
}

void interface::drawPauseMenu()
{
	glColor4f(0.0f, 0.0f, 0.0f, 0.8f);
	glRectf(0.0, 0.0, (GLfloat)screenWidth, (GLfloat)screenHeight);
	glEnable( GL_TEXTURE_2D );
	glColor4f(0.0, 0.0, 1.0, 0.4 + (float)(pMenu == 1)*0.4);
	drawGlyph("Unpause", 0, 1600, 360, 60, 1);
	glColor4f(0.0, 0.0, 1.0, 0.4 + (float)(pMenu == 2)*0.4);
	drawGlyph("Character Select", 0, 1600, 420, 60, 1);
	glColor4f(0.0, 0.0, 1.0, 0.4 + (float)(pMenu == 3)*0.4);
	drawGlyph("Quit Game", 0, 1600, 480, 60, 1);
	glDisable( GL_TEXTURE_2D );
	glColor4f(1.0, 1.0, 1.0, 1.0f);
}

void interface::drawRematchMenu()
{
	glColor4f(0.0f, 0.0f, 0.0f, 0.8f);
	glRectf(0.0, 0.0, (GLfloat)screenWidth, (GLfloat)screenHeight);
	glEnable( GL_TEXTURE_2D );
	glColor4f(0.0, 0.0, 1.0, 0.4 + (float)(rMenu == 1)*0.4);
	drawGlyph("Rematch", 0, 1600, 360, 60, 1);
	glColor4f(0.0, 0.0, 1.0, 0.4 + (float)(rMenu == 2)*0.4);
	drawGlyph("Character Select", 0, 1600, 420, 60, 1);
	glColor4f(0.0, 0.0, 1.0, 0.4 + (float)(rMenu == 3)*0.4);
	drawGlyph("Quit Game", 0, 1600, 480, 60, 1);
	glDisable( GL_TEXTURE_2D );
	glColor4f(1.0, 1.0, 1.0, 1.0f);
}

void player::drawMeters(int n)
{
	std::vector<SDL_Rect> r (n);
	for(int i = 0; i < n; i++){
		r[i].y = 24; r[i].w = 20; r[i].h = 10;
		if(ID == 1) r[i].x = 680 - 24 * i; 
		else r[i].x = 900 + 24 * i;
	}
	for(int i = 0; i < n; i++){
		if(rounds > i) glColor4f(0.0f, 1.0f, 1.0f, 1.0f);
		else glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
		glRectf((GLfloat)(r[i].x), (GLfloat)(r[i].y), (GLfloat)(r[i].x + r[i].w), (GLfloat)(r[i].y + r[i].h));
	}
	glFlush();
	int h = 0;
	if(cMove){
		if(cMove->hidesMeter) h = cMove->cost;
	}
	pick()->drawMeters(ID, h, meter);
	glFlush();
}

void character::drawMeters(int ID, int hidden, int * meter)
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
	glRectf((GLfloat)(h.x), (GLfloat)(h.y), (GLfloat)(h.x + h.w), (GLfloat)(h.y + h.h));
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glColor4f((float)R, (float)G, (float)B, 1.0f);
	glRectf((GLfloat)(m.x), (GLfloat)(m.y), (GLfloat)(m.x + m.w), (GLfloat)(m.y + m.h));
}

void instance::drawBoxen()
{
	glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
	glPushMatrix();
		glTranslatef(collision.x, -collision.y, 0);
		glRectf(0.0f, 0.0f, (GLfloat)(collision.w), (GLfloat)(-collision.h));
	glPopMatrix();
	for(int i = 0; i < regComplexity; i++){
		glFlush();
		glColor4f(0.0f, 1.0f, (GLfloat)(ID - 1.0f)/2.0f, 0.5f);
		glNormal3f(0.0f, 0.0f, 1.0f);
		glPushMatrix();
			glTranslatef(hitreg[i].x, -hitreg[i].y, 0);
			glRectf(0.0f, 0.0f, (GLfloat)(hitreg[i].w), (GLfloat)(-hitreg[i].h));
		glPopMatrix();
	}
	for(int i = 0; i < hitComplexity; i++){
		glFlush();
		glColor4f(1.0f, 0.0f, (GLfloat)(ID - 1.0f)/2.0f, 0.5f);
		glPushMatrix();
			glTranslatef(hitbox[i].x, -hitbox[i].y, 0);
			glRectf(0.0f, 0.0f, (GLfloat)(hitbox[i].w), (GLfloat)(-hitbox[i].h));
		glPopMatrix();
	}
	glFlush();
	glDisable( GL_TEXTURE_2D );
}

void instance::draw()
{
	int realPosY = collision.y;
	int realPosX = posX;
	glEnable(GL_TEXTURE_2D);
	if(spriteCheck()){
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
		glPushMatrix();
			glTranslatef(realPosX, -realPosY, 0);
			glPushMatrix();
				glScalef(facing, 1.0, 1.0);
				pick()->draw(cMove, currentFrame);
			glPopMatrix();
		glPopMatrix();
	}
	glDisable(GL_TEXTURE_2D);
	if(!spriteCheck() || boxen){
		drawBoxen();
	}
}

void player::drawHitParticle()
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
		glPushMatrix();
			glTranslatef(posX, -collision.y, 0.0f);
			glRectf((GLfloat)(-10*facing), (GLfloat)(-collision.h), (GLfloat)(50 * facing), (GLfloat)(-collision.h - 40));
		glPopMatrix();
		particleLife--;
	} else blockType = 0;
}

void avatar::draw(action *& cMove, int f)
{
	cMove->draw(f);
}

int gameInstance::drawGlyph(const char * string, int x, int space, int y, int height, int just)
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
			glPushMatrix();
				glTranslatef(x + padding, y, 0.f);
				glBegin(GL_QUADS);
				glTexCoord2i(0, 0);
				glVertex3f(0, 0, 0.f);

				glTexCoord2i(1, 0);
				glVertex3f(width, 0, 0.f);

				glTexCoord2i(1, 1);
				glVertex3f(width, height, 0.f);

				glTexCoord2i(0, 1);
				glVertex3f(0, height, 0.f);
				glEnd();
			glPopMatrix();
			x += (float)width;
		}
	}
	return x;
}

void action::draw(int f)
{
	if(modifier && basis) basis->draw(currentFrame);
	if(sprite[f]){
		glBindTexture(GL_TEXTURE_2D, sprite[f]);
		glBegin(GL_QUADS);
		glTexCoord2i(0, 0);
		glVertex3f(0.0f, (GLfloat)(-height[f]), 0.f);

		glTexCoord2i(1, 0);
		glVertex3f((GLfloat)(width[f]), (GLfloat)(-height[f]), 0.f);

		glTexCoord2i(1, 1);
		glVertex3f((GLfloat)(width[f]), 0.0f, 0.f);

		glTexCoord2i(0, 1);
		glVertex3f(0.0f, 0.0f, 0.f);
		glEnd();
	}
}

bool instance::spriteCheck()
{
	if(cMove) return cMove->spriteCheck(currentFrame);
	else return 0;
}
bool avatar::spriteCheck(action *& cMove, int f)
{
	if(cMove == NULL) return 0;
	else return cMove->spriteCheck(f);
}

bool action::spriteCheck(int f)
{
	if(modifier && basis) basis->spriteCheck(currentFrame);
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

	sprintf(fname, "content/characters/%s#%i.bmp", movename, frame);

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

bool gameInstance::screenInit(int w, int h)
{
	/*Initialize SDL*/
	if(SDL_Init(SDL_INIT_EVERYTHING) < 0) return false;
	/*WM stuff*/
	if(window::screen){ 
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

void session::draw(model & object)
{
	glViewport(0, 0, w, h);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION); 
	glLoadIdentity(); 
	glMatrixMode(GL_MODELVIEW); 
	glLoadIdentity(); 
	glPushMatrix(); 
	glPushMatrix(); 
	glMatrixMode( GL_PROJECTION );
	gluPerspective(100.0, 1.5, 1.0, 150.0);
	float modelColor[] = { r, g, b, 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, modelColor);
	glMatrixMode( GL_MODELVIEW );
	glTranslatef(camX, camY, camZ);
	glPushMatrix(); 
	glRotatef(spin, 0.0, 0.0, 1.0f);
	glPushMatrix(); 
	glRotatef(pan, 0.0, 1.0, 0.0f);
	glPushMatrix(); 
	glRotatef(tilt, 1.0, 0.0, 0.0f);
	glPushMatrix();
	object.draw();
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	glPopMatrix(); 
	SDL_GL_SwapBuffers();
}

void model::draw()
{
/* Load the identity matrix into modelmatrix. rotate the model, and move it back 5 */
	vect v;
	for(unsigned int i = 0; i < faces.size(); i ++){
		v.unitNormal( vertices[0][faces[i][0]-1], vertices[1][faces[i][0]-1], vertices[2][faces[i][0]-1],
				vertices[0][faces[i][1]-1], vertices[1][faces[i][1]-1], vertices[2][faces[i][1]-1],
				vertices[0][faces[i][2]-1], vertices[1][faces[i][2]-1], vertices[2][faces[i][2]-1]);
		switch (faces[i].size()){
		case 3:
			glBegin(GL_TRIANGLES);
				glNormal3f(v.x, v.y, v.z);
				glVertex3f(vertices[0][faces[i][0]-1], vertices[1][faces[i][0]-1], vertices[2][faces[i][0]-1]);
				glVertex3f(vertices[0][faces[i][1]-1], vertices[1][faces[i][1]-1], vertices[2][faces[i][1]-1]);
				glVertex3f(vertices[0][faces[i][2]-1], vertices[1][faces[i][2]-1], vertices[2][faces[i][2]-1]);
			glEnd();
			break;
		case 4:
			glBegin(GL_QUADS);
				glNormal3f(v.x, v.y, v.z);
				glVertex3f(vertices[0][faces[i][0]-1], vertices[1][faces[i][0]-1], vertices[2][faces[i][0]-1]);
				glVertex3f(vertices[0][faces[i][1]-1], vertices[1][faces[i][1]-1], vertices[2][faces[i][1]-1]);
				glVertex3f(vertices[0][faces[i][2]-1], vertices[1][faces[i][2]-1], vertices[2][faces[i][2]-1]);
				glVertex3f(vertices[0][faces[i][3]-1], vertices[1][faces[i][3]-1], vertices[2][faces[i][3]-1]);
			glEnd();
			break;
		}
	}
}
