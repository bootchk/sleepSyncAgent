Work in progress

A sleep synchronization algorithm



Characteristics of the algorithm:

Ultra low power:  Unit is duty-cycled, sleeping mostly without the radio on.

Symmetric powered: no unit has more electrical power than other units.

Distributed, non-centralized: all units have same program.  All units may perform any role. No unit is responsible for gathering complete information from other units.

Leader election: cliques form, each clique elects a master transmitting sync.  Cliques merge.


References

Digi Corporation's Digimesh has sleep synchronization

"Reachback firefly algorithm".  But here, we elect a master.

"time triggered communication".  Units are synced and communicate on a schedule of slots.
