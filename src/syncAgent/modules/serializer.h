#pragma once

#include <inttypes.h>
#include "message.h"

class Serializer {

	// Payload of an app's message sent using BT Broadcast/Observer (sic advertise)
	static constexpr uint8_t advertData[14] =
	{
	  // Flags; this sets the device to use limited discoverable
	  // mode (advertises for 30 seconds at a time) instead of general
	  // discoverable mode (advertises indefinitely)
	  0x02,   // length of this data
	  66, // GAP_ADTYPE_FLAGS,
	  66, // GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED,

	  // three-byte broadcast of the data "1 2 3"
	  0x0A,   // length of this data including the data type byte
	  66, // GAP_ADTYPE_MANUFACTURER_SPECIFIC, // manufacturer specific adv data type
	  1,	// app MessageType
	  2,	// app MasterID, 6 bytes, 48-bits same as MAC
	  3,
	  4,
	  5,
	  6,
	  7,
	  8,	// app Offset, 2 bytes, 128k
	  9
	};

public:
	static Message* unserialize(void* data);
	static uint8_t* serialize(WorkMessage& msg);
	static uint8_t* serialize(SyncMessage& msg);
};
