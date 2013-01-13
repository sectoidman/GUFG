#include "thing.h"
void thing::draw(double s)
{
/* Load the identity matrix into modelmatrix. rotate the model, and move it back 5 */
	for(unsigned int i = 0; i < vertices[0].size(); i += 3){
		glBegin(GL_TRIANGLES);
			glVertex3f(vertices[0][i]/s, vertices[1][i]/s, vertices[2][i]/s);
			glVertex3f(vertices[0][i+1]/s, vertices[1][i+1]/s, vertices[2][i+1]/s);
			glVertex3f(vertices[0][i+2]/s, vertices[1][i+2]/s, vertices[2][i+2]/s);
		glEnd();
	}
}
