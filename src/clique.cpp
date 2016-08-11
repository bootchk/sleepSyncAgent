
#include "clique.h"
#include "os.h"


// static
Schedule Clique::schedule;


void Clique::reset(){
	masterID = myID();
}

bool Clique::isSelfMaster() {
	return masterID == myID();
}
