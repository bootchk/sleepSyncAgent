
#pragma once

#include <platformTypes.h>	// SystemID

// From embeddedMath
#include <timeMath.h>	// LongTime, DeltaTime

/*
 * History of clique masters.
 *
 * Always includes one, when unit was its own master.
 *
 * "former" clique is a pointer into the history
 */

class CliqueHistory {
public:

	/*
	 * No result if exhaust storage.
	 */
	// TODO call to this
	// TODO implement this
	static void add();


	/*
	 * Point former clique at next one prior.
	 * No effect if is no prior.
	 */
	static void back();

	static bool isFormerCliqueSelf();

	static SystemID formerCliqueMasterID();
	static DeltaTime offsetToFormerClique();
};
