#include <stdio.h>
#include <cmath>
#include <cstring>
#include <iostream>
#include <fstream>
#include <assert.h>
#include <cstdlib>
#include "character.h"
using namespace std;

void character::build(char* n)
{
	char buffer[101];
	char buffer2[101];
	char name[51];
	char moveName[151];
	char type;
	moveTrie * t = NULL;
	move * m = NULL;
	char component;
	bool commentFlag;
	char * token;
	int q;
	ifstream read; 
	sprintf(buffer, "%s/%s.ch", n, n);
	
	read.open(buffer);
	assert(!read.fail());
	read.get(name, 50); read.ignore(100, '\n');
	while(!read.eof()){
		commentFlag = 0;
		read.get(buffer, 100, '\n'); read.ignore(100, '\n');
		type = buffer[0];
		if(type == '#' || type == '\0')
			commentFlag = 1;
		if(!commentFlag){
			strcpy(buffer2, buffer);
			token = strtok(buffer, " \t-%\n");
			sprintf(moveName, "%s/%s", name, token);
			switch(type){
			case '%':
				m = new special(moveName);
				break;
			case '-':
				m = new utility(moveName);
				break;
			default:
				m = new move(moveName);
				break;	
			}
			token = strtok(buffer2, " \t-%\n");
			while (token){
				token = strtok(NULL, " \t=-%\n");
				if(token) {
					switch (token[0]){
					case 'h':
						t = head;
						break;
					case 'a':
						t = airHead;
						break;
					default:
						break;
					}
					for(int i = strlen(token)-1; i > 0; i--){
						component = token[i];
						q = atoi(&component);
						t = t->insert(q);
					}
					t->insert(m);
				}
			}
		}
	}
	read.close();	
}
