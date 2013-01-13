#include "thing.h"
#include "auxil.h"
void thing::draw()
{
/* Load the identity matrix into modelmatrix. rotate the model, and move it back 5 */
	GLfloat lightColor0[] = {0.5f, 0.2f, 0.2f, 1.0f};
	GLfloat lightPos0[] = {5.0f, 0.0f, -2.0f, 1.0f};
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
	float nx[2], ny[2], nz[2];
	for(unsigned int i = 0; i < vertices[0].size(); i += 3){
		glBegin(GL_TRIANGLES);
			for(int j = 0; j < 2; j++){
				nx[j] = vertices[0][i+j] - vertices[0][i];
				ny[j] = vertices[1][i+j] - vertices[1][i];
				nz[j] = vertices[2][i+j] - vertices[2][i];
			}
			vect v; 
			v.crossProduct(nx[0], ny[0], nz[0], nx[1], ny[1], nz[1]);
			glNormal3f(v.x, v.y, v.z);
			glVertex3f(vertices[0][i], vertices[1][i], vertices[2][i]);
			glVertex3f(vertices[0][i+1], vertices[1][i+1], vertices[2][i+1]);
			glVertex3f(vertices[0][i+2], vertices[1][i+2], vertices[2][i+2]);
		glEnd();
	}
}
