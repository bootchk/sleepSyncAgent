
#pragma once



/*
 * Singletons.
 * Global:
 * - some are built on concurrent, peripheral devices
 * - some are used by many slots.
 * Formerly owned by SyncAgent.
 */


#include "../platform/platform.h"	// Radio, Sleeper, LEDLogger
extern Radio* radio;
extern Sleeper sleeper;

#include "modules/clique.h"
extern Clique clique;

#include "syncAgent.h"
extern SyncAgent syncAgent;

#include "modules/serializer.h"
extern Serializer serializer;
