
#include "workIn.h"


namespace {
	static bool heardWork = false;
}



void WorkIn::resetState() {
	heardWork = false;
}

void WorkIn::hearWork() {
	heardWork = true;
}

bool WorkIn::isHeardWork() {
	return heardWork;
}
