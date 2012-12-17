#include <iostream>
#include <stdio.h>
#include <fstream>
#include "engine/interface.h"
#include <cstring>
int main(int argc, char* argv[])
{
	interface z;
	char buffer[150];
	if(argc < 2) printf("Please supply a move\n");
	else{
		for(int i = 1; i < argc; i++){
			strcpy(buffer, argv[i]);
			buffer[strlen(argv[i]) - 3] = '\0';
			printf("%s\n", buffer);
			action * a = new action(buffer);

			for(int i = 0; i < a->frames; i++){
				z.writeImage(buffer, i, a);
			}
			delete a;
		}
	}
}

