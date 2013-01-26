#include "session.h"
#include "thing.h"
#include <time.h>
int main(int argc, char * argv[])
{
	session game;
	model object;
	game.init();
	if(argc > 1) object.readModel(argv[1]);
	else object.readModel();
	while(!game.gameover){
		game.readInput();
		game.draw(object);
	}
	return 1;
}
