#include "player.h"
daemon::daemon(){
	init();
}

void genEvent(bool *& sAxis, int *& posEdge, bool *& negEdge) {
	for(int i = 0; i < 5; i++){
		if(i < 4) sAxis[i] = rand()%2;
		posEdge[i] = rand()%2;
		if(!posEdge[i]) negEdge[i] = rand()%2;
		else negEdge[i] = 0;
	}
}
