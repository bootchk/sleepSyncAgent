#ifdef OBS
	// Scheduling slots tasks

	static void scheduleStartSyncSlotTask(void callback());
	static void scheduleEndSyncSlotTask(void callback());

	// Work slot follows sync without start callback
	static void scheduleEndWorkSlotTask(void callback());

	static void scheduleStartFishSlotTask(void callback());
	static void scheduleEndFishSlotTask(void callback());

	static void scheduleStartMergeSlotTask(void callback(), DeltaTime offset);
	// Merge slot ends w/o event, next event is startSyncSlot
#endif



	#ifdef OBS
This obsolete design did not advance startTimeOfPeriod when power is low
void Schedule::resumeAfterPowerRestored(){
	// Roll period forward to nearest period boundary after nowTime()

	// Integer division.
	int periodsMissed = (longClock.nowTime() - startTimeOfPeriod) / SlotDuration;
	int rollTime = (periodsMissed + 1 ) * SlotDuration;
	startTimeOfPeriod = startTimeOfPeriod + rollTime;
	assert(startTimeOfPeriod > longClock.nowTime() );
	// caller will call scheduleStartSyncSlot()
}
#endif


#ifdef OBS
This obsolete design used a scheduler.  Currently using sleepUntil...
// Sync is first slot of next period
void Schedule::scheduleStartSyncSlotTask(void callback()) {
	scheduleTask(callback, clampedTimeDifferenceFromNow(startTimeOfNextPeriod()));}
void Schedule::scheduleEndSyncSlotTask(void callback()) {
	scheduleTask(callback, clampedTimeDifference(timeOfThisSyncSlotEnd(), longClock.nowTime())); }


void Schedule::scheduleEndWorkSlotTask(void callback()) {
	scheduleTask(callback, clampedTimeDifferenceFromNow(timeOfThisWorkSlotEnd())); }


void Schedule::scheduleStartFishSlotTask(void callback()) {
	/*
	 * Chosen randomly from sleeping slots.
	 * Remember it, to schedule end.
	 */
	startTimeOfFishSlot = startTimeOfPeriod + randInt(FirstSleepingSlotOrdinal-1, CountSlots-1) * SlotDuration;
	assert(startTimeOfFishSlot >= longClock.nowTime());
	assert(startTimeOfFishSlot <= startTimeOfNextPeriod());
	scheduleTask(callback, clampedTimeDifferenceFromNow(startTimeOfFishSlot));
}
void Schedule::scheduleEndFishSlotTask(void callback()) {
	// Require called during fish slot
	// i.e. assert startTimeOfFishSlot > nowTime > startTimeOfFishSlot + SlotDuration
	// else fish slot end is in the past.
	LongTime time = startTimeOfFishSlot + SlotDuration;
	assert(time >= longClock.nowTime());
	assert(time <= startTimeOfNextPeriod());
	scheduleTask(callback, clampedTimeDifferenceFromNow(time));
}


void Schedule::scheduleStartMergeSlotTask(void callback(), DeltaTime offset) {
	/*
	 * !!!Not like others, is not aligned with slots.
	 * Is scheduled within some usual sleepSlot of this period, but need not at start of slot.
	 * offset is from CliqueMerger.
	 */
	LongTime time = startTimeOfPeriod + offset;
	assert(time >= longClock.nowTime());
	assert(time <= startTimeOfNextPeriod());
	scheduleTask(callback, clampedTimeDifferenceFromNow(time));
}
#endif
