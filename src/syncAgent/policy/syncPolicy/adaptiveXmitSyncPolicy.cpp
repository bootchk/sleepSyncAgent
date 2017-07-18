
#include "adaptiveXmitSyncPolicy.h"

#include "../../../augment/random.h"
#include "../../logging/logger.h"



namespace {

MasterTransmitSyncPolicy wrappedTransmitSyncPolicy;
static bool isAdvancedStage = false;

} // namespace


void AdaptiveTransmitSyncPolicy::reset() {
	wrappedTransmitSyncPolicy.reset();
	isAdvancedStage = false;
}

// Called every sync slot
bool AdaptiveTransmitSyncPolicy::shouldTransmitSync() {
	if (isAdvancedStage )
		// xmit sync according to wrapped policy (which is more random, and less frequently.)
		return wrappedTransmitSyncPolicy.shouldTransmitSync();
	else {
		/*
		 * !!!! Should not xmit sync on every call, since it would always contend with MergeSync intended for us.
		 */
		return randBool();
	}
}

// Advance to next stage (retard frequency of xmittals.)
void AdaptiveTransmitSyncPolicy::advanceStage() {
	isAdvancedStage = true;
}

void AdaptiveTransmitSyncPolicy::disarmForOneCycle() {
	Logger::log("disarm xmit policy\n");
	wrappedTransmitSyncPolicy.disarmForOneCycle();
}
