#include "window.h"
#include "harness.h"
#include "thing.h"
#include <time.h>
void draw(window & mainWin, thing & object, harness & joy)
{
	glViewport(0, 0, mainWin.w, mainWin.h);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION); 
	glLoadIdentity(); 
	glMatrixMode(GL_MODELVIEW); 
	glLoadIdentity(); 
	glPushMatrix(); 
	glPushMatrix(); 
	glColor4f(1.0f, 1.0f, 0.0f, 0.5f);
	glRectf(0.0f, 0.0f, mainWin.w/2, mainWin.h);
	glPopMatrix();
	glPushMatrix(); 
	glColor4f(0.1f, 0.1f, 0.1f, 0.9f);
	glTranslatef(joy.camX, joy.camY, joy.camZ);
	glRotatef(45, joy.pan, 0.0f, 0.0f);
	object.draw(joy.pan);
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
