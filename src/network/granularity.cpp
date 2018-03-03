
#include "granularity.h"

#include "../message/message.h"

#include "../logging/logger.h"


//#include <radioSoC.h>


namespace {

NetGranularity _granularity;

}


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

/*
 * Area of received signal is proportional to xmit power.
 * i.e. small area equals less xmit power
 */
TransmitPowerdBm Granularity::xmitPowerForGranularity(NetGranularity granularity) {
	TransmitPowerdBm result;

	switch(granularity) {
	case NetGranularity::Small:  result = TransmitPowerdBm::Minus40; break;
	case NetGranularity::Medium: result = TransmitPowerdBm::Minus12; break;
	case NetGranularity::Large:  result = TransmitPowerdBm::Plus0;   break;
	case NetGranularity::Invalid:
		Logger::log(" Defaulting invalid granularity to low xmit powerl\n");
		result = TransmitPowerdBm::Minus40;;
	}
	return result;
}


NetGranularity Granularity::getCliqueGranularity() { return _granularity; }
unsigned char Granularity::getCliqueGranularityRaw() { return getRaw(_granularity); }
unsigned char Granularity::getRaw(NetGranularity aGranularity) { return static_cast <unsigned char> (aGranularity); }


NetGranularity Granularity::getFromRaw(unsigned char rawTSS){
	// raw granularity (aka TSS) is OTA and could have bit errors
	NetGranularity result;

	switch (rawTSS) {
	case 0:  result = NetGranularity::Large; break;
	case 1:  result = NetGranularity::Medium; break;
	case 2:  result = NetGranularity::Small; break;
	// Other values are invalid, i.e. garbled
	default:
		result = NetGranularity::Invalid; break;
	}
	return result;
}


void Granularity::trySetGranularity(NetGranularity granularity) {

	if (granularity == NetGranularity::Invalid) {
		// OTA granularity is garbled
		// Already logged when decoded
		return;
	}

	// Requires radio not in use, required by Radio::

	Logger::log("Xmit power before: ");
	Logger::log(XmitPower::repr(Radio::getXmitPower()));

	_granularity = granularity;

	Radio::configureXmitPower(xmitPowerForGranularity(granularity));

	// Ensure xmit power set, ensured by Radio::

	Logger::log(XmitPower::repr(Radio::getXmitPower()));
}

#ifdef NOT_USED
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

	result ? Logger::log("Msg IN range\n") : Logger::log("Msg OUT range\n");

	return result;
}
#endif
