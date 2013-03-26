#ifndef ___masks
#define ___masks

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
#endif
