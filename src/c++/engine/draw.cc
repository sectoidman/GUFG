#include "thing.h"
#include "auxil.h"
#include "session.h"
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
