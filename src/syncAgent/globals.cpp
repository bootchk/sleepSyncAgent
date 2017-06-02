#include "globals.h"

Radio* radio;
Mailbox* workOutMailbox;
SyncPowerManager* syncPowerManager;

SyncSleeper syncSleeper;

Clique clique;
SyncAgent syncAgent;
Serializer serializer;

//SimpleFishPolicy fishPolicy;
SyncRecoveryFishPolicy fishPolicy;

