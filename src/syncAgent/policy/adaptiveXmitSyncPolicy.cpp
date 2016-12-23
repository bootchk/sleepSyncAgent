
#include <nRF5x.h>  // logger

#include "adaptiveXmitSyncPolicy.h"
#include "../../augment/random.h"



namespace {

MasterXmitSyncPolicy wrappedXmitSyncPolicy;
static bool isAdvancedStage = false;

} // namespace


void AdaptiveXmitSyncPolicy::reset() {
	wrappedXmitSyncPolicy.reset();
	isAdvancedStage = false;
}

// Called every sync slot
bool AdaptiveXmitSyncPolicy::shouldXmitSync() {
	if (isAdvancedStage )
		// xmit sync according to wrapped policy (which is more random, and less frequently.)
		return wrappedXmitSyncPolicy.shouldXmitSync();
	else {
		/*
		 * !!!! Should not xmit sync on every call, since it would always contend with MergeSync intended for us.
		 */
		return randBool();
	}
}

// Advance to next stage (retard frequency of xmittals.)
void AdaptiveXmitSyncPolicy::advanceStage() {
	isAdvancedStage = true;
}

void AdaptiveXmitSyncPolicy::disarmForOneCycle() {
	log("disarm xmit policy\n");
	wrappedXmitSyncPolicy.disarmForOneCycle();
}
