#include "auxil.h"
#include <math.h>
void vect::crossProduct(float Ax, float Ay, float Az, float Bx, float By, float Bz, float Cx, float Cy, float Cz)
{
	vect AB, AC;
	float i, j, k, len;
	AB.x = Bx - Ax;
	AB.y = By - Ay;
	AB.z = Bz - Az;
	AC.x = Cx - Ax;
	AC.y = Cy - Ay;
	AC.z = Cz - Az;
	i = (AB.y*AC.z-AC.y*AB.z); j = -(AB.x*AC.z-AC.x*AB.z); k = (AB.x*AC.y-AC.x*AB.y);
	len = sqrt(i*i + j*j + k*k);
	x = i / len;
	y = j / len;
	z = k / len;
};
