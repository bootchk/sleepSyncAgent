
#include <cassert>
#include <cstring>	// memcpy

#include "serializer.h"

#include "otaPacket.h"
#include "messageFactory.h"

#include "../logging/logger.h"

/*
 * Implementation notes:
 *
 * Message objects need not be packed.
 * This code does not take a brute force approach (one loop over data into object/struct.)
 * Instead, we loop over message fields.
 *
 * Use shared, resued static Messages (not on the heap), SyncAgent only uses one at a time.
 * Thus many methods have side effects on shared Messages, and return pointer to shared.
 *
 * The count of bytes must match the constant FixedPayloadCount defined in platform/radio.h
 */



namespace {

/*
 * Serializer does NOT own message objects but returns pointers to them.
 *
 * There is only one SyncMessage instance, be careful in reuse,
 * i.e. don't try to send WorkSync while still accessing incoming WorkSync.
 */

// Serializer does not own buffer (Radio owns it), only knows pointer to it
BufferPointer radioBufferPtr = nullptr;
uint8_t radioBufferSize;



// suppress compiler warning for pointer arith
#pragma GCC diagnostic ignored "-Wpointer-arith"


/*
 * Matched pairs for each field
 */


/*
 * This byte is packed.
 * This MIGHT allow old code to work
 * message from old code (where TransmitSignalStrength was not defined and zero)
 * except that types are greater than 2^6
 */
void unserializeTypeAndTSSInto(SyncMessage* msgPtr) {
	uint8_t raw;

	memcpy( (void*) &(raw),	// dest
			(void*) radioBufferPtr + OTAPayload::TypeTSSIndex,	// src
			OTAPayload::TypeTSSLength);

	/*
	 * Not use static_cast because theoretically it could throw exception.
	 */
	//  Mask all but lower two bits.
	uint8_t rawTSS = raw & 0x3;
	msgPtr->transmittedSignalStrength = Granularity::getFromRaw(rawTSS);

	uint8_t rawType;
	// Shift away lower two bits
	// Cast the shift result from int to unsigned char.  C says shift operands always promoted to int.
	rawType = static_cast <unsigned char> (raw >> 2);
	msgPtr->type = SyncMessage::messageTypeFromRaw(rawType);
}


void serializeTypeAndTSSCommonIntoStream(SyncMessage* msgPtr){
	uint8_t raw;

	// Shift MsgType left, leaving two bits for TSS
	// Cast to avoid compiler warning about int result from shift
	// assert type < 2^6
	raw = static_cast <uint8_t> (msgPtr->type);
	raw = static_cast <unsigned char> (raw << 6);

	// Or in two bits of TSS
	raw = raw || (static_cast <uint8_t>(msgPtr->transmittedSignalStrength) & 0x3);

	memcpy( (void*) radioBufferPtr + OTAPayload::TypeTSSIndex, 	// dest
			(void*) &(raw),	// src
			OTAPayload::TypeTSSLength);
}



void unserializeWorkInto(SyncMessage* msgPtr) {
	memcpy( (void*) &(msgPtr->work),	// dest
			(void*) radioBufferPtr + OTAPayload::WorkIndex,	// src
			OTAPayload::WorkLength);
}
void serializeWorkCommonIntoStream(SyncMessage* msgPtr){
	memcpy( (void*) radioBufferPtr + OTAPayload::WorkIndex, 	// dest
			(void*) &(msgPtr->work),	// src
			OTAPayload::WorkLength);
}




void unserializeMasterIDInto(SyncMessage* msgPtr) {
	assert(sizeof(msgPtr->masterID)>=OTAPayload::MasterIDLength);
	msgPtr->masterID = 0; // ensure MSB two bytes are zero.
	// Fill LSB 6 bytes of a 64-bit
	memcpy( (void*) &(msgPtr->masterID),	// dest
			(void*) radioBufferPtr + OTAPayload::MasterIndex,	// src
			OTAPayload::MasterIDLength);
}

void serializeMasterIDCommonIntoStream(SyncMessage* msgPtr) {
	// Send LSB 6 bytes of 64-bit
	memcpy( (void*) radioBufferPtr + OTAPayload::MasterIndex, 	// dest
			(void*) &(msgPtr->masterID),	// src
			OTAPayload::MasterIDLength);
}

DeltaTime unserializeOffset() {
	// assert sizeof(DeltaTime) >= OTAPayload::OffsetLength
	// 24-bits OTA, little-endian into LSB three bytes of a 32-bit OSTime
	// FUTURE, code for 32-bit OSClock

	// !!! // Ensure MSB byte is zero because we only copy in LSB
	DeltaTime result = 0;
	memcpy( (void*) &result, 	// dest
			(void*) radioBufferPtr + OTAPayload::OffsetIndex,	// src
			OTAPayload::OffsetLength);	// count
	return result;
}


void unserializeOffsetInto(SyncMessage* msgPtr) {
	DeltaTime otaDeltaSync = unserializeOffset();
	msgPtr->deltaToNextSyncPoint.set(otaDeltaSync);
	// assert deltaToNextSyncPoint is set to a valid value
}

void serializeOffsetCommonIntoStream(SyncMessage* msgPtr) {
	DeltaTime offset = msgPtr->deltaToNextSyncPoint.get();
	// DeltaTime is 32-bit, DeltaSync insures only 24-bit, serialize only low order 3 bytes.
	// Not portable big endian
	memcpy( (void*) radioBufferPtr + OTAPayload::OffsetIndex, 	// dest
			(void*) &offset,	// src
			OTAPayload::OffsetLength);
}

// End of pointer arith
#pragma GCC diagnostic pop



void unserializeOTAFieldsIntoMessageFields(MessageType receivedMsgType, SyncMessage* msgPtr) {
	msgPtr->type = receivedMsgType;
	unserializeMasterIDInto(msgPtr);
	unserializeOffsetInto(msgPtr);
	unserializeWorkInto(msgPtr);
}

#ifdef OBSOLETE
SyncMessage* getPointerToMessageOfOTAType(MessageType receivedMsgType) {
	// already assert isReceivedTypeASyncType
	SyncMessage* msgPtr = &masterSyncMsg;	// FUTURE InvalidMsgRef

	switch(receivedMsgType) {
	case MessageType::MasterSync: msgPtr = &masterSyncMsg; break;
	case MessageType::MergeSync: msgPtr = &mergeSyncMsg; break;
	case MessageType::WorkSync: msgPtr = &workSyncMsg; break;
	case MessageType::AbandonMastership: msgPtr = &abandonMastershipMsg; break;
	}
	return msgPtr;
}



// FUTURE only unserializeOffset() once
/*
 * Check received OTA data.
 *
 * To protect against malice broadcasters, and failure of CRC to detect multiple bit errors,
 * enforce algorithm's constraints on DeltaSync,
 * and valid MessageType
 *
 * This should be rare, since CRC was valid.
 * It would take multiple bit errors to corrupt message type and still have valid CRC?
 * Or does preamble, address, all zeroes have a correct CRC?
 *
 * validity of SystemID and WorkPayload not checked.
 * An invalid SystemID might change master temporarily, but algorithm should recover.
 *
 * See also bufferIsSane, for outgoing messages.
 */
// XXX combine bufferIsSane w isOTA...
bool isOTABufferAlgorithmicallyValid() {
	bool result = true;
	if (! MessageFactory::isReceivedTypeASyncType(radioBufferPtr[0])) {
		Logger::log("Invalid message type\n");
		Logger::logInt(radioBufferPtr[0]);
		result = false;
	}
	if (! DeltaSync::isValidValue(unserializeOffset())) {
		// already logged
		result = false;
	}

	return result;
}
#endif



}	// namespace










void Serializer::init(BufferPointer aRadioBuffer, uint8_t aBufferSize)
{
	// Knows address, size of radio's buffer
	radioBufferPtr = aRadioBuffer;
	radioBufferSize = aBufferSize;
}



SyncMessage* Serializer::unserialize() {
	// require validCRC  data in radioBuffer, of proper length

	// Reuse common message
	SyncMessage* result = MessageFactory::getMessagePtr();

	// radioBufferPtr is volatile, which prevents compiler from optimizing repeated references.

	unserializeTypeAndTSSInto(result);


	/*
	 * Structure does not depend on valid type, but protocol behaviour does.
	 */
	if (result->type != MessageType::Invalid) {
		// unserialize other fields
		unserializeOTAFieldsIntoMessageFields(result->type, result);
	}
	else {
		result = nullptr;
	}

	// assert result is pointer to valid SleepSyncMessage, or nullptr
	// !!!Offset is not checked until later
	// TODO
	return result;
}

#ifdef OLD
SyncMessage* Serializer::unserialize() {
	// require validCRC  data in radioBuffer, of proper length
	SyncMessage* result = nullptr;

	// Minor optimization: only access radioBufferPtr[0] once.
	// It is volatile, which prevents compiler from optimizing repeated references.
	if (isOTABufferAlgorithmicallyValid()) {
		// Fast and loose cast
		MessageType receivedMsgType = (MessageType) radioBufferPtr[0];
		result = MessageFactory::getMessagePtr();
		// unserialize into singleton owned by MessageFactory
		unserializeOTAFieldsIntoMessageFields(receivedMsgType, result);
	}
	// assert result is pointer to valid SyncMessage, or nullptr
	return result;
}
#endif

#ifdef OLD
bool Serializer::bufferIsSane(){
	// FUTURE other validity checks?
	return MessageFactory::isReceivedTypeASyncType(radioBufferPtr[0]);
}
#endif


void Serializer::serializeSyncMessageIntoRadioBuffer(SyncMessage* msgPtr) {
	// requires init() called previously
	assert(radioBufferPtr != nullptr);

	serializeTypeAndTSSCommonIntoStream(msgPtr);	// 1 byte
	serializeMasterIDCommonIntoStream(msgPtr);	// 6
	serializeOffsetCommonIntoStream(msgPtr);	// 3
	serializeWorkCommonIntoStream(msgPtr);	// 1

	// Size of serialized message equals size fixed length payload of the wireless protocol
	static_assert(Radio::FixedPayloadCount == 11, "Protocol payload length mismatch.");
}

