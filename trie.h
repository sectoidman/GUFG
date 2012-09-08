/*Header file for "action" class in project Ground Up Fighting Game
 *
 *Written by Alex Kelly in 2012.
 *Licensed under MIT OSI, see "COPYING" for details
 */


#include <SDL/SDL.h>
#include <cstring>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include "action.h"
#include "compat.h"

#ifndef MOVETRIE
#define MOVETRIE
/*Move trie class. Essentially a recursive trie-search algorithm that looks for input hooks*/

class actionTrie{
public:
	actionTrie();
	actionTrie(action*);
	actionTrie(action*, char*);
	~actionTrie();
	action * actionHook(int[40], int, int, int*, bool[5], bool[5], action *, SDL_Rect&, int&, int&);
	actionTrie * child[10];
	action ** fish;       //Because it's what a hook catches! Okay, this name probably needs to change.
	actionTrie * insert(int);
	actionTrie * insert(int, action*);
	void insert(action *);
	int occupants;
private:
	int * button;
};
#endif
