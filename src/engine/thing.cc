#include "thing.h"
#include <iostream>
#include <fstream>
#include <assert.h>
#include <stdio.h>
#include <cstring>
#include "tokenizer.h"
bool model::readModel()
{
	return readModel("model.obj");
}

bool model::readModel(string fname)
{
        string buffer;
	std::ifstream read;
	read.open(fname);
	assert(!read.fail());
	while(!read.eof()){
		getline(read, buffer);
                tokenizer t(buffer, " \n");
		if(t().size()){
			if(!t.current().compare("v")){
				for(int j = 0; j < 3; j++){
					vertices[j].push_back(stof(t()));
				}
			} else if(!t.current().compare("f")){
				std::vector<int> face;
				while(t().size())
					face.push_back(stoi(t.current()));
				if(face.size() > 2 && face.size() < 5) faces.push_back(face);
			}
		}
	}
	read.close();
	return 1;
}
