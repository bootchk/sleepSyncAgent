/*
 * !!! Parameters of schedule.
 *
 * Included by schedule.h, i.e. constants of the Schedule class
 *
 * Other params of algorithm at DropoutMonitor.h
 */

public:	// for assertions


/*
 * Duration of all slots.
 * Units: OSTicks
 * (When OSClock freq is 32khz, OSTick is 0.03ms)
 * SlotDuration should > on-air time of a message
 * since we want to send a message (Sync, Work) within a slot.
 * e.g. if Bluetooth, one message is ~ 1msec
 * e.g. if RawWireless, one message is ~0.1msec
 */
static const DeltaTime     SlotDuration = 300;	// ~ 10msec

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
static const int           DutyCycleInverse = 20;



/*
 * Fixed by algorithm design.
 * Sync, Work, Sleep, ..., Sleep
 */
static const ScheduleCount FirstSleepingSlotOrdinal = 3;

/*
 * Count of slots in sync period.
 * Must be less than MAX_UINT16 (256k)
 *
 * Only used to calculate SyncPeriodDuration
 * Here 3 is the average count of active slots (with radio on) Sync, Work, Fish.
 * (Average, since Fish slot is alternative to Merge slot,
 * which is a short transmit, probablistically transmitted within a SyncPeriod.)
 */
static const ScheduleCount CountSlots = 3*DutyCycleInverse;

// Duration of SyncPeriod not adjusted (extended)
static const DeltaTime NormalSyncPeriodDuration = CountSlots * SlotDuration;


//
//
/*
 * Duration of message on air.
 * Units ticks.
 * Assuming 1 Mbit baud rate and ~100bit message = .1mSec
 * Used in sanity assertions only?
 */
static const DeltaTime MsgDurationInTicks = 3;

