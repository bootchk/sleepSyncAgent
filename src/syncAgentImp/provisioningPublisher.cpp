

#include "../syncAgentImp/provisioningPublisher.h"

#include "../scheduleParameters.h"
#include "../clique/periodTime.h"

#include "../logging/logger.h"

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

bool shouldFilter(int8_t rssi) {
	// TODO also need tss, use NetGranularity
	(void) rssi;
	return false;
}

/*
 * Understands what values handler callbacks want.
 */
uint32_t mangleProvisionedValue(ProvisionedValueType provisionedValue) {
	/*
	 * Dispatch conversions.
	 * Provisioning on BLE serializes to cross the network in on BT characteristic.
	 * provisionedValue is raw from the network.
	 * Reverse the type conversions,
	 * and perform other semantic twists.
	 */
	uint32_t result;

	switch(provisionedValue.index){

	case 0:	// work time
		// Here we don't care about pv.value, but when it occurred
		result = calculatePeriodTime(provisionedValue.offset);
		break;

	case 1: // scatter
			// Handler ignores value: the fact of provisioning is a signal
	case 2:	// work cycle
	case 3: // net granularity i.e. cluster/clique size
		//  No conversion or type checking.  The callback should enforce further requirements.
		result = provisionedValue.value;
		break;

	default:
		// Index is OTA and could be garbled.
		// But we checked earlier that index was in range
		// Generate a hard fault and reset.
		assert(false);
	}
	return result;
}


}  // namespace


void ProvisioningPublisher::subscribe(PropertyIndex propertyIndex, ProvisionCallback aCallback){
	assert(propertyIndex < 4);
	provisioningCallbacks[propertyIndex] = aCallback;
}




void ProvisioningPublisher::notify(
		PropertyIndex propertyIndex,
		ProvisionedValueType provisionedValue,
		int8_t rssi
		)
{
	Logger::log("Publisher notify\n");
	Logger::logInt(propertyIndex);
	assert(propertyIndex < 4);

	if (shouldFilter(rssi)) {
		Logger::log("Discard weak prov\n");
		return;
	}

	uint32_t convertedValue = mangleProvisionedValue(provisionedValue);

	// Call callback
	assert(provisioningCallbacks[propertyIndex] != nullptr);
    provisioningCallbacks[propertyIndex](convertedValue);
}
