//A 6-bit field to keep track of block states.
struct bf{
	bool high:1;
	bool low:1;
	bool air:1;
};

//A 16-bit field to keep track of cancel states.
struct cf{
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

struct effect{
	bool otherCMove:1;
	bool character:1;
	bool otherCharacter:1;
	bool player:1;
	bool otherPlayer:1;
	bool interface:1;
};
	
struct gpf{
	bool crouch:1;
	bool dropVectors:1;
	bool dropDelta:1;
};

struct hpf{
	bool launch:1;
	bool elasticX:1;
	bool elasticY:1;
};

typedef union {
	unsigned int i;
	cf b;
} cancelField;

typedef union {
	unsigned int i;
	bf b; 
} blockField;

typedef union{
	unsigned int i;
	effect b;
} effectHook;

typedef union{
	unsigned int i;
	hpf b;
} properties;

typedef union{
	unsigned int i;
	gpf b;
} status;
