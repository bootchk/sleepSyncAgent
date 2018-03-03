
#pragma once

#include <radioSoC.h>	// TransmitPowerdBm



/*
 * Network cluster (clique) diameter.
 *
 * Used:
 * - transmitted in messages as the TSS
 * - transmitted in ControlSync granularity messages as a value to control other clique members
 * - transmitted during Provisioning as a value
 */
/*
 * The encoding should correspond to the one used by client provisioning app,
 * which currently is Small=0, ...
 */
enum class NetGranularity {
	Small = 0,
	Medium,
	Large,
	Invalid
};



/*
 * Knows granularity.
 * A property of a clique.
 *
 * AKA diameter of cliques or subnetworks in a hopping/mesh network.
 *
 * Implemented by:
 * - reducing xmit power
 * - filtering messages on virtual range
 *
 * Any change may result in:
 * - Decrease: cliques lose master and seek new master.
 * - Increase: cliques merge
 */
class Granularity {
private:
	static TransmitPowerdBm xmitPowerForGranularity(NetGranularity granularity);

public:
	/*
	 * Record granularity of self's clique.
	 *
	 * Effect granularity on my radio.
	 *
	 * Other member's of clique should be xmitting with same granularity.
	 *
	 * This happens:
	 * - after provisioning, on receiving SyncControl message
	 * - when I detect that my master is xmitting MasterSync with different granularity.
	 *
	 * Try: reject garbled values
	 */
	static void trySetGranularity(NetGranularity granularity);

	// Get clique's granularity (is a cache, may not correspond with radio's XmitPower.)
	static NetGranularity getCliqueGranularity();

	/*
	 * Is distance to sender within radius of my granularity.
	 */
#ifdef NOT_USED
	static bool isMsgInVirtualRange(
		unsigned int receivedSignalStrength,
		NetGranularity transmittedSignalStrength
		);
#endif

	/*
	 * Validate raw OTA value and convert to default if invalid.
	 */
	static NetGranularity getFromRaw(unsigned char rawTSS);
	/*
	 * Get OTA encoding of clique's current granularity
	 */
	static unsigned char getCliqueGranularityRaw();
	/*
	 * OTA encoding of given granularity.
	 */
	static unsigned char getRaw(NetGranularity aGranularity);
};
