
#include <cassert>

#include "../slots/merging/mergePolicy.h"
#include "../syncAgentImp/state/role.h"
#include "../syncAgentImp/syncAgentImp.h"


/*
 * Actions for role transitions
 *
 * See state/role.h
 */


/*
 * Whenever transition to of from NoFishing, not reset FishPolicy.
 * I.E. resume fishing order.
 *
 * Fishing order is only reset when master drops out.
 */

void SyncAgentImp::toFisherFromNoFishing() {
	MergerFisherRole::toFisher();
}

void SyncAgentImp::ToNoFishingFromOther() {
	if (MergerFisherRole::isMerger()) {
		// TODO MergePolicy::restart();
		cliqueMerger.deactivate();
	}
	MergerFisherRole::toNoFishing();
}


/*
 * Was Fisher, caught another clique.
 * Begin role Merger.
 */
void SyncAgentImp::toMergerFromFisher(SyncMessage* msg){
	// assert slot is fishSlot
	assert (SyncMessage::doesCarrySync(msg->type));
	assert(MergerFisherRole::isFisher());

	MergerFisherRole::toMerger();
	cliqueMerger.initFromMsg(msg);

	// assert my schedule might have been adjusted
	// assert I might have relinquished mastership
	// assert I might have joined another clique
	assert(MergerFisherRole::isMerger());
}


void SyncAgentImp::stopMerger(){
	MergePolicy::restart();
	/*
	 * Go directly to Role::Fisher.
	 * Self as a Merger has already suffered a random delay.
	 */
	SyncAgentImp::toFisherFromMerger();	// deactivates CliqueMerger
	// assert next SyncPeriod will schedule FishSlot
	assert(MergerFisherRole::isFisher());
}


void SyncAgentImp::toFisherFromMerger(){
	MergerFisherRole::toFisher();
	// role does not know about cliqueMerger
	cliqueMerger.deactivate();
}
