
#pragma once

// libBLEProvisionee
#include <provisioner.h>	// ProvisionedValueType


class ProvisionerCallback {
public:
	static void succeed(ProvisionedValueType, int8_t);
	static void fail();
};
