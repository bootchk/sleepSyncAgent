

#include "serializer.h"


Message* Serializer::unserialize(void* data) {
	return new Message();
}

uint8_t* Serializer::serialize(WorkMessage& msg) {
	return (uint8_t*) 0;
}

uint8_t* Serializer::serialize(SyncMessage& msg) {
	return (uint8_t*) 0;
}


