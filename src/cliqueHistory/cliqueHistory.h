
#pragma once

#include <platformTypes.h>	// SystemID

// From embeddedMath
#include <timeMath.h>	// LongTime, DeltaTime

/*
 * History of clique masters.
 *
 * Always includes one, when unit was its own master.
 *
 * currentCliqueIndex is into the history
 */

class CliqueHistory {
public:

	static void init();

	/*
	 * No result if exhaust storage.
	 */
	static void add(SystemID,  DeltaTime offsetToNextClique);


	/*
	 * Point current clique at next one prior.
	 * Illegal to call if current clique is self.
	 */
	static void setCurrentCliqueRecordToFormerClique();

	// Illegal to call except when there was a former clique (at least one, with self Master of own clique)
	static bool isCurrentCliqueRecordSelf();

	static SystemID currentCliqueRecordMasterID();

	/*
	 * Return a DeltaTime from now to current clique (not the lost one, but one we are going back to.)
	 */
	static DeltaTime offsetToCurrentClique();
};
