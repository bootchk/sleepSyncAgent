
#include "fishPowerMgr.h"

#include "slottedFishSession.h"

// radioSoC
#include <modules/vcc.h>

#include "../logging/logger.h"



namespace {

bool isMaxFishing = false;

}  // namespace





void FishPowerMgr::init() {
	// Uses Vcc, which uses some ADC device which requires init
	Vcc::init();
}




void FishPowerMgr::manage() {
	unsigned int value = Vcc::measure();

	Logger::logInt(value);

	if (value >= VccResult::Result2_7V) {
		if ( not isMaxFishing ) {
			SlottedFishSession::incrementFishSessionDuration(20);
			isMaxFishing = true;
		}
	}
	else {
		SlottedFishSession::setDurationToMinDuration();
		isMaxFishing = false;
	}
}
