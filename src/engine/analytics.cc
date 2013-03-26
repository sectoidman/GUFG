#include "analytics.h"
#include <ctime>
#include <fstream>
#include <iostream>

using std::ifstream;
using std::ofstream;

void script::init(int players)
{
	for(int i = 0; i < players; i++){
		selection.push_back(-1);
		vector<frame> tvec;
		command.push_back(tvec);
	}
}

void script::init(vector<int> s)
{
	for(unsigned int i = 0; i < s.size(); i++){
		selection.push_back(s[i]);
		vector<frame> tvec;
		command.push_back(tvec);
	}
}

bool script::test()
{
	frame l;
	for(int i = 0; i < 4; i++)
		l.axis.push_back(0);
	for(int i = 4; i < 6; i++){
		l.buttons.push_back(0);
	}
	l.n.i = 0;
	l.n.raw.dir = 5;
	return genEvent(0, 0, l);
}

bool script::genEvent(int p, int f, frame &t)
{
	if(command.empty() || (unsigned int)p >= command.size()) return 0;
	if(command[p].empty() || (unsigned int)f >= command[p].size()) return 0;

	//printf("%i: %i\n", p, command[p][f].n.i);
	t.n.i = command[p][f].n.i;
	return 1;
}

void script::push(frame t)
{
	push(0, t);
}

void script::push(int p, frame t)
{
	command[p].push_back(t);
}

script::script(string filename)
{
	load(filename);
}

void script::load(string filename)
{
	ifstream read;
	read.open(filename);
	if(read.fail()){
		init(1); return;
	}
	int players, buttons, s;
	read >> players; read >> buttons;
	for(int i = 0; i < players; i++){
		read >> s;
		selection.push_back(s);
	}
	for(int i = 0; i < players; i++){
		vector<frame> t;
		command.push_back(t);
	}
	while(!read.eof()){
		for(int i = 0; i < players; i++){
			frame temp;
			read >> temp.n.i;
			command[i].push_back(temp);
		}
	}
}

void script::write()
{
	write("");
}

void script::write(string name)
{
	ofstream scribe;
	if(name.size()) scribe.open(name);
	else{
		time_t now = time(0);
		tm* lT = localtime(&now);
		scribe.open(string(".data/replays/"+string(asctime(lT))));
	}
	if(command[0].size() == 0){
		scribe.close();
		return;
	}
	scribe << selection.size() << " " << command[0][0].buttons.size() << '\n';
	for(unsigned int i = 0; i < selection.size(); i++){
		scribe << selection[i];
		if(i == 0) scribe << " ";
	}
	scribe << '\n';
	for(unsigned int i = 0; i < command[0].size(); i++){
		for(unsigned int j = 0; j < command.size(); j++){
			scribe << command[j][i].n.i;
		}
		scribe << '\n';
	}
	scribe << '\n';
	scribe.close();
}

script::~script(){}

chart::chart(int size)
{
	vector<int> row;
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
	ifstream read;
	unsigned int check;
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
	ofstream wr;
	wr.open(".data/.charts.csv");
	wr << wins.size() << '\n' << " ";
	for(unsigned int i = 0; i < wins.size(); i++){
		wr << ",";
		wr << i;
	}
	wr << "\n";
	for(unsigned int i = 0; i < wins.size(); i++){
		wr << i;
		for(unsigned int j = 0; j < wins.size(); j++){
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
