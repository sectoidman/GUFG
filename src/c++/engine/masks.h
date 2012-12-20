#ifndef MASKS
#define MASKS
struct buttonBits
{
	bool up:1;
	bool down:1;
	bool left:1;
	bool right:1;
	bool A:1;
	bool B:1;
	bool C:1;
	bool D:1;
	bool E:1;
	bool S:1;
};

//A 6-bit field to keep track of block states.
struct blockBits{
	bool low:1;
	bool high:1;
	bool air:1;
};

//A 16-bit field to keep track of cancel states.
struct cancelBits{
	bool neutral:1;
	bool jump:1;
	bool chain1:1;
	bool chain2:1;
	bool chain3:1;
	bool chain4:1;
	bool chain5:1;
	bool special:1;
	bool super:1;
	bool block:1;
	bool tech:1;
	bool misc1:1;
	bool misc2:1;
	bool misc3:1;
	bool misc4:1;
	bool misc5:1;
	bool dash:1;
};

typedef union {
	unsigned int i;
	cancelBits b;
} cancelField;

typedef union {
	unsigned int i;
	blockBits b; 
} blockField;

typedef union {
	unsigned int i;
	buttonBits b;
} buttonField;
#endif
