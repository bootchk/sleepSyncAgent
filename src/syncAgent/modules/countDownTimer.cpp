
#include <cassert>

#include "countDownTimer.h"

#include "../../augment/timeMath.h"

namespace {
LongTime _endingTime;
DeltaTime _previousRemaining;

// Uses but does not own
LongClock* _longClock;
}


void CountdownTimer::init(LongClock* aLongClock) {
	_longClock = aLongClock;
	_previousRemaining = 0;
}


void CountdownTimer::setExpiration(DeltaTime expiration){
	assert(expiration > 0);

	LongTime _endingTime = _longClock->nowTime() + expiration;
	_previousRemaining = expiration;
}


DeltaTime CountdownTimer::getRemaining(){
	// illegal to call when previous call returned 0
	assert(_previousRemaining != 0);

	DeltaTime result;
	result = TimeMath::clampedTimeDifferenceFromNow(_endingTime);

	return result;
}

