#include "thing.h"
#include <iostream>
#include <fstream>
#include <assert.h>
#include <stdio.h>
#include <cstring>
bool thing::readModel()
{
	readModel("model.obj");
}

bool thing::readModel(const char * fname)
{
	char buffer[300];
	char * token;
	std::ifstream read;
	read.open(fname);
	assert(!read.fail());
	while(!read.eof()){
		read.getline(buffer, 300);
		if(token = strtok(buffer, " \n")){
			if(!strcmp(token, "v")){
				for(int j = 0; j < 3; j++){
					token = strtok(NULL, " \n");
					vertices[j].push_back(atof(token));
				}
			} else if(!strcmp(token, "f")){
				std::vector<int> face;
				while(token = strtok(NULL, " \n"))
					face.push_back(atoi(token));
				if(face.size() > 2 && face.size() < 5) faces.push_back(face);
			}
		}
	}
	read.close();
	return 1;
}
