
#pragma once

#include "netTypes.h"

/*
 * Knows how to communicate to clique
 *
 * Primarily network control.
 */

class IntraCliqueManager {
public:
	static void doUpstreamCliqueSizeChange(NetGranularity aGranularity);
	static void doDownstreamCliqueSizeChange(NetGranularity aGranularity);

	static void doUpstreamScatter();
	static void doDownstreamScatter();
};
