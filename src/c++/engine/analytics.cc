#include "analytics.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <ctime>

frame::frame()
{
	next = NULL;
}

frame::frame(std::vector<bool> ax, std::vector<int> pe, std::vector<bool> ne)
{
	for(int i = 0; i < 4; i++)
		axis.push_back(ax[i]);
	for(unsigned int i = 0; i < pe.size(); i++){
		neg.push_back(ne[i]);
		pos.push_back(pe[i]);
	}
	next = NULL;
}

void frame::cull()
{
	if(next){ 
		next->cull(); 
		delete next;
	}
	next = NULL;
}

void frame::append(frame * n)
{
	next = n;
}

replay::replay()
{
	start[0] = NULL;
	current[0] = NULL;
	fcounter = 0;
}

replay::replay(int p1, int p2, int r)
{
	start[0] = NULL;
	current[0] = NULL;
	selection[0] = p1;
	selection[1] = p2;
	rcount = r;
	fcounter = 0;
}

replay::replay(const char* filename)
{
	start[0] = NULL;
	current[0] = NULL;
	load(filename);
}

void replay::append(frame * p1, frame * p2)
{
	if(!start[0]){ 
		start[0] = p1;
		start[1] = p2;
	} else { 
		current[0]->append(p1);
		current[1]->append(p2);
	}
	current[0] = p1;
	current[1] = p2;
	fcounter++;
}

void replay::load(const char* filename)
{
	std::ifstream read;
	int tempInt;
	bool tempBool;
	read.open(filename);
	frame * iterator[2];
	for(int i = 0; i < 2; i++){
		if(start[i]) start[i]->cull();
		else start[i] = new frame;
		iterator[i] = start[i];
	}
	read >> selection[0] >> selection[1] >> fcounter >> rcount;
	for(int i = 0; i < fcounter; i++){
		for(int i = 0; i < 2; i++){
			for(int j = 0; j < 5; j++){
				if(j < 4){ 
					read >> tempBool;
					iterator[i]->axis.push_back(tempBool);
				}
				read >> tempBool;
				iterator[i]->neg.push_back(tempBool);
				read >> tempInt;
				iterator[i]->pos.push_back(tempInt);
			}
			iterator[i]->next = new frame;
			iterator[i] = iterator[i]->next;
		}
	}
	for(int i = 0; i < 2; i++) iterator[i]->cull();
	read.close();
}

void replay::write()
{
	std::ofstream scribe;
	frame * iterator[2];
	for(int i = 0; i < 2; i++)
		iterator[i] = start[i];
	char buffer[100];
	time_t currtime;
	time(&currtime);
	sprintf(buffer, ".data/replays/%s.frp", asctime(localtime(&currtime)));
	scribe.open(buffer);
	scribe << selection[0] << " " << selection[1] << " " << fcounter << " " << rcount << "\n";
	do{
		for(int i = 0; i < 2; i++){
			for(unsigned int j = 0; j < iterator[i]->pos.size(); j++){
				if(j < 4) scribe << iterator[i]->axis[j] << " ";
				scribe << iterator[i]->pos[j] << " ";
				scribe << iterator[i]->neg[j] << " ";
			}
			iterator[i] = iterator[i]->next;
		}
		scribe << "\n";
	} while (iterator[0] != NULL);
	scribe.close();
}

replay::~replay()
{
	if(start[0]){
		start[0]->cull();
		start[1]->cull();
	}
}

