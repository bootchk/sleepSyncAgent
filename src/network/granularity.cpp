
#include "granularity.h"

#include "../message/message.h"

//#include <radioSoC.h>


namespace {

NetGranularity _granularity;

/*
 * Plus4 = 0x04,
	// Plus3 = 0x03,	// Not available on nrf51
	Plus0 = 0x00,
    Minus4 = -4,
	Minus8 = -8,
	Minus12 = -12,
	Minus16 = -16,
	Minus20 = -20,
	Minus40 = -40
 */

TransmitPowerdBm xmitPowerForGranularity(NetGranularity granularity) {
	TransmitPowerdBm result;

	switch(granularity) {
	case NetGranularity::Small:
		result = TransmitPowerdBm::Minus40;
		break;
	case NetGranularity::Medium:
		result = TransmitPowerdBm::Minus12;
		break;
	case NetGranularity::Large:
		result = TransmitPowerdBm::Plus0;
	}
	return result;
}

}


void Granularity::setGranularity(NetGranularity granularity) {

	// Requires radio not in use, required by Radio::

	_granularity = granularity;

	Radio::configureXmitPower(xmitPowerForGranularity(granularity));

	// Ensure xmit power set, ensured by Radio::
}


bool Granularity::isMsgInVirtualRange(
		unsigned int rssi,	// receivedSignalStrength,
		NetGranularity transmittedSignalStrength)
{
	bool result;

	switch(_granularity) {
	case NetGranularity::Small:
		// high power senders with moderate received signals are out of range
		// medium power senders with weak received signals are out of range
		result = not ( (transmittedSignalStrength == NetGranularity::Large and rssi < 750)
		             or (transmittedSignalStrength == NetGranularity::Medium and rssi < 60) );
		break;
	case NetGranularity::Medium:
		// Only high power senders with weak received signals are out of range
		result =  not (transmittedSignalStrength == NetGranularity::Large and rssi < 60);
		break;
	case NetGranularity::Large:
		// Any one I hear is in range.
		result = true;
	}
	return result;
}