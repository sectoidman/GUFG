#ifndef CREATURE_INCLUDED
#define CREATURE_INCLUDED
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
