
#include <cassert>
#include <cstring>	// memcpy

#include "../../platform/platform.h"  // MaxDeltaTime

#include "serializer.h"

/*
 * Implementation notes:
 *
 * Message objects need not be packed.
 * This code does not take a brute force approach (one loop over data into object/struct.)
 * Instead, we loop over msg fields.
 *
 * Use common static Messages (not new on the heap), SyncAgent only uses one at a time.
 * Common means: shared and reused, as in Fortran.
 * Thus many methods have side effects on common Messages,
 * and return pointer to common.
 */


// static data members
BufferPointer Serializer::radioBufferPtr;
uint8_t Serializer::radioBufferSize;

SyncMessage Serializer::inwardCommonSyncMsg;
SyncMessage Serializer::outwardCommonSyncMsg;
//FUTURE WorkMessage Serializer::outwardCommonWorkMsg;


void Serializer::init(BufferPointer aRadioBuffer, uint8_t aBufferSize)
{
	// Knows address, size of radio's buffer
	radioBufferPtr = aRadioBuffer;
	radioBufferSize = aBufferSize;
}

SyncMessage* Serializer::unserialize() {
	// assert valid data in radioBuffer, of proper length
	SyncMessage * result;

	// Minor optimization: only access radioBufferPtr[0] once.
	// It is volatile, which prevents compiler from optimizing repeated references.
	if (SyncMessage::isByteASyncType(radioBufferPtr[0]))
	{
		unserializeIntoCommonSyncMessage();
		result = &inwardCommonSyncMsg;
	}
	/* FUTURE
	else if (radioBufferPtr[0] == Work) {
		unserializeWorkIntoCommon();
		result = &inwardCommonWorkMsg;
	}
	*/
	else {
		/*
		 * Unexpected or garbled message type.
		 * This should be rare, since CRC was valid.
		 * It would take multiple bit errors to corrupt message type and still have valid CRC?
		 * Or does preamble, address, all zeroes have a correct CRC?
		 */
		// Unserialize it so we can debug it
		// unserializeIntoCommonSyncMessage();
		//assert(false);	// TESTING
		result = nullptr;	// PRODUCTION
	}
	// assert validity of SystemID and offset have not been checked
	// FUTURE check offset is in range that scheduling can handle
	return result;
}


bool Serializer::bufferIsSane(){
	// FUTURE or work message
	// FUTURE other validity checks?
	return SyncMessage::isByteASyncType(radioBufferPtr[0]);
}


void Serializer::unserializeIntoCommonSyncMessage() {
	// assert(aType == MasterSync || aType == MergeSync || aType == AbandonMastership);
	inwardCommonSyncMsg.type = (MessageType) radioBufferPtr[0];
	unserializeMasterIDIntoCommon();
	unserializeOffsetIntoCommon();
}



// Overloaded

#ifdef FUTURE
uint8_t* Serializer::serialize(WorkMessage& msg) {
	//FUTURE
	(void) msg;
	return (uint8_t*) 0;
}
#endif


// suppress compiler warning for pointer arith
#pragma GCC diagnostic ignored "-Wpointer-arith"

void Serializer::serializeOutwardCommonSyncMessage() {
	radioBufferPtr[0] = outwardCommonSyncMsg.type;
	serializeMasterIDCommonIntoStream(outwardCommonSyncMsg);
	serializeOffsetCommonIntoStream(outwardCommonSyncMsg);
}


// TODO Constants for field lengths and addresses
// Matched pairs
void Serializer::unserializeMasterIDIntoCommon() {
	assert(sizeof(inwardCommonSyncMsg.masterID)>=6);
	inwardCommonSyncMsg.masterID = 0; // ensure MSB two bytes are zero.
	// Fill LSB 6 bytes of a 64-bit
	memcpy( (void*) &inwardCommonSyncMsg.masterID,	// dest
			(void*) radioBufferPtr + 1,	// src
			6);	// count
}

void Serializer::serializeMasterIDCommonIntoStream(SyncMessage& msg) {
	// Send LSB 6 bytes of 64-bit
	memcpy( (void*) radioBufferPtr + 1, 	// dest
			(void*) &msg.masterID,	// src
			6);	// count
}


void Serializer::unserializeOffsetIntoCommon() {
	assert(sizeof(inwardCommonSyncMsg.deltaToNextSyncPoint)==4);
	// 24-bit offset:
	// Little-endian into LSB three bytes of a 32-bit OSTime
	inwardCommonSyncMsg.deltaToNextSyncPoint = 0;	// ensure MSB byte is zero
	memcpy( (void*) &inwardCommonSyncMsg.masterID, 	// dest
			(void*) radioBufferPtr + 7,	// src
			3);	// count

	// FUTURE, code for 32-bit OSClock
	assert(inwardCommonSyncMsg.deltaToNextSyncPoint < MaxDeltaTime);
}

void Serializer::serializeOffsetCommonIntoStream(SyncMessage& msg) {
	memcpy( (void*) radioBufferPtr + 7, 	// dest
			(void*) &msg.masterID,	// src
			3);	// count
}


// End of pointer arith
#pragma GCC diagnostic pop

