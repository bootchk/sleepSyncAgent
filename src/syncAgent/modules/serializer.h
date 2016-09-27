#pragma once

#include <inttypes.h>
#include "message.h"

class Serializer {

public:
	// Sync messages are constant length.
	// type: 1
	// master: 6
	// offset: 4

	static const uint8_t OnAirSyncMsgPayloadLength = 11;


public:
	static Message* unserialize(void* data);
	static uint8_t* serialize(WorkMessage& msg);
	static uint8_t* serialize(SyncMessage& msg);
};
