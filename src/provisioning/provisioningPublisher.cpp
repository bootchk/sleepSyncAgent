

#include "provisioningPublisher.h"

#include "../scheduleParameters.h"
#include "../clique/periodTime.h"

#include "../logging/logger.h"

#include <cassert>





namespace {

ProvisionCallback provisioningCallbacks[4];


// Conversions

#ifdef NOT_USED
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
#endif

unsigned int calculateElapsedSyncPeriods(uint8_t offsetTime) {
	/*
	 * offsetTime is a numerator of fraction (having denominator 255) of 10 seconds
	 */
	unsigned int ticksElapsed = (10 * offsetTime * ScheduleParameters::TicksPerSecond) / 255;

	unsigned int result = ticksElapsed / ScheduleParameters::NormalSyncPeriodDuration;

	Logger::log("\nPeriods elapsed: ");
	Logger::logInt(result);
	return result;
}

bool shouldFilter(int8_t rssi) {
	// TODO also need tss, use NetGranularity
	(void) rssi;
	return false;
}

/*
 * Understands what values handler callbacks want.
 */
uint32_t mangleProvisionedValue(
		ProvisionablePropertyIndex propertyIndex,
		ProvisionedValueType provisionedValue) {
	/*
	 * Dispatch conversions.
	 * Provisioning on BLE serializes to cross the network in on BT characteristic.
	 * provisionedValue is raw from the network.
	 * Reverse the type conversions,
	 * and perform other semantic twists.
	 */
	uint32_t result;

	switch(propertyIndex){

	case ProvisionablePropertyIndex::WorkTime	:
		// Here we don't care about pv.value, but when it occurred

		// TODO we actually need both of these: periods elapsed and offset in current period.
		//result = calculatePeriodTime(provisionedValue.offset);
		result = calculateElapsedSyncPeriods(provisionedValue.offset);
		break;

	case ProvisionablePropertyIndex::Scatter:
			// Handler ignores value: the fact of provisioning is a signal
	case ProvisionablePropertyIndex::WorkCycle:
	case ProvisionablePropertyIndex::NetGranularity: // i.e. cluster/clique size
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


/*
 * No checking:
 * - that index is not subscribed to many times
 */
void ProvisioningPublisher::subscribe(ProvisionablePropertyIndex propertyIndex, ProvisionCallback aCallback){
	/*
	 * ProvisionablePropertyIndex is enum class.
	 * Must static_cast to use as an integral type for array indexing.
	 */
	RawPropertyIndex rawPropertyIndex = static_cast<RawPropertyIndex> (propertyIndex);
	assert(rawPropertyIndex < 4);
	provisioningCallbacks[rawPropertyIndex] = aCallback;
}




void ProvisioningPublisher::ProvisioningPublisher::notify(
		ProvisionablePropertyIndex propertyIndex,
		ProvisionedValueType provisionedValue,
		int8_t rssi
		)
{
	RawPropertyIndex rawPropertyIndex = static_cast<RawPropertyIndex> (propertyIndex);
	Logger::log("Publisher notify\n");
	//Logger::logInt(rawPropertyIndex);

	if (shouldFilter(rssi)) {
		Logger::log("Discard weak prov\n");
		return;
	}

	uint32_t convertedValue = mangleProvisionedValue(propertyIndex, provisionedValue);

	// Call callback
	assert(provisioningCallbacks[rawPropertyIndex] != nullptr);
    provisioningCallbacks[rawPropertyIndex](convertedValue);
}

ProvisionablePropertyIndex ProvisioningPublisher::ppiFromRawPPI( RawPropertyIndex index) {
	// assert caller ensures index in range, and caller does not require result != PPI::Invalid (caller will check)
	return static_cast<ProvisionablePropertyIndex> (index);
}
