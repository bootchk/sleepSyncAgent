
#pragma once

#include <inttypes.h>

// libBLEProvisionee
#include <provisioner.h>



/*
 * Parameter type is general and obscures real type,
 * which may be uint8_t.
 * Sleep sync may convert units and types before callback.
 */
typedef void (*ProvisionCallback)(uint32_t);

/*
 * Function provided by app
 */
typedef unsigned int (*ConverterFunc)(unsigned int);


/*
 * Int values must correspond to those defined by provisioning protocol i.e. implemented in client.
 */
enum class ProvisionablePropertyIndex {
	WorkTime = 0,
	Scatter,
	WorkCycle,
	NetGranularity,
	Invalid
};


typedef unsigned int RawPropertyIndex;


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
 * - syncAgent (configure network: clique size, scatter)
 */

class ProvisioningPublisher {
public:

	static ProvisionablePropertyIndex ppiFromRawPPI( RawPropertyIndex index);

	/*
	 * Called by:
	 *  - external app (work provisioning)
	 *  - internal (network topology provisioning)
	 */
	static void subscribe(ProvisionablePropertyIndex propertyIndex, ProvisionCallback aCallback);
	static void notify(
			ProvisionablePropertyIndex propertyIndex,
			ProvisionedValueType provisionedValue,
			int8_t rssi
			);
};
