
#include "workPolicy.h"

#include "../globals.h"    // clique

#include "../clique/clique.h"

bool WorkPolicy::shouldXmitWorkSync() {
	return clique.isSelfMaster();
}

