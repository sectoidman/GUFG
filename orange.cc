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

avatar * orange::spawn(action * source)
{
	avatar * n = source->spawn();
	if(!n) return NULL;
	else {
		avatar ** temp; 
		if(matrixComplexity < 6) temp = new avatar*[matrixComplexity+1];
		else{
			temp = new avatar*[6];
			matrix[5]->dead = true;
		}
		int i;
		for(i = matrixComplexity; i > 0; i--){
			if(i != 6) temp[i] = matrix[i - 1];
		}
		temp[0] = n;
		if(matrix) delete [] matrix;
		matrix = temp;
		return matrix[0];
	}
}
