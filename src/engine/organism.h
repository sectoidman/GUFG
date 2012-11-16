#ifndef ORGANISM
#define ORGANISM
class organism{
public:
	char * name;
	char * nickname;
	unsigned int type:16;
	unsigned int HP;
	unsigned int IQ;
	unsigned int speed;
	unsigned int atk;
	unsigned int def;
	//attack ** moves;
};
#endif
