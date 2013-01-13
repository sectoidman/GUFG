#include "window.h"
#include "harness.h"
#include "thing.h"
int main(int argc, char * argv[])
{
	window mainWin;
	harness joy;
	thing object;
	if( !mainWin.screenInit() ){
		fprintf( stderr, "Failed to initialize\n" );
		return -1;
	}
	joy.init();
	object.readModel();
	while(!joy.gameover){
		joy.readInput();
		object.draw();
	}
	return 1;
}
