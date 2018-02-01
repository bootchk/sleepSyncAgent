
#include "provisionManager.h"

namespace {
	int counter = 0;
}




/*
 * Provision periodically, regularly.
 */
bool ProvisionManager::shouldProvision() {
	bool result = false;

	counter += 1;
	if (counter > 5) {
		counter = 0;
		result = true;
	}
	return result;
}
