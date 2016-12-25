#pragma once

#include "types.h"  // ScheduleCount, DeltaTime


/* !!! Parameters of schedule.
 *
 * Used by schedule.h, mergeOffset.h, deltaSync.h and others, i.e. constants of the Schedule class
 *
 * Other params of algorithm at DropoutMonitor.h
 */



class ScheduleParameters {
public:	// for assertions


/*
 * Duration of all slots.
 * Units: OSTicks
 * (When OSClock freq is 32khz, OSTick is 0.03ms)
 * SlotDuration should > on-air time of a message
 * since we want to send a message (Sync, Work) within a slot.
 * e.g. if Bluetooth, one message is ~ 1msec
 * e.g. if RawWireless, one message is ~0.1msec
 * @32kHz xtal RTC, tick is 0.03msec
 */
//static const DeltaTime     SlotDuration = 300;	// ~ 10msec
//static const DeltaTime     SlotDuration = 20;	// ~ 0.6msec
// static const DeltaTime     SlotDuration = 30;	// ~ 0.9msec
//static const DeltaTime     SlotDuration = 40;	// ~ 1.2msec

/*
 * Ratio of sync period duration to wake duration.
 * Unitless.
 *
 * E.G. 1% DutyCycle same as 100 DutyCycleInverse, sleep about 99% of time
 *
 * Lower limit: 1 = always on (one fish slot), 2 = 50% duty cycle.
 *outer
 * Upper limit: See types.h MaximumScheduleCount
 * Upper limit is constrained because this affects SyncPeriodDuration,
 * which cannot be longer than the duration
 * we can schedule on a Timer provided by OS and RTC hardware.
 */
// Production: 3 active slots, ~300 sleeping, ~3 second period
// static const int           DutyCycleInverse = 100;

// Testing: 3 active slots, ~60 sleeping, 0.6 second period
//static const int           DutyCycleInverse = 20;

// 3 * 400 * .6 msec = 720 mSec = 0.7Sec
// 3 * 200 * 1.2 msec = 720 mSec = 0.7 Sec
//static const int           DutyCycleInverse = 200;


// 30, 400 Sync period 0.7 sec, average current 10uA?
// 40, 400 Sync period 1.0 sec
//static const DeltaTime     SlotDuration = 40;
//static const int           DutyCycleInverse = 400;

// 40, 100 Sync period 250 mSec (1/4 sec)
//static const DeltaTime     SlotDuration = 40;
//static const unsigned int  DutyCycleInverse = 100;

// 50, 100
static const DeltaTime     SlotDuration = 50;
static const unsigned int  DutyCycleInverse = 100;


/*
 * This is:
 * - ordinal of first sleeping slot
 * - count of active (radio on) slots
 *
 * Partially determined by algorithm design:
 * the first few slots are the active slots.
 *
 * See SIMPLE_SYNC_PERIOD in config.h:
 * 1. Sync, 2. Work, 3. Sleep, ..., Sleep
 *
 * When combined syncWorkSlot:
 * 1. SyncWork 2. Sleep, ...., Sleep
 */
//static const ScheduleCount FirstSleepingSlotOrdinal = 3;
static const ScheduleCount FirstSleepingSlotOrdinal = 2;


/*
 * Average count of active slots (with radio on) Sync, Fish.
 * (In an alternative design, also a separate Work slot.)
 * Average, since Fish slot is alternative to Merge slot,
 * which is a short transmit, probablistically transmitted within a SyncPeriod.
 */
static const ScheduleCount CountActiveSlots = 2;

/*
 * Count of slots in sync period.
 * Must be less than MAX_UINT16 (256k)
 *
 * Used:
 * - to calculate SyncPeriodDuration
 * - to schedule Fish slots (this defines the max of the range.)
 */
static const ScheduleCount CountSlots = CountActiveSlots*DutyCycleInverse;

/*
 * Duration of 'normal' SyncPeriod in units ticks.
 * Note that some actual SyncPeriods are not normal, extended in duration while merging cliques.
 */
static const DeltaTime NormalSyncPeriodDuration = CountSlots * SlotDuration;



/*
 * Duration of message over-the-air (OTA).
 * Units ticks.
 * Function of bitrate, message length, and RTC frequency:
 *
 * OTA ticks = 1/bitrate [second/bits] * messageLength [bits] * RTCFreq [ticks/second]
 *
 * Message bytes 1 preamble 3 address 11 payload 1 CRC => 16 bytes
 * 16 bytes is 128 bits
 * Used in sanity assertions only?
 */
// 1 Mbit bitrate, 128bit message, 32kHz   yields .12mSec = 4 ticks
// 2 Mbit bitrate, 128bit message, 32kHz   yields .064mSec == 64uSec = 2 ticks
// 2 Mbit, 120 bits, 32kHz yields 1.8 ticks
static const DeltaTime MsgOverTheAirTimeInTicks = 2;

/*
 * The delay between the time the sender fetches offset time
 * and sender actually sends it.
 * This is currently just a guess or measured.
 * If sending code changes, or optimization, this changes too.
 */
static const DeltaTime SenderLatency = 4;


/*
 * After radio is powered on, delay until radio is enableable (DISABLED state).
 * This is mainly time for HFXO clock to stabilize.
 * Software is involved in making this transition.
 *
 * The constant is experimentally measured,
 * plus an allowance for variance (expected worst deviation from experiments.)
 * If enough allowance is not made,
 * there will be dead gaps in listening/fishing.
 * This is used to overlap real slots.
 *
 * !!! Should not be more than SlotDuration, else the last FishSlot will lap into SyncSlot
 */
static const DeltaTime PowerOffToActiveDelay = 16;


/*
 * After radio is enabled, delay until radio is ready for OTA (TXIDLE or RXIDLE)
 */
#ifdef NRF52
	// ramp up in fast mode is 40uSec, i.e. 1.3 ticks
	static const DeltaTime RampupDelay = 2;
#else // NRF51
	// ramp up is 130 uSec i.e. 4.3 ticks
	static const DeltaTime RampupDelay = 4;
#endif



// Sanity.  SleepSync uses timeouts less than this, 5 seconds
static const DeltaTime MaxSaneTimeout = 164000;
};
