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
	GLuint t[2];
	SDL_Surface * back = SDL_DisplayFormatAlpha(background);
	SDL_Rect rounds1[numRounds], rounds2[numRounds];

	for(int i = 0; i < numRounds; i++){
		rounds1[i].y = 12; rounds1[i].w = 10; rounds1[i].h = 5;
		rounds2[i].y = 12; rounds2[i].w = 10; rounds2[i].h = 5;
		rounds1[i].x = 340 - 12 * i; rounds2[i].x = 450 + 12 * i;
	}
	glBindTexture(GL_TEXTURE_2D, bground);
	glBegin(GL_QUADS);
		glTexCoord2i(0, 0);
		glVertex3f((float)(bg.x), (float)(900 - (bg.y + bg.h)), 0.f);

		glTexCoord2i(1, 0);
		glVertex3f((float)(bg.x + bg.w), (float)(900 - (bg.y + bg.h)), 0.f);

		glTexCoord2i(1, 1);
		glVertex3f((float)(bg.x + bg.w), (float)(900 - bg.y), 0.f);

		glTexCoord2i(0, 1);
		glVertex3f((float)(bg.x), (float)(900 - bg.y), 0.f);
	glEnd();
	for(int i = 0; i < 2; i++){
		/*if(p[i]->sprite){
			aux::surface_to_texture(p[i]->sprite);
			glBindTexture(GL_TEXTURE_2D, t[i]);
			glBegin(GL_QUADS);
				glTexCoord2i(0, 0);
				glVertex3f((GLfloat)(p[i]->spr.x), (GLfloat)(p[i]->spr.y + p[i]->spr.h), 0.f);

				glTexCoord2i(1, 0);
				glVertex3f((GLfloat)(p[i]->spr.x + p[i]->spr.w), (GLfloat)(p[i]->spr.y + p[i]->spr.h), 0.f);

				glTexCoord2i(1, 1);
				glVertex3f((GLfloat)(p[i]->spr.x + p[i]->spr.w), (GLfloat)(p[i]->spr.y), 0.f);

				glTexCoord2i(0, 1);
				glVertex3f((GLfloat)(p[i]->spr.x), (GLfloat)(p[i]->spr.y), 0.f);
			glEnd();
		} else {*/
			glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
			glRectf((GLfloat)(p[i]->collision.x), (GLfloat)(p[i]->collision.y), (GLfloat)(p[i]->collision.x + p[i]->collision.w), (GLfloat)(p[i]->collision.y + p[i]->collision.h));
			for(int j = 0; j < p[i]->regComplexity; j++){
				glColor4f(0.0f, 1.0f, 0.0f, 0.5f);
				glRectf((GLfloat)(p[i]->hitreg[j].x), (GLfloat)(p[i]->hitreg[j].y), (GLfloat)(p[i]->hitreg[j].x + p[i]->hitreg[j].w), (GLfloat)(p[i]->hitreg[j].y + p[i]->hitreg[j].h));
			}
			for(int j = 0; j < p[i]->hitComplexity; j++){
				glColor4f(1.0f, 0.0f, 0.0f, 0.5f);
				glRectf((GLfloat)(p[i]->hitbox[j].x), (GLfloat)(p[i]->hitbox[j].y), (GLfloat)(p[i]->hitbox[j].x + p[i]->hitbox[j].w), (GLfloat)(p[i]->hitbox[j].y + p[i]->hitbox[j].h));
			}
//		}

	}
//	SDL_FillRect(screen, &screen->clip_rect, SDL_MapRGB(screen->format, 255, 212, 120));

	for(int i = 0; i < numRounds; i++){
		if(p[0]->rounds > i) glColor4f(0.0f, 1.0f, 1.0f, 1.0f);
		else glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
		glRectf((GLfloat)(rounds1[i].x), (GLfloat)(rounds1[i].y), (GLfloat)(rounds1[i].x + rounds1[i].w), (GLfloat)(rounds1[i].y + rounds1[i].h));
		if(p[1]->rounds > i) glColor4f(0.0f, 1.0f, 1.0f, 1.0f);
		else glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
		glRectf((GLfloat)(rounds2[i].x), (GLfloat)(rounds2[i].y), (GLfloat)(rounds2[i].x + rounds2[i].w), (GLfloat)(rounds2[i].y + rounds2[i].h));
	}

	for(int i = 0; i < 2; i++){
		p[i]->pick->drawMeters(screen, i);
	}

	SDL_GL_SwapBuffers();
	SDL_FreeSurface(back);
}

void player::spriteInit()
{
	int realPosY = collision.y;
	int realPosX = 0;
	for(int i = 0; i < hitComplexity; i++){
		if(hitbox[i].y < realPosY) realPosY = hitbox[i].y;
		if(hitbox[i].x < realPosX) realPosX = hitbox[i].x;
	}
	for(int i = 0; i < regComplexity; i++){
		if(hitreg[i].y < realPosY) realPosY = hitreg[i].y;
		if(hitreg[i].x < realPosX) realPosX = hitreg[i].x;
	}

	/*Doing moves*/
//	if(pick->freeze > 0) pick->freeze--;
//	else 
		sprite = pick->draw(facing);
	if(facing == -1) {
		if(sprite) spr.x = posX - sprite->w;
	} else {
		if(sprite) spr.x = posX;
	}
	if(sprite) spr.y = realPosY;
}

SDL_Surface * character::draw(int facing)
{
	SDL_Surface * temp;
	if(freeze < 0) freeze = 0;
	temp = cMove->draw(facing, freeze, meter);
	if(freeze > 0) freeze--;
	if(cMove->currentFrame == cMove->frames){
		cMove->init();
		cMove = cMove->next;
	}
	return temp;
}

void character::drawMeters(SDL_Surface *& screen, int ID)
{	
	SDL_Rect m;
	SDL_Rect h;
	if(health >= 0) h.w = health; else h.w = 1; 

	if(ID == 0) h.x = 50 + (300 - h.w); 
	else h.x = 450;
	h.h = 5;
	h.y = 5;

	int R = 0, G = 255, B = 0;
	if(meter[0] >= 0) m.w = meter[0]; else m.w = 1; 
	if(ID == 0) m.x = 100; 
	else m.x = 500 + (200 - m.w);
	m.h = 5; m.y = 580;

	if(m.w < 100) R = 191;
	else if(m.w < 200) B = 255;
	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
	glRectf((GLfloat)(h.x), (GLfloat)(h.y), (GLfloat)(h.x + h.w), (GLfloat)(h.y + h.h));
	glColor4f((float)R, (float)G, (float)B, 1.0f);
	glRectf((GLfloat)(m.x), (GLfloat)(m.y), (GLfloat)(m.x + m.w), (GLfloat)(m.y + m.h));
//	SDL_FillRect(screen, &m, SDL_MapRGB(screen->format, R, G, B));
//	SDL_FillRect(screen, &h, SDL_MapRGB(screen->format, 255, 0, 0));
}

SDL_Surface * move::draw(int facing, bool freeze, int *& meter)
{
	SDL_Surface * temp;
	if(facing == -1) temp = fSprite[currentFrame];
	else temp = sprite[currentFrame];
	if(freeze == 0) 
		step(meter);
	return temp;
}
