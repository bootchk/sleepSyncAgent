#pragma once

#include <inttypes.h>
#include <types.h>
#include "message.h"


/*
 * Serialize: convert data stream to object.
 *
 * Knows order and size of data elements in stream (message payload.)
 * type: 1
 * masterID: 6
 * syncOffset: 3   (OSTime is 24-bit. 2 is max of 128k ticks)
 *
 * !!! This assumes:
 * - the radio is half-duplex (can't xmit and receive at the same time)
 * - SyncAgent is single message at a time (wireless stack does not chain or auto-send e.g. acks)
 * Thus:
 * - knows one radioBuffer, i.e. pointer to data is not passed to methods.
 * - owns common Messages
 *
 * FUTURE: use queues and multiple live messages
 */

class Serializer {
public:
	static SyncMessage inwardCommonSyncMsg;
	static SyncMessage outwardCommonSyncMsg;
	static WorkMessage outwardCommonWorkMsg;

	// Sync messages are constant length.
	//static const uint8_t OnAirSyncMsgPayloadLength = 10;


	// Methods
	static void init(BufferPointer radioBuffer, uint8_t aBufferSize);

	static Message* unserialize();
	// overloaded
	static uint8_t* serialize(WorkMessage& msg);
	static void serialize(SyncMessage& msg);

private:
	static void unserializeIntoCommonSyncMessage();

	static void unserializeMasterIDIntoCommon();
	static void serializeMasterIDCommonIntoStream(SyncMessage& msg);

	static void unserializeOffsetIntoCommon();
	static void serializeOffsetCommonIntoStream(SyncMessage& msg);
};
