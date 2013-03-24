#include <iostream>
#include <stdio.h>
#include <fstream>
#include "engine/interface.h"
#include <cstring>
int main(int argc, char* argv[])
{
	interface z;
	if(argc < 2) printf("Please supply a move\n");
	else{
		tokenizer t(argv[1], "/");
		string content = t();
		string characters = t();
		string ch = t();
		string mv = t();
		string s = t();
		if(!s.empty()){ 
			ch += "/" + mv;
			mv = s;
		}
		tokenizer m(mv, ".");
		std::cout << ch + ": " + mv + "\n";
		mv = m();
		action * a = new action(ch, mv);

		for(int i = 0; i < a->frames; i++){
			z.writeImage(mv, i, a);
		}
		delete a;
	}
}

