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
		int t = 0;
		read.getline(buffer, 300);
		if(token = strtok(buffer, " \n")){
			if(!strcmp(token, "v")){
				for(int j = 0; j < 3; j++){
					token = strtok(NULL, " \n");
					printf("%f ", atof(token));
					vertices[j].push_back(atof(token));
				}
				printf("\n");
				t++;
			}
		}
	}
	read.close();
	return 1;
}

thing::~thing()
{
	for(unsigned int j = 0; j < vertices[0].size(); j++){
		for(int i = 0; i < 3; i++){
			printf("%f ", vertices[i][j]);
		}
		printf("\n");
	}
}
