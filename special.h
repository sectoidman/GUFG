#include "hitstun.h"
class special : public move {
public:
	special(char*);
	bool check(bool[], bool[], int);
	int chip;
};
