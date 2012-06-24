/*Drawing routines for project: Ground Up Fighting Game
 *
 *Written by Alex Kelly in 2012, under MIT OSI
 *For detailed license information, see the file COPYING in this directory
 */

#include "interface.h"
#include <math.h>
#include <SDL/SDL_opengl.h>
void interface::draw()
{
	glClear(GL_COLOR_BUFFER_BIT);

	glBindTexture(GL_TEXTURE_2D, background);
	glBegin(GL_QUADS);
		glTexCoord2i(0, 0);
		glVertex3f((GLfloat)(-bg.x), (GLfloat)(-bg.y), 0.f);

		glTexCoord2i(1, 0);
		glVertex3f((GLfloat)(bg.w - bg.x), (GLfloat)(-bg.y), 0.f);

		glTexCoord2i(1, 1);
		glVertex3f((GLfloat)(bg.w - bg.x), (GLfloat)(bg.h - bg.y), 0.f);

		glTexCoord2i(0, 1);
		glVertex3f((GLfloat)(-bg.x), (GLfloat)(bg.h - bg.y), 0.f);
	glEnd();

	for(int i = 0; i < 2; i++){
		p[i]->drawMeters(numRounds);
		if(!p[i]->spriteCheck()) p[i]->drawBoxen(bg.x, bg.y);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	}

	for(int i = 0; i < 2; i++){
		if(p[i]->spriteCheck()) p[i]->draw(bg.x, bg.y);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	}
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	SDL_GL_SwapBuffers();
}

void player::drawBoxen(int x, int y)
{
	glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
	glRectf((GLfloat)(collision.x - x), (GLfloat)(collision.y - y), (GLfloat)(collision.x + collision.w - x), (GLfloat)(collision.y + collision.h - y));
	for(int i = 0; i < regComplexity; i++){
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glColor4f(0.0f, 1.0f, 0.0f, 0.5f);
		glRectf((GLfloat)(hitreg[i].x - x), (GLfloat)(hitreg[i].y - y), (GLfloat)(hitreg[i].x + hitreg[i].w - x), (GLfloat)(hitreg[i].y + hitreg[i].h - y));
	}
	for(int i = 0; i < hitComplexity; i++){
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glColor4f(1.0f, 0.0f, 0.0f, 0.5f);
		glRectf((GLfloat)(hitbox[i].x - x), (GLfloat)(hitbox[i].y - y), (GLfloat)(hitbox[i].x + hitbox[i].w - x), (GLfloat)(hitbox[i].y + hitbox[i].h - y));
	}
	glEnd();
}

void player::draw(int x, int y)
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
	
	pick->draw(facing, realPosX - x, realPosY - y);
}


void player::drawMeters(int n)
{
	SDL_Rect r[n];
	for(int i = 0; i < n; i++){
		r[i].y = 12; r[i].w = 10; r[i].h = 5;
		if(ID == 1) r[i].x = 340 - 12 * i; 
		else r[i].x = 450 + 12 * i;
	}
	for(int i = 0; i < n; i++){
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		if(rounds > i) glColor4f(0.0f, 1.0f, 1.0f, 1.0f);
		else glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
		glRectf((GLfloat)(r[i].x), (GLfloat)(r[i].y), (GLfloat)(r[i].x + r[i].w), (GLfloat)(r[i].y + r[i].h));
	}
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	pick->drawMeters(ID);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

void character::drawMeters(int ID)
{
	SDL_Rect m;
	SDL_Rect h;
	if(health >= 0) h.w = health; else h.w = 1; 

	if(ID == 1) h.x = 50 + (300 - h.w); 
	else h.x = 450;
	h.h = 5;
	h.y = 5;

	int R = 0, G = 255, B = 0;
	if(meter[0] >= 0) m.w = meter[0]; else m.w = 1; 
	if(ID == 1) m.x = 100; 
	else m.x = 500 + (200 - m.w);
	m.h = 5; m.y = 430;

	if(m.w < 100) R = 191;
	else if(m.w < 200) B = 255;
	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
	glRectf((GLfloat)(h.x), (GLfloat)(h.y), (GLfloat)(h.x + h.w), (GLfloat)(h.y + h.h));
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glColor4f((float)R, (float)G, (float)B, 1.0f);
	glRectf((GLfloat)(m.x), (GLfloat)(m.y), (GLfloat)(m.x + m.w), (GLfloat)(m.y + m.h));
}

void character::draw(int facing, int x, int y)
{
	cMove->draw(facing, x, y);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

void move::draw(int facing, int x, int y)
{
	if(sprite[currentFrame]){
		glBindTexture(GL_TEXTURE_2D, sprite[currentFrame]);
		glBegin(GL_QUADS);
		if(facing == 1){
			glTexCoord2i(0, 0);
			glVertex3f((GLfloat)(x), (GLfloat)(y), 0.f);

			glTexCoord2i(1, 0);
			glVertex3f((GLfloat)(x + width[currentFrame]), (GLfloat)(y), 0.f);

			glTexCoord2i(1, 1);
			glVertex3f((GLfloat)(x + width[currentFrame]), (GLfloat)(y + height[currentFrame]), 0.f);

			glTexCoord2i(0, 1);
			glVertex3f((GLfloat)(x), (GLfloat)(y + height[currentFrame]), 0.f);
		} else {
			glTexCoord2i(0, 0);
			glVertex3f((GLfloat)(x), (GLfloat)(y), 0.f);

			glTexCoord2i(1, 0);
			glVertex3f((GLfloat)(x - width[currentFrame]), (GLfloat)(y), 0.f);

			glTexCoord2i(1, 1);
			glVertex3f((GLfloat)(x - width[currentFrame]), (GLfloat)(y + height[currentFrame]), 0.f);

			glTexCoord2i(0, 1);
			glVertex3f((GLfloat)(x), (GLfloat)(y + height[currentFrame]), 0.f);
		}
		glEnd();
	}
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

bool player::spriteCheck()
{
	return pick->spriteCheck();
}
bool character::spriteCheck()
{
	return cMove->spriteCheck();
}
bool move::spriteCheck()
{
	if(sprite[currentFrame]) return 1;
	else return 0;
}
