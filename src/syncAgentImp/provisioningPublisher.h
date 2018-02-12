
#pragma once

#include <inttypes.h>


/*
 * Parameter type is general and obscures real type,
 * which may be uint8_t.
 * Sleep sync may convert units and types before callback.
 */
typedef void (*ProvisionCallback)(uint32_t);

typedef unsigned int PropertyIndex;


/*
 *Responsibilities:
 * - Observer pattern i.e. publish/subscribe or subscribe/notify
 * - Convert type of transported (Bluetooth) provisioned values (uint8_t) to types expected by subscribers.
 * - Filter provisioning event by virtualRange (inverse rssi)
 *
 * understands subscribers more than it should since it knows their types
 *
 * Knows:
 * - set of provisioning properties.
 */

/*
 * Subscribers:
 * - app	(configure work)
 * - network (configure clique size, scatter)
 */

class ProvisioningPublisher {
public:

	static void subscribe(PropertyIndex propertyIndex, ProvisionCallback aCallback);
	static void notify(
			PropertyIndex propertyIndex,
			uint8_t provisionedValue,
			int8_t rssi
			);
};
