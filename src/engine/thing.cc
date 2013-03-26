#include "thing.h"
#include "tokenizer.h"
#include <assert.h>
#include <cstring>
#include <fstream>
#include <iostream>

using std::ifstream;

bool model::readModel()
{
	return readModel("model.obj");
}

bool model::readModel(string fname)
{
    string buffer;
	ifstream read;
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
				vector<int> face;
				while(t().size())
					face.push_back(stoi(t.current()));
				if(face.size() > 2 && face.size() < 5) faces.push_back(face);
			}
		}
	}
	read.close();
	return 1;
}
