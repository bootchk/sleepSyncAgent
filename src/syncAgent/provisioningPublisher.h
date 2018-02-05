
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
 * Observer pattern i.e. publish/subscribe or subscribe/notify
 *
 * Understands:
 * - set of provisioning properties.
 *
 * - How to convert provisioned values (uint8_t)
 * to values expected by subscribers.
 *
 * - How to filter provisioning event by virtualRange (inverse rssi)
 *
 * I.E. understands subscribers more than it should.
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
