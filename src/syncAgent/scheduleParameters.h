#pragma once

#include "types.h"  // ScheduleCount, DeltaTime


/* !!! Parameters of schedule.
 *
 * Used by schedule.h, mergeOffset.h, deltaSync.h and others, i.e. constants of the Schedule class
 *
 * Other params of algorithm at DropoutMonitor.h
 */

/*
 * VirtualSlotDuration:
 *
 * Virtual slots divide a sync period into non-overlapping segments.
 * Real slot duration is greater, and real slots overlap.
 *
 * Real slots are longer to accommodate HFXOStartup.
 * That is, real slots have radio dead time for radio lag, but then listen a full VirtualSlotDuration.
 * Thus, a sequence of FishSlots listens to the whole sync period without gaps in listening.
 *
 * Virtual slots start on the divisions of a sync period.
 * The end of one real slot overlaps onto the beginning of the next.
 * The end of the sync slot overlaps onto the beginning of the first fish slot,
 * so when a first fish slot is scheduled, the scheduled time is already over and the fish slot starts immediately.
 *
 * The last virtual slot of a sync period is not fished.
 * The last fishing slot overlaps into the last virtual slot of the sync period.
 *
 * Thus there are some dead gaps: a short time at the beginning of the first fishing slot (when the radio is lagging)
 * and the tail end of the last virtual slot (which is not fished at all.)
 *
 * FUTURE sync period w/o deadtime gaps
 * e.g. that keeps the radio hot after the sync slot, when the second virtual slot (the first fish slot) is to be fished.
 * And some way of fishing closer to the sync point in the last virtual slot.
 */
// XXX design sync period without deadtime gaps


class ScheduleParameters {
public:	// for assertions

/*
 * Slot duration and duty cycle.
 */

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

// 40, 800 Sync period 2 sec
// This is the one that I did most testing with
static const DeltaTime     VirtualSlotDuration = 40;
static const unsigned int  DutyCycleInverse = 800;

// 40, 1600 Sync period 4 sec
//static const DeltaTime     VirtualSlotDuration = 40;
//static const unsigned int  DutyCycleInverse = 800;




static const DeltaTime     HalfSlotDuration = VirtualSlotDuration / 2;







/*
 * Enumeration of slots.
 */

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
 * Constants used by FishPolicy.
 *
 * Now, because of HFXO startup delay, we don't fish every sleeping slot.
 * TODO, improve the design so that we fish every sleeping slot.
 */
/*
 * Deficient design:
 * Last sleeping slot could be the last slot i.e. CountSlots.
 * But we don't fish it, since it overlaps/delays start of SyncPeriod.
 * Also, fishing the first slot actually fishes the second sleeping slot,
 * and the first sleeping slot is never fished.
 */

static const ScheduleCount LastSlotOrdinalToFish = ScheduleParameters::CountSlots - 1;	// !!!
static const ScheduleCount FirstSlotOrdinalToFish = ScheduleParameters::FirstSleepingSlotOrdinal;




/*
 * Duration of 'normal' SyncPeriod in units ticks.
 * Note that some actual SyncPeriods are not normal, extended in duration while merging cliques.
 */
static const DeltaTime NormalSyncPeriodDuration = CountSlots * VirtualSlotDuration;




/*
 * Durations of real OTA events and other delays
 */

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
 * The delay between the time the sender fetches offset time and sender actually sends it.
 * IOW delay between send and receive, on a reference clock.
 * This is currently just a guess or measured.
 * If sending code changes, or optimization, this changes too.
 */
static const DeltaTime SenderLatency = 4;


/*
 * After system wakes from radio off state,
 * delay until we should activate radio (from DISABLED state to active state).
 * This is one component of 'dead' time for radio.
 * (Even after we activate, their is more dead time: rampup.)
 *
 * Comprises:
 * - time for HFXO clock to stabilize (1200uSec, 40 ticks)
 * - a few ticks for other overhead (some execution time.)
 * - an allowance for variance (expected worst deviation from experiments.)
 *
 * The constant is experimentally measured, for specific HF crystals.
 * If enough allowance is not made,  there will be dead gaps in listening/fishing.
 * This is used to overlap real slots.
 *
 * !!! Should not be more than SlotDuration, else the last FishSlot will lap into SyncSlot
 */
static const DeltaTime PowerOffToActiveDelay = 41;
// TODO check these constants at runtime
#if PowerOffToActiveDelay > VirtualSlotDuration
#error "delay too long"
#endif

/*
 * After radio is enabled, delay until radio is ready for OTA (TXIDLE or RXIDLE).
 * This is another component of 'dead' time.
 * !!! RampupDelay can be incurred again when switching from xmit to rcv or vice versa.
 */
#ifdef NRF52
	// ramp up in fast mode is 40uSec, i.e. 1.3 ticks
	static const DeltaTime RampupDelay = 2;
#else // NRF51
	// ramp up is 130 uSec i.e. 4.3 ticks
	static const DeltaTime RampupDelay = 4;
#endif

/*
 * Delay from sleeping to radio ready (TXIDLE or RXIDLE).
 *
 */
/*
 * OLD Design:
 * static const DeltaTime HFXOStartup = PowerOffToActiveDelay + RampupDelay;
 */
/*
 * NEW Design:
 * Make it the same as VirtualSlot.
 * As long as VirtualSlot is greater than 40,
 * that is ample time for HFXO startup.
 * And it simplifies thinking about overlap of RealSlot and VirtualSlot
 */
static const DeltaTime HFXOStartup = VirtualSlotDuration;

/*
 * DeltaTime from a time we must wakeup
 * to the time when we want a message to be received.
 */
static const DeltaTime PreflightDelta = HFXOStartup + RampupDelay + MsgOverTheAirTimeInTicks;

/*
 * Real slots are greater duration than virtual slots.
 */
static const DeltaTime RealSlotDuration = VirtualSlotDuration + HFXOStartup;


/*
 * Middle of virtual (active) portion of RealSyncSlot.
 *
 * Radio is active a HFXOStartup duration after starting RealSyncSlot.
 * Active means "able to receive or transmit."
 * (Here we are ignoring initial Radio RampupDelay.)
 * Radio is active a full VirtualSlotDuration
 * Center of the active period is HFXOStartup plus HalfSlotDuration.
 * But a RampupDelay is incurred switching from rcv to xmit.
 * So to center the xmit, must start one RampupDelay before center of active period,
 * hence we subtract one RampupDelay.
 *
 * This is a time when transmission starts.
 * A transmission is received MsgOverTheAirTimeInTicks later.
 */
static const DeltaTime DeltaSyncPointToSyncSlotMiddle = HalfSlotDuration + HFXOStartup - RampupDelay;





/*
 * Sanity checks for parameters passed to sleeping methods.
 */

// TODO don't use this, but more specific values
/*
 * Sanity.
 * SleepSync uses timeouts less than this.
 *
 * This absolutely must be less than LongClock::MaxTimeout of 0xFFFFFF == 16M == 503 seconds == 8 minutes
 * Should not schedule anything longer than 2 SyncPeriods of say 2000 * 50 ticks
 */
static const DeltaTime MaxSaneTimeout = 164000;
// For power measurements
//static const DeltaTime MaxSaneTimeout = 2000000;




#ifdef OLD
/*
 * Initial delay to recover boot energy and wait for stable, accurate clock.
 * Depends on platform specs for startup time of 32kHz crystal oscillator.
 * Here, nRF5x, max .25 seconds for LFXO
 */

//static const DeltaTime StabilizedClockTimeout = 10000;	// 10k * 30uSec == 300kuSec == 0.3 seconds
static const DeltaTime StabilizedClockTimeout = 20000;	// 0.6 seconds

//static const DeltaTime StabilizedClockTimeout = 1000000;	// 1M * 30uSec == 300kuSec == 30 seconds
#endif


/*
 * How long we should sleep for SyncPower
 * Units ticks of 30uSeconds
 * 40k == 1.2 seconds
 * 160k == 4.4 seconds
 *
 * !!! Keep it long.
 * This delay is only incurred on startup.
 * If the hysteresis of the voltage monitor is small (say 0.2V)
 * and the storage capacitor is small, then delaying too short
 * means when we wake, we may not have recovered boot energy and we will brownout.
 */
static const DeltaTime TimeoutWaitingForSyncPowerSleeper = 160000;


/*
 * Max timeout used by SyncSleeper
 */
static const DeltaTime MaxSaneTimeoutSyncSleeper = 2* NormalSyncPeriodDuration;





/*
 * Constants used by OversleepMonitor.
 */

/* Ticks the code takes to get to sleep and wake from sleep.
 *
 * This is measured.
 * Depends on the target, assertions, and optimization.
 *
 * 5 ticks == 150uSec, @16Mhz is about 2500 instruction cycles.
 * ??? That seems excessive
 */
static const DeltaTime CodesSleepOverhead = 5;

/* Latitude when checking for oversleep.
 * Allows for:
 * - clock jitter (+1 + -(-1) == 2)
 * - + code overhead == 5
 * Anyway, when intendedSleep is 0, measured oversleep is 5.
 */
static const DeltaTime OversleepMargin = CodesSleepOverhead;

};
