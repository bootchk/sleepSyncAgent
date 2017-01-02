
#pragma once

#include <nRF5x.h>  // logger
#include "../types.h"  // DeltaTime


/*
 * Type of field of SyncMessage to adjust SyncPoint.
 *
 * Design considerations:
 * This is transmitted over-the-air; to keep packets short, it should be small.
 * But the platform RTC clock (OSTime, typically 24 or 32 bits) limits the max size;
 * DeltaSync shouldn't be any larger than the platform RTC allows for timeouts.
 *
 * The size can be no smaller than determined by a combination of:
 * - resolution of the platform RTC clock (OSClock)
 * - SlotDuration
 * - DutyCycleInverse
 * As illustrated by the following derivation:
 * MaxSyncPeriod == MaxDeltaSync / 2    (we must schedule two SyncPeriods forward)
 * MaxSlotCount = MaxSyncPeriod / SlotDuration
 * MaxDutyCycle == 3 awake slots / MaxSlotCou;nt
 * MaxDutyCycleInverse = 1 / MaxDutyCycle
 *
 * Example design choices:
 * for OSClock resolution==1/32khz and SlotDuration==300 ticks:
 * - 2-bytes, 16-bits, MaxDeltaSync is 64k:  max DutyCyleInverse is 30
 * - 3-bytes, 24-bits, MaxDeltaSync is 16M: max DutyCycleInverse is 10k
 * for OSClock resolution==1/32khz and SlotDuration==50 ticks:
 * - 2-bytes, 16-bits, MaxDeltaSync is 64k:  max DutyCyleInverse is 180
 *
 * OTA transmitted serialization of values of this type must be as large
 * as the maximum value defined by this type.
 *
 * We use -Wconversion to catch possible loss of data,
 * and the code should be asserting no loss of data.
 *
 * This type is a property: a value with getter and setter that enforce constraints.
 * Use this type instead of DeltaTime,
 * to signify additional meaning of the difference between two times.
 * Avoid promiscuous use of DeltaTime.
 */



class DeltaSync {

public:
	static DeltaTime get();

	// Throws assertion if out of range
	static void set(DeltaTime value);

	// Preflight check value not out of range
	static bool isValidValue(DeltaTime value);
};

