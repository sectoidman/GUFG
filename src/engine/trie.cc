#include "trie.h"
actionTrie::actionTrie()
{
	for(int i = 0; i < 10; i++)
		child[i] = NULL;
}

actionTrie::actionTrie(action * a, int p)
{
	for(int i = 0; i < 10; i++)
		child[i] = NULL;
	pattern.push_back(p);
	fish.push_back(a);
}

actionTrie::actionTrie(action * a)
{
	for(int i = 0; i < 10; i++)
		child[i] = NULL;
	pattern.push_back(0);
	fish.push_back(a);
}

void actionTrie::insert(action * b, char * p)
{
	char component[2];
	actionTrie * t = this;
	int q, pattern = 0;
	for(int i = strlen(p)-1; i > 0; i--){
		switch(p[i]){
		case 'A':
		case 'B':
		case 'C':
		case 'D':
		case 'E':
			pattern += 1 << (p[i] - 'A');
			break;
		default:
			sprintf(component, "%c", p[i]);
			q = atoi(component);
			if(q > 10) q = q % 10;
			t = t->insert(q);
			break;
		}
	}
	t->insert(b, pattern);
}

void actionTrie::insert(action * b, int p)
{
	fish.push_back(b);
	pattern.push_back(p);
}

actionTrie * actionTrie::insert(int a, action * b)
{
	if(a < 10 && a >= 0){
		if(!child[a]) child[a] = new actionTrie(b);
		else child[a]->insert(b, 0);
		return child[a];
	}
	else return NULL;
}

actionTrie * actionTrie::insert(int a)
{
	if(a < 10 && a >= 0) {
		if(!child[a]) child[a] = new actionTrie();
		return child[a];
	}
	else return NULL;
}

actionTrie::~actionTrie()
{
	for(int i = 0; i < 9; i++){
		if(child[i] != NULL){
			delete child[i];
			child[i] = NULL;
		}
	}
}

action * actionTrie::actionHook(int inputBuffer[30], int i, int first, int * r, std::vector<int> buttons, action * c, SDL_Rect &prox, int &cFlag, int &hFlag)
{
	actionTrie * test = NULL;
	action * result = NULL;
	int j;
	for(j = i; j < 30; j++){
		if(inputBuffer[j] < 10) test = child[inputBuffer[j]];
		if(test != NULL){
			if (first < 0) result = test->actionHook(inputBuffer, j, j, r, buttons, c, prox, cFlag, hFlag);
			else result = test->actionHook(inputBuffer, j, first, r, buttons, c, prox, cFlag, hFlag);
			if(result != NULL) return result;
		}
	}
	if(fish.size() != 0){
		for(unsigned int k = 0; k < fish.size(); k++){
			if(fish[k] != NULL){
				if(fish[k]->activate(buttons, pattern[k], i, first, r, prox) == 1){
					if(fish[k]->cancel(c, cFlag, hFlag)){
						return fish[k];
					}
				}
			}
		}
	}
	return NULL;
}
