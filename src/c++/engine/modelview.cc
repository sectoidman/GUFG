#include "gameinstance.h"
#include "thing.h"
#include <time.h>
int main(int argc, char * argv[])
{
	gameInstance game;
	thing object;
	game.init();
	if(argc > 1) object.readModel(argv[1]);
	else object.readModel();
	while(!game.gameover){
		game.readInput();
		game.draw(object);
	}
	return 1;
}
