#include "projectile.h"
class utility : public move {
public:
	utility() {}
	utility(char *);
	bool check(bool[], bool[], int, int);
};
