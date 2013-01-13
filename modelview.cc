#include "window.h"
#include "harness.h"
int main(int argc, char * argv[])
{
	window mainWin;
	harness joy;
	if( !mainWin.screenInit() ){
		fprintf( stderr, "Failed to initialize\n" );
		return -1;
	}
	joy.init();
	while(true);
	return 1;
}
