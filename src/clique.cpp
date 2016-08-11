
#include "clique.h"


void Clique::reset(){
	masterID = myID;
}

bool Clique::isSelfMaster() {
	return masterID == myID;
}
