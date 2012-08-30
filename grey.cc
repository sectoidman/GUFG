#include "grey.h"
grey::grey(){
	head = new actionTrie;
	airHead = new actionTrie;
	meter = new int[3];
	build("Grey", "Grey");
}
