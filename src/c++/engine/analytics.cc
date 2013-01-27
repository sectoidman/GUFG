#include "analytics.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <ctime>
frame::frame(std::vector<bool> ax, std::vector<int> pe, std::vector<bool> ne)
{
	for(int i = 0; i < 4; i++)
		axis.push_back(ax[i]);
	for(unsigned int i = 0; i < pe.size(); i++){
		neg.push_back(ne[i]);
		pos.push_back(pe[i]);
	}
}

replay::replay(const char* filename)
{
	load(filename);
}

void replay::load(const char* filename)
{
	std::ifstream read;
	read.open(filename);
	for(int i = 0; i < 2; i++){
		read >> selection[i];
	}
	while(!read.eof()){
		for(int i = 0; i < 2; i++){
			std::vector<int> pe; int po;
			std::vector<bool> ne; int n;
			std::vector<bool> ax; int a;
			for(int j = 0; j < 4; j++){ read >> a; ax.push_back(a); }
			for(int j = 0; j < 6; j++){ read >> po; pe.push_back(po); }
			for(int j = 0; j < 6; j++){ read >> n; ne.push_back(n); }
			p[i].push_back(new frame(ax,pe,ne));
		}
	}
}

replay::~replay()
{
	write();
}

void replay::write()
{
	std::ofstream scribe;
	scribe << selection[0] << " " << selection[1] << '\n';
	for(unsigned int i = 0; i < p[0].size(); i++){
		for(int j = 0; j < 2; j++){
			for(unsigned int k = 0; k < p[j][i]->axis.size(); k++) scribe << p[j][i]->axis[k] << " ";
			for(unsigned int k = 0; k < p[j][i]->pos.size(); k++) scribe << p[j][i]->pos[k] << " ";
			for(unsigned int k = 0; k < p[j][i]->neg.size(); k++) scribe << p[j][i]->neg[k] << " ";
			scribe << '\n';
		}
	}
	scribe.close();
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
	if(!fresh){
		for(unsigned int i = 0; i < wins.size(); i++){
			read.ignore(200, ',');
			for(unsigned int j = 0; j < wins.size(); j++){
				read >> wins[i][j];
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
