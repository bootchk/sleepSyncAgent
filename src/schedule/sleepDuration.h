
#pragma once

#include "../types.h"



class SleepDuration {
public:

	static DeltaTime powerCheckInterval();

	// varies since now varies
	static DeltaTime nowTilPreludeWSync();
	static DeltaTime nowTilPreludeWFish();

	static DeltaTime nowTilSyncPoint();
	static DeltaTime nowTilFishStart();

	// Fixed
	static DeltaTime preludeTilNextTask();

};
