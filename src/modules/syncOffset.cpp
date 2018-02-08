
#include "syncOffset.h"

#include "../globals.h"	// clique
#include "../clique/clique.h"  // Clique
#include "../physicalParameters.h"


/*
 * Components of sendLatency: see PhysicalParameters.
 * All incurred after we get raw deltaNowToNextSyncPoint
 *  - duration to create message
 *  - duration to serialize
 *  - duration OTA
 */


DeltaTime SyncOffset::calculate(){
	/*
	 * Since we are in sync slot near front of sync period, offset should (0, NormalSyncPeriodDuration)
	 * Type DeltaSync ensures that.
	 */
	DeltaTime rawOffset = clique.schedule.deltaNowToNextSyncPoint();

	/*
	 * Sender specific send latency
	 * The offset will be correct to receiver at the time receiver receives last bit.
	 *
	 * Susceptible to breakpoints: If breakpointed, nextSyncPoint is in past and forwardOffset is zero.
	 * Here we use clampedSubtraction to insure clock subtraction not yield large number.
	 * An alternative design it discussed below: perform simple subtraction and just not send the message
	 * if the result is greater than one SyncPeriodDuration from now.
	 * Again, this situation usually results from debugging.
	 */
	DeltaTime sendLatencyAdjustedOffset = TimeMath::clampedSubtraction(rawOffset, PhysicalParameters::SendLatency);

	// XXX robust code: check sendLatencyAdjustedOffset in range now and return if not
	// XXX rawOffset greater than zero except when breakpointed

	// XXX assert we are not xmitting sync past end of syncSlot?
	// i.e. calculations are rapid and sync slot not too short?

	return sendLatencyAdjustedOffset;
}
