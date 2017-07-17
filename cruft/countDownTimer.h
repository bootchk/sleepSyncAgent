#pragma once

#include <nRF5x.h>	// LongClock

#include "../types.h"



/*
 * Counts down.
 *
 * Algebra:
 *
 * Normal:   setExpiration(timeout); getRemaining(); ...getRemaining(); setExpiration
 *
 * Illegal to call getRemaining() when it previously returned 0 (countdown is complete)
 * getRemaining() == 0; getRemaining() is illegal
 *
 * Illegal: setExpiration(0)
 *
 * Invariants:
 * 0 <= getRemaining() < timeout
 * Monotonic: getRemaing() <= previous getRemaining() without intervening setExpiration()
 *
 * Implementation: depends on a LongClock
 */

class CountdownTimer {
public:
	static void init(LongClock*);

	/*
	 * Take now time as starting time and countdown until now+expiration
	 */
	static void setExpiration(DeltaTime expiration);

	static DeltaTime getRemaining();
};
