/*Copyright Somnambulent Studios 2012-2013*/
#include "action.h"
#include <SDL/SDL.h>
#include <cstring>
#include <fstream>
#include <iostream>
//#include <stdio.h>
#include <string>
#include <vector>

#ifndef ___move_trie
#define ___move_trie

class actionTrie{
public:
	actionTrie();
	actionTrie(action*);
	actionTrie(action*, int);
	~actionTrie();
	action * actionHook(int[40], int, int, vector<int>, vector<int>, action *, SDL_Rect&, int&, int&);
	actionTrie * child[10];
	vector<action*> fish;
	actionTrie * insert(int);
	actionTrie * insert(int, action*);
	void insert(action *, int);
	void insert(action *, string);
	unsigned int patternMatch(int[], bool[]);
private:
	vector<int> pattern;
};
#endif
