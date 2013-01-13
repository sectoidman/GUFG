#include "window.h"
#include <GL/glu.h>
#include <GL/glut.h>
window::window()
{
	screen = NULL;
	h = 450, w = 800;
}

bool window::screenInit()
{
	/*Initialize SDL*/
	if(SDL_Init(SDL_INIT_EVERYTHING) < 0){  
		return false;

	}
	/*WM stuff*/
	SDL_WM_SetCaption("downtop", "downtop");
	if((screen = SDL_SetVideoMode(w, h, 32, SDL_OPENGL)) == NULL)
		return false;
	SDL_ShowCursor(SDL_DISABLE);

//	glDisable (GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable (GL_BLEND);
	glEnable (GL_POINT_SMOOTH);
	glEnable (GL_LINE_SMOOTH);
	glEnable (GL_POLYGON_SMOOTH);

	glHint (GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glClearColor(1, 1, 1, 1);
	glEnable(GL_SMOOTH);
	glShadeModel(GL_SMOOTH);
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	glClearDepth(1.0f);
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glEnable(GL_LIGHTING);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	return true;
}

void window::setLighting()
{
	GLfloat lightColor0[] = {0.5f, 0.2f, 0.2f, 1.0f}; //Color (0.5, 0.5, 0.5)
	GLfloat lightPos0[] = {5.0f, 0.0f, 9.0f, 1.0f}; //Positioned at (4, 0, 8)
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
}

