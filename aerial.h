#include "utility.h"
class airMove : public move {
	void build (char *);
	void land(move *&);
	move * landing;
};

class airSpecial : public airMove : public special {

}
