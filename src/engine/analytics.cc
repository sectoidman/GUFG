#include "analytics.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <ctime>

frame::frame()
{
	next = NULL;
}

frame::frame(bool a[], int p[], bool n[])
{
	for(int i = 0; i < 5; i++){
		if(i < 4) axis[i] = a[i];
		pos[i] = p[i];
		neg[i] = n[i];
	}
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
}

replay::replay(int p1, int p2)
{
	start[0] = NULL;
	current[0] = NULL;
	selection[0] = p1;
	selection[1] = p2;
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
}

void replay::load(const char* filename)
{
	printf("Stub function: This will load a replay file\n");
}

void replay::write()
{
	std::ofstream scribe;
	char buffer[100];
	time_t time;
	sprintf(buffer, "(%i)v(%i), %s.frp", selection[0], selection[1], asctime(localtime(&time)));
	scribe.open(buffer);
	scribe.close();
	printf("Stub function: This will save a replay file\n");
}

replay::~replay()
{
	if(start[0]){
		start[0]->cull();
		start[1]->cull();
	}
}

