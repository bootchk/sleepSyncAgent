
#include <cassert>

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
 */

// private data member
namespace {
uint8_t* radioBuffer;
}

SyncMessage Serializer::inwardCommonSyncMsg;

SyncMessage Serializer::outwardCommonSyncMsg;
WorkMessage Serializer::outwardCommonWorkMsg;


void Serializer::init(uint8_t * aRadioBuffer)
{
	radioBuffer = aRadioBuffer;
}

Message* Serializer::unserialize() {
	// assert valid data in radioBuffer, of proper length
	Message * result;
	if (radioBuffer[0] == Sync) {
		unserializeSyncIntoCommon(Sync);
		result = &inwardCommonSyncMsg;
	}
	else if (radioBuffer[0] == AbandonMastership) {
		unserializeSyncIntoCommon(AbandonMastership);
		result = &inwardCommonSyncMsg;
	}
	/* FUTURE
	else if (radioBuffer[0] == Work) {
		unserializeWorkIntoCommon();
		result = &inwardCommonWorkMsg;
	}
	*/
	else {
		// Unexpected or garbled message type
		result = nullptr;
	}
	// assert validity of SystemID and offset have not been checked
	// TODO check offset is in range that scheduling can handle
	return result;
}

void Serializer::unserializeSyncIntoCommon(MessageType aType) {
	assert(aType == Sync || aType == AbandonMastership);
	inwardCommonSyncMsg.type = aType;
	unserializeMasterIDIntoCommon();
	unserializeOffsetIntoCommon();
}



// Overloaded

uint8_t* Serializer::serialize(WorkMessage& msg) {
	//TODO
	return (uint8_t*) 0;
}

void Serializer::serialize(SyncMessage& msg) {
	radioBuffer[0] = Sync;
	serializeMasterIDCommonIntoStream(msg);
	serializeOffsetCommonIntoStream(msg);
}


// Matched pairs
void Serializer::unserializeMasterIDIntoCommon() {
	for (int i =1; i<7; i++)
			((uint8_t *) &inwardCommonSyncMsg.masterID)[i] = radioBuffer[i];
}
void Serializer::serializeMasterIDCommonIntoStream(SyncMessage& msg) {
	for (int i =1; i<7; i++)
		radioBuffer[i] = ((uint8_t *) &msg.masterID)[i] ;
}


void Serializer::unserializeOffsetIntoCommon() {
	for (int i =7; i<11; i++)
		((uint8_t *) &inwardCommonSyncMsg.offset)[i] = radioBuffer[i];
}
void Serializer::serializeOffsetCommonIntoStream(SyncMessage& msg) {
	for (int i =7; i<11; i++)
		radioBuffer[i] = ((uint8_t *) &msg.offset)[i] ;
}

