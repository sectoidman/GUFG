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

chart::chart(int size)
{
	std::vector<int> row;
	for(int i = 0; i < size + 1; i++){
		for(int j = 0; j < size + 1; j++){
			row.push_back(0);
		}
		wins.push_back(row);
	}
}

float chart::matchup(int a, int b)
{
	return (float)wins[a][b] / ((float)wins[b][a] + (float)wins[a][b]);
}

void chart::recordWin(int a, int b)
{
	wins[a][b]++;
}

void chart::init()
{
	std::ifstream read;
	int check;
	char buffer[500];
	char* token;
	bool fresh = false;
	read.open(".data/.charts.csv");
	if(read.fail()) fresh = true;
	read >> check;
	if(check != wins.size()) fresh = true;
	for(unsigned int i = 0; i < wins.size(); i++){
		if(!fresh){
			read.get(buffer, 400, '\n'); read.ignore();
			token = strtok(buffer, "\n,");
		}
		for(unsigned int j = 0; j < wins.size(); j++){
			if(!fresh){
				if(i != j) token = strtok(NULL, "\n, ");
				wins[i][j] = atoi(token);
			}
		}
	}
	read.close();
}

void chart::write()
{
	std::ofstream wr;
	wr.open(".data/.charts.csv");
	wr << wins.size() << '\n' << " ";
	for(unsigned int i = 0; i < wins.size(); i++){
		wr << ",";
		wr << i;
	}
	wr << "\n";
	for(unsigned int i = 0; i < wins.size(); i++){
		wr << i;
		for(int j = 0; j < wins.size(); j++){
			wr << ",";
			if(i == j) wr << "-";
			else wr << wins[i][j];
		}
		wr << "\n";
	}
	wr.close();
}

chart::~chart()
{
	write();
}
