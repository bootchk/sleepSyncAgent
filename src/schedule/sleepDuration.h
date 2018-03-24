
#pragma once

#include "../types.h"



class SleepDuration {
public:

	// varies since now varies
	static DeltaTime nowTilPreludeWSync();

	// Fixed
	static DeltaTime preludeTilSync();

};
