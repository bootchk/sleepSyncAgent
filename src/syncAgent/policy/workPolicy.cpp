
#include "workPolicy.h"

#include "../globals.h"    // clique

bool WorkPolicy::shouldXmitWorkSync() {
	return clique.isSelfMaster();
}

