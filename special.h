#include "hitstun.h"
class special : public move {
	//FIXME Should these really all be private?
	special(char*);
	bool check(bool[], bool[], int);
	int chip;
};
