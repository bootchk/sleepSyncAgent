
#pragma once

#include <radioSoC.h>	// LongTime, SystemID, WorkPayload
#include "../modules/deltaSync.h"
#include "../network/granularity.h"


/*
 * !!! Message IS a singleton class.
 * Only one message at a time is being processed.
 * The receiver is never active until the current received message has been processed.
 * The transmitter is never active until we are done with the received message.
 *
 * MessageFactory owns one instance.
 *
 * FUTURE: many heap instances, queued.

 * SyncAgent level message, carried as payload in radio messages
 *
 * Current design: WorkMsg is not a separate class,
 * just a SyncMsg having distinct type and carrying an initialized WorkPayload
 *
 * I use 'acceleration' to mean delta of delta: how much a SyncPeriod is changing.
 * Acceleration from a MasterSync is small: deltaToNextSyncPoint is not much different from local time to next SyncPoint
 * Acceleration from a MergeSync is large: deltaToNextSyncPoint is much different from SyncPeriodDuration
 *
 * Subtypes of type Sync, but not separate classes
 * - MergeSync, offset has large acceleration
 * - MasterSync, offset has small acceleration
 * - AbandonMastership, offset is unused
 * - WorkSync, work payload is initialized
 *
 * The WorkSync subtype helps achieve sync, but has an extra field "work" that is not used by other subtypes.
 * In an earlier design, the "offset" field was a union, used to carry "work".
 * An alternative design is a distinct WorkMessage class.
 */

/*
 * Encoding for OTA type
 */
enum class MessageType {
	// Carry sync
	MasterSync = 1,	// Don't start at 0
	WorkSync,
	ControlNetGranularity,
	ControlScatterClique,

	// Merging, also carrying an offset
	EnticingInferior,	//  5  Was MergeSync
	MasterMergedAway,
	SlaveMergedAway,

	AbandonMastership,	// 8

	// From app controller
	//WorkSetProximity = 139,
	//WorkScatterTime = 151,

	Info,

	Invalid
};



/*
 *  Message data.  Just a struct, no methods.
 *
 *  The implementation does NOT have subclasses by message type.
 *  All knowledge about the validity and interpretation of the fields is in MessageFactory class
 *  and spread through the design.
 *  The design choice to not use classes for message types is simpler and avoid overhead for virtual methods, etc.
 *
 *  !!! Note the order and packing of fields in struct is not necessarily order of data in serialized OTA.
 *  The size of the fields is nearly the same as OTA, but e.g. a 24-bit OTA field could be stored in 32-bit field of struct.
 *
 * MasterSync
 * Usual sync from a unit in Master role.
 * DeltaToNextSyncPoint is typically small.
 *
 *
 * MergeSync
 * Sync from unit in Merger role (master or slave) requesting other clique change its sync time.
 * DeltaToNextSyncPoint is typically but not always large, more than one slot duration.
 *
 *
 * WorkMessage
 * Carries a unit of work.
 * Also helps to maintain sync.
 * I.E. a work field is piggybacked onto a sync carrying message.
 * Only the work field is received by app.
 * When a slave sends WorkSync, it sends its own sync offset (not from the master)
 * and thus may send the clique drift in a new direction
 * (especially if the master doesn't hear it?  TO BE EXPLORED.)
 *
 *
 * AbandonMastershipMessage
 * Dying breath message from master which is power failing.  deltaToNextSyncPoint is moot.
 */


class SyncMessage{
public:
	/*
	 * TOA is captured by Radio.
	 * It is not an attribute of SyncMessage.
	 */

	/*
	 * Calculated property.
	 * Time on local LongClock when sender started transmit.
	 */
	static LongTime timeOfTransmittal();
	static char const * representation(SyncMessage*);


	// These fields are sent over the air OTA
	MessageType type;
	DeltaSync deltaToNextSyncPoint;	// forward in time
	SystemID masterID;
	WorkPayload work;	// work always allocated/transmitted, but often empty/null

	NetGranularity transmittedSignalStrength;

	static MessageType messageTypeFromRaw(unsigned char);

	/*
	 * Does this message type have property "carries sync":
	 * - non-null offset?
	 * - non-null MasterID
	 *
	 * AbandonMastership may have null offset
	 */
	static bool doesCarrySync(MessageType type);
};


