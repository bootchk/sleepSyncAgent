
#pragma once

#include "../types.h"



class SleepDuration {
public:

	// varies since now varies
	static DeltaTime nowTilPreludeWSync();
	static DeltaTime nowTilSyncPoint();

	// Fixed
	static DeltaTime preludeTilSync();

};
