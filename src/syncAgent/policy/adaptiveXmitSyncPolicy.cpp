
#include "adaptiveXmitSyncPolicy.h"


namespace {

MasterXmitSyncPolicy wrappedXmitSyncPolicy;
static bool isAdvancedStage;

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
	else
		// xmit sync on every call
		return true;
}

// Advance to next stage (retard frequency of xmittals.)
void AdaptiveXmitSyncPolicy::advanceStage() {
	isAdvancedStage = true;
}

void AdaptiveXmitSyncPolicy::disarmForOneCycle() {
	wrappedXmitSyncPolicy.disarmForOneCycle();
}
