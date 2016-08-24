#include "serializer.h"


Message* Serializer::unserialize(void* data) {
	return new Message();
}

void* Serializer::serialize(Message& msg) {
	return (void*) 0;
}


