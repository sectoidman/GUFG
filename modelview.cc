#include "window.h"
int main(int argc, char * argv[])
{
	window mainWin;
	if( !mainWin.screenInit() ){
		fprintf( stderr, "Failed to initialize\n" );
		return -1;
	}
	while(true);
	return 1;
}
