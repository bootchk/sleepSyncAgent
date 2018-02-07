
#include "granularity.h"

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

bool Granularity::isMsgInVirtualRange(int8_t rssi) {

}
