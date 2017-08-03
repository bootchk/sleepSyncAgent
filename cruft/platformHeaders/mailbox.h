
#pragma once

#include "../platformHeaders/types.h"

/*
 * Simple mailbox:
 * - holding ints
 * - listener polls
 * - not thread-safe (only one poster and listener)
 */



class Mailbox {
public:
	static void put(WorkPayload item);
	static WorkPayload fetch();
	static bool isMail();
};
