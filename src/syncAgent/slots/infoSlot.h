#pragma once

#include <inttypes.h>



class InfoSlot{

public:
	/*
	 * perform regardless of power
	 *
	 * System might be foobar: brownout, assertion, etc.
	 */
	static void perform(uint8_t);
};
