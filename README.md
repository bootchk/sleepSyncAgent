A sleep synchronization algorithm for ultra low power radio networks


Work in progress.  I am designing the algorithm.  I plan to test it either using simulation or directly in hardware such as TI's CC2650.


Characteristics of the algorithm:
-

Ultra low power:  Unit is duty-cycled, sleeping mostly, with radio off.

Symmetric powered: no unit has more electrical power than other units.

Distributed, non-centralized: all units have same program.  All units may perform any role. No unit is responsible for gathering complete information from other units.

Leader election: cliques form, each clique elects a master transmitting sync.  Cliques merge.

Not a sensor network:  external events might not be reliably sensed since units sleep often, and in sync (at the same time.)

Contention with collision avoidance: the algorithm, in many places, probablistically xmits (after a random interval) to avoid sure collisions

Single-hop, broadcast: each unit hears all other units; no unit relays messages to others

Isolated: no unit is a gateway to a larger network, and the system does not have an external clock reference


Architecture:
-

Implements a SyncAgent.  SyncAgent handles radio for app.  The app manages power, sync, and work.  Most work is done on receiving a work message from the SyncAgent.

The system can be in these states (in order of electrical power available):

	- reset: not enough power for the mcu
	- low power: mcu duty-cycled, app running, but syncAgent not started and radio off
	- enough power to sync: mcu and radio duty-cycled, app running, and syncAgent started
	- enough power to work: same as above, but enough power to do work when work messages received
	
If work takes little power, the last two states might be the same.  If work takes much power, then in the third state, work messages might be received but ignored (while sync is still maintained.)

Essence
-

The goal is to spread the cost (electrical power) of detecting and merging other cliques among units.

The algorithm is "reachback": each sync message has an offset indicating when the sync slot should start.  A sync message also identifies the master of the clique (but slaves also send syncs, so the sender ID i.e. MAC can differ from the master ID.  The master ID is NOT just the MAC of the sender.)

Two role pairs:
	master/slave
	fisher/merger
A unit can have any combination of roles from the role pairs e.g. slave AND fisher, or slave AND merger, etc.

A fisher periodically listens for syncs in normally sleeping slots of its schedule.  If a fisher hears a sync from another clique, it becomes a merger.

A merger decides which clique is better (the other, or its own.)  A merger broadcast syncs (with a large offset) to other cliques or its own clique, telling said clique to merge.



References
-

Digi Corporation's Digimesh has sleep synchronization

"Reachback firefly algorithm".  But in firefly algorithms, all units xmit sync. Here, we elect a master.

"time triggered communication".  Units are synced and communicate on a schedule of slots.
