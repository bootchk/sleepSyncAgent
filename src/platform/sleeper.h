
#pragma once

#include "types.h"	// OSTIme

/*
 * Sleep until a waking event occurs, or until timeout.
 *
 * Waking event includes:
 * - radio msg received (if radio is on.)
 * - Timer expired
 *
 * !!! SleepSync expects no other events.
 *  Any other events:
 *  - normal e.g. GPIO e.g. button press events.
 *  - exceptions such as brownout or fault
 *  SyncAgent now halts (see dispatchMsgUntil.cpp )
 *
 *
 * Sleep is low power.
 * Platform must put all unused peripherals into low power mode.
 * (Automatic on some targets.)
 *
 * Typically, platform's RTC peripheral is NOT powered off, provides Timer.
 *
 * Platform must not put radio peripheral into low-power if SyncAgent has powerOn()'d it.
 */

typedef enum {
	MsgReceived,
	TimerExpired,
	None
} ReasonForWake;


class Sleeper {
private:
	ReasonForWake reasonForWake;

public:
	static void init();
	static bool isOSClockRunning();
	static void sleepUntilEventWithTimeout(OSTime);

	static ReasonForWake getReasonForWake();
	static void clearReasonForWake();

	// Public because passed to radio so it can hook IRQ into it
	static void msgReceivedCallback();
};

