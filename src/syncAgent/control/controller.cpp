#include "controller.h"

#include <radioSoC.h>

void Controller::setXmitPower(WorkPayload xmitPowerValue) {
	// Requires radio not in use, required by Radio::

	// Check validity of OTA value
	if (Radio::isValidXmitPower(static_cast<TransmitPowerdBm>(xmitPowerValue))) {
		Radio::configureXmitPower(static_cast<TransmitPowerdBm>(xmitPowerValue));
	}

	// Ensure xmit power set, ensured by Radio::
}
