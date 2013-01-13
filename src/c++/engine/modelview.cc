#include "window.h"
#include "harness.h"
#include "thing.h"
#include <time.h>
void draw(window & mainWin, thing & object, harness & joy)
{
	float dim = 5.0;

	glViewport(0, 0, mainWin.w, mainWin.h);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION); 
	glLoadIdentity(); 
	glMatrixMode(GL_MODELVIEW); 
	glLoadIdentity(); 
	glPushMatrix(); 
	glPushMatrix(); 
	glMatrixMode( GL_PROJECTION );
	// void gluOrtho2D ( left, right, bottom, top);
	gluOrtho2D(-dim, dim, -dim, dim);
	glMatrixMode( GL_MODELVIEW );
	glColor4f(0.1f, 0.1f, 0.1f, 0.9f);
	glPushMatrix(); 
	glRotatef(joy.pan, 0.0, 1.0, 0.0f);
	glPushMatrix(); 
	glRotatef(joy.tilt, 1.0, 0.0, 0.0f);
	glTranslatef(joy.camX, joy.camY, joy.camZ);
	object.draw(joy.size);
	glPopMatrix();
	glPopMatrix();
	glPopMatrix(); 
	SDL_GL_SwapBuffers();
}

int main(int argc, char * argv[])
{
	window mainWin;
	harness joy;
	thing object;
	if( !mainWin.screenInit() ){
		fprintf( stderr, "Failed to initialize\n" );
		return -1;
	}
	joy.init();
	printf("%i\n", joy.camZ);
	object.readModel();
	while(!joy.gameover){
		joy.readInput();
		draw(mainWin, object, joy);
	}
	return 1;
}
