#include "controller.h"

#include <radioSoC.h>

void Controller::setXmitPower(WorkPayload xmitPowerValue) {
	// Check validity of OTA value
	if (Radio::isValidXmitPower(static_cast<TransmitPowerdBm>(xmitPowerValue)))
		Radio::configureXmitPower(static_cast<TransmitPowerdBm>(xmitPowerValue));
}
