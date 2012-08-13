#include "interface.h"
orange::orange()
{
	head = new actionTrie;
	airHead = new actionTrie;
	build("Orange", "Orange");
}

void orange::init(action *& cMove)
{
	character::init(cMove);
	matrixComplexity = 0;
	matrix = NULL;
}

instance * orange::spawn(action * source)
{
	instance * n = source->spawn();
	if(!n) return NULL;
	else {
		instance ** temp; 
		if(matrixComplexity < 7) temp = new instance*[matrixComplexity+1];
		else{
			temp = new instance*[7];
			matrix[6]->dead = true;
		}
		int i;
		for(i = matrixComplexity; i > 0; i--){
			if(i != 7) temp[i] = matrix[i - 1];
		}
		temp[0] = n;
		if(matrix) delete [] matrix;
		matrix = temp;
		return matrix[0];
	}
}
