

#include "provisioningPublisher.h"

// For conversions implementation
#include "scheduleParameters.h"
#include "clique/periodTime.h"

#include <cassert>



namespace {

ProvisionCallback provisioningCallbacks[4];


// Conversions

DeltaTime calculatePeriodTime(uint8_t offsetTime) {
	/*
	 * offsetTime is a numerator of fraction (having denominator 255) of 10 seconds
	 * Convert to ticks without loss of precsion i.e. multiply before divide
	 */

	unsigned int ticksBeforeNowOfButtonPush = (10 * offsetTime * ScheduleParameters::TicksPerSecond) / 255;

	/*
	 * Period time  when button was pushed (if periods have not changed recently.
	 */
	return PeriodTime::convertTickOffset(ticksBeforeNowOfButtonPush);
}

}


void ProvisioningPublisher::subscribe(PropertyIndex propertyIndex, ProvisionCallback aCallback){
	assert(propertyIndex < 4);
	provisioningCallbacks[propertyIndex] = aCallback;
}




void ProvisioningPublisher::notify(
		PropertyIndex propertyIndex,
		uint8_t provisionedValue,
		int8_t rssi
		) {
	// TODO filter by rssi
	// Don't callback if provisioner was virtually out of range.
	// TODO (provisionedValue, provisionedVirtualRange), rssi

	// Dispatch to conversion routines
	switch(propertyIndex){

	// work time
	case 1: {
		uint32_t convertedValue = calculatePeriodTime(provisionedValue);
		provisioningCallbacks[1](convertedValue);
		}
		break;

	// work freq
	case 2: {
		// No conversion
		provisioningCallbacks[2](provisionedValue);
	}
	break;

	default:
		;
	}

}
