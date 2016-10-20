
#pragma once

#include "../platform/platform.h"	// Radio, Sleeper, LEDLogger

Radio* radio;
Sleeper sleeper;

#include "modules/clique.h"

Clique clique;

#include "syncAgent.h"

SyncAgent syncAgent;

#include "modules/serializer.h"

Serializer serializer;
