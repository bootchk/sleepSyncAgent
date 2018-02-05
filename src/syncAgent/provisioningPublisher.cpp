

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


	uint32_t convertedValue = 0;

	/*
	 * Dispatch conversions.
	 * Provisioning on BLE crams all value types into uint8_t to cross the network.
	 * provisionedValue is raw from the network.
	 * Reverse the type conversion here.
	 */
	switch(propertyIndex){

	// work time
	case 0:
		convertedValue = calculatePeriodTime(provisionedValue);
		break;

	// work freq
	case 1:
		/*
		 *  No conversion or type checking.
		 *  The callback should enforce requirements.
		 */
		convertedValue = provisionedValue;
		break;

	default:
		;
	}
	assert(propertyIndex < 4);
    provisioningCallbacks[propertyIndex](convertedValue);
}
