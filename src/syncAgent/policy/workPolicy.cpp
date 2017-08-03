
#include "workPolicy.h"

#include "../globals.h"    // clique

#include "../modules/clique.h"

bool WorkPolicy::shouldXmitWorkSync() {
	return clique.isSelfMaster();
}

