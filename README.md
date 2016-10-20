A sleep synchronization algorithm for ultra low power radio networks


Work in progress. 

Status:

	- algorithm seems to work (for two units)
	- built as a static library
	- target platform Nordic nrf52, without OS, using raw protocol (see my other GitHub repository)


Todo:

    - testing with many units
    - fleshing out corners of algorithm: dropping out, adjusting mergers in progress, etc. 
    - other platforms
    - broadcast mesh:  relay sync to cliques that can't hear my master.  No addressing or routes, just hop count


Characteristics of the algorithm:
=

Ultra low power:  Unit is duty-cycled, sleeping mostly, with radio off.

Symmetric powered: no unit has more electrical power than other units.

Distributed, non-centralized: all units have same program.  All units may perform any role. No unit is responsible for gathering complete information from other units.

Leader election: cliques form, each clique elects a master transmitting sync.  Cliques merge.

Not a sensor network:  external events might not be reliably sensed since units sleep often, and in sync (at the same time.)

Contention with collision avoidance: the algorithm, in many places, probablistically xmits (after a random interval) to avoid sure collisions

Single-hop, broadcast: each unit hears all other units; no unit relays messages to others

Isolated: no unit is a gateway to a larger network, and the system does not have an external clock reference

Platform independent:  platform layer isolates the algorithm from the RTOS and wireless stack.



Discussion: it is not IoT, since there is no gateway and no Network time.  It is not a mesh network (keeping topology.)


Performance:
=

Keeps in sync to about 0.2 mSec.

Duty cycle (ratio of on to off) is about 1/100 or more, waking every few seconds or more.

At 0dBm, communicates at distance no more than ???
At -40dBm, communicates at distance no more than about one foot


Architecture:
=

Implements a SleepSyncAgent.  SleepSyncAgent handles radio for app.  The app manages power, sync, and work.  Most work is done on receiving a work message from the SyncAgent.

SleepSyncAgent is a wedge into the app. Built as a library cross-compiled to the target.  Linked into the app.  The app project provides platform libs that SleepSyncAgent requires.

    App                        SleepSyncAgent             ISRs 
                              (higher priority)           (highest priority)
    Startup
    main()                 --> loopOnEvents() 
                               never returns
    
                                          reasonForWake <- RTC
                                          reasonForWake <- Radio
                                                           Exceptions
    platform libs (or RTOS) <-- SyncAgent 
      radio
      clock
      timer
      readVcc
      queue      
      
    Work thread (lowest priority
    	--> workFromAppQue  --> isQueueEmpty()
    	<-- workToAppQue    <-- onWorkMsgReceive()
 

The system can be in these states (in order of electrical power available):

	- reset: not enough power for the mcu
	- low power: mcu duty-cycled, app running, but syncAgent stopped and radio off
	- enough power to sync: mcu and radio duty-cycled, app running, and syncAgent started
	- enough power to work: same as above, but enough power to do work when work messages received
	
If work takes little power, the last two states might be the same.  If work takes much power, then in the third state, work messages might be received but ignored (while sync is still maintained.)

Essence of algorithm
=

The goal is to be powered on at the same time.  A sub-goal is to spread the cost (electrical power) to achieve sync (to detect and merge other cliques.)

The algorithm is "reachback": each sync message payload carries offset: the time until next SyncPoint.  Sync message payload also carries ID of the master of the clique (but slaves also send syncs, so the sender ID i.e. MAC can differ from the master ID.  The master ID is NOT just the MAC of the sender.)

Two role pairs:

	master/slave
	fisher/merger
	
A unit can have any combination of roles from the role pairs e.g. slave AND fisher, or slave AND merger, etc.

A fisher periodically listens for syncs in normally sleeping slots of its schedule.  If a fisher hears a sync from another clique, it becomes a merger.

A merger decides which clique is better (the other, or its own, based on ordering of ID's)  A merger broadcasts syncs (with a large offset) to other cliques or its own clique, telling said clique to merge.


References
=

Digi Corporation's Digimesh has sleep synchronization

"Reachback firefly algorithm".  But in firefly algorithms, all units xmit sync. Here, we decide a master.  Here, more than one unit (even slaves) may xmit sync, but in the ideal state, only one master does.

"time triggered communication".  Units are synced and communicate on a schedule of slots.


Building
=

Different Eclipse build configurations:

    - Debug: builds and links for the host architecture with stubs for platform libs.
    - ArchiveArmM4: builds static lib for ARM M4 architecture leaving undefined references to platform libs
    - ArchiveArmM0: " for M0
    
CFLAGS are defined in the configurations, e.g. for M0  -mthumb -mcpu=cortex-m0 -mabi=aapcs -mfloat-abi=soft  These must match the app that you are linking to.

Platforms
-

Platforms could be:

	- stubs for platform (compile only)
	- wireless network simulator (FUTURE)
	- RTOS and wireless stack on target chip

		- a Bluetooth stack with Broadcaster/Observer roles to implement a UDP like protocol, without connections (TI CC2650, FUTURE)
		- a raw wireless protocol stack (Nordic nRF52)

Debug configuration
-

In config.h, comment out the define SYNC_AGENT_IS_LIBRARY.  Then stubs for the platform are compiled and linked in.  In this configuration, ProjectProperties>C/C++Builder>Settings>BuildArtifact>ArtifactType  is *Executable*.


Archive configuration
-

In config.h, define SYNC_AGENT_IS_LIBRARY   Then platform stubs are excluded from the build by #ifdefs, and the library depends on implementation in the app project of the platform API's.

ProjectProperties>C/C++Builder>Settings>Build Artifact>ArtifactType: Static library
ProjectProperties>C/C++Builder>Settings>Tool Chain>  compiler flags for ARM

Note that platform/platform.h currently has hard-coded paths to headers for the app's implementation of the platform.


Linking with app
-

    Copy the archive to the app project.
    Implement the API defined by platform/platform.h
    In the app's main, instantiate a SleepSyncAgent and call its loopOnEvents() method.  See main.cpp
    Implement a work thread reading to and writing to work queues (FUTURE)
    Build app project with same CFLAGS for ARM ISA.

