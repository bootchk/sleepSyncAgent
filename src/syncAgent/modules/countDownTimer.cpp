
#include "countDownTimer.h"

#include "../globals.h"	// longClockTimer

namespace {
LongTime _endingTime;
DeltaTime _previousRemaining;

// Uses but does not own
LongClockTimer* _longClockTimer;
}


void CountdownTimer::init(LongClockTimer* aLongClockTimer) {
	_longClockTimer = aLongClockTimer;
	_previousRemaining = 0;
}


void CountdownTimer::setExpiration(DeltaTime expiration){
	assert(expiration > 0);

	LongTime _endingTime = _longClockTimer->nowTime() + expiration;
	_previousRemaining = expiration;
}


DeltaTime CountdownTimer::getRemaining(){
	// illegal to call when previous call returned 0
	assert(_previousRemaining != 0);

	DeltaTime result;
	result = TimeMath::clampedTimeDifferenceFromNow(_endingTime);

	return result;
}

