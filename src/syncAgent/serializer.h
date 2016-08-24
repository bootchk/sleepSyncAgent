#pragma once

#include "message.h"

class Serializer {

public:
	static Message* unserialize(void* data);
	static void* serialize(Message& msg);
};
