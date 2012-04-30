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
	char name[51];
	char moveName[151];
	char type[20];
	char * trie = 0;
	char * head = "(ground)";
	char * air = "(aerial)";
	char component;
	bool commentFlag;
	char * token;
	ifstream read; 
	sprintf(buffer, "%s.ch", n);
	
	read.open(buffer);
	assert(!read.fail());
	read.get(name, 50); read.ignore(100, '\n');
	while(!read.eof()){
		commentFlag = 0;
		strcpy(type, "Normal");
		read.get(buffer, 100, '\n'); read.ignore(100, '\n');
		switch(buffer[0]){
		case '#': 
		case '\0':
			commentFlag = 1;
			break;
		case '%':
			strcpy(type, "Special");
			break;
		case '-':
			strcpy(type, "Utility");
			break;
		default:
			strcpy(type, "Normal");
			break;
		}
		if(!commentFlag){
			token = strtok(buffer, " \t-%");
			sprintf(moveName, "%s/%s", name, token);
			printf("%s (%s)\n", moveName, type);
			printf ("Hooks: ");
			while (token){
				token = strtok(NULL, " \t=-%");
				if(token) {
					switch (token[0]){
					case 'h':
						trie = head;
						break;
					case 'a':
						trie = air;
						break;
					default:
						break;
					}
					for(int i = strlen(token)-1; i > 0; i--){
						component = token[i];
						printf("%i", atoi(&component));
					}
					printf("%s ", trie);
				}
			}
			printf("\n");
		}
	}
	read.close();	
}
