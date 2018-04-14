
#pragma once

#include "../types.h"  // DeltaTime


class ProvisionSchedule {
public:
	static DeltaTime deltaToProvisionStart();
	static DeltaTime deltaToProvisionEnd();
};
