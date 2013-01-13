#include "thing.h"
void thing::draw()
{
/* Load the identity matrix into modelmatrix. rotate the model, and move it back 5 */
	for(unsigned int i = 0; i < vertices[0].size(); i += 3){
		glBegin(GL_TRIANGLES);
			glVertex3f(vertices[0][i], vertices[1][i], vertices[2][i]);
			glVertex3f(vertices[0][i+1], vertices[1][i+1], vertices[2][i+1]);
			glVertex3f(vertices[0][i+2], vertices[1][i+2], vertices[2][i+2]);
		glEnd();
	}
}
