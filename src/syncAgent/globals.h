
#pragma once

/*
 * Singletons.
 * Global:
 * - some are built on concurrent, peripheral devices
 * - some are used by many slots.
 * Formerly owned by SyncAgent.
 */


#include "../platformHeaders/platform.h"	// Radio, Sleeper, LEDLogger
extern Radio* radio;
extern Mailbox* workOutMailbox;

#include "syncAgent.h"
extern SyncAgent syncAgent;

class Clique;
//#include "modules/clique.h"
extern Clique clique;

#include "modules/role.h"
extern MergerFisherRole role;

#include "modules/serializer.h"
extern Serializer serializer;

#include "modules/syncSleeper.h"
extern SyncSleeper syncSleeper;

#include "modules/syncSender.h"
extern SyncSender syncSender;

#include "modules/syncBehaviour.h"
extern SyncBehaviour syncBehaviour;

/*
 * fishPolicy used by clique and fishSchedule
 */
#include "policy/fishPolicy.h"
//extern SimpleFishPolicy fishPolicy;
extern SyncRecoveryFishPolicy fishPolicy;







