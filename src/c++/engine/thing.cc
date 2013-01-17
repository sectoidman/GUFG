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
				printf("Vertex: ");
				for(int j = 0; j < 3; j++){
					token = strtok(NULL, " \n");
					printf("%f ", atof(token));
					vertices[j].push_back(atof(token));
				}
				printf("\n");
			} else if(!strcmp(token, "f")){
				std::vector<int> face;
				printf("Face: ");
				while(token = strtok(NULL, " \n")){
					printf("%i ", atoi(token));
					face.push_back(atoi(token));
				}
				faces.push_back(face);
				printf("\n");
			}
		}
	}
	read.close();
	return 1;
}
