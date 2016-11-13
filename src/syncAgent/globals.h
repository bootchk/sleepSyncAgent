
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
extern Mailbox* workOutMailbox;

#include "modules/clique.h"
extern Clique clique;

#include "syncAgent.h"
extern SyncAgent syncAgent;

#include "modules/serializer.h"
extern Serializer serializer;

#include "modules/syncSleeper.h"
extern SyncSleeper syncSleeper;

#include "modules/syncSender.h"
extern SyncSender syncSender;

#include "modules/syncBehaviour.h"
extern SyncBehaviour syncBehaviour;

#include "policy/fishPolicy.h"
extern FishPolicy fishPolicy;
