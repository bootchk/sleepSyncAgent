
#include "clique.h"

/*
 * Part of clique implementation.
 */

void Clique::scatterSync(){

	grabMastership();
	/*
	 * Grabbing does not change schedule, so do that now
	 */
	schedule.adjustWithRandomAddedTime();
}
