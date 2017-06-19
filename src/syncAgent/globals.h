
#pragma once

/*
 * Singletons.
 * Global:
 * - some are built on concurrent, peripheral devices
 * - some are used by many slots.
 * Formerly owned by SyncAgent.
 */

#include <nRF5x.h>	// Radio, Sleeper

// Owned by calling app, implemented by lib nRF5x
extern Radio* radio;
extern Mailbox* workOutMailbox;

// Owned by calling app
#include "modules/syncPowerManager.h"
extern SyncPowerManager* syncPowerManager;

extern Sleeper sleeper;

#include "syncAgent.h"
extern SyncAgent syncAgent;

class Clique;
//#include "modules/clique.h"
extern Clique clique;

#include "state/role.h"
extern MergerFisherRole role;

#include "state/phase.h"
extern Phase phase;

#include "modules/serializer.h"
extern Serializer serializer;

#include "modules/syncSleeper.h"
extern SyncSleeper syncSleeper;

#include "modules/syncBehaviour.h"
extern SyncBehaviour syncBehaviour;


#include "policy/workManager.h"
extern WorkManager workManager;


/*
 * fishPolicy used by clique and fishSchedule
 */
#include "policy/fishPolicy.h"
//extern SimpleFishPolicy fishPolicy;
extern SyncRecoveryFishPolicy fishPolicy;







