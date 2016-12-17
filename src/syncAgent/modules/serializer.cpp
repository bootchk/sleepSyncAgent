
#include <cassert>
#include <cstring>	// memcpy

#include "serializer.h"

#include "../../platformHeaders/platform.h"  // MaxDeltaTime
#include "otaPacket.h"


/*
 * Implementation notes:
 *
 * Message objects need not be packed.
 * This code does not take a brute force approach (one loop over data into object/struct.)
 * Instead, we loop over msg fields.
 *
 * Use common static Messages (not new'd on the heap), SyncAgent only uses one at a time.
 * Common means: shared and reused, as in Fortran.
 * Thus many methods have side effects on common Messages,
 * and return pointer to common.
 *
 * The count of bytes must match the constant FixedPayloadCount defined in platform/radio.h
 */



// static data members



// Serializer owns inward and returns reference to it
SyncMessage Serializer::inwardCommonSyncMsg;
// Serializer and its outward are globally visible
SyncMessage Serializer::outwardCommonSyncMsg;


// Local to this file

namespace {

BufferPointer radioBufferPtr;
uint8_t radioBufferSize;



// suppress compiler warning for pointer arith
#pragma GCC diagnostic ignored "-Wpointer-arith"


void unserializeWorkIntoCommon() {
	memcpy( (void*) &Serializer::inwardCommonSyncMsg.work,	// dest
			(void*) radioBufferPtr + OTAPayload::WorkIndex,	// src
			OTAPayload::WorkLength);
}
void serializeWorkCommonIntoStream(SyncMessage& msg){
	memcpy( (void*) radioBufferPtr + OTAPayload::WorkIndex, 	// dest
			(void*) &msg.work,	// src
			OTAPayload::WorkLength);
}

// Matched pairs
void unserializeMasterIDIntoCommon() {
	assert(sizeof(Serializer::inwardCommonSyncMsg.masterID)>=OTAPayload::MasterIDLength);
	Serializer::inwardCommonSyncMsg.masterID = 0; // ensure MSB two bytes are zero.
	// Fill LSB 6 bytes of a 64-bit
	memcpy( (void*) &Serializer::inwardCommonSyncMsg.masterID,	// dest
			(void*) radioBufferPtr + OTAPayload::MasterIndex,	// src
			OTAPayload::MasterIDLength);
}

void serializeMasterIDCommonIntoStream(SyncMessage& msg) {
	// Send LSB 6 bytes of 64-bit
	memcpy( (void*) radioBufferPtr + OTAPayload::MasterIndex, 	// dest
			(void*) &msg.masterID,	// src
			OTAPayload::MasterIDLength);
}


void unserializeOffsetIntoCommon() {
	// TODO this assertion must go
	assert(sizeof(Serializer::inwardCommonSyncMsg.deltaToNextSyncPoint)==4);

	/*
	 * Data was received OTA.
	 * To protect against malice broadcasters, and failure of CRC to detect multiple bit errors,
	 * we enforce algorithm's constraints on DeltaSync.
	 */
	// TODO return failure code

	DeltaTime otaDeltaSync;
	// FUTURE, code for 32-bit OSClock
	// 24-bits OTA, little-endian into LSB three bytes of a 32-bit OSTime
	Serializer::inwardCommonSyncMsg.deltaToNextSyncPoint = 0;	// ensure MSB byte is zero
	memcpy( (void*) &otaDeltaSync, 	// dest
			(void*) radioBufferPtr + OTAPayload::OffsetIndex,	// src
			OTAPayload::OffsetLength);	// count

	// TODO range check so we don't get assertion implemented in DeltaSync.set()
	if ( DeltaSync::isValidValue(otaDeltaSync))
		Serializer::inwardCommonSyncMsg.deltaToNextSyncPoint.set(otaDeltaSync);
	// TODO else

	// assert deltaToNextSyncPoint is set to a valid value
}

void serializeOffsetCommonIntoStream(SyncMessage& msg) {
	memcpy( (void*) radioBufferPtr + OTAPayload::OffsetIndex, 	// dest
			(void*) &msg.deltaToNextSyncPoint,	// src
			OTAPayload::OffsetLength);
}

// End of pointer arith
#pragma GCC diagnostic pop

void unserializeIntoCommonSyncMessage() {
	MessageType msgType = (MessageType) radioBufferPtr[0];
	// already assert isReceivedTypeASyncType
	Serializer::inwardCommonSyncMsg.type = msgType;
	unserializeMasterIDIntoCommon();
	unserializeOffsetIntoCommon();
	unserializeWorkIntoCommon();
}


}	// namespace










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
	if (SyncMessage::isReceivedTypeASyncType(radioBufferPtr[0]))
	{
		unserializeIntoCommonSyncMessage();
		result = &inwardCommonSyncMsg;
	}
	/* FUTURE when WorkMsg distinct from SyncMsg
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
		log("Invalid message type\n");
		logLongLong(radioBufferPtr[0]);
		result = nullptr;	// PRODUCTION
	}
	// assert validity of SystemID and offset have not been checked
	// FUTURE check offset is in range that scheduling can handle
	return result;
}


bool Serializer::bufferIsSane(){
	// FUTURE other validity checks?
	return SyncMessage::isReceivedTypeASyncType(radioBufferPtr[0]);
}




// Payload is 6+3+1==10
void Serializer::serializeOutwardCommonSyncMessage() {
	radioBufferPtr[0] = outwardCommonSyncMsg.type;
	serializeMasterIDCommonIntoStream(outwardCommonSyncMsg);
	serializeOffsetCommonIntoStream(outwardCommonSyncMsg);
	serializeWorkCommonIntoStream(outwardCommonSyncMsg);
}

