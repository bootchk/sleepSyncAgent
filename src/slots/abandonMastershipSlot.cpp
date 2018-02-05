/*
 * Not enough power to use radio for full sync slot.
 * Ask an other unit in my clique to assume mastership.
 * Might not be heard, in which case other units should detect DropOut.
 *
 * Takes minimal power, for a short transmittal.
 */
#ifdef NOT_USED
void doDyingBreath() {
	// TODO this should be in the middle of the SyncSlot
	phase = Phase::AbandonMastership;
	syncSender.sendAbandonMastership();
}

/*
 * When I have paused normal SyncPeriod for many consecutive SyncPeriods.
 * If:
 * - I was master of a non-empty clique,
 * - and there is enough power
 * then: abandonMastership
 */
void tryAbandonMastership() {
	// TODO AbandonMastershipSlot and remainder of SyncPeriod
	if (clique.isSelfMaster()) {
		doDyingBreath();
		// TODO AbandonMastership, am I still master?
	}
}
#endif
