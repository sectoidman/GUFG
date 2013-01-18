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

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable (GL_BLEND);
	glEnable (GL_POINT_SMOOTH);
	glEnable (GL_LINE_SMOOTH);
	glEnable (GL_POLYGON_SMOOTH);
	glEnable (GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);

	glHint (GL_POLYGON_SMOOTH_HINT, GL_NICEST);

	glClearColor(1, 1, 1, 0);
	glShadeModel (GL_SMOOTH);
	glClearDepth(1.0f);
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, w, h, 0, 1, -1);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	return true;
}

void window::setLighting()
{
	GLfloat lightColor0[] = {0.5f, 0.2f, 0.2f, 0.5f}; //Color (0.5, 0.5, 0.5)
	GLfloat lightPos0[] = {5.0f, 0.0f, 9.0f, 1.0f}; //Positioned at (4, 0, 8)
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_shininess[] = { 50.0 };
	GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
}
