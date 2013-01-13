#include "auxil.h"

void vect::crossProduct(float Ax, float Ay, float Az, float Bx, float By, float Bz)
{
	x = (Ay*Bz)-(By*Az);
	y = -(Ax*Bz)+(Bx*Az);
	z = (Ax*By)-(Ay*Bx);
};
