
#include <cassert>

#include "../../platform/platform.h"  // MaxDeltaTime

#include "serializer.h"



/*
 * Static data members.
 * Use common static Messages (not new on the heap), SyncAgent only uses one at a time.
 * Common i.e. shared and reused.
 *
 * Thus many methods have side effects on common Messages,
 * and return pointer to common.
 *
 * Message objects need not be packed.
 * This code does not take a brute force approach (one loop over data into object/struct.)
 * Instead, we loop over msg fields.
 */

// private data member
namespace {
BufferPointer radioBuffer;
uint8_t radioBufferSize;
}

SyncMessage Serializer::inwardCommonSyncMsg;

SyncMessage Serializer::outwardCommonSyncMsg;
WorkMessage Serializer::outwardCommonWorkMsg;


void Serializer::init(BufferPointer aRadioBuffer, uint8_t aBufferSize)
{
	// Knows address, size of radio's buffer
	radioBuffer = aRadioBuffer;
	radioBufferSize = aBufferSize;
}

Message* Serializer::unserialize() {
	// assert valid data in radioBuffer, of proper length
	Message * result;
	if (       (radioBuffer[0] == MasterSync)
			|| (radioBuffer[0] == MergeSync)
			|| (radioBuffer[0] == AbandonMastership)
	)
	{
		unserializeIntoCommonSyncMessage();
		result = &inwardCommonSyncMsg;
	}
	/* FUTURE
	else if (radioBuffer[0] == Work) {
		unserializeWorkIntoCommon();
		result = &inwardCommonWorkMsg;
	}
	*/
	else {
		/*
		 * Unexpected or garbled message type.
		 * This should be rare, since CRC was valid.
		 * It would take multiple bit errors to corrupt message type and still have valid CRC.
		 */
		// Unserialize it so we can debug it
		unserializeIntoCommonSyncMessage();
		assert(false);	// TESTING
		result = nullptr;	// PRODUCTION
	}
	// assert validity of SystemID and offset have not been checked
	// FUTURE check offset is in range that scheduling can handle
	return result;
}

void Serializer::unserializeIntoCommonSyncMessage() {
	// assert(aType == MasterSync || aType == MergeSync || aType == AbandonMastership);
	inwardCommonSyncMsg.type = (MessageType) radioBuffer[0];
	unserializeMasterIDIntoCommon();
	unserializeOffsetIntoCommon();
}



// Overloaded

uint8_t* Serializer::serialize(WorkMessage& msg) {
	//FUTURE
	(void) msg;
	return (uint8_t*) 0;
}

void Serializer::serialize(SyncMessage& msg) {
	radioBuffer[0] = msg.type;
	serializeMasterIDCommonIntoStream(msg);
	serializeOffsetCommonIntoStream(msg);
}


// TODO Constants for field lengths and addresses
// Matched pairs
void Serializer::unserializeMasterIDIntoCommon() {
	assert(sizeof(inwardCommonSyncMsg.masterID)>=6);
	inwardCommonSyncMsg.masterID = 0; // ensure MSB two bytes are zero.
	for (int i =1; i<7; i++)
		// Fill LSB 6 bytes of a 64-bit
		((uint8_t *) &inwardCommonSyncMsg.masterID)[i] = radioBuffer[i];
}
void Serializer::serializeMasterIDCommonIntoStream(SyncMessage& msg) {
	for (int i =1; i<7; i++)
		// Send LSB 6 bytes of 64-bit
		radioBuffer[i] = ((uint8_t *) &msg.masterID)[i] ;
}


void Serializer::unserializeOffsetIntoCommon() {
	assert(sizeof(inwardCommonSyncMsg.deltaToNextSyncPoint)==4);
	inwardCommonSyncMsg.deltaToNextSyncPoint = 0;	// ensure MSB byte is zero
	for (int i =7; i<radioBufferSize; i++)
		// FUTURE, code for 32-bit OSClock
		// 24-bit offset:
		// Little-endian into LSB three bytes of a 32-bit OSTime
		((uint8_t *) &inwardCommonSyncMsg.deltaToNextSyncPoint)[i] = radioBuffer[i];

	assert(inwardCommonSyncMsg.deltaToNextSyncPoint < MaxDeltaTime);
}
void Serializer::serializeOffsetCommonIntoStream(SyncMessage& msg) {
	for (int i =7; i<radioBufferSize; i++)
		radioBuffer[i] = ((uint8_t *) &msg.deltaToNextSyncPoint)[i] ;
}

